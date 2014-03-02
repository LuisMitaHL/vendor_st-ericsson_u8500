package com.stericsson.sdk.signing.signerservice;

import java.net.InetSocketAddress;
import java.net.Socket;

import junit.framework.TestCase;

import org.apache.log4j.Logger;
import org.junit.Test;

import com.stericsson.sdk.signing.mockup.FakeSignServer;

/**
 * @author emicroh
 */
public class SignerServiceSocketFactoryTest extends TestCase {

    private static Logger log = Logger.getLogger(SignerServiceSocketFactoryTest.class.getName());

    /**
     * @throws Throwable
     *             When something went wrong.
     */
    @Test
    public void testCreateSocket() throws Throwable {
        FakeSignServer signServer = new FakeSignServer();
        signServer.startServer();

        Socket clSocket =
            SignerServiceSocketFactory.createSocket(new InetSocketAddress(
                signServer.getServerSocket().getInetAddress(), signServer.getServerSocket().getLocalPort()), log);

        Socket acceptedSocket = signServer.getAcceptedSocket();
        if (clSocket == null || acceptedSocket == null) {
            throw new Exception("Socket is null.");
        }
        assertNotNull(clSocket);
        assertTrue(clSocket.getPort() == acceptedSocket.getLocalPort());

        signServer.close();
    }

}
