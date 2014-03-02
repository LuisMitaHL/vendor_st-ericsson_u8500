package com.stericsson.sdk.signing.mockup;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;
import java.util.Arrays;

import junit.framework.TestCase;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

/**
 * @author emicroh
 * 
 */
public class FakeRTTServerTest extends TestCase {

    private FakeRTTServer rttServer;

    /**
     * 
     */
    @Before
    public void init() {
        rttServer = new FakeRTTServer();
    }

    /**
     * 
     */
    @After
    public void endUp() {
        rttServer.close();
    }

    /**
     * @throws IOException
     *             TBD
     */
    @Test
    public void testConnection() throws IOException {
        Socket clientSocket = null;
        try {
            // for Junit3 since it does not support "Before" tag
            if (rttServer == null) {
                init();
            }
            if (rttServer == null) {
                throw new IOException("Server is null");
            }
            rttServer.startServer();
            // connect
            clientSocket =
                new Socket(rttServer.getServerSocket().getInetAddress(), rttServer.getServerSocket().getLocalPort());
            rttServer.joinServerSocketThread();
            Socket acceptedSocket = rttServer.getAcceptedSocket();
            if (acceptedSocket == null) {
                fail("Socket is null");
                throw new IOException();
            }
            assertTrue(clientSocket.getLocalPort() == acceptedSocket.getPort());

        } catch (InterruptedException e) {
            e.printStackTrace();
        } finally {
            if (clientSocket != null) {
                try {
                    clientSocket.close();
                    endUp();
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }
    }

    /**
     * @throws Throwable
     *             TBD
     */
    @Test
    public void testCommunication() throws Throwable {
        InputStream in = null;
        OutputStream out = null;
        Socket clientSocket = null;
        try {
            MockProtocolMessage[] protocolMessages =
                new MockProtocolMessage[] {
                    new MockProtocolMessage(new FakeMessage("Hello server.."), new FakeMessage(
                        "Hi client, what do you want?")),
                    new MockProtocolMessage(new FakeMessage("Nothing, just kidding.."), new FakeMessage(
                        "Sorry, I'm bussy"))};

            // for Junit3 since it does not support "Before" tag
            if (rttServer == null) {
                init();
            }
            if (rttServer == null) {
                throw new IOException("Server is null");
            }
            rttServer.setMockMessages(protocolMessages);
            rttServer.startServer();

            // connect
            clientSocket =
                new Socket(rttServer.getServerSocket().getInetAddress(), rttServer.getServerSocket().getLocalPort());
            // verify connection
            // Assert.assertTrue(clientSocket.getLocalPort() ==
            // rttServer.getAcceptedSocket().getPort());

            byte[] readBuf;

            in = clientSocket.getInputStream();
            out = clientSocket.getOutputStream();

            for (int i = 0; i < protocolMessages.length; i++) {
                out.write(protocolMessages[i].getClientMessage().getMsg());
                readBuf = new byte[protocolMessages[i].getServerResponse().getMsg().length];
                int read = in.read(readBuf);
                if (read < 0) {
                    fail("Read failed");
                }

                assertTrue(Arrays.equals(readBuf, protocolMessages[i].getServerResponse().getMsg()));
                System.out.println("Server sent back: " + new String(readBuf, "UTF-8"));
            }
        } finally {
            if (in != null) {
                in.close();
            }
            if (out != null) {
                out.close();
            }
            if (clientSocket != null) {
                try {
                    clientSocket.close();
                    endUp();
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }
    }

    /**
     * @throws Throwable
     *             TBD
     */
    @Test
    public void testCommunication2() throws Throwable {
        InputStream in = null;
        OutputStream out = null;
        Socket clientSocket = null;
        try {
            MockProtocolMessage[] protocolMessages =
                new MockProtocolMessage[] {
                    new MockProtocolMessage(new FakeMessage(""), new FakeMessage("Hello?"), true),
                    new MockProtocolMessage(new FakeMessage("Hi.."), new FakeMessage(""))};

            // for Junit3 since it does not support "Before" tag
            if (rttServer == null) {
                init();
            }
            if (rttServer == null) {
                throw new IOException("Server is null");
            }
            rttServer.setMockMessages(protocolMessages);
            rttServer.startServer();

            // connect
            clientSocket =
                new Socket(rttServer.getServerSocket().getInetAddress(), rttServer.getServerSocket().getLocalPort());
            // verify connection
            // Assert.assertTrue(clientSocket.getLocalPort() ==
            // rttServer.getAcceptedSocket().getPort());

            byte[] readBuf;

            in = clientSocket.getInputStream();
            out = clientSocket.getOutputStream();

            for (int i = 0; i < protocolMessages.length; i++) {
                out.write(protocolMessages[i].getClientMessage().getMsg());
                readBuf = new byte[protocolMessages[i].getServerResponse().getMsg().length];
                int read = in.read(readBuf);
                if (read < 0) {
                    fail("Read failed");
                }

                assertTrue(Arrays.equals(readBuf, protocolMessages[i].getServerResponse().getMsg()));
                System.out.println("Server sent back: " + new String(readBuf, "UTF-8"));
            }
        } finally {
            if (in != null) {
                in.close();
            }
            if (out != null) {
                out.close();
            }
            if (clientSocket != null) {
                try {
                    clientSocket.close();
                    endUp();
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }

    }

}
