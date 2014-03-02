/**
 * Copyright ST-Ericsson 2010. All rights reserved.
 */
package com.stericsson.RefMan.UmlExportFilter;

import java.security.InvalidParameterException;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/**
 * The <code>Platform</code> represents a platform using a specific interface,
 * as exported from the UML model into the UML export XML file.
 *
 * @author Fredrik Lundström
 *
 */
public class Platform {

    /** The logger */
    final static Logger logger = LoggerFactory.getLogger(Platform.class);

    /** Name of platform, as shown in the Reference Manual. */
    private String name;

    /**
     * "yes" or "no"
     */
    private String interface_internal_only;

    /**
     * Constructor for the <code>Platform</code>.
     */
    public Platform() {
        this.name = "";
        this.interface_internal_only = "";
    }

    /**
     * See return description.
     *
     * @return The name of the platform.
     */
    public String getName() {
        return name;
    }

    /**
     * See parameter description.
     *
     * @param name
     *            The name of the platform.
     */
    public void setName(String name) {
        if (name != null) {
            this.name = name;
        } else {
            throw new InvalidParameterException("non-null required");
        }
    }

    /**
     * See return description.
     *
     * @return "yes" or "no", depending on if the interface is for internal use
     *         only.
     */
    public String getInterfaceInternalOnly() {
        return interface_internal_only;
    }

    /**
     * See return description.
     *
     * @return true if the interface is for internal use only.
     */
    public boolean isInterfaceInternalOnly() {
        return (interface_internal_only.compareTo("yes") == 0);
    }

    /**
     * See parameter description.
     *
     * @param interfaceInternalOnly
     *            "yes" or "no", depending on if the interface is for internal
     *            use only.
     */
    public void setInterfaceInternalOnly(String interfaceInternalOnly) {
        if (interfaceInternalOnly != null
                && (interfaceInternalOnly.compareTo("yes") == 0 || interfaceInternalOnly
                        .compareTo("no") == 0)) {
            this.interface_internal_only = interfaceInternalOnly;
        } else {
            throw new InvalidParameterException(
                    "interfaceInternalOnly must be one of ['yes', 'no'].");
        }
    }

    /**
     * Compares an <code>Object</code> to this <code>Platform</code> to see if
     * they are equal.
     *
     * @param o
     *            The <code>Object</code> to be compared to this
     *            <code>Platform</code>
     * @return {@code true} if the <code>Object</code> and <code>Platform</code>
     *         are equal, otherwise <code>false</code>.
     */
    @Override
    public boolean equals(Object o) {
        if (o == this) {
            return true;
        }
        if (o instanceof Platform) {
            Platform p = (Platform) o;
            if (name.compareTo(p.getName()) == 0
                    && isInterfaceInternalOnly() == p.isInterfaceInternalOnly()) {
                return true;
            }
        }
        return false;
    }

    /**
     * Calculates the hashCode of this <code>Element</code>.
     *
     * @return A hashCode value for this <code>Element</code>.
     */
    @Override
    public int hashCode() {
        return toString().hashCode();
    }

    /**
     * See return description.
     *
     * @return A <code>String</code> containing information about the
     *         <code>Element</code>.
     */
    @Override
    public String toString() {
        return "name: " + name + "; interface_internal_only: "
                + interface_internal_only;
    }

}
