package com.stericsson.sdk.common.ui.wizards.logic;

import java.io.BufferedOutputStream;
import java.io.DataOutputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.Calendar;
import java.util.Vector;

/**
 * Utilities to manage EEVAR files
 * 
 * @author kapalpaw
 * 
 */
public class EEVARFile {

    private DefaultDataSheetPropertiesData mPropertiesData;

    private DataOutputStream mEEVARFile;

    private boolean mEEVARFileOpenedOk;

    private String mEEVARFileName;

    /**
     * Default constructor
     * 
     * @param pData
     *            properties data
     */
    public EEVARFile(DefaultDataSheetPropertiesData pData) {
        mPropertiesData = pData;
    }

    /**
     * A method that creates GDVAR and GDF files from data fed to the method. It also creates
     * appropriate header to both files If one of the filennames is empty the file is not opened.
     * 
     * @param eevarFileName
     *            Filenamne of the EEVAR file
     * @param dataToStore
     *            This is a vector that points to a xxx structure.
     * @param specialInfo
     *            If this string is not empty it is written into the header
     */
    public void createEEVARFilesFromDataStore(String eevarFileName, Vector<DefaultDataInfo> dataToStore,
        String specialInfo) {

        mEEVARFileName = eevarFileName;
        String strWriteToEEVARFile = "";

        // First of all we need to create the files
        // EEVAR file (header file) with extension .h
        try {
            mEEVARFile = new DataOutputStream(new BufferedOutputStream(new FileOutputStream(eevarFileName)));
            mEEVARFileOpenedOk = true;
        } catch (FileNotFoundException e) {
            ErrorInfoUtils.error("Unable to create file " + eevarFileName, "",
                "Make sure that the path and filename are valid");
        }

        if (!ErrorInfoUtils.errorHasOccured()) {
            // So far so good, now start writing some data to the files

            // Write some headers for both file types

            writeToEEVARFileHeaders(specialInfo);

            // Okay, headers done... Now start pushing down data to the files.
            // We start off with the EEVAR file
            writeToEEVARFileData(dataToStore, strWriteToEEVARFile);
        }

        // Finally close the files
        closeEEVARFile();
    }

    /**
     * TODO
     * 
     * @param pSpecialInfo
     */
    private void writeToEEVARFileHeaders(String pSpecialInfo) {
        writeToVARFile(IDefaultDataSheetConstants.FILEHEADER1);
        writeToVARFile(IDefaultDataSheetConstants.FILEHEADER2);
        for (int i = 0; i < IDefaultDataSheetConstants.C_MAXNBROFUSERCOMMENTS; i++) {
            writeToVARFile(IDefaultDataSheetConstants.FILEHEADER2 + IDefaultDataSheetConstants.FILESPACING
                + mPropertiesData.getUserCommentsInfo()[i].column1 + IDefaultDataSheetConstants.FILECOLON
                + mPropertiesData.getUserCommentsInfo()[i].column2);
            writeToVARFile(IDefaultDataSheetConstants.FILEHEADER2);
        }

        Calendar tempCal = Calendar.getInstance();

        writeToVARFile(IDefaultDataSheetConstants.FILEHEADER2 + IDefaultDataSheetConstants.FILESPACING
            + "Created by : Default Data Sheet Tool Wizard");
        // Platform.getVersion()
        writeToVARFile(IDefaultDataSheetConstants.FILEHEADER2 + IDefaultDataSheetConstants.FILESPACING
            + "Created    : " + tempCal.getTime().toString());
        writeToVARFile(IDefaultDataSheetConstants.FILEHEADER2 + IDefaultDataSheetConstants.FILESPACING
            + "User       : " + System.getProperty("user.name"));
        writeToVARFile(IDefaultDataSheetConstants.FILEHEADER2 + IDefaultDataSheetConstants.FILESPACING
            + "Product    : " + mPropertiesData.getSelectedPlatformName());
        if (pSpecialInfo.length() != 0) {
            writeToVARFile(IDefaultDataSheetConstants.FILEHEADER2 + IDefaultDataSheetConstants.FILESPACING
                + "Special    : " + pSpecialInfo);
        }

        writeToVARFile(IDefaultDataSheetConstants.FILEHEADER2);
        writeToVARFile(IDefaultDataSheetConstants.FILEHEADER1);
        writeToVARFile("");
        writeToVARFile(IDefaultDataSheetConstants.EEVARFILEHEADER1);
        writeToVARFile(IDefaultDataSheetConstants.GDVARFILEHEADER2); // same for EE
    }

    /**
     * TODO
     * 
     * @param dataToStore
     * @param strWriteToEEVARFile
     */
    private void writeToEEVARFileData(Vector<DefaultDataInfo> dataToStore, String strWriteToEEVARFile) {
        int counter = 0;
        while ((counter < dataToStore.size()) && (!ErrorInfoUtils.errorHasOccured())) {
//            DefaultDataSheetProperties.checkIfUserWantsToBreak();
            DefaultDataInfo defaultDataInfo = dataToStore.elementAt(counter++);

            // First build the string to write for the GDVAR file

            if (!mEEVARFileOpenedOk) {
                continue;
            }
            if (defaultDataInfo.enabled) {
                strWriteToEEVARFile =
                    StringTools.padRight(DocumentCreator.getTypeString(defaultDataInfo.type), 12);
            } else {
                strWriteToEEVARFile =
                    StringTools.padRight(DocumentCreator
                        .getTypeString(IDefaultDataSheetConstants.TYPEEERESERVEINT), 12);
            }
            strWriteToEEVARFile += "(";
            strWriteToEEVARFile += StringTools.padLeft(String.valueOf(defaultDataInfo.id), 4);
            strWriteToEEVARFile += ",";
            if (defaultDataInfo.enabled) {
                strWriteToEEVARFile += StringTools.padLeft(String.valueOf(defaultDataInfo.block), 2);
                strWriteToEEVARFile += ", ";
                strWriteToEEVARFile += StringTools.padRight(defaultDataInfo.variable, 68);
                strWriteToEEVARFile += ",";
                strWriteToEEVARFile += StringTools.padLeft(String.valueOf(defaultDataInfo.size), 6);
            } else {
                if (defaultDataInfo.type == IDefaultDataSheetConstants.TYPEGDARRAYINT) {
                    strWriteToEEVARFile += StringTools.padLeft(String.valueOf(defaultDataInfo.array), 2);
                    strWriteToEEVARFile += StringTools.padLeft(" ", 6);
                } else {
                    strWriteToEEVARFile += StringTools.padLeft("1", 2);
                }
                strWriteToEEVARFile += StringTools.padLeft(" ", 77);
            }
            switch (defaultDataInfo.type) {
                case IDefaultDataSheetConstants.TYPEGDVARINT:
                case IDefaultDataSheetConstants.TYPEGDRESERVEINT:
                    strWriteToEEVARFile +=
                        IDefaultDataSheetConstants.FILELONGSPACING + IDefaultDataSheetConstants.FILESHORTSPACING;
                    break;
                case IDefaultDataSheetConstants.TYPEGDARRAYINT:
                    if (defaultDataInfo.enabled) {
                        strWriteToEEVARFile += "," + StringTools.padLeft(String.valueOf(defaultDataInfo.array), 5);
                    }
                    break;
                default:
            }
            strWriteToEEVARFile += ")  // ";
            strWriteToEEVARFile += defaultDataInfo.responsible;
            writeToVARFile(strWriteToEEVARFile);
            // Done with EEVAR file
            /*
             * }
             * 
             * counter++;
             */
        }
    }

    private void closeEEVARFile() {

        if (mEEVARFileOpenedOk) {
            try {
                writeToVARFile(IDefaultDataSheetConstants.EEVARFILEFOOTER1);
                mEEVARFile.close();
            } catch (IOException e) {
                ErrorInfoUtils.error("Unable to close file " + mEEVARFileName);
            }

            mEEVARFileOpenedOk = false; // Don't try to close it again
        }

    }

    /**
     * Method that writes a string to the GD_VAR file if it was successfully opened
     * 
     * @param pData
     *            The string that should be written to the GDVAR file
     */
    private void writeToVARFile(String pData) {
        if ((mEEVARFileOpenedOk) && (!ErrorInfoUtils.errorHasOccured())) {
            try {
                mEEVARFile.writeBytes(pData + IDefaultDataSheetConstants.FILENEWLINE);
            } catch (IOException e) {
                ErrorInfoUtils.error("Unable to write (" + pData + ") to file " + mEEVARFileName);
            }
        }
    }

    /**
     * Clears properties.
     */
    public void clearProperties() {
        mEEVARFileOpenedOk = false;
    }

}
