package LamportTimeStamp;

import java.io.IOException;
import java.util.LinkedList;
import java.util.concurrent.*;


public class WorkingThread implements Runnable{
    private Thread thread;
    private String threadName;
    private LinkedList<WorkingThread> createdThreads;

    private int clock = 0;
    private LinkedBlockingDeque<Message> inboxQueue;
    private LinkedList<Message> history;

    private int receivedMessageNumber = 0;
    private int messagesNumber;


    public WorkingThread(String name, int mNum){
        threadName = name;
        messagesNumber = mNum;
        history = new LinkedList<>();
        inboxQueue = new LinkedBlockingDeque<>();
    }

    private LinkedList<Message> getClockOrder(LinkedList<Message> list){
        LinkedList<Message> history = new LinkedList<>();
        int size = list.size();
        for(int i = 0; i < size; i++) {
            Message message = list.peek();
            for (Message m : list) {
                if (m.getThreadClock() < message.getThreadClock())
                    message = m;
            }
            history.add(message);
            list.remove(message);
        }
        return history;
    }

    private LinkedList<Message> getIdOrder(LinkedList<Message> list){
        LinkedList<Message> history = new LinkedList<>();
        int size = list.size();
        for(int i = 0; i < size; i++) {
            Message message = list.peek();
            for (Message m : list) {
                if (m.getThreadClock() == message.getThreadClock() && Integer.parseInt(m.getThreadID()) < Integer.parseInt(message.getThreadID()))
                    message = m;
            }
            history.add(message);
            list.remove(message);
        }
        return history;
    }

    public void processing() throws IOException {
        Message message = inboxQueue.poll();
        if(message.getMessageType() == 0){
            message.setThreadID(threadName);

            clock = clock + 1;
            message.setThreadClock(clock);

            if(!history.contains(message)) {
                history.add(message);
                receivedMessageNumber++;
            }

            message.setMessageType(1);
            for(WorkingThread wThread : createdThreads){
                if(!wThread.thread.getName().equals(threadName))
                    if (!wThread.history.contains(message) && !wThread.inboxQueue.contains(message)) {
                            wThread.inboxQueue.add(message);
                    }
            }
        }
        else {
            if(message.getThreadClock() > clock)
                clock = message.getThreadClock() + 1;
            else clock = clock + 1;

            history.add(message);
            receivedMessageNumber++;
        }

        if(receivedMessageNumber == messagesNumber){
            Store store = new Store(threadName);
            history = getClockOrder(history);
            history = getIdOrder(history);
            store.storeMessages(history);
            thread.stop();
        }
    }

    public void receiveThreads() throws Exception {
        while (true) {
            if (!inboxQueue.isEmpty()) {
                processing();
            }
        }
    }

    @Override
    public void run(){
        try {
            this.receiveThreads();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public void start(){
        if (thread == null) {
            thread = new Thread(this, "Thread " + threadName);
        }
        thread.start();
    }

    public Thread getThread(){ return thread; }

    public LinkedBlockingDeque<Message> getInboxQueue(){ return inboxQueue; }

    public void setCreatedThreads(LinkedList<WorkingThread> createdThreads) {
        this.createdThreads = createdThreads;
    }
}
