package com.stericsson.sdk.signing.signerservice.local.encryption;

import java.math.BigInteger;
import java.nio.ByteBuffer;
import java.security.interfaces.RSAPrivateKey;

/**
 * 
 * @author xolabju
 * 
 */
public class A2Encrypter {

    /**
     * Constructor
     */
    public A2Encrypter() {

    }

    /**
     * Calculates an A2 signatures based on the data with the private key key.
     * 
     * @param data
     *            - The data to calculate signature over.
     * @param key
     *            - The private key used to calculate the signature.
     * @return The signature as a byte array. The length of the result may is not necessarily
     *         Modulus length, it can be shorter.
     */
    public byte[] calculateA2Signature(byte[] data, RSAPrivateKey key) {

        byte[] signature = null;
        BigInteger n = key.getModulus();
        BigInteger d = key.getPrivateExponent();

        int k = n.bitLength() / 8;
        byte[] pad = new byte[k - data.length - 3];
        for (int i = 0; i < pad.length; i++) {
            pad[i] = (byte) 0xFF;
        }

        ByteBuffer kData = ByteBuffer.allocate(3 + pad.length + data.length);
        kData.put((byte) 0x00);
        kData.put((byte) 0x01);
        kData.put(pad);
        kData.put((byte) 0x00);
        kData.put(data);

        BigInteger m = new BigInteger(1, kData.array());
        // Encrypt it using m^d mod n
        BigInteger s = m.modPow(d, n);

        ByteBuffer temp = ByteBuffer.wrap(s.toByteArray()); // remove sign bit

        if (temp.capacity() > k) { // BigInteger has added sign bit
            signature = new byte[temp.capacity() - 1];
            temp.position(1);
            temp.get(signature, 0, signature.length);
        } else {
            signature = temp.array();
        }

        return signature;
    }
}
