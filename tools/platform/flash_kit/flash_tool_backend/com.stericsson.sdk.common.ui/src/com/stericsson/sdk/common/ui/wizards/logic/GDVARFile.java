package com.stericsson.sdk.common.ui.wizards.logic;

import java.io.BufferedOutputStream;
import java.io.DataOutputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.Calendar;
import java.util.Vector;

/**
 * Utilities to manage GDVAR files
 * 
 * @author kapalpaw
 * 
 */
public class GDVARFile {

    private DataOutputStream mGDVARFile;

    private boolean mGDVARFileOpenedOk;

    private String mGDVARFileName;

    private DefaultDataSheetPropertiesData mPropertiesData;

    /**
     * Default constructor
     * 
     * @param pData
     *            properties data
     */
    public GDVARFile(DefaultDataSheetPropertiesData pData) {
        mPropertiesData = pData;
    }

    /**
     * TODO
     * 
     * @param pDataToStore
     */
    private void writeToGDVARFileData(Vector<DefaultDataInfo> pDataToStore) {
        String strWriteToGDVARFile;
        int counter = 0;
        while ((counter < pDataToStore.size()) && (!ErrorInfoUtils.errorHasOccured())) {
//            DefaultDataSheetProperties.checkIfUserWantsToBreak();

            DefaultDataInfo defaultDataInfo = pDataToStore.elementAt(counter);

            // First build the string to write for the GDVAR file
            if (mGDVARFileOpenedOk) {
                if (defaultDataInfo.enabled) {
                    strWriteToGDVARFile =
                        StringTools.padRight(DocumentCreator.getTypeString(defaultDataInfo.type), 12);
                } else {
                    strWriteToGDVARFile =
                        StringTools.padRight(DocumentCreator
                            .getTypeString(IDefaultDataSheetConstants.TYPEGDRESERVEINT), 12);
                }
                strWriteToGDVARFile += "(";
                strWriteToGDVARFile += StringTools.padLeft(String.valueOf(defaultDataInfo.id), 4);
                strWriteToGDVARFile += ",";
                if (defaultDataInfo.enabled) {
                    strWriteToGDVARFile += StringTools.padLeft(String.valueOf(defaultDataInfo.block), 2);
                    strWriteToGDVARFile += ", ";
                    strWriteToGDVARFile += StringTools.padRight(defaultDataInfo.variable, 68);
                    strWriteToGDVARFile += ",";
                    strWriteToGDVARFile += StringTools.padLeft(String.valueOf(defaultDataInfo.size), 6);
                } else {
                    if (defaultDataInfo.type == IDefaultDataSheetConstants.TYPEGDARRAYINT) {
                        strWriteToGDVARFile += StringTools.padLeft(String.valueOf(defaultDataInfo.array), 2);
                        strWriteToGDVARFile += StringTools.padLeft(" ", 6);
                    } else {
                        strWriteToGDVARFile += StringTools.padLeft("1", 2);
                    }
                    strWriteToGDVARFile += StringTools.padLeft(" ", 77);
                }
                switch (defaultDataInfo.type) {
                    case IDefaultDataSheetConstants.TYPEGDVARINT:
                    case IDefaultDataSheetConstants.TYPEGDRESERVEINT:
                        strWriteToGDVARFile +=
                            IDefaultDataSheetConstants.FILELONGSPACING + IDefaultDataSheetConstants.FILESHORTSPACING;
                        break;
                    case IDefaultDataSheetConstants.TYPEGDARRAYINT:
                        if (defaultDataInfo.enabled) {
                            strWriteToGDVARFile += "," + StringTools.padLeft(String.valueOf(defaultDataInfo.array), 5);
                        }
                        break;
                    default:
                }
                strWriteToGDVARFile += ")  // ";
                strWriteToGDVARFile += defaultDataInfo.responsible;
                writeToVARFile(strWriteToGDVARFile);
                // Done with GDVAR file
            }

            counter++;
        }
    }

    /**
     * TODO
     * 
     * @param pSpecialInfo
     */
    private void writeToGDVARFileHeaders(String pSpecialInfo) {
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
        writeToVARFile(IDefaultDataSheetConstants.GDVARFILEHEADER1);
        writeToVARFile(IDefaultDataSheetConstants.GDVARFILEHEADER2);
    }

    /**
     * A method that creates GDVAR and GDF files from data fed to the method. It also creates
     * appropriate header to both files If one of the filennames is empty the file is not opened.
     * 
     * @param pGdvarFileName
     *            File name of the GDVAR file
     * @param pDataToStore
     *            This is a vector that points to a xxx structure.
     * @param pSpecialInfo
     *            If this string is not empty it is written into the header
     */
    public void createGDVARFilesFromDataStore(String pGdvarFileName, Vector<DefaultDataInfo> pDataToStore,
        String pSpecialInfo) {

        mGDVARFileName = pGdvarFileName;
        // First of all we need to create the files
        // GDVAR file (header file) with extension .h
        try {
            mGDVARFile = new DataOutputStream(new BufferedOutputStream(new FileOutputStream(pGdvarFileName)));
            mGDVARFileOpenedOk = true;
        } catch (FileNotFoundException e) {
            ErrorInfoUtils.error("Unable to create file " + pGdvarFileName, "",
                "Make sure that the path and filename are valid");
        }

        if (!ErrorInfoUtils.errorHasOccured()) {
            // So far so good, now start writing some data to the files

            // Write some headers for both file types
            writeToGDVARFileHeaders(pSpecialInfo);

            // Okay, headers done... Now start pushing down data to the files.
            // We start off with the GDVAR file

            writeToGDVARFileData(pDataToStore);
        }

        // Finally close the files
        closeGDVARFile();
    }

    private void closeGDVARFile() {
        if (mGDVARFileOpenedOk) {
            try {
                writeToVARFile(IDefaultDataSheetConstants.GDVARFILEFOOTER1);
                mGDVARFile.close();
            } catch (IOException e) {
                ErrorInfoUtils.error("Unable to close file " + mGDVARFileName);
            }

            mGDVARFileOpenedOk = false; // Don't try to close it again
        }
    }

    /**
     * Method that writes a string to the GD_VAR file if it was successfully opened
     * 
     * @param pData
     *            The string that should be written to the GDVAR file
     */
    private void writeToVARFile(String pData) {
        if ((mGDVARFileOpenedOk) && (!ErrorInfoUtils.errorHasOccured())) {
            try {
                mGDVARFile.writeBytes(pData + IDefaultDataSheetConstants.FILENEWLINE);
            } catch (IOException e) {
                ErrorInfoUtils.error("Unable to write (" + pData + ") to file " + mGDVARFileName);
            }

        }
    }

    /**
     * Clears properties.
     */
    public void clearProperties() {
        mGDVARFileOpenedOk = false;
    }

}
