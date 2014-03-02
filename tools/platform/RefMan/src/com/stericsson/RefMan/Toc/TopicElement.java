/**
 * Copyright ST-Ericsson 2010. All rights reserved.
 */
package com.stericsson.RefMan.Toc;

import java.util.Vector;

public class TopicElement extends Element {

    /** A <code>String</code> saying that this is a <code>TopicElement</code> */
    private static final String type = "TOPIC";

    /**
     * Constructor for the <code>TopicElement</code>.
     * 
     * @param label
     *            Will be showed in the Reference Manual.
     * @param href
     *            Will be used as link path in the Reference Manual.
     * @param topics
     *            Contains Elements and is used to create a tree structure for
     *            the data type.
     */
    public TopicElement(String label, String href, Vector<Element> topics) {
        super(label, href, topics);
    }

    /**
     * Constructor for the <code>TopicElement</code>.
     */
    public TopicElement() {
        super();
    }

    /**
     * See return description.
     * 
     * @return A <code>String</code> representing the type of the
     *         <code>TopicElement</code>.
     */
    public String getType() {
        return type;
    }

    /**
     * See return description.
     * 
     * @return A <code>String</code> containing information about the
     *         <code>TopicElement</code>.
     */
    @Override
    public String toString() {
        return "type: TOPIC; label: " + label + "; href: " + href
                + "; topics: " + topics.size();
    }

    /**
     * Calculates the hashCode of this <code>TopicElement</code>.
     * 
     * @return A hashCode value for this <code>TopicElement</code>.
     */
    public int hashCode() {
        int hash = 7;
        hash = 31 * hash + getType().hashCode();
        hash = 31 * hash + (null == label ? 0 : label.hashCode());
        hash = 31 * hash + (null == href ? 0 : href.hashCode());
        hash = 31 * hash + topics.hashCode();
        return hash;
    }

    /**
     * Compares an <code>Object</code> to this <code>TopicElement</code> to see
     * if they are equal.
     * 
     * @param o
     *            The <code>Object</code> to be compared to this
     *            <code>TopicElement</code>
     * @return {@code true} if the <code>Object</code> and
     *         <code>TopicElement</code> are equal, otherwise <code>false</code>
     *         .
     */
    @Override
    public boolean equals(Object o) {
        if (o == this) {
            return true;
        }
        if (o instanceof TopicElement) {
            TopicElement e = (TopicElement) o;
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
}
