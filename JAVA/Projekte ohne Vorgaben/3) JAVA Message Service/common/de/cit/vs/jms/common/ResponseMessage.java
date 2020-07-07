package de.tu_berlin.cit.vs.jms.common;

public class ResponseMessage extends BrokerMessage {
    private String responseText;
    public ResponseMessage(String responseText) {
        super(Type.SYSTEM_RESPONSE);
        this.responseText = responseText;
    }

    public String getResponseText() {
        return responseText;
    }

    public void setResponseText(String responseText) {
        this.responseText = responseText;
    }
}
