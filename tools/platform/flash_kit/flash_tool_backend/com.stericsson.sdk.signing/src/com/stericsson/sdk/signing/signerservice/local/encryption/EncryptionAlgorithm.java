package com.stericsson.sdk.signing.signerservice.local.encryption;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.IOException;

/**
 * Methods used for encryption loader
 * 
 */
public final class EncryptionAlgorithm {

    private EncryptionAlgorithm() {

    }

    /**
     * Transforms a byte array to an integer.
     * 
     * @param src
     *            the source to transform.
     * @return the transformed value.
     */
    public static int fromBytesToInt(byte[] src) {

        int toReturn = 0;

        toReturn |= ((0xff & (src[3])) << 24);
        toReturn |= ((0xff & (src[2])) << 16);
        toReturn |= ((0xff & (src[1])) << 8);
        toReturn |= ((0xff & (src[0])) << 0);

        return toReturn;
    }

    /**
     * Transforms an integer to a byte array.
     * 
     * @param src
     *            the source to transform.
     * @param toReturn
     *            a place -- the user allocate -- to save the value in.
     * @return the same byte array that the user sent as toReturn arg.
     */
    public static byte[] fromIntToBytes(int src, byte[] toReturn) {

        toReturn[3] = (byte) ((src >> 24) & 0xff);
        toReturn[2] = (byte) ((src >> 16) & 0xff);
        toReturn[1] = (byte) ((src >> 8) & 0xff);
        toReturn[0] = (byte) ((src >> 0) & 0xff);

        return toReturn;
    }

    /**
     * This method will encrypt the stream in stream and then the encrypted data will be written to
     * the ostream. It will encrypt with the key argument.
     * 
     * @param amountOfRounds
     *            iteration count
     * 
     * @param stream
     *            The data to be encrypted.
     * @param key
     *            The key to encrypt with.
     * @param ostream
     *            The place to write and save the encrypted data.
     * @throws IOException
     *             Exception rethrown from other exception, TODO maybe catch and then rethrow.
     */
    public static void encryptPayload(int amountOfRounds, ByteArrayInputStream stream, int[] key,
        ByteArrayOutputStream ostream) throws IOException {

        int amount = -1;

        byte[] payload64bits = {
            0, 0, 0, 0, 0, 0, 0, 0};

        while ((amount = stream.read(payload64bits, 0, 8)) != -1) {
            byte[] payload32bits1 = new byte[4];

            if (amount >= 1) {
                payload32bits1[0] = payload64bits[0];
            } else {
                payload32bits1[0] = 0;
            }
            if (amount >= 2) {
                payload32bits1[1] = payload64bits[1];
            } else {
                payload32bits1[1] = 0;
            }
            if (amount >= 3) {
                payload32bits1[2] = payload64bits[2];
            } else {
                payload32bits1[2] = 0;
            }
            if (amount >= 4) {
                payload32bits1[3] = payload64bits[3];
            } else {
                payload32bits1[3] = 0;
            }

            byte[] payload32bits2 = new byte[4];

            if (amount >= 5) {
                payload32bits2[0] = payload64bits[4];
            } else {
                payload32bits2[0] = 0;
            }
            if (amount >= 6) {
                payload32bits2[1] = payload64bits[5];
            } else {
                payload32bits2[1] = 0;
            }
            if (amount >= 7) {
                payload32bits2[2] = payload64bits[6];
            } else {
                payload32bits2[2] = 0;
            }
            if (amount >= 8) {
                payload32bits2[3] = payload64bits[7];
            } else {
                payload32bits2[3] = 0;
            }

            encrypt(amountOfRounds, payload32bits1, payload32bits2, key);

            byte[] encryptedData =
                {
                    payload32bits1[0], payload32bits1[1], payload32bits1[2], payload32bits1[3], payload32bits2[0],
                    payload32bits2[1], payload32bits2[2], payload32bits2[3]};

            ostream.write(encryptedData);
            for (int i = 0; i < 8; ++i) {
                payload64bits[i] = 0;
            }
        }
    }

    /**
     * @param amountOfRounds
     *            iteration count
     * @param data1
     *            Data to encrypt.
     * @param data2
     *            Data to encrypt.
     * @param key
     *            Key to encrypt with.
     */
    public static void encrypt(int amountOfRounds, byte[] data1, byte[] data2, int[] key) {
        long sum = 0;
        long delta = 0x9E3779B9;
        int v0 = 0;
        int v1 = 0;
        v0 = fromBytesToInt(data1);
        v1 = fromBytesToInt(data2);

        int i;
        for (i = 0; i < amountOfRounds; i++) {
            v0 += (((v1 << 4) ^ (v1 >>> 5)) + v1) ^ (sum + key[(int) (sum & 3)]);
            sum += delta;
            v1 += (((v0 << 4) ^ (v0 >>> 5)) + v0) ^ (sum + key[(int) (sum >>> 11 & 3)]);
        }

        // Store data back
        data1 = fromIntToBytes(v0, data1);
        data2 = fromIntToBytes(v1, data2);
    }
}
