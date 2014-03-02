/**
 * Copyright ST-Ericsson 2010. All rights reserved.
 */
package com.stericsson.RefMan.USDoc;

import java.util.List;

public interface FindTocLocationsFiles {

    /**
     * A method which finds all toc-locations.xml files.
     *
     * @return A <code>List</code> containing <code>String</code>'s representing
     *         the absolute paths to the toc-locations.xml files.
     */
    public List<String> findTocLocationsFiles();
}
