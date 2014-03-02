package com.stericsson.sdk.ftp.communication;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.List;
import java.util.Scanner;

import org.apache.log4j.Logger;

import com.stericsson.sdk.equipment.io.port.IFTPPort;
import com.stericsson.sdk.equipment.io.port.IPort;
import com.stericsson.sdk.equipment.io.port.PortException;
import com.stericsson.sdk.ftp.communication.configuration.FTPConfiguration;

/**
 * Service to list, download and delete files from a dumping ME.
 * 
 * @author esrimpa
 * 
 */
public class FTPCommunicationService implements IFTPCommunicationService {

    static final Logger log = Logger.getLogger(FTPCommunicationService.class.getName());

    /** Temp file prefix */
    private static final String FTP_TEMP_FILE_PREFIX = "brp";

    /** Temp file suffix */
    private static final String FTP_TEMP_FILE_SUFFIX = ".tmp";

    /** File Downloaded */
    private static final String FTP_DOWNLOADED = "|Downloaded";

    /** File not Downloaded */
    private static final String FTP_NOT_DOWNLOADED = "|Not Downloaded";

    /** Failure */
    private static final int ERROR = 1;

    /** Success */
    private static final int SUCCESS = 0;

    private IFTPCommunicatorListener communicationListener;

    /**
     * Constructor
     */
    public FTPCommunicationService() {
    }

    /**
     * {@inheritDoc}
     */
    public int shutdown(IPort port) {
        try {
            port.close();
            return SUCCESS;
        } catch (PortException e) {
            log.error("Unable to close the FTP port " + e.getMessage());
            return ERROR;
        }
    }

    /**
     * {@inheritDoc}
     */
    public List<String> listFile(IPort port) {

        boolean returnValue;

        File tempFile = null;

        IFTPPort ftpport = (IFTPPort) port;

        returnValue = ftpport.connect(FTPConfiguration.FTP_USER_NAME, FTPConfiguration.FTP_PASSWD);

        if (returnValue) {

            List<String> allDumpFiles = new ArrayList<String>();

            List<String> fileWithDwnldInfo = new ArrayList<String>();

            try {
                // Create a temp file to download the dump information file
                tempFile = File.createTempFile(FTP_TEMP_FILE_PREFIX, FTP_TEMP_FILE_SUFFIX);

                // Get the list of dump files present in the directory
                allDumpFiles = ftpport.listAllFiles();

                if ((allDumpFiles == null) || (allDumpFiles.isEmpty())) {

                    // Do not want to return null, otherwise it will throw an error every time it
                    // list file on the GUI while there is no files on the FTP server. Instead
                    // return an empty list.
                    log.error("No file to download from the FTP server.");
                    ftpport.disconnect();
                    return fileWithDwnldInfo;
                }
                // Download the downloaded dump info file
                if (!ftpport.downloadFile(FTPConfiguration.FTP_DOWNLOADED_INFO_FILE, tempFile.getAbsolutePath())) {
                    log.info("No info file available on target. New info file will be created");
                }

                // Now compare which files are downloaded and which are not
                fileWithDwnldInfo = listDumpFileNotDownloaded(tempFile, allDumpFiles);

                // Delete the the tempfile
                tempFile.delete();

            } catch (IOException e) {
                log.error("Unable to create temp file for the downloaded dump file information.");
                if (tempFile != null) {
                    tempFile.delete();
                }
                ftpport.disconnect();
                return null;
            }
            ftpport.disconnect();
            return fileWithDwnldInfo;
        }

        return null;
    }

    /**
     * List the dumpfiles which are not downloaded by comparing the files. Adds a suffix to the file
     * name with current download status.
     * 
     * @param downloadedFiles
     *            File which contains the information related to the downloaded files.
     * @param allFiles
     *            All the files on the Target.
     * @return returns a list of files with downloaded status information appended.
     */
    private List<String> listDumpFileNotDownloaded(File downloadedFiles, List<String> allFiles) {

        List<String> filesList = new ArrayList<String>();
        List<String> fileContent = new ArrayList<String>();
        Scanner scanner = null;
        FileInputStream fileReader = null;

        try {
            // Read the file and put into a list for comparision
            fileReader = new FileInputStream(downloadedFiles);
            scanner = new Scanner(fileReader, "UTF-8");

            // first use a Scanner to get each line
            while (scanner.hasNextLine()) {
                String line = scanner.nextLine();
                if (line.length() != 0) {
                    fileContent.add(line);
                }
            }

            // Now do the actual compare
            for (String value : allFiles) {
                String valueToAdd;
                // If the file is downloaded add a string "filename|Downloaded". It will be
                // used to parse which files are downloaded and which are not from the client.
                if (!fileContent.contains(value)) {
                    valueToAdd = value + FTP_NOT_DOWNLOADED;
                } else {
                    valueToAdd = value + FTP_DOWNLOADED;
                }

                filesList.add(valueToAdd);
            }

        } catch (FileNotFoundException e) {
            log.error("Unable to open temp file for the downloaded dump file information.");
        } finally {
            if (scanner != null) {
                scanner.close();
                try {
                    fileReader.close();
                } catch (IOException e) {
                    log.error("Unable to close file.");
                }
            }
        }
        return filesList;
    }

    /**
     * {@inheritDoc}
     */
    public int downloadDump(IPort port, String coreFileName, String localDirectoryPath, boolean isAutoDelete,
        IFTPCommunicatorListener listener) {

        IFTPPort ftpPort = null;
        boolean returnValue = false;
        int returnStatus = ERROR;

        if (coreFileName == null) {
            return returnStatus;
        }

        try {
            setCommunicationListener(listener);

            ftpPort = (IFTPPort) port;
            returnValue = ftpPort.connect(FTPConfiguration.FTP_USER_NAME, FTPConfiguration.FTP_PASSWD);

            if (returnValue) {
                if (coreFileName.equals("all")) {
                    // Download all files.
                    returnStatus = downloadAllDumps(ftpPort, localDirectoryPath, isAutoDelete);
                } else {
                    // Download one file
                    returnStatus = downloadOneDump(ftpPort, coreFileName, localDirectoryPath, isAutoDelete);
                }
            }
        } finally {
            if (returnValue && ftpPort != null) {
                ftpPort.disconnect();
            }
        }
        return returnStatus;
    }

    private int downloadAllDumps(IFTPPort ftpPort, String localDirectoryPath, boolean isAutoDelete) {
        List<String> dumpFiles = ftpPort.listAllFiles();
        if (dumpFiles == null) {
            return ERROR;
        }
        for (String fileName : dumpFiles) {
            int retValue = downloadOneDump(ftpPort, fileName, localDirectoryPath, isAutoDelete);
            if (retValue == ERROR) {
                log.error("Unable to download all dump files.");
                return ERROR;
            }
        }
        return SUCCESS;
    }

    private int downloadOneDump(IFTPPort ftpPort, String coreFileName, String localDirectoryPath, boolean isAutoDelete) {
        if (!downloadCoreDump(ftpPort, coreFileName, localDirectoryPath)) {
            return ERROR;
        }
        File tempFile = updateInfoFile(ftpPort, coreFileName);
        if (tempFile == null) {
            return ERROR;
        }
        if (!uploadInfoFile(ftpPort, tempFile)) {
            return ERROR;
        }
        if (isAutoDelete) {
            if (!ftpPort.deleteFile(coreFileName)) {
                log.error("Able to download the file but unable to delete from FTP server.");
                return ERROR;
            }
        }
        return SUCCESS;
    }

    private boolean downloadCoreDump(IFTPPort ftpPort, String coreFileName, String localDirectoryPath) {
        // Download the actual file
        boolean returnValue = ftpPort.downloadFile(coreFileName, localDirectoryPath);

        if (!returnValue) {
            log.error("Unable to download the dump file." + coreFileName);
        }
        return returnValue;
    }

    private boolean uploadInfoFile(IFTPPort ftpPort, File tempFile) {
        boolean returnValue = ftpPort.uploadFile(tempFile.getAbsolutePath(), FTPConfiguration.FTP_DOWNLOADED_INFO_FILE);

        if (!returnValue) {
            log.error("Unable to upload the dump information file.");
            ftpPort.disconnect();
        }
        // Delete the the tempfile
        tempFile.delete();
        return returnValue;
    }

    private File updateInfoFile(IFTPPort ftpPort, String coreFileName) {
        File tempFile;
        try {
            tempFile = File.createTempFile(FTP_TEMP_FILE_PREFIX, FTP_TEMP_FILE_SUFFIX);
            tempFile.deleteOnExit();
            // Download the downloaded dump info file.
            // If the file is not there create a new file.
            ftpPort.downloadFile(FTPConfiguration.FTP_DOWNLOADED_INFO_FILE, tempFile.getAbsolutePath());
            BufferedWriter outFile =
                new BufferedWriter(new OutputStreamWriter(new FileOutputStream(tempFile, true), "UTF-8"));
            PrintWriter out = new PrintWriter(outFile);

            // Write the downloaded file name into the file
            out.println(coreFileName);
            out.close();
        } catch (IOException e) {
            log.error("Unable to update the info file for the downloaded dump.");
            return null;
        }
        return tempFile;

    }

    /**
     * {@inheritDoc}
     */
    public int deleteDump(IPort port, String coreFileName) {
        boolean connected;
        IFTPPort ftpPort = (IFTPPort) port;
        connected = ftpPort.connect(FTPConfiguration.FTP_USER_NAME, FTPConfiguration.FTP_PASSWD);

        if (connected) {
            boolean retVal = false;

            if (coreFileName.equals("all")) {
                // Delete all files.
                List<String> dumpFiles = ftpPort.listAllFiles();

                if (dumpFiles == null) {
                    ftpPort.disconnect();
                    return ERROR;
                }
                for (String file : dumpFiles) {
                    retVal = ftpPort.deleteFile(file);
                    if (!retVal) {
                        log.error("Unable to delete the dump file :" + file);
                        ftpPort.disconnect();
                        return ERROR;
                    }
                }
                // Remove also the "list.txt" - file.
                ftpPort.deleteFile(IFTPPort.FTP_DOWNLOADED_INFO_FILE);
            } else {
                // Delete the actual file
                retVal = ftpPort.deleteFile(coreFileName);
            }

            if (!retVal) {
                log.error("Unable to delete the dump file :" + coreFileName);
                ftpPort.disconnect();
                return ERROR;
            }
            ftpPort.disconnect();
            return SUCCESS;
        }
        return ERROR;
    }

    /**
     * {@inheritDoc}
     * 
     */
    public IFTPCommunicatorListener getCommunicationListener() {
        return communicationListener;
    }

    /**
     * Communication listener used to track task progress.
     * 
     * @param comListener
     *            Listener to set
     */
    private void setCommunicationListener(IFTPCommunicatorListener comListener) {
        communicationListener = comListener;
    }
}
