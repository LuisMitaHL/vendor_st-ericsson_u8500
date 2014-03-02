package com.stericsson.sdk.signing.mockup;

import java.io.ByteArrayInputStream;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.ServerSocket;
import java.net.Socket;
import java.security.KeyStore;
import java.security.SecureRandom;
import java.security.cert.CertificateException;
import java.security.cert.X509Certificate;
import java.util.List;

import javax.net.ssl.KeyManagerFactory;
import javax.net.ssl.SSLContext;
import javax.net.ssl.SSLServerSocket;
import javax.net.ssl.SSLServerSocketFactory;
import javax.net.ssl.SSLSocket;
import javax.net.ssl.X509TrustManager;

/**
 * Instance of this class simulate Sign server which can follow any protocol. To force following of
 * some protocol protocol messages has to be provided either in constructor or later using setter
 * but always before starting server. Main purpose for this class is to help with testing and
 * provide functionality for simulating Sign Server protocol.
 * 
 * @author emicroh
 * 
 */
public class FakeSignServer {

    private static final int SOCKET_TIMEOUT = 60000;

    private SSLServerSocket serverSocket;

    private ServerSocketAcceptThread serverSocketThread;

    /**
     * Will create instance of Sign server which will follow protocol defined by given protocol
     * messages.
     * 
     * @param protocolMessages
     *            Describes protocol which server should simulate.
     */
    public FakeSignServer(MockProtocolMessage... protocolMessages) {
        try {
            serverSocket = createFakeServerSocket();
            serverSocketThread = new ServerSocketAcceptThread(serverSocket, protocolMessages);
        } catch (Throwable e) {
            throw new RuntimeException("Unable to create fake sign server!", e);
        }
    }

    /**
     * Will create instance of Sign server which will follow protocol defined by given protocol
     * messages.
     * 
     * @param protocolMessages
     *            Describes protocol which server should simulate.
     */
    public FakeSignServer(List<MockProtocolMessage> protocolMessages) {
        this(protocolMessages == null ? null : protocolMessages
            .toArray(new MockProtocolMessage[protocolMessages.size()]));
    }

    /**
     * Will start server and simulate protocol given by set messages.
     */
    public void startServer() {
        serverSocketThread.start();
    }

    /**
     * Force join this thread end of processing internal thread which handles socket operations and
     * protocol.
     * 
     * @throws InterruptedException
     *             TBD
     * @{@link Thread#join()}
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
     * Other possibility how to force following some protocol. This method should be called before
     * server is started to take effect.
     * 
     * @param messages
     *            Messages defining protocol to follow by server.
     */
    public void setMockMessages(MockProtocolMessage[] messages) {
        this.serverSocketThread.messages = messages;
    }

    /**
     * Getter for server socket to provide useful information about it in tests. Intention of this
     * getter is only to provide additional information about server socket not allow to manipulate
     * it.
     * 
     * @return Server socket.
     */
    public SSLServerSocket getServerSocket() {
        return serverSocket;
    }

    /**
     * When server has been started once it waits for client connection. When some client was
     * connected you can get using this method associated socket.
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

    private SSLServerSocket createFakeServerSocket() throws Throwable {
        char[] passphrase = "password".toCharArray();
        SSLContext sc = SSLContext.getInstance("SSL");
        KeyManagerFactory kmf = KeyManagerFactory.getInstance("SunX509");
        KeyStore ks = KeyStore.getInstance("jks");

        ks.load(getKeyStoreInputStream(), passphrase);

        kmf.init(ks, passphrase);

        sc.init(kmf.getKeyManagers(), new X509TrustManager[] {
            new FakeTrustManager()}, new SecureRandom());

        SSLServerSocketFactory serverFac = sc.getServerSocketFactory();
        SSLServerSocket serverSock = (SSLServerSocket) serverFac.createServerSocket(0);
        return serverSock;
    }

    private InputStream getKeyStoreInputStream() {
        return new ByteArrayInputStream(KEYSTORE);
    }

    /**
     * @author emicroh
     */
    private final class ServerSocketAcceptThread extends Thread {

        Socket acceptedClient;

        InputStream in;

        OutputStream out;

        ServerSocket serverSocket;

        MockProtocolMessage[] messages;

        private ServerSocketAcceptThread(ServerSocket ss, MockProtocolMessage[] msgs) {
            super("FakeSignServer");
            serverSocket = ss;
            messages = msgs;
        }

        @Override
        public void run() {
            int read = 0;
            try {
                acceptedClient = serverSocket.accept();
                acceptedClient.setSoTimeout(SOCKET_TIMEOUT);

                ((SSLSocket) acceptedClient).startHandshake();

                if ((messages != null) && (messages.length > 0)) {
                    byte[] buf;

                    in = acceptedClient.getInputStream();
                    out = acceptedClient.getOutputStream();

                    for (MockProtocolMessage mock : messages) {
                        if (mock.isServerInitiated()) {
                            out.write(mock.getServerResponse().getMsg());

                            buf = new byte[mock.getClientMessage().getMsg().length];
                            read = in.read(buf);
                            if (read < 0) {
                                throw new Exception("Read failed");
                            }
                        } else {
                            buf = new byte[mock.getClientMessage().getMsg().length];
                            read = in.read(buf);
                            if (read < 0) {
                                throw new Exception("Read failed");
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
                throw new RuntimeException("IO error in server socket thread, read = " + read, t);
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

    private static class FakeTrustManager implements X509TrustManager {

        public X509Certificate[] getAcceptedIssuers() {
            return null;
        }

        public void checkClientTrusted(X509Certificate[] certificates, String arg1) throws CertificateException {
        }

        public void checkServerTrusted(X509Certificate[] certificates, String arg1) throws CertificateException {
        }

    }

    static final byte[] KEYSTORE =
        new byte[] {
            (byte) 0xFE, (byte) 0xED, (byte) 0xFE, (byte) 0xED, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x02,
            (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x01, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x01,
            (byte) 0x00, (byte) 0x10, (byte) 0x66, (byte) 0x61, (byte) 0x6B, (byte) 0x65, (byte) 0x5F, (byte) 0x73,
            (byte) 0x65, (byte) 0x72, (byte) 0x76, (byte) 0x65, (byte) 0x72, (byte) 0x5F, (byte) 0x63, (byte) 0x65,
            (byte) 0x72, (byte) 0x74, (byte) 0x00, (byte) 0x00, (byte) 0x01, (byte) 0x23, (byte) 0x60, (byte) 0x3F,
            (byte) 0xB3, (byte) 0x93, (byte) 0x00, (byte) 0x00, (byte) 0x02, (byte) 0xBB, (byte) 0x30, (byte) 0x82,
            (byte) 0x02, (byte) 0xB7, (byte) 0x30, (byte) 0x0E, (byte) 0x06, (byte) 0x0A, (byte) 0x2B, (byte) 0x06,
            (byte) 0x01, (byte) 0x04, (byte) 0x01, (byte) 0x2A, (byte) 0x02, (byte) 0x11, (byte) 0x01, (byte) 0x01,
            (byte) 0x05, (byte) 0x00, (byte) 0x04, (byte) 0x82, (byte) 0x02, (byte) 0xA3, (byte) 0x45, (byte) 0x32,
            (byte) 0xD3, (byte) 0xA6, (byte) 0x74, (byte) 0x56, (byte) 0x01, (byte) 0x89, (byte) 0x6D, (byte) 0x48,
            (byte) 0x11, (byte) 0xFB, (byte) 0x7D, (byte) 0xC1, (byte) 0xFC, (byte) 0x78, (byte) 0x9D, (byte) 0x87,
            (byte) 0x95, (byte) 0x92, (byte) 0xE0, (byte) 0x8B, (byte) 0xD3, (byte) 0xF5, (byte) 0x8D, (byte) 0x2D,
            (byte) 0xD4, (byte) 0xC0, (byte) 0x62, (byte) 0x9C, (byte) 0x3B, (byte) 0x91, (byte) 0xAB, (byte) 0x82,
            (byte) 0x39, (byte) 0x91, (byte) 0xAC, (byte) 0x6D, (byte) 0x1C, (byte) 0xE2, (byte) 0x9C, (byte) 0x62,
            (byte) 0x42, (byte) 0xEF, (byte) 0xD2, (byte) 0x70, (byte) 0x79, (byte) 0x4A, (byte) 0x89, (byte) 0x94,
            (byte) 0xF5, (byte) 0x85, (byte) 0x4F, (byte) 0xCB, (byte) 0x5C, (byte) 0xE6, (byte) 0x62, (byte) 0xC8,
            (byte) 0x82, (byte) 0x4C, (byte) 0x92, (byte) 0xF8, (byte) 0xC1, (byte) 0x2D, (byte) 0xBD, (byte) 0x9F,
            (byte) 0x2F, (byte) 0x16, (byte) 0xFA, (byte) 0x1C, (byte) 0x6C, (byte) 0x35, (byte) 0x49, (byte) 0x14,
            (byte) 0x90, (byte) 0xB0, (byte) 0x6B, (byte) 0x49, (byte) 0x4E, (byte) 0x41, (byte) 0x89, (byte) 0x93,
            (byte) 0x39, (byte) 0xA5, (byte) 0xEE, (byte) 0x08, (byte) 0x4E, (byte) 0xD0, (byte) 0x87, (byte) 0x7A,
            (byte) 0xE9, (byte) 0xBD, (byte) 0x76, (byte) 0x63, (byte) 0xDE, (byte) 0xF2, (byte) 0x87, (byte) 0x9E,
            (byte) 0x27, (byte) 0xB1, (byte) 0xBD, (byte) 0x9E, (byte) 0xDB, (byte) 0x32, (byte) 0xEA, (byte) 0xE0,
            (byte) 0xA7, (byte) 0xC4, (byte) 0xD5, (byte) 0x0F, (byte) 0x78, (byte) 0x34, (byte) 0x3B, (byte) 0x5F,
            (byte) 0xF2, (byte) 0x1B, (byte) 0xF2, (byte) 0x92, (byte) 0x6F, (byte) 0x80, (byte) 0xC1, (byte) 0x1B,
            (byte) 0x86, (byte) 0x29, (byte) 0x22, (byte) 0xBE, (byte) 0xCB, (byte) 0xF2, (byte) 0x63, (byte) 0x2E,
            (byte) 0x76, (byte) 0xB7, (byte) 0x06, (byte) 0x0C, (byte) 0xCF, (byte) 0x70, (byte) 0x28, (byte) 0x9B,
            (byte) 0x00, (byte) 0x82, (byte) 0xF9, (byte) 0x2C, (byte) 0x9C, (byte) 0x2B, (byte) 0x6A, (byte) 0x56,
            (byte) 0xEF, (byte) 0xB0, (byte) 0xAB, (byte) 0x4D, (byte) 0xE1, (byte) 0x0C, (byte) 0xDA, (byte) 0x0C,
            (byte) 0x08, (byte) 0x44, (byte) 0x43, (byte) 0xB7, (byte) 0xBD, (byte) 0x41, (byte) 0xC0, (byte) 0xE9,
            (byte) 0xFA, (byte) 0x5F, (byte) 0x96, (byte) 0x45, (byte) 0xF5, (byte) 0x0C, (byte) 0x52, (byte) 0x7D,
            (byte) 0x15, (byte) 0xAC, (byte) 0xC7, (byte) 0xA8, (byte) 0x41, (byte) 0xB7, (byte) 0xA5, (byte) 0xA3,
            (byte) 0xC5, (byte) 0x0A, (byte) 0x84, (byte) 0xEA, (byte) 0xD5, (byte) 0x98, (byte) 0xFF, (byte) 0x7E,
            (byte) 0xEA, (byte) 0xF8, (byte) 0x7D, (byte) 0x39, (byte) 0x04, (byte) 0x9C, (byte) 0x10, (byte) 0xCA,
            (byte) 0x86, (byte) 0x77, (byte) 0x26, (byte) 0x95, (byte) 0xC2, (byte) 0xD4, (byte) 0x2A, (byte) 0x45,
            (byte) 0x68, (byte) 0x5D, (byte) 0x3F, (byte) 0x7D, (byte) 0xFB, (byte) 0x88, (byte) 0x7C, (byte) 0x24,
            (byte) 0x84, (byte) 0xBD, (byte) 0x37, (byte) 0x7B, (byte) 0xA4, (byte) 0x2A, (byte) 0x60, (byte) 0xAD,
            (byte) 0x82, (byte) 0x01, (byte) 0x6E, (byte) 0x11, (byte) 0xCE, (byte) 0xE8, (byte) 0xBA, (byte) 0x48,
            (byte) 0xE6, (byte) 0x07, (byte) 0x0D, (byte) 0x8C, (byte) 0x85, (byte) 0xAE, (byte) 0x61, (byte) 0x80,
            (byte) 0x9F, (byte) 0x83, (byte) 0x31, (byte) 0x45, (byte) 0x3A, (byte) 0x18, (byte) 0xCB, (byte) 0x19,
            (byte) 0x2A, (byte) 0x9F, (byte) 0x50, (byte) 0x84, (byte) 0x20, (byte) 0x55, (byte) 0xF2, (byte) 0xFF,
            (byte) 0x68, (byte) 0xA1, (byte) 0x54, (byte) 0xFE, (byte) 0xB5, (byte) 0x46, (byte) 0x0D, (byte) 0x2B,
            (byte) 0xE4, (byte) 0x04, (byte) 0x15, (byte) 0x62, (byte) 0x86, (byte) 0x15, (byte) 0xA3, (byte) 0x52,
            (byte) 0x8F, (byte) 0xAF, (byte) 0x47, (byte) 0xC5, (byte) 0x11, (byte) 0x0C, (byte) 0xF1, (byte) 0x57,
            (byte) 0x2B, (byte) 0x24, (byte) 0x2B, (byte) 0x0B, (byte) 0x28, (byte) 0x3E, (byte) 0xE7, (byte) 0x1A,
            (byte) 0x98, (byte) 0xA6, (byte) 0xA7, (byte) 0xE9, (byte) 0xDF, (byte) 0x0E, (byte) 0x8D, (byte) 0xE5,
            (byte) 0xC5, (byte) 0xA8, (byte) 0xF0, (byte) 0x04, (byte) 0x8E, (byte) 0xD1, (byte) 0x90, (byte) 0x3B,
            (byte) 0x05, (byte) 0x60, (byte) 0xA1, (byte) 0xAB, (byte) 0x20, (byte) 0x94, (byte) 0xBC, (byte) 0xDE,
            (byte) 0xA5, (byte) 0x26, (byte) 0xD2, (byte) 0xC0, (byte) 0xD8, (byte) 0x42, (byte) 0x14, (byte) 0x67,
            (byte) 0x2D, (byte) 0xAE, (byte) 0x95, (byte) 0x38, (byte) 0xCF, (byte) 0x2E, (byte) 0x5B, (byte) 0x94,
            (byte) 0xC6, (byte) 0x65, (byte) 0x3C, (byte) 0xBE, (byte) 0x63, (byte) 0x0A, (byte) 0x13, (byte) 0x97,
            (byte) 0xAA, (byte) 0xBE, (byte) 0xB1, (byte) 0x98, (byte) 0x8A, (byte) 0x9C, (byte) 0x1D, (byte) 0x63,
            (byte) 0x31, (byte) 0xD6, (byte) 0xDE, (byte) 0x31, (byte) 0x7B, (byte) 0x3B, (byte) 0x6A, (byte) 0x83,
            (byte) 0x05, (byte) 0xC1, (byte) 0x58, (byte) 0xAD, (byte) 0x1F, (byte) 0x87, (byte) 0x10, (byte) 0x7B,
            (byte) 0xBE, (byte) 0x88, (byte) 0x50, (byte) 0x51, (byte) 0x39, (byte) 0x62, (byte) 0xC9, (byte) 0xB5,
            (byte) 0xF1, (byte) 0x9A, (byte) 0x20, (byte) 0x55, (byte) 0xFA, (byte) 0x6C, (byte) 0x02, (byte) 0xCA,
            (byte) 0xC7, (byte) 0x83, (byte) 0xFC, (byte) 0x55, (byte) 0xE8, (byte) 0x2B, (byte) 0xF8, (byte) 0xC1,
            (byte) 0x82, (byte) 0x4C, (byte) 0x88, (byte) 0xCC, (byte) 0xC7, (byte) 0x0B, (byte) 0x8C, (byte) 0x07,
            (byte) 0xFD, (byte) 0x01, (byte) 0xC3, (byte) 0x49, (byte) 0x85, (byte) 0x11, (byte) 0x6D, (byte) 0xC8,
            (byte) 0x8E, (byte) 0xE9, (byte) 0xF2, (byte) 0xE8, (byte) 0x87, (byte) 0x56, (byte) 0xC4, (byte) 0xE0,
            (byte) 0x7E, (byte) 0xAE, (byte) 0xCF, (byte) 0xDE, (byte) 0x09, (byte) 0x49, (byte) 0xD2, (byte) 0x0A,
            (byte) 0x87, (byte) 0x65, (byte) 0xFB, (byte) 0xEA, (byte) 0x55, (byte) 0xDC, (byte) 0x34, (byte) 0x79,
            (byte) 0xCD, (byte) 0x13, (byte) 0x88, (byte) 0xF8, (byte) 0xD7, (byte) 0xD3, (byte) 0x2A, (byte) 0x49,
            (byte) 0x83, (byte) 0x90, (byte) 0x7C, (byte) 0x51, (byte) 0xFB, (byte) 0xB9, (byte) 0x0E, (byte) 0x98,
            (byte) 0x99, (byte) 0x09, (byte) 0x2D, (byte) 0x4B, (byte) 0x9D, (byte) 0x22, (byte) 0x82, (byte) 0x81,
            (byte) 0x48, (byte) 0x65, (byte) 0xC6, (byte) 0x64, (byte) 0x0C, (byte) 0x5F, (byte) 0x89, (byte) 0xEC,
            (byte) 0x14, (byte) 0xBD, (byte) 0xB8, (byte) 0x2C, (byte) 0x08, (byte) 0xF1, (byte) 0xCF, (byte) 0xCA,
            (byte) 0x9F, (byte) 0x6C, (byte) 0x7D, (byte) 0xD7, (byte) 0xB5, (byte) 0xAD, (byte) 0x2B, (byte) 0x12,
            (byte) 0x67, (byte) 0x6B, (byte) 0xAF, (byte) 0x23, (byte) 0x7B, (byte) 0x3F, (byte) 0x62, (byte) 0xD2,
            (byte) 0x13, (byte) 0x60, (byte) 0x53, (byte) 0xA4, (byte) 0xD6, (byte) 0x17, (byte) 0x64, (byte) 0xE2,
            (byte) 0x71, (byte) 0x56, (byte) 0x29, (byte) 0xBD, (byte) 0x08, (byte) 0xF0, (byte) 0x2B, (byte) 0x51,
            (byte) 0x75, (byte) 0xFE, (byte) 0xCB, (byte) 0x1A, (byte) 0x41, (byte) 0x41, (byte) 0x03, (byte) 0x02,
            (byte) 0x07, (byte) 0x56, (byte) 0x88, (byte) 0x7F, (byte) 0x1D, (byte) 0x12, (byte) 0x55, (byte) 0x6C,
            (byte) 0xC8, (byte) 0x26, (byte) 0xA9, (byte) 0x7C, (byte) 0x49, (byte) 0x31, (byte) 0x79, (byte) 0x76,
            (byte) 0xE9, (byte) 0x9C, (byte) 0xEA, (byte) 0x68, (byte) 0x12, (byte) 0x8F, (byte) 0x07, (byte) 0x79,
            (byte) 0xC7, (byte) 0x7D, (byte) 0xBE, (byte) 0x6C, (byte) 0xA4, (byte) 0xAE, (byte) 0xA2, (byte) 0xBA,
            (byte) 0x7B, (byte) 0xE9, (byte) 0x13, (byte) 0xA1, (byte) 0xDA, (byte) 0x0C, (byte) 0x64, (byte) 0xAF,
            (byte) 0xF2, (byte) 0x9E, (byte) 0x46, (byte) 0xB8, (byte) 0x5F, (byte) 0x30, (byte) 0x5D, (byte) 0x68,
            (byte) 0x4C, (byte) 0xFE, (byte) 0x18, (byte) 0x4A, (byte) 0x57, (byte) 0xAC, (byte) 0x55, (byte) 0xFA,
            (byte) 0xE0, (byte) 0xED, (byte) 0x6F, (byte) 0x73, (byte) 0x90, (byte) 0xC4, (byte) 0x73, (byte) 0x32,
            (byte) 0xD9, (byte) 0x9A, (byte) 0x7D, (byte) 0x90, (byte) 0x00, (byte) 0x57, (byte) 0xE9, (byte) 0x2C,
            (byte) 0x51, (byte) 0x9F, (byte) 0xFE, (byte) 0x59, (byte) 0x40, (byte) 0xB9, (byte) 0x88, (byte) 0xD5,
            (byte) 0x36, (byte) 0x06, (byte) 0x3E, (byte) 0x03, (byte) 0x92, (byte) 0xBC, (byte) 0xE9, (byte) 0x45,
            (byte) 0x3C, (byte) 0xFA, (byte) 0x2B, (byte) 0x5A, (byte) 0x30, (byte) 0xE7, (byte) 0x5D, (byte) 0xAE,
            (byte) 0x58, (byte) 0x3A, (byte) 0xCA, (byte) 0xDF, (byte) 0xB0, (byte) 0x3B, (byte) 0x14, (byte) 0xA9,
            (byte) 0xB1, (byte) 0x09, (byte) 0x00, (byte) 0xAF, (byte) 0x1F, (byte) 0x0F, (byte) 0xBF, (byte) 0xEF,
            (byte) 0x43, (byte) 0x1F, (byte) 0x4F, (byte) 0xDE, (byte) 0x4A, (byte) 0xF2, (byte) 0x56, (byte) 0x6B,
            (byte) 0xBD, (byte) 0x10, (byte) 0x54, (byte) 0xF9, (byte) 0x26, (byte) 0x96, (byte) 0xD7, (byte) 0xBF,
            (byte) 0x21, (byte) 0x29, (byte) 0xB6, (byte) 0x33, (byte) 0x53, (byte) 0xA6, (byte) 0xFC, (byte) 0xAB,
            (byte) 0x19, (byte) 0xD8, (byte) 0xE7, (byte) 0x60, (byte) 0xBF, (byte) 0x45, (byte) 0xA0, (byte) 0x5C,
            (byte) 0x31, (byte) 0x46, (byte) 0x4B, (byte) 0xAB, (byte) 0xB1, (byte) 0xE3, (byte) 0x85, (byte) 0x04,
            (byte) 0x1D, (byte) 0x18, (byte) 0x30, (byte) 0x7B, (byte) 0xB0, (byte) 0xA0, (byte) 0x21, (byte) 0x46,
            (byte) 0x52, (byte) 0xBA, (byte) 0x66, (byte) 0xFB, (byte) 0x04, (byte) 0x35, (byte) 0xF2, (byte) 0x38,
            (byte) 0x78, (byte) 0x79, (byte) 0x86, (byte) 0x55, (byte) 0x65, (byte) 0x5B, (byte) 0xE8, (byte) 0x29,
            (byte) 0x1C, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x01, (byte) 0x00, (byte) 0x05, (byte) 0x58,
            (byte) 0x2E, (byte) 0x35, (byte) 0x30, (byte) 0x39, (byte) 0x00, (byte) 0x00, (byte) 0x02, (byte) 0x55,
            (byte) 0x30, (byte) 0x82, (byte) 0x02, (byte) 0x51, (byte) 0x30, (byte) 0x82, (byte) 0x01, (byte) 0xBA,
            (byte) 0xA0, (byte) 0x03, (byte) 0x02, (byte) 0x01, (byte) 0x02, (byte) 0x02, (byte) 0x04, (byte) 0x4A,
            (byte) 0x97, (byte) 0x9D, (byte) 0x98, (byte) 0x30, (byte) 0x0D, (byte) 0x06, (byte) 0x09, (byte) 0x2A,
            (byte) 0x86, (byte) 0x48, (byte) 0x86, (byte) 0xF7, (byte) 0x0D, (byte) 0x01, (byte) 0x01, (byte) 0x05,
            (byte) 0x05, (byte) 0x00, (byte) 0x30, (byte) 0x6D, (byte) 0x31, (byte) 0x0B, (byte) 0x30, (byte) 0x09,
            (byte) 0x06, (byte) 0x03, (byte) 0x55, (byte) 0x04, (byte) 0x06, (byte) 0x13, (byte) 0x02, (byte) 0x43,
            (byte) 0x5A, (byte) 0x31, (byte) 0x17, (byte) 0x30, (byte) 0x15, (byte) 0x06, (byte) 0x03, (byte) 0x55,
            (byte) 0x04, (byte) 0x08, (byte) 0x13, (byte) 0x0E, (byte) 0x43, (byte) 0x7A, (byte) 0x65, (byte) 0x63,
            (byte) 0x68, (byte) 0x20, (byte) 0x52, (byte) 0x65, (byte) 0x70, (byte) 0x75, (byte) 0x62, (byte) 0x6C,
            (byte) 0x69, (byte) 0x63, (byte) 0x31, (byte) 0x10, (byte) 0x30, (byte) 0x0E, (byte) 0x06, (byte) 0x03,
            (byte) 0x55, (byte) 0x04, (byte) 0x07, (byte) 0x13, (byte) 0x07, (byte) 0x4F, (byte) 0x73, (byte) 0x74,
            (byte) 0x72, (byte) 0x61, (byte) 0x76, (byte) 0x61, (byte) 0x31, (byte) 0x0E, (byte) 0x30, (byte) 0x0C,
            (byte) 0x06, (byte) 0x03, (byte) 0x55, (byte) 0x04, (byte) 0x0A, (byte) 0x13, (byte) 0x05, (byte) 0x54,
            (byte) 0x69, (byte) 0x65, (byte) 0x74, (byte) 0x6F, (byte) 0x31, (byte) 0x0C, (byte) 0x30, (byte) 0x0A,
            (byte) 0x06, (byte) 0x03, (byte) 0x55, (byte) 0x04, (byte) 0x0B, (byte) 0x13, (byte) 0x03, (byte) 0x54,
            (byte) 0x52, (byte) 0x44, (byte) 0x31, (byte) 0x15, (byte) 0x30, (byte) 0x13, (byte) 0x06, (byte) 0x03,
            (byte) 0x55, (byte) 0x04, (byte) 0x03, (byte) 0x13, (byte) 0x0C, (byte) 0x4D, (byte) 0x69, (byte) 0x63,
            (byte) 0x68, (byte) 0x61, (byte) 0x6C, (byte) 0x20, (byte) 0x52, (byte) 0x6F, (byte) 0x68, (byte) 0x61,
            (byte) 0x63, (byte) 0x30, (byte) 0x1E, (byte) 0x17, (byte) 0x0D, (byte) 0x30, (byte) 0x39, (byte) 0x30,
            (byte) 0x38, (byte) 0x32, (byte) 0x38, (byte) 0x30, (byte) 0x39, (byte) 0x30, (byte) 0x34, (byte) 0x32,
            (byte) 0x34, (byte) 0x5A, (byte) 0x17, (byte) 0x0D, (byte) 0x30, (byte) 0x39, (byte) 0x31, (byte) 0x31,
            (byte) 0x32, (byte) 0x36, (byte) 0x30, (byte) 0x39, (byte) 0x30, (byte) 0x34, (byte) 0x32, (byte) 0x34,
            (byte) 0x5A, (byte) 0x30, (byte) 0x6D, (byte) 0x31, (byte) 0x0B, (byte) 0x30, (byte) 0x09, (byte) 0x06,
            (byte) 0x03, (byte) 0x55, (byte) 0x04, (byte) 0x06, (byte) 0x13, (byte) 0x02, (byte) 0x43, (byte) 0x5A,
            (byte) 0x31, (byte) 0x17, (byte) 0x30, (byte) 0x15, (byte) 0x06, (byte) 0x03, (byte) 0x55, (byte) 0x04,
            (byte) 0x08, (byte) 0x13, (byte) 0x0E, (byte) 0x43, (byte) 0x7A, (byte) 0x65, (byte) 0x63, (byte) 0x68,
            (byte) 0x20, (byte) 0x52, (byte) 0x65, (byte) 0x70, (byte) 0x75, (byte) 0x62, (byte) 0x6C, (byte) 0x69,
            (byte) 0x63, (byte) 0x31, (byte) 0x10, (byte) 0x30, (byte) 0x0E, (byte) 0x06, (byte) 0x03, (byte) 0x55,
            (byte) 0x04, (byte) 0x07, (byte) 0x13, (byte) 0x07, (byte) 0x4F, (byte) 0x73, (byte) 0x74, (byte) 0x72,
            (byte) 0x61, (byte) 0x76, (byte) 0x61, (byte) 0x31, (byte) 0x0E, (byte) 0x30, (byte) 0x0C, (byte) 0x06,
            (byte) 0x03, (byte) 0x55, (byte) 0x04, (byte) 0x0A, (byte) 0x13, (byte) 0x05, (byte) 0x54, (byte) 0x69,
            (byte) 0x65, (byte) 0x74, (byte) 0x6F, (byte) 0x31, (byte) 0x0C, (byte) 0x30, (byte) 0x0A, (byte) 0x06,
            (byte) 0x03, (byte) 0x55, (byte) 0x04, (byte) 0x0B, (byte) 0x13, (byte) 0x03, (byte) 0x54, (byte) 0x52,
            (byte) 0x44, (byte) 0x31, (byte) 0x15, (byte) 0x30, (byte) 0x13, (byte) 0x06, (byte) 0x03, (byte) 0x55,
            (byte) 0x04, (byte) 0x03, (byte) 0x13, (byte) 0x0C, (byte) 0x4D, (byte) 0x69, (byte) 0x63, (byte) 0x68,
            (byte) 0x61, (byte) 0x6C, (byte) 0x20, (byte) 0x52, (byte) 0x6F, (byte) 0x68, (byte) 0x61, (byte) 0x63,
            (byte) 0x30, (byte) 0x81, (byte) 0x9F, (byte) 0x30, (byte) 0x0D, (byte) 0x06, (byte) 0x09, (byte) 0x2A,
            (byte) 0x86, (byte) 0x48, (byte) 0x86, (byte) 0xF7, (byte) 0x0D, (byte) 0x01, (byte) 0x01, (byte) 0x01,
            (byte) 0x05, (byte) 0x00, (byte) 0x03, (byte) 0x81, (byte) 0x8D, (byte) 0x00, (byte) 0x30, (byte) 0x81,
            (byte) 0x89, (byte) 0x02, (byte) 0x81, (byte) 0x81, (byte) 0x00, (byte) 0xA2, (byte) 0xD4, (byte) 0xA9,
            (byte) 0x5F, (byte) 0xE0, (byte) 0x66, (byte) 0xB6, (byte) 0xE6, (byte) 0x66, (byte) 0x3E, (byte) 0xF5,
            (byte) 0x76, (byte) 0x27, (byte) 0x20, (byte) 0x04, (byte) 0x6B, (byte) 0x59, (byte) 0xA4, (byte) 0xE1,
            (byte) 0x9A, (byte) 0x6F, (byte) 0x72, (byte) 0x9E, (byte) 0x2B, (byte) 0x1E, (byte) 0x38, (byte) 0x49,
            (byte) 0xA5, (byte) 0xDA, (byte) 0xB2, (byte) 0x60, (byte) 0x89, (byte) 0x73, (byte) 0x24, (byte) 0x93,
            (byte) 0x1C, (byte) 0x5C, (byte) 0x93, (byte) 0x01, (byte) 0x6E, (byte) 0xF0, (byte) 0xC5, (byte) 0xBA,
            (byte) 0x56, (byte) 0x71, (byte) 0x5B, (byte) 0xDB, (byte) 0x8B, (byte) 0x60, (byte) 0xFD, (byte) 0xD9,
            (byte) 0xF2, (byte) 0x45, (byte) 0x5F, (byte) 0xA2, (byte) 0xF8, (byte) 0xF4, (byte) 0x78, (byte) 0x5E,
            (byte) 0x33, (byte) 0x20, (byte) 0x50, (byte) 0xFF, (byte) 0xAD, (byte) 0xE0, (byte) 0x5F, (byte) 0x9C,
            (byte) 0x90, (byte) 0x34, (byte) 0x18, (byte) 0x4A, (byte) 0x8D, (byte) 0x6B, (byte) 0x40, (byte) 0x72,
            (byte) 0x8A, (byte) 0xCA, (byte) 0xBD, (byte) 0x94, (byte) 0xEA, (byte) 0x80, (byte) 0xAE, (byte) 0x6F,
            (byte) 0xA8, (byte) 0xD4, (byte) 0x00, (byte) 0x31, (byte) 0x5A, (byte) 0xBC, (byte) 0x01, (byte) 0x0D,
            (byte) 0xEB, (byte) 0xA0, (byte) 0x77, (byte) 0x92, (byte) 0xAD, (byte) 0x34, (byte) 0x0F, (byte) 0xAC,
            (byte) 0x5B, (byte) 0x16, (byte) 0x71, (byte) 0xFF, (byte) 0x80, (byte) 0x91, (byte) 0x43, (byte) 0x19,
            (byte) 0x7E, (byte) 0x50, (byte) 0xA5, (byte) 0xC6, (byte) 0xAC, (byte) 0xB9, (byte) 0xBC, (byte) 0xD3,
            (byte) 0xB9, (byte) 0x91, (byte) 0xC3, (byte) 0x60, (byte) 0x85, (byte) 0xFD, (byte) 0xD4, (byte) 0x13,
            (byte) 0xF0, (byte) 0xB8, (byte) 0xD7, (byte) 0xD3, (byte) 0x27, (byte) 0x02, (byte) 0x03, (byte) 0x01,
            (byte) 0x00, (byte) 0x01, (byte) 0x30, (byte) 0x0D, (byte) 0x06, (byte) 0x09, (byte) 0x2A, (byte) 0x86,
            (byte) 0x48, (byte) 0x86, (byte) 0xF7, (byte) 0x0D, (byte) 0x01, (byte) 0x01, (byte) 0x05, (byte) 0x05,
            (byte) 0x00, (byte) 0x03, (byte) 0x81, (byte) 0x81, (byte) 0x00, (byte) 0x93, (byte) 0xBF, (byte) 0xF5,
            (byte) 0x08, (byte) 0x3B, (byte) 0x3F, (byte) 0xE4, (byte) 0xF4, (byte) 0x61, (byte) 0x34, (byte) 0xFC,
            (byte) 0x43, (byte) 0x69, (byte) 0x2E, (byte) 0x42, (byte) 0x3E, (byte) 0x26, (byte) 0x39, (byte) 0x1C,
            (byte) 0x61, (byte) 0xB4, (byte) 0x48, (byte) 0x60, (byte) 0xFC, (byte) 0x63, (byte) 0x4F, (byte) 0x1E,
            (byte) 0xBA, (byte) 0x98, (byte) 0xDF, (byte) 0xA8, (byte) 0xAA, (byte) 0x7E, (byte) 0xCB, (byte) 0xC6,
            (byte) 0x8E, (byte) 0x31, (byte) 0x11, (byte) 0xEE, (byte) 0x2B, (byte) 0xF9, (byte) 0xC1, (byte) 0xB9,
            (byte) 0x29, (byte) 0xA2, (byte) 0xD6, (byte) 0x27, (byte) 0x78, (byte) 0xD1, (byte) 0x06, (byte) 0x87,
            (byte) 0x03, (byte) 0x81, (byte) 0x73, (byte) 0xD5, (byte) 0xB9, (byte) 0xC1, (byte) 0x47, (byte) 0xE6,
            (byte) 0x7D, (byte) 0x98, (byte) 0xBD, (byte) 0x74, (byte) 0xE7, (byte) 0xDB, (byte) 0x45, (byte) 0x1B,
            (byte) 0x9F, (byte) 0xF0, (byte) 0x18, (byte) 0xCC, (byte) 0x27, (byte) 0x3D, (byte) 0xC5, (byte) 0x71,
            (byte) 0x95, (byte) 0x04, (byte) 0xFD, (byte) 0x50, (byte) 0x21, (byte) 0x68, (byte) 0x33, (byte) 0x3B,
            (byte) 0x3B, (byte) 0x47, (byte) 0x1A, (byte) 0xB1, (byte) 0x35, (byte) 0xC6, (byte) 0x6B, (byte) 0xF1,
            (byte) 0x6A, (byte) 0xB2, (byte) 0x1B, (byte) 0x9C, (byte) 0xCF, (byte) 0xCD, (byte) 0x68, (byte) 0x20,
            (byte) 0xC9, (byte) 0x4B, (byte) 0x07, (byte) 0xB3, (byte) 0x36, (byte) 0x84, (byte) 0x1B, (byte) 0x7A,
            (byte) 0x42, (byte) 0xE8, (byte) 0x98, (byte) 0x5F, (byte) 0x2B, (byte) 0x09, (byte) 0x30, (byte) 0xBF,
            (byte) 0xA8, (byte) 0x7E, (byte) 0x7A, (byte) 0x7D, (byte) 0xE3, (byte) 0xE6, (byte) 0x5F, (byte) 0xEB,
            (byte) 0x1F, (byte) 0xAE, (byte) 0x89, (byte) 0x42, (byte) 0x91, (byte) 0x5F, (byte) 0x41, (byte) 0xAA,
            (byte) 0x75, (byte) 0xAB, (byte) 0xBB, (byte) 0xF6, (byte) 0xDA, (byte) 0x11, (byte) 0x0D, (byte) 0x90,
            (byte) 0x12, (byte) 0x76, (byte) 0x7E, (byte) 0x89, (byte) 0x4D, (byte) 0x92, (byte) 0x2C, (byte) 0x36,
            (byte) 0x3F};
}
