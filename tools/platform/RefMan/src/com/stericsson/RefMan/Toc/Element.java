/**
 * Copyright ST-Ericsson 2010. All rights reserved.
 */
package com.stericsson.RefMan.Toc;

import java.util.Collection;
import java.util.Collections;
import java.util.List;
import java.util.Vector;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/**
 *
 * @author Thomas Palmqvist
 *
 */
public abstract class Element {

    /** The logger */
    final static Logger logger = LoggerFactory.getLogger(Element.class);

    /** Will be showed in the Reference Manual. */
    protected String label;

    /** Will be used as link path in the Reference Manual. */
    protected String href;

    /**
     * Contains Elements and is used to create a tree structure for the data
     * type.
     */
    protected Vector<Element> topics;

    /**
     * Constructor for the <code>Element</code>.
     *
     * @param label
     *            Will be showed in the Reference Manual.
     * @param href
     *            Will be used as link path in the Reference Manual.
     * @param topics
     *            Contains Elements and is used to create a tree structure for
     *            the data type.
     */
    public Element(String label, String href, Vector<Element> topics) {
        this.label = label;
        this.href = href;
        this.topics = topics;
    }

    /**
     * Constructor for the <code>Element</code>.
     */
    public Element() {
        this.label = "";
        this.href = "";
        topics = new Vector<Element>();
        Collections.sort(topics, new ElementComparator());
    }

    /**
     * See return description.
     *
     * @return The <code>String</code> that will be showed in the Reference
     *         Manual.
     */
    public String getLabel() {
        return label;
    }

    /**
     * See set description.
     *
     * @param label
     *            The <code>String</code> that will be showed in the Reference
     *            Manual
     */
    public void setLabel(String label) {
        this.label = label;
    }

    /**
     * See return description.
     *
     * @return The <code>String</code> that will be used as the link path in the
     *         Reference Manual.
     */
    public String getHref() {
        return href;
    }

    /**
     * See set description.
     *
     * @param href
     *            The <code>String</code> that will be used as the link path in
     *            the Reference Manual.
     */
    public void setHref(String href) {
        this.href = href;
    }

    /**
     * See return description.
     *
     * @return A <code>List</code> containing the <code>Elements</code> topics.
     */
    public List<Element> getTopics() {
        return topics;
    }

    /**
     * See set description.
     *
     * @param topics
     *            Sets this <code>Element's</code> topics to the ones provided by the
     *            <code>Collection</code> .
     */
    public void setTopics(Collection<Element> topics) {
        this.topics = new Vector<Element>(topics);
    }

    /**
     * Adds a topic to the <code>Element's</code> topics.
     *
     * @param topic
     *            <code>Element</code> to be appended to this <code>Element's</code>
     *            topics.
     * @return {@code true} (as specified by {@link Collection#add})
     */
    public boolean addTopic(Element topic) {
        return topics.add(topic);
    }

    /**
     * Adds topics to the <code>Element's</code> topics.
     *
     * @param topics
     *            <code>Elements</code> to be appended to this <code>Element's</code>
     *            topics.
     * @return {@code true} (as specified by {@link Collection#addAll})
     */
    public boolean addTopics(Collection<Element> topics) {
        return this.topics.addAll(topics);
    }

    /**
     * See return description.
     *
     * @return A <code>String</code> representing the type of the
     *         <code>Element</code>.
     */
    abstract public String getType();

    /**
     * Calculates the hashCode of this <code>Element</code>.
     *
     * @return A hashCode value for this <code>Element</code>.
     */
    abstract public int hashCode();

    /**
     * Compares an <code>Object</code> to this <code>Element</code> to see if
     * they are equal.
     *
     * @param o
     *            The <code>Object</code> to be compared to this
     *            <code>Element</code>
     * @return {@code true} if the <code>Object</code> and <code>Element</code>
     *         are equal, otherwise <code>false</code>.
     */
    @Override
    abstract public boolean equals(Object o);

    /**
     * See return description.
     *
     * @return A <code>String</code> containing information about the
     *         <code>Element</code>.
     */
    @Override
    abstract public String toString();

}
