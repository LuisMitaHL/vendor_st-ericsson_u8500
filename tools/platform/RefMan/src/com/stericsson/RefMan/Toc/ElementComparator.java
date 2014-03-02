/**
 * Copyright ST-Ericsson 2010. All rights reserved.
 */
package com.stericsson.RefMan.Toc;

import java.io.Serializable;
import java.util.Comparator;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/**
 *
 * @author Thomas Palmqvist
 *
 */
public class ElementComparator implements Comparator<Element>, Serializable {

    /**
     * The version of this class with consideration of serialization.
     */
    private static final long serialVersionUID = 1L;

    /** The logger */
    final static Logger logger = LoggerFactory.getLogger(DocumentFactory.class);

    // Java 1.6 @Override
    /**
     * Compares two <code>Element</code> with regards to it's label.
     *
     * @param e1
     *            The first <code>Element</code> to be used in the comparison.
     * @param e2
     *            The second <code>Element</code> to be used in the comparison.
     * @return Lexicographically comparison.
     *         <p>
     *         Except when the two labels are "Kernel space" and "User space" or
     *         vice versa.
     */
    public int compare(Element e1, Element e2) {
        int result;
        // If Kernel space and User space are the two labels compared.
        if ((e1.getLabel().equalsIgnoreCase("Kernel space") && e2.getLabel()
                .equalsIgnoreCase("User space"))
                || (e2.getLabel().equalsIgnoreCase("Kernel space") && e1
                        .getLabel().equalsIgnoreCase("User space"))) {
            result = -e1.getLabel().compareToIgnoreCase(e2.getLabel());
            // If the two labels are the same lexicographically then use case
            // sensitive comparison.
        } else if (e1.getLabel().compareToIgnoreCase(e2.getLabel()) == 0) {
            result = e1.getLabel().compareTo(e2.getLabel());
            // Default case use non case sensitive comparison.
        } else {
            result = e1.getLabel().compareToIgnoreCase(e2.getLabel());
        }
        return result;
    }

}
