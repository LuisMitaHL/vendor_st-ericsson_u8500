package com.stericsson.sdk.signing.signerservice.local.keys;

import java.io.BufferedReader;
import java.io.ByteArrayInputStream;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.security.KeyFactory;
import java.security.PrivateKey;
import java.security.interfaces.RSAPrivateKey;
import java.security.spec.PKCS8EncodedKeySpec;

import org.apache.log4j.Logger;
import org.bouncycastle.asn1.ASN1InputStream;
import org.bouncycastle.asn1.ASN1Sequence;
import org.bouncycastle.asn1.DERObject;
import org.bouncycastle.asn1.pkcs.RSAPrivateKeyStructure;

import com.stericsson.sdk.common.HexUtilities;
import com.stericsson.sdk.signing.util.PEMParser;

/**
 * @author efreson, xolabju
 * 
 *         Parses different format keys from various inputs into RSAKeys.
 */
public final class KeyParser {

    private static Logger logger = Logger.getLogger(KeyParser.class.getName());

    private KeyParser() {
    }

    /**
     * Creates and returns an RSAPrivateKey from InputStream is. The key must be a PKCS#8 or PKCS#1
     * encoded private key.
     * 
     * @param is
     *            - The InputStream the key is read from
     * @return The RSAPrivateKey that was created from the specified file.
     * @throws IOException
     *             if any errors occur.
     */
    public static RSAPrivateKey parseX509Key(InputStream is) throws IOException {
        byte[] kvalue = new byte[is.available()];
        int read = is.read(kvalue);
        if (read < 1) {
            throw new IOException("Failed to read key");
        }
        PrivateKey pkey = null;
        PKCS8EncodedKeySpec keyspec = new PKCS8EncodedKeySpec(kvalue);
        try {
            KeyFactory keyFactory = KeyFactory.getInstance("RSA");
            pkey = keyFactory.generatePrivate(keyspec);
        } catch (Exception e) {
            // ignore
            e.getMessage();
        }
        if (pkey != null && pkey instanceof RSAPrivateKey) {
            return (RSAPrivateKey) pkey;
        }
        try {
            ByteArrayInputStream bais = new ByteArrayInputStream(kvalue);
            // Can't use InputStream is, because it's already read from (null)
            ASN1InputStream ais = new ASN1InputStream(bais);
            DERObject dobj = ais.readObject();
            ASN1Sequence priv = (ASN1Sequence) dobj; // Must be an
            // ASN1Sequence
            RSAPrivateKeyStructure rsap = new RSAPrivateKeyStructure(priv);
            pkey = new COPSRSAPrivateKey(rsap.getModulus(), rsap.getPrivateExponent());
        } catch (Exception e) {
            // ignore
            e.getMessage();
        }
        if (pkey != null && pkey instanceof RSAPrivateKey) {
            return (RSAPrivateKey) pkey;
        }
        // check more formats here...
        throw new IOException("Input key is not a PKCS#8 or PKCS#1 encoded RSA private key");
    }

    /**
     * Parses an A2 private key in text format into an RSAPrivateKey.
     * 
     * Example:
     * 
     * PrivateKey: 606 30 82 02 5a 02 01 00 02 81 81 00 97 a4 34 db 26 4d 8d e0 bc e3 e5 60 10 6a 62
     * 32 ca 68 98 22 6c 15 09 81 fb 88 ae ec f2 b1 39 a4 04 70 ce dc 57 82 60 5b 36 98 66 55 89 f7
     * 91 8b be ba b8 9e 1e d6 75 b2 c7 81 9f 8b dd 3a c7 4d 99 39 9e bb 49 c5 ea ff cb aa 99 f9 9e
     * 1c f0 9a 8a 87 6f 28 dd ee b6 d2 2c f4 5e bb b0 63 2d f9 41 10 2b 29 77 bd 19 a6 ce 17 fb 9e
     * 06 06 86 e4 9c 7d 63 f4 5d ef 0f b5 28 71 a6 18 be fd b0 6b 02 01 05 02 81 80 3c a8 15 24 75
     * b8 9f 26 b1 f4 c2 26 6c f7 5a 7a b7 5d 09 a7 5e 6e d0 9a 64 9d 12 c5 2d e0 7d 74 ce 93 85 f1
     * bc 9a 8c f1 49 09 c2 88 9d 96 3a 37 e5 e4 49 d8 d9 22 95 7a b6 33 d9 6b 25 4a b6 1e 6c 58 35
     * f4 1b 52 6e 12 74 56 f3 09 90 32 d9 e6 4f 28 3a 0f 22 42 bb 69 53 c2 64 ab 1d f3 05 f2 04 d5
     * 3c f9 e7 13 77 f7 fb 0b 25 85 50 23 39 90 2b 9f 63 2c 95 a6 a0 88 1f 95 44 84 11 46 46 6d 02
     * 41 00 bd 67 ea 47 f4 a6 a6 24 b2 62 cc fe 85 e7 24 8d 1b 59 06 e5 ee fb 0f d9 7d e6 82 c9 c5
     * b1 a8 11 f1 4e 46 ee ea d4 d1 0e c2 9c 95 39 3a a0 89 60 e4 c2 0c 55 f5 87 88 96 1b 86 2e d1
     * 00 9d c7 c9 02 41 00 cc f5 2d 91 10 d1 31 ac f6 6e 6d 63 2f b6 ab 4d a9 49 d7 1d 19 4c d2 71
     * 5d a7 e0 46 1f d1 f7 0a 43 ac cb c9 cb 37 9c ac 17 df 88 97 83 0d ed 9b 4a ac df ae f2 46 f5
     * ca bd f6 4b fd 93 30 38 93 02 41 00 97 86 55 06 5d 52 1e 83 c1 e8 a3 fe d1 85 b6 d7 49 14 05
     * 84 bf 2f 3f e1 31 85 35 6e 37 c1 53 41 8d d8 38 bf 22 43 da 72 35 4a 10 fa 95 4d 3a b3 ea 34
     * d6 ab 2a d2 d3 ab 49 38 25 74 00 7e 39 6d 02 40 51 fb ab d3 a0 53 ad 78 62 92 92 27 ac af 77
     * b8 aa 1d 89 3e d6 eb 87 60 8b dc c0 1c 0c ba 62 d0 e7 de b7 ea 51 49 71 de 6f f3 03 6f ce 05
     * 92 3e 1d de bf df 94 1c 62 51 18 c8 eb 32 3a e0 16 a1 02 40 1c a4 2b b9 72 f2 e5 07 15 a9 60
     * 15 67 22 53 b7 22 56 52 a3 b7 28 94 a8 45 d3 85 1d 0b 67 df 61 92 99 67 55 24 59 a9 b9 6b fc
     * 42 86 7c c0 d2 02 33 9f 19 b8 54 1e e3 30 2e c8 3c 0b 70 1d 7b 10 CID: 3 ID: 6b 2b 05 80 88
     * 58 32 3f b3 2c c6 63 5d 17 59 75 47 9b 28 86
     * 
     * @param filename
     *            - The path to the private key text file.
     * @return The RSAPrivateKey that is parsed from the input file.
     * @throws IOException
     *             If there is a problem parsing the key.
     */
    public static RSAPrivateKey parseA2PrivateKey(String filename) throws IOException {
        FileInputStream fis = null;
        BufferedReader br = null;
        try {
            fis = new FileInputStream(filename);
            br = new BufferedReader(new InputStreamReader(fis, "UTF-8"));
            String line = null;
            // String key = "";
            String skeyLength = "";
            // int keyLength;
            String privKeyId = "PrivateKey:";

            while ((line = br.readLine()) != null) {
                if (line.startsWith(privKeyId)) {
                    skeyLength = line.substring(privKeyId.length()).trim();
                    try {
                        /* keyLength = */Integer.decode(skeyLength).intValue();
                    } catch (Exception e) {
                        logger.warn("Failed to parse A2 key: " + e.getMessage());
                    }
                    line = br.readLine();
                    StringBuffer sBuf = new StringBuffer();
                    while (line != null && !line.startsWith("CID") && !line.startsWith("ID")) { // read
                        sBuf.append(line.replaceAll(" ", ""));
                        line = br.readLine();
                    }
                    ByteArrayInputStream bais = new ByteArrayInputStream(HexUtilities.toByteArray(sBuf.toString()));

                    return parseX509Key(bais);
                }
            }
        } catch (IOException ioe) {
            throw ioe;
        } finally {
            if (fis != null) {
                try {
                    fis.close();
                } catch (Exception e) {
                    logger.warn(e.getMessage());
                }
            }
            if (br != null) {
                try {
                    br.close();
                } catch (Exception e) {
                    logger.warn(e.getMessage());
                }
            }
        }
        throw new IOException("Input string could not be parsed into an A2 private key");
    }

    /**
     * 
     * @param filename
     *            key path
     * @return rsa private key
     * @throws IOException
     *             on errors
     */
    public static RSAPrivateKey parsePEMPrivateKey(String filename) throws IOException {
        return PEMParser.parsePrivateKey(filename);
    }
}
