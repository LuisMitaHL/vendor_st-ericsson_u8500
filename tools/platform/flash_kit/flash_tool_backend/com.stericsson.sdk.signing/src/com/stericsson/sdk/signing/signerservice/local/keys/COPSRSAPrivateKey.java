package com.stericsson.sdk.signing.signerservice.local.keys;

import java.math.BigInteger;
import java.security.interfaces.RSAPrivateKey;

/**
 * Represents the private COPS security key.
 * 
 */
public class COPSRSAPrivateKey implements RSAPrivateKey {

    /**
     * FIXME added by Eclipse, version number may be invalid.
     */
    private static final long serialVersionUID = 1L;

    /**
     * This key's exponent.
     */
    private BigInteger mExponent;

    /**
     * This key's mod.
     */
    private BigInteger mModulus;

    /**
     * Create the Key with the specified mod/exp
     * 
     * @param modulus
     *            The mod to use for this key.
     * @param exponent
     *            The exp to use for this key.
     */
    public COPSRSAPrivateKey(BigInteger modulus, BigInteger exponent) {
        mModulus = modulus;
        mExponent = exponent;
    }

    /**
     * Creates a new key.
     */
    public COPSRSAPrivateKey() {
    }

    /**
     * Returns the name of the crypto algorithm used.
     * 
     * @return The algo name.
     */
    public String getAlgorithm() {
        return "RSA";
    }

    /**
     * NOTE: Not used, returns null always.
     * 
     * @return NULL!
     */
    public byte[] getEncoded() {
        return null;
    }

    /**
     * Returns format of the key. NOTE: Not used!
     * 
     * @return ???
     */
    public String getFormat() {
        return "???";
    }

    /**
     * Returns the mod. for this key.
     * 
     * @return The mod.
     */
    public BigInteger getModulus() {
        return mModulus;
    }

    /**
     * Returns the exponent for this key.
     * 
     * @return The exponent.
     */
    public BigInteger getPrivateExponent() {
        return mExponent;
    }

    /**
     * Sets the mod. that this key should use.
     * 
     * @param mod
     *            The mod.
     */
    public void setModulus(final BigInteger mod) {
        mModulus = mod;
    }

    /**
     * Sets the exponent that this key should use.
     * 
     * @param exp
     *            The exponent that this key should use.
     */
    public void setPrivateExponent(BigInteger exp) {
        mExponent = exp;
    }
}
