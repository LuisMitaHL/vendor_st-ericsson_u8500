package com.stericsson.sdk.signing.data;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

import com.stericsson.sdk.signing.generic.GenericSoftwareType;

/**
 * Represents data of an ISSW certificate. Used for ISSW certificate creation.
 * 
 * @author TSIKOR01
 * 
 */
public class ISSWCertificateData {

    /**
     * ISSW certificate type.
     */
    private ISSWCertificateType type;

    /**
     * Root key data.
     */
    private KeyData rootKey;

    /**
     * Key data collection.
     */
    private List<KeyData> keys;

    /**
     * ISSW certificate file path.
     */
    private String path;

    /**
     * Collection of model IDs.
     */
    private List<Short> modelID;

    /**
     * Hash map of signing constraints where software type is mapped to an index of a key data
     * collection element.
     */
    private HashMap<GenericSoftwareType, Integer> signingConstraints;

    /**
     * ISSW certificate reserved bytes.
     */
    private byte[] reserved;

    /**
     * Class constructor.
     * 
     * @param pPath
     *            ISSW certificate file path.
     * @param pType
     *            ISSW certificate type.
     * @param pRootKey
     *            Root key data.
     * @param pModelID
     *            Collection of model IDs.
     * @param pKeys
     *            Key data collection.
     * @param pSigningConstrains
     *            Hash map of signing constraints where software type is mapped to an index of a key
     *            data collection element.
     */

    public ISSWCertificateData(String pPath, ISSWCertificateType pType, KeyData pRootKey, List<Short> pModelID,
        List<KeyData> pKeys, HashMap<GenericSoftwareType, Integer> pSigningConstrains) {
        type = pType;
        rootKey = pRootKey;
        keys = pKeys;
        path = pPath;
        modelID = pModelID;
        this.signingConstraints = pSigningConstrains;
    }

    /**
     * @return ISSW certificate type.
     */
    public ISSWCertificateType getType() {
        return type;
    }

    /**
     * @param value
     *            ISSW certificate type.
     */
    public void setType(ISSWCertificateType value) {
        this.type = value;
    }

    /**
     * @return Root key data.
     */
    public KeyData getRootKey() {
        return rootKey;
    }

    /**
     * @param value
     *            Root key data.
     */
    public void setRootKey(KeyData value) {
        this.rootKey = value;
    }

    /**
     * @return Key data collection.
     */
    public List<KeyData> getKey() {
        return keys;
    }

    /**
     * @param value
     *            Key data collection.
     */
    public void setKey(List<KeyData> value) {
        this.keys = value;
    }

    /**
     * @return ISSW certificate file path.
     */
    public String getPath() {
        return path;
    }

    /**
     * @param value
     *            ISSW certificate file path.
     */
    public void setPath(String value) {
        this.path = value;
    }

    /**
     * @return Collection of model IDs.
     */
    public List<Short> getModelID() {
        if (modelID == null) {
            modelID = new ArrayList<Short>();
        }
        return this.modelID;
    }

    /**
     * @param pModelID
     *            Collection of model IDs.
     */
    public void setModelID(List<Short> pModelID) {
        this.modelID = pModelID;
    }

    /**
     * @return Hash map of signing constraints where software type is mapped to an index of a key
     *         data collection element.
     */
    public HashMap<GenericSoftwareType, Integer> getSigningConstraints() {
        if (signingConstraints == null) {
            signingConstraints = new HashMap<GenericSoftwareType, Integer>();
        }
        return signingConstraints;
    }

    /**
     * @param pSigningConstraints
     *            Hash map of signing constraints where software type is mapped to an index of a key
     *            data collection element.
     */
    public void setSigningConstraints(HashMap<GenericSoftwareType, Integer> pSigningConstraints) {
        this.signingConstraints = pSigningConstraints;
    }

    /**
     * @return ISSW certificate reserved bytes.
     */
    public byte[] getReserved() {
        if (reserved == null) {
            reserved = new byte[0];
        }
        return reserved;
    }

    /**
     * @param pReserved
     *            ISSW certificate reserved bytes.
     */
    public void setReserved(byte[] pReserved) {
        this.reserved = pReserved;
    }

}
