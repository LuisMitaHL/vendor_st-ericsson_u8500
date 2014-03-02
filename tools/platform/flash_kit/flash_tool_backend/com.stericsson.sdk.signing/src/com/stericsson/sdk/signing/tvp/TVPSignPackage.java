package com.stericsson.sdk.signing.tvp;

import com.stericsson.sdk.signing.signerservice.ISignPackage;

/**
 * 
 * @author xolabju
 * 
 */
public class TVPSignPackage implements ISignPackage {

    private X509CertificateChain certificateChain;

    private int headerVersion;

    private String alias;

    private String type;

    /**
     * 
     * @param pkgAlias
     *            alias
     * 
     * @param pkgType
     *            type
     * @param version
     *            Sign package header version
     * @param certChain
     *            certificate chain
     */
    public TVPSignPackage(String pkgAlias, String pkgType, int version, X509CertificateChain certChain) {
        alias = pkgAlias;
        type = pkgType;
        setHeaderVersion(version);
        setCertificateChain(certChain);
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
     * @param certChain
     *            cert chain
     */
    public void setCertificateChain(X509CertificateChain certChain) {
        this.certificateChain = certChain;
    }

    /**
     * 
     * @return cert chain
     */
    public X509CertificateChain getCertificateChain() {
        return certificateChain;
    }

    /**
     * 
     * @param version
     *            header version
     */
    public void setHeaderVersion(int version) {
        this.headerVersion = version;
    }

    /**
     * 
     * @return header version
     */
    public int getHeaderVersion() {
        return headerVersion;
    }

    /**
     * {@inheritDoc}
     */
    public String getDescription() {
        return null;
    }

    /**
     * {@inheritDoc}
     */
    public String getParent() {
        return null;
    }

    /**
     * {@inheritDoc}
     */
    public String getRevision() {
        return null;
    }

}
