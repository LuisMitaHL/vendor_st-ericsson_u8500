package com.stericsson.sdk.common.ui.wizards.logic;

/**
 * Class that is used to keep track of errors that might occur. ErrorHasOccured is used to
 * keep track if an error really has occured. User is intended as a message informing the user
 * regarding the error. Cell contains the cell position if applicable (i.e. for instance "A5")
 * where the error occured. SuggestedSolution contains a suggested solution if any
 */
public class ErrorInfoData {
    private boolean errorHasOccured;

    private String message;

    private String cell;

    private String suggestedSolution;

    /**
     * 
     */
    public ErrorInfoData() {
    }

    /**
     * @return true if error has occured
     */
    public boolean errorHasOccured() {
        return errorHasOccured;
    }

    /**
     * @param pErrorHasOccured error has occured
     */
    public void setErrorHasOccured(boolean pErrorHasOccured) {
        errorHasOccured = pErrorHasOccured;
    }

    /**
     * @return message
     */
    public String getMessage() {
        return message;
    }

    /**
     * @param pMessage - message
     */
    public void setMessage(String pMessage) {
        message = pMessage;
    }

    /**
     * @return cell
     */
    public String getCell() {
        return cell;
    }

    /**
     * @param pCell cell
     */
    public void setCell(String pCell) {
        cell = pCell;
    }

    /**
     * @return suggested solution
     */
    public String getSuggestedSolution() {
        return suggestedSolution;
    }

    /**
     * @param pSuggestedSolution solution
     */
    public void setSuggestedSolution(String pSuggestedSolution) {
        suggestedSolution = pSuggestedSolution;
    }
}