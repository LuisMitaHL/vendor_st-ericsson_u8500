package com.stericsson.sdk.common.ui.wizards.logic;

import java.util.Locale;

import org.apache.poi.hssf.usermodel.HSSFCell;
import org.apache.poi.hssf.usermodel.HSSFRow;
import org.apache.poi.hssf.usermodel.HSSFSheet;

/**
 * Takes data out of cells.
 * 
 * 
 */
final class CellsReader implements IDefaultDataSheetConstants {

    private CellsReader() {
    }

    /**
     * A method that returns a positive number from a given cell. If the cell contents is not
     * positive an error is generated.
     * 
     * @param columnIndex
     *            is the Excel column that is to be used
     * @param rowIndex
     *            is the Excel row that is to be used
     * @return A the function succeeds the positive number is returned, otherwise Integer.MIN_VALUE
     *         is returned.
     */
    static int getPositiveNumberAtPosition(String columnIndex, int rowIndex, HSSFSheet mCurrentSheetUsed) {
        int tempInt = CellsReader.getNumberAtPosition(columnIndex, rowIndex, mCurrentSheetUsed);
        if (!ErrorInfoUtils.errorHasOccured()) {
            if (tempInt < 0) {
                ErrorInfoUtils.error("Value is negative (" + tempInt + ")", columnIndex + rowIndex,
                    ERRORMSGMAKESURETHATTHECELLCONTAINS + "a positive value");
                tempInt = Integer.MIN_VALUE;
            }
        }
        return tempInt;
    }

    /**
     * A method that returns a String from a given cell. If the cell contents is not blank or of
     * string or numeric type an empty string is returned.
     * 
     * @param columnIndex
     *            is the Excel column that is to be used
     * @param rowIndex
     *            is the Excel row that is to be used
     * @return A string of the contents that was found in the cell.
     */
    static String getStringAtPosition(String columnIndex, int rowIndex, HSSFSheet mCurrentSheetUsed) {
        String result = "";
        HSSFCell cell = getCellAtPosition(columnIndex, rowIndex, mCurrentSheetUsed);
        if (cell != null) {
            if ((cell.getCellType() == HSSFCell.CELL_TYPE_BLANK) || (cell.getCellType() == HSSFCell.CELL_TYPE_STRING)) {
                result = cell.getStringCellValue();
            } else if ((cell.getCellType() == HSSFCell.CELL_TYPE_BLANK)
                || (cell.getCellType() == HSSFCell.CELL_TYPE_NUMERIC)) {
                result = Integer.toString((int) cell.getNumericCellValue());
            }
        }
        return result;
    }

    /**
     * A method that returns an int from a given cell. If the cell contents is not blank, numeric or
     * a formula (or a hex value, i.e. a string) a Integer.MIN_VALUE is returned.
     * 
     * @param columnIndex
     *            is the Excel column that is to be used
     * @param rowIndex
     *            is the Excel row that is to be used
     * @return An int that was found in the cell.
     */
    static int getNumberAtPosition(String columnIndex, int rowIndex, HSSFSheet mCurrentSheetUsed) {
        int result = Integer.MIN_VALUE;
        HSSFCell cell = getCellAtPosition(columnIndex, rowIndex, mCurrentSheetUsed);
        if (cell != null) {
            if ((cell.getCellType() == HSSFCell.CELL_TYPE_BLANK)
                || (cell.getCellType() == HSSFCell.CELL_TYPE_NUMERIC || (cell.getCellType() == HSSFCell.CELL_TYPE_FORMULA))) {
                // This is a numerical value
                try {
                    result = (int) cell.getNumericCellValue();
                } catch (NumberFormatException e) {
                    // cLogger.warn("Error while getting number at pos.", e);

                    ErrorInfoUtils.error("Value is non numeric", columnIndex + rowIndex,
                        ERRORMSGMAKESURETHATTHECELLCONTAINS + "a numeric value");
                }
            } else if ((cell.getCellType() == HSSFCell.CELL_TYPE_BLANK)
                || (cell.getCellType() == HSSFCell.CELL_TYPE_STRING)) {
                String tempStr = cell.getStringCellValue();
                // Aha, this seems to be a hexadecimal value
                try {
                    result = Integer.decode(tempStr).intValue();
                } catch (NumberFormatException e) {
                    // cLogger.warn("Error decoding hex value!", e);

                    ErrorInfoUtils.error("Value is non numeric (" + tempStr + ")", columnIndex + rowIndex,
                        ERRORMSGMAKESURETHATTHECELLCONTAINS + "a numeric value");
                }
            }
        }
        return result;
    }

    /**
     * A method that returns a HSSFCell from a given cell. NB! The selectde sheet to read from is
     * determined by the variable m_CurrentSheetUsed
     * 
     * @param columnIndex
     *            is the Excel column that is to be used
     * @param rowIndex
     *            is the Excel row that is to be used
     * @return A HSSFCell reference to the specified cell.
     */
    @SuppressWarnings("deprecation")
    static HSSFCell getCellAtPosition(String columnIndex, int rowIndex, HSSFSheet mCurrentSheetUsed) {
        // First determine which column number we are trying to access, since
        // it is given in Excel 'format' i.e. A, AA, or CE etc.
        // Recalculate it to match a numerical value...
        int tempInt = 1;
        columnIndex = columnIndex.toUpperCase(Locale.getDefault());
        switch (columnIndex.length()) {
            case 0:
                // cLogger.error("Empty columnIndex to 'GetCellAtPosition'");
                break;
            case 1:
                tempInt = (int) (columnIndex.charAt(0)) - 64;
                break;
            case 2:
                tempInt = ((int) (columnIndex.charAt(0) - 64) * 26 + ((int) (columnIndex.charAt(1)) - 64));
                break;
            default:
                // cLogger.error("No support of this large Excel sheets. 'GetCellAtPosition'");
                break;
        }
        // Accessing the sheet is zero based
        HSSFRow row = mCurrentSheetUsed.getRow(rowIndex - 1);
        HSSFCell cell = row.getCell((short) (tempInt - 1));
        return cell;
    }

    static boolean checkIfDoneReading(DefaultDataSheetPropertiesData pPropData, HSSFSheet pCurrentSheetUsed,
        int currentWorkRow, boolean doneReading) {
        HSSFCell cell;
        if (currentWorkRow > pPropData.getEndRow()) {
            doneReading = true;
        } else if (currentWorkRow > pCurrentSheetUsed.getPhysicalNumberOfRows()) {
            doneReading = true;
        } else {
            cell = getCellAtPosition(pPropData.getColumnDataInfo()[IDCOLUMN].column, currentWorkRow, pCurrentSheetUsed);
            if ((cell == null) || (cell.getCellType() == HSSFCell.CELL_TYPE_BLANK)) {
                doneReading = true;
            }
        }
        return doneReading;
    }

    /**
     * @param currentWorkRow
     * @param doneReading
     * @return
     */
    static boolean isCellEmpty(int currentWorkRow, DefaultDataSheetPropertiesData pPropData, HSSFSheet pCurrentSheetUsed) {

        // Make sure there is something to read at the first row
        HSSFCell cell =
            getCellAtPosition(pPropData.getColumnDataInfo()[IDCOLUMN].column, currentWorkRow, pCurrentSheetUsed);

        if ((cell == null) || (cell.getCellType() == HSSFCell.CELL_TYPE_BLANK)) {
            return true;
        }

        return false;
    }

}
