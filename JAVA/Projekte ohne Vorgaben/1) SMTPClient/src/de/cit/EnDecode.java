package de.cit;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.CharBuffer;
import java.nio.channels.SelectionKey;
import java.nio.channels.SocketChannel;
import java.nio.charset.*;
import java.sql.SQLOutput;
import java.util.LinkedList;
import java.util.List;

public class EnDecode {

    private static Charset messageCharset = null;
    private static CharsetDecoder decoder = null;
    private static CharsetEncoder encoder = null;
    private static Mail email;


    /**
     * Explains how to use this program
     */

    public EnDecode(){

    }
    private static void printBuffer(String message) {

        System.out.println(message);
    }

    private static void debugAndExit(String msg, SocketChannel client, ByteBuffer buffer, SMTPClientState state) throws IOException {
        int command = state.getState();
        String commandName = "";
        if(command == 0) commandName = "HELO";
        if(command == 1) commandName = "MAIL FROM";
        if(command == 2) commandName = "RCPT TO";
        if(command == 3) commandName = "DATA";
        if(command == 4) commandName = "Message";
        if(command == 5) commandName = "QUIT";

        //printBuffer(msg);
        System.err.print("expected command " + commandName + " but the client has send another command, exiting...");
        prepareReplyInBUffer("503", buffer,client);
        System.err.println("\nBad sequence of commands");
        //client.close();
        //System.exit(1);
    }

    private static String getCommandHint(String commandName){
        String reply = "";
        switch (commandName){
            case "HELO":
                reply = " 214 This command is used to identify the sender-SMTP to the\n" +
                        "receiver-SMTP. The argument field contains the host name of\n" +
                        "the sender-SMTP.";
                break;

            case "MAIL":
                reply = "214 This command is used to initiate a mail transaction in which\n" +
                        "the mail data is delivered to one or more mailboxes. The\n" +
                        "argument field contains a reverse-path.";
                break;

            case "RCPT":
                reply = "214 This command is used to identify an individual recipient of\n" +
                        "the mail data; multiple recipients are specified by multiple\n" +
                        "use of this command.";
                break;

            case "DATA":
                reply = "214 The receiver treats the lines following the command as mail\n" +
                        "data from the sender. This command causes the mail data\n" +
                        "from this command to be appended to the mail data buffer.\n" +
                        "The mail data may contain any of the 128 ASCII character\n" +
                        "codes.";
                break;

            case "QUIT":
                reply = "214 This command specifies that the receiver must send an OK\n" +
                        "reply, and then close the transmission channel.\n"+
                        "The receiver should not close the transmission channel until\n" +
                        "it receives and replies to a QUIT command (even if there was\n" +
                        "an error). The sender should not close the transmission\n" +
                        "channel until it send a QUIT command and receives the reply\n" +
                        "(even if there was an error response to a previous command)";
                break;

            case "HELP":
                reply = "214 This command causes the receiver to send helpful information\n" +
                        "to the sender of certain command. The command may take an\n" +
                        "argument (e.g., any command name) and return more specific\n" +
                        "information as a response.";
                break;

            default:
                reply = "214 The follwing commands are supported by this Server \n"+
                        "HELO : This command is used to identify the sender-SMTP to the\n" +
                        "receiver-SMTP.\n"+

                        "MAIL : This command is used to initiate a mail transaction in which\n" +
                        "the mail data is delivered to one or more mailboxes \n"+

                        "RCPT : this command is used to inform the server about the recipients of the email \n"+

                        "DATA : this command used to inform the server about the content of the email\n"+

                        "HELP : This command causes the receiver to send helpful information\n" +
                        "to the sender of certain command\n"+

                        "QUIT : This command specifies that the receiver must send an OK\n" +
                        "reply, and then close the transmission channel.\r\n";


        }
        return reply;
    }

    private static void prepareReplyInBUffer(String responsecode, ByteBuffer buffer, SocketChannel client) throws IOException {
        String reply = responsecode + " " + "\r\n";
        buffer.clear();
        buffer.put(reply.getBytes(messageCharset));
        buffer.flip();
        client.write(buffer);
        buffer.clear();
    }

    public void get_content(String msg, Mail email, ByteBuffer buffer, SocketChannel client, SMTPClientState state)throws IOException{
        email.setContent(msg);
        state.setState(SMTPClientState.MESSAGESENT);
        //Part03 and Part02
        String storeState;
        if(email.createFile() == 1)
            storeState = "250";
        else {
            storeState = "451";
            System.err.println("Can't save the message");
        }
        prepareReplyInBUffer(storeState, buffer,client);
    }
    public Integer handleConnection(SelectionKey key, SMTPClientState state, ByteBuffer buffer)throws IOException{

        Integer returnValue = 0;

        SocketChannel client = (SocketChannel) key.channel();
        client.read(buffer);
        buffer.flip();

        try {
            messageCharset = Charset.forName("US-ASCII");
        } catch(UnsupportedCharsetException uce) {
            System.err.println("Cannot create charset for this application. Exiting...");
            return returnValue;
        }

        String msg = messageCharset.decode(buffer).toString();

        switch (msg.substring(0,4)){

            case"HELO":

                if(state.getState() != SMTPClientState.CONNECTED){debugAndExit(msg, client,buffer, state); return 0;}

                String clientName = msg.substring(msg.indexOf("O")+2);
                clientName = clientName.substring(0,clientName.indexOf("\r"));

                System.out.println("connected to "+ clientName);
                state.setState(SMTPClientState.RECEIVEDWELCOME);

                prepareReplyInBUffer("250", buffer,client);
                System.out.println("client send Hello");
                break;


            case "MAIL":
                if(state.getState() != SMTPClientState.RECEIVEDWELCOME){debugAndExit(msg, client,buffer, state); return 0;}

                String sender = msg.substring(msg.indexOf(":")+1);
                sender = sender.substring(0,sender.indexOf("\r"));

                email = new Mail();
                email.setSender(sender);

                System.out.println("email from "+ sender);
                state.setState(SMTPClientState.MAILFROMSENT);

                prepareReplyInBUffer("250", buffer,client);
                System.out.println("client send Mail");
                break;

            case "RCPT":
                if(state.getState() != SMTPClientState.MAILFROMSENT){debugAndExit(msg, client,buffer, state); return 0;}
                String tmp = msg;
                String recipient;

                recipient = tmp.substring(tmp.indexOf(":")+1);

                if (tmp.contains(",")){

                    while (tmp.contains(",") || tmp.contains("\n")) {

                        if (!tmp.contains(",")) {
                            recipient = recipient.substring(0,recipient.indexOf("\r"));
                        }else {
                            recipient = recipient.substring(0,recipient.indexOf(","));
                        }

                        email.addRecipient(recipient);

                        tmp = tmp.substring(tmp.indexOf(",")+1);
                        System.out.println("send email to "+ email.recipientListToString());
                    }

                }else{
                    recipient = recipient.substring(0,recipient.indexOf("\r"));

                    email.addRecipient(recipient);
                    System.out.println("send email to "+ email.recipientListToString());
                }

                state.setState(SMTPClientState.RCPTTOSENT);

                prepareReplyInBUffer("250", buffer,client);
                System.out.println("client send Recipient");
                break;

            case "DATA":
                if(state.getState() != SMTPClientState.RCPTTOSENT){debugAndExit(msg, client,buffer, state); return 0;}

                state.setState(SMTPClientState.DATASENT);

                prepareReplyInBUffer("354", buffer,client);
                System.out.println("client send DATA");
                break;

            case "QUIT":
                if(state.getState() != SMTPClientState.MESSAGESENT){debugAndExit(msg, client,buffer, state); return 0;}
                prepareReplyInBUffer("221",buffer,client);
                buffer.flip();
                client.write(buffer);
                buffer.clear();
                System.out.println("client send QUIT");
                System.out.println("__________________________");
                return 1;
            case "HELP":
                String reply = "";
                if(msg.contains(" ")){
                    String commandName = msg.substring(msg.indexOf("["));
                    commandName = commandName.substring(0,commandName.indexOf("]"));

                    reply = getCommandHint(commandName);
                    System.out.println("----------------");
                    System.out.println("client send HELP");
                    System.out.println("----------------");

                }else{
                    reply = getCommandHint("");
                }
                buffer.clear();
                buffer.put(reply.getBytes(messageCharset));
                System.out.println("----------------");
                System.out.println("client send HELP");
                System.out.println("----------------");
                break;
            default:
                if(state.getState() != SMTPClientState.DATASENT){debugAndExit(msg, client,buffer, state); return 0;}
                get_content(msg, email, buffer, client, state);
                System.out.println("client send Message");
        }
        buffer.flip();
        client.write(buffer);
        buffer.clear();
        return returnValue;
    }


}
