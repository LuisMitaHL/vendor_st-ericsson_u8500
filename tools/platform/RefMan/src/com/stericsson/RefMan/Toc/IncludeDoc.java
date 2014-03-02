/**
 * Copyright ST-Ericsson 2010. All rights reserved.
 */
package com.stericsson.RefMan.Toc;

public class IncludeDoc {

    /** The name of the document. */
    private String name;

    /** The path to the destination the document should be copied to. */
    private String destination;

    /**
     * <code>True</code> if the documentation is in a directory,
     * <code>False</code> if it is a single file.
     */
    private boolean isDirectory;

    /**
     * Constructor for the <code>IncludeDoc</code>
     * 
     * @param name
     *            The name of the document
     * @param destination
     *            The path to the destination the document should be copied to
     */
    public IncludeDoc(String name, String destination, boolean directory) {
        this.name = name;
        this.destination = destination;
        this.isDirectory = directory;
    }

    /**
     * See set description.
     * 
     * @param name
     *            The <code>String</code> representing the name of the document.
     */
    public void setName(String name) {
        this.name = name;
    }

    /**
     * See return description.
     * 
     * @return The <code>String</code> representing the name of the document.
     */
    public String getName() {
        return name;
    }

    /**
     * See set description.
     * 
     * @param destination
     *            The <code>String</code> the path to the destination the
     *            document should be copied to.
     */
    public void setDestination(String destination) {
        this.destination = destination;
    }

    /**
     * See return description.
     * 
     * @return The <code>String</code> representing the path to the destination
     *         the document should be copied to.
     */
    public String getDestination() {
        return destination;
    }

    /**
     * See return description.
     * 
     * @return {@code true} if the <code>IncludeDoc</code> represents
     *         documentation in a directory, otherwise <code>false</code>.
     */
    public boolean isDirectory() {
        return isDirectory;
    }

    /**
     * Compares an <code>Object</code> to this <code>IncludeDoc</code> to see if
     * they are equal.
     * 
     * @param o
     *            The <code>Object</code> to be compared to this
     *            <code>IncludeDoc</code>
     * @return {@code true} if the <code>Object</code> and
     *         <code>IncludeDoc</code> are equal, otherwise <code>false</code>.
     */
    public boolean equals(Object o) {
        if (o == this) {
            return true;
        }
        if (o instanceof IncludeDoc) {
            IncludeDoc inc = (IncludeDoc) o;
            if (name.equals(inc.getName())
                    && destination.equals(inc.getDestination())) {
                return true;
            }
        }
        return false;
    }

    /**
     * Calculates the hashCode of this <code>IncludeDoc</code>.
     * 
     * @return A hashCode value for this <code>IncludeDoc</code>.
     */
    public int hashCode() {
        int hash = 7;
        hash = 31 * hash + name.hashCode();
        hash = 31 * hash + destination.hashCode();
        return hash;
    }
}
