package de.cit;

import java.io.IOException;
import java.net.InetSocketAddress;
import java.nio.ByteBuffer;
import java.nio.channels.SelectionKey;
import java.nio.channels.Selector;
import java.nio.channels.ServerSocketChannel;
import java.nio.channels.SocketChannel;
import java.nio.charset.Charset;
import java.nio.charset.CharsetDecoder;
import java.nio.charset.CharsetEncoder;
import java.nio.charset.UnsupportedCharsetException;
import java.util.Iterator;
import java.util.concurrent.TimeUnit;

import static java.lang.System.exit;

public class SMTPServer {

    private static Charset messageCharset = null;
    private static CharsetDecoder decoder = null;
    private static CharsetEncoder encoder = null;

    /**
     * Explains how to use this program
     */
    private static void printUsage() {

        System.err.println("Usage: java SMTPClient <address> <port>");
    }

    public static void main(String args[]) throws IOException {

        ServerSocketChannel serverChannel = null;
        InetSocketAddress serverAddress = null;
        Selector selector = null;

        if(args.length != 2){
            printUsage();
            exit(1);
        }

        try {
            messageCharset = Charset.forName("US-ASCII");
        } catch(UnsupportedCharsetException uce) {
            System.err.println("Cannot create charset for this application. Exiting...");
            System.exit(1);
        }

        decoder = messageCharset.newDecoder();
        encoder = messageCharset.newEncoder();

        try {
            serverAddress = new InetSocketAddress(args[0], Integer.parseInt(args[1]));
        } catch (IllegalArgumentException e) {
            printUsage();
            exit(1);
        } catch (SecurityException e) {
            printUsage();
            exit(1);
        }

        try {
            selector = Selector.open();
        } catch (IOException e1) {
            e1.printStackTrace();
            System.exit(1);
        }

        try {
            serverChannel = ServerSocketChannel.open();
            serverChannel.configureBlocking(false);
            serverChannel.register(selector, SelectionKey.OP_ACCEPT);
            serverChannel.socket().bind(serverAddress);
        } catch (IOException e) {
            e.printStackTrace();
            exit(1);
        }

        ByteBuffer buffer = ByteBuffer.allocate(8192);

        System.out.println("Waiting for connection: ");
        while (true) {
            selector.select();

            Iterator selectedKeys = selector.selectedKeys().iterator();

            while (selectedKeys.hasNext()) {
                SelectionKey key = (SelectionKey) selectedKeys.next();

                try {
	                if (key.isAcceptable()) {
	
	                	ServerSocketChannel sChannel = (ServerSocketChannel) key.channel();
	                    SocketChannel channel = sChannel.accept();
	                    channel.configureBlocking(false);
                        SMTPClientState state = new SMTPClientState();
                        channel.register(selector, SelectionKey.OP_READ | SelectionKey.OP_WRITE,state);

	                    String message = "220 \r\n";
	                    buffer.put(message.getBytes(messageCharset));
	                    buffer.flip();
	                    channel.write(buffer);
	                    buffer.clear();
	                    
	                } else if (key.isReadable()) {
                        SMTPClientState clientState = (SMTPClientState) key.attachment();
	                    SocketChannel clientChannel = (SocketChannel) key.channel();

	                    if(clientState.getState() == 3)
                            TimeUnit.SECONDS.sleep(1);
                        try {
                            //Part02 and Part01
                            EnDecode enDecodeState = new EnDecode();
                            Integer ecDeState = enDecodeState.handleConnection(key, clientState, buffer);

                            // channel.close() because of QUIT.
                            if(ecDeState == 1) clientChannel.close();

                        } catch (IOException e) {
                            System.err.println("failed read...");
                        }
					}
	                selectedKeys.remove();
				} catch(IOException | InterruptedException ioe) {
					ioe.printStackTrace();
					System.exit(1);
				}
            }
        }
    }
}
