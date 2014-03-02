package com.stericsson.sdk.ftp.communication;

import java.util.List;

import com.stericsson.sdk.equipment.io.port.IPort;

/**
 * Interface for FTPCommunicationService.
 * 
 * @author esrimpa
 * 
 */
public interface IFTPCommunicationService {

    /**
     * 
     * Shutdown the equipment
     * 
     * @param port
     *            IPort
     * @return 0 on success and 1 on failure.
     */
    int shutdown(IPort port);

    /**
     * List the dumpfiles which are not downloaded by comparing the files. Adds a suffix to the file
     * name with current download status.
     * 
     * @param port
     *            IPort
     * @return returns a list of files with downloaded status information appended or null on
     *         failure.
     */
    List<String> listFile(IPort port);

    /**
     * Download the coredump file from the ME
     * 
     * @param port
     *            IPort
     * @param coreFileName
     *            name of the coredump. If "all" all coredumps on the ftp server will be downloaded.
     * @param localFilePath
     *            Local file path where to download
     * @param isAutoDelete
     *            Is auto delete set to delete the file after download
     * @param listener
     *            FTP communication listener
     * 
     * @return 0 on success and 1 on failure.
     */
    int downloadDump(IPort port, String coreFileName, String localFilePath, boolean isAutoDelete,
        IFTPCommunicatorListener listener);

    /**
     * Delete the core file from the ME
     * 
     * @param port
     *            IPort
     * @param coreFileName
     *            name of the core file. If "all" all coredumps on the ftp server will be deleted.
     * 
     * @return 0 on success and 1 on failure.
     */
    int deleteDump(IPort port, String coreFileName);

    /**
     * Get listener for ftp communication
     * 
     * @return An object of IFTPCommunicationListener
     */
    IFTPCommunicatorListener getCommunicationListener();
}
