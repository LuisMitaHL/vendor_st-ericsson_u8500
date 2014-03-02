package com.stericsson.sdk.common.ui.wizards.logic;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileNotFoundException;
import java.util.Vector;

import com.stericsson.sdk.common.gdfs.GDData;
import com.stericsson.sdk.common.gdfs.GdfParser;

// TEST

/**
 * This file contains all methods that are necessary to read a 19062 Excel sheet. The flow of events
 * is :
 * 
 * 1. Create an instance of this class 2. Open the 19062 document by calling
 * ReadDocumentDataSheetProperties(Filename) 3. Call ReadDefaultData() to read the document default
 * data 4. Call DataConsistensyCheck() to make sure that data is valid.
 * 
 * From this point, depending on what you want to do, you can
 * 
 * 5. Call CreateGDFAndGDVARFiles to create either GDF, GFVAR or both those files or 6. Call
 * CreateCustomerDocument to create a new customer Excel file or 7a. Call ReadGDFFileFromDisk to
 * read additional GDF data from disk 7b. and call Merge19062andGDFFileCreateNewGDFFile to create a
 * new GDF file based on the merging of the current 19062 Excel file and the newly read GDF file.
 * 
 * 8. Finally you can create and show a report if so desired by calling CreateAndShowStatusReport
 * 
 * Class for reading and managing the data that found in a 19062 Excel Default Data file.
 * 
 */
public final class DefaultDataSheetProperties implements IDefaultDataSheetConstants {

    // General properties

    private boolean mDefaultDataSheetHasBeenReadCorrectly;

    private String mExcelFileName;

    private String mLastReadGDFFileName;

    private DefaultDataSheetPropertiesData mPropData = new DefaultDataSheetPropertiesData();

    private DocumentCreator docCreator;

    /**
     * 
     */
    public DefaultDataSheetProperties() {
        initializeProperties();
    }

    /**
     * Creates and initializes crucial variables when the class is instantiated.
     */
    private void initializeProperties() {
        docCreator = new DocumentCreator(mPropData);

        StatusReporter.initializeBlockUsageInfo();
        mPropData.setDocumentInfo(new DocumentInfo());
        mPropData.setSelectedPlatformName("@Not set!!@");

        // Create storage space for the user comments which are at the to of the header and MAP
        // file
        mPropData.setUserCommentsInfo(new UserCommentsInfo[C_MAXNBROFUSERCOMMENTS]);
        for (int i = 0; i < C_MAXNBROFUSERCOMMENTS; i++) {
            mPropData.getUserCommentsInfo()[i] = new UserCommentsInfo();
        }

        // Create storage storage space for document data such as ID, Variable etc.
        mPropData.setmColumnDataInfo(new ColumnDataInfo[NBROFCOLUMNS]);
        for (int i = 0; i < NBROFCOLUMNS; i++) {
            mPropData.getColumnDataInfo()[i] = new ColumnDataInfo();
        }
        // Fill it with some information that is constant
        mPropData.getColumnDataInfo()[IDCOLUMN].name = "ID";
        mPropData.getColumnDataInfo()[IDCOLUMN].widthInExcelFile = 5;
        mPropData.getColumnDataInfo()[VARIABLECOLUMN].name = "Variable";
        mPropData.getColumnDataInfo()[VARIABLECOLUMN].widthInExcelFile = 59;
        mPropData.getColumnDataInfo()[RESPONSIBLECOLUMN].name = "Responsible";
        mPropData.getColumnDataInfo()[RESPONSIBLECOLUMN].widthInExcelFile = 12;
        mPropData.getColumnDataInfo()[BLOCKCOLUMN].name = "Block";
        mPropData.getColumnDataInfo()[BLOCKCOLUMN].widthInExcelFile = 8;
        mPropData.getColumnDataInfo()[MODULECOLUMN].name = "N/A"; // The customer should not
        // see the
        // true
        // column contents
        mPropData.getColumnDataInfo()[MODULECOLUMN].widthInExcelFile = 6;
        mPropData.getColumnDataInfo()[TYPECOLUMN].name = "Type";
        mPropData.getColumnDataInfo()[TYPECOLUMN].widthInExcelFile = 11;
        mPropData.getColumnDataInfo()[SIZECOLUMN].name = "Size";
        mPropData.getColumnDataInfo()[SIZECOLUMN].widthInExcelFile = 6;
        mPropData.getColumnDataInfo()[ARRAYCOLUMN].name = "Array";
        mPropData.getColumnDataInfo()[ARRAYCOLUMN].widthInExcelFile = 6;
        mPropData.getColumnDataInfo()[INITIALDATACOLUMN].name = "Initial data";
        mPropData.getColumnDataInfo()[INITIALDATACOLUMN].widthInExcelFile = 33;
        mPropData.getColumnDataInfo()[RANGECOLUMN].name = "Range";
        mPropData.getColumnDataInfo()[RANGECOLUMN].widthInExcelFile = 11;
        mPropData.getColumnDataInfo()[CUSTOMIZECOLUMN].name = "Customize";
        mPropData.getColumnDataInfo()[CUSTOMIZECOLUMN].widthInExcelFile = 11.5;
        mPropData.getColumnDataInfo()[CALIBRATECOLUMN].name = "Calibrate";
        mPropData.getColumnDataInfo()[CALIBRATECOLUMN].widthInExcelFile = 11.5;
        mPropData.getColumnDataInfo()[SHORTDESCCOLUMN].name = "Short description";
        mPropData.getColumnDataInfo()[SHORTDESCCOLUMN].widthInExcelFile = 68;
        mPropData.getColumnDataInfo()[DETAILDESCCOLUMN].name = "Detailed description";
        mPropData.getColumnDataInfo()[DETAILDESCCOLUMN].widthInExcelFile = 62;

        // Create the storage space for platforms used
        mPropData.clearPlatformsInfo();
        clearProperties();
    }

    /**
     * Method that sets important parameters to a known default value
     */
    private void clearProperties() {
        WorkbookReader.mFileHasBeenOpenedCorrectly = false;
        mDefaultDataSheetHasBeenReadCorrectly = false;

        docCreator.clearProperties();

        mExcelFileName = "";

        mPropData.getmDocumentInfo().firstRowNbr = -1;
        mPropData.getmDocumentInfo().lastRowNbr = -1;
        mPropData.getmDocumentInfo().maxFoundBlockNumber = -1;
        mPropData.getmDocumentInfo().nbrOfEnabledRows = 0;
        mPropData.getmDocumentInfo().nbrOfAlternateDataRows = 0;
        mPropData.getmDocumentInfo().totalNbrOfRows = 0;
        mPropData.getmDocumentInfo().infoIsValid = false;

        ErrorInfoUtils.clearErrorInfo();

        mPropData.setNbrOfCustomerRows(-1);

        for (int i = 0; i < C_MAXNBROFUSERCOMMENTS; i++) {
            mPropData.getUserCommentsInfo()[i].column1 = "";
            mPropData.getUserCommentsInfo()[i].column2 = "";
        }
        mPropData.getPlatformsInfo().removeAllElements();
    }

    /**
     * Reads the properties of the Document Data sheet in the workbook. This is the first stage that
     * should always be performed when accessing a 19062 document
     * 
     * @param ddsFileName
     *            The filename of the 19062 Excel file that is to be read
     * @return If the reading of the file contents was successful true is returned, if unsuccessful
     *         false is returned. For a more detailed description of what went wrong, examine the
     *         m_ErrorInfo parameter.
     */
    public boolean readDocumentDataSheetProperties(String ddsFileName) {
        // Clear properties first
        clearProperties();

        // NOTE: moved outside the method, check if it is required
        if (new File(ddsFileName).isFile()) {
            mExcelFileName = ddsFileName;
        }

        boolean result = WorkbookReader.readDocumentDataSheetProperties(ddsFileName, mPropData);

        return result;
    }

    // Unused?
    // /**
    // * Method that is used to "calculate" a new column index. If the method is fed with "B", 2 the
    // * result will be "D"
    // *
    // * @param ColumnChar
    // * A string from which the first character will be used for the calculation
    // * @param offset
    // * The number of steps forward or backward that the columnindex will be calculated
    // * @return The calculated return column
    // */
    // private String calculateNewColumn(String columnStr, int offset) {
    // return Character.valueOf((char) ((int) (columnStr.charAt(0)) + offset)).toString();
    // }

    /**
     * Method that checks if the Default Data is valid. The method is intended to be called after
     * default data has been read. Consistency checks consists of: -> That the number of default
     * data items is a multiple of the Size. -> That the number of default data items does not
     * exceed the maximum size, i.e. Size * Array -> That every default data value is within the
     * allowed range. If everything seems okay, the block report structure is updated.
     * 
     * @return True if all values passed the consistency check, otherwise false
     */
    private boolean dataConsistencyCheck() {
        boolean result = false;

        if (mDefaultDataSheetHasBeenReadCorrectly) {
            int maxAllowedDataSize = 0;
            ErrorInfoUtils.logn("Checking Data consistency");

            Vector<DefaultDataInfo> dataElements = docCreator.getDefaultDataVector();
            for (DefaultDataInfo defaultDataInfo : dataElements) {
                // while (dataElements.hasMoreElements() && (!ErrorInfoUtils.errorHasOccured())) {
                if (ErrorInfoUtils.errorHasOccured()) {
                    break;
                }
                // checkIfUserWantsToBreak();

                // DefaultDataInfo defaultDataInfo = dataElements.nextElement();

                maxAllowedDataSize = (1 > defaultDataInfo.array ? 1 : defaultDataInfo.array) * defaultDataInfo.size;

                dataConsistencyCheckSizes(maxAllowedDataSize, defaultDataInfo);

                // No idea to to this if this row is not enabled for the current platform
                if (defaultDataInfo.enabled && !ErrorInfoUtils.errorHasOccured()) {

                    // Make sure that all default data items are within the specified range
                    checkConsistencySizes(defaultDataInfo);

                    // Everything seems to be okay, so let's update our report
                    StatusReporter.updateBlockUsageInfo(maxAllowedDataSize, defaultDataInfo);

                }
            }
            result = !ErrorInfoUtils.errorHasOccured();
        }
        if (result) {
            ErrorInfoUtils.logn(USERINFODONESTRING);
            mPropData.getmDocumentInfo().selectedPlatform = mPropData.getSelectedPlatformName();
            mPropData.getmDocumentInfo().infoIsValid = true;
        }
        return result;
    }

    /**
     * Checks if data is in range.
     * 
     * @param defaultDataInfo
     */
    private void checkConsistencySizes(DefaultDataInfo defaultDataInfo) {
        int currValue = 0;
        String tempString;
        for (int i = 0; i < defaultDataInfo.initialDataVec.size(); i++) {
            try {
                tempString = (String) defaultDataInfo.initialDataVec.elementAt(i);
                currValue = Integer.decode(tempString).intValue();
            } catch (NumberFormatException e) {
                ErrorInfoUtils.error("Unexpected exception", defaultDataInfo.dataFoundInCellPos + " [ Item " + (i + 1)
                    + "]", "Contact support");
                break;
            }
            if (currValue > defaultDataInfo.rangeUpper) {
                ErrorInfoUtils.error("Value (" + tempString + ") in cell exceeds upper limit",
                    defaultDataInfo.dataFoundInCellPos + " [" + (i + 1) + "]", ERRORMSGMAKESURETHATTHECELLCONTAINS
                        + "value(s) below the upper limit (" + defaultDataInfo.rangeUpper + ")");
                break;
            } else if (currValue < defaultDataInfo.rangeLower) {
                ErrorInfoUtils.error("Value (" + tempString + ") in cell exceeds lower limit",
                    defaultDataInfo.dataFoundInCellPos + " [" + (i + 1) + "]", ERRORMSGMAKESURETHATTHECELLCONTAINS
                        + "value(s) above the lower limit (" + defaultDataInfo.rangeLower + ")");
                break;
            }
        }

    }

    private void dataConsistencyCheckSizes(int maxAllowedDataSize, DefaultDataInfo defaultDataInfo) {
        // Check that the number of default data items are exactly the number of the size *
        // array field
        if ((defaultDataInfo.size != maxAllowedDataSize) && (defaultDataInfo.type == TYPEGDVARINT)) {
            ErrorInfoUtils.error("Cell items mismatch. " + TYPEGDVARSTRING + " or  " + TYPEGDARRAYSTRING + " ?",
                defaultDataInfo.dataFoundInCellPos, "Adjust cell type or change "
                    + mPropData.getColumnDataInfo()[ARRAYCOLUMN].name + " value");
        }

        // Check that the number of default data items are exactly the number of the size *
        // array field
        if ((defaultDataInfo.initialDataVec.size() != maxAllowedDataSize)
            && (defaultDataInfo.initialDataVec.size() != 0) && (defaultDataInfo.type == TYPEGDARRAYINT)) {
            ErrorInfoUtils.error("Invalid number of items. " + defaultDataInfo.initialDataVec.size()
                + " found, should be " + maxAllowedDataSize, defaultDataInfo.dataFoundInCellPos,
                ERRORMSGMAKESURETHATTHECELLCONTAINS + "correct number of items");
        }

        // Check that the number of default data items does not exceed the allowed limit
        if (!ErrorInfoUtils.errorHasOccured()) {
            if (defaultDataInfo.initialDataVec.size() > maxAllowedDataSize) {
                ErrorInfoUtils.error("Cell contains too many values. (" + defaultDataInfo.initialDataVec.size()
                    + " items found)", defaultDataInfo.dataFoundInCellPos, ERRORMSGMAKESURETHATTHECELLCONTAINS
                    + "no more than " + maxAllowedDataSize + " item(s)");
            }
        }
    }

    /**
     * This method reads a GDF file from disc and stores in a struct/record data strcture in a
     * vector
     * 
     * @param pGDFFFileName
     *            Filename of the GDF file to read
     * @return If the operation succeeded the method returns true otherwise false
     */
    private boolean readGDFFileFromDisk(String pGDFFFileName) {
        ErrorInfoUtils.logn("Reading GDF file for merge");

        // Is there a filename at all ?
        if (pGDFFFileName.length() == 0) {
            return false;
        }

        mLastReadGDFFileName = pGDFFFileName;
        docCreator.clearGDFData();

        try {
            // Read GDFS variables from input file
            File gdfsInputFile = new File(pGDFFFileName);
            GdfParser parser = new GdfParser(gdfsInputFile);

            while ((parser.hasMoreData()) && (!ErrorInfoUtils.errorHasOccured())) {
                GDData gdData = parser.nextData();
                GDFDataInfo gdfDataInfo = new GDFDataInfo();
                if (gdData != null) {
                    gdfDataInfo.id = gdData.getIndex();
                    gdfDataInfo.size = gdData.getLength();
                    gdfDataInfo.block = gdData.getBlock();
                    Vector<String> data = new Vector<String>();
                    for (int i = 0; i < gdData.getLength(); i++) {
                        data.add(String.valueOf(gdData.getData()[i]));
                    }
                    gdfDataInfo.data = data;
                    docCreator.addGDFData(gdfDataInfo);
                }

                // checkIfUserWantsToBreak();
            }
        } catch (FileNotFoundException e) {
            ErrorInfoUtils.error("Unable to open file " + pGDFFFileName, "",
                "Make sure that the path and filename are valid.");
        } catch (Exception e) {
            ErrorInfoUtils.error("Reading file " + pGDFFFileName + " caused error " + e);
        }

        docCreator.trimGDFData();

        if (!ErrorInfoUtils.errorHasOccured()) {
            ErrorInfoUtils.logn(USERINFODONESTRING);
            return true;
        }

        return false;
    }

    /**
     * Method that reads default data from the 19062 file. This method is intended to be run after
     * the ReadDocumentDataSheetProperties has been executed correctly and will all data on the
     * Document Data sheet.
     * 
     * @return True if data was read from the file without problems
     */
    public boolean readDefaultData() {
        mDefaultDataSheetHasBeenReadCorrectly =
            WorkbookReader.readDefaultData(WorkbookReader.mFileHasBeenOpenedCorrectly, docCreator, mPropData);
        if (mDefaultDataSheetHasBeenReadCorrectly) {
            StatusReporter.createBlockStorage(mPropData);
            ErrorInfoUtils.logn(USERINFODONESTRING);
        }
        return mDefaultDataSheetHasBeenReadCorrectly;
    }

    /**
     * A method to set the platform to process.
     * 
     * @param index
     *            The index in the list given from GetAvailablePlatforms()
     * @param name
     *            The name of the platform to process.
     */
    public void setPlatformToProcess(int index, String name) {
        if (index < mPropData.getPlatformsInfo().size()) {
            mPropData.setSelectedPlatformIndex(index);
            mPropData.setSelectedPlatformName(name);
        }
    }

    /**
     * @return The name of the currently selected platform
     */
    public String getPlatformNameToProcess() {
        return mPropData.getSelectedPlatformName();
    }

    /**
     * Clears the user's previous selection whether calibration data should be included or not,
     * which means that the question will be asked again at next operation that creates a GDF file.
     */
    public void clearIncludeCalibrationDataSelection() {
        docCreator.setIncludeCalibrationData(false);
    }

    /**
     * @return available platforms
     */
    public Vector<PlatformsInfo> getAvailablePlatforms() {
        return mPropData.getPlatformsInfo();
    }

    /**
     * @return log message
     */
    public String getLogTxt() {
        return ErrorInfoUtils.getLogString();
    }

    /**
     * Creates report of publishing files.
     * 
     * @param pReport
     *            Stream for report info.
     */
    public void createStatusReport(ByteArrayOutputStream pReport) {
        StatusReporter.createStatusReport(pReport, mPropData.getmDocumentInfo(), docCreator
            .isMergeProcessExecutedCorrectly());
    }

    /**
     * Creates customer excel document.
     * 
     * @param pExcelFileName
     *            name of new customer excel file.
     * @return Result of creating file.
     */
    public boolean createCustomerDocument(String pExcelFileName) {
        return docCreator.createCustomerDocument(pExcelFileName, mPropData);
    }

    /**
     * Creates GDVAR file.
     * 
     * @param pGDVARFileName
     *            name of new GDVAR file.
     * @return Result of creating file.
     */
    public boolean createGDVARFile(String pGDVARFileName) {
        return docCreator.createGDVARFile(pGDVARFileName);

    }

    /**
     * Sets calibration data include flag for doc creator usage.
     * 
     * @param pIncludeCalibrationDataSelected
     *            new flag value
     */
    public void setDocCreatorIncludeCalibrationData(boolean pIncludeCalibrationDataSelected) {
        docCreator.setIncludeCalibrationData(pIncludeCalibrationDataSelected);
    }

    /**
     * This method merges the data of a GDF file and a 19062 document.
     * 
     * @param pGDFFileName
     *            GDF file name.
     * @param pInputGDFFileName
     *            Input gdf file name to be merged with gdf file
     * @return True if new file created.
     */
    public boolean merge19062andGDFFileCreateNewGDFFile(String pGDFFileName, String pInputGDFFileName) {
        final boolean fileReadCorrectly = readGDFFileFromDisk(pInputGDFFileName);
        if (!fileReadCorrectly) {
            return false;
        }
        return docCreator.merge19062andGDFFileCreateNewGDFFile(pGDFFileName, mLastReadGDFFileName, mExcelFileName,
            fileReadCorrectly);
    }

    /**
     * Creates GDF file.
     * 
     * @param pGDFFileName
     *            Filename of the GDF file
     * @return Returns true if successful
     */
    public boolean createGDFFile(String pGDFFileName) {
        return docCreator.createGDFFile(pGDFFileName);
    }

    /**
     * Creates EEVar file.
     * 
     * @param pEEVARFileName
     *            Filename of the EEVar file
     * @return Returns true if successful
     */
    public boolean createEEVARFile(String pEEVARFileName) {
        return docCreator.createEEVARFile(pEEVARFileName);

    }

    /**
     * Converts File from GDF to EEF.
     * 
     * @param pAbsolutePath
     *            Absolute path.
     * @param pEEFFileName
     *            EEF file name.
     * @param pDeleteSource
     *            Flag whether delete source file.
     * @return Returns true if successful
     */
    public boolean createEEFFile(String pAbsolutePath, String pEEFFileName, boolean pDeleteSource) {
        return docCreator.createEEFFile(pAbsolutePath, pEEFFileName, pDeleteSource);

    }

    /**
     * Checks consistency for selected platform
     * 
     * @return returns true if the check has passed, false otherwise
     */
    public boolean consistencyCheckForSelectedPlatform() {
        boolean resultIsOk = true;

        if (resultIsOk) {
            resultIsOk = readDefaultData();
        }

        if (resultIsOk) {
            resultIsOk = dataConsistencyCheck();
        }

        return resultIsOk;
    }

}
