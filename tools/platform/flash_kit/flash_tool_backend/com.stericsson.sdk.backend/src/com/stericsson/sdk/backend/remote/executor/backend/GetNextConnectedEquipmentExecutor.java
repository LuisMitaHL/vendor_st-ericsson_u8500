package com.stericsson.sdk.backend.remote.executor.backend;

import java.io.IOException;

import org.apache.log4j.Logger;
import org.osgi.framework.BundleContext;
import org.osgi.framework.ServiceRegistration;

import com.stericsson.sdk.backend.Activator;
import com.stericsson.sdk.backend.config.BackendConfiguration;
import com.stericsson.sdk.backend.remote.executor.ServerCommandExecutor;
import com.stericsson.sdk.brp.AbstractCommand;
import com.stericsson.sdk.brp.ExecutionException;
import com.stericsson.sdk.equipment.EquipmentState;
import com.stericsson.sdk.equipment.IEquipment;
import com.stericsson.sdk.equipment.IEquipmentBootedListener;
import com.stericsson.sdk.equipment.IEquipmentTask;
import com.stericsson.sdk.equipment.IPlatform;
import com.stericsson.sdk.equipment.io.DeadPort;

/**
 * Executor for getting the active profile
 * 
 * @author xolabju
 * 
 */
public class GetNextConnectedEquipmentExecutor extends ServerCommandExecutor implements IEquipmentBootedListener {

    /** */
    public static final String COMMAND_NAME = "BACKEND_GET_NEXT_CONNECTED_EQUIPMENT";

    private boolean isDone;

    private boolean bootFailed = false;

    private volatile String resultMessage;

    static final Logger log = Logger.getLogger(GetNextConnectedEquipmentExecutor.class);

    private static final int SECOND_CONNECTED_EQUIPMENT_TIMEOUT = 4000;

    private Thread timeoutThread = null;

    /**
     * {@inheritDoc}
     */
    public String execute() throws ExecutionException {

        BundleContext context = Activator.getBundleContext();
        ServiceRegistration registration = null;

        BackendConfiguration backendConfiguration = BackendConfiguration.getInstance();
        int timeToWait = BackendConfiguration.DEFAULT_TIME_TO_WAIT_BOOT_TASK_FAIL;
        try{
            timeToWait = Integer.valueOf(backendConfiguration.getProperty(BackendConfiguration.TIME_TO_WAIT_BOOT_TASK_FAIL));
        }catch(Exception e){log.error(e.getMessage());}

        try {
            // register this class as a task listener
            registration = context.registerService(IEquipmentBootedListener.class.getName(), this, null);

            if (!isDone) {
                log.info("Waiting for next connected ME.");
            }

            // lets wait until boot task for next connected ME will be finished
            int i = 0;
            while (!isDone) {
                Thread.sleep(100);
                //Wait timeToWait*100 seconds before displaying the message to check cables
                i++;

                if(i == timeToWait){
                    try {
                        progressDebugMessage("CHECK DEVICE AND/OR CABLES");
                    } catch (IOException e) {
                        // TODO Auto-generated catch block
                        log.error(e.getMessage());
                    }
                    i = 0;
                }
            }
        } catch (InterruptedException e) {
            throw new ExecutionException(e.getMessage());
        } finally {
            if (registration != null) {
                registration.unregister();
            }
        }

        if (bootFailed) {
            throw new ExecutionException(resultMessage);
        }
        return resultMessage;
    }

    /**
     * {@inheritDoc}
     */
    public void taskMessage(IEquipmentTask pTask, String pMessage) {
        // N/A
    }

    /**
     * {@inheritDoc}
     */
    public void taskProgress(IEquipmentTask pTask, long pTotalBytes, long pTransferredBytes) {
        // N/A
    }

    /**
     * {@inheritDoc}
     */
    public void taskStart(IEquipmentTask pTask) {
        // N/A
    }

    /**
     * {@inheritDoc}
     */
    public void newEquipmentBooted(final IEquipment equipment) {
        if (equipment.getStatus().getState() != EquipmentState.ERROR) {
            if (timeoutThread != null) {
                timeoutThread.interrupt();
            }

            resultMessage = equipment.getProperty(IPlatform.PLATFORM_PROPERTY) + AbstractCommand.DELIMITER;
            resultMessage += equipment.getProperty(IPlatform.PORT_PROPERTY) + AbstractCommand.DELIMITER;
            resultMessage += equipment.getProfile().getAlias() + AbstractCommand.DELIMITER;
            resultMessage += equipment.getStatus().getState().name() + AbstractCommand.DELIMITER;
            resultMessage +=
                equipment.getStatus().getStateMessage().trim().equals("") ? "---" + AbstractCommand.DELIMITER
                    : equipment.getStatus().getStateMessage() + AbstractCommand.DELIMITER;

            isDone = true;
        } else {
            // Waiting for limited time for second connected equipment to be successfully booted if
            // the first one was not.
            if (timeoutThread == null) {
                timeoutThread = new Thread(new Runnable() {
                    public void run() {
                        try {
                            Thread.sleep(SECOND_CONNECTED_EQUIPMENT_TIMEOUT);
                        } catch (InterruptedException e) {
                            return;
                        }
                        if (equipment.getPort().getPortName().equals(DeadPort.PORT_ID)){
                            try {
                                progressDebugMessage("INCOMPATIBLE HARDWARE");
                            } catch (IOException e) {
                                log.error(e.getMessage());
                            }
                        }
                        resultMessage = "Boot failed : " + equipment.getStatus().getStateMessage();
                        bootFailed = true;
                        isDone = true;
                    }
                });
                timeoutThread.start();
            }
        }
    }
}
