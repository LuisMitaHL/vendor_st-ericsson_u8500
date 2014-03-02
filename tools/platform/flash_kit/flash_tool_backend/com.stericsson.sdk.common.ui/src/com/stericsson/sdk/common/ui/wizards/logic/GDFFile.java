package com.stericsson.sdk.common.ui.wizards.logic;

import java.io.BufferedOutputStream;
import java.io.DataOutputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.Calendar;
import java.util.Collections;
import java.util.Iterator;
import java.util.Vector;

import com.stericsson.sdk.common.gdfs.GDData;

/**
 * Utilities to manage GDF files
 * 
 * @author kapalpaw
 * 
 */
public class GDFFile {

    private DefaultDataSheetPropertiesData mPropertiesData;

    private DataOutputStream mGDFFile;

    private String mGDFFileName;

    private boolean mGDFFileOpenedOk;

    /**
     * Default constructor
     * 
     * @param pData
     *            properties data
     */
    public GDFFile(DefaultDataSheetPropertiesData pData) {
        mPropertiesData = pData;
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
     * @param pIncludeCalibrationData
     *            include calibration data
     * @see Se also CreateGDFFilesFromMergedData
     */
    public void createGDFFilesFromDataStore(String pGdfFileName, Vector<DefaultDataInfo> pDataToStore,
        String pSpecialInfo, boolean pIncludeCalibrationData) {

        mGDFFileName = pGdfFileName;

        // First of all we need to create the files
        // GDF file (MAP file) with extension .map
        mGDFFileOpenedOk = false;

        try {
            mGDFFile = new DataOutputStream(new BufferedOutputStream(new FileOutputStream(pGdfFileName)));
            mGDFFileOpenedOk = true;
        } catch (FileNotFoundException e) {
            ErrorInfoUtils.error("Unable to create file " + pGdfFileName, "",
                "Make sure that the path and filename are valid");
        }

        if (!ErrorInfoUtils.errorHasOccured()) {
            // So far so good, now start writing some data to the file

            // Write some headers
            writeGDFFileHeaders(pSpecialInfo);

            // Okay, headers done... Now start pushing down data to the files.
            // We start off with the GDVAR file and then we continue with the
            // GDF file

            if (mGDFFileOpenedOk) {
                writeGDFFileData(pDataToStore, pIncludeCalibrationData);
            }
        }

        // Finally close the files
        closeGDFFile();
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
     * @see Se also CreateGDFFilesFromDataStore
     */
    public void createGDFFilesFromMergedData(String pGdfFileName, Vector<GDFDataInfo> pDataToStore, String pSpecialInfo) {

        mGDFFileName = pGdfFileName;

        // First of all we need to create the files
        // GDF file (MAP file) with extension .map
        mGDFFileOpenedOk = false;

        try {
            mGDFFile = new DataOutputStream(new BufferedOutputStream(new FileOutputStream(pGdfFileName)));
            mGDFFileOpenedOk = true;
        } catch (FileNotFoundException e) {
            ErrorInfoUtils.error("Unable to create file " + pGdfFileName, "",
                "Make sure that the path and filename are valid");
        }

        if (!ErrorInfoUtils.errorHasOccured()) {
            // So far so good, now start writing some data to the file

            // Write some headers
            writeToGDFFile(IDefaultDataSheetConstants.FILEHEADER1);
            writeToGDFFile(IDefaultDataSheetConstants.FILEHEADER2);

            for (int i = 0; i < IDefaultDataSheetConstants.C_MAXNBROFUSERCOMMENTS; i++) {
                writeToGDFFile(IDefaultDataSheetConstants.FILEHEADER2 + IDefaultDataSheetConstants.FILESPACING
                    + mPropertiesData.getUserCommentsInfo()[i].column1 + IDefaultDataSheetConstants.FILECOLON
                    + mPropertiesData.getUserCommentsInfo()[i].column2);
                writeToGDFFile(IDefaultDataSheetConstants.FILEHEADER2);
            }

            Calendar tempCal = Calendar.getInstance();

            writeToGDFFile(IDefaultDataSheetConstants.FILEHEADER2 + IDefaultDataSheetConstants.FILESPACING
                + "Created by : Default Data Sheet Tool Wizard");
            // Platform.getVersion()
            writeToGDFFile(IDefaultDataSheetConstants.FILEHEADER2 + IDefaultDataSheetConstants.FILESPACING
                + "Created    : " + tempCal.getTime().toString());
            writeToGDFFile(IDefaultDataSheetConstants.FILEHEADER2 + IDefaultDataSheetConstants.FILESPACING
                + "User       : " + System.getProperty("user.name"));
            if (pSpecialInfo.length() != 0) {
                writeToGDFFile(IDefaultDataSheetConstants.FILEHEADER2 + IDefaultDataSheetConstants.FILESPACING
                    + "Special    : " + pSpecialInfo);
            }

            writeToGDFFile(IDefaultDataSheetConstants.FILEHEADER2);
            writeToGDFFile(IDefaultDataSheetConstants.FILEHEADER1);
            writeToGDFFile("");

            // Okay, headers done... Now start pushing down data to the file.
            // Since we get this from the merge operation the data to store
            // vector only contains
            // T_GDFDataInfo
            Iterator<GDFDataInfo> i = pDataToStore.iterator();
            Vector<GDData> v = new Vector<GDData>();

            while ((i.hasNext()) && (!ErrorInfoUtils.errorHasOccured())) {
                v.addElement(convertToGDData(i.next()));
            }

            Collections.sort(v);
            Iterator<GDData> it = v.iterator();

            while ((it.hasNext()) && (!ErrorInfoUtils.errorHasOccured())) {
                GDData gd = it.next();
                if (gd.getLength() > 0) {
                    writeToGDFFile(gd.toString());
                }
            }
        }

        // Finally close the files
        closeGDFFile();
    }

    /**
     * Method that writes a string to the GDF file if it was successfully opened
     * 
     * @param pData
     *            The string that should be written to the GDF file
     */
    private void writeToGDFFile(String pData) {
        if ((mGDFFileOpenedOk) && (!ErrorInfoUtils.errorHasOccured())) {
            try {
                mGDFFile.writeBytes(pData + IDefaultDataSheetConstants.FILENEWLINE);
            } catch (IOException e) {
                ErrorInfoUtils.error("Unable to write (" + pData + ") to file " + mGDFFileName);
            }
        }
    }

    /**
     * TODO
     * 
     * @param pDataToStore
     */
    private void writeGDFFileData(Vector<DefaultDataInfo> pDataToStore, boolean pIncludeCalibrationData) {
        Iterator<DefaultDataInfo> i = pDataToStore.iterator();
        Vector<GDData> v = new Vector<GDData>();

        while ((i.hasNext()) && (!ErrorInfoUtils.errorHasOccured())) {
            DefaultDataInfo defaultdata = i.next();
            if (!(defaultdata.calibrate && !pIncludeCalibrationData)) {
                // If we have some actual data to write... Let's do it !
                if (defaultdata.initialDataVec.size() > 0) {
                    GDFDataInfo[] gdData = GDFFile.convertToGDFDataFormat(defaultdata);
                    for (int x = 0; x < gdData.length; x++) {
                        v.addElement(convertToGDData(gdData[x]));
                    }
                }
            }
//            DefaultDataSheetProperties.checkIfUserWantsToBreak(); // If so, will
            // set error
            // that
            // ends loop
        }
        Collections.sort(v);
        Iterator<GDData> it = v.iterator();

        while ((it.hasNext()) && (!ErrorInfoUtils.errorHasOccured())) {
            GDData gd = it.next();
            if (gd.getLength() > 0) {
                writeToGDFFile(gd.toString());
            }
//            DefaultDataSheetProperties.checkIfUserWantsToBreak(); // If so, will
            // set error
            // that
            // ends loop
        }
    }

    /**
     * TODO
     * 
     * @param pSpecialInfo
     */
    private void writeGDFFileHeaders(String pSpecialInfo) {
        writeToGDFFile(IDefaultDataSheetConstants.FILEHEADER1);
        writeToGDFFile(IDefaultDataSheetConstants.FILEHEADER2);
        for (int i = 0; i < IDefaultDataSheetConstants.C_MAXNBROFUSERCOMMENTS; i++) {
            writeToGDFFile(IDefaultDataSheetConstants.FILEHEADER2 + IDefaultDataSheetConstants.FILESPACING
                + mPropertiesData.getUserCommentsInfo()[i].column1 + IDefaultDataSheetConstants.FILECOLON
                + mPropertiesData.getUserCommentsInfo()[i].column2);
            writeToGDFFile(IDefaultDataSheetConstants.FILEHEADER2);
        }
        Calendar tempCal = Calendar.getInstance();
        writeToGDFFile(IDefaultDataSheetConstants.FILEHEADER2 + IDefaultDataSheetConstants.FILESPACING
            + "Created by : Default Data Sheet Tool Wizard");
        // Platform.getVersion()
        writeToGDFFile(IDefaultDataSheetConstants.FILEHEADER2 + IDefaultDataSheetConstants.FILESPACING
            + "Created    : " + tempCal.getTime().toString());
        writeToGDFFile(IDefaultDataSheetConstants.FILEHEADER2 + IDefaultDataSheetConstants.FILESPACING
            + "User       : " + System.getProperty("user.name"));
        writeToGDFFile(IDefaultDataSheetConstants.FILEHEADER2 + IDefaultDataSheetConstants.FILESPACING
            + "Product    : " + mPropertiesData.getSelectedPlatformName());
        if (pSpecialInfo.length() != 0) {
            writeToGDFFile(IDefaultDataSheetConstants.FILEHEADER2 + IDefaultDataSheetConstants.FILESPACING
                + "Special    : " + pSpecialInfo);
        }
        writeToGDFFile(IDefaultDataSheetConstants.FILEHEADER2);
        writeToGDFFile(IDefaultDataSheetConstants.FILEHEADER1);
        writeToGDFFile("");
    }

    /**
     * Converts an instance of <code>T_GDFDataInfo</code> to an instance of <code>GDData</code>.
     * 
     * @param pInfo
     *            The <code>T_GDFDataInfo</code> to "copy".
     * @return The GDData object which is a "copy" of the argument.
     */
    private GDData convertToGDData(GDFDataInfo pInfo) {
        GDData data = new GDData();
        data.setIndex(pInfo.id);
        data.setBlock(pInfo.block);
        byte[] abData = new byte[pInfo.data.size()];
        if (abData.length > 0) {
            for (int x = 0; x < abData.length; x++) {
                String value = pInfo.data.elementAt(x);
                int val = Integer.decode(value).intValue();
                abData[x] = (byte) (val & 0xFF);
            }
            data.setData(abData);
        }
        return data;
    }

    /**
     * Clears properties.
     */
    public void clearProperties() {
        mGDFFileOpenedOk = false;
    }

    /**
     * Method that closes GD_VAR and GDF files if they were successfully opened. (It also appends a
     * string to the GD_VAR file before closing it)
     */
    private void closeGDFFile() {
        if (mGDFFileOpenedOk) {
            try {
                mGDFFile.close();
            } catch (IOException e) {
                ErrorInfoUtils.error("Unable to close file " + mGDFFileName);
            }

            mGDFFileOpenedOk = false; // Don't try to close it again
        }
    }

    /**
     * Converts an instance of T_DefaultDataInof that can be read from the 19062 specification
     * document and creates the neccessary T_GDFDataInfo object to reflect the specification. Since
     * the specification can contain GDFS arrays in one entry it must be converted into several
     * instances of T_GDFDataInfo.
     * 
     * @param pDefaultdata
     *            The specification document entry to convert.
     * @return One or more GDFS instances to reflect the given default data entry.
     */
    /* package */static GDFDataInfo[] convertToGDFDataFormat(DefaultDataInfo pDefaultdata) {
        GDFDataInfo[] gddata = null;
        if (pDefaultdata.type == IDefaultDataSheetConstants.TYPEGDARRAYINT && (pDefaultdata.initialDataVec.size() > 0)) {
            int nbrUnits = pDefaultdata.array;
            gddata = new GDFDataInfo[nbrUnits];
            for (int x = 0; x < nbrUnits; x++) {
                gddata[x] = new GDFDataInfo();
                gddata[x].id = pDefaultdata.id + x;
                gddata[x].size = pDefaultdata.size;
                gddata[x].block = pDefaultdata.block;
                Vector<String> tmp = pDefaultdata.initialDataVec;
                int firstDataItem = (tmp.size() / nbrUnits) * x;
                Vector<String> array = new Vector<String>();
                for (int p = 0; p < pDefaultdata.size; p++) {
                    array.addElement(tmp.elementAt(firstDataItem + p));
                }
                gddata[x].data = array;
            }

        } else {
            gddata = new GDFDataInfo[1];
            gddata[0] = new GDFDataInfo();
            gddata[0].id = pDefaultdata.id;
            gddata[0].size = pDefaultdata.size;
            gddata[0].block = pDefaultdata.block;
            gddata[0].data = pDefaultdata.initialDataVec;
        }
        return gddata;
    }
}
