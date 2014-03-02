/**
 *
 */
package com.stericsson.sdk.equipment.io.ip.subscription;

import org.apache.log4j.Logger;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.equipment.IEquipment;
import com.stericsson.sdk.equipment.IEquipmentTask;
import com.stericsson.sdk.equipment.IEquipmentTaskListener;
import com.stericsson.sdk.equipment.io.ip.internal.FtpPortHandler;
import com.stericsson.sdk.equipment.io.ip.internal.FtpPortProvider;

/**
 * Handles automatic download and delete tasks.
 *
 * @author eolabor
 *
 */
public final class AutomaticTaskHandler implements IDumpEventListener, IEquipmentTaskListener {

    private static final AutomaticTaskHandler INSTANCE = new AutomaticTaskHandler();

    /** */
    private static Logger log = Logger.getLogger(AutomaticTaskHandler.class);

    private FtpPortHandler portHandler = FtpPortHandler.getInstance();

    private IEquipment equipment;

    private int resultCode;

    private IEquipmentTask downloadDumpTask;

    /**
     * Get instance of this singelton class.
     *
     * @return Instance of this singleton.
     */
    public static synchronized AutomaticTaskHandler getInstance() {
        return INSTANCE;
    }

    /**
     * {@inheritDoc}
     */
    public void dumpingMEdetected(DumpEvent event) {
        if (portHandler.getAutoDownload()) {
            downloadLatestDump(event.getEquipmentId(), event.getDumpPath());
        }
    }

    private void downloadLatestDump(String identifier, String dumpPath) {
        try {
            boolean isAutoDelete = portHandler.getAutoDelete();
            String[] args =
                new String[] {
                    CommandName.COREDUMP_DOWNLOAD_DUMP.name(), identifier, dumpPath,
                    portHandler.getAutoDownloadDirectory(), String.valueOf(isAutoDelete)};
            FtpPortProvider portProvider = portHandler.getFtpPortProvider(identifier);
            equipment = portProvider.getEquipment(identifier);
            // If equipment is just created. We need to wait for the BOOT task.
            if (equipment == null) {
                for (int i = 0; i < 10; i++) {
                    Thread.sleep(100);
                    equipment = portProvider.getEquipment(identifier);
                    if (equipment != null) {
                        break;
                    }
                }
            }
            if (equipment != null) {
                downloadDumpTask = equipment.createTask(args);
                if (downloadDumpTask == null) {
                    throw new NullPointerException("Created task is null.");
                }
                downloadDumpTask.addTaskListener(this);
                EquipmentTaskResult result = downloadDumpTask.execute();
                resultCode = result.getResultCode();

                if (resultCode == 0) {
                    String info =
                        "Notfication type: " + Notification.AUTOMATIC_COREDUMP_DOWNLOADED + " Equipment: " + identifier
                            + " Downloaded dump: " + dumpPath;
                    SubscriptionNotificationService.getInstance().notifySubscribers(
                        Notification.AUTOMATIC_COREDUMP_DOWNLOADED, info);
                } else {
                    log.error("Dumping ME has been detected, but unable to perform automatic download of dump.");
                }
            } else {
                resultCode = -1;
            }
        } catch (InterruptedException e) {
            log.error("Download was interupted");
        }
    }

    /**
     * @return the resultCode
     */
    public int getResultCode() {
        return resultCode;
    }

    /**
     * {@inheritDoc}
     */
    public void taskStart(IEquipmentTask task) {
    }

    /**
     * {@inheritDoc}
     */
    public void taskProgress(IEquipmentTask task, long totalBytes, long transferredBytes) {
    }

    /**
     * {@inheritDoc}
     */
    public void taskMessage(IEquipmentTask task, String message) {
    }
}
