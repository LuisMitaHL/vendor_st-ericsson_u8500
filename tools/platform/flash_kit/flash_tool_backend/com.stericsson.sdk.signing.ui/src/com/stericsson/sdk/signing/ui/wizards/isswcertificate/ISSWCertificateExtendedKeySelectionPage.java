package com.stericsson.sdk.signing.ui.wizards.isswcertificate;

import com.stericsson.sdk.signing.generic.GenericSoftwareType;

/**
 * Key selection page
 * 
 * @author TSIKOR01
 * 
 */
public class ISSWCertificateExtendedKeySelectionPage extends ISSWCertificateKeySelectionPage {
    /**
     * Default constructor
     * 
     * @param pageTitle
     *            Title of the page
     */
    protected ISSWCertificateExtendedKeySelectionPage(String pageTitle) {
        super(pageTitle);
    }

    /**
     * Initialization, adding additional values to array
     */
    @Override
    public void initializeValues() {
        super.initializeValues();
        values.put(GenericSoftwareType.USTA, KEY_NOT_SELECTED);
        values.put(GenericSoftwareType.IMAD, KEY_NOT_SELECTED);
    }
}
