package LamportTimeStamp;

import java.util.LinkedList;

public class main {

    private static void errorHandling(){
        System.err.println("errorHandling: we need  <number of Threads>  <number of message>  to serve you!");
    }


    public static void main(String args[]) throws InterruptedException {
        int numberOfMessages;
        int numberOfThreads;

        if(args.length != 2){
            errorHandling();
            System.exit(1);
        }

        System.out.println("The system started Successfully!");
        numberOfThreads = Integer.parseInt(args[0]);
        numberOfMessages = Integer.parseInt(args[1]);

        LinkedList<WorkingThread> createdThreads = new LinkedList<>();
        for (int i = 0; i < numberOfThreads; i++) {
            WorkingThread workingThread = new WorkingThread(Integer.toString(i), numberOfMessages);
            createdThreads.add(workingThread);
            workingThread.start();
        }

        for (WorkingThread w : createdThreads){
            w.setCreatedThreads(createdThreads);
        }

        Client client = new Client(numberOfMessages ,createdThreads);

        client.start();
        for(WorkingThread t : createdThreads){
            t.getThread().join();
        }

        System.out.println("The system is shutting down! bye!");
    }
}
