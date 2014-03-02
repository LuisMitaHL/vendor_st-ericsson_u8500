package com.stericsson.sdk.assembling.internal.u8500;

/**
 * U8500ConfigurationEntryAttribute class stores information about a configuration entry attribute
 * (name, value).
 * 
 * @author xtomlju
 */
public class U8500ConfigurationEntryAttribute {
    /** Name of attribute */
    private String name;

    /** Value of attribute */
    private String value;

    /**
     * @param n
     *            Attribute name
     */
    public U8500ConfigurationEntryAttribute(final String n) {
        name = n;
    }

    /**
     * @param n
     *            Attribute name
     * @param v
     *            Attribute value
     */
    public U8500ConfigurationEntryAttribute(final String n, final String v) {
        name = n;
        value = v;
    }

    /**
     * @param v
     *            Attribute value
     */
    public void setValue(final String v) {
        value = v;
    }

    /**
     * @return Attribute name
     */
    public String getName() {
        return name;
    }

    /**
     * @return Attribute value
     */
    public String getValue() {
        return value;
    }
}
