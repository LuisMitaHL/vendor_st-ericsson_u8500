package com.stericsson.sdk.signing.data;

/**
 * Represents data of a one subpackage. Used for root sign package creation.
 * 
 * @author pkutac01
 * 
 */
public class SubpackageData {

    /**
     * Subpackage one-based index.
     */
    private int index;

    /**
     * Subpackage directory path.
     */
    private String path;

    /**
     * Subpackage spare value.
     */
    private int spare;

    /**
     * Class constructor.
     * 
     * @param pIndex
     *            Subpackage one-based index.
     * @param pPath
     *            Subpackage directory path.
     * @param pSpare
     *            Subpackage spare value.
     */
    public SubpackageData(int pIndex, String pPath, int pSpare) {
        index = pIndex;
        path = pPath;
        spare = pSpare;
    }

    /**
     * @param pIndex
     *            Subpackage one-based index.
     */
    public void setIndex(int pIndex) {
        this.index = pIndex;
    }

    /**
     * @return Subpackage one-based index.
     */
    public int getIndex() {
        return index;
    }

    /**
     * @param pPath
     *            Subpackage directory path.
     */
    public void setPath(String pPath) {
        this.path = pPath;
    }

    /**
     * @return Subpackage directory path.
     */
    public String getPath() {
        return path;
    }

    /**
     * @param pSpare
     *            Subpackage spare value.
     */
    public void setSpare(int pSpare) {
        this.spare = pSpare;
    }

    /**
     * @return Subpackage spare value.
     */
    public int getSpare() {
        return spare;
    }
}
