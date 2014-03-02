package com.stericsson.sdk.equipment.io.port;

import java.util.List;

/**
 * Add all generic FTPCommunication port functionality in this interface
 *
 * @author esrimpa
 */
public interface IFTPPort extends IPort {

    /**
     * The file, located on target, listing which files have already been downloaded or not.
     */
    String FTP_DOWNLOADED_INFO_FILE = "list.txt";

    /**
     * Get the FTP connection
     *
     * @return true on success or false on failure
     */
    boolean connect();

    /**
     * Get the FTP connection with username and password
     *
     * @param username
     *            User Name for the login to FTP server
     * @param password
     *            password for the user to login
     * @return true on success or false on failure
     */
    boolean connect(String username, String password);

    /**
     * List the files in the FTP Server
     *
     * @return list of undownloaded files on the ME or null on failure
     */
    List<String> listAllFiles();

    /**
     * Download file from the FTP server
     *
     * @param ftpFileName
     *            File to be downloaded. If null, latest file available on ftp server will be
     *            downloaded.
     * @param localDirectoryPath
     *            Full path of the folder where to save the dump file. Could also be a path to a temp
     *            file (used when updating information file on target).
     *
     * @return return true on success or false in failure
     */
    boolean downloadFile(String ftpFileName, String localDirectoryPath);

    /**
     * Delete the file from FTP Server
     *
     * @param coreFileName
     *            name of the core file
     *
     * @return true on success otherwise false on failure
     */
    boolean deleteFile(String coreFileName);

    /**
     * Upload file to the FTP server
     *
     * @param fileName
     *            File to be uploaded
     * @param localFilePath
     *            Full path of the file to be place in local machine.
     *
     * @return return true on success or false in failure
     */
    boolean uploadFile(String localFilePath, String fileName);

    /**
     * Disconnect from FTP Server
     *
     */
    void disconnect();
}
