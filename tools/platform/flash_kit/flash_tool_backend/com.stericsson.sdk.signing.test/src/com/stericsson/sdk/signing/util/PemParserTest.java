package com.stericsson.sdk.signing.util;

import java.security.interfaces.RSAPublicKey;
import java.util.Arrays;

import junit.framework.TestCase;

import com.stericsson.sdk.signing.generic.cert.subparts.GenericKey;
import com.stericsson.sdk.signing.test.Activator;

/**
 * 
 * @author xolabju
 * 
 */
public class PemParserTest extends TestCase {

    /***/
    public static final String PUBLIC_PEM_KEY = "/u5500/1024_pub.pem";

    /***/
    public static final String PRIVATE_PEM_KEY = "/u5500/1024_priv.pem";

    /**
     * 
     */
    public void testParseRSAPublicKey() {
        try {
            String privKey = Activator.getResourcesPath() + PRIVATE_PEM_KEY;
            RSAPublicKey fromPriv = PEMParser.parseRSAPublicKey(privKey);

            String pubKey = Activator.getResourcesPath() + PUBLIC_PEM_KEY;
            RSAPublicKey fromPub = PEMParser.parseRSAPublicKey(pubKey);

            assertNotNull(fromPriv);
            assertNotNull(fromPub);

            if (fromPriv == null || fromPub == null) {
                throw new Exception("Got null key");
            }
            assertEquals(fromPub.getPublicExponent().intValue(), fromPriv.getPublicExponent().intValue());
            assertTrue(Arrays.equals(fromPub.getModulus().toByteArray(), fromPriv.getModulus().toByteArray()));

            GenericKey u5500key = PEMParser.parseU5500Key(pubKey);
            assertNotNull(u5500key);
            assertEquals(128, u5500key.getModulusSize());
            assertEquals(128, u5500key.getModulus().length);
            assertEquals(fromPub.getPublicExponent().intValue(), u5500key.getPublicExponent());

        } catch (Exception e) {
            fail(e.getMessage());
        }
    }

}
