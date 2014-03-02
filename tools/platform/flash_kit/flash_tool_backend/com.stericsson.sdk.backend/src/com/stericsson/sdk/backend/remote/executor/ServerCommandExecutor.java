package com.stericsson.sdk.backend.remote.executor;

import java.io.IOException;
import java.util.Locale;

import org.osgi.framework.BundleContext;
import org.osgi.framework.InvalidSyntaxException;
import org.osgi.framework.ServiceReference;

import com.stericsson.sdk.backend.Activator;
import com.stericsson.sdk.backend.remote.executor.backend.GetNextConnectedEquipmentExecutor;
import com.stericsson.sdk.backend.remote.io.BackendClientHandler;
import com.stericsson.sdk.brp.AbstractCommand;
import com.stericsson.sdk.brp.AbstractCommandExecutor;
import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.brp.ExecutionException;
import com.stericsson.sdk.brp.ICommandExecutor;
import com.stericsson.sdk.brp.ServerResponse;
import com.stericsson.sdk.equipment.EquipmentState;
import com.stericsson.sdk.equipment.IEquipment;
import com.stericsson.sdk.equipment.IEquipmentTask;

/**
 * Abstract class for a remote protocol server command executor
 * 
 * @author xolabju
 * 
 */
public abstract class ServerCommandExecutor extends AbstractCommandExecutor implements Runnable, ICommandExecutor {

    /**
     * Default constructor
     */
    public ServerCommandExecutor() {

    }

    private BackendClientHandler getClientHandler() {
        return (BackendClientHandler) getExecutionHandler();
    }

    /**
     * Writes a message to the client
     * 
     * @param message
     *            the message to write
     * @throws IOException
     *             on errors
     */
    private void writeMessage(String message) throws IOException {
        getClientHandler().writeMessage(message);
    }

    /**
     * Writes an acknowledge message for a specified command
     * 
     * @throws IOException
     *             on errors
     */
    private void writeAcknowledgeMessage() throws IOException {
        writeMessage(getCommand().getCommandString() + AbstractCommand.DELIMITER + ServerResponse.ACK);
    }

    /**
     * Writes a success message for a specified command
     * 
     * @throws IOException
     *             on errors
     */
    public void returnSuccess() throws IOException {
        returnSuccess(null);
    }

    /**
     * Writes a success message for a specified command
     * 
     * @param returnValue
     *            the additonal message to return together with the success response
     * @throws IOException
     *             on errors
     */
    public void returnSuccess(String returnValue) throws IOException {
        getExecutionHandler().done(this);
        writeMessage(getCommand().getCommandString() + AbstractCommand.DELIMITER + ServerResponse.SUCCESS
            + (returnValue == null || returnValue.trim().equals("") ? "" : AbstractCommand.DELIMITER + returnValue));
    }

    /**
     * Writes an error message for a specified command
     * 
     * @param error
     *            the additonal message to return together with the error response
     * @throws IOException
     *             on errors
     */
    public void returnError(String error) throws IOException {
        getExecutionHandler().done(this);
        writeMessage(getCommand().getCommandString() + AbstractCommand.DELIMITER + ServerResponse.ERROR
            + AbstractCommand.DELIMITER + error);
    }

    /**
     * Sends progress to the client
     * 
     * @param percent
     *            percent completed
     * @param speed
     *            the current transmission speed in bytes / second
     * @throws IOException
     *             on errors
     */
    public void progress(int percent, long speed) throws IOException {
        writeMessage(getCommand().getCommandString() + AbstractCommand.DELIMITER + ServerResponse.PROGRESS
            + AbstractCommand.DELIMITER + percent + AbstractCommand.DELIMITER + speed);
    }

    /**
     * Sends progress to the client
     * 
     * @param percent
     *            percent completed
     * 
     * @throws IOException
     *             on errors
     */
    public void progressPercent(int percent) throws IOException {
        writeMessage(getCommand().getCommandString() + AbstractCommand.DELIMITER + ServerResponse.PROGRESS
            + AbstractCommand.DELIMITER + percent + AbstractCommand.DELIMITER + " ");
    }

    /**
     * Sends progress debug message to the client
     * 
     * @param message
     *            message
     * @throws IOException
     *             on errors
     */
    public void progressDebugMessage(String message) throws IOException {
        writeMessage(getCommand().getCommandString() + AbstractCommand.DELIMITER + ServerResponse.PROGRESSDEBUGMESSAGE
            + AbstractCommand.DELIMITER + message);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void run() {
        try {
            writeAcknowledgeMessage();
            String value = getCommand().getValue(AbstractCommand.PARAMETER_EQUIPMENT_ID);
            if ((value != null) && (AbstractCommand.EQUIPMENT_ID_NEXT.equalsIgnoreCase(value))) {
                getCommand().setValue(AbstractCommand.PARAMETER_EQUIPMENT_ID, getNextEquipmentPort());
            } else if ((value != null) && (AbstractCommand.EQUIPMENT_ID_CURRENT.equalsIgnoreCase(value))) {
                try {
                    getCommand().setValue(AbstractCommand.PARAMETER_EQUIPMENT_ID, getCurrentEquipmentPort());
                } catch (ExecutionException e) {
                    returnError(e.getMessage());
                    return;
                }
            }
            super.run();
        } catch (Exception e) {
            //Call LSD_Shutdown function
            try{
                disconnectConnectedEquipments();
            }catch(Exception ex){
                ex.printStackTrace();
            }
            getExecutionHandler().fatalError(getCommand(), e.getMessage());
        }
    }

    private void disconnectConnectedEquipments() throws org.osgi.framework.InvalidSyntaxException, Exception{
        writeMessage("Error occured while executing command: " + getCommand().getCommandString());

        @SuppressWarnings("rawtypes")
        ServiceReference[] serviceReferences = null;
        serviceReferences = Activator.getBundleContext().getServiceReferences(IEquipment.class.getName(), null);

        if (serviceReferences != null) {
            for (int i = 0; i < serviceReferences.length; i++) {
                @SuppressWarnings("unchecked")
                IEquipment equipment = (IEquipment) Activator.getBundleContext().getService(serviceReferences[i]);
                if(equipment != null){
                    String portName = equipment.getPort().getPortName();
                    if(portName != null && portName.length() > 0 && !portName.equalsIgnoreCase("n/a")){
                        executeShutDownTask(portName, equipment);
                    }
                }
            }
        }
    }

    private void executeShutDownTask(String portName, IEquipment equipment) throws IOException{
        IEquipmentTask shutdownTask = equipment.createTask(new String[] {
                CommandName.SYSTEM_SHUTDOWN_EQUIPMENT.name(), portName});
        if(shutdownTask != null){
            shutdownTask.execute();
        }
        writeMessage("Calling shutdown function for equipment id " + portName);
    }

    /**
     * @return port of next connected equipment
     * @throws ExecutionException
     *             when something went wrong
     */
    private String getNextEquipmentPort() throws ExecutionException {
        String equipment = new GetNextConnectedEquipmentExecutor().execute();
        String[] resultParts = equipment.split(";");
        String port = null;
        if (resultParts != null && resultParts.length > 1) {
            port = resultParts[1];
        }
        if (port == null) {
            throw new ExecutionException("Failed to get next connected equipment.");
        }
        return port;
    }

    /**
     * @return port of current connected equipment
     * @throws ExecutionException
     *             if the number of connected equipment != 1
     */
    @SuppressWarnings({"rawtypes", "unchecked"})
    private String getCurrentEquipmentPort() throws ExecutionException {
        String port = null;
        BundleContext context = Activator.getBundleContext();
        IEquipment equipment;

        try {
            ServiceReference[] serviceReferences = context.getServiceReferences(IEquipment.class.getName(), null);
            if (serviceReferences != null && serviceReferences.length > 0) {

                int c = 0;
                ServiceReference result = null;

                for (ServiceReference ref : serviceReferences) {
                    equipment = (IEquipment) context.getService(ref);
                    if (equipment.getStatus().getState() != EquipmentState.ERROR
                        && equipment.getStatus().getState() != EquipmentState.BOOTING) {
                        c++;
                        result = ref;
                    }
                }

                if (c == 0) {
                    throw new ExecutionException("No equipment connected to the backend.");
                }

                if (c > 1) {
                    throw new ExecutionException(
                        "When using unspecified equipment id, the number of connected equipments cannot be more than one.");
                }

                equipment = (IEquipment) context.getService(result);
                if (equipment.getPort() != null) {
                    port = equipment.getPort().getPortName();
                }
            } else {
                throw new ExecutionException("No equipment connected to the backend.");
            }

        } catch (InvalidSyntaxException e) {
            throw new ExecutionException(e.getMessage());
        }

        return port;
    }

    /**
     * @param message
     *            TBD
     * @param duration
     *            TBD
     * @param totalTransferred
     *            TBD
     * @return TBD
     */
    protected String formatDuration(String message, long duration, long totalTransferred) {
        long hours = duration / 3600000;
        long minutes = (duration - hours * 3600000) / 60000;
        long seconds = (duration - hours * 3600000 - minutes * 60000) / 1000;
        long milliseconds = duration - hours * 3600000 - minutes * 60000 - seconds * 1000;

        double transferred = totalTransferred / (1024.0 * 1024.0);
        double speed = (totalTransferred / (duration / 1000.0)) / 1024.0;

        return String.format(Locale.getDefault(), "%s (%.2fMB in %02d:%02d:%02d.%03d, avg. speed %.2fKB/s)", message,
            transferred, hours, minutes, seconds, milliseconds, speed);
    }

    /**
     * 
     * @param port
     *            port
     * @return equipment connected on port
     * @throws ExecutionException .
     */
    @SuppressWarnings({"rawtypes", "unchecked"})
    protected IEquipment getEquipment(String port) throws ExecutionException {
        IEquipment equipment = null;
        BundleContext context = Activator.getBundleContext();

        String filter = "(" + IEquipmentTask.PORT_PROPERTY + "=" + port + ")";
        try {
            ServiceReference[] serviceReferences = context.getServiceReferences(IEquipment.class.getName(), filter);
            if (serviceReferences != null) {
                equipment = (IEquipment) context.getService(serviceReferences[0]);
            }
        } catch (InvalidSyntaxException e) {
            throw new ExecutionException(e.getMessage());
        }
        return equipment;
    }
}
