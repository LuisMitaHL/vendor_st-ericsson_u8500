package com.stericsson.sdk.backend.remote.executor;

import org.osgi.framework.BundleContext;
import org.osgi.framework.InvalidSyntaxException;
import org.osgi.framework.ServiceReference;

import com.stericsson.sdk.backend.Activator;
import com.stericsson.sdk.brp.AbstractCommand;
import com.stericsson.sdk.brp.ExecutionException;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.equipment.IEquipment;
import com.stericsson.sdk.equipment.IEquipmentTask;
import com.stericsson.sdk.equipment.IEquipmentTaskListener;

/**
 * Remote command executor.
 * 
 * @author pkutac01
 * 
 */
public class LCCommandExecutor extends ServerCommandExecutor implements IEquipmentTaskListener {

    private volatile IEquipment equipment = null;

    private volatile IEquipmentTask executedTask = null;

    /**
     * {@inheritDoc}
     */
    @Override
    public String execute() throws ExecutionException {
        BundleContext context = Activator.getBundleContext();
        String port = getCommand().getValue(AbstractCommand.PARAMETER_EQUIPMENT_ID);
        String filter = "(" + IEquipmentTask.PORT_PROPERTY + "=" + port + ")";

        try {
            ServiceReference[] serviceReferences = context.getServiceReferences(IEquipment.class.getName(), filter);
            if (serviceReferences != null && serviceReferences.length > 0) {
                equipment = (IEquipment) context.getService(serviceReferences[0]);
            } else {
                throw new ExecutionException("Can't get a service reference for the class "
                    + IEquipment.class.getName() + " with the filter " + filter);
            }
        } catch (InvalidSyntaxException e) {
            throw new ExecutionException(e.getMessage());
        }

        if (equipment == null) {
            throw new ExecutionException("No connected equipment on port " + port);
        }

        try {

            // schedule task
            String[] arguments = getCommand().getCommandParts();
            if (arguments == null) {
                throw new ExecutionException("Array of arguments is null.");
            }
            executedTask = equipment.createTask(arguments);
            if (executedTask == null) {
                throw new ExecutionException("Command is not supported by the equipment connected to port " + port);
            }
            executedTask.addTaskListener(this);
            EquipmentTaskResult result = executedTask.execute();

            if (result.getResultCode() != 0) {
                throw new ExecutionException(result.getResultMessage());
            }

            return formatOutputMessage(result);
        } catch (Exception e) {
            throw new ExecutionException(e.getMessage());
        }
    }

    /**
     * Format output message before it is send to client
     * @param result task result
     * @return client message
     */
    protected String formatOutputMessage(EquipmentTaskResult result) {
        // default implementation
        return result.getResultMessage();
    }

    /**
     * {@inheritDoc}
     */
    public void taskStart(IEquipmentTask pTask) {

    }

    /**
     * {@inheritDoc}
     */
    public void taskProgress(IEquipmentTask pTask, long pTotalBytes, long pTransferredBytes) {

    }

    /**
     * {@inheritDoc}
     */
    public void taskMessage(IEquipmentTask pTask, String pMessage) {

    }

    /**
     * Returns equipment or null.
     * 
     * @return Equipment or null.
     */
    public IEquipment getEquipment() {
        return equipment;
    }

    /**
     * Returns executed task or null.
     * 
     * @return Executed task or null.
     */
    public IEquipmentTask getExecutedTask() {
        return executedTask;
    }

}
