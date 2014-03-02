package com.stericsson.sdk.common.ui.wizards.logic;

/**
 * Class that is used to keep track of the platforms listed in the Document Data sheet. Name is the
 * platform name Apply is the column where it is anticipated to find either a X or a bold X which
 * determines if an Initial data row should be used for this platform. AlternateData is the column
 * where Alternate Initial Data is found. This data is used if the X in the apply column is marked
 * with in bold style.
 */
public class PlatformsInfo {
    String name;

    String apply;

    String alternateData;

    /**
     * @return platform name
     */
    public String getName() {
        return this.name;
    }
}
