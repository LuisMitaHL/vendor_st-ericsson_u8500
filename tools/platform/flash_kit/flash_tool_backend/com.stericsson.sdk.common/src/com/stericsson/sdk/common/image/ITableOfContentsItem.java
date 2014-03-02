/**
 * 
 */
package com.stericsson.sdk.common.image;

/**
 * @author xolabju
 * @author cizovhel
 * 
 */
public interface ITableOfContentsItem {
    /**
     * Total size of a single TOC item
     */
    int SIZE = 32;

    /** */
    int SIZE_START = 4;

    /** */
    int SIZE_SIZE = 4;

    /** */
    int SIZE_FLAGS = 4;

    /** */
    int SIZE_ENTRY_POINT = 4;

    /** */
    int SIZE_LOAD_ADDRESS = 4;

    /** The size of the filename field of the item */
    int SIZE_FILENAME = 12;

    /** */
    int FIELD_START = 1;

    /** */
    int FIELD_SIZE = 2;

    /** */
    int FIELD_FLAGS = 3;

    /** */
    int FIELD_ENTRY_POINT = 4;

    /** */
    int FIELD_LOAD_ADDRESS = 5;

    /** */
    int FIELD_FILENAME = 6;

    // TOC item file names.
    /** */
    String FILENAME_ISSW = "ISSW";

    /** */
    String FILENAME_XLOADER = "X-LOADER";

    /** */
    String FILENAME_TEE = "TEE";

    /** */
    String FILENAME_MEM_INIT = "MEM_INIT";

    /** */
    String FILENAME_NORMAL = "NORMAL";

    /** Power Management / XP70 */
    String FILENAME_PWR_MGT = "PWR_MGT";

    /** */
    String FILENAME_IPL = "IPL";

    /** */
    String FILENAME_MODEM = "MODEM";

    /** */
    String FILENAME_ADL = "ADL";

    /** */
    String FILENAME_PRODUCTION = "PRODUCTION";

    /** */
    String FILENAME_CRKC = "CRKC";

    /** Generic DNT certificate */
    String FILENAME_DNT = "DNT-CERT";

    /** DNT certificate is used for customer separation */
    String FILENAME_STE_DNT = "STE-DNT-CERT";
}
