package LamportTimeStamp;

import java.util.LinkedList;
import java.util.Random;

public class Client implements Runnable{
    private Thread thread;
    final String threadName = "client";
    private int messagesNumber;
    private LinkedList<WorkingThread> createdThreads;
    private int threadsNumber;

    public Client(int number, LinkedList<WorkingThread> createdThreads){
        messagesNumber = number;
        threadsNumber = createdThreads.size();
        this.createdThreads = createdThreads;
    }

    public WorkingThread getRandomThread(){
        Random rand = new Random();
        int thread = rand.nextInt(threadsNumber);
        return createdThreads.get(thread);
    }

    private void sendThread(){
        LinkedList<Message> messages = new LinkedList<>();

        for(int i = 0; i < messagesNumber; i++){
            int rand = (int) (Math.random() * 10000 * messagesNumber);
            Message message = new Message(0);
            message.setMessage(rand);
            messages.add(message);
        }

        WorkingThread thread;
        for (Message message : messages) {
            thread = getRandomThread();
            //System.out.println("I sending the message: " + message.getMessage() + " to thread: " + thread.getThread().getName());
            thread.getInboxQueue().push(message);
        }
    }

    @Override
    public void run(){
        //System.out.println(threadName + " running");
        this.sendThread();
    }

    public void start(){
        //System.out.println(threadName + " starting");
        if (thread == null) {
            thread = new Thread(this, threadName);
        }
        thread.start();
    }
}

