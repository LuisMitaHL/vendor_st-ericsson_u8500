package com.stericsson.sdk.signing.generic;

import com.stericsson.sdk.signing.IPayload;
import com.stericsson.sdk.signing.generic.cert.GenericCertificate;

/**
 * Interface for a U8500 signable payload
 * 
 * @author xolabju
 * 
 */
public interface IGenericSignablePayload extends IPayload {

    /**
     * Sets the signature
     * 
     * @param signature
     *            the signature data
     */
    void setSignature(byte[] signature);

    /**
     * Sets the certificate
     * 
     * @param certificate
     *            the certificate
     */
    void setCertificate(GenericCertificate certificate);

    /**
     * 
     * @return true if the input payload was built with a header, else false
     */
    boolean hasPreBuiltHeader();
}
