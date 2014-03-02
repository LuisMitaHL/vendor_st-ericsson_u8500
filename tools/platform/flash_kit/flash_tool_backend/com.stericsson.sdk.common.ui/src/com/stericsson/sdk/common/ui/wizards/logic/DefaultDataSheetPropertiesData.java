package com.stericsson.sdk.common.ui.wizards.logic;

import java.util.Vector;

/**
 * 
 * @author mielcluk
 * 
 *         Holds data read out of from 19062 document.
 */
public class DefaultDataSheetPropertiesData {
    private int mStartRow;

    private int mEndRow;

    private int mNbrOfCustomerRows;

    private ColumnDataInfo[] mColumnDataInfo;

    private Vector<PlatformsInfo> mPlatformsInfo;

    private int mSelectedPlatformIndex;

    private String mSelectedPlatformName;

    private UserCommentsInfo[] mUserCommentsInfo;

    private DocumentInfo mDocumentInfo;

    /**
     * Sets user comments info.
     * 
     * @param pUserCommentsInfo
     *            User comments info
     */
    public void setUserCommentsInfo(UserCommentsInfo[] pUserCommentsInfo) {
        mUserCommentsInfo = pUserCommentsInfo;
    }

    /**
     * Constructor.
     */
    public DefaultDataSheetPropertiesData() {
        mPlatformsInfo = new Vector<PlatformsInfo>();
    }

    /**
     * Gets user comments info
     * 
     * @return User comments info.
     */
    public UserCommentsInfo[] getUserCommentsInfo() {
        return mUserCommentsInfo;
    }

    /**
     * Gets start row.
     * 
     * @return Start row.
     */
    public int getStartRow() {
        return mStartRow;
    }

    /**
     * Sets start row.
     * 
     * @param pStartRow
     *            Start row.
     */
    public void setStartRow(int pStartRow) {
        mStartRow = pStartRow;
    }

    /**
     * Gets end end row.
     * 
     * @return End row.
     */
    public int getEndRow() {
        return mEndRow;
    }

    /**
     * Sets end row
     * 
     * @param pEndRow
     *            End row.
     */
    public void setEndRow(int pEndRow) {
        mEndRow = pEndRow;
    }

    /**
     * Gets number of rows.
     * 
     * @return Number of customer rows.
     */
    public int getNbrOfCustomerRows() {
        return mNbrOfCustomerRows;
    }

    /**
     * Sets number of customer rows.
     * 
     * @param pNbrOfCustomerRows
     *            Number of rows.
     */
    public void setNbrOfCustomerRows(int pNbrOfCustomerRows) {
        mNbrOfCustomerRows = pNbrOfCustomerRows;
    }

    /**
     * Gets column data info.
     * 
     * @return Column data info.
     */
    public ColumnDataInfo[] getColumnDataInfo() {
        return mColumnDataInfo;
    }

    /**
     * Sets column data info.
     * 
     * @param pColumnDataInfo
     *            Column data info.
     */
    public void setmColumnDataInfo(ColumnDataInfo[] pColumnDataInfo) {
        mColumnDataInfo = pColumnDataInfo;
    }

    /**
     * Gets platforms info.
     * 
     * @return Platforms info.
     */
    public Vector<PlatformsInfo> getPlatformsInfo() {
        return mPlatformsInfo;
    }

    // /**
    // * Sets platforms info.
    // *
    // * @param pPlatformsInfo
    // * Platofrms info.
    // */
    // public void setPlatformsInfo(Vector<PlatformsInfo> pPlatformsInfo) {
    // mPlatformsInfo = pPlatformsInfo;
    // }

    /**
     * Gets platform index.
     * 
     * @return Platform index.
     */
    public int getSelectedPlatformIndex() {
        return mSelectedPlatformIndex;
    }

    /**
     * Set platform index.
     * 
     * @param pSelectedPlatformIndex
     *            Platform index.
     */
    public void setSelectedPlatformIndex(int pSelectedPlatformIndex) {
        mSelectedPlatformIndex = pSelectedPlatformIndex;
    }

    /**
     * Gets platform name.
     * 
     * @return PLatofrm name.
     */
    public String getSelectedPlatformName() {
        return mSelectedPlatformName;
    }

    /**
     * Sets selected platform name.
     * 
     * @param pSelectedPlatformName
     *            Selected platform name.
     */
    public void setSelectedPlatformName(String pSelectedPlatformName) {
        mSelectedPlatformName = pSelectedPlatformName;
    }

    /**
     * Sets document info.
     * 
     * @param pDocumentInfo
     *            Document info.
     */

    public void setDocumentInfo(DocumentInfo pDocumentInfo) {
        mDocumentInfo = pDocumentInfo;

    }

    /**
     * Gets document info.
     * 
     * @return Document info.
     */
    public DocumentInfo getmDocumentInfo() {
        return mDocumentInfo;
    }

    /**
     * 
     * @param info
     *            platform info
     */
    public void addPlatformsInfo(PlatformsInfo info) {
        mPlatformsInfo.add(info);
    }

    /**
     * 
     */
    public void clearPlatformsInfo() {
        mPlatformsInfo.clear();

    }
}
