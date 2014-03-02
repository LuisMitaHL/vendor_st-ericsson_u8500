/**
 * Copyright ST-Ericsson 2010. All rights reserved.
 */
package com.stericsson.RefMan.Toc;

import java.util.List;
import java.util.Vector;

public class TocElement extends Element {

    /** A <code>String</code> saying that this is a <code>TocElement</code> */
    private static final String type = "TOC";

    /**
     * A list of all Elements that contain the name of the interface. This is
     * for convenience when checking against the PAS model.
     */
    private List<Element> interfaceNames;

    /**
     * Constructor for the <code>TocElement</code>.
     *
     * @param label
     *            Will be showed in the Reference Manual.
     * @param href
     *            Will be used as link path in the Reference Manual.
     * @param topics
     *            Contains Elements and is used to create a tree structure for
     *            the data type.
     */
    public TocElement(String label, String href, Vector<Element> topics) {
        super(label, href, topics);
    }

    /**
     * Constructor for the <code>TocElement</code>.
     */
    public TocElement() {
        super();
    }

    /**
     * See return description.
     *
     * @return A <code>String</code> representing the type of the
     *         <code>TocElement</code>.
     */
    @Override
    public String getType() {
        return type;
    }

    /**
     * See return description.
     *
     * @return A <code>String</code> containing information about the
     *         <code>TocElement</code>.
     */
    @Override
    public String toString() {
        return "type: TOC; label: " + label + "; href: " + href + "; topics: "
                + topics.size();
    }

    /**
     * Calculates the hashCode of this <code>TocElement</code>.
     *
     * @return A hashCode value for this <code>TocElement</code>.
     */
    @Override
    public int hashCode() {
        int hash = 7;
        hash = 31 * hash + getType().hashCode();
        hash = 31 * hash + (null == label ? 0 : label.hashCode());
        hash = 31 * hash + (null == href ? 0 : href.hashCode());
        hash = 31 * hash + topics.hashCode();
        return hash;
    }

    /**
     * Compares an <code>Object</code> to this <code>TocElement</code> to see if
     * they are equal.
     *
     * @param o
     *            The <code>Object</code> to be compared to this
     *            <code>TocElement</code>
     * @return {@code true} if the <code>Object</code> and
     *         <code>TocElement</code> are equal, otherwise <code>false</code>.
     */
    @Override
    public boolean equals(Object o) {
        if (o == this) {
            return true;
        }
        if (o instanceof TocElement) {
            TocElement e = (TocElement) o;
            if (href == null) {
                if (label.equals(e.getLabel()) && href == e.getHref()
                        && topics.equals(e.getTopics())) {
                    return true;
                }
            } else {
                if (label.equals(e.getLabel()) && href.equals(e.getHref())
                        && topics.equals(e.getTopics())) {
                    return true;
                }
            }

        }
        return false;
    }

    /**
     * Get the element that contains the name of the specified interface. This
     * is for convenience when checking against the PAS model.
     */
    public Element getElement(String interfaceName) {
        for (Element e : interfaceNames) {
            if (e.getLabel().compareToIgnoreCase(interfaceName) == 0) {
                return e;
            }
        }
        return null;
    }

    /**
     * See return description.
     *
     * @return A list of all Elements that contain the name of the interface.
     */
    public List<Element> getInterfaces() {
        return interfaceNames;
    }

    /**
     * Sets a list of all Elements that contain the name of the interface. This
     * is for convenience when checking against the PAS model.
     */
    public void setInterfaces(List<Element> interfaceNames) {
        logger.trace("setInterfaces() + " + interfaceNames);
        this.interfaceNames = interfaceNames;

    }

    /**
     * Removes all interfaces from the TocElement that are part of the specified
     * list.
     *
     * @param interfaceNamesToRemove
     *            Names of interfaces to remove.
     */
    public void removeInterfaces(List<String> interfaceNamesToRemove) {
        List<Element> stacks = getTopics();
        Vector<Element> newStacks = new Vector<Element>();
        List<Element> newInterfaceList = new Vector<Element>();
        int stacksRemoved = 0;
        for (Element stack : stacks) {
            String stackName = stack.getLabel();
            List<Element> spaces = stack.getTopics();
            Vector<Element> newSpaces = new Vector<Element>();
            int spacesRemoved = 0;
            for (Element space : spaces) {
                String spaceName = space.getLabel();
                List<Element> interfaces = space.getTopics();
                Vector<Element> newInterfaces = new Vector<Element>();
                int interfacesRemoved = 0;
                for (Element i : interfaces) {
                    String interfaceName = i.getLabel();
                    if (interfaceNamesToRemove.contains(interfaceName)) {
                        logger.debug("Removing interface '{}' from '{}'/'"
                                + spaceName + "'", interfaceName, stackName);
                        interfacesRemoved++;
                    } else {
                        newInterfaces.add(i);
                        newInterfaceList.add(i);
                    }
                }
                if (interfacesRemoved > 0) {
                    logger
                            .debug(
                                    "Setting new interfaces to a list of {} interfaces",
                                    newInterfaces.size());
                    space.setTopics(newInterfaces);
                }
                if (newInterfaces.size() == 0) {
                    logger
                            .debug(
                                    "No more interfaces left for layerspace '{}', removing it as well from stack ''",
                                    spaceName, stackName);
                    spacesRemoved++;
                } else {
                    newSpaces.add(space);
                }
            }
            if (spacesRemoved > 0) {
                logger.debug(
                        "Setting new layerspaces to a list of {} layerspaces ",
                        newSpaces.size());
                stack.setTopics(newSpaces);
            }
            if (newSpaces.size() == 0) {
                logger
                        .debug(
                                "No more layerspaces left for stack '{}', removing it as well from manual",
                                stackName);
                stacksRemoved++;
            } else {
                newStacks.add(stack);
            }
        }

        if (stacksRemoved > 0) {
            logger.debug("Setting new stacks to a list of {} stacks ",
                    newStacks.size());
            setTopics(newStacks);
        }
        if (newStacks.size() == 0) {
            logger
                    .info("WARNING: No more stacks for manual! Something is probably wrong...");
        }
        logger
                .debug(
                        "And finally updating the interfaces list with a list of {} interfaces",
                        newInterfaceList.size());
        setInterfaces(newInterfaceList);
    }

    /**
     * Returns a list of Strings, each on the format
     * "stackName/spaceName/interfaceName", for easy comparasion.
     *
     * This method is mostly designed for debugging and testing.
     *
     * @return List<String> on the format "stackName/spaceName/interfaceName".
     */
    public List<String> getInterfaceNames() {
        List<Element> stacks = getTopics();
        List<String> foundInterfaceNames = new Vector<String>();
        for (Element stack : stacks) {
            List<Element> spaces = stack.getTopics();
            String stackName = stack.getLabel();
            for (Element space : spaces) {
                List<Element> interfaces = space.getTopics();
                String spaceName = space.getLabel();
                for (Element i : interfaces) {
                    String interfaceName = i.getLabel();
                    String foundInterface = stackName + '/' + spaceName + '/'
                            + interfaceName;
                    foundInterfaceNames.add(foundInterface);
                }
            }
        }
        return foundInterfaceNames;
    }

}
