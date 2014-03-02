/**
 *
 */
package com.stericsson.sdk.equipment.io;

import java.io.BufferedReader;
import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.List;

import org.apache.commons.net.ftp.FTPClient;
import org.apache.commons.net.ftp.FTPFile;
import org.apache.commons.net.ftp.FTPReply;
import org.apache.commons.net.io.CopyStreamEvent;
import org.apache.commons.net.io.CopyStreamException;
import org.apache.commons.net.io.CopyStreamListener;
import org.apache.commons.net.io.Util;
import org.apache.log4j.Logger;

import com.stericsson.sdk.equipment.Activator;
import com.stericsson.sdk.equipment.io.port.IFTPPort;
import com.stericsson.sdk.equipment.io.port.IPort;
import com.stericsson.sdk.equipment.io.port.PortException;
import com.stericsson.sdk.ftp.communication.IFTPCommunicationService;
import com.stericsson.sdk.ftp.communication.IFTPCommunicatorListener;

/**
 *
 * Abstract FTP port. Handles communication with ftp server on target.
 *
 * @author esrimpa
 *
 */

public abstract class AbstractFTPPort implements IFTPPort {

    static final Logger log = Logger.getLogger(AbstractFTPPort.class);

    private final String portName;

    private final Integer portId = IPort.PortIdGenerator.INSTANCE.getNextId();

    private boolean opened;

    private FTPClient ftpClient;

    /** File containing information regarding dump location on target */
    private static final String CORE_DUMP_LOCATION_INFO = "/coredump/dir_list.txt";

    /** Default modem core dump path on target */
    private static final String DEFAULT_MODEM_COREDUMP_PATH = "/sdcard/core_dumps/modem/";

    /** Default kernel core dump path on target */
    private static final String DEFAULT_KERNEL_COREDUMP_PATH = "/sdcard/core_dumps/kernel/";

    /** Temp file suffix */
    private static final String FTP_TEMP_FILE_SUFFIX = ".tmp";

    /** Folder delimiter */
    private static final String FOLDER_DELIMITER = "/";

    /**
     * Constructor
     *
     * @param name
     *            IP address of the Port
     */
    public AbstractFTPPort(String name) {
        portName = name;
        ftpClient = new FTPClient();
    }

    /**
     * {@inheritDoc}
     *
     * @see com.stericsson.sdk.equipment.io.IPort#getPortName()
     */
    public String getPortName() {
        return portName;
    }

    /** 
     * {@inheritDoc}
     */
    public Integer getPortIdentifier() {
        return portId;
    }

    /**
     * {@inheritDoc}
     *
     * @see com.stericsson.sdk.equipment.io.IPort#close()
     */
    public void close() throws PortException {
        disconnect();
    }

    /**
     * {@inheritDoc}
     *
     * @see com.stericsson.sdk.equipment.io.IPort#isOpen()
     */
    public boolean isOpen() {
        return opened;
    }

    /**
     * {@inheritDoc}
     *
     * @see com.stericsson.sdk.equipment.io.IPort#open()
     */
    public void open() throws PortException {
        opened = true;
    }

    /**
     * {@inheritDoc}
     *
     * @see com.stericsson.sdk.equipment.io.port.IFTPPort#connect(java.lang.String)
     */
    public boolean connect() {
        return connect(null, null);

    }

    /**
     * {@inheritDoc}
     *
     */
    public boolean connect(String username, String password) {
        if (ftpClient.isConnected()) {
            return true;
        }
        try {
            int reply;
            ftpClient.connect(portName);

            // After connection attempt, you should check the reply code to verify
            // success.
            reply = ftpClient.getReplyCode();

            if (!FTPReply.isPositiveCompletion(reply)) {
                ftpClient.disconnect();
                log.error("FTP server refused connection. Server : " + portName);
                return false;
            }

            // After connection , provide the login information for a specific user
            if (username != null) {
                if (!ftpClient.login(username, password)) {
                    log.error("Not able to login to FTP server " + portName);
                    return false;
                }
            }
        } catch (IOException e) {
            if (ftpClient.isConnected()) {
                try {
                    ftpClient.disconnect();
                } catch (IOException f) {
                    log.error("Error when trying to disconnect from FTP server after IOException");
                }
            }
            log.error("Could not connect to FTP server." + portName);
            return false;
        }
        return true;
    }

    /**
     * {@inheritDoc}
     *
     */
    public List<String> listAllFiles() {
        List<String> fileList = new ArrayList<String>();
        List<String> directoryList = new ArrayList<String>();

        try {
            if (ftpClient.isConnected()) {

                directoryList = readCoreDumpLocationFile();

                for (String directory : directoryList) {

                    FTPFile[] ftpFiles = ftpClient.listFiles(directory);
                    if (ftpFiles != null) {

                        for (FTPFile ftpFile : ftpFiles) {
                            // Check if FTPFile is a File
                            if (ftpFile.getType() == FTPFile.FILE_TYPE
                                && !ftpFile.getName().equalsIgnoreCase(FTP_DOWNLOADED_INFO_FILE)) {
                                fileList.add(directory + ftpFile.getName());
                            }
                        }
                    }
                }
                if (fileList.isEmpty()) {
                    return null;
                }
            }
        } catch (IOException e) {
            try {
                log.error("Unable to list files on FTP server.");
                ftpClient.disconnect();
                return null;
            } catch (IOException f) {
                return null;
            }
        }
        return fileList;
    }

    private List<String> readCoreDumpLocationFile() {
        boolean downloadResult = false;
        List<String> fileContent = new ArrayList<String>();
        ByteArrayOutputStream downloadedFile = null;
        try {
            downloadedFile = new ByteArrayOutputStream();
            downloadResult = ftpClient.retrieveFile(CORE_DUMP_LOCATION_INFO, downloadedFile);
        } catch (IOException e) {
            log.warn("No coredump location information file found on server. Using default coredump location paths.");
        }

        if (downloadResult) {
            InputStream fileReader = new ByteArrayInputStream(downloadedFile.toByteArray());
            BufferedReader in = null;
            try {
                in = new BufferedReader(new InputStreamReader(fileReader, "UTF-8"));
                String line;
                while ((line = in.readLine()) != null) {
                    if (line.length() != 0) {
                        fileContent.add(line);
                    }
                }
            } catch (Exception e) {
                log.error("Error trying to read input from the core dump location file.", e);
            }
        } else {
            fileContent.add(DEFAULT_MODEM_COREDUMP_PATH);
        }
        fileContent.add(DEFAULT_KERNEL_COREDUMP_PATH);
        return fileContent;
    }

    /**
     * {@inheritDoc}
     *
     */
    public boolean downloadFile(String ftpFileName, String localDirectoryPath) {

        FileOutputStream downloadedFile = null;
        String localFilePath = localDirectoryPath;

        // Check if download directory exists. If not, create it.
        File directory = new File(localDirectoryPath);
        if (!directory.exists()) {
            directory.mkdirs();
        }

        boolean retVal = false;

        if (!localDirectoryPath.endsWith(FTP_TEMP_FILE_SUFFIX)) {
            localFilePath = localDirectoryPath + FOLDER_DELIMITER + getFileName(ftpFileName);
        }
        try {

            if (ftpClient.isConnected()) {

                downloadedFile = new FileOutputStream(localFilePath);

                // Download the file
                retVal = downloadFileWithProgress(downloadedFile, ftpFileName);
            }
        } catch (IOException e) {
            log.error("Could not perform file transfer operation on server.");
        } finally {
            try {
                if (downloadedFile != null) {
                    downloadedFile.close();
                }
            } catch (IOException e) {
                log.error("Failed to close file after download.");
            }
        }
        return retVal;
    }

    private boolean downloadFileWithProgress(FileOutputStream out, final String fileName) {
        IFTPCommunicationService service = Activator.getServiceObject(IFTPCommunicationService.class);
        final IFTPCommunicatorListener listener;

        if (service != null) {
            listener = service.getCommunicationListener();
        } else {
            listener = null;
        }

        final long fileSize = getFileSize(fileName);
        if (fileSize == -1) {
            log.warn("Getting size of file " + fileName + " has failed");
            return false;
        }

        CopyStreamListener csl = new CopyStreamListener() {
            long byteCount = 0;

            /**
             * {@inheritDoc}
             *
             */
            public void bytesTransferred(long totalBytesTransferred, int bytesTransferred, long streamSize) {
                byteCount += bytesTransferred;
                if (listener != null) {
                    if (byteCount >= (fileSize / 20) || (totalBytesTransferred >= fileSize)) {
                        listener.taskProgress(fileSize, totalBytesTransferred);
                        byteCount = 0;
                    }
                }
            }

            public void bytesTransferred(CopyStreamEvent arg0) {
            }
        };

        try {
            InputStream inputStream = ftpClient.retrieveFileStream(fileName);
            if (inputStream != null) {
                Util.copyStream(inputStream, out, ftpClient.getBufferSize(), CopyStreamEvent.UNKNOWN_STREAM_SIZE, csl);
                if (!ftpClient.completePendingCommand()) {
                    return false;
                }
            }
        } catch (CopyStreamException e) {
            log.error("Download of file " + fileName + " failed: ", e);
            return false;
        } catch (IOException e) {
            log.error(
                "I/O error occured while either sending a command to the server or receiving a reply from the server",
                e);
            return false;
        }
        return true;
    }

    private long getFileSize(String fileName) {
        try {
            if (ftpClient.isConnected()) {
                FTPFile[] ftpFiles = ftpClient.listFiles(fileName);
                if (ftpFiles == null) {
                    return -1;
                }
                if (ftpFiles.length > 0) {
                    return ftpFiles[0].getSize();
                }
            }
        } catch (IOException e) {
            try {
                log.error("Could not get file size");
                ftpClient.disconnect();
            } catch (IOException f) {
                log.error("Failed to disconnect from ftp server");
            }
        }
        return -1;
    }

    /**
     * {@inheritDoc}
     *
     */
    public boolean deleteFile(String fileName) {
        boolean returnValue = false;
        try {
            if (ftpClient.isConnected()) {
                // Delete the file
                returnValue = ftpClient.deleteFile(fileName);
            }
        } catch (IOException e) {
            try {
                log.error("Unable to perform file delete operation on server.", e);
                ftpClient.disconnect();
            } catch (IOException f) {
                log.error("Unable to disconnect from FTP server");
            }
        }
        return returnValue;
    }

    /**
     * {@inheritDoc}
     *
     */
    public boolean uploadFile(String localFilePath, String fileName) {

        FileInputStream uploadFile = null;
        boolean ret = false;
        try {
            if (ftpClient.isConnected()) {
                uploadFile = new FileInputStream(localFilePath);

                // Download the file
                ret = ftpClient.storeFile(fileName, uploadFile);
            }
        } catch (IOException e) {
            log.error("Could not perform file transfer operation on server.");
        } finally {
            try {
                if (uploadFile != null) {
                    uploadFile.close();
                }
            } catch (IOException e) {
                log.error("Unable to close the file or disconnect from ftp server.");
            }
        }
        return ret;
    }

    /**
     * {@inheritDoc}
     *
     */
    public void disconnect() {

        if (ftpClient.isConnected()) {
            try {
                ftpClient.disconnect();
            } catch (IOException f) {
                log.error("Unable to disconnect from FTP server.");
            }
        }
    }

    /**
     * Used for unittests only
     *
     * @param testFTPClient
     *            test implementation of FTPClient
     */
    public void setFTPClient(FTPClient testFTPClient) {
        ftpClient = testFTPClient;
    }

    /**
     * Return whether ME has new dumps available. Will default only return false. Needs to be
     * overridden.
     *
     * @return boolean false
     */
    public boolean isDumping() {
        return false;
    }

    private String getFileName(String ftpFileName) {
        if (ftpFileName.contains(FOLDER_DELIMITER)) {
            return ftpFileName.substring(ftpFileName.lastIndexOf(FOLDER_DELIMITER) + 1);
        }
        return ftpFileName;
    }

}
