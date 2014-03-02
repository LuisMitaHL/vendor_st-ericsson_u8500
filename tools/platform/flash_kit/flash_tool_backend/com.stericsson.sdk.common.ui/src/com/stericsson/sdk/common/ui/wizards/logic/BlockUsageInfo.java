package com.stericsson.sdk.common.ui.wizards.logic;

/**
 * Class that is used to keep track of how much allocation is done for each block This is
 * later used for a report
 */
public final class BlockUsageInfo {

    int allocated;

    int used;

    int nbrOfItems;

    int nbrCalibrated;

    int nbrCustomize;
}
