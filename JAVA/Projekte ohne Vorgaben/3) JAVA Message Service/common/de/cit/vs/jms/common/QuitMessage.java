package de.tu_berlin.cit.vs.jms.common;

import de.tu_berlin.cit.vs.jms.common.BrokerMessage.Type;

public class QuitMessage extends BrokerMessage {
    public QuitMessage() {
        super(Type.SYSTEM_QUIT);
    }
}
