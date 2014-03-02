package com.stericsson.sdk.signing.data;

import java.util.ArrayList;
import java.util.List;

/**
 * Represents data of a one root sign package. Used for root sign package creation.
 * 
 * @author TSIKOR01
 * 
 */
public class RootSignPackageData {

    /**
     * Root sign package directory path.
     */
    private String path;

    /**
     * File path of an ISSW certificate.
     */
    private String isswCertificate;

    /**
     * Root sign package alias.
     */
    private String alias;

    /**
     * Root sign package revision.
     */
    private String revision;

    /**
     * Root sign package description.
     */
    private String description;

    /**
     * Subpackage data collection.
     */
    private List<SubpackageData> subpackages = null;

    /**
     * Class constructor.
     * 
     * @param pPath
     *            Root sign package directory path.
     * @param pISSWCertificate
     *            File path of an ISSW certificate.
     * @param pAlias
     *            Root sign package alias.
     * @param pRevision
     *            Root sign package revision.
     * @param pDescription
     *            Root sign package description.
     */
    public RootSignPackageData(String pPath, String pISSWCertificate, String pAlias, String pRevision,
        String pDescription) {
        this.path = pPath;
        this.alias = pAlias;
        this.revision = pRevision;
        this.description = pDescription;
        this.isswCertificate = pISSWCertificate;
    }

    /**
     * @return Root sign package directory path.
     */
    public String getPath() {
        return path;
    }

    /**
     * @param value
     *            Root sign package directory path.
     */
    public void setPath(String value) {
        this.path = value;
    }

    /**
     * @return File path of an ISSW certificate.
     */
    public String getIsswCertificate() {
        return isswCertificate;
    }

    /**
     * @param value
     *            File path of an ISSW certificate.
     */
    public void setIsswCertificate(String value) {
        this.isswCertificate = value;
    }

    /**
     * @return Root sign package alias.
     */
    public String getAlias() {
        return alias;
    }

    /**
     * @param value
     *            Root sign package alias.
     */
    public void setAlias(String value) {
        this.alias = value;
    }

    /**
     * @return Root sign package revision.
     */
    public String getRevision() {
        return revision;
    }

    /**
     * @param value
     *            Root sign package revision.
     */
    public void setRevision(String value) {
        this.revision = value;
    }

    /**
     * @return Root sign package description.
     */
    public String getDescription() {
        return description;
    }

    /**
     * @param value
     *            Root sign package description.
     */
    public void setDescription(String value) {
        this.description = value;
    }

    /**
     * @return Subpackage data collection.
     */
    public List<SubpackageData> getSubpackages() {
        if (subpackages == null) {
            subpackages = new ArrayList<SubpackageData>();
        }
        return subpackages;
    }

    /**
     * @param pSubpackages
     *            Subpackage data collection.
     */
    public void setSubpackages(List<SubpackageData> pSubpackages) {
        this.subpackages = pSubpackages;
    }
}
