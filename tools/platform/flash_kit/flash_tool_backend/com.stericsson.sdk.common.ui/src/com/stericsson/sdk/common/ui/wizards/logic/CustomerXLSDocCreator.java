package com.stericsson.sdk.common.ui.wizards.logic;

import java.io.FileOutputStream;
import java.util.Vector;

import org.apache.poi.hssf.usermodel.HSSFCell;
import org.apache.poi.hssf.usermodel.HSSFCellStyle;
import org.apache.poi.hssf.usermodel.HSSFFont;
import org.apache.poi.hssf.usermodel.HSSFPalette;
import org.apache.poi.hssf.usermodel.HSSFRow;
import org.apache.poi.hssf.usermodel.HSSFSheet;
import org.apache.poi.hssf.usermodel.HSSFWorkbook;
import org.apache.poi.hssf.util.HSSFColor;
import org.apache.poi.hssf.util.Region;

/**
 * Creates customer excel sheet.
 * 
 * 
 */
public final class CustomerXLSDocCreator implements IDefaultDataSheetConstants {

    private int mStartRow;

    private int mNbrOfCustomerRows;

    private ColumnDataInfo[] mColumnDataInfo;

    private Vector<PlatformsInfo> mPlatformsInfo;

    private Vector<DefaultDataInfo> mDefaultDataVector;

    private int mSelectedPlatformIndex;

    private UserCommentsInfo[] mUserCommentsInfo;

    // Document styles

    private HSSFCellStyle mItalicSmallStyle;

    private HSSFCellStyle mBoldRedStyle;

    private HSSFCellStyle mBoldBlueStyle;

    private HSSFCellStyle mHeaderStyle;

    private HSSFCellStyle mDataHeaderStyle;

    private HSSFCellStyle mDataHeaderStyleNoBorder;

    private HSSFCellStyle mDataCellWithBorderCenterAlign;

    private HSSFCellStyle mDataCellWithBorderLeftAlign;

    private HSSFCellStyle mDataCellWithBorderLeftAlignWWrap;

    private HSSFCellStyle mDataCellWithBorderRightAlign;

    /**
     * Constructor.
     * 
     */
    private CustomerXLSDocCreator(Vector<DefaultDataInfo> pDefaultDataVector, DefaultDataSheetPropertiesData pData) {
        mStartRow = pData.getStartRow();
        mNbrOfCustomerRows = pData.getNbrOfCustomerRows();
        mColumnDataInfo = pData.getColumnDataInfo();
        mPlatformsInfo = pData.getPlatformsInfo();

        mDefaultDataVector = pDefaultDataVector;

        // NOTE: unneeded vector rewrite deleted
        // mDefaultDataVector = new Vector();
        // Vector<DefaultDataInfo> defaultData = pCreator.getDefaultDataVector(); // for
        // (DefaultDataInfo ddi : defaultData) { // mDefaultDataVector.add(ddi); // }
        mSelectedPlatformIndex = pData.getSelectedPlatformIndex();
        mUserCommentsInfo = pData.getUserCommentsInfo();

    }

    /**
     * Creates customer excel sheet of product.
     * 
     * @param pExcelFileName
     *            excel file name
     * @param pDefaultDataSheetHasPassedConsistencyCheck
     *            has data sheet passed consistency check?
     * @param pDefaultDataVector
     *            default data vector
     * @param pPropData
     *            properties data
     * @return true if successful, false otherwise
     */
    /* package */static boolean create(String pExcelFileName, Vector<DefaultDataInfo> pDefaultDataVector,
        DefaultDataSheetPropertiesData pPropData) {
        CustomerXLSDocCreator creator =
            new CustomerXLSDocCreator(pDefaultDataVector, pPropData);
        return creator.createCustomerDocument(pExcelFileName);
    }

    /**
     * 
     * Creates customer excel sheet of product.
     * 
     * @param pFileName
     *            Name of new file.
     * @return Returns true if successful.
     */
    @SuppressWarnings("deprecation")
    private boolean createCustomerDocument(String pFileName) {

        boolean result = false;
        HSSFPalette colorPalette = null;
        HSSFCell tempCell = null;
        HSSFRow tempRow = null;
        // HSSFFont tempFont = null;
        int currentRow = 0;
        int currentCol = 1;

        // This method contains a lot of formatting code (styles etc) which makes it rather big.....
        try {
            // First create the workbook and the sheets
            HSSFWorkbook newCustWorkBook = new HSSFWorkbook();
            colorPalette = newCustWorkBook.getCustomPalette();
            HSSFSheet documentDataSheet = newCustWorkBook.createSheet(DOCUMENTDATASHEETNAME);
            HSSFSheet defaultDataSheet = newCustWorkBook.createSheet(DEFAULTDATASHEETNAME);

            // Make sure that the columns have a nice width...

            for (int i = 1; i < 6; i++) {
                documentDataSheet.setColumnWidth((short) i, (short) 4800); // Approx 18
            }

            createFontStyles(colorPalette, newCustWorkBook);

            // First let's concentrate on the Document Data page

            currentRow = 0;
            documentDataSheet.createRow((short) currentRow); // Empty row
            tempRow = documentDataSheet.createRow((short) currentCol);
            currentRow++;

            // The 'header' cell requires some work...phew..

            createHeader(colorPalette, tempCell, tempRow, currentRow, currentCol, newCustWorkBook, documentDataSheet);
            currentRow++;
            // Done with the 'header' cell

            documentDataSheet.createRow(currentRow); // Empty row
            currentRow++;

            // Document type
            currentRow = createDcoumentType(currentRow, currentCol, documentDataSheet);

            // Startrow
            currentRow = createStartRow(currentRow, currentCol, documentDataSheet);

            // Endrow
            currentRow = createEndRow(currentRow, currentCol, documentDataSheet);

            // ID, variable, etc...
            // NB ! It is seems to be important that the Module data is not visible
            // so we have to constantly remove that throughout the rest of the
            // process....
            for (int i = IDCOLUMN; i <= DETAILDESCCOLUMN; i++) {
                tempRow = documentDataSheet.createRow(currentRow);
                currentRow++;
                tempCell = tempRow.createCell((short) currentCol);
                tempCell.setCellValue(mColumnDataInfo[i].name);
                tempCell.setCellStyle(mItalicSmallStyle);
                tempCell = tempRow.createCell((short) (currentCol + 1));
                if (i <= RANGECOLUMN) {
                    tempCell.setCellValue(Character.valueOf((char) (i + 65)).toString());
                } else {
                    tempCell.setCellValue(Character.valueOf((char) (i + 66)).toString());
                }
                tempCell.setCellStyle(mBoldRedStyle);
            }

            // Platforms used
            currentRow = createPlatformsUsed(currentRow, currentCol, documentDataSheet);

            // Apply column
            currentRow = applyColumn(currentRow, currentCol, documentDataSheet);

            // Alternate Data
            currentRow = createAlternateData(currentRow, currentCol, documentDataSheet);

            // User comments
            createUserComments(currentRow, currentCol, documentDataSheet);

            // ------------------------
            // Now let's concentrate on the Default Data sheet
            // First create some usable fonts
            createDataSheetHeaderSyles(colorPalette, newCustWorkBook);

            currentRow = 0;

            currentRow = createDataSheetHeader(currentRow, defaultDataSheet);
            currentRow = currentRow + 2;

            // The header is now done. Let's start filling the sheet with actual data
            // but first make a nice cell style
            createDataSheetStyles(colorPalette, newCustWorkBook);

            int counter = 0;

            fillInDataSheet(currentRow, defaultDataSheet, counter);

            // Write the output to a file
            FileOutputStream fileOnDisc = new FileOutputStream(pFileName);
            newCustWorkBook.write(fileOnDisc);
            fileOnDisc.close();
            result = true;
        } catch (Exception e) {
            ErrorInfoUtils.error("Exception occured while creating file " + pFileName);
        }

        if (result) {
            ErrorInfoUtils.logn(USERINFODONESTRING);
        }
        return result;
    }

    @SuppressWarnings("deprecation")
    private void fillInDataSheet(int currentRow, HSSFSheet defaultDataSheet, int counter) {
        HSSFCell tempCell;
        HSSFRow tempRow;
        int currentCol;
        while (counter < mDefaultDataVector.size()) {

            // DefaultDataSheetProperties.checkIfUserWantsToBreak();

            DefaultDataInfo defaultDataInfo = mDefaultDataVector.elementAt(counter);

            if ((defaultDataInfo.customize || defaultDataInfo.calibrate) && (defaultDataInfo.enabled)) {

                tempRow = defaultDataSheet.createRow((short) (currentRow));
                currentCol = 0;

                // ID
                tempCell = tempRow.createCell((short) currentCol);
                tempCell.setCellValue(defaultDataInfo.id);
                tempCell.setCellStyle(mDataCellWithBorderRightAlign);
                currentCol++;

                // Variable
                tempCell = tempRow.createCell((short) currentCol);
                tempCell.setCellValue(defaultDataInfo.variable);
                tempCell.setCellStyle(mDataCellWithBorderLeftAlign);
                currentCol++;

                // Responsible
                tempCell = tempRow.createCell((short) currentCol);
                tempCell.setCellValue(defaultDataInfo.responsible);
                // TempCell.setCellValue( "" );//Leave empty, internal responsible cannot be
                // responsible for product document.
                tempCell.setCellStyle(mDataCellWithBorderLeftAlign);
                currentCol++;

                // Block
                tempCell = tempRow.createCell((short) currentCol);
                tempCell.setCellValue(Integer.toString(defaultDataInfo.block));
                tempCell.setCellStyle(mDataCellWithBorderCenterAlign);
                currentCol++;

                // Module - Not to be seen by customer
                // Just create empty space
                tempCell = tempRow.createCell((short) currentCol);
                tempCell.setCellValue("");
                tempCell.setCellStyle(mDataCellWithBorderLeftAlign);
                currentCol++;

                // Type
                tempCell = tempRow.createCell((short) currentCol);
                tempCell.setCellValue(DocumentCreator.getTypeString(defaultDataInfo.type));
                tempCell.setCellStyle(mDataCellWithBorderLeftAlign);
                currentCol++;

                // Size
                tempCell = tempRow.createCell((short) currentCol);
                tempCell.setCellValue(defaultDataInfo.size);
                tempCell.setCellStyle(mDataCellWithBorderRightAlign);
                currentCol++;

                // Array
                tempCell = tempRow.createCell((short) currentCol);
                if (defaultDataInfo.array != 0) {
                    tempCell.setCellValue(defaultDataInfo.array);
                }
                tempCell.setCellStyle(mDataCellWithBorderRightAlign);
                currentCol++;

                // Initial data
                tempCell = tempRow.createCell((short) currentCol);
                tempCell.setCellValue(defaultDataInfo.initialDataStr);
                tempCell.setCellStyle(mDataCellWithBorderLeftAlignWWrap);
                currentCol++;

                // Range
                tempCell = tempRow.createCell((short) currentCol);

                String tempStr = defaultDataInfo.rangeText;

                tempCell.setCellValue(tempStr);
                tempCell.setCellStyle(mDataCellWithBorderCenterAlign);
                currentCol++;

                // Platform
                tempCell = tempRow.createCell((short) currentCol);
                if (defaultDataInfo.enabled) {
                    tempCell.setCellValue(PLATFORMUSEDEFAULTDATACOLUMN);
                }
                tempCell.setCellStyle(mDataCellWithBorderCenterAlign);
                currentCol++;

                // Customize
                tempCell = tempRow.createCell((short) currentCol);
                if (defaultDataInfo.customize) {
                    tempCell.setCellValue(YESABBREVIATION);
                } else {
                    tempCell.setCellValue(NOABBREVIATION);
                }
                tempCell.setCellStyle(mDataCellWithBorderCenterAlign);
                currentCol++;

                // Calibrate
                tempCell = tempRow.createCell((short) currentCol);
                if (defaultDataInfo.calibrate) {
                    tempCell.setCellValue(YESABBREVIATION);
                } else {
                    tempCell.setCellValue(NOABBREVIATION);
                }
                tempCell.setCellStyle(mDataCellWithBorderCenterAlign);
                currentCol++;

                // Short description
                tempCell = tempRow.createCell((short) currentCol);
                tempCell.setCellValue(defaultDataInfo.shortDesc);
                tempCell.setCellStyle(mDataCellWithBorderLeftAlignWWrap);
                currentCol++;

                // Detailed description
                tempCell = tempRow.createCell((short) currentCol);
                tempCell.setCellValue(defaultDataInfo.detailedDesc);
                tempCell.setCellStyle(mDataCellWithBorderLeftAlignWWrap);

                currentRow++;
            }
            counter++;
        }
    }

    private void createDataSheetStyles(HSSFPalette colorPalette, HSSFWorkbook newCustWorkBook) {
        HSSFFont tempFont;
        tempFont = newCustWorkBook.createFont();
        tempFont.setFontHeightInPoints((short) 10);
        tempFont.setFontName("Arial");
        tempFont.setColor(colorPalette.findColor((byte) 0x00, (byte) 0x00, (byte) 0x00).getIndex()/*
                                                                                                   * HSSFCellStyle.
                                                                                                   * BLACK
                                                                                                   */);
        mDataCellWithBorderCenterAlign = newCustWorkBook.createCellStyle();
        mDataCellWithBorderCenterAlign.setFont(tempFont);
        mDataCellWithBorderCenterAlign.setBorderBottom(HSSFCellStyle.BORDER_THIN);
        mDataCellWithBorderCenterAlign.setBottomBorderColor(colorPalette.findColor((byte) 0x00, (byte) 0x00,
            (byte) 0x00).getIndex()/* HSSFCellStyle.BLACK */);
        mDataCellWithBorderCenterAlign.setBorderTop(HSSFCellStyle.BORDER_THIN);
        mDataCellWithBorderCenterAlign.setTopBorderColor(colorPalette.findColor((byte) 0x00, (byte) 0x00, (byte) 0x00)
            .getIndex()/* HSSFCellStyle.BLACK */);
        mDataCellWithBorderCenterAlign.setBorderLeft(HSSFCellStyle.BORDER_THIN);
        mDataCellWithBorderCenterAlign.setLeftBorderColor(colorPalette.findColor((byte) 0x00, (byte) 0x00, (byte) 0x00)
            .getIndex()/* HSSFCellStyle.BLACK */);
        mDataCellWithBorderCenterAlign.setBorderRight(HSSFCellStyle.BORDER_THIN);
        mDataCellWithBorderCenterAlign.setRightBorderColor(colorPalette
            .findColor((byte) 0x00, (byte) 0x00, (byte) 0x00).getIndex()/* HSSFCellStyle.BLACK */);
        mDataCellWithBorderCenterAlign.setAlignment(HSSFCellStyle.ALIGN_CENTER);
        mDataCellWithBorderCenterAlign.setVerticalAlignment(HSSFCellStyle.VERTICAL_TOP);

        mDataCellWithBorderLeftAlign = newCustWorkBook.createCellStyle();
        mDataCellWithBorderLeftAlign.setFont(tempFont);
        mDataCellWithBorderLeftAlign.setBorderBottom(HSSFCellStyle.BORDER_THIN);
        mDataCellWithBorderLeftAlign.setBottomBorderColor(colorPalette.findColor((byte) 0x00, (byte) 0x00, (byte) 0x00)
            .getIndex()/* HSSFCellStyle.BLACK */);
        mDataCellWithBorderLeftAlign.setBorderTop(HSSFCellStyle.BORDER_THIN);
        mDataCellWithBorderLeftAlign.setTopBorderColor(colorPalette.findColor((byte) 0x00, (byte) 0x00, (byte) 0x00)
            .getIndex()/* HSSFCellStyle.BLACK */);
        mDataCellWithBorderLeftAlign.setBorderLeft(HSSFCellStyle.BORDER_THIN);
        mDataCellWithBorderLeftAlign.setLeftBorderColor(colorPalette.findColor((byte) 0x00, (byte) 0x00, (byte) 0x00)
            .getIndex()/* HSSFCellStyle.BLACK */);
        mDataCellWithBorderLeftAlign.setBorderRight(HSSFCellStyle.BORDER_THIN);
        mDataCellWithBorderLeftAlign.setRightBorderColor(colorPalette.findColor((byte) 0x00, (byte) 0x00, (byte) 0x00)
            .getIndex()/* HSSFCellStyle.BLACK */);
        mDataCellWithBorderLeftAlign.setAlignment(HSSFCellStyle.ALIGN_LEFT);
        mDataCellWithBorderLeftAlign.setVerticalAlignment(HSSFCellStyle.VERTICAL_TOP);

        mDataCellWithBorderLeftAlignWWrap = newCustWorkBook.createCellStyle();
        mDataCellWithBorderLeftAlignWWrap.setFont(tempFont);
        mDataCellWithBorderLeftAlignWWrap.setBorderBottom(HSSFCellStyle.BORDER_THIN);
        mDataCellWithBorderLeftAlignWWrap.setBottomBorderColor(colorPalette.findColor((byte) 0x00, (byte) 0x00,
            (byte) 0x00).getIndex()/* HSSFCellStyle.BLACK */);
        mDataCellWithBorderLeftAlignWWrap.setBorderTop(HSSFCellStyle.BORDER_THIN);
        mDataCellWithBorderLeftAlignWWrap.setTopBorderColor(colorPalette.findColor((byte) 0x00, (byte) 0x00,
            (byte) 0x00).getIndex()/* HSSFCellStyle.BLACK */);
        mDataCellWithBorderLeftAlignWWrap.setBorderLeft(HSSFCellStyle.BORDER_THIN);
        mDataCellWithBorderLeftAlignWWrap.setLeftBorderColor(colorPalette.findColor((byte) 0x00, (byte) 0x00,
            (byte) 0x00).getIndex()/* HSSFCellStyle.BLACK */);
        mDataCellWithBorderLeftAlignWWrap.setBorderRight(HSSFCellStyle.BORDER_THIN);
        mDataCellWithBorderLeftAlignWWrap.setRightBorderColor(colorPalette.findColor((byte) 0x00, (byte) 0x00,
            (byte) 0x00).getIndex()/* HSSFCellStyle.BLACK */);
        mDataCellWithBorderLeftAlignWWrap.setWrapText(true);
        mDataCellWithBorderLeftAlignWWrap.setAlignment((short) (HSSFCellStyle.ALIGN_LEFT));
        mDataCellWithBorderLeftAlignWWrap.setVerticalAlignment(HSSFCellStyle.VERTICAL_TOP);

        mDataCellWithBorderRightAlign = newCustWorkBook.createCellStyle();
        mDataCellWithBorderRightAlign.setFont(tempFont);
        mDataCellWithBorderRightAlign.setBorderBottom(HSSFCellStyle.BORDER_THIN);
        mDataCellWithBorderRightAlign.setBottomBorderColor(colorPalette
            .findColor((byte) 0x00, (byte) 0x00, (byte) 0x00).getIndex()/* HSSFCellStyle.BLACK */);
        mDataCellWithBorderRightAlign.setBorderTop(HSSFCellStyle.BORDER_THIN);
        mDataCellWithBorderRightAlign.setTopBorderColor(colorPalette.findColor((byte) 0x00, (byte) 0x00, (byte) 0x00)
            .getIndex()/* HSSFCellStyle.BLACK */);
        mDataCellWithBorderRightAlign.setBorderLeft(HSSFCellStyle.BORDER_THIN);
        mDataCellWithBorderRightAlign.setLeftBorderColor(colorPalette.findColor((byte) 0x00, (byte) 0x00, (byte) 0x00)
            .getIndex()/* HSSFCellStyle.BLACK */);
        mDataCellWithBorderRightAlign.setBorderRight(HSSFCellStyle.BORDER_THIN);
        mDataCellWithBorderRightAlign.setRightBorderColor(colorPalette.findColor((byte) 0x00, (byte) 0x00, (byte) 0x00)
            .getIndex()/* HSSFCellStyle.BLACK */);
        mDataCellWithBorderRightAlign.setAlignment(HSSFCellStyle.ALIGN_RIGHT);
        mDataCellWithBorderRightAlign.setVerticalAlignment(HSSFCellStyle.VERTICAL_TOP);
    }

    @SuppressWarnings("deprecation")
    private int createDataSheetHeader(int currentRow, HSSFSheet defaultDataSheet) {
        HSSFCell tempCell;
        HSSFRow tempRow;
        int currentCol;
        defaultDataSheet.createRow(currentRow); // Empty row
        currentRow++;
        tempRow = defaultDataSheet.createRow(currentRow);
        for (int i = 0; i <= 10; i++) {
            tempCell = tempRow.createCell((short) i);
            tempCell.setCellStyle(mHeaderStyle);
        }
        defaultDataSheet.addMergedRegion(new Region(currentRow, (short) 0, currentRow, (short) 10));
        currentRow++;
        currentRow++;
        tempCell = tempRow.getCell((short) 0);
        tempCell.setCellValue("Manufacturer Instruction and Data");
        tempCell.setCellStyle(mHeaderStyle);

        tempRow = defaultDataSheet.createRow(currentRow);
        HSSFRow tempRow1 = defaultDataSheet.createRow((currentRow + 1));
        currentCol = 0;
        // ID, variable, etc...
        for (int i = IDCOLUMN; i <= DETAILDESCCOLUMN; i++) {
            if (i == CUSTOMIZECOLUMN) {
                // Make some extra room for the one platform
                tempCell = tempRow.createCell((short) currentCol);
                tempCell.setCellValue("Platform");
                tempCell.setCellStyle(mDataHeaderStyle);
                tempCell = tempRow1.createCell((short) currentCol);
                tempCell.setCellValue(((PlatformsInfo) mPlatformsInfo.elementAt(mSelectedPlatformIndex)).name);
                tempCell.setCellStyle(mDataHeaderStyleNoBorder);
                defaultDataSheet.setColumnWidth((short) currentCol, (short) (9 * CONVERSIONFACTOREXCELCOL));
                currentCol++;
            }
            // The Module MKRR should not be visible
            tempCell = tempRow.createCell((short) currentCol);
            tempCell.setCellValue(mColumnDataInfo[i].name);
            tempCell.setCellStyle(mDataHeaderStyle);
            tempCell = tempRow1.createCell((short) currentCol);
            tempCell.setCellValue("");
            tempCell.setCellStyle(mDataHeaderStyleNoBorder);
            defaultDataSheet.setColumnWidth((short) currentCol,
                (short) (mColumnDataInfo[i].widthInExcelFile * CONVERSIONFACTOREXCELCOL));
            currentCol++;
        }

        currentCol = 0;
        for (int i = IDCOLUMN; i <= DETAILDESCCOLUMN; i++) {
            if (i != CUSTOMIZECOLUMN) {
                defaultDataSheet.addMergedRegion(new Region(currentRow, (short) currentCol, (currentRow + 1),
                    (short) currentCol));
            }
            currentCol++;
        }
        return currentRow;
    }

    private void createDataSheetHeaderSyles(HSSFPalette colorPalette, HSSFWorkbook newCustWorkBook) {
        HSSFFont tempFont;
        tempFont = newCustWorkBook.createFont();
        tempFont.setFontHeightInPoints((short) 10);
        tempFont.setFontName("Arial");
        tempFont.setBoldweight(HSSFFont.BOLDWEIGHT_BOLD);
        tempFont.setColor(colorPalette.findColor((byte) 0x00, (byte) 0x00, (byte) 0x00).getIndex()/*
                                                                                                   * HSSFCellStyle.
                                                                                                   * BLACK
                                                                                                   */);
        mDataHeaderStyle = newCustWorkBook.createCellStyle();
        mDataHeaderStyle.setFont(tempFont);
        // DataHeaderStyle.setFillForegroundColor( colorPalette.findColor( (byte)0xFF,
        // (byte)0xFF, (byte)0x90 ).getIndex() );// HSSFCellStyle.LIGHT_YELLOW);
        mDataHeaderStyle.setFillForegroundColor(new HSSFColor.LIGHT_YELLOW().getIndex());
        mDataHeaderStyle.setFillPattern(HSSFCellStyle.SOLID_FOREGROUND);
        mDataHeaderStyle.setAlignment((short) (HSSFCellStyle.ALIGN_CENTER));
        mDataHeaderStyle.setBorderTop(HSSFCellStyle.BORDER_DOUBLE);

        tempFont = newCustWorkBook.createFont();
        tempFont.setFontHeightInPoints((short) 10);
        tempFont.setFontName("Arial");
        tempFont.setColor(colorPalette.findColor((byte) 0x00, (byte) 0x00, (byte) 0xFF).getIndex()/*
                                                                                                   * HSSFCellStyle.
                                                                                                   * BLUE
                                                                                                   */);
        mDataHeaderStyleNoBorder = newCustWorkBook.createCellStyle();
        mDataHeaderStyleNoBorder.setFont(tempFont);
        // DataHeaderStyleNoBorder.setFillForegroundColor( colorPalette.findColor(
        // (byte)0xFF, (byte)0xFF, (byte)0x9C ).getIndex() );//HSSFCellStyle.LIGHT_YELLOW);
        mDataHeaderStyleNoBorder.setFillForegroundColor(new HSSFColor.LIGHT_YELLOW().getIndex());
        mDataHeaderStyleNoBorder.setFillPattern(HSSFCellStyle.SOLID_FOREGROUND);
        mDataHeaderStyleNoBorder.setAlignment(HSSFCellStyle.ALIGN_CENTER);
        mDataHeaderStyleNoBorder.setAlignment(HSSFCellStyle.VERTICAL_CENTER);
    }

    @SuppressWarnings("deprecation")
    private void createUserComments(int currentRow, int currentCol, HSSFSheet documentDataSheet) {
        HSSFCell tempCell;
        HSSFRow tempRow;
        for (int i = 0; i < C_MAXNBROFUSERCOMMENTS; i++) {
            tempRow = documentDataSheet.createRow(currentRow);
            currentRow++;
            tempCell = tempRow.createCell((short) currentCol);
            tempCell.setCellValue(mUserCommentsInfo[i].column1);
            tempCell.setCellStyle(mBoldBlueStyle);
            tempCell = tempRow.createCell((short) (currentCol + 1));
            if (mUserCommentsInfo[i].column1.indexOf("resp") == -1) {
                tempCell.setCellValue(mUserCommentsInfo[i].column2);
            } else {
                tempCell.setCellValue("");
            }
            tempCell.setCellStyle(mBoldBlueStyle);
        }
    }

    @SuppressWarnings("deprecation")
    private int createAlternateData(int currentRow, int currentCol, HSSFSheet documentDataSheet) {
        HSSFCell tempCell;
        HSSFRow tempRow;
        tempRow = documentDataSheet.createRow(currentRow);
        currentRow++;
        tempCell = tempRow.createCell((short) currentCol);
        tempCell.setCellValue("Alternate Data");
        tempCell.setCellStyle(mItalicSmallStyle);
        tempCell = tempRow.createCell((short) (currentCol + 1));
        tempCell.setCellValue("I");
        tempCell.setCellStyle(mBoldRedStyle);
        return currentRow;
    }

    @SuppressWarnings("deprecation")
    private int applyColumn(int currentRow, int currentCol, HSSFSheet documentDataSheet) {
        HSSFCell tempCell;
        HSSFRow tempRow;
        tempRow = documentDataSheet.createRow(currentRow);
        currentRow++;
        tempCell = tempRow.createCell((short) currentCol);
        tempCell.setCellValue("Apply column");
        tempCell.setCellStyle(mItalicSmallStyle);
        tempCell = tempRow.createCell((short) (currentCol + 1));
        tempCell.setCellValue("K");
        tempCell.setCellStyle(mBoldRedStyle);
        return currentRow;
    }

    @SuppressWarnings("deprecation")
    private int createPlatformsUsed(int currentRow, int currentCol, HSSFSheet documentDataSheet) {
        HSSFCell tempCell;
        HSSFRow tempRow;
        tempRow = documentDataSheet.createRow(currentRow);
        currentRow++;
        tempCell = tempRow.createCell((short) currentCol);
        tempCell.setCellValue("Platforms used");
        tempCell.setCellStyle(mItalicSmallStyle);
        tempCell = tempRow.createCell((short) (currentCol + 1));
        tempCell.setCellValue(((PlatformsInfo) mPlatformsInfo.elementAt(mSelectedPlatformIndex)).name);
        tempCell.setCellStyle(mBoldRedStyle);
        return currentRow;
    }

    @SuppressWarnings("deprecation")
    private int createEndRow(int currentRow, int currentCol, HSSFSheet documentDataSheet) {
        HSSFCell tempCell;
        HSSFRow tempRow;
        tempRow = documentDataSheet.createRow(currentRow);
        currentRow++;
        tempCell = tempRow.createCell((short) currentCol);
        tempCell.setCellValue("Endrow");
        tempCell.setCellStyle(mItalicSmallStyle);
        tempCell = tempRow.createCell((short) (currentCol + 1));
        tempCell.setCellValue(Integer.toString(mNbrOfCustomerRows + mStartRow - 1));
        tempCell.setCellStyle(mBoldRedStyle);
        return currentRow;
    }

    @SuppressWarnings("deprecation")
    private int createStartRow(int currentRow, int currentCol, HSSFSheet documentDataSheet) {
        HSSFCell tempCell;
        HSSFRow tempRow;
        tempRow = documentDataSheet.createRow(currentRow);
        currentRow++;
        tempCell = tempRow.createCell((short) currentCol);
        tempCell.setCellValue("Startrow");
        tempCell.setCellStyle(mItalicSmallStyle);
        tempCell = tempRow.createCell((short) (currentCol + 1));
        tempCell.setCellValue(Integer.toString(mStartRow));
        tempCell.setCellStyle(mBoldRedStyle);
        return currentRow;
    }

    @SuppressWarnings("deprecation")
    private int createDcoumentType(int currentRow, int currentCol, HSSFSheet documentDataSheet) {
        HSSFCell tempCell;
        HSSFRow tempRow;
        tempRow = documentDataSheet.createRow(currentRow);
        currentRow++;
        tempCell = tempRow.createCell((short) currentCol);
        tempCell.setCellValue("Document type");
        tempCell.setCellStyle(mItalicSmallStyle);
        tempCell = tempRow.createCell((short) (currentCol + 1));
        tempCell.setCellValue(DOCUMENTTYPESTRCUST);
        tempCell.setCellStyle(mBoldRedStyle);
        return currentRow;
    }

    @SuppressWarnings("deprecation")
    private void createHeader(HSSFPalette colorPalette, HSSFCell tempCell, HSSFRow tempRow, int currentRow,
        int currentCol, HSSFWorkbook newCustWorkBook, HSSFSheet documentDataSheet) {
        HSSFFont tempFont;
        mHeaderStyle = newCustWorkBook.createCellStyle();
        mHeaderStyle.setBorderBottom(HSSFCellStyle.BORDER_THICK);
        mHeaderStyle.setBottomBorderColor(colorPalette.findColor((byte) 0x00, (byte) 0x00, (byte) 0x00).getIndex());
        /*
         * HSSFCellStyle. BLACK
         */
        mHeaderStyle.setBorderTop(HSSFCellStyle.BORDER_THICK);
        mHeaderStyle.setTopBorderColor(colorPalette.findColor((byte) 0x00, (byte) 0x00, (byte) 0x00).getIndex()/*
                                                                                                                * HSSFCellStyle.
                                                                                                                * BLACK
                                                                                                                */);
        mHeaderStyle.setFillForegroundColor(new HSSFColor.TAN().getIndex());
        mHeaderStyle.setFillPattern(HSSFCellStyle.SOLID_FOREGROUND);

        for (int i = currentCol; i < currentCol + 5; i++) {
            tempCell = tempRow.createCell((short) i);
            tempCell.setCellStyle(mHeaderStyle);
        }
        HSSFCellStyle tempStyle = mHeaderStyle;
        mHeaderStyle.setBorderRight(HSSFCellStyle.BORDER_THICK);
        mHeaderStyle.setRightBorderColor(colorPalette.findColor((byte) 0x00, (byte) 0x00, (byte) 0x00).getIndex()/*
                                                                                                                  * HSSFCellStyle.
                                                                                                                  * BLACK
                                                                                                                  */);
        if (tempCell != null) {
            tempCell.setCellStyle(mHeaderStyle);
        }
        tempStyle.setBorderLeft(HSSFCellStyle.BORDER_THICK);
        tempStyle.setLeftBorderColor(colorPalette.findColor((byte) 0x00, (byte) 0x00, (byte) 0x00).getIndex()/*
                                                                                                              * HSSFCellStyle.
                                                                                                              * BLACK
                                                                                                              */);
        tempStyle.setAlignment(HSSFCellStyle.ALIGN_CENTER);
        tempCell = tempRow.getCell((short) currentCol);
        tempCell.setCellStyle(tempStyle);

        documentDataSheet.addMergedRegion(new Region(currentRow, (short) currentCol, currentRow,
            (short) (currentCol + 4)));
        tempCell.setCellValue("Document data");
        tempFont = newCustWorkBook.createFont();
        tempFont.setFontHeightInPoints((short) 12);
        tempFont.setFontName("Arial");
        tempFont.setBoldweight(HSSFFont.BOLDWEIGHT_BOLD);
        tempFont.setColor(colorPalette.findColor((byte) 0x00, (byte) 0x00, (byte) 0x00).getIndex()/*
                                                                                                   * HSSFCellStyle.
                                                                                                   * BLACK
                                                                                                   */);
        mHeaderStyle.setFont(tempFont);
        tempCell.setCellStyle(mHeaderStyle);
    }

    private void createFontStyles(HSSFPalette colorPalette, HSSFWorkbook newCustWorkBook) {
        HSSFFont tempFont;
        mItalicSmallStyle = newCustWorkBook.createCellStyle();
        tempFont = newCustWorkBook.createFont();
        tempFont.setFontHeightInPoints((short) 10);
        tempFont.setFontName("Arial");
        tempFont.setItalic(true);
        mItalicSmallStyle.setFont(tempFont);

        tempFont = newCustWorkBook.createFont();
        tempFont.setFontHeightInPoints((short) 10);
        tempFont.setFontName("Arial");
        tempFont.setBoldweight(HSSFFont.BOLDWEIGHT_BOLD);
        // TempFont.setColor( HSSFFont.COLOR_RED );
        tempFont.setColor(colorPalette.findColor((byte) 0xFF, (byte) 0x00, (byte) 0x00).getIndex());
        mBoldRedStyle = newCustWorkBook.createCellStyle();
        mBoldRedStyle.setFont(tempFont);

        tempFont = newCustWorkBook.createFont();
        tempFont.setFontHeightInPoints((short) 10);
        tempFont.setFontName("Arial");
        tempFont.setBoldweight(HSSFFont.BOLDWEIGHT_BOLD);

        tempFont.setColor(new HSSFColor.BLUE().getIndex());
        mBoldBlueStyle = newCustWorkBook.createCellStyle();
        mBoldBlueStyle.setFont(tempFont);
    }

}
