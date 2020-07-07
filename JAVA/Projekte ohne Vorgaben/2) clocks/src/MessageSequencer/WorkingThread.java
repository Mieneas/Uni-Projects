package MessageSequencer;

import java.util.ArrayList;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.Semaphore;

public class WorkingThread implements Runnable {
	private LinkedBlockingQueue<Message> inboxQueue;
    private ArrayList<Message> history;
    private int threadId;
    private boolean running = true;
    private MessageSequencer seq;
    private int numMsg;
    private Semaphore sem;

    public void terminate() {
		try{
			// versuche directory 'ThreadHistories' zu erstellen wenn sie noch nicht existiert.
		    File dir = new File("MessageSequencerLogs");
		    if (!dir.exists()){
		        dir.mkdir();
		    }

		    // Erstelle oder berschreibe den txt File fr die history von diesem Working Thread
            File file = new File("MessageSequencerLogs/thread_" + threadId + ".txt");
            if (!file.exists()) {
                file.createNewFile();
            }
            FileWriter fw = new FileWriter(file.getAbsoluteFile());
            BufferedWriter bw = new BufferedWriter(fw);

            // schreibe jede message von der history auf seine eigene Zeile in den txt File rein
            for (Message msg : history) {
				bw.write(msg.toString());
				bw.newLine();
		    }
            // flushe und close den Writer vom txt File
            bw.flush();
            bw.close();
        }
        catch(Exception e){
        	System.out.println("unable to create/write to file.");
		    e.printStackTrace();
        }
		running = false;
    }
    
    public WorkingThread(MessageSequencer msgseq, int id, int newnumMsg) {
    	seq = msgseq;
    	threadId = id;
    	inboxQueue = new LinkedBlockingQueue<Message>();
    	history = new ArrayList<Message>();
    	numMsg = newnumMsg;
    	sem = new Semaphore(1);
    }

    @Override
    public void run() {
    	while (running) {
            try {
            	// Wenn die inboxQueue nicht leer ist, polle den ersten Eintrag. Falls die Message
            	// External ist, erstelle eine Internal Message und schicke diese and den Message
            	// Sequencer. Falls die Message Internal ist, wird sie zu der history hinzugefgt.
            	// Falls die history 'numMsg' Anzahl von messages enthlt, terminiert der Thread
                if (!inboxQueue.isEmpty()) {
                    Message poll = inboxQueue.poll();
                	if (poll.getMessageType() == 0) {
                		Message internal = new Message(poll.getPayload(), -1);
                		seq.acceptMessage(internal);
                	} else {
            	        history.add(poll);
                		if (history.size() >= numMsg) {
                			terminate();
                		}
                	}
                }
                
            } catch (Exception e) {
            	e.printStackTrace();
                running = false;
            }
    	}
    }

    public void acceptMessage(Message msg) {
    	// neue message wird zur inboxQueue hinzugefgt
		inboxQueue.add(msg);
    }
}
