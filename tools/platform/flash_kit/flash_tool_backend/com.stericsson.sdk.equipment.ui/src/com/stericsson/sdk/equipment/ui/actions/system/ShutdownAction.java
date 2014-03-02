package com.stericsson.sdk.equipment.ui.actions.system;

import com.stericsson.sdk.equipment.IEquipment;
import com.stericsson.sdk.equipment.ui.Activator;
import com.stericsson.sdk.equipment.ui.actions.BackendAction;
import com.stericsson.sdk.equipment.ui.jobs.ShutdownJob;
import com.stericsson.sdk.equipment.ui.listeners.IBackendViewer;

/**
 * Shutdown Action
 * 
 * @author xolabju
 * 
 */
public class ShutdownAction extends BackendAction {

    /**
     * Constructor
     * 
     * @param pViewer
     *            the equipment viewer
     */
    public ShutdownAction(IBackendViewer pViewer) {
        super(pViewer);
        setText("Shutdown");
        setToolTipText("Shutdown selected equipment");
        setImageDescriptor(Activator.getDefault().getImageRegistry().getDescriptor(Activator.EQUIPMENT_ICON_SHUTDOWN));
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void run() {
        IEquipment equipment = viewer.getSelectedEquipment();
        if (equipment != null) {
            ShutdownJob job = new ShutdownJob(equipment);
            job.schedule();
        }
    }
}
