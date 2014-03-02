package com.stericsson.sdk.signing.signerservice;

import java.io.IOException;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.security.KeyManagementException;
import java.security.NoSuchAlgorithmException;
import java.security.SecureRandom;

import javax.net.ssl.HandshakeCompletedEvent;
import javax.net.ssl.HandshakeCompletedListener;
import javax.net.ssl.SSLContext;
import javax.net.ssl.SSLSocket;
import javax.net.ssl.SSLSocketFactory;
import javax.net.ssl.X509TrustManager;

import org.apache.log4j.Logger;


/**
 * This factory class creates a socket to a remote signer service.
 * 
 * @author xtomlju
 */
public final class SignerServiceSocketFactory {

    /**
     * @param address
     *            Internet socket address to use
     * @param logger
     *            logger instance to use for outputting logging messages
     * @return Socket to remote signer service server
     * @throws SignerServiceException
     *             If a signer service related error occurred. 
     */
    public static Socket createSocket(final InetSocketAddress address, final Logger logger)
        throws SignerServiceException {
        SSLContext sslContext;
        SSLSocketFactory sslSocketFactory;
        SSLSocket sslSocket = null;

        try {
            sslContext = SSLContext.getInstance("SSL");
            sslContext.init(null, new X509TrustManager[] {
                new SignerServiceTrustManager()}, new SecureRandom(new byte[] {
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
        } catch (NoSuchAlgorithmException e) {
            throw new SignerServiceException("");
        } catch (KeyManagementException e) {
            throw new SignerServiceException("");
        } catch (IOException e) {
            logger.warn("Unable to connect to signer service server " + address.getHostName() + " at port "
                + address.getPort());
        }

        return sslSocket;
    }

    private SignerServiceSocketFactory() {

    }
}
