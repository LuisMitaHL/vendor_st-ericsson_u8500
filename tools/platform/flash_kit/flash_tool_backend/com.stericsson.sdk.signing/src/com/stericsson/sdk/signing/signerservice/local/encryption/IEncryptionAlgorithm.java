package com.stericsson.sdk.signing.signerservice.local.encryption;

/**
 * @author qdaneke Implement this interface and inherit EncryptionAlgorithm to be able to use the
 *         methods needed in PA signing component to be able to encrypt payloads.
 */
public interface IEncryptionAlgorithm {

    /**
     * The method to be executed when encrypting an 8-byte block of code.
     * 
     * @param src1
     *            The first four bytes to be encrypted.
     * @param src2
     *            The second four bytes to be encrypted.
     * @param key
     *            The key to encrypt with.
     */
    void encrypt(byte[] src1, byte[] src2, int[] key);

    /**
     * The method to be executed when decrypting an 8-byte block of code.
     * 
     * @param src1
     *            The first four bytes to be decrypted.
     * @param src2
     *            The second four bytes to be decrypted.
     * @param key
     *            The key to decrypt with.
     */
    void decrypt(byte[] src1, byte[] src2, int[] key);
}
