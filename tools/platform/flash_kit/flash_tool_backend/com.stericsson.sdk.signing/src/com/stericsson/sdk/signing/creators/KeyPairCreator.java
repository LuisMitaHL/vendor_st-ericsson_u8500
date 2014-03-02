package com.stericsson.sdk.signing.creators;

import java.io.File;
import java.io.IOException;
import java.security.KeyPair;
import java.security.KeyPairGenerator;
import java.security.NoSuchAlgorithmException;

import com.stericsson.sdk.signing.data.KeyPairData;
import com.stericsson.sdk.signing.util.PEMParser;

/**
 * Creates RSA key pairs.
 * 
 * @author TSIKOR01
 * 
 */
public class KeyPairCreator {

    /**
     * Class constructor.
     */
    public KeyPairCreator() {

    }

    /**
     * The standard string name of the RSA algorithm.
     */
    private static final String RSA = "RSA";

    /**
     * Creates RSA key pair based on provided key pair data.
     * 
     * @param keyPairData
     *            Key pair data.
     * @throws IOException
     *             Exception is thrown when the output file cannot be created or opened for writing.
     * @throws NoSuchAlgorithmException
     *             Exception is thrown when there is not a provider for the RSA algorithm.
     */
    public void create(KeyPairData keyPairData) throws IOException, NoSuchAlgorithmException {
        if (keyPairData == null) {
            throw new NullPointerException();
        }

        KeyPairGenerator keyGen = KeyPairGenerator.getInstance(RSA);
        keyGen.initialize(keyPairData.getKeySize());
        KeyPair keyPair = keyGen.genKeyPair();
        PEMParser.writeRSAkey(new File(keyPairData.getPrivateKey()), keyPair.getPrivate());
        PEMParser.writeRSAkey(new File(keyPairData.getPublicKey()), keyPair.getPublic());
    }
}
