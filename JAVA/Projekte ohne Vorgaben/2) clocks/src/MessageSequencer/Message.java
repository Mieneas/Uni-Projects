package MessageSequencer;

public class Message {
    public final static int EXTERNAL = 0;
    public final static int INTERNAL = 1;

    private int messageType;
    private int payload;
    private int threadId;

    public Message(int payload, int threadId) {
        this.messageType = INTERNAL;
        this.setPayload(payload);
        this.threadId = threadId;
    }

    public Message(int payload) {
        this.messageType = EXTERNAL;
        this.setPayload(payload);
    }

    public int getMessageType() {
        return messageType;
    }

	public int getPayload() {
		return payload;
	}
	
	public int getthreadId() {
		return threadId;
	}

	@Override
	public String toString() {
		return "" + payload;
	}

	public void setPayload(int newpayload) {
		payload = newpayload;
	}
}
