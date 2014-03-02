package com.stericsson.sdk.common.ui.wizards.logic;

/**
 * Contains constants for DefaultDataSheet.
 * 
 * @author mielcluk
 * 
 */
interface IDefaultDataSheetConstants {

    // These constants are used in conjunction with the merge report
    int EVENTDESCREPLACEDID = 1;

    String EVENTDESCREPLACEDSTR = "Replaced ID number";

    int EEF_HEADER_LENGTH = 12;

    int EEF_HEADER_VERSION_1 = 1;

    // private int EEF_HEADER_VERSION_2 = 2;

    // Constants used to access the the document
    int IDCOLUMN = 0;

    int VARIABLECOLUMN = 1;

    int RESPONSIBLECOLUMN = 2;

    int BLOCKCOLUMN = 3;

    int MODULECOLUMN = 4;

    int TYPECOLUMN = 5;

    int SIZECOLUMN = 6;

    int ARRAYCOLUMN = 7;

    int INITIALDATACOLUMN = 8;

    int RANGECOLUMN = 9;

    int CUSTOMIZECOLUMN = 10;

    int CALIBRATECOLUMN = 11;

    int SHORTDESCCOLUMN = 12;

    int DETAILDESCCOLUMN = 13;

    int NBROFCOLUMNS = 14;

    String DOCDATACOLUMN = "C";

    // General constants

    String TYPEGDVARSTRING = "GD_VAR";

    int TYPEGDVARINT = 0;

    String TYPEGDARRAYSTRING = "GD_ARRAY";

    int TYPEGDARRAYINT = 1;

    String TYPEGDRESERVESTRING = "GD_RESERVE";

    int TYPEGDRESERVEINT = 2;

    String DOCUMENTTYPESTREMP = "E";

    // private int c_documentTypeIntEMP = 0;
    String DOCUMENTTYPESTRCUST = "C";

    // private int c_documentTypeIntCust = 1;

    double CONVERSIONFACTOREXCELCOL = 260;

    String PLATFORMUSEDEFAULTDATACOLUMN = "X";

    String YESABBREVIATION = "Y";

    String NOABBREVIATION = "N";

    String DEFAULTDATASHEETNAME = "Default Data";

    String DOCUMENTDATASHEETNAME = "Document Data";

    String ERRORMSGMAKESURETHATTHECELLCONTAINS = "Make sure that the cell contains ";

    // Constants used as header for the GDF and GDVAR file headers
    String FILEHEADER1 = "//****************************************************************************";

    int TYPEEEVARINT = 3;

    String TYPEEEARRAYSTRING = "GD_ARRAY";

    int TYPEEEARRAYINT = 4;

    String TYPEEERESERVESTRING = "GD_RESERVE";

    int TYPEEERESERVEINT = 5;

    String TYPEEEVARSTRING = "EE_VAR";

    String EEVARFILEHEADER1 = "EE_VAR_BASE()";

    String EEVARFILEFOOTER1 = "EE_VAR_END()";

    String FILEHEADER2 = "//";

    String GDVARFILEHEADER1 = "GD_VAR_BASE()";

    String GDVARFILEHEADER2 =
        "// Type        Id Blk"
            + "                                 Name                                  Esize  N       Resp";

    String GDVARFILEFOOTER1 = "GD_VAR_END()";

    String FILESPACING = "   ";

    String FILECOLON = " : ";

    String FILENEWLINE = "\r\n";

    String FILELONGSPACING = "    ";

    String FILESHORTSPACING = "  ";

    String USERINFODONESTRING = "Done - no errors";

//    String USERINFOSKIPPEDSTRING = "Skipped by user";

    int C_MAXNBROFUSERCOMMENTS = 4;
}
