package com.stericsson.sdk.equipment.m7x00.tasks;

import java.util.Collection;

import com.stericsson.sdk.equipment.AbstractEquipmentTask;
import com.stericsson.sdk.equipment.AbstractLoaderCommunicationEquipment;
import com.stericsson.sdk.equipment.EquipmentState;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.equipment.m7x00.bsu.BSUUtils;
import com.stericsson.sdk.equipment.m7x00.bsu.LoadModuleDescription;
import com.stericsson.sdk.equipment.m7x00.bsu.BSUUtils.BSUPath;
import com.stericsson.sdk.equipment.m7x00.internal.M7x00Equipment;

/**
 * Task lists all the installed modules on a device.
 * @author xadazim
 *
 */
public class M7X00EquipmentListLoadModulesTask extends AbstractEquipmentTask {

    /**
     * Id of this task.
     */
    public static final String TASK_ID = "LIST_LOAD_MODULES";

    /**
     * Constructs task.
     * 
     * @param pEquipment
     *            equipment from which load modules should be fetched
     */
    public M7X00EquipmentListLoadModulesTask(AbstractLoaderCommunicationEquipment pEquipment) {
        super(pEquipment);
    }

    /**
     * 
     * {@inheritDoc}
     */
    public EquipmentTaskResult execute() {
        notifyTaskStart();
        getEquipment().setStatus(EquipmentState.BUSY);

        StringBuilder resultMessage = new StringBuilder();
        Collection<LoadModuleDescription> loadModules = null;
        boolean errorState = false;

        for (BSUPath p : BSUPath.values()) {

            resultMessage.append("Check modules in path ");
            resultMessage.append(p.getPath() + System.getProperty("line.separator"));

            try {
                loadModules = BSUUtils.getLoadModules((M7x00Equipment) getEquipment(), p.getPath());
                for (LoadModuleDescription desc : loadModules) {
                    resultMessage.append(desc.getModuleName() + ": ");
                    resultMessage.append("type=" + desc.getManifest().getType() + " ");
                    resultMessage.append("name=" + desc.getManifest().getName() + " ");
                    resultMessage.append("version=" + desc.getManifest().getVersion()
                        + System.getProperty("line.separator"));
                }
            } catch (Exception e) {
                resultMessage.append("Error description: " + e.getMessage() + System.getProperty("line.separator"));
                errorState = true;
            }
        }

        getEquipment().setStatus(EquipmentState.IDLE);
        return new EquipmentTaskResult((errorState) ? -1 : 0, resultMessage.toString(), null, false);

    } 
    /**
     * 
     * {@inheritDoc}
     */
    public String getId() {
        return TASK_ID;
    }

    /**
     * 
     * {@inheritDoc}
     */
    public boolean isCancelable() {
        return false;
    }

    /**
     * 
     * {@inheritDoc}
     */
    public void cancel() {
        throw new RuntimeException("Task is not cancellable");
    }

    /**
     * 
     * {@inheritDoc}
     */
    public void setArguments(String[] pArguments) {
    }

}
