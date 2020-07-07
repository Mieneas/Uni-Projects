package de.cit;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.channels.FileChannel;
import java.nio.charset.Charset;
import java.nio.charset.UnsupportedCharsetException;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.text.SimpleDateFormat;
import java.util.Arrays;
import java.util.Date;
import java.util.LinkedList;
import java.util.List;

public class Mail {
    private String sender;
    private List<String> recipients;
    private String content;

    public Mail() {
        this.sender = "";
        this.recipients = new LinkedList<String>();
        this.content = "";
    }

    public Mail(String sender, List<String> recipients, String content) {
        this.sender = sender;
        this.recipients = recipients;
        this.content = content;
    }

    public String getSender() {
        return sender;
    }

    public void setSender(String sender) {
        this.sender = sender;
    }

    public List<String> getRecipients() {
        return recipients;
    }

    public void setRecipients(List<String> recipients) {
        this.recipients = recipients;
    }

    //Ahmad
    public void addRecipient(String recipient) {
        this.recipients.add(recipient);
    }

    public String getContent() {
        return content;
    }

    public void setContent(String content) {
        this.content = content;
    }

    //Ahmad
    public String recipientListToString() {
        return this.recipients.toString();
    }

    public int createFile() throws IOException {

        ByteBuffer mailBuffer = ByteBuffer.allocate(8192);

        for (String recipient : this.getRecipients()
        ) {
            // set name of the file
            int messageID = (int) (Math.random() * 10000);
            String directoryName = this.getSender() + "_" + messageID + ".txt";

            // create directory for sender
            Files.createDirectories(Paths.get("E-Mails" + File.separator + directoryName));


            // Get date and time
            SimpleDateFormat formatter = new SimpleDateFormat("dd/MM/yyyy HH:mm:ss");
            Date date = new Date();

            // content of the file
            String mailDate = "FROM: " + this.getSender() + "\nTO: " + recipient
                    + "\n\nDate: " + date + "\n\n\nMessage:\n" + this.getContent();

            // encoding of the email (switching from String to byte[])
            Charset messageCharset = null;
            try {
                messageCharset = Charset.forName("US-ASCII");
            } catch (UnsupportedCharsetException uce) {
                System.out.println("unsupported character");
                return 0;
            }

            byte[] b = mailDate.getBytes(messageCharset);
            mailBuffer.put(b);
            mailBuffer.flip();

            // create the file in the right directory
            FileOutputStream f = new FileOutputStream("E-Mails" + File.separator + directoryName + File.separator + recipient);

            // right the content of the file throw channel
            FileChannel ch = f.getChannel();
            ch.write(mailBuffer);

            // close channel and clear buffer
            ch.close();
            mailBuffer.clear();
        }
        return 1;
    }
}