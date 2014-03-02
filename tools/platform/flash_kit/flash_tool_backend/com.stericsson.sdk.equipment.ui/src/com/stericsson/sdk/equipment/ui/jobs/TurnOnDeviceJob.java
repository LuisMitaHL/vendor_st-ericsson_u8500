/**
 * 
 */
package com.stericsson.sdk.equipment.ui.jobs;

import java.util.HashMap;

import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.runtime.jobs.Job;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.MessageBox;
import org.eclipse.swt.widgets.Shell;

import com.stericsson.sdk.equipment.IEquipment;
import com.stericsson.sdk.equipment.ui.Activator;
import com.stericsson.sdk.equipment.ui.JobsArgHashKeys;

/**
 * @author xhelciz
 * 
 */
public class TurnOnDeviceJob extends Job {

    private IProgressMonitor progressMonitor;

    private boolean serviceMode;

    String result;

    /**
     * Constructor
     * 
     */
    public TurnOnDeviceJob() {
        super("Turn On Device");
    }

    /**
     * Custom constructor used by job factories
     * 
     * @param args
     *            arguments of constructor in the order the constructor accepts them
     * @param pEquipment
     *            equipment on which the job should be run
     * @throws NoSuchMethodException
     *             thrown if the arguments do not fit to the arguments requested by constructor
     */
    public TurnOnDeviceJob(HashMap<String, String> args, IEquipment pEquipment) throws NoSuchMethodException {
        super("Turn On Device");

        if (args == null || args.isEmpty()) {
            throw new NoSuchMethodException("Undefined constructor!");
        }

        if (args.containsKey(JobsArgHashKeys.SERVICE_MODE)) {
            serviceMode = Boolean.parseBoolean(args.get(JobsArgHashKeys.SERVICE_MODE));
        } else {
            throw new NoSuchMethodException("Missing argumet: " + JobsArgHashKeys.SERVICE_MODE + "!");
        }
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected IStatus run(IProgressMonitor pMonitor) {
        progressMonitor = pMonitor;
        progressMonitor.setTaskName("Waiting...");

        try {

            final Display display = Activator.getDefault().getWorkbench().getDisplay();
            display.syncExec(new Runnable() {

                public void run() {
                    MessageBox turnOn = new MessageBox(new Shell(display), SWT.ICON_INFORMATION);
                    if (serviceMode) {
                        turnOn.setMessage("Turn On connected device in service mode and press OK.");
                    } else {
                        turnOn.setMessage("Turn On connected device in normal mode and press OK.");
                    }
                    if (turnOn.open() == SWT.OK) {
                        result = "OK";
                    } else {
                        result = "CANCEL";
                    }
                }

            });

            return new Status(IStatus.OK, Activator.PLUGIN_ID, result);
        } catch (Exception e) {
            return new Status(IStatus.ERROR, Activator.PLUGIN_ID, "Command failed", e);
        } finally {
            progressMonitor.done();
        }

        // return null;
    }
}
