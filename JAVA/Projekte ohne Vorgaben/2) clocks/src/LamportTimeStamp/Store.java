package LamportTimeStamp;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.LinkedList;

public class Store {
    final String directorName = "LamportTimeStampLogs";
    private String fileName;

    public Store(String name){
        fileName = name;
    }

    public synchronized void storeMessages(LinkedList<Message> history) throws IOException {
        Files.createDirectories(Paths.get(directorName));

        FileOutputStream file = new FileOutputStream(directorName + File.separator + "thread_"+fileName + ".txt");

        ByteBuffer buf = ByteBuffer.allocate((Integer.BYTES + Integer.BYTES + 10 + 37) * history.size());

        String data = "";
        for(Message m : history){
            data = data + m.getMessage() + "\n";
        }

        buf.put(data.getBytes());
        buf.flip();
        file.getChannel().write(buf);
    }
}
