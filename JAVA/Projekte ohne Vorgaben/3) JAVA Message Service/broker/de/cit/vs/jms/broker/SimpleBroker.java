package de.cit.vs.jms.broker;

import java.io.Serializable;
import java.util.*;
import java.util.concurrent.TimeUnit;
import javax.jms.*;
import javax.jms.Queue;

import de.tu_berlin.cit.vs.jms.common.*;
import org.apache.activemq.ActiveMQConnection;
import org.apache.activemq.ActiveMQConnectionFactory;
import org.apache.activemq.command.ActiveMQDestination;


public class SimpleBroker {
    /* TODO: variables as needed */

    private LinkedList<String> clientsNameListIn;
    private LinkedList<String> clientsNameListOut;
    private LinkedList<Session> clientsSessions;

    private LinkedList<Queue> inputQueues;
    private LinkedList<Queue> outputQueues;

    private List<Stock> stockList;
    private PriorityQueue<String> registrationQueue;

    private Map<String, MessageProducer> messageProducerMap;
    private List<Topic> topicList;

    Connection connection;
    Session session;

    private Queue inputQ;
    private String clientName = "";
    private ActiveMQConnection activeMQConnection;
    private LinkedList<MessageConsumer> consumers;
    Queue mainQueue;
    MessageConsumer mainConsumer;
    Session mainSession;

    RegisterMessage registerMessage;
    UnregisterMessage unregisterMessage;
    BuyMessage buyMessage;
    SellMessage sellMessage;
    RequestListMessage requestListMessage;
    ListMessage listMessage;

    private int getIndexOfStock(String name){
        int i = 0;
        for(Stock s : stockList){
            if(s.getName().equals(name))
                return i;
            i++;
        }
        return -1;
    }

    private int getRightIndex(String inputQ1_Name) throws JMSException {
        for(Queue q : inputQueues){
            if(q.getQueueName().equals(inputQ1_Name))
                return inputQueues.indexOf(q);
        }
        return -1;
    }

    private void sendMessage(Queue outputQ, BrokerMessage.Type message) throws JMSException {
        MessageProducer messageProducer = session.createProducer(outputQ);
        String content = message.toString();
        ResponseMessage responseMessage = new ResponseMessage(content);
        ObjectMessage objectMessage = session.createObjectMessage();
        objectMessage.setObject(responseMessage);
        messageProducer.send(objectMessage);
    }
    
    private final MessageListener listener = new MessageListener() {
        @Override
        public void onMessage(Message msg) {
            if(msg instanceof ObjectMessage) {
                //TODO
                MessageConsumer clientConsumer;
                try {
                    Destination destination = msg.getJMSDestination();
                    String inputQ_Name = destination.toString();
                    inputQ_Name = inputQ_Name.substring(inputQ_Name.indexOf("//")+2);

                    Serializable serializableMessage = ((ObjectMessage) msg).getObject();
                    String messageContent = serializableMessage.toString();
                    if (messageContent.contains("RegisterMessage")) {
                        registerMessage = ((RegisterMessage) ((ObjectMessage) msg).getObject());


                        registrationQueue.add(registerMessage.getClientName());

                        session = connection.createSession(false, Session.AUTO_ACKNOWLEDGE);
                        clientsSessions.add(session);
                        Queue outputQ = session.createQueue(registerMessage.getClientName() + "_inQ");
                        if(!clientsNameListOut.contains(registerMessage.getClientName())){
                            clientsNameListOut.add(registerMessage.getClientName());
                            outputQueues.add(outputQ);
                        }
                        sendMessage(outputQ, BrokerMessage.Type.SYSTEM_REGISTER);

                        inputQ = session.createQueue(registerMessage.getClientName() + "_outQ");
                        if(!clientsNameListIn.contains(registerMessage.getClientName())){
                            clientConsumer = session.createConsumer(inputQ);
                            consumers.add(clientConsumer);
                        }
                        else{
                            clientConsumer = consumers.get(getRightIndex(inputQ_Name));
                        }

                        if(!clientsNameListIn.contains(registerMessage.getClientName())){
                            clientsNameListIn.add(registerMessage.getClientName());
                            inputQueues.add(inputQ);
                        }

                        clientConsumer.setMessageListener(listener);
                    } else if (messageContent.contains("UnregisterMessage")) {
                        unregisterMessage = ((UnregisterMessage) ((ObjectMessage) msg).getObject());
                        registrationQueue.remove(unregisterMessage.getClientName());

                        session = clientsSessions.get(clientsNameListOut.indexOf(unregisterMessage.getClientName()));
                        sendMessage(outputQueues.get(clientsNameListOut.indexOf(unregisterMessage.getClientName())), BrokerMessage.Type.SYSTEM_UNREGISTER);

                        clientConsumer = consumers.get(getRightIndex(inputQ_Name));
                        clientConsumer.setMessageListener(listener);
                    } else if (messageContent.contains("BuyMessage")) {
                        buyMessage = ((BuyMessage) ((ObjectMessage) msg).getObject());
                        int signal = buy(buyMessage.getStockName(), buyMessage.getAmount());

                        int rightIndex = getRightIndex(inputQ_Name);
                        Queue outputQ = outputQueues.get(rightIndex);
                        session = clientsSessions.get(rightIndex);
                        inputQ = inputQueues.get(rightIndex);
                        clientName = clientsNameListIn.get(rightIndex);
                        if (signal == 0) {
                            sendMessage(outputQ, BrokerMessage.Type.STOCK_BUY);
                        } else if (signal == -1){
                            sendMessage(outputQ, BrokerMessage.Type.SYSTEM_ERROR);
                        }
                        clientConsumer = consumers.get(getRightIndex(inputQ_Name));
                        clientConsumer.setMessageListener(listener);
                    } else if (messageContent.contains("SellMessage")) {
                        sellMessage = ((SellMessage) ((ObjectMessage) msg).getObject());
                        int signal = sell(sellMessage.getStockName(), sellMessage.getAmount());

                        int rightIndex = getRightIndex(inputQ_Name);
                        Queue outputQ = outputQueues.get(rightIndex);
                        session = clientsSessions.get(rightIndex);
                        inputQ = inputQueues.get(rightIndex);
                        clientName = clientsNameListIn.get(rightIndex);
                        if (signal == 0) {
                            sendMessage(outputQ, BrokerMessage.Type.STOCK_SELL);
                        }
                        clientConsumer = consumers.get(getRightIndex(inputQ_Name));
                        clientConsumer.setMessageListener(listener);
                    } else if (messageContent.contains("RequestListMessage")) {
                        requestListMessage = ((RequestListMessage) ((ObjectMessage) msg).getObject());
                        List<Stock> stockList = getStockList();

                        int rightIndex = getRightIndex(inputQ_Name);
                        Queue outputQ = outputQueues.get(rightIndex);
                        session = clientsSessions.get(rightIndex);
                        inputQ = inputQueues.get(rightIndex);
                        clientName = clientsNameListIn.get(rightIndex);
                        if (stockList != null) {
                            listMessage = new ListMessage(stockList);
                            MessageProducer messageProducer = session.createProducer(outputQ);
                            ObjectMessage objectMessage = session.createObjectMessage();
                            objectMessage.setObject(listMessage);
                            messageProducer.send(objectMessage);
                        } else {
                            sendMessage(outputQ, BrokerMessage.Type.SYSTEM_ERROR);
                        }
                        clientConsumer = consumers.get(getRightIndex(inputQ_Name));
                        clientConsumer.setMessageListener(listener);
                    } else if(messageContent.contains("QuitMessage")){
                        int rightIndex = getRightIndex(inputQ_Name);

                        synchronized (clientsSessions) {
                            session = clientsSessions.get(rightIndex);
                        }
                        clientsSessions.remove(rightIndex);

                        clientsNameListIn.remove(rightIndex);
                        clientsNameListOut.remove(rightIndex);

                        synchronized (consumers) {
                            clientConsumer = consumers.remove(getRightIndex(inputQ_Name));
                        }

                        TimeUnit.SECONDS.sleep(1);
                        clientConsumer.close();
                        session.close();
                        activeMQConnection.destroyDestination((ActiveMQDestination) outputQueues.remove(rightIndex));
                        activeMQConnection.destroyDestination((ActiveMQDestination) inputQueues.remove(rightIndex));
                    }
                    else {
                        int rightIndex = getRightIndex(inputQ_Name);
                        Queue outputQ = outputQueues.get(rightIndex);
                        session = clientsSessions.get(rightIndex);
                        inputQ = inputQueues.get(rightIndex);
                        clientName = clientsNameListIn.get(rightIndex);
                        sendMessage(outputQ, BrokerMessage.Type.SYSTEM_ERROR);

                        clientConsumer = consumers.get(getRightIndex(inputQ_Name));
                        clientConsumer.setMessageListener(listener);
                    }
                } catch (JMSException  | InterruptedException e) {
                    e.printStackTrace();
                }
            }
        }
    };
    
    public SimpleBroker(List<Stock> stockList) throws JMSException {
        /* TODO: initialize connection, sessions, etc. */
        System.setProperty("org.apache.activemq.SERIALIZABLE_PACKAGES", "*");

        this.stockList = stockList;
        registrationQueue = new PriorityQueue<>();
        clientsNameListIn = new LinkedList<>();
        clientsNameListOut = new LinkedList<>();
        clientsSessions = new LinkedList<>();
        inputQueues = new LinkedList<>();
        outputQueues = new LinkedList<>();
        consumers = new LinkedList<>();

        ActiveMQConnectionFactory cFac = new ActiveMQConnectionFactory("tcp://localhost:61616");

        activeMQConnection = (ActiveMQConnection) cFac.createConnection();

        connection = cFac.createConnection();
        connection.start();
        mainSession = connection.createSession(false, Session.AUTO_ACKNOWLEDGE);
        mainQueue = mainSession.createQueue("registerQ");
        mainConsumer = mainSession.createConsumer(mainQueue);
        mainConsumer.setMessageListener(listener);

        messageProducerMap = new HashMap();
        topicList = new ArrayList<>();
        for(Stock stock : stockList) {
            /* TODO: prepare stocks as topics */
            Topic topic = mainSession.createTopic(stock.getName());
            MessageProducer topicProducer = mainSession.createProducer(topic);
            topicList.add(topic);
            messageProducerMap.put(stock.getName(), topicProducer);
        }
    }

    public void stop() throws JMSException, InterruptedException{
        //TODO
        for(int i = 0; i < inputQueues.size(); i++) {
            consumers.get(i).close();
            clientsSessions.get(i).close();
            activeMQConnection.destroyDestination((ActiveMQDestination) inputQueues.get(i));
            activeMQConnection.destroyDestination((ActiveMQDestination) outputQueues.get(i));
        }
        TimeUnit.SECONDS.sleep(1);
        mainConsumer.close();
        mainSession.close();
        activeMQConnection.destroyDestination((ActiveMQDestination) mainQueue);
        activeMQConnection.close();
        connection.close();
    }

    public synchronized int buy(String stockName, int amount) throws JMSException {
        //TODO
        int stockIndex = getIndexOfStock(stockName);
        if (stockIndex == -1) {return -1;}
        Stock currentStock = stockList.get(stockIndex);
        int availableStocks = currentStock.getAvailableCount();
        currentStock.setAvailableCount(availableStocks - amount);

        //send notification to the clients, who are following this stock.
        String notification = "+++++++++++++++++++++++++++++++++++++\n"
                +"current state of "+ currentStock.getName() + " \n"
                +"available amount = " + currentStock.getAvailableCount() + " \n"
                +"total amount = "+ currentStock.getStockCount() + " \n"
                +"stock price = "+ currentStock.getPrice() + "\n"+
                "+++++++++++++++++++++++++++++++++++++\n";

        Session newSession = connection.createSession(false, Session.AUTO_ACKNOWLEDGE);
        TextMessage textMessage =newSession.createTextMessage(notification);
        MessageProducer currentMessageProducer = messageProducerMap.get(currentStock.getName());
        currentMessageProducer.send(textMessage);


            return 0;
    }
    
    public synchronized int sell(String stockName, int amount) throws JMSException {
        //TODO
        int stockIndex = getIndexOfStock(stockName);
        Stock currentStock = stockList.get(stockIndex);
        int availableStocks = currentStock.getAvailableCount();
        currentStock.setAvailableCount(availableStocks + amount);

        //send notification to the clients, who are following this stock.
        String notification = "+++++++++++++++++++++++++++++++++++++\n"
                +"current state of "+ currentStock.getName() + " \n"
                +"available amount = " + currentStock.getAvailableCount() + " \n"
                +"total amount = "+ currentStock.getStockCount() + " \n"
                +"stock price = "+ currentStock.getPrice() + "\n" +
                "+++++++++++++++++++++++++++++++++++++\n";
        Session newSession = connection.createSession(false, Session.AUTO_ACKNOWLEDGE);
        TextMessage textMessage =newSession.createTextMessage(notification);
        MessageProducer currentMessageProducer = messageProducerMap.get(currentStock.getName());
        currentMessageProducer.send(textMessage);

        return 0;
    }
    
    public synchronized List<Stock> getStockList() {
        List<Stock> stockList = new ArrayList<>();

        /* TODO: populate stockList */
        stockList.addAll(0, this.stockList);
        return stockList;
    }
}
