/**
 * Copyright ST-Ericsson 2010. All rights reserved.
 */
package com.stericsson.RefMan.UmlExportFilter;

import java.util.Collection;
import java.util.List;
import java.util.Vector;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.stericsson.RefMan.Toc.Element;
import com.stericsson.RefMan.Toc.TocElement;

/**
 * The <code>UmlExport</code> represents the part of the UML model exported from
 * the UML model into the UML export XML file.
 *
 * @author Fredrik Lundström
 *
 */
public class UmlExport {

    /** The logger */
    private final static Logger logger = LoggerFactory
            .getLogger(UmlExport.class);

    /**
     * Contains all interfaces.
     */
    private final Vector<Interface> interfaces;

    /**
     * A list of interface names that are documented in the UML Model, but not
     * found in the API Reference Manual.
     */
    private List<String> undocumentedInRefMan;

    /**
     * A list of interface names that are documented in the API Reference
     * Manual, but not found in the UML Model .
     */
    private List<String> undocumentedInModel;

    /**
     * A list of interface names that are documented in the API Reference
     * Manual, but shall be hidden according to the UML Model.
     */
    private List<String> hideInRefMan;

    /**
     * The unfiltered API Reference Manual as discovered by the tool.
     */
    private TocElement api;

    /**
     * The name of the platfrom for which the API Reference Manual has been
     * generated.
     */
    private String platformName;

    /**
     * Constructor for the <code>UmlExport</code>.
     *
     * @param api
     *            A TocElement representing the API Reference Manual
     * @param platformName
     *            The name of the platform for which the API Reference Manual
     *            shall be generated.
     */
    public UmlExport(TocElement api, String platformName) {
        this.interfaces = new Vector<Interface>();
        this.api = api;
        this.platformName = platformName;
    }

    /**
     * Constructor for the <code>UmlExport</code>. For use when testing error
     * checking.
     */
    UmlExport() {
        this.interfaces = new Vector<Interface>();
    }

    /**
     * See return description.
     *
     * @return A <code>String</code> containing information about the
     *         <code>UmlExport</code>.
     */
    @Override
    public String toString() {
        return interfaces.toString();
    }

    /**
     * Adds an interface to the set of interfaces.
     *
     * @param i
     *            <code>Interface</code> to be appended to the set of
     *            interfaces.
     * @return {@code true} (as specified by {@link Collection#add})
     */
    public boolean addInterface(Interface i) {
        return interfaces.add(i);
    }

    /**
     * See return description.
     *
     * @return A <code>List</code> containing the <code>Interface</code>.
     */
    public List<Interface> getInterfaces() {
        return new Vector<Interface>(interfaces);
    }

    /**
     * Checks that the interfaces specified by the reference manual match those
     * read in the UML export. Logs a WARN message for every mismatch, and
     * returns the number of mismatches. Get the lists of mismatching interfaces
     * by calling getUndocumentedInRefMan() or getUndocumentedInModel(). Get the
     * list of interfaces that shall be hidden in the API Reference Manual by
     * calling getHiddenInRefMan().
     *
     * @return Number of faults detected.
     */
    public int checkInterfaces() {
        int faults = 0;

        undocumentedInModel = new Vector<String>();
        undocumentedInRefMan = new Vector<String>();
        hideInRefMan = new Vector<String>();

        // First: Check all interfaces in the UML model,
        // is documented?
        for (Interface i : interfaces) {
            String name = i.getName();
            List<Platform> platforms = i.getPlatforms();
            boolean found = false;
            boolean isInternalOnly = false;

            for (Platform p : platforms) {
                if (p.getName().compareToIgnoreCase(platformName) == 0) {
                    found = true;
                    if (p.isInterfaceInternalOnly()) {
                        isInternalOnly = true;
                    }
                    break;
                }
            }
            if (found == false) {
                // This interface is not used in this platform at all, according
                // to UML model: Ok so far
                Element foundElement = api.getElement(name);
                if (foundElement == null) {
                    logger
                            .debug(
                                    "Interface '{}' found in UML model but OK even though it is not documented in Reference Manual, since it is not used in this platform",
                                    name);
                } else {
                    logger
                            .debug(
                                    "Interface '{}' found both in UML model and Reference Manual, but not used in this platform",
                                    name);
                    hideInRefMan.add(name);
                }
            } else {
                Element foundElement = api.getElement(name);
                if (foundElement == null) {
                    if (isInternalOnly) {
                        logger
                                .debug(
                                        "Interface '{}' found in UML model but OK even though it is NOT documented in Reference Manual, since is documented as INTERNAL ONLY in UML model",
                                        name);
                    } else {
                        logger
                                .warn(
                                        "Interface '{}' is NOT documented in Reference Manual, but is documented in UML model",
                                        name);
                        undocumentedInRefMan.add(name);
                        faults++;
                    }
                } else if (isInternalOnly) {
                    logger
                            .debug(
                                    "Interface '{}' found both in Reference Manual and UML model BUT is documented as INTERNAL ONLY in this platform",
                                    name);
                    hideInRefMan.add(name);
                } else {

                    logger
                            .debug(
                                    "Interface '{}' OK found both in Reference Manual and UML model",
                                    name);
                }
            }
        }

        // Now check the manual, does them exist in the UML model?
        for (Element e : api.getInterfaces()) {
            String name = e.getLabel();
            boolean found = false;
            boolean notUsedInThisPlatform = false;
            for (Interface i : interfaces) {
                if (i.getName().compareToIgnoreCase(name) == 0) {
                    // found
                    List<Platform> platforms = i.getPlatforms();
                    for (Platform p : platforms) {
                        if (p.getName().compareToIgnoreCase(platformName) == 0) {
                            found = true;
                            // We don't care about the setting of the UML model
                            // at this stage, since it is taken care of above.
                            break;
                        }
                    }

                    if (found) {
                        break;
                    } else {
                        notUsedInThisPlatform = true;
                    }
                }
            }
            if (found) {
                // Ok, we should already have detected this case.
                logger
                        .debug(
                                "Interface '{}' OK found both in Reference Manual and UML model (2nd attempt)",
                                name);
            } else if (notUsedInThisPlatform) {
                // Ok, we should already have detected this case.
                logger
                        .debug(
                                "Interface '{}' found both in Reference Manual and UML model BUT but is not used in this platform (2nd attempt)",
                                name);
            } else {

                logger
                        .warn(
                                "Interface '{}' is NOT documented in UML model, but is documented in Reference Manual",
                                name);
                undocumentedInModel.add(name);
                faults++;
            }
        }

        if (faults > 0) {
            logger.debug("Total number of mismatches: {}", faults);
        } else {
            logger.debug("No mismatches detected");
        }

        return faults;
    }

    /**
     * Returns a list of names indicating which interfaces that are documented
     * in the UML model, but not documented in the Reference Manual.
     *
     * Call checkInterfaces() prior to calling this method.
     *
     * @return A list of interface names
     */
    List<String> getUndocumentedInRefMan() {
        return new Vector<String>(undocumentedInRefMan);
    }

    /**
     * Returns a list of names indicating which interfaces that are not
     * documented in the UML model, but are documented in the Reference Manual.
     *
     * Call checkInterfaces() prior to calling this method.
     *
     * @return A list of interface names
     */
    List<String> getUndocumentedModel() {
        return new Vector<String>(undocumentedInModel);
    }

    /**
     * Returns a list of names indicating which interfaces that are at least
     * somewhat documented in the UML model, and are documented in the Reference
     * Manual, but should be hidden anyway
     *
     * Call checkInterfaces() prior to calling this method.
     *
     * @return A list of interface names
     */
    public List<String> getHiddenInRefMan() {
        return new Vector<String>(hideInRefMan);
    }

    /**
     * Removes all interfaces from the API Reference Manual, depending on if
     * they are missing in the UML model or present, but configured to be
     * non-used in this platform.
     *
     * This method operates on the underlying non-filtered version of the API
     * Reference Manual, returning the result.
     *
     * @return A TocElement representing the filtered version of the API
     *         Reference Manual.
     */
    public TocElement removeInterfaces() {
        api.removeInterfaces(undocumentedInModel);
        api.removeInterfaces(hideInRefMan);

        return api;
    }

}
