package LamportTimeStamp;
public class Message {
    private Integer messageType;//0 for external message and 1 for internal message
    private Integer message;
    private String threadID;
    private Integer threadClock;

    public Message(int type){
        messageType = type;
    }

    ///////// messageType ////////
    public void setMessageType(int messageType) {
        this.messageType = messageType;
    }

    public int getMessageType() {
        return messageType;
    }
    ///////// messageType ////////


    ///////// Message ////////
    public void setMessage(int message) {
        this.message = message;
    }

    public int getMessage() {
        return message;
    }
    ///////// Message ////////


    ///////// ThreadID ////////
    public void setThreadID(String threadID) {
        this.threadID = threadID;
    }

    public String getThreadID() {
        return threadID;
    }
    ///////// ThreadTD ////////


    ///////// ThreadClock ////////
    public void setThreadClock(int threadClock) {
        this.threadClock = threadClock;
    }

    public int getThreadClock() {
        return threadClock;
    }
    ///////// ThreadClock ////////
}
