package com.stericsson.sdk.backend.remote.executor.flash;

import java.io.IOException;

import org.apache.log4j.Logger;

import com.stericsson.sdk.backend.remote.executor.LCCommandExecutor;
import com.stericsson.sdk.backend.remote.executor.ServerCommandExecutor;
import com.stericsson.sdk.brp.ExecutionException;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.equipment.IEquipmentTask;

/**
 * Executor for the command 'dump area' on an equipment
 * 
 * @author xolabju
 * 
 */
public class DumpAreaExecutor extends LCCommandExecutor {

    long lastProgressTime;

    long lastProgressBytes;

    private long dumpSize;

    static Logger logger = Logger.getLogger(ServerCommandExecutor.class);

    /**
     * 
     * {@inheritDoc}
     */
    @Override
    public String execute() throws ExecutionException {
        return super.execute();
    }

    /**
    * {@inheritDoc}
    */
    @Override
    protected String formatOutputMessage(EquipmentTaskResult result) {
        return formatDuration(result.getResultMessage(), result.getDuration(), dumpSize);
    }

    /**
     * {@inheritDoc}
     */
    public void taskProgress(IEquipmentTask task, long totalBytes, long transferredBytes) {
        if (task == getExecutedTask()) {
            dumpSize = totalBytes;
            long currentTimeMillis = System.currentTimeMillis();
            long time = currentTimeMillis - lastProgressTime;
            long bytes = transferredBytes - lastProgressBytes;
            double speed = 0;
            double seconds = (time / 1000.0);
            if (seconds != 0) {
                speed = bytes / seconds;
            }
            double percent = 0;
            if (totalBytes != 0) {
                percent = (((double) transferredBytes / (double) totalBytes) * 100);
            }

            try {
                progress((int) percent, (long) speed);
            } catch (IOException e) {
                logger.error(e.getMessage());
            }
            lastProgressBytes = transferredBytes;
            lastProgressTime = currentTimeMillis;
        }
    }

    /**
     * {@inheritDoc}
     */
    public void taskStart(IEquipmentTask task) {
        if (task == getExecutedTask()) {
            lastProgressTime = System.currentTimeMillis();
        }
    }
}
