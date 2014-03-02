package com.stericsson.sdk.signing.cops;

/**
 * Payload interface that also can populate a COPSHashList with data.
 * 
 * @author xolabju
 */
public interface ICOPSPayloadWithHashList extends ICOPSPayload {

    /**
     * Returns the hash list digested with the specified hash algorithm
     * 
     * @see DigestDecorator#SHA_1
     * @see DigestDecorator#SHA_256
     * @param hashList
     *            COPS hash list to populate
     * @param hashAlgorithm
     *            The hash algorithm (i.e. SHA-1 or SHA-256)
     */
    void populateHashList(COPSHashList hashList, int hashAlgorithm);
}
