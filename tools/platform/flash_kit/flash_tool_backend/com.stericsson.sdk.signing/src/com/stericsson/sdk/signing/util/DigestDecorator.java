package com.stericsson.sdk.signing.util;

import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.HashMap;

/**
 * Added support for SHA-256
 * 
 * @author etomjun
 * @author ecspasc
 */
public class DigestDecorator {

    /** Supported hash algorithm */
    public static final int SHA_1 = 1;

    /** Supported hash algorithm */
    public static final int SHA_256 = 2;

    /** Supported hash algorithm */
    public static final int SHA_384 = 3;

    /** Supported hash algorithm */
    public static final int SHA_512 = 4;

    private static HashMap<Integer, Integer> hashSizes = new HashMap<Integer, Integer>();
    static {
        hashSizes.put(SHA_1, 20);
        hashSizes.put(SHA_256, 32);
        hashSizes.put(SHA_384, 48);
        hashSizes.put(SHA_512, 64);
    }

    /** Message digest instance */
    private MessageDigest messageDigest;

    /**
     * Constructor. Creates a new message digest with specified algorithm.
     * 
     * @param algorithm
     *            Algorithm to use
     */
    public DigestDecorator(final int algorithm) {

        try {
            if (algorithm == SHA_1) {
                messageDigest = MessageDigest.getInstance("SHA-1");
            } else if (algorithm == SHA_256) {
                messageDigest = MessageDigest.getInstance("SHA-256");
            } else if (algorithm == SHA_384) {
                messageDigest = MessageDigest.getInstance("SHA-384");
            } else if (algorithm == SHA_512) {
                messageDigest = MessageDigest.getInstance("SHA-512");
            }
        } catch (NoSuchAlgorithmException e) {
            messageDigest = null;
            // Ignore this error and let program throw null pointer
            // exception if algorithm was not found
        }
    }

    /**
     * Creates the digest. It operates on a copy of the message digest instance so the original
     * message digest instance will not be reset with this operation.
     * 
     * @return The result of the digest operation or null if failed.
     */
    public byte[] digest() {

        byte[] result = null;

        try {
            MessageDigest tempMessageDigest = (MessageDigest) messageDigest.clone();
            result = tempMessageDigest.digest();
        } catch (CloneNotSupportedException e) {
            result = null;
            // Ignore this error and let program throw null pointer
            // exception when trying to access result.
        }

        return result;
    }

    /**
     * @param data
     *            TBD
     */
    public void update(final byte[] data) {
        messageDigest.update(data);
    }

    /**
     * @param data
     *            TBD
     * @param offset
     *            TBD
     * @param length
     *            TBD
     */
    public void update(final byte[] data, final int offset, final int length) {
        messageDigest.update(data, offset, length);
    }

    /**
     * 
     * @param algorithm
     *            the hash algorithm
     * @return the size of the resulting hash, or -1 if the algorithm is not supported
     */
    public static int getHashSize(int algorithm) {
        int size = -1;
        if (hashSizes.containsKey(algorithm)) {
            size = hashSizes.get(algorithm);
        }
        return size;
    }
}
