package com.stericsson.sdk.signing.signerservice;

/**
 * @author xtomlju
 */
public interface ISignPackageInformation {



    /**
     * @return Sign package name
     */
    String getName();

    /**
     * @return A list of chip identifiers
     */
    String[] getChipIdentifiers();

    /**
     * @return the sign package parent, or null if there is no parent
     */
    String getParent();

    /**
     * @return the sign package type type
     */
    String getType();

    /**
     * @return True if encrypt should be required
     */
    boolean isEncryptRequired();
}
