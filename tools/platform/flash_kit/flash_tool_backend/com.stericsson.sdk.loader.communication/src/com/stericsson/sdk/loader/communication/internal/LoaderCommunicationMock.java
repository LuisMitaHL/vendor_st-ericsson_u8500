package com.stericsson.sdk.loader.communication.internal;

import java.io.File;
import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.List;

import org.apache.log4j.Logger;

import com.stericsson.sdk.equipment.io.port.IPort;
import com.stericsson.sdk.loader.communication.Activator;
import com.stericsson.sdk.loader.communication.IAuthenticate;
import com.stericsson.sdk.loader.communication.ILoaderCommunicationListener;
import com.stericsson.sdk.loader.communication.ILoaderCommunicationService;
import com.stericsson.sdk.loader.communication.LCCommandMappings;
import com.stericsson.sdk.loader.communication.LCDefinitions;
import com.stericsson.sdk.loader.communication.types.ListDevicesType;
import com.stericsson.sdk.loader.communication.types.subtypes.SupportedCommand;
import com.stericsson.sdk.common.FileUtils;

/**
 * @author xdancho
 * 
 */
public class LoaderCommunicationMock implements ILoaderCommunicationService, ILoaderCommunicationMock {

    Object result = null;

    boolean forceError = false;

    private static Logger logger = Logger.getLogger(LoaderCommunicationMock.class.getName());

    boolean forceMessage = false;

    IPort port = null;

    private int delay;

    static List<String> nonImmediateReturnCommands = new ArrayList<String>();

    static Method[] methods = null;

    private static final String PSU_MANFIESTS_FILE = "psu_manifests.bin";

    private static final String DPU_MANIFESTS_FILE = "dpu_manifests.bin";

    private static final String PSU_ME_PATH = ":/boot/";

    private static final String DPU_ME_PATH = ":/bin/";

    static {

        // add commands that should take one second to complete
        nonImmediateReturnCommands.add(LCDefinitions.METHOD_NAME_FLASH_ERASE_AREA);
        // nonImmediateReturnCommands.add(LCDefinitions.METHOD_NAME_FLASH_LIST_DEVICES);
        nonImmediateReturnCommands.add(LCDefinitions.METHOD_NAME_SECURITY_BIND_PROPERTIES);

        try {
            methods =
                Class.forName("com.stericsson.sdk.loader.communication.internal.LoaderCommunicationInterface")
                    .getDeclaredMethods();
        } catch (Exception e) {
            logger
                .debug("failed to find the class: com.stericsson.sdk.loader.communication.internal.LoaderCommunicationInterface");
        }

    }

    private Method getMethod(String command) {
        for (Method m : methods) {
            if (m.getName().equals(command)) {
                return m;

            }
        }
        return null;
    }

    /**
     * (non-Javadoc)
     * 
     * @see com.stericsson.sdk.loader.communication.ILoaderCommunicationService#cleanUp(long)
     *      {@inheritDoc}
     */
    public boolean cleanUp(long instanceId) {

        Object returnValue = result;

        try {
            if (result != null && result instanceof Boolean) {

                return (Boolean) returnValue;
            }

            return true;

        } catch (Exception e) {
            return true;
        } finally {
            reset();
        }
    }

    /**
     * (non-Javadoc)
     * 
     * @see com.stericsson.sdk.loader.communication.ILoaderCommunicationService#getSupportedCommands(long)
     *      {@inheritDoc}
     */
    @SuppressWarnings("unchecked")
    public List<SupportedCommand> getSupportedCommands(long instanceId) {

        Object returnValue = result;

        try {
            if (result != null && result instanceof List<?>) {

                return (List<SupportedCommand>) returnValue;
            }
            return null;
        } catch (Exception e) {
            return null;
        } finally {
            reset();
        }
    }

    /**
     * (non-Javadoc)
     * 
     * @see com.stericsson.sdk.loader.communication.ILoaderCommunicationService#getTimeouts(java.lang.String)
     *      {@inheritDoc}
     */
    public long getTimeouts(String type) {
        Object returnValue = result;

        try {
            if (result != null && result instanceof Long) {

                return (Long) returnValue;
            }
            return 0;
        } catch (Exception e) {
            return 0;
        } finally {
            reset();
        }

    }

    private void reset() {
        result = null;
        forceError = false;
        forceMessage = false;
        port = null;
        delay = 0;

    }

    /**
     * (non-Javadoc)
     * 
     * @see com.stericsson.sdk.loader.communication.ILoaderCommunicationService#initialize
     *      (com.stericsson.sdk.equipment.io.port.IPort) {@inheritDoc}
     */
    public long initialize(IPort p) {
        Object returnValue = result;

        try {
            if (result != null && result instanceof Integer) {

                return (Integer) returnValue;
            }
            return 1234;
        } catch (Exception e) {
            return 1234;
        }
    }

    /**
     * (non-Javadoc) {@inheritDoc}
     * 
     * @see com.stericsson.sdk.loader.communication.ILoaderCommunicationService#sendCommand(long,
     *      java.lang.String, java.lang.Object[],
     *      com.stericsson.sdk.loader.communication.ILoaderCommunicationListener) {@inheritDoc}
     */
    public Object sendCommand(long instanceId, String methodName, Object[] args, ILoaderCommunicationListener listener) {

        // to overcome the boot step
        if (methodName.equals(LCDefinitions.METHOD_NAME_SYSTEM_LOADER_STARTUP_STATUS)) {
            return 0;
        }

        if (testError(methodName, args, listener)) {
            return null;
        }

        return sendCommandHelper(instanceId, methodName, args, listener);
    }

    /**
     * Exists only to cheat cyclomatic complexity, will have to be done in a better way...
     * 
     */
    private Object sendCommandHelper(long instanceId, String methodName, Object[] args,
        ILoaderCommunicationListener listener) {
        Object returnValue = 0;

        try {

            // if it is a bulk command, make the command last for 2,5 sec
            if (LCCommandMappings.isBulkCommand(methodName)) {

                long length = 0;
                if (methodName.equalsIgnoreCase(LCDefinitions.METHOD_NAME_FLASH_DUMP_AREA)) {
                    length = (Long) args[2];

                    sendProgress(listener, length);
                } else {
                    sendProgress(listener);
                }

                if (forceMessage) {
                    listener.loaderCommunicationMessage("title", "message from LC mock class", true);
                }

                // this will simulate that the command takes some time to complete
            } else if (methodName.equalsIgnoreCase(LCDefinitions.METHOD_NAME_SYSTEM_AUTHENTICATE)) {
                IAuthenticate authenticate = (IAuthenticate) args[1];
                authenticate.getChallengeResponse(new byte[] {
                    0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, (byte) 0x88, (byte) 0x99, (byte) 0xAA, (byte) 0xBB,
                    (byte) 0xCC, (byte) 0xDD, (byte) 0xEE, (byte) 0xFF, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
                    (byte) 0x88, (byte) 0x99, (byte) 0xAA, (byte) 0xBB, (byte) 0xCC, (byte) 0xDD, (byte) 0xEE,
                    (byte) 0xFF, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, (byte) 0x88, (byte) 0x99, (byte) 0xAA,
                    (byte) 0xBB, (byte) 0xCC, (byte) 0xDD, (byte) 0xEE, (byte) 0xFF});

            } else if (nonImmediateReturnCommands.contains(methodName)) {
                Thread.sleep(2500);
            } else if (methodName.equalsIgnoreCase(LCDefinitions.METHOD_NAME_FLASH_LIST_DEVICES) && result == null) {
                returnValue = commandListDevices();
            } else if (methodName.equalsIgnoreCase(LCDefinitions.METHOD_NAME_FS_READ_LOAD_MODULE_MANIFESTS)
                && result == null) {
                returnValue = commandReadLoadModuleManifests(args);
            }

            // if (delay != 0) {
            Thread.sleep(delay);
            // }

            if (result != null) {
                return result;
            } else {
                return returnValue;
            }

        } catch (Exception e) {
            System.out.println(e.getMessage());
            return null;
        } finally {
            reset();
        }
    }

    private Integer commandReadLoadModuleManifests(Object[] args) {
        try {
            File psuManifests = new File(Activator.getResourcesPath(), PSU_MANFIESTS_FILE);
            File dpuManifests = new File(Activator.getResourcesPath(), DPU_MANIFESTS_FILE);

            // first argument describes where to store the file on local disk
            // second argument contains the path on ME that contains the load modules
            if (args.length != 2) {
                throw new RuntimeException("Command " + LCDefinitions.METHOD_NAME_FS_READ_LOAD_MODULE_MANIFESTS
                    + " was invoked with wrong number of arguments");
            }

            File outputFile = new File((String) args[0]);
            File manifestsFile;
            String pathOnME = (String) args[1];
            if (pathOnME.equalsIgnoreCase(PSU_ME_PATH)) {
                manifestsFile = psuManifests;
            } else if (pathOnME.equalsIgnoreCase(DPU_ME_PATH)) {
                manifestsFile = dpuManifests;
            } else {
                throw new RuntimeException("Neither PSU nor DPU path was given in the argument");
            }

            FileUtils.copyFile(manifestsFile, outputFile);
        } catch (Exception e) {
            logger.error("Reading of load module manifests has failed", e);
            return -1;
        }

        return 0;
    }

    private Object commandListDevices() {
        ListDevicesType devices = new ListDevicesType(0, 3);
        devices.addDevice("/flash0/test2", "/flash0/test2".length(), "Partition", "partition".length(), 100, 0, 50000);
        devices.addDevice("/flash0/test", "/flash0/test".length(), "Partition", "partition".length(), 100, 0, 20000);
        devices.addDevice("/flash0/test/test2", "/flash0/test/test2".length(), "Partition", "partition".length(), 100,
            20000, 30000);
        devices.addDevice("/wrong/", "/wrong/".length(), "wrong", "wrong".length(), 100, 0, 10000);
        result = devices;
        return result;
    }

    private boolean testError(String methodName, Object[] args, ILoaderCommunicationListener listener) {
        if (forceError) {
            listener.loaderCommunicationError(port, "Error on port");
            return true;
        }

        boolean error = checkParams(methodName, args);

        if (error) {
            return true;
        }

        return false;
    }

    /**
     * (non-Javadoc)
     * 
     * @see com.stericsson.sdk.loader.communication.internal.ILoaderCommunicationMock#setDelay(int)
     *      {@inheritDoc}
     */
    public void setDelay(int delayInMs) {
        delay = delayInMs;
    }

    private boolean checkParams(String methodName, Object[] args) {
        Method m = getMethod(methodName);

        if (m == null) {
            logger.debug("Could not find the method: " + methodName + " in LoaderCommunicationInterface");
            return true;
        }

        Class<?>[] parameters = m.getParameterTypes();

        try {
            for (int i = 1; i < parameters.length; i++) {

                // if the observer is set as null
                if (args[i - 1] == null && i == parameters.length - 1) {
                    continue;
                }
                if (!isEqual(parameters[i], args[i - 1].getClass())) {
                    logger.debug("the parameter input does not match the required parameters in method: " + methodName);
                    return true;
                }

            }
        } catch (Exception e) {
            logger.debug(e.getMessage());
        }
        return false;
    }

    private void sendProgress(ILoaderCommunicationListener listener) throws InterruptedException {
        sendProgress(listener, 1000000);
    }

    private void sendProgress(ILoaderCommunicationListener listener, long length) throws InterruptedException {
        // send some status
        for (int i = 1; i < 6; i++) {
            Thread.sleep(500);
            listener.loaderCommunicationProgress(length, i * (length / 5));
        }
    }

    /**
     * 
     * 
     * @param type1
     *            type
     * @param type2
     *            type
     * @return if the types are equal
     */
    public static boolean isEqual(Class<?> type1, Class<?> type2) {

        String type1str = "";
        String type2str = "";

        if (type1.isInterface()) {
            Class<?>[] interfaces = type2.getInterfaces();
            for (Class<?> i : interfaces) {
                if (i.getName().equals(type1.getName())) {
                    return true;
                }
            }
        }

        if (type1.isPrimitive()) {

            type1str = getTypeClass(type1).toString();

        } else {
            type1str = type1.toString();
        }

        if (type2.isPrimitive()) {

            type2str = getTypeClass(type2).toString();

        } else {
            type2str = type2.toString();
        }

        return type1str.equals(type2str);
    }

    /**
     * get the class of a primitive type
     * 
     * @param type
     *            the primitive type
     * @return the object class
     */
    public static Class<?> getTypeClass(Class<?> type) {

        try {
            if (type.toString().equals("byte")) {
                return Byte.class;
            } else if (type.toString().equals("short")) {
                return Short.class;
            } else if (type.toString().equals("int")) {
                return Integer.class;
            } else if (type.toString().equals("long")) {
                return Long.class;
            } else if (type.toString().equals("char")) {
                return Character.class;
            } else if (type.toString().equals("float")) {
                return Float.class;
            } else if (type.toString().equals("double")) {
                return Double.class;
            } else if (type.toString().equals("boolean")) {
                return Boolean.class;
            } else {

                return Object.class;
            }

        } catch (Exception e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
            return Object.class;
        }
    }

    /**
     * {@inheritDoc}
     */
    public void sendRawData(long instanceId, byte[] rawData) {
        reset();
    }

    /**
     * (non-Javadoc)
     * 
     * @see com.stericsson.sdk.loader.communication.ILoaderCommunicationService#setTimeouts(java.lang.String,
     *      long) {@inheritDoc}
     */
    public void setTimeouts(String type, long value) {
        reset();

    }

    /**
     * (non-Javadoc)
     * 
     * @see com.stericsson.sdk.loader.communication.internal.ILoaderCommunicationMock#setResult(java.lang.Object)
     *      {@inheritDoc}
     */
    public void setResult(Object resultIn) {
        this.result = resultIn;
    }

    /**
     * (non-Javadoc)
     * 
     * @see com.stericsson.sdk.loader.communication.internal.ILoaderCommunicationMock#forceError
     *      (com.stericsson.sdk.equipment.io.port.IPort) {@inheritDoc}
     */
    public void forceError(IPort portIn) {
        this.port = portIn;
        this.forceError = true;
    }

    /**
     * (non-Javadoc)
     * 
     * @see com.stericsson.sdk.loader.communication.internal.ILoaderCommunicationMock#forceMessage()
     *      {@inheritDoc}
     */
    public void forceMessage() {
        this.forceMessage = true;
    }

    /**
     * {@inheritDoc}
     */
    public String getLoaderErrorDesc(int error) {
        switch (error) {
            case 0:
                return "Operation successful. Operation finished successfully.";
            case 1:
                return "Loader Communication Status 1: General Failure. Unknown error.";
            case 5:
                return "Loader Communication Status 5: ";
            case 56:
                return "Loader Communication Status 56: N/A";
            default:
                return "N/A";
        }
    }

}
