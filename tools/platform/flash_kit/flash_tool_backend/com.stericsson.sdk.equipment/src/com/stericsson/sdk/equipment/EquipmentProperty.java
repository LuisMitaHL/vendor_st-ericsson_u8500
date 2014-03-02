/**
 * 
 */
package com.stericsson.sdk.equipment;

/**
 * @author xhelciz
 * 
 */
public class EquipmentProperty {

    private String propertyId;

    private String propertyName;

    private String propertyValue;

    private boolean isSecurity;

    private boolean isReadable;

    private boolean isWritable;

    /**
     * Constructor for equipment property settings. By default readable/writable is set to false.
     * This values apply only for security properties.
     * 
     * @param id
     *            property Id
     * @param name
     *            property name
     * @param value
     *            property value
     * @param security
     *            true if the property is a security property, false otherwise
     * @param read
     *            true if the security property is readable
     * @param write
     *            true if the security property is writable
     */
    public EquipmentProperty(String id, String name, String value, boolean security, boolean read, boolean write) {
        setPropertyId(id);
        this.propertyName = name;
        this.propertyValue = value;
        this.isSecurity = security;
        this.isReadable = read;
        this.isWritable = write;
    }

    /**
     * Constructor for equipment property settings where readable and writable is not specified. By
     * default readable/writable is set to false. This values apply only for security properties.
     * 
     * @param id
     *            property Id
     * @param name
     *            property name
     * @param value
     *            property value
     * @param security
     *            true if the property is a security property, false otherwise
     */
    public EquipmentProperty(String id, String name, String value, boolean security) {
        this(id, name, value, security, false, false);
    }

    /**
     * returns property Id
     * 
     * @return propertyId
     */
    public String getPropertyId() {
        return propertyId;
    }

    /**
     * sets property Id
     * 
     * @param pPropertyId
     *            id to be set
     */
    protected void setPropertyId(String pPropertyId) {
        String idDec;
        if (pPropertyId.startsWith("0x")) {
            idDec = pPropertyId.replace("0x", " ").trim();
            idDec = String.valueOf(Integer.parseInt(idDec, 16));
        } else {
            idDec = pPropertyId;
        }
        propertyId = idDec;
    }

    /**
     * returns property name
     * 
     * @return propertyName
     */
    public String getPropertyName() {
        return propertyName;
    }

    /**
     * sets property name
     * 
     * @param pPropertyName
     *            name of property
     */
    protected void setPropertyName(String pPropertyName) {
        propertyName = pPropertyName;
    }

    /**
     * returns property value
     * 
     * @return propertyValue
     */
    public String getPropertyValue() {
        return propertyValue;
    }

    /**
     * sets property value
     * 
     * @param pPropertyValue
     *            value of property
     */
    protected void setPropertyValue(String pPropertyValue) {
        propertyValue = pPropertyValue;
    }

    /**
     * returns whether property is a security property
     * 
     * @return isSecurity true if property is a security property
     */
    public boolean isSecurity() {
        return isSecurity;
    }

    /**
     * sets whether property is a security property
     * 
     * @param pIsSecurity
     *            true if property is a security property
     */
    protected void setSecurity(boolean pIsSecurity) {
        isSecurity = pIsSecurity;
    }

    /**
     * returns whether property is readable. Valid only for security property.
     * 
     * @return true if property is readable
     */
    public boolean isReadable() {
        return isReadable;
    }

    /**
     * sets whether property is readable. Applies only to security property. All other are by
     * default false
     * 
     * @param pIsReadable
     *            true if property is readable, false otherwise
     */
    protected void setReadable(boolean pIsReadable) {
        if (isSecurity) {
            isReadable = pIsReadable;
        } else {
            isReadable = false;
        }
    }

    /**
     * returns whether property is writable. Valid only for security property.
     * 
     * @return true if property is writable
     */
    public boolean isWritable() {
        return isWritable;
    }

    /**
     * sets whether property is writable. Applies only to security property. All other are by
     * default false
     * 
     * @param pIsWritable
     *            true if property is writable, false otherwise
     */
    protected void setWritable(boolean pIsWritable) {
        if (isSecurity) {
            isWritable = pIsWritable;
        } else {
            isWritable = false;
        }
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public String toString() {
        return propertyValue == null ? super.toString() : propertyValue;
    }

}
