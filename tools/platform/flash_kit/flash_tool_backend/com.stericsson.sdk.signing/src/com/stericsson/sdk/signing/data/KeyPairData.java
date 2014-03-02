package com.stericsson.sdk.signing.data;

/**
 * Represents data of a one RSA key pair. Used for RSA key pair creation.
 * 
 * @author TSIKOR01
 * 
 */
public class KeyPairData {

    /**
     * Private key file path.
     */
    private String privateKey;

    /**
     * Public key file path.
     */
    private String publicKey;

    /**
     * Key size in bits.
     */
    private int keySize;

    /**
     * Class constructor.
     * 
     * @param pPrivateKey
     *            Private key file path.
     * @param pPublicKey
     *            Public key file path.
     * @param pKeySize
     *            Key size in bits.
     */
    public KeyPairData(String pPrivateKey, String pPublicKey, int pKeySize) {
        this.privateKey = pPrivateKey;
        this.publicKey = pPublicKey;
        this.keySize = pKeySize;
    }

    /**
     * @return Private key file path.
     */
    public String getPrivateKey() {
        return privateKey;
    }

    /**
     * @param value
     *            Private key file path.
     */
    public void setPrivateKey(String value) {
        this.privateKey = value;
    }

    /**
     * @return Public key file path.
     */
    public String getPublicKey() {
        return publicKey;
    }

    /**
     * @param value
     *            Public key file path.
     */
    public void setPublicKey(String value) {
        this.publicKey = value;
    }

    /**
     * @return Key size in bits.
     */
    public int getKeySize() {
        return keySize;
    }

    /**
     * @param value
     *            Key size in bits.
     */
    public void setKeySize(int value) {
        this.keySize = value;
    }

}
