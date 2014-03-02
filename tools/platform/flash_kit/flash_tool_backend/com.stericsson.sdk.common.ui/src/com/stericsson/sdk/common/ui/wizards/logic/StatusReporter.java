package com.stericsson.sdk.common.ui.wizards.logic;

import java.io.File;
import java.io.OutputStream;
import java.io.PrintStream;
import java.io.UnsupportedEncodingException;
import java.util.Date;
import java.util.Vector;

/**
 * Creates report of process of creating files.
 * 
 * @author mielcluk
 * 
 */
public final class StatusReporter implements IDefaultDataSheetConstants {

    private StatusReporter() {
    };

    private static PrintStream mReportPrintStream;

    private static final File REPORTFILE = new File("ddreport.txt");

    private static Vector<BlockUsageInfo> mBlockUsageInfo;

    static void initializeBlockUsageInfo() {
        mBlockUsageInfo = new Vector<BlockUsageInfo>();
    }

    /**
     * @param maxAllowedDataSize
     * @param defaultDataInfo
     */
    static void updateBlockUsageInfo(int maxAllowedDataSize, DefaultDataInfo defaultDataInfo) {
        BlockUsageInfo blockUsageInfo = mBlockUsageInfo.elementAt(defaultDataInfo.block);
        blockUsageInfo.allocated = blockUsageInfo.allocated + maxAllowedDataSize;
        blockUsageInfo.used = blockUsageInfo.used + defaultDataInfo.initialDataVec.size();
        blockUsageInfo.nbrOfItems = blockUsageInfo.nbrOfItems + 1;
        blockUsageInfo.nbrCalibrated = blockUsageInfo.nbrCalibrated + (defaultDataInfo.calibrate ? 1 : 0);
        blockUsageInfo.nbrCustomize = blockUsageInfo.nbrCustomize + (defaultDataInfo.customize ? 1 : 0);
    }

    /**
     * This method creates the status report as a file on disc. If selected, it will be shown to the
     * user.
     * 
     * @param pOutputStream
     *            - Represents output stream for report file.
     */
    static void createStatusReport(OutputStream pOutputStream, DocumentInfo mDocumentInfo,
        boolean mMergeProcessExecutedCorrectly) {
        try {
            mReportPrintStream = new PrintStream(pOutputStream, false, "UTF-8");
        } catch (UnsupportedEncodingException e) {
            e.printStackTrace();
            return;
        }
        mReportPrintStream.println("**************************** DDW report ****************************");
        mReportPrintStream.println("Date: " + new Date(System.currentTimeMillis()).toString());
        mReportPrintStream.println("Default Data Wizard");
        mReportPrintStream.println("This report file is located at: " + REPORTFILE.getAbsolutePath());
        mReportPrintStream.println("OBSERVE: This report will be overwritten the next time the tool is executed!");
        mReportPrintStream.println("********************************************************************");
        mReportPrintStream.println("");
        mReportPrintStream.println("");
        mReportPrintStream.println("--- Document information ---");
        if (mDocumentInfo.infoIsValid) {
            mReportPrintStream.println("File processed : " + mDocumentInfo.fileName);
            mReportPrintStream.println("Selected platform : " + mDocumentInfo.selectedPlatform);
            mReportPrintStream.println("Largest blocknumber : " + mDocumentInfo.maxFoundBlockNumber);
            mReportPrintStream.println("Total number of rows : " + (mDocumentInfo.totalNbrOfRows));
            mReportPrintStream.println("Enabled number of rows : " + mDocumentInfo.nbrOfEnabledRows);
            mReportPrintStream.println("Number of rows using alternate data : " + mDocumentInfo.nbrOfAlternateDataRows);
            mReportPrintStream.println("First row number : " + mDocumentInfo.firstRowNbr);
            mReportPrintStream.println("Last row number : " + mDocumentInfo.lastRowNbr);
            mReportPrintStream.println("");
            mReportPrintStream.println("");
            mReportPrintStream.println("--- Block information ---");
            for (int i = 0; i < mBlockUsageInfo.size(); i++) {
                BlockUsageInfo blockUsageInfo = mBlockUsageInfo.elementAt(i);
                mReportPrintStream.println("Block number : " + i);
                mReportPrintStream.println("Bytes allocated : " + blockUsageInfo.allocated);
                mReportPrintStream.println("Bytes used : " + blockUsageInfo.used);
                mReportPrintStream.println("Number of items : " + blockUsageInfo.nbrOfItems);
                mReportPrintStream.println("Number of 'calibrated' rows : " + blockUsageInfo.nbrCalibrated);
                mReportPrintStream.println("Number of 'customer' rows : " + blockUsageInfo.nbrCustomize);
                mReportPrintStream.println("");
            }

            // TODO: this will NEVER occur ?! Someone messed this...
            // if (mMergeProcessExecutedCorrectly) {
            // mReportPrintStream.println("");
            // mReportPrintStream.println("--- Merge information ---");
            // mReportPrintStream.println(mMergeReportInfo.size() +
            // " items were affected as shown below");
            // for (int i = 0; i < mMergeReportInfo.size(); i++) {
            // MergeReportInfo mergeReportInfo = (MergeReportInfo) mMergeReportInfo.elementAt(i);
            // String eventDescription = "";
            // switch (mergeReportInfo.eventDescID) {
            // case EVENTDESCREPLACEDID:
            // eventDescription = EVENTDESCREPLACEDSTR;
            // break;
            // default:
            // eventDescription = "Unknown reason";
            // }
            // mReportPrintStream.println("ID " + mergeReportInfo.id + " : " + eventDescription);
            // }
            // }
        } else {
            mReportPrintStream.println("No valid document data to show !");
        }
        mReportPrintStream.println("");
        mReportPrintStream.println("");
        mReportPrintStream.println("********************* End of report *******************************");
        mReportPrintStream.println("");
        mReportPrintStream.println("");
        mReportPrintStream.flush();
        mReportPrintStream.close();
    }

    /**
     * This method create space for the block report. The method is intended to be called when
     * Default Data has been read since the number of different block (rather the biggest block
     * number) is known at this stage. This method then sets the size of the vector and fills it
     * with default zeros which is then appended as the data is checked.
     * 
     * @see DataConsistensyCheck
     */
    static void createBlockStorage(DefaultDataSheetPropertiesData pPropData) {
        mBlockUsageInfo.removeAllElements(); // Nice with garbage collection otherwise just this
        // row loose some memory...
        for (int i = 0; i <= pPropData.getmDocumentInfo().maxFoundBlockNumber; i++) {
            BlockUsageInfo tempBUI = new BlockUsageInfo();
            tempBUI.allocated = 0;
            tempBUI.nbrOfItems = 0;
            tempBUI.used = 0;
            tempBUI.nbrCalibrated = 0;
            tempBUI.nbrCustomize = 0;
            mBlockUsageInfo.add(i, tempBUI);
        }
    }
}
