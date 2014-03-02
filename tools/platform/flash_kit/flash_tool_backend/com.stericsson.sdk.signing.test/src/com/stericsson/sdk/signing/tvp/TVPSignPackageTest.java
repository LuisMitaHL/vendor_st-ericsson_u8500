package com.stericsson.sdk.signing.tvp;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;

import junit.framework.TestCase;

import org.junit.Test;

import com.stericsson.sdk.signing.test.Activator;

/**
 * 
 * @author xolabju
 * 
 */
public class TVPSignPackageTest extends TestCase {

    /**
     * @throws Exception
     *             on errors
     */
    @Test
    public void testX509SignPackage() throws Exception {
        String alias = "alias123";
        String type = "type123";
        int headerId = 1233245234;
        X509CertificateChain chain = createChain();
        TVPSignPackage pkg = new TVPSignPackage(alias, type, headerId, chain);
        assertEquals(chain, pkg.getCertificateChain());
        assertEquals(headerId, pkg.getHeaderVersion());
        assertEquals(alias, pkg.getAlias());
        assertEquals(type, pkg.getType());
    }

    private X509CertificateChain createChain() throws Exception {
        File file = new File(Activator.getResourcesPath() +"/tvp/CertificateChain.bin");
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
        return new X509CertificateChain(bytes);

    }

}
