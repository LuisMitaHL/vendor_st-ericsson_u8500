package com.stericsson.sdk.equipment.ui.actions.system;

import com.stericsson.sdk.equipment.IEquipment;
import com.stericsson.sdk.equipment.ui.actions.BackendAction;
import com.stericsson.sdk.equipment.ui.jobs.RebootJob;
import com.stericsson.sdk.equipment.ui.listeners.IBackendViewer;

/**
 * Reboot Action
 * 
 * @author xolabju
 * 
 */
public abstract class RebootAction extends BackendAction {

    /**
     * Constructor
     * 
     * @param pViewer
     *            the equipment viewer
     */
    protected RebootAction(IBackendViewer pViewer) {
        super(pViewer);
        setText(getActionText());
        setToolTipText(getActionToolTipText());
    }

    /**
     * @return Text of inherited action.
     */
    public abstract String getActionText();

    /**
     * @return ToolTip text for inherited action.
     */
    public abstract String getActionToolTipText();

    /**
     * @return Identifier of reboot mode.
     */
    public abstract int getRestartMode();

    /**
     * @param pViewer TBD
     * @return TBD
     */
    public static RebootAction[] getRebootActions(IBackendViewer pViewer) {
        RebootAction[] actions = new RebootAction[] {
            new NormalModeRebootAction(pViewer),
            new ServiceModeRebootAction(pViewer),
            new NormalModeWithJTAGRebootAction(pViewer),
            new ServiceModeWithJTAGRebootAction(pViewer)
        };
        return actions;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void run() {

        IEquipment equipment = viewer.getSelectedEquipment();

        if (equipment != null) {
            new RebootJob(equipment, getRestartMode()).schedule();
        }
    }

}
