package com.stericsson.sdk.common.ui.wizards.logic;

/**
 * Helper class for ErrorInfoData and logging
 * 
 * @author kapalpaw
 * 
 */
/* package */final class ErrorInfoUtils {

    private static ErrorInfoData errorInfo = null;

    private static StringBuilder logTxt = null;

    private ErrorInfoUtils() {

    }

    /**
     * Gets an signle instance of {@link ErrorInfoData}, creates one if there is none
     * 
     * @return {@link ErrorInfoData} instance
     */
    private static ErrorInfoData getErrorInfoInstance() {
        if (errorInfo == null) {
            errorInfo = new ErrorInfoData();
        }
        return errorInfo;
    }

    /**
     * Returns instance of logger
     * 
     * @return logger instance
     */
    private static StringBuilder getLogInstance() {
        if (logTxt == null) {
            logTxt = new StringBuilder();
        }
        return logTxt;
    }

    /**
     * Clears error info
     */
    public static void clearErrorInfo() {
        getErrorInfoInstance();
        errorInfo.setErrorHasOccured(false);
        errorInfo.setMessage("");
        errorInfo.setCell("");
        errorInfo.setSuggestedSolution("");
    }

    /**
     * Checks if an error occurred
     * 
     * @return true if error has occurred, false elsewhere
     */
    public static boolean errorHasOccured() {
        getErrorInfoInstance();
        return errorInfo.errorHasOccured();
    }

    /**
     * Reports an error
     * 
     * @param pMessage
     *            error message
     */
    public static void error(String pMessage) {
        getErrorInfoInstance();
        error(pMessage, "", "");
    }

    /**
     * Reports an error
     * 
     * @param pMessage
     *            error message
     * @param pCell
     *            cell
     * @param pSuggestedSolution
     *            suggested solution
     */
    public static void error(String pMessage, String pCell, String pSuggestedSolution) {
        getErrorInfoInstance();
        errorInfo.setErrorHasOccured(true);
        errorInfo.setMessage(pMessage);
        errorInfo.setCell(pCell);
        errorInfo.setSuggestedSolution(pSuggestedSolution);
    }

    /**
     * Sets the cell in last error.
     * 
     * @param pCell
     *            cell
     */
    public static void setCell(String pCell) {
        getErrorInfoInstance();
        if (ErrorInfoUtils.errorHasOccured()) {
            ErrorInfoUtils.getErrorInfoInstance().setCell(pCell);
        }
    }

    /**
     * Returns cell of last error.
     * 
     * @return cell of last error, null if none
     */
    public static String getCell() {
        return getErrorInfoInstance().getCell();
    }

    /**
     * Write a single line of logs. With end of line character.
     * 
     * @param pMessage
     *            message to be logged
     */
    public static void logn(String pMessage) {
        getLogInstance();
        logTxt.append(pMessage);
        logTxt.append('\n');
    }

    /**
     * Write a single line of logs. Without end of line character.
     * 
     * @param pMessage
     *            message to be logged
     */
    public static void log(String pMessage) {
        getLogInstance();
        logTxt.append(pMessage);
    }

    /**
     * Return the log as a string
     * 
     * @return string log representation
     */
    public static String getLogString() {
        if (logTxt != null) {
            return logTxt.toString();
        } else {
            return "";
        }
    }

}
