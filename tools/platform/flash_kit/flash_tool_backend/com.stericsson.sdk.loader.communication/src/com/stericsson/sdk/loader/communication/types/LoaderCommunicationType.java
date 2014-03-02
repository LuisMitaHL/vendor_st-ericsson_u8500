package com.stericsson.sdk.loader.communication.types;

/**
 * @author xdancho
 * 
 */
public class LoaderCommunicationType {

    /**
     * 
     */
    protected int status = 0;

    /**
     * @param result
     *            result
     */
    public LoaderCommunicationType(int result) {
        this.status = result;
    }

    /**
     * @return status
     */
    public int getStatus() {
        return status;
    }

    /**
     * method used to check the strings and lengths provided by the JNI layer
     * 
     * @param str
     *            the string
     * @param length
     *            the length of the string
     * @return return false if string or length is 0, or length > str
     */
    public static boolean checkString(String str, int length) {

        if (length == 0 || str.length() == 0 || length > str.length()) {
            return false;
        }
        return true;
    }

}
