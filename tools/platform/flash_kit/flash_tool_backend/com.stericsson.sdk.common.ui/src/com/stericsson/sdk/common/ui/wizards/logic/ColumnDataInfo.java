package com.stericsson.sdk.common.ui.wizards.logic;

/**
 * Class that is used to keep track of which column that a certain parameter is found in
 * the Document Data sheet. Name is used when creating new Excel workbooks (i.e. 19062
 * documents) Column is the column that is found on the Document Data sheet.
 */
public class ColumnDataInfo {
    /**
     * 
     */
    String name;

    /**
     * 
     */
    String column;

    /**
     * 
     */
    double widthInExcelFile;
}
