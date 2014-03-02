package com.stericsson.sdk.backend.remote.executor.coredump;

import java.io.IOException;

import org.apache.log4j.Logger;

import com.stericsson.sdk.backend.remote.executor.LCCommandExecutor;
import com.stericsson.sdk.equipment.IEquipmentTask;

/**
 * Remote command executor for download dump
 * 
 * @author esrimpa
 * 
 */
public class DownloadDumpExecutor extends LCCommandExecutor {

    static final Logger log = Logger.getLogger(DownloadDumpExecutor.class);

    long lastProgressTime;

    long lastProgressBytes;

    private long filesize = -1;

    /**
     * {@inheritDoc}
     */
    public void taskStart(IEquipmentTask task) {
        if (task == getExecutedTask()) {
            lastProgressTime = System.currentTimeMillis();
        }

    }

    /**
     * {@inheritDoc}
     */
    public void taskProgress(IEquipmentTask task, long totalBytes, long transferredBytes) {
        if (task == getExecutedTask()) {
            if (filesize == -1) {
                filesize = totalBytes;
            }
            filesize = totalBytes;
            long currentTimeMillis = System.currentTimeMillis();
            long time = currentTimeMillis - lastProgressTime;
            long bytes = transferredBytes - lastProgressBytes;
            double speed = 0;
            double seconds = (time / 1000.0);
            if ((seconds != 0) && (bytes > 0)) {
                speed = bytes / seconds;
            }
            double percent = 0;
            if (totalBytes != 0) {
                percent = (((double) transferredBytes / (double) totalBytes) * 100);
            }

            try {
                progress((int) percent, (long) speed);
            } catch (IOException e) {
                log.error(e.getMessage());
            }
            lastProgressBytes = transferredBytes;
            lastProgressTime = currentTimeMillis;
        }

    }

}
