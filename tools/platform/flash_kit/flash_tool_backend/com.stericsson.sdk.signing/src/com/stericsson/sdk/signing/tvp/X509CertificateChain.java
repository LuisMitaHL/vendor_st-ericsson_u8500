package com.stericsson.sdk.signing.tvp;

import java.io.ByteArrayInputStream;
import java.nio.ByteBuffer;
import java.security.cert.CertificateFactory;
import java.security.cert.X509Certificate;

/**
 * 
 * @author xolabju
 * 
 *         Represents an X.509 certificate chain
 */
public class X509CertificateChain {

    /**
     * The certificate chain.
     */
    private X509Certificate[] mChain;

    /**
     * Create an X509CertificateChain. chain should have all the certificates in the chain from the
     * signing certificate up to, but excluding, the root. The signing certificate is at index 0,
     * each following certificate should certify the one preceding it in the chain.
     * 
     * @param chain
     *            the chain
     */
    public X509CertificateChain(X509Certificate[] chain) {
        mChain = chain;
    }

    /**
     * Create the certificate chain using just a byte array, this can be used when parsing a sign
     * package
     * 
     * @param base
     *            byte array that is the certificate chain that exists in the sign package, it must
     *            be in the header format(see getBytes()).
     */
    public X509CertificateChain(byte[] base) {
        try {
            CertificateFactory cf = CertificateFactory.getInstance("X.509");
            ByteBuffer bb = ByteBuffer.wrap(base);
            bb.rewind();

            int nbrCerts = bb.getInt();

            mChain = new X509Certificate[nbrCerts];
            for (int i = 0; i < nbrCerts; i++) {
                int currCertLength = bb.getInt();
                byte[] bcert = new byte[currCertLength];
                bb.get(bcert);
                ByteArrayInputStream bis = new ByteArrayInputStream(bcert);
                mChain[i] = (X509Certificate) cf.generateCertificate(bis);
            }
        } catch (Exception e) {
            mChain = null;
            throw new IllegalArgumentException("Could not parse certificate chain. Exception message: "
                + e.getMessage());
        }
    }

    /**
     * Returns the chain in a byte array, in the format that it should have in the header, i.e:
     * first 4 bytes denotes number of certificates in chain, N Certificate j, 1 <= j <= N, is
     * represented by 4+Lj bytes, the first 4 bytes denotes the length Lj of the certificate and the
     * following Lj bytes is the certificate
     * 
     * @return The certificate chain in the byte representation it should have in the header
     */
    public byte[] getBytes() {
        try {
            ByteBuffer bb = ByteBuffer.allocate(getLength());
            bb.rewind();
            bb.putInt(mChain.length);
            for (int i = 0; i < mChain.length; i++) {
                X509Certificate cert = mChain[i];
                byte[] encoded = cert.getEncoded();
                bb.putInt(encoded.length);
                bb.put(encoded);
            }
            return bb.array();
        } catch (Exception e) {
            throw new IllegalArgumentException(
                "Could not generate byte array representing certificate chain. Exception message: " + e.getMessage());
        }
    }

    private int getLength() throws Exception {
        int length = 4;
        if (mChain != null) {
            for (X509Certificate cert : mChain) {
                if (cert != null) {
                    length += 4 + cert.getEncoded().length;
                }
            }
        }
        return length;
    }

}
