package com.stericsson.sdk.signing.generic;

import com.stericsson.sdk.signing.SignerException;
import com.stericsson.sdk.signing.generic.cert.GenericCertificate;
import com.stericsson.sdk.signing.generic.cert.GenericSignedHeader;
import com.stericsson.sdk.signing.signerservice.ISignPackage;

/**
 * Utility class to avoid checkstyle errors in U5500Signer
 * 
 * @author xolabju
 * 
 */
public final class GenericCertificateFactory {

    private GenericCertificateFactory() {
    }

    /**
     * 
     * @param signPackage
     *            the sign package to use
     * @param payload
     *            the payload to sign
     * @return a certificate instance
     * @throws SignerException
     *             on errors
     */
    public static GenericCertificate getCertificate(GenericSignPackage signPackage, IGenericSignablePayload payload)
        throws SignerException {
        GenericCertificate certificate;
        if (signPackage.getType().equalsIgnoreCase(ISignPackage.PACKAGE_TYPE_U5500_AUTH) 
                || signPackage.getType().equalsIgnoreCase(ISignPackage.PACKAGE_TYPE_EMPTY_GENERIC)) {
            certificate = new GenericSignedHeader();
        } else if (payload instanceof GenericSignedAuthenticationCertificate) {
            certificate = ((GenericSignedAuthenticationCertificate) payload).getCertificate();
        } else if (payload instanceof GenericSignedDebugAndTestCertificate) {
            certificate = ((GenericSignedDebugAndTestCertificate) payload).getCertificate();
        } else {
            certificate = signPackage.getCertificate();
        }
        return certificate;
    }
}
