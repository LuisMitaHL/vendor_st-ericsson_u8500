package com.stericsson.sdk.common.ui.wizards.logic;

/**
 * Class that keeps track of general statistics regarding the document
 */
public final class DocumentInfo {
    boolean infoIsValid;

    String fileName;

    String selectedPlatform;

    int maxFoundBlockNumber;

    int totalNbrOfRows;

    int nbrOfEnabledRows;

    int nbrOfAlternateDataRows;

    int firstRowNbr;

    int lastRowNbr;

}
