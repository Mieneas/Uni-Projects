package de.cit.vs.jms.client;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.Serializable;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.Hashtable;
import java.util.Map;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.jms.*;

import de.tu_berlin.cit.vs.jms.common.*;
import de.tu_berlin.cit.vs.jms.common.BrokerMessage.Type;
import org.apache.activemq.ActiveMQConnectionFactory;


public class JmsBrokerClient {
	private String clientName;
	private MessageProducer producer;
	private MessageConsumer consumer;
	private Session session;
	private Connection connection;
	private Queue registerQ;
	private Hashtable<String, MessageConsumer> registeredConsumers;
	private ArrayList<Stock> stocks;
	
    public JmsBrokerClient(String clientName) throws JMSException {
		this.clientName = clientName;
		
		System.setProperty("org.apache.activemq.SERIALIZABLE_PACKAGES", "*");
		
		ActiveMQConnectionFactory confactory = new ActiveMQConnectionFactory("tcp://localhost:61616");
		connection = confactory.createConnection();
		connection.start();
		confactory.setTrustAllPackages(true);
		confactory.setTrustedPackages(Arrays.asList("de.tu_berlin.cit.vs.jms.common"));

		session = connection.createSession(false, Session.AUTO_ACKNOWLEDGE);
		Queue inputQueue = session.createQueue(clientName + "_inQ");
		Queue outputQueue = session.createQueue(clientName + "_outQ");

		// registration now happens automatically when a client is initialized (with no specific stock)
        registerQ = session.createQueue("registerQ");
        RegisterMessage msg = new RegisterMessage(clientName);
        ObjectMessage objMsg = session.createObjectMessage();
        objMsg.setObject(msg);
        MessageProducer registerProducer = session.createProducer(registerQ);
        registerProducer.send(objMsg);

		stocks = new ArrayList<Stock>();
		registeredConsumers = new Hashtable<String, MessageConsumer>();
		
		producer = session.createProducer(outputQueue);
		consumer = session.createConsumer(inputQueue);
		MessageListener listener = new MessageListener() {
			public void onMessage(Message msg) {
				if(msg instanceof ObjectMessage) {
					try {
						Serializable serializableMessage = ((ObjectMessage) msg).getObject();
	                    String messageContent = serializableMessage.toString();
						if (messageContent.contains("ListMessage")) {
							ListMessage resmsg = (ListMessage) ((ObjectMessage) msg).getObject();
							System.out.println("Received message:	" + resmsg.getStocks());
						} else {
							ResponseMessage resmsg = (ResponseMessage) ((ObjectMessage) msg).getObject();
							System.out.println("Received message:	"+resmsg.getResponseText());
						}

					} catch (JMSException e) {
						System.err.println("failed to receive message");
						e.printStackTrace();
					}
				}
			}

		};
		consumer.setMessageListener(listener);
    }
    
    public void requestList() throws JMSException {
    	//System.out.println("'requestlist' command read");
    	try {
	    	RequestListMessage msg = new RequestListMessage();
	    	ObjectMessage objMsg = session.createObjectMessage();
	    	objMsg.setObject(msg);
	    	producer.send(objMsg);
    	} catch (JMSException e) {
    		System.err.println("'requestList' command failed");
    		e.printStackTrace();
    	}
    }
    
    public void buy(String stockName, int amount) throws JMSException {
    	//System.out.println("'buy " + stockName + " " + amount + "' command read");
    	try {
	    	BuyMessage msg = new BuyMessage(stockName, amount);
	    	ObjectMessage objMsg = session.createObjectMessage();
	    	objMsg.setObject(msg);
	    	producer.send(objMsg);
		} catch (JMSException e) {
			System.err.println("'buy' command failed");
			e.printStackTrace();
		}
    }
    
    public void sell(String stockName, int amount) throws JMSException {
    	//System.out.println("'sell " + stockName + " " + amount +"' command read");
    	try {
	    	SellMessage msg = new SellMessage(stockName, amount);
	    	ObjectMessage objMsg = session.createObjectMessage();
	    	objMsg.setObject(msg);
	    	producer.send(objMsg);
		} catch (JMSException e) {
			System.err.println("'sell' command failed");
			e.printStackTrace();
		}
    }
    
    public void watch(String stockName) throws JMSException {
    	//System.out.println("'register " + stockName + "' command read");
    	try {
			Topic newTopic= session.createTopic(stockName);
			MessageConsumer registerConsumer = session.createConsumer(newTopic);
			MessageListener listener = new MessageListener() {
				public void onMessage(Message msg) { 
					if(msg instanceof TextMessage) {
						try {
							String notification = ((TextMessage) msg).getText();
							System.out.println(notification);
	
						} catch (JMSException e) {
							System.err.println("failed to receive topic message");
							e.printStackTrace();
					}
				 }
			 }
			};
			registerConsumer.setMessageListener(listener);
			registeredConsumers.put(stockName, registerConsumer);
		} catch (JMSException e) {
			System.err.println("'register' command failed");
			e.printStackTrace();
		}
    }
    
    public void unwatch(String stockName) throws JMSException {
    	//System.out.println("'unregister " + stockName + "' command read");
    	try {
	    	UnregisterMessage msg = new UnregisterMessage(clientName);
	    	ObjectMessage objMsg = session.createObjectMessage();
	    	objMsg.setObject(msg);
	    	producer.send(objMsg);
	    	registeredConsumers.get(stockName).close();
		} catch (JMSException e) {
			System.err.println("'unregister' command failed");
			e.printStackTrace();
		}
    }
    
    public void quit() throws JMSException {
    	//System.out.println("'quit' command read");
    	try {
	    	QuitMessage msg = new QuitMessage();
	    	ObjectMessage objMsg = session.createObjectMessage();
	    	objMsg.setObject(msg);
	    	producer.send(objMsg);
	        session.close();
	        connection.close();
    	} catch (JMSException e) {
    		System.err.println("'quit' command failed");
    		e.printStackTrace();
    	}
    }
    
    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        try {
            BufferedReader reader = new BufferedReader(new InputStreamReader(System.in));
            System.out.println("Enter the client name:");
            String clientName = reader.readLine();
            
            JmsBrokerClient client = new JmsBrokerClient(clientName);
            
            boolean running = true;
            while(running) {
                System.out.println("Enter command:");
                String[] task = reader.readLine().split(" ");
                
                synchronized(client) {
                    switch(task[0].toLowerCase()) {
                        case "quit":
                            client.quit();
                            System.out.println("Bye bye");
                            running = false;
                            break;
                        case "list":
                            client.requestList();
                            break;
                        case "buy":
                            if(task.length == 3) {
                                client.buy(task[1], Integer.parseInt(task[2]));
                            } else {
                                System.out.println("Correct usage: buy [stock] [amount]");
                            }
                            break;
                        case "sell":
                            if(task.length == 3) {
                                client.sell(task[1], Integer.parseInt(task[2]));
                            } else {
                                System.out.println("Correct usage: sell [stock] [amount]");
                            }
                            break;
                        case "watch":
                            if(task.length == 2) {
                                client.watch(task[1]);
                            } else {
                                System.out.println("Correct usage: watch [stock]");
                            }
                            break;
                        case "unwatch":
                            if(task.length == 2) {
                                client.unwatch(task[1]);
                            } else {
                                System.out.println("Correct usage: watch [stock]");
                            }
                            break;
                        default:
                            System.out.println("Unknown command. Try one of:");
                            System.out.println("quit, list, buy, sell, watch, unwatch");
                    }
                }
            }
            
        } catch (JMSException | IOException ex) {
            Logger.getLogger(JmsBrokerClient.class.getName()).log(Level.SEVERE, null, ex);
        }
        
    }
    
}
