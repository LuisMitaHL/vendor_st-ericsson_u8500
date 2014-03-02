package com.stericsson.sdk.signing.signerservice.local.encryption;

import java.math.BigInteger;
import java.nio.ByteBuffer;
import java.security.interfaces.RSAPrivateKey;

/**
 * 
 * @author xolabju
 * 
 */
public class RSAPKCS1Encrypter {

    /**
     * Constructor
     */
    public RSAPKCS1Encrypter() {

    }

    /**
     * Returns the signature based on hash, the rest of DigestInfo is inserted here. DigestInfo:
     * These 15 bytes: {0x30,0x21,0x30,0x09,0x06,0x05,0x2b,0x0e,0x03,0x02,0x1a,0x05,0x00,0x04,0x14}
     * followed by the 20 bytes that is the SHA-1 hash.
     * 
     * @param hash
     *            - The 20 bytes that is the hash of the message
     * @param key
     *            - The private key that is to be used to encrypt
     * @return A byte array that is the signature
     */
    public byte[] calculateRSAPKCS1Signature(byte[] hash, RSAPrivateKey key) {
        byte[] signature = null;
        byte[] di =
            new byte[] {
                (byte) 0x30, (byte) 0x21, (byte) 0x30, (byte) 0x09, (byte) 0x06, (byte) 0x05, (byte) 0x2b, (byte) 0x0e,
                (byte) 0x03, (byte) 0x02, (byte) 0x1a, (byte) 0x05, (byte) 0x00, (byte) 0x04, (byte) 0x14};

        BigInteger n = key.getModulus();
        BigInteger d = key.getPrivateExponent();

        ByteBuffer bb = ByteBuffer.allocate(di.length + hash.length);
        bb.rewind();
        bb.put(di);
        bb.put(hash);

        final byte[] t = bb.array();

        int k = n.bitLength() / 8;
        byte[] pad = new byte[k - t.length - 3];
        for (int i = 0; i < pad.length; i++) {
            pad[i] = (byte) 0xFF;
        }

        ByteBuffer input = ByteBuffer.allocate(3 + pad.length + t.length);
        input.rewind();
        input.put((byte) 0x00);
        input.put((byte) 0x01);
        input.put(pad);
        input.put((byte) 0x00);
        input.put(t);

        BigInteger m = new BigInteger(1, input.array());
        // Encrypt it using m^d mod n
        BigInteger s = m.modPow(d, n);

        byte[] temp = s.toByteArray(); // remove sign bit

        if (temp.length > k) { // BigInteger has added sign bit
            signature = new byte[temp.length - 1];
            System.arraycopy(temp, 1, signature, 0, signature.length);
        } else {
            signature = temp;
        }

        return signature;
    }
}
