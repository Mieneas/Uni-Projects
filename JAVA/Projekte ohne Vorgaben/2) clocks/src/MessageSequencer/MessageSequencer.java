package MessageSequencer;

import java.util.LinkedList;
import java.util.Queue;
import java.util.concurrent.Semaphore;

public class MessageSequencer implements Runnable {
    private Queue<Message> inboxQueue;
	private boolean running = true;
	private WorkingThread queueList[];
	private Semaphore sem;
	private int numMsg;

	// terminiert den Message Sequencer
    public void terminate() {
        running = false;
    }
	
    public MessageSequencer(WorkingThread qlist[], int newnumMsg){
        queueList = qlist;
    	inboxQueue = new LinkedList<Message>();
    	sem = new Semaphore(1);
    	numMsg = newnumMsg;
    }
    
    @Override
    public void run() {
    	int count = 0;
    	while (running) {
            try {
        		// Sequencer wartet auf ein permit fr die inboxQueue
    			sem.acquire();
    			// Wenn die inboxQueue nicht leer ist, dann wird der erste Eintrag gepollt
    			// und an alle Working Threads weitergeleitet. Falls 'numMsg' Messages empfangen
    			// worden sind, dann wird der Thread terminiert
                if (!inboxQueue.isEmpty()) {
                    Message poll = inboxQueue.poll();
            		for (int i = 0; i < queueList.length; i++) {
                        queueList[i].acceptMessage(poll);
                    }
            		count++;
            		if (count >= numMsg) {
            			terminate();
            		}
                }
                // Sequencer released seinen permit fr die inboxQueue
                sem.release();
            } catch (InterruptedException e) {
            	e.printStackTrace();
            }
    	}
    }
    
    public void acceptMessage(Message msg) {
    	try {
    		// Sequencer wartet auf ein permit fr die inboxQueue
			sem.acquire();
			// neue message wird zur inboxQueue hinzugefgt
			inboxQueue.add(msg);
	        sem.release();
	        // Sequencer released seinen permit fr die inboxQueue
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
    }
    
}
