package com.stericsson.sdk.signing.signerservice.local.keys;

import java.security.interfaces.RSAPrivateKey;

/**
 * Key to path class
 */
public class Key2Path implements Comparable<Object> {

    private byte[] mHeaderPackage;

    private RSAPrivateKey mKey;

    private String mSecretPath;

    private String mAlias;

    private String keyPath;

    /**
     * Constructor
     * 
     * @param alias
     *            alias
     * @param key
     *            key
     * @param secret
     *            secret
     * @param pack
     *            pack
     * @param pKeyPath keyPath
     */
    public Key2Path(String alias, RSAPrivateKey key, String secret, byte[] pack, String pKeyPath) {
        mAlias = alias;
        mKey = key;
        mSecretPath = secret;
        mHeaderPackage = pack;
        keyPath = pKeyPath;
    }

    /**
     * Get alias
     * 
     * @return alias
     */
    public String getAlias() {
        return mAlias;
    }

    /**
     * Get header package
     * 
     * @return header package
     */
    public byte[] getHeaderPackage() {
        return mHeaderPackage;
    }

    /**
     * Get key
     * 
     * @return key
     */
    public RSAPrivateKey getKey() {
        return mKey;
    }

    /**
     * Get secret path
     * 
     * @return secret path
     */
    public String getSecretPath() {
        return mSecretPath;
    }

    /**
     * Compare this Key2Path object with another object
     * 
     * @see java.lang.Comparable#compareTo(java.lang.Object)
     * @param o
     *            object to compare
     * @return 0 if equal
     */
    public int compareTo(Object o) {
        if (o instanceof Key2Path) {
            return mAlias.compareTo(((Key2Path) o).getAlias());
        }
        return -1;
    }

    /**
     * Compare this Key2Path object with another object
     * 
     * @see java.lang.Comparable#equals(java.lang.Object)
     * @param o
     *            object to compare
     * @return <code>true</code> if equal
     */
    public boolean equals(Object o) {
        if (o instanceof Key2Path) {
            return this.getAlias().equals(((Key2Path) o).getAlias());
        }
        return false;
    }

    /**
     * Calculate the hashcode of this object, based only on the alias string.
     * 
     * @return the hashcode for the Key2Path object
     */
    public int hashCode() {
        return this.getAlias().hashCode();
    }

    /**
     * @return key path
     */
    public String getKeyPath() {
        return keyPath;
    }
}
