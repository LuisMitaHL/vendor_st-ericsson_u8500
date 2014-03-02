/**
 * 
 */
package com.stericsson.sdk.equipment.ui.jobs;

import java.util.HashMap;

import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.runtime.jobs.Job;

import com.stericsson.sdk.backend.remote.executor.backend.SetActiveProfileExecutor;
import com.stericsson.sdk.brp.AbstractCommand;
import com.stericsson.sdk.brp.CommandFactory;
import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.brp.InvalidSyntaxException;
import com.stericsson.sdk.equipment.IEquipment;
import com.stericsson.sdk.equipment.ui.Activator;
import com.stericsson.sdk.equipment.ui.JobsArgHashKeys;

/**
 * @author xhelciz
 * 
 */
public class SetActiveProfileJob extends Job {

    private String profileName;

    private IProgressMonitor progressMonitor;

    /**
     * Constructor
     * 
     * @param pProfileName
     *            the profile to be set
     */
    public SetActiveProfileJob(String pProfileName) {
        super("Set active profile to " + pProfileName);
        profileName = pProfileName;
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
    public SetActiveProfileJob(HashMap<String, String> args, IEquipment pEquipment) throws NoSuchMethodException {
        super("Set active profile.");

        if (args == null || args.isEmpty()) {
            throw new NoSuchMethodException("Undefined constructor!");
        }
        if (args.containsKey(JobsArgHashKeys.TARGET_PROFILE)) {
            profileName = args.get(JobsArgHashKeys.TARGET_PROFILE);
        } else {
            throw new NoSuchMethodException("Missing argumet: " + JobsArgHashKeys.TARGET_PROFILE + "!");
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

            SetActiveProfileExecutor executor = new SetActiveProfileExecutor();
            executor.setCommand(createCommand(profileName));
            String result = executor.execute();
            return new Status(IStatus.OK, Activator.PLUGIN_ID, result);
        } catch (Exception e) {
            return new Status(IStatus.ERROR, Activator.PLUGIN_ID, "Command failed", e);
        } finally {
            progressMonitor.done();
        }

    }

    private AbstractCommand createCommand(String param) throws InvalidSyntaxException {
        String completeString = CommandName.BACKEND_SET_ACTIVE_PROFILE.name() + AbstractCommand.DELIMITER + param;
        return CommandFactory.createCommand(completeString);
    }
}
