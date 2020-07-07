package MessageSequencer;

import static java.lang.System.exit;
import static java.lang.System.setOut;

public class main {

	private static void printUsage(){
		System.err.println("required program parameters: <number of Threads>  <number of message> <the number of the threads will receive Message from Client>");
		exit(1);
	}

    public static void main(String args[]) throws InterruptedException {

		if (args.length!=3){
			printUsage();
		}

		System.out.println("The system started Successfully!");

		// erstelle Liste von threads und die eigentlichen Threads, sowie starte alle Threads
    	// mit hilfe von parametern. 
    	// Parameter: args[0] = anzahl von threads, args[1] = anzahl von messages,
    	// args[2] = anzahl von threads die external messages entgegennehmen duerfen
		WorkingThread wThreads[] = new WorkingThread[Integer.parseInt(args[0])];
		Thread createdThreads[] = new Thread[Integer.parseInt(args[0])];
		MessageSequencer seqThread = new MessageSequencer(wThreads, Integer.parseInt(args[1]));
    	
		for (int i = 0; i < Integer.parseInt(args[0]); i++) {
			wThreads[i] = new WorkingThread(seqThread, i, Integer.parseInt(args[1]));
			createdThreads[i] = new Thread(wThreads[i]);
			createdThreads[i].start();

		}

		new Thread(seqThread).start();
		Client clientThread = new Client(Integer.parseInt(args[0]), Integer.parseInt(args[1]), Integer.parseInt(args[2]), wThreads); 
		new Thread(clientThread).start();

		for (Thread t : createdThreads) {
			t.join();
		}

		System.out.println("The system is shutting down! bye!");

    }
}