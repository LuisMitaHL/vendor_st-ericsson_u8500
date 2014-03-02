package com.stericsson.sdk.common.ui.wizards.logic;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.util.Locale;
import java.util.StringTokenizer;
import java.util.Vector;

import org.apache.poi.hssf.usermodel.HSSFSheet;
import org.apache.poi.hssf.usermodel.HSSFWorkbook;
import org.apache.poi.poifs.filesystem.POIFSFileSystem;

/**
 * Workbook reader utilities
 * 
 * @author kapalpaw
 * 
 */
public final class WorkbookReader implements IDefaultDataSheetConstants {

    private WorkbookReader() {

    }

    /**
     * TODO
     */
    /* package */static void getColumnDataInfo(DefaultDataSheetPropertiesData pPropData, HSSFSheet pCurrentSheetUsed) {
        // Read ID, Variable, Responsible, etc.
        pPropData.getColumnDataInfo()[IDCOLUMN].column =
            CellsReader.getStringAtPosition(DOCDATACOLUMN, 7, pCurrentSheetUsed).toUpperCase(Locale.getDefault());
        pPropData.getColumnDataInfo()[VARIABLECOLUMN].column =
            CellsReader.getStringAtPosition(DOCDATACOLUMN, 8, pCurrentSheetUsed).toUpperCase(Locale.getDefault());
        pPropData.getColumnDataInfo()[RESPONSIBLECOLUMN].column =
            CellsReader.getStringAtPosition(DOCDATACOLUMN, 9, pCurrentSheetUsed).toUpperCase(Locale.getDefault());
        pPropData.getColumnDataInfo()[BLOCKCOLUMN].column =
            CellsReader.getStringAtPosition(DOCDATACOLUMN, 10, pCurrentSheetUsed).toUpperCase(Locale.getDefault());
        pPropData.getColumnDataInfo()[MODULECOLUMN].column =
            CellsReader.getStringAtPosition(DOCDATACOLUMN, 11, pCurrentSheetUsed).toUpperCase(Locale.getDefault());
        pPropData.getColumnDataInfo()[TYPECOLUMN].column =
            CellsReader.getStringAtPosition(DOCDATACOLUMN, 12, pCurrentSheetUsed).toUpperCase(Locale.getDefault());
        pPropData.getColumnDataInfo()[SIZECOLUMN].column =
            CellsReader.getStringAtPosition(DOCDATACOLUMN, 13, pCurrentSheetUsed).toUpperCase(Locale.getDefault());
        pPropData.getColumnDataInfo()[ARRAYCOLUMN].column =
            CellsReader.getStringAtPosition(DOCDATACOLUMN, 14, pCurrentSheetUsed).toUpperCase(Locale.getDefault());
        pPropData.getColumnDataInfo()[INITIALDATACOLUMN].column =
            CellsReader.getStringAtPosition(DOCDATACOLUMN, 15, pCurrentSheetUsed).toUpperCase(Locale.getDefault());
        pPropData.getColumnDataInfo()[RANGECOLUMN].column =
            CellsReader.getStringAtPosition(DOCDATACOLUMN, 16, pCurrentSheetUsed).toUpperCase(Locale.getDefault());
        pPropData.getColumnDataInfo()[CUSTOMIZECOLUMN].column =
            CellsReader.getStringAtPosition(DOCDATACOLUMN, 17, pCurrentSheetUsed).toUpperCase(Locale.getDefault());
        pPropData.getColumnDataInfo()[CALIBRATECOLUMN].column =
            CellsReader.getStringAtPosition(DOCDATACOLUMN, 18, pCurrentSheetUsed).toUpperCase(Locale.getDefault());
        pPropData.getColumnDataInfo()[SHORTDESCCOLUMN].column =
            CellsReader.getStringAtPosition(DOCDATACOLUMN, 19, pCurrentSheetUsed).toUpperCase(Locale.getDefault());
        pPropData.getColumnDataInfo()[DETAILDESCCOLUMN].column =
            CellsReader.getStringAtPosition(DOCDATACOLUMN, 20, pCurrentSheetUsed).toUpperCase(Locale.getDefault());
    }

    private static HSSFSheet mDefaultData;

    static boolean mFileHasBeenOpenedCorrectly;

    static boolean readDocumentDataSheetProperties(String pddsFileName, DefaultDataSheetPropertiesData pPropData) {
        boolean result = true;
        String tempString;
        int workRowNbr = 0;
        File genericFile = null;
        FileInputStream fis = null;

        // NOTE: moved outside the method
        // // Clear properties first
        // clearProperties();

        ErrorInfoUtils.log("Opening file ");
        ErrorInfoUtils.logn(pddsFileName);

        genericFile = new File(pddsFileName);
        if (genericFile.isFile()) {
            try {
                // Read the actual sheets and make sure they exist
                fis = new FileInputStream(pddsFileName);
                final POIFSFileSystem pFile = new POIFSFileSystem(fis);
                final HSSFWorkbook pWorkbook = new HSSFWorkbook(pFile);
                final HSSFSheet pDocumentData = pWorkbook.getSheet(DOCUMENTDATASHEETNAME);
                // TODO: side effect of this method, verify if it's needed elsewere
                mDefaultData = pWorkbook.getSheet(DEFAULTDATASHEETNAME);

                // Then read the document data
                if ((pDocumentData != null) && (mDefaultData != null)) {

                    // TODO: moved outside the method
                    // mExcelFileName = ddsFileName;
                    // Make sure we're reading from the correct sheet first
                    // mCurrentSheetUsed = pDocumentData;

                    // Read the Document type
                    workRowNbr = 4; // The first row with data in a 19062 document
                    tempString =
                        CellsReader.getStringAtPosition(DOCDATACOLUMN, workRowNbr, pDocumentData).toUpperCase(
                            Locale.getDefault());

                    if (tempString.compareTo(DOCUMENTTYPESTREMP) != 0 && tempString.compareTo(DOCUMENTTYPESTRCUST) != 0) {
                        // m_documentType = -1;
                        result = false;
                        ErrorInfoUtils.error("Incorrect value of Document type (" + tempString + ")", DOCDATACOLUMN
                            + workRowNbr, ERRORMSGMAKESURETHATTHECELLCONTAINS + "either a " + DOCUMENTTYPESTREMP
                            + " or a " + DOCUMENTTYPESTRCUST);
                    }

                    ErrorInfoUtils.logn("Reading Document Data");
                    // Read the Startrow
                    workRowNbr = 5;
                    pPropData.setStartRow(CellsReader.getNumberAtPosition(DOCDATACOLUMN, workRowNbr, pDocumentData));
                    if (pPropData.getStartRow() < 1) {
                        result = false;
                        ErrorInfoUtils.error("Incorrect value of Startrow (" + pPropData.getStartRow() + ")",
                            DOCDATACOLUMN + workRowNbr, ERRORMSGMAKESURETHATTHECELLCONTAINS
                                + "a numeric value greater than zero");
                    }

                    // Read the Startrow
                    workRowNbr = 6;
                    pPropData.setEndRow(CellsReader.getNumberAtPosition(DOCDATACOLUMN, workRowNbr, pDocumentData));
                    if (pPropData.getEndRow() <= pPropData.getStartRow()) {
                        result = false;
                        ErrorInfoUtils.error("Incorrect value of Endrow (" + pPropData.getEndRow() + ")", DOCDATACOLUMN
                            + workRowNbr, ERRORMSGMAKESURETHATTHECELLCONTAINS
                            + "a numeric value greater than the Startrow");
                    }

                    // Gets info from columns
                    getColumnDataInfo(pPropData, pDocumentData);

                    // Find all the platforms used
                    getPlatformsUsed(pDocumentData, pPropData);

                    // Read all user comments to be used in the document
                    getUserComments(pDocumentData, pPropData);

                    // Okay, we're done reading the Document Data tab.
                    mFileHasBeenOpenedCorrectly = !ErrorInfoUtils.errorHasOccured();
                    if (mFileHasBeenOpenedCorrectly) {
                        ErrorInfoUtils.logn(USERINFODONESTRING);
                        pPropData.getmDocumentInfo().fileName = pddsFileName;
                    } else {
                        // cLogger.warn("" + DOCUMENTDATASHEETNAME + " or " + DEFAULTDATASHEETNAME +
                        // " does not exist !!");
                        ErrorInfoUtils.error("Either the " + DOCUMENTDATASHEETNAME + " sheet or the "
                            + DEFAULTDATASHEETNAME + " sheet does not exist", "", "Make sure that "
                            + DOCUMENTDATASHEETNAME + " and " + DEFAULTDATASHEETNAME
                            + " exists and that they are correctly spelt");
                        result = false;
                    }
                }
            } catch (Exception e) {
                // cLogger.warn("Failed to read document properties!");

                ErrorInfoUtils.error("Exception occured while reading " + DOCUMENTDATASHEETNAME, "",
                    "Make sure that the Excel document has a valid syntax");
                result = false;
            } finally {
                if (fis != null) {
                    try {
                        fis.close();
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                }
            }
        } else {
            ErrorInfoUtils.error("File does not exist", "", "Please make sure that a valid file is selected");
            result = false;
        }

        return result;
    }

    /**
     * TODO
     */
    private static void getUserComments(HSSFSheet pCurrentSheetUsed, DefaultDataSheetPropertiesData pPropData) {
        int workRowNbr;
        workRowNbr = 24;
        // 'Calculate' the previous column. If the first one is 'C' this will result in
        // a 'B'
        String docDataColumn1 = Character.valueOf((char) ((int) (DOCDATACOLUMN.charAt(0)) - 1)).toString();
        for (int i = 0; i < C_MAXNBROFUSERCOMMENTS; i++) {
            pPropData.getUserCommentsInfo()[i].column1 =
                CellsReader.getStringAtPosition(docDataColumn1, workRowNbr + i, pCurrentSheetUsed);
            pPropData.getUserCommentsInfo()[i].column2 =
                CellsReader.getStringAtPosition(DOCDATACOLUMN, workRowNbr + i, pCurrentSheetUsed);
        }
    }

    /**
     * Reads all utilized platforms.
     */
    private static void getPlatformsUsed(HSSFSheet pCurrentSheetUsed, DefaultDataSheetPropertiesData pPropData) {
        int workRowNbr;
        int startCol = (int) (DOCDATACOLUMN.charAt(0));
        workRowNbr = 21;

        // Get the first platform name
        int platformIndex = 0;
        String workCol = Character.valueOf((char) (platformIndex + startCol)).toString();
        String platformName = CellsReader.getStringAtPosition(workCol, workRowNbr, pCurrentSheetUsed).trim();

        while (platformName.length() != 0) { // As long as there are more platforms
            // to process
            PlatformsInfo platformsInfo = new PlatformsInfo();
            platformsInfo.name = platformName;
            platformsInfo.apply =
                CellsReader.getStringAtPosition(workCol, workRowNbr + 1, pCurrentSheetUsed).toUpperCase(
                    Locale.getDefault());
            platformsInfo.alternateData =
                CellsReader.getStringAtPosition(workCol, workRowNbr + 2, pCurrentSheetUsed).toUpperCase(
                    Locale.getDefault());
            pPropData.addPlatformsInfo(platformsInfo);

            // Get next platform name
            platformIndex++;
            workCol = Character.valueOf((char) (platformIndex + startCol)).toString();
            platformName = CellsReader.getStringAtPosition(workCol, workRowNbr, pCurrentSheetUsed).trim();
        }
    }

    /**
     * A method that checks if the current platform column is enabled
     * 
     * @param columnIndex
     *            is the Excel column that is to be used
     * @param rowIndex
     *            is the Excel row that is to be used
     * @return True if the platform is enabled, false if not
     */
    private static boolean getPlatformEnabled(String columnIndex, int rowIndex, HSSFSheet pCurrentSheetUsed) {
        String tempStr =
            CellsReader.getStringAtPosition(columnIndex, rowIndex, pCurrentSheetUsed).trim().toUpperCase(
                Locale.getDefault());
        if (tempStr.compareTo(PLATFORMUSEDEFAULTDATACOLUMN) == 0) {
            return true;
        } else if (tempStr.compareTo("") == 0) {
            return false;
        } else {
            ErrorInfoUtils.error("Cell contains invalid character", columnIndex + rowIndex,
                ERRORMSGMAKESURETHATTHECELLCONTAINS + "a " + PLATFORMUSEDEFAULTDATACOLUMN + " or is empty");
            return false;
        }
    }

    /**
     * A method that checks whether the original Initial Data column should be used (marked with a
     * X) or if the Alternate Initial Data column should be used (marked with a bold X) Note: The
     * value of m_SelectedPlatformIndex is used to know which columns to check.
     * 
     * @param columnIndex
     *            is the Excel column that is to be used
     * @param rowIndex
     *            is the Excel row that is to be used
     * @return True if a bold X is found, else false is returned
     */
    static boolean useAlternateInitialDataColumn(int rowIndex, DefaultDataSheetPropertiesData pPropData,
        HSSFSheet pCurrentSheetUsed) {
        boolean result = false;
        // First make sure that we have an X in the Apply column
        String applyColumnIndex =
            ((PlatformsInfo) pPropData.getPlatformsInfo().elementAt(pPropData.getSelectedPlatformIndex())).apply;

        if (CellsReader.getStringAtPosition(applyColumnIndex, rowIndex, pCurrentSheetUsed).trim().toUpperCase(
            Locale.getDefault()).compareTo(PLATFORMUSEDEFAULTDATACOLUMN) == 0) {
            // Then check if there is any data in the Alternate column
            String alternateDataColumnIndex =
                ((PlatformsInfo) pPropData.getPlatformsInfo().elementAt(pPropData.getSelectedPlatformIndex())).alternateData;
            String alternateData =
                CellsReader.getStringAtPosition(alternateDataColumnIndex, rowIndex, pCurrentSheetUsed);
            result = (!isThisStringConsideredBlank(alternateData)); // Use Alternate data if
            // column has contents
        } else {
            ErrorInfoUtils.error("Cell contains invalid character", applyColumnIndex + rowIndex,
                ERRORMSGMAKESURETHATTHECELLCONTAINS + "a " + PLATFORMUSEDEFAULTDATACOLUMN);
        }
        return result;
    }

    /**
     * A method that reads the contents of the default data column. This column is either the
     * original column or the alternate column. The method decides which one.
     * 
     * @param rowIndex
     *            is the Excel row that is to be used
     * @return A DefaultDataInfo structure. This structure is filled with the initial data string
     *         and the initial data vector
     */
    static DefaultDataInfo readInitialData(int rowIndex, DefaultDataSheetPropertiesData pPropData,
        HSSFSheet pCurrentSheetUsed) {
        String tempColumn;
        DefaultDataInfo tempDDI = new DefaultDataInfo();

        if (useAlternateInitialDataColumn(rowIndex, pPropData, pCurrentSheetUsed)) {
            tempColumn =
                ((PlatformsInfo) pPropData.getPlatformsInfo().elementAt(pPropData.getSelectedPlatformIndex())).alternateData;
        } else {
            tempColumn = pPropData.getColumnDataInfo()[INITIALDATACOLUMN].column;
        }

        tempDDI.dataFoundInCellPos = tempColumn + rowIndex;
        if (!ErrorInfoUtils.errorHasOccured()) {
            tempDDI.initialDataStr = CellsReader.getStringAtPosition(tempColumn, rowIndex, pCurrentSheetUsed);
            tempDDI.initialDataVec = createInitialDataVector(tempDDI.initialDataStr);

            ErrorInfoUtils.setCell(tempColumn + rowIndex + " " + ErrorInfoUtils.getCell());
        }
        return tempDDI;
    }

    /**
     * A method that creates a (string) vector from an initial data string. It is checked that all
     * values are numeric.
     * 
     * @param initialDataStr
     *            A string that should be have the correct delimiter and contain only numerical
     *            values
     * @return A vector of numerical string values. The values could be in either decimal or
     *         hexadecimal notation.
     */
    private static Vector<String> createInitialDataVector(String initialDataStr) {
        String tempToken;
        int tempCnt;
        Vector<String> resultVector = new Vector<String>();
        initialDataStr = initialDataStr.trim();

        // Is the string empty ?
        if (!isThisStringConsideredBlank(initialDataStr)) {
            StringTokenizer initialDataTkn = new StringTokenizer(initialDataStr, ",");

            // Does it contain any data at all
            if (initialDataTkn.countTokens() == 1) {
                // Aha, it contains only one parameter, i.e. no delimiter
                try {
                    Integer.decode(initialDataStr).intValue();
                    resultVector.add(initialDataStr);
                } catch (NumberFormatException e) {
                    ErrorInfoUtils.error("One non numerical value found (" + initialDataStr + ")", "",
                        ERRORMSGMAKESURETHATTHECELLCONTAINS + "a numerical value");
                }
            } else {
                tempCnt = 0;
                while (initialDataTkn.hasMoreTokens()) {
                    tempToken = initialDataTkn.nextToken().trim();
                    tempCnt++;
                    if (isThisStringConsideredBlank(tempToken)) {
                        ErrorInfoUtils.error("Empty or N/A value are not allowed", "[" + tempCnt + "]",
                            ERRORMSGMAKESURETHATTHECELLCONTAINS + "numerical values");
                        break;
                    } else {
                        try {
                            Integer.decode(tempToken).intValue();
                            resultVector.add(tempToken);
                        } catch (NumberFormatException e) {
                            ErrorInfoUtils.error("Non numerical value found (" + tempToken + ")", "[" + tempCnt + "]",
                                ERRORMSGMAKESURETHATTHECELLCONTAINS + "numerical values");
                            break;
                        }
                    }
                }
            }
        }
        return resultVector;
    }

    /**
     * A method that examines if a string is considered beeing blank.
     * 
     * @param The
     *            string to be examined
     * @return True if the string is "empty" otherwise false
     */
    private static boolean isThisStringConsideredBlank(String inStr) {
        if ((inStr.trim().length() == 0) || (inStr.compareToIgnoreCase("N/A") == 0)
            || (inStr.compareToIgnoreCase("NA") == 0) || (inStr.compareTo("-") == 0)) {
            return true;
        }
        return false;
    }

    /**
     * A method that reads the contents of a cell and expects to find a range expression. If this is
     * done it fills a DefaultData structure and returns this
     * 
     * @param columnIndex
     *            is the Excel column that is to be used
     * @param rowIndex
     *            is the Excel row that is to be used
     * @return A DefaultDataInfo structure that has the upper and lower range parameters set
     */
    static DefaultDataInfo readAndProcessRange(String columnIndex, int rowIndex, HSSFSheet pCurrentSheetUsed) {
        DefaultDataInfo tempDDI = new DefaultDataInfo();
        String tempString = CellsReader.getStringAtPosition(columnIndex, rowIndex, pCurrentSheetUsed).trim();
        tempDDI.rangeText = tempString;
        StringTokenizer rangeTkn = new StringTokenizer(tempString, ":");
        // First find out if it is empty. In that case we have no range values
        if (isThisStringConsideredBlank(tempString)) {
            tempDDI.rangeLower = Integer.MIN_VALUE;
            tempDDI.rangeUpper = Integer.MAX_VALUE;
        } else if (rangeTkn.countTokens() != 2) {
            // There should be one delimiter in the string
            ErrorInfoUtils.error("Invalid number of delimiters", columnIndex + rowIndex,
                ERRORMSGMAKESURETHATTHECELLCONTAINS + "only one :");
        } else {
            // Okay, the string has one delimiter, now try to figure out the contents
            // First the lower range
            try {
                tempString = ((String) rangeTkn.nextElement()).trim();
                if (isThisStringConsideredBlank(tempString)) {
                    tempDDI.rangeLower = Integer.MIN_VALUE;
                } else {
                    tempDDI.rangeLower = Integer.decode(tempString).intValue();
                }
            } catch (NumberFormatException e) {
                ErrorInfoUtils.error("The lower range is a non numeric value", columnIndex + rowIndex,
                    ERRORMSGMAKESURETHATTHECELLCONTAINS + "a numerical value\n"
                        + "If the value is a negative HEX value you must have the minus sign first e.g. -0x14");
            }
            // Now look at the upper range value
            try {
                tempString = ((String) rangeTkn.nextElement()).trim();
                if (isThisStringConsideredBlank(tempString)) {
                    tempDDI.rangeUpper = Integer.MAX_VALUE;
                } else {
                    tempDDI.rangeUpper = Integer.decode(tempString).intValue();
                }
            } catch (NumberFormatException e) {
                ErrorInfoUtils.error("The upper range is a non numeric value", columnIndex + rowIndex,
                    ERRORMSGMAKESURETHATTHECELLCONTAINS + "a numerical value\n"
                        + "If the value is a negative HEX value you must have the minus sign first e.g. -0x14");
            }
        }
        return tempDDI;
    }

    /**
     * A method that reads the cell contents and enforces the contents to be one of two strings.
     * 
     * @param columnIndex
     *            is the Excel column that is to be used
     * @param rowIndex
     *            is the Excel row that is to be used
     * @return true if one stringvalue is found, else false
     */
    private static boolean getYesOrNoValueFromPosition(String columnIndex, int rowIndex, HSSFSheet pCurrentSheetUsed) {
        String tempStr =
            CellsReader.getStringAtPosition(columnIndex, rowIndex, pCurrentSheetUsed).trim().toUpperCase(
                Locale.getDefault());
        if (tempStr.equals(YESABBREVIATION)) {
            return true;
        } else if (tempStr.equals(NOABBREVIATION)) {
            return false;
        } else {
            ErrorInfoUtils.error("Unexpected cell contents", columnIndex + rowIndex,
                ERRORMSGMAKESURETHATTHECELLCONTAINS + "either a " + YESABBREVIATION + " or a " + NOABBREVIATION);
            return false;
        }
    }

    /**
     * A method that determines which type of string that the type column contains. Available
     * constants are found in the c_TypeGD_VARString, c_TypeGD_ARRAYString, and
     * c_TypeGD_RESERVEString constants.
     * 
     * @param columnIndex
     *            is the Excel column that is to be used
     * @param rowIndex
     *            is the Excel row that is to be used
     * @return Returns an integer that reflects the type
     */
    static int readAndDetermineType(String columnIndex, int rowIndex, HSSFSheet pCurrentSheetUsed) {
        String tempString =
            CellsReader.getStringAtPosition(columnIndex, rowIndex, pCurrentSheetUsed).toUpperCase(Locale.getDefault());
        if (tempString.compareTo(TYPEGDVARSTRING) == 0) {
            return TYPEGDVARINT;
        } else if (tempString.compareTo(TYPEGDARRAYSTRING) == 0) {
            return TYPEGDARRAYINT;
        } else if (tempString.compareTo(TYPEGDRESERVESTRING) == 0) {
            return TYPEGDRESERVEINT;

        } else if (tempString.compareTo(TYPEEEVARSTRING) == 0) {
            return TYPEEEVARINT;
        } else if (tempString.compareTo(TYPEEEARRAYSTRING) == 0) {
            return TYPEEEARRAYINT;
        } else if (tempString.compareTo(TYPEEERESERVESTRING) == 0) {
            return TYPEEERESERVEINT;

        } else {
            ErrorInfoUtils.error("Incorrect type value (" + tempString + ")", columnIndex + rowIndex,
                ERRORMSGMAKESURETHATTHECELLCONTAINS + "either " + TYPEGDVARSTRING + ", " + TYPEGDARRAYSTRING + " or "
                    + TYPEGDRESERVESTRING);
            return Integer.MIN_VALUE;
        }
    }

    static boolean readDefaultData(boolean pFileHasBeenOpenedCorrectly, DocumentCreator pCreator,
        DefaultDataSheetPropertiesData pPropData) {

        if (!pFileHasBeenOpenedCorrectly) {
            return false;
        }

        DefaultDataInfo defaultDataInfo;

        ErrorInfoUtils.logn("Reading Default Data");

        // TODO: moved outside the method, check if this has any effect
        // // Make sure we're reading from the correct sheet first
        // mCurrentSheetUsed = mDefaultData;

        HSSFSheet currentSheet = mDefaultData;

        // Some initial cleaning
        pCreator.clearDefaultDataVector();
        ErrorInfoUtils.clearErrorInfo();

        // Some initial settings
        int currentWorkRow = pPropData.getStartRow();
        boolean doneReading = false;
        pPropData.setNbrOfCustomerRows(0);

        doneReading = CellsReader.isCellEmpty(currentWorkRow, pPropData, currentSheet);

        if (currentWorkRow > pPropData.getEndRow()) {
            doneReading = true;
        }

        while (!doneReading && !ErrorInfoUtils.errorHasOccured()) {

            // DefaultDataSheetProperties.checkIfUserWantsToBreak();

            defaultDataInfo = new DefaultDataInfo();
            // get row data
            readRowData(defaultDataInfo, currentWorkRow, currentSheet, pPropData);

            // Store the data in our vector
            pCreator.addDefaultData(defaultDataInfo);

            if (!ErrorInfoUtils.errorHasOccured()) {

                updateDocStats(defaultDataInfo, currentWorkRow, currentSheet, pPropData);
            }

            currentWorkRow++;
            // Is there anything more to read ?
            // First check if we're outside of the readable area. If we are, we better stop
            // since
            // the package clearly does not like us reading outside of borders... (Exception !!)

            doneReading = CellsReader.checkIfDoneReading(pPropData, currentSheet, currentWorkRow, doneReading);
        }
        final boolean resultOk = (doneReading && !ErrorInfoUtils.errorHasOccured());
        // NOTE: moved outside the method
        // if (resultOk) {
        // createBlockStorage(pBlockUsageInfo, pPropData);
        // pLogTxt.append(USERINFODONESTRING);
        // pLogTxt.append("\n");
        // }
        return resultOk;
    }

    private static void updateDocStats(DefaultDataInfo defaultDataInfo, int currentWorkRow,
        HSSFSheet pCurrentSheetUsed, DefaultDataSheetPropertiesData pPropData) {
        // Do some document fixing - needed for statistics
        if (defaultDataInfo.enabled) {
            updateDocumentStats(defaultDataInfo, currentWorkRow, pCurrentSheetUsed, pPropData);

            pPropData.getmDocumentInfo().nbrOfEnabledRows++;
        }
        pPropData.getmDocumentInfo().totalNbrOfRows++;
    }

    private static void readRowData(DefaultDataInfo defaultDataInfo, int currentWorkRow, HSSFSheet mCurrentSheetUsed,
        DefaultDataSheetPropertiesData mPropData) {

        defaultDataInfo.initialDataVec = new Vector<String>();

        // First figure out if the current row is used for this platform at all
        try {
            checkErrorHasOccured();
            defaultDataInfo.enabled =
                getPlatformEnabled(((PlatformsInfo) mPropData.getPlatformsInfo().elementAt(
                    mPropData.getSelectedPlatformIndex())).apply, currentWorkRow, mCurrentSheetUsed);

            // ID
            checkErrorHasOccured();
            defaultDataInfo.id =
                CellsReader.getPositiveNumberAtPosition(mPropData.getColumnDataInfo()[IDCOLUMN].column, currentWorkRow,
                    mCurrentSheetUsed);

            // Variable
            checkErrorHasOccured();
            defaultDataInfo.variable =
                CellsReader.getStringAtPosition(mPropData.getColumnDataInfo()[VARIABLECOLUMN].column, currentWorkRow,
                    mCurrentSheetUsed);

            // Responsible
            checkErrorHasOccured();
            defaultDataInfo.responsible =
                CellsReader.getStringAtPosition(mPropData.getColumnDataInfo()[RESPONSIBLECOLUMN].column,
                    currentWorkRow, mCurrentSheetUsed);

            // Block
            checkErrorHasOccured();
            defaultDataInfo.block =
                CellsReader.getPositiveNumberAtPosition(mPropData.getColumnDataInfo()[BLOCKCOLUMN].column,
                    currentWorkRow, mCurrentSheetUsed);

            // Let's keep track of maximum block number. It is useful later on....
            checkErrorHasOccured();
            if (defaultDataInfo.block > mPropData.getmDocumentInfo().maxFoundBlockNumber) {
                mPropData.getmDocumentInfo().maxFoundBlockNumber = defaultDataInfo.block;
            }

            // Module
            checkErrorHasOccured();
            defaultDataInfo.module =
                CellsReader.getStringAtPosition(mPropData.getColumnDataInfo()[MODULECOLUMN].column, currentWorkRow,
                    mCurrentSheetUsed);

            // Type
            checkErrorHasOccured();
            defaultDataInfo.type =
                readAndDetermineType(mPropData.getColumnDataInfo()[TYPECOLUMN].column, currentWorkRow,
                    mCurrentSheetUsed);

            // Size
            checkErrorHasOccured();
            defaultDataInfo.size =
                CellsReader.getPositiveNumberAtPosition(mPropData.getColumnDataInfo()[SIZECOLUMN].column,
                    currentWorkRow, mCurrentSheetUsed);

            // Array
            checkErrorHasOccured();
            String arrayStr =
                CellsReader.getStringAtPosition(mPropData.getColumnDataInfo()[ARRAYCOLUMN].column, currentWorkRow,
                    mCurrentSheetUsed);
            if (isThisStringConsideredBlank(arrayStr)) {
                defaultDataInfo.array = 0;
            } else {
                defaultDataInfo.array =
                    CellsReader.getPositiveNumberAtPosition(mPropData.getColumnDataInfo()[ARRAYCOLUMN].column,
                        currentWorkRow, mCurrentSheetUsed);
            }

            // Initial Data
            checkErrorHasOccured();
            if (defaultDataInfo.enabled) {
                DefaultDataInfo tempDDI = readInitialData(currentWorkRow, mPropData, mCurrentSheetUsed);
                defaultDataInfo.dataFoundInCellPos = tempDDI.dataFoundInCellPos;
                if (!ErrorInfoUtils.errorHasOccured()) {
                    defaultDataInfo.initialDataStr = tempDDI.initialDataStr;
                    defaultDataInfo.initialDataVec = tempDDI.initialDataVec;
                }
            }

            // Range
            checkErrorHasOccured();
            DefaultDataInfo tempDDI =
                readAndProcessRange(mPropData.getColumnDataInfo()[RANGECOLUMN].column, currentWorkRow,
                    mCurrentSheetUsed);
            if (!ErrorInfoUtils.errorHasOccured()) {
                defaultDataInfo.rangeLower = tempDDI.rangeLower;
                defaultDataInfo.rangeUpper = tempDDI.rangeUpper;
                defaultDataInfo.rangeText = tempDDI.rangeText;
            }

            // Customize
            checkErrorHasOccured();
            defaultDataInfo.customize =
                getYesOrNoValueFromPosition(mPropData.getColumnDataInfo()[CUSTOMIZECOLUMN].column, currentWorkRow,
                    mCurrentSheetUsed);

            // Calibrate
            checkErrorHasOccured();
            defaultDataInfo.calibrate =
                getYesOrNoValueFromPosition(mPropData.getColumnDataInfo()[CALIBRATECOLUMN].column, currentWorkRow,
                    mCurrentSheetUsed);

            // Short Description
            checkErrorHasOccured();
            defaultDataInfo.shortDesc =
                CellsReader.getStringAtPosition(mPropData.getColumnDataInfo()[SHORTDESCCOLUMN].column, currentWorkRow,
                    mCurrentSheetUsed);

            // Detailed Description
            checkErrorHasOccured();
            defaultDataInfo.detailedDesc =
                CellsReader.getStringAtPosition(mPropData.getColumnDataInfo()[DETAILDESCCOLUMN].column, currentWorkRow,
                    mCurrentSheetUsed);
        } catch (WizardException ex) {
            ex.getMessage();
            // do nothing, just leave method
        }

    }

    /**
     * TODO
     * 
     * @param defaultDataInfo
     * @param currentWorkRow
     */
    static void updateDocumentStats(DefaultDataInfo defaultDataInfo, int currentWorkRow, HSSFSheet pCurrentSheetUsed,
        DefaultDataSheetPropertiesData pPropData) {
        if (pPropData.getmDocumentInfo().firstRowNbr < 0) {
            pPropData.getmDocumentInfo().firstRowNbr = currentWorkRow;
        }

        if (currentWorkRow > pPropData.getmDocumentInfo().lastRowNbr) {
            pPropData.getmDocumentInfo().lastRowNbr = currentWorkRow;
        }

        if (defaultDataInfo.customize || defaultDataInfo.calibrate) {
            pPropData.setNbrOfCustomerRows(pPropData.getNbrOfCustomerRows() + 1);
        }

        if (useAlternateInitialDataColumn(currentWorkRow, pPropData, pCurrentSheetUsed)) {
            pPropData.getmDocumentInfo().nbrOfAlternateDataRows++;
        }
    }

    private static void checkErrorHasOccured() throws WizardException {

        if (ErrorInfoUtils.errorHasOccured()) {
            throw new WizardException("Error while reading data");
        }

    }

}
