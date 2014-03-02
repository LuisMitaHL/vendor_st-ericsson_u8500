/**
 * 
 */
package com.stericsson.sdk.ftp.communication.configuration;

/**
 * Configuration class used by the FTP communication service 
 * 
 * @author esrimpa
 * 
 */
public final class FTPConfiguration {

    private FTPConfiguration() {
    }

    /**
     * User name for the FTP login
     */
    public static final String FTP_USER_NAME = "root";

    /**
     * Passwd for the FTP login
     */
    public static final String FTP_PASSWD = "root";

    /**
     * File name containing the downloaded dump names
     */
    public static final String FTP_DOWNLOADED_INFO_FILE = "list.txt";

}
