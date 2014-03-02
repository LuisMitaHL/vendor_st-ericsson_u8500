/**
 *
 */
package com.stericsson.sdk.equipment.ui.actions.flash;

import org.eclipse.core.runtime.jobs.IJobChangeEvent;
import org.eclipse.core.runtime.jobs.IJobChangeListener;
import org.eclipse.core.runtime.jobs.Job;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.FileDialog;

import com.stericsson.sdk.equipment.IEquipment;
import com.stericsson.sdk.equipment.ui.Utils;
import com.stericsson.sdk.equipment.ui.actions.BackendAction;
import com.stericsson.sdk.equipment.ui.jobs.WriteGlobalDataSetJob;
import com.stericsson.sdk.equipment.ui.listeners.IBackendViewer;

/**
 * Write GDFS Area action
 *
 * @author xhelciz
 *
 */
public class WriteGDFSAreaAction extends BackendAction {
    /**
     * Constructor
     *
     * @param pViewer
     *            the equipment viewer
     */
    public WriteGDFSAreaAction(IBackendViewer pViewer) {
        super(pViewer);
        setText("Write GDFS Area...");
        setToolTipText("Read GDFS area data from a file and write it to selected equipment");
        // setImageDescriptor(Activator.getDefault().getImageRegistry().getDescriptor(Activator.EQUIPMENT_ICON_DUMP));
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void run() {
        IEquipment equipment = viewer.getSelectedEquipment();
        if (equipment != null) {

            FileDialog fd = new FileDialog(Display.getDefault().getActiveShell(), SWT.OPEN);
            fd.setFilterExtensions(new String[] {
                "*.gdf", "*.bin"});
            String fileName = fd.open();
            if (fileName == null) {
                return;
            }

            Job job = new WriteGlobalDataSetJob(fileName, equipment, "gdfs");
            job.addJobChangeListener(new IJobChangeListener() {

                public void sleeping(IJobChangeEvent pEvent) {
                }

                public void scheduled(IJobChangeEvent pEvent) {
                }

                public void running(IJobChangeEvent pEvent) {
                }

                public void done(IJobChangeEvent pEvent) {
                    if (!pEvent.getResult().isOK()) {
                        Utils.logAndShowError("Reading/writing of set has failed", pEvent.getResult().getMessage()
                            + "\nError code:" + pEvent.getResult().getCode(), null);
                    }
                }

                public void awake(IJobChangeEvent pEvent) {
                }

                public void aboutToRun(IJobChangeEvent pEvent) {
                }
            });
            job.schedule();

        }
    }
}
