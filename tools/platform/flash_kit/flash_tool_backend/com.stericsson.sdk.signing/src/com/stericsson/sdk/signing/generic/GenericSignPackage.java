package com.stericsson.sdk.signing.generic;

import com.stericsson.sdk.signing.generic.cert.GenericCertificate;
import com.stericsson.sdk.signing.signerservice.ISignPackage;

/**
 * U5500 sign package implementation of ISignPackage
 * 
 * @author xolabju
 */
public class GenericSignPackage implements ISignPackage {

    private GenericCertificate certificate;

    private String alias;

    private String type;

    private String parent;

    /**
     * 
     * @param pkgAlias
     *            alias
     * @param pkgType
     *            type
     * @param pkgParent
     *            parent package
     * @param cert
     *            certificate
     */
    public GenericSignPackage(String pkgAlias, String pkgParent, String pkgType, GenericCertificate cert) {
        alias = pkgAlias;
        type = pkgType;
        certificate = cert;
        parent = pkgParent;
    }

    /**
     * 
     * @return the certificate
     */
    public GenericCertificate getCertificate() {
        return certificate;
    }

    /**
     * {@inheritDoc}
     */
    public String getAlias() {
        return alias;
    }

    /**
     * {@inheritDoc}
     */
    public String getType() {
        return type;
    }

    /**
     * 
     * @return parent package
     */
    public String getParent() {
        return parent;
    }

    /**
     * {@inheritDoc}
     */
    public String getDescription() {
        // TODO Auto-generated method stub
        return null;
    }

    /**
     * {@inheritDoc}
     */
    public String getRevision() {
        // TODO Auto-generated method stub
        return null;
    }

}
