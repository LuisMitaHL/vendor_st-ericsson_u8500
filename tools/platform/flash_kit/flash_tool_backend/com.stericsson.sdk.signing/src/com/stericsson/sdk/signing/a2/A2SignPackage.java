package com.stericsson.sdk.signing.a2;

import com.stericsson.sdk.signing.cops.COPSCertificateList;
import com.stericsson.sdk.signing.signerservice.ISignPackage;

/**
 * A2 sign package implementation of ISignPackage
 * 
 * @author xtomlju
 */
public class A2SignPackage implements ISignPackage {

    private COPSCertificateList copsCertificateList;

    private int copsVersion;

    private int copsChipClass;

    private String alias;

    private String type;

    /**
     * @param pkgAlias
     *            alias
     * 
     * @param pkgType
     *            type
     * @param version
     *            Sign package header version
     * @param chipClass
     *            Chip class value
     * @param certificateList
     *            Certificate list instance
     */
    public A2SignPackage(String pkgAlias, String pkgType, int version, int chipClass,
        COPSCertificateList certificateList) {
        alias = pkgAlias;
        type = pkgType;
        copsVersion = version;
        copsChipClass = chipClass;
        copsCertificateList = certificateList;
    }

    /**
     * @return Sign package version
     */
    public int getVersion() {
        return copsVersion;
    }

    /**
     * @return Chip class specified in sign package
     */
    public int getChipClass() {
        return copsChipClass;
    }

    /**
     * @return TBD
     */
    public COPSCertificateList getCertificateList() {
        return copsCertificateList;
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
