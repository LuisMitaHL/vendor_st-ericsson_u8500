package com.stericsson.sdk.common.ui.wizards.logic;

/**
 * Class that is used to create a report when a GDF file has been merged with a 19062
 * document. The idea with this information is to give the user an idea of which ID numbers that
 * have been replaced (i.e. GDF ID number that has been overwritten by 19062 data with the same
 * ID)
 */
public final class MergeReportInfo {
    int id;

    int eventDescID;
}
