package com.stericsson.sdk.equipment.ui.actions.system;

import com.stericsson.sdk.equipment.ui.listeners.IBackendViewer;

/**
 * @author xmicroh
 *
 */
public class NormalModeRebootAction extends RebootAction {

    /**
     * @param pViewer TBD
     */
    protected NormalModeRebootAction(IBackendViewer pViewer) {
        super(pViewer);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public String getActionText() {
        return "Normal mode";
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public String getActionToolTipText() {
        return "Reboot equipment in normal mode";
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public int getRestartMode() {
        return 0;
    }

}
