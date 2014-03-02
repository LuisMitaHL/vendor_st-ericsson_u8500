/**
 * Copyright ST-Ericsson 2010. All rights reserved.
 */
package com.stericsson.RefMan.UmlExportFilter;

import java.security.InvalidParameterException;
import java.util.Collection;
import java.util.List;
import java.util.Vector;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/**
 * The <code>Interface</code> represents the the meta-data about an interface,
 * as exported from the UML model into the UML export XML file.
 *
 * @author Fredrik Lundström
 *
 */
public class Interface {

    /** The logger */
    final static Logger logger = LoggerFactory.getLogger(Interface.class);

    /** Will be shown in the Reference Manual. */
    private String name;

    /** Might be shown in the Reference Manual in the future. */
    private String classification;

    /** Might be shown in the Reference Manual in the future. */
    private String packageName;

    /** Stack - category name from UML model. */
    private String stack;

    /**
     * The platforms using this Interface.
     */
    private Vector<Platform> platforms;

    /**
     * The Guid of this Interface.
     *
     */
    private String guid;

    /**
     * Constructor for the <code>Interface</code>.
     *
     */
    public Interface() {
        this.guid = "";
        this.name = "";
        this.packageName = "";
        this.platforms = new Vector<Platform>();
        this.classification = "";
        this.stack = "";
    }

    /**
     * See return description.
     *
     * @return The <code>String</code> that will be shown as the name of the
     *         interface in the Reference Manual .
     */
    public String getName() {
        return name;
    }

    /**
     * See parameter description.
     *
     * @param name
     *            The <code>String</code> that will be shown as the name of the
     *            interface in the Reference Manual
     */
    public void setName(String name) {
        if (name != null) {
            this.name = name;
        } else {
            throw new InvalidParameterException("name must not be null");
        }
    }

    /**
     * See return description.
     *
     * @return The <code>String</code> that will be used as the classification
     *         of the interface in the Reference Manual .
     */
    public String getClassification() {
        return classification;
    }

    /**
     * See parameter description.
     *
     * @param classification
     *            Must be one of [Proprietary, Standard, ProprietaryExtension].
     */
    public void setClassification(String classification) {
        if (classification != null
                && (classification.compareTo("Proprietary") == 0
                        || classification.compareTo("Standard") == 0 || classification
                        .compareTo("ProprietaryExtension") == 0)) {
            this.classification = classification;
        } else {
            throw new InvalidParameterException(
                    "classification must be one of [Proprietary, Standard, ProprietaryExtension].");

        }

    }

    /**
     * See return description.
     *
     * @return True if the classification of this interface is Proprietary.
     */
    public boolean isProprietary() {
        return (classification.compareTo("Proprietary") == 0);
    }

    /**
     * See return description.
     *
     * @return True if the classification of this interface is Standard.
     */
    public boolean isStandard() {
        return (classification.compareTo("Standard") == 0);
    }

    /**
     * See return description.
     *
     * @return True if the classification of this interface is
     *         ProprietaryExtension.
     */
    public boolean isProprietaryExtension() {
        return (classification.compareTo("ProprietaryExtension") == 0);
    }

    /**
     * See return description.
     *
     * @return The <code>String</code> that will be used as the package name in
     *         the Reference Manual.
     */
    public String getPackage() {
        return packageName;
    }

    /**
     * See parameter description.
     *
     * @param packageName
     *            The <code>String</code> that will be used as the package name
     *            in the Reference Manual.
     */
    public void setPackage(String packageName) {
        if (packageName != null) {
            this.packageName = packageName;
        } else {
            throw new InvalidParameterException("packageName must not be null");
        }
    }

    /**
     * See return description.
     *
     * @return The <code>String</code> that will be used as the guid in the
     *         Reference Manual. The guid is the one exported from the UML model
     *         for the interface.
     */
    public String getGuid() {
        return guid;
    }

    /**
     * See parameter description.
     *
     * @param guid
     *            The <code>String</code> that will be used as the guid in the
     *            Reference Manual. The guid is the one exported from the UML
     *            model for the interface.
     */
    public void setGuid(String guid) {
        if (guid != null) {
            this.guid = guid;
        } else {
            throw new InvalidParameterException("guid must not be null");
        }
    }

    /**
     * See return description.
     *
     * @return A <code>List</code> containing the <code>Platform</code>'s
     *         implementing this <code>Interface</code>.
     */
    public List<Platform> getPlatforms() {
        return new Vector<Platform>(platforms);
    }

    /**
     * See parameter description.
     *
     * @param platforms
     *            Sets the list of <code>Platform</code>'s implementing this
     *            interface to the ones provided by the <code>Collection</code>.
     */
    public void setPlatforms(Collection<Platform> platforms) {
        this.platforms = new Vector<Platform>(platforms);
    }

    /**
     * Adds a platform to this <code>Interface</code>'s set of platforms
     * implementing this interface.
     *
     * @param platform
     *            <code>Platform</code> to be appended to this
     *            <code>Interface</code>'s set of platforms.
     * @return {@code true} (as specified by {@link Collection#add})
     */
    public boolean addPlatform(Platform platform) {
        return platforms.add(platform);
    }

    /**
     * Adds platforms to the <code>Interface</code>'s set of platforms.
     *
     * @param platformset
     *            <code>Platforms</code> to be appended to this
     *            <code>Interface</code>'s set of platforms.
     * @return {@code true} (as specified by {@link Collection#addAll})
     */
    public boolean addPlatforms(Collection<Platform> platformset) {
        return this.platforms.addAll(platformset);
    }

    /**
     * Calculates the hashCode of this <code>Interface</code>.
     *
     * @return A hashCode value for this <code>Interface</code>.
     */
    @Override
    public int hashCode() {
        return toString().hashCode();
    }

    /**
     * See return description.
     *
     * @return A <code>String</code> containing information about the
     *         <code>Interface</code>.
     */
    @Override
    public String toString() {
        return "name: " + name + "; package: " + packageName + "; guid: "
                + guid + "; classification: " + classification + "platforms: "
                + platforms.size();

    }

    /**
     * Returns the category name from the UML model.
     *
     * @return stack - category name from UML model.
     */
    public String getStack() {
        return stack;
    }

    /**
     * Sets the category name from the UML model.
     *
     * @param stackName
     *            The category name from the UML model.
     */
    public void setStack(String stackName) {
        if (stack == null) {
            stack = "";
        } else {
            stack = stackName;
        }
    }

}
