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
import com.stericsson.sdk.equipment.ui.jobs.ReadGlobalDataSetJob;
import com.stericsson.sdk.equipment.ui.listeners.IBackendViewer;

/**
 * Read GDFS Area action
 *
 * @author xhelciz
 *
 */
public class ReadGDFSAreaAction extends BackendAction {

    /**
     * Constructor
     *
     * @param pViewer
     *            the equipment viewer
     */
    public ReadGDFSAreaAction(IBackendViewer pViewer) {
        super(pViewer);
        setText("Read GDFS Area...");
        setToolTipText("Read GDFS area from selected equipment and save it to file");
        // setImageDescriptor(Activator.getDefault().getImageRegistry().getDescriptor(Activator.EQUIPMENT_ICON_DUMP));
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void run() {
        IEquipment equipment = viewer.getSelectedEquipment();
        if (equipment != null) {

            FileDialog fd = new FileDialog(Display.getDefault().getActiveShell(), SWT.SAVE);
            fd.setFilterExtensions(new String[] {
                "*.gdf", "*.bin"});
            fd.setOverwrite(true);
            String fileName = fd.open();
            if (fileName == null) {
                return;
            }

            Job job = new ReadGlobalDataSetJob(fileName, equipment, "gdfs");
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
