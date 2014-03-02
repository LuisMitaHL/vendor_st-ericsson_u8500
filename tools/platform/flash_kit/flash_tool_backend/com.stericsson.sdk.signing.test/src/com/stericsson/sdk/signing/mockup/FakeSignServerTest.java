package com.stericsson.sdk.signing.mockup;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.security.SecureRandom;
import java.security.cert.CertificateException;
import java.security.cert.X509Certificate;
import java.util.Arrays;

import javax.net.ssl.HandshakeCompletedEvent;
import javax.net.ssl.HandshakeCompletedListener;
import javax.net.ssl.SSLContext;
import javax.net.ssl.SSLSocket;
import javax.net.ssl.SSLSocketFactory;
import javax.net.ssl.X509TrustManager;

import junit.framework.TestCase;

import org.apache.log4j.Logger;
import org.junit.After;
import org.junit.Before;
import org.junit.Test;

/**
 * @author emicroh
 * 
 */
public class FakeSignServerTest extends TestCase {

    private static final Logger log = Logger.getLogger("FakeSignServerTest");

    private FakeSignServer signServer;

    /**
     * @throws Exception
     *             TBD
     */
    @Before
    public void setUp() throws Exception {
        signServer = new FakeSignServer();
    }

    /**
     * 
     */
    @After
    public void endUp() {
        signServer.close();
    }

    /**
     * @throws Throwable
     *             TBD
     */
    @Test
    public void testFakeSignServerProtocolStartedByClient() throws Throwable {
        InputStream in = null;
        OutputStream out = null;
        SSLSocket clientSocket = null;
        try {
            MockProtocolMessage[] protocolMessages =
                new MockProtocolMessage[] {
                    new MockProtocolMessage(new FakeMessage("Hello server.."), new FakeMessage(
                        "Hi client, what do you want?")),
                    new MockProtocolMessage(new FakeMessage("Nothing, just kidding.."), new FakeMessage(
                        "Sorry, I'm bussy"))};

            signServer.setMockMessages(protocolMessages);
            signServer.startServer();

            // connect
            clientSocket =
                createSocket(new InetSocketAddress(signServer.getServerSocket().getInetAddress(), signServer
                    .getServerSocket().getLocalPort()), log);

            // verify connection
            // Assert.assertTrue(clientSocket.getLocalPort() ==
            // rttServer.getAcceptedSocket().getPort());

            byte[] readBuf;

            in = clientSocket.getInputStream();
            out = clientSocket.getOutputStream();

            for (int i = 0; i < protocolMessages.length; i++) {
                out.write(protocolMessages[i].getClientMessage().getMsg());
                System.out.println("Client sent : "
                    + new String(protocolMessages[i].getClientMessage().getMsg(), "UTF-8"));
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
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
            signServer.close();
        }

    }

    /**
     * @throws Throwable
     *             TBD
     */
    @Test
    public void testFakeSignServerProtocolStartedByServer() throws Throwable {
        InputStream in = null;
        OutputStream out = null;
        SSLSocket clientSocket = null;
        try {
            MockProtocolMessage[] protocolMessages =
                new MockProtocolMessage[] {
                    new MockProtocolMessage(new FakeMessage("Fine..."), new FakeMessage("Hi client, how are you?"),
                        true),
                    new MockProtocolMessage(new FakeMessage("No..."), new FakeMessage(
                        "Hey client, you don't talk much."), true)};

            signServer.setMockMessages(protocolMessages);
            signServer.startServer();

            // connect
            clientSocket =
                createSocket(new InetSocketAddress(signServer.getServerSocket().getInetAddress(), signServer
                    .getServerSocket().getLocalPort()), log);

            // verify connection
            // Assert.assertTrue(clientSocket.getLocalPort() ==
            // rttServer.getAcceptedSocket().getPort());

            byte[] readBuf;

            in = clientSocket.getInputStream();
            out = clientSocket.getOutputStream();

            for (int i = 0; i < protocolMessages.length; i++) {
                readBuf = new byte[protocolMessages[i].getServerResponse().getMsg().length];
                int read = in.read(readBuf);
                if (read < 0) {
                    fail("Read failed");
                }
                System.out.println("Server sent : " + new String(readBuf, "UTF-8"));

                out.write(protocolMessages[i].getClientMessage().getMsg());
                System.out.println("Client sent : "
                    + new String(protocolMessages[i].getClientMessage().getMsg(), "UTF-8"));
                assertTrue(Arrays.equals(readBuf, protocolMessages[i].getServerResponse().getMsg()));
            }
        } finally {
            if (in != null) {
                in.close();
            }
            if (out != null) {
                out.close();
            }
            try {
                if (clientSocket != null) {
                    clientSocket.close();
                }
            } catch (Exception e) {
                e.printStackTrace();
            }
            signServer.close();
        }

    }

    /**
     * @throws Throwable
     *             TBD
     */
    @Test
    public void testFakeSignServer() throws Throwable {
        signServer.startServer();

        Socket clSocket =
            createSocket(new InetSocketAddress(signServer.getServerSocket().getInetAddress(), signServer
                .getServerSocket().getLocalPort()), log);

        assertNotNull(clSocket);
        Socket acceptedSocket = signServer.getAcceptedSocket();
        if (acceptedSocket == null) {
            fail("Socket is null");
            throw new IOException();
        }
        assertTrue(clSocket.getPort() == acceptedSocket.getLocalPort());
    }

    private static SSLSocket createSocket(final InetSocketAddress address, final Logger logger) throws Throwable {
        SSLContext sslContext;
        SSLSocketFactory sslSocketFactory;
        SSLSocket sslSocket = null;

        sslContext = SSLContext.getInstance("SSL");
        sslContext.init(null, new X509TrustManager[] {
            new FakeTrustManager()}, new SecureRandom(new byte[] {
            1, 2, 3, 4, 5, 6, 7, 8}));
        sslSocketFactory = sslContext.getSocketFactory();
        sslSocket = (SSLSocket) sslSocketFactory.createSocket(address.getAddress(), address.getPort());
        sslSocket.addHandshakeCompletedListener(new HandshakeCompletedListener() {
            public void handshakeCompleted(HandshakeCompletedEvent hce) {
                logger.info("SSL handshake completed with sign server " + address.getHostName() + " at port "
                    + address.getPort());
            }
        });
        sslSocket.startHandshake();

        return sslSocket;
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

}
