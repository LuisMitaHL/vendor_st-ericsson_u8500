package com.stericsson.sdk.equipment.ui.actions.backend;

import com.stericsson.sdk.equipment.ui.actions.BackendAction;
import com.stericsson.sdk.equipment.ui.listeners.IBackendViewer;

/**
 * Get Equipment Properties Action
 * 
 * @author xolabju
 * 
 */
public class GetEquipmentPropertiesAction extends BackendAction {

    /**
     * Constructor
     * 
     * @param pViewer
     *            the equipment viewer
     */
    public GetEquipmentPropertiesAction(IBackendViewer pViewer) {
        super(pViewer);
        setText("Properties");
        setToolTipText("Display properties for selected equipment");
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void run() {
        // TODO: implement
    }
}
