package com.stericsson.sdk.signing.mockup;

import java.io.InputStream;
import java.io.OutputStream;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.Arrays;
import java.util.List;

/**
 * Instance of this class simulate RTT server which can follow any protocol. To force following of some protocol protocol
 * messages has to be provided either in constructor or later using setter but always before starting server. Main purpose for
 * this class is to help with testing and provide functionality for simulating RTT protocol.
 * 
 * @author emicroh
 *
 */
public class FakeRTTServer {

    private ServerSocket serverSocket;

    private ServerSocketAcceptThread serverSocketThread;

    /**
     * Will create instance of RTT server which will follow protocol defined by given protocol messages.
     * 
     * @param protocolMessages Describes protocol which server should simulate.
     */
    public FakeRTTServer(MockProtocolMessage... protocolMessages) {
        try {
            serverSocket = new ServerSocket(0);
            serverSocketThread = new ServerSocketAcceptThread(serverSocket, protocolMessages);
        } catch (Throwable e) {
            throw new RuntimeException("Unable to create fake sign server!", e);
        }
    }

    /**
     * Will create instance of RTT server which will follow protocol defined by given protocol messages.
     * 
     * @param protocolMessages Describes protocol which server should simulate.
     */
    public FakeRTTServer(List<MockProtocolMessage> protocolMessages) {
        this(protocolMessages == null ? null : protocolMessages.toArray(new MockProtocolMessage[protocolMessages
            .size()]));
    }

    /**
     * Will start server and simulate protocol given by set messages.
     */
    public void startServer() {
        serverSocketThread.start();
    }

    /**
     * Force join this thread end of processing internal thread which handles socket operations and protocol. 
     * 
     * @throws InterruptedException @{@link Thread#join()}
     */
    public void joinServerSocketThread() throws InterruptedException {
        if (serverSocketThread.getState() != Thread.State.NEW) {
            serverSocketThread.join();
        }
    }

    /**
     * Use this method to dealocate resources like free sockets etc.
     */
    public void close() {
        try {
            if (serverSocketThread.acceptedClient != null) {
                serverSocketThread.close();
            }
            if (serverSocket != null) {
                serverSocket.close();
            }
        } catch (Exception e) {
            throw new RuntimeException("Closing sockets failed!");
        }
    }

    /**
     * Other possibility how to force following some protocol. This method should be called before server is started 
     * to take effect.
     * 
     * @param messages Messages defining protocol to follow by server.
     */
    public void setMockMessages(MockProtocolMessage[] messages) {
        this.serverSocketThread.messages = messages;
    }

    /**
     * Getter for server socket to provide useful information about it in tests. Intention of this getter is only
     * to provide additional information about server socket not allow to manipulate it. 
     * 
     * @return Server socket.
     */
    public ServerSocket getServerSocket() {
        return serverSocket;
    }

    /**
     * When server has been started once it waits for client connection. When some client was connected you can get
     * using this method associated socket.
     * 
     * @return Socket associated with connected client.
     */
    public Socket getAcceptedSocket() {
        if (serverSocketThread == null) {
            return null;
        } else {
            return serverSocketThread.acceptedClient;
        }
    }

    /**
     * @author emicroh
     */
    private static final class ServerSocketAcceptThread extends Thread {

        private static final int SOCKET_TIMEOUT = 20000;

        Socket acceptedClient;

        InputStream in;

        OutputStream out;

        ServerSocket serverSocket;

        MockProtocolMessage[] messages;

        private ServerSocketAcceptThread(ServerSocket ss, MockProtocolMessage[] msgs) {
            super("FakeRTTServer");
            serverSocket = ss;
            messages = msgs;
        }

        @Override
        public void run() {
            try {
                acceptedClient = serverSocket.accept();
                acceptedClient.setSoTimeout(SOCKET_TIMEOUT);

                if ((messages != null) && (messages.length > 0)) {
                    byte[] buf;

                    in = acceptedClient.getInputStream();
                    out = acceptedClient.getOutputStream();

                    for (MockProtocolMessage mock : messages) {
                        if (mock.isServerInitiated()) {
                            out.write(mock.getServerResponse().getMsg());

                            buf = new byte[mock.getClientMessage().getMsg().length];
                            int read = in.read(buf);
                            if (read < 0 || !Arrays.equals(buf, mock.getClientMessage().getMsg())) {
                                throw new RuntimeException("Expected message hasn't been recieved!");
                            }
                        } else {
                            buf = new byte[mock.getClientMessage().getMsg().length];
                            int read = in.read(buf);
                            if (read < 0 || !Arrays.equals(buf, mock.getClientMessage().getMsg())) {
                                throw new RuntimeException("Expected message hasn't been recieved!");
                            }

                            out.write(mock.getServerResponse().getMsg());
                        }

                        if (mock.getTimeout() > 0) {
                            Thread.sleep(mock.getTimeout());
                        }
                    }
                }
            } catch (Throwable t) {
                try {
                    close();
                } catch (Exception e1) {
                    e1.initCause(t);
                    throw new RuntimeException("IO error in server socket thread", e1);
                }
                throw new RuntimeException("IO error in server socket thread");
            }
        }

        public void close() throws Exception {
            if ((Thread.currentThread() != this) && (this.getState() != Thread.State.TERMINATED)) {
                this.join();
            }
            if (in != null) {
                in.close();
            }
            if (out != null) {
                out.close();
            }
            if (acceptedClient != null) {
                acceptedClient.close();
            }
        }
    }

}
