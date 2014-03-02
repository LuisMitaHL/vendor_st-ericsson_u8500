package com.stericsson.sdk.common.ui.wizards.logic;

import java.util.Vector;

/**
 * Class that represents an entire row in the Default Data sheet.
 */
public final class DefaultDataInfo {
    boolean enabled; // If the data below is enabled for the current platform

    int id;

    String variable;

    String responsible;

    int block;

    String module;

    int type;

    int size;

    int array;

    String initialDataStr;

    Vector<String> initialDataVec;

    String dataFoundInCellPos;

    int rangeUpper;

    int rangeLower;

    String rangeText;

    boolean customize;

    boolean calibrate;

    String shortDesc;

    String detailedDesc;

}
