package com.stericsson.sdk.signing.signerservice;

import java.security.cert.CertificateException;
import java.security.cert.X509Certificate;

import javax.net.ssl.X509TrustManager;

/**
 * Dummy implementation of the X509TrustManager interface to be used by the
 * SignerServiceSocketFactory for SSL handling.
 * 
 * @author xtomlju
 */
public class SignerServiceTrustManager implements X509TrustManager {

    /*
     * (non-Javadoc)
     * 
     * @see javax.net.ssl.X509TrustManager#getAcceptedIssuers()
     */
    /**
     * @return An array of certificates of accepted issuers
     */
    public X509Certificate[] getAcceptedIssuers() {
        return null;
    }

    /*
     * (non-Javadoc)
     * 
     * @see javax.net.ssl.X509TrustManager#checkClientTrusted(java.security.cert.X509Certificate[],
     *      java.lang.String)
     */
    /**
     * @param certificates
     *            Array of certificates
     * @param arg1
     *            TBD
     * @throws CertificateException
     *             If a certificate related error occurred
     */
    public void checkClientTrusted(X509Certificate[] certificates, String arg1) throws CertificateException {
    }

    /*
     * (non-Javadoc)
     * 
     * @see javax.net.ssl.X509TrustManager#checkServerTrusted(java.security.cert.X509Certificate[],
     *      java.lang.String)
     */
    /**
     * @param certificates
     *            Array of certificates
     * @param arg1
     *            TBD
     * @throws CertificateException
     *             If a certificate related error occurred
     */
    public void checkServerTrusted(X509Certificate[] certificates, String arg1) throws CertificateException {
    }

}
