/**
 * 
 */
package com.stericsson.sdk.equipment.ui.jobs;

import java.util.HashMap;

import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.runtime.jobs.Job;

import com.stericsson.sdk.backend.remote.executor.backend.DisableAutoSenseExecutor;
import com.stericsson.sdk.backend.remote.executor.backend.EnableAutoSenseExecutor;
import com.stericsson.sdk.equipment.IEquipment;
import com.stericsson.sdk.equipment.ui.Activator;
import com.stericsson.sdk.equipment.ui.JobsArgHashKeys;

/**
 * @author xhelciz
 * 
 */
public class SetAutoSenseJob extends Job {
    private String autosense;

    private IProgressMonitor progressMonitor;

    /**
     * Constructor
     * 
     * @param pAutosense
     *            the profile to be set
     */
    public SetAutoSenseJob(String pAutosense) {
        super("Set Auto Sense " + pAutosense);
        autosense = pAutosense;
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
    public SetAutoSenseJob(HashMap<String, String> args, IEquipment pEquipment) throws NoSuchMethodException {
        super("Set active profile.");

        if (args == null || args.isEmpty()) {
            throw new NoSuchMethodException("Undefined constructor!");
        }

        if (args.containsKey(JobsArgHashKeys.VALUE)) {
            autosense = args.get(JobsArgHashKeys.VALUE);
        } else {
            throw new NoSuchMethodException("Missing argumet: " + JobsArgHashKeys.VALUE + "!");
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
            String result;
            if (autosense.equalsIgnoreCase("on")) {
                EnableAutoSenseExecutor executor = new EnableAutoSenseExecutor();
                result = executor.execute();
            } else {
                DisableAutoSenseExecutor executor = new DisableAutoSenseExecutor();
                result = executor.execute();
            }

            return new Status(IStatus.OK, Activator.PLUGIN_ID, result);
        } catch (Exception e) {
            return new Status(IStatus.ERROR, Activator.PLUGIN_ID, "Command failed", e);
        } finally {
            progressMonitor.done();
        }

    }
}
