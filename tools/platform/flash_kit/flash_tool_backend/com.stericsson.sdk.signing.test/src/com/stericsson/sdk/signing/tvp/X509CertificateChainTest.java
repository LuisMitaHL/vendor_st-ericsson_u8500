package com.stericsson.sdk.signing.tvp;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.security.cert.Certificate;
import java.security.cert.CertificateFactory;
import java.security.cert.X509Certificate;
import java.util.Arrays;

import junit.framework.TestCase;

import org.junit.Test;

import com.stericsson.sdk.signing.test.Activator;

/**
 * 
 * @author xolabju
 * 
 */
public class X509CertificateChainTest extends TestCase {

    /***/
    public static final String TEST_CHAIN = "/tvp/CertificateChain.bin";

    /***/
    public static final String TEST_CERT = "/tvp/x509_cert.der";

    /**
     * 
     * @throws Exception
     *             on errors
     */
    @Test
    public void testX509CertificateChain() throws Exception {
        FileInputStream fis = null;
        X509CertificateChain chain1 = null;
        X509CertificateChain chain2 = null;

        try {
            CertificateFactory cf = CertificateFactory.getInstance("X.509");
            fis = new FileInputStream(new File(Activator.getResourcesPath() + TEST_CERT));

            Certificate tempCert = cf.generateCertificate(fis);
            if (tempCert != null && tempCert instanceof X509Certificate) {
                X509Certificate cert = (X509Certificate) tempCert;
                chain1 = new X509CertificateChain(new X509Certificate[] {
                    cert});

                byte[] chainBytes = readFromFile(Activator.getResourcesPath() + TEST_CHAIN);
                chain2 = new X509CertificateChain(chainBytes);

                assertTrue(Arrays.equals(chain1.getBytes(), chain2.getBytes()));

            } else {
                fail();
            }
        } finally {
            if (fis != null) {
                fis.close();
            }
        }

    }

    private byte[] readFromFile(String filename) throws Exception {
        File file = new File(filename);
        byte[] bytes = null;
        if (file == null) {
            throw new Exception("File is null");
        }
        FileInputStream fis = null;
        try {
            fis = new FileInputStream(file);
            bytes = new byte[(int) file.length()];
            int read = fis.read(bytes);
            if (read < 0) {
                throw new Exception("Failed to read from " + file.getAbsolutePath());
            }
        } finally {
            if (fis != null) {
                try {
                    fis.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }
        return bytes;
    }

}
