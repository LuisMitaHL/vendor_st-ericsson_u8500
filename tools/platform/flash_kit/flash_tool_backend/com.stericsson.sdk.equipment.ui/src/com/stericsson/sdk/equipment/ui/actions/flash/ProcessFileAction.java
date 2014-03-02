package com.stericsson.sdk.equipment.ui.actions.flash;

import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.FileDialog;

import com.stericsson.sdk.equipment.IEquipment;
import com.stericsson.sdk.equipment.ui.Activator;
import com.stericsson.sdk.equipment.ui.actions.BackendAction;
import com.stericsson.sdk.equipment.ui.jobs.ProcessFileJob;
import com.stericsson.sdk.equipment.ui.listeners.IBackendViewer;

/**
 * Process File Action
 * 
 * @author xolabju
 * 
 */
public class ProcessFileAction extends BackendAction {

    /**
     * Constructor
     * 
     * @param pViewer
     *            the equipment viewer
     */
    public ProcessFileAction(IBackendViewer pViewer) {
        super(pViewer);
        setText("Process File to Flash...");
        setToolTipText("Flash specified flash archive to selected equipment");
        setImageDescriptor(Activator.getDefault().getImageRegistry().getDescriptor(Activator.EQUIPMENT_ICON_PROCESS));
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void run() {
        IEquipment equipment = viewer.getSelectedEquipment();
        if (equipment != null) {
            FileDialog dialog = new FileDialog(Display.getDefault().getActiveShell(), SWT.OPEN);
            dialog.setFilterExtensions(new String[] {
                "*.zip", "*.*"});
            dialog.setText("Select file to flash to " + equipment.toString());
            String fileName = dialog.open();
            if (fileName != null) {
                ProcessFileJob job = new ProcessFileJob(fileName, equipment);
                job.schedule();
            }
        }
    }
}
