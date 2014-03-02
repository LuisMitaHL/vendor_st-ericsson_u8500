package com.stericsson.sdk.signing.signerservice.local.encryption;

import java.io.IOException;
import java.security.interfaces.RSAPrivateKey;

import junit.framework.TestCase;

import com.stericsson.sdk.signing.test.Activator;
import com.stericsson.sdk.signing.util.PEMParser;


/**
 * @author Xxvs0002
 *
 */
public class RSAPKCS1EncrypterTest extends TestCase{

    RSAPKCS1Encrypter rsapkcs1Encrypter = new RSAPKCS1Encrypter();

    static final String PRIVATE_PEM_KEY = "/u5500/1024_priv.pem";

    RSAPrivateKey fromPriv = null;

    byte[] data = new byte[]{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    /**
     * 
     */
    public void testCalculateRSAPKCS1Signature(){
        String privKey = Activator.getResourcesPath() + PRIVATE_PEM_KEY;
        try {
            fromPriv = PEMParser.parsePrivateKey(privKey);
        } catch (IOException e) {
            e.printStackTrace();
        }

        byte[] returnData = rsapkcs1Encrypter.calculateRSAPKCS1Signature(data, fromPriv);

        assertNotNull(returnData);
    }

}
