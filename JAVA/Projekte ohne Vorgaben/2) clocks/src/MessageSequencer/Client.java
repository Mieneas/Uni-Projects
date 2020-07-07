package MessageSequencer;

import java.util.ArrayList;
import java.util.Collections;
import java.util.Random;

public class Client implements Runnable {
	private int numThreads;
	private int numMsg;
	private int recThreads;
	private WorkingThread queueList[];
    
    public Client(int newnumThreads, int newnumMsg, int newrecThreads, WorkingThread qlist[]){
        queueList = qlist;
        numThreads = newnumThreads;
    	numMsg = newnumMsg;
    	recThreads = newrecThreads;
    }
	
    @Override
    public void run() {
    	//erstelle Random number generator und nutze ihn um 'recThreads' Anzahl von
    	// einzigartigen Working Threads auszusuchen
        Random gen = new Random();
        ArrayList<Integer> threadList = new ArrayList<Integer>();
        while (threadList.size() < recThreads) {
        	int temp = gen.nextInt(numThreads);
        	if (!threadList.contains(temp)) {
            	threadList.add(temp);
        	}
        }
        //System.out.println("Threads " + threadList.toString() + " can receive");

        // Erstelle 'numMsg' Anzahl von Messages mit randomly generated aber einzigartige (unique) payloads
        // und pushe sie auf einer von den Working Threads die Externe Messages empfangen drfen
        ArrayList<Integer> alist = new ArrayList<Integer>();
        for (int i = 0; i < numMsg; i++) {
            alist.add(i);
        }
        Collections.shuffle(alist);
        for (int j = 0; j < numMsg; j++) {
        	Message msg = new Message(alist.get(j));
            queueList[threadList.get(gen.nextInt(recThreads))].acceptMessage(msg);
        }
    }

}
