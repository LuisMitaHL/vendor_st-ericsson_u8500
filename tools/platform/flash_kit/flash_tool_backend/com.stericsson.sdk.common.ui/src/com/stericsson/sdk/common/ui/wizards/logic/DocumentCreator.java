package com.stericsson.sdk.common.ui.wizards.logic;

import java.io.File;
import java.util.Vector;

/**
 * Creates output files basing on 19062 file.
 * 
 * 
 */
public class DocumentCreator implements IDefaultDataSheetConstants {

    private final GDFFile gdfFile;

    private final GDVARFile gdvarFile;

    private final EEVARFile eevarFile;

    private final Vector<DefaultDataInfo> mDefaultDataVector;

    private final Vector<GDFDataInfo> mGDFDataVector;

    private boolean mMergeProcessExecutedCorrectly;

    private boolean mIncludeCalibrationData = false;

    /**
     * Constructor.
     * 
     * @param pData
     *            Properties read out of 19062 document.
     */
    /* package */DocumentCreator(DefaultDataSheetPropertiesData pData) {

        mDefaultDataVector = new Vector<DefaultDataInfo>();
        mGDFDataVector = new Vector<GDFDataInfo>();

        gdfFile = new GDFFile(pData);
        gdvarFile = new GDVARFile(pData);
        eevarFile = new EEVARFile(pData);
    }

    /**
     * Gets result of merging files.
     * 
     * @return True if successful.
     */
    public boolean isMergeProcessExecutedCorrectly() {
        return mMergeProcessExecutedCorrectly;
    }

    /**
     * Returns default data vector
     * 
     * @return default data elements
     */
    public Vector<DefaultDataInfo> getDefaultDataVector() {
        return mDefaultDataVector;
    }

    /**
     * Clears the default data vector
     */
    public void clearDefaultDataVector() {
        mDefaultDataVector.clear();
    }

    /**
     * Adds default data to default data vector
     * 
     * @param info
     *            Default data info to be added
     */
    public void addDefaultData(DefaultDataInfo info) {
        mDefaultDataVector.add(info);
    }

    /**
     * @param info
     *            info
     */
    public void addGDFData(GDFDataInfo info) {
        mGDFDataVector.add(info);
    }

    /**
     * 
     */
    public void trimGDFData() {
        mGDFDataVector.trimToSize();
    }

    /**
     * 
     */
    public void clearGDFData() {
        mGDFDataVector.clear();
    }

    /**
     * @param eevarFileName
     *            Filename of the EEVAR file
     * @return True if successful
     */
    public boolean createEEVARFile(String eevarFileName) {
        return createEEVARFilesFromDataStore(eevarFileName, mDefaultDataVector, "");
    }

    /**
     * Method that converts a GDF file to an EEF file
     * 
     * @param src
     *            is the path to the .tmp GDF file
     * @param dest
     *            is the path to the Early Enumeration Customization data file (EEF)
     * @param deleteSource
     *            if the source file should be deleted after completion
     * @return false if not OK
     */
    public boolean createEEFFile(String src, String dest, boolean deleteSource) {
        createOutputFilePath(dest);
        return EEFFileUtils.createEEFFile(src, dest, deleteSource);
    }

    /**
     * A method that creates GDVAR and GDF files from data fed to the method. It also creates
     * appropriate header to both files If one of the filen names is empty the file is not opened.
     * 
     * @param eevarFileName
     *            Filenamne of the EEVAR file
     * @param dataToStore
     *            This is a vector that points to a DefaultDataInfo structure.
     * @param specialInfo
     *            If this string is not empty it is written into the header
     * @return Returns true if successful
     */
    private boolean createEEVARFilesFromDataStore(String eevarFileName, Vector<DefaultDataInfo> dataToStore,
        String specialInfo) {

        ErrorInfoUtils.logn("Creating EEVAR file");
        createOutputFilePath(eevarFileName);
        eevarFile.createEEVARFilesFromDataStore(eevarFileName, dataToStore, specialInfo);

        if (!ErrorInfoUtils.errorHasOccured()) {
            ErrorInfoUtils.logn(USERINFODONESTRING);
        }

        return !ErrorInfoUtils.errorHasOccured();
    }

    /**
     * @param pGdvarFileName
     *            Filename of the GDVAR file
     * @return True if successful
     */
    public boolean createGDVARFile(String pGdvarFileName) {
        return this.createGDVARFilesFromDataStore(pGdvarFileName, mDefaultDataVector, "");
    }

    /**
     * @param pGdfFileName
     *            Filename of the GDF file
     * @return Returns true if successful
     */
    public boolean createGDFFile(String pGdfFileName) {

        return createGDFFilesFromDataStore(pGdfFileName, mDefaultDataVector, "");
    }

    /**
     * A method that creates GDF files from default data fed to the method. It also creates
     * appropriate header to the files
     * 
     * @param pGdfFileName
     *            Filename of the GDF file
     * @param pDataToStore
     *            This is a vector that points to a xxx structure.
     * @param pSpecialInfo
     *            If this string is not empty it is written into the header
     * @return Returns true if successful
     * @see Se also CreateGDFFilesFromMergedData
     */
    private boolean createGDFFilesFromDataStore(String pGdfFileName, Vector<DefaultDataInfo> pDataToStore,
        String pSpecialInfo) {

        /*
         * if (!generatingEEFfile()) { mLogTxt.append("Creating GDF file"); mLogTxt.append("\n");
         * 
         * if (!mIncludeCalibrationDataSelected) { String[] message = new String[] {
         * "Calibration data from this file will overwrite and destroy",
         * "any existing calibration data in targets that this file is sent to." ,
         * "\nThis may seriously affect the performance of the target ME.",
         * "Do you want to include calibration data in the GDFS file?"};
         * 
         * MessageBox messageBox = new
         * MessageBox(PlatformUI.getWorkbench().getDisplay().getActiveShell(), SWT.ICON_QUESTION |
         * SWT.YES | SWT.NO); messageBox.setMessage(message[0] + message[1] + message[2] +
         * message[3]); messageBox.setText("Include calibration data");
         * 
         * mIncludeCalibrationData = (SWT.YES == messageBox.open()); mIncludeCalibrationDataSelected
         * = true; } }
         */
        ErrorInfoUtils.logn("Creating GDF file");

        createOutputFilePath(pGdfFileName);
        gdfFile.createGDFFilesFromDataStore(pGdfFileName, pDataToStore, pSpecialInfo, mIncludeCalibrationData);

        if (!ErrorInfoUtils.errorHasOccured()) {
            ErrorInfoUtils.logn(USERINFODONESTRING);
        }

        return !ErrorInfoUtils.errorHasOccured();
    }

    /**
     * A method that creates GDVAR and GDF files from data fed to the method. It also creates
     * appropriate header to both files If one of the filennames is empty the file is not opened.
     * 
     * @param GDFFileName
     *            Filename of the GDF file
     * @param pGdvarFileName
     *            File namne of the GDVAR file
     * @param pDataToStore
     *            This is a vector that points to a xxx structure.
     * @param pSpecialInfo
     *            If this string is not empty it is written into the header
     * @return Returns true if successful
     */
    private boolean createGDVARFilesFromDataStore(String pGdvarFileName, Vector<DefaultDataInfo> pDataToStore,
        String pSpecialInfo) {

        ErrorInfoUtils.logn("Creating GDVAR file");
        createOutputFilePath(pGdvarFileName);
        gdvarFile.createGDVARFilesFromDataStore(pGdvarFileName, pDataToStore, pSpecialInfo);

        if (!ErrorInfoUtils.errorHasOccured()) {
            ErrorInfoUtils.logn(USERINFODONESTRING);
        }

        return !ErrorInfoUtils.errorHasOccured();
    }

    /**
     * A method that creates GDF files from GDF data fed to the method. It also creates appropriate
     * header to the files
     * 
     * @param pGdfFileName
     *            Filename of the GDF file
     * @param pDataToStore
     *            This is a vector that points to a xxx structure.
     * @param pSpecialInfo
     *            If this string is not empty it is written into the header
     * @return Returns true if successful
     * @see Se also CreateGDFFilesFromDataStore
     */
    private boolean createGDFFilesFromMergedData(String pGdfFileName, Vector<GDFDataInfo> pDataToStore,
        String pSpecialInfo) {

        ErrorInfoUtils.logn("Creating merged GDF file");
        createOutputFilePath(pGdfFileName);
        gdfFile.createGDFFilesFromMergedData(pGdfFileName, pDataToStore, pSpecialInfo);

        if (!ErrorInfoUtils.errorHasOccured()) {
            ErrorInfoUtils.logn(IDefaultDataSheetConstants.USERINFODONESTRING);
        }

        return !ErrorInfoUtils.errorHasOccured();
    }

    /**
     * This method merges the data of a GDF file and a 19062 document. Prior to calling this
     * function it is assumed that ReadGDFFileFromDisk and DataConsistensyCheck has been executed,
     * i.e. that the data from the Excel file and the GDF file has already been read. The idea is
     * that data from the 19062 document always has priority, i.e. if both files has an item with
     * the same ID number, the 19062 data is selected.
     * 
     * @param pNewGDFFileName
     *            Filename of the GDF file that is the result of the merge
     * @param pLastReadGDFFileName
     *            Last read GDF file.
     * @param pExcelFileName
     *            Excel file name.
     * @param pGDFFileHasBeenReadCorrectly
     *            True if GDF has been read correctly.
     * @return True if the function succeeds
     */
    public boolean merge19062andGDFFileCreateNewGDFFile(String pNewGDFFileName, String pLastReadGDFFileName,
        String pExcelFileName, boolean pGDFFileHasBeenReadCorrectly) {
        boolean result = false;
        Vector<GDFDataInfo> mergedData = new Vector<GDFDataInfo>();
        ErrorInfoUtils.logn("Merging data");

        if (pGDFFileHasBeenReadCorrectly) {

            mergedData.addAll(mGDFDataVector);

            for (DefaultDataInfo defaultdata : mDefaultDataVector) {
                if (!(defaultdata.calibrate && !mIncludeCalibrationData)) {
                    GDFDataInfo[] gddata = GDFFile.convertToGDFDataFormat(defaultdata);
                    for (int z = 0; z < gddata.length; z++) {
                        if (mergedData.contains(gddata[z])) {
                            mergedData.setElementAt(gddata[z], mergedData.indexOf(gddata[z]));
                        } else {
                            mergedData.addElement(gddata[z]);
                        }
                    }
                }
            }

            // Okay, the two vectors are merged
            // Now we want to create just a new GDF file
            if (!ErrorInfoUtils.errorHasOccured()) {
                ErrorInfoUtils.logn(USERINFODONESTRING);
            }

            result =
                createGDFFilesFromMergedData(pNewGDFFileName, mergedData, "Result of merging " + pLastReadGDFFileName
                    + " and " + pExcelFileName);
            mMergeProcessExecutedCorrectly = (result && !ErrorInfoUtils.errorHasOccured());
        }
        return mMergeProcessExecutedCorrectly;
    }

    /**
     * Clears properties.
     */
    public void clearProperties() {
        mMergeProcessExecutedCorrectly = false;
        gdfFile.clearProperties();
        gdvarFile.clearProperties();
        eevarFile.clearProperties();
    }

    /**
     * Sets whether calibrate data.
     * 
     * @param pIncludeCalibrationData
     *            True if do calibrate.
     */
    public void setIncludeCalibrationData(boolean pIncludeCalibrationData) {
        mIncludeCalibrationData = pIncludeCalibrationData;
    }

    /**
     * Creates customer excel sheet of product.
     * 
     * @param pExcelFileName
     *            excel file name
     * @param pPropData
     *            properties data
     * @return true if successful, false otherwise
     */
    public boolean createCustomerDocument(String pExcelFileName, DefaultDataSheetPropertiesData pPropData) {
        ErrorInfoUtils.logn("Creating product 19062 Excel file");
        createOutputFilePath(pExcelFileName);
        return CustomerXLSDocCreator.create(pExcelFileName, getDefaultDataVector(), pPropData);
    }

    /**
     * Method that returns the type string from the type constant
     * 
     * @param typeNo
     *            The GD_VAR constant
     * @return The string that corresponds to the GD_VAR constant
     */
    static String getTypeString(int typeNo) {
        switch (typeNo) {
            case TYPEGDVARINT:
                return TYPEGDVARSTRING;
            case TYPEGDARRAYINT:
                return TYPEGDARRAYSTRING;
            case TYPEGDRESERVEINT:
                return TYPEGDRESERVESTRING;

            case TYPEEEVARINT:
                return TYPEEEVARSTRING;
            case TYPEEEARRAYINT:
                return TYPEEEARRAYSTRING;
            case TYPEEERESERVEINT:
                return TYPEEERESERVESTRING;

            default:
                return "";
        }
    }

    /**
     * Verifies that the output file is a file and that necessary folders are created in the file
     * path. If the file does not exists, and parent directory exists, the path is created.
     * 
     * @param outputFileName
     *            The fully qualified path of the output file.
     * @return true if output is ok to use, false otherwise.
     */
    private static void createOutputFilePath(String outputFileName) {
        File f = new File(outputFileName);
        if (f.exists()) {
            return;
        }
        File parentFile = f.getParentFile();
        if (parentFile != null) {
            parentFile.mkdirs();
        }
    }

}
