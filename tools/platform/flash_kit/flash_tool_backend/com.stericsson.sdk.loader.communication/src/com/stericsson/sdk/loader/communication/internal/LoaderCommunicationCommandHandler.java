package com.stericsson.sdk.loader.communication.internal;

import java.lang.reflect.Method;
import java.util.Hashtable;
import java.util.Iterator;
import java.util.List;
import java.util.Timer;
import java.util.TimerTask;
import java.util.concurrent.ArrayBlockingQueue;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.Callable;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;

import org.apache.log4j.Logger;

import com.stericsson.sdk.common.EnvironmentProperties;
import com.stericsson.sdk.equipment.io.port.IPort;
import com.stericsson.sdk.loader.communication.ILoaderCommunicationListener;
import com.stericsson.sdk.loader.communication.ILoaderCommunicationService;
import com.stericsson.sdk.loader.communication.LCCommandMappings;
import com.stericsson.sdk.loader.communication.LCDefinitions;
import com.stericsson.sdk.loader.communication.types.subtypes.SupportedCommand;

/**
 * Class responsible for initialization of the LCM and all loader commands sent to an ME
 * 
 * @author xdancho
 */
public class LoaderCommunicationCommandHandler implements ILoaderCommunicationService {

    private static Logger logger = Logger.getLogger(LoaderCommunicationCommandHandler.class.getName());

    private static Class<?> lcInterfaceClass;

    private static Method getInstance;

    private static Method[] methods;

    private static Object lcInstance;

    private static final String INTERFACE_CLASS_NAME =
        "com.stericsson.sdk.loader.communication.internal.LoaderCommunicationInterface";

    private Hashtable<Long, CommandExecution> instanceExecutionMap = new Hashtable<Long, CommandExecution>();

    private Hashtable<Long, ExecutorService> instanceServiceMap = new Hashtable<Long, ExecutorService>();

    private boolean canceledByTimer = false;

    private boolean initialized = false;

    static void initializeStaticFields() {
        try {
            lcInterfaceClass = Class.forName(INTERFACE_CLASS_NAME);

            getInstance = lcInterfaceClass.getDeclaredMethod("getInstance");

            // get all methods
            methods = lcInterfaceClass.getDeclaredMethods();
            lcInstance = getInstance.invoke(null, new Object[] {});
        } catch (Exception e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
    }

    /**
     * constructor
     */
    public void initCommunication() {

        initializeStaticFields();

        initialized = true;

    }

    /**
     * {@inheritDoc}
     * 
     * @throws Exception
     *             exception will be thrown if initialization fails
     */
    public long initialize(IPort port) throws Exception {

        if (!initialized) {
            initCommunication();
        }

        if (LoaderCommunicationInterface.getInstance().isAlreadyInitialized(port)) {
            return 0;
        }

        LoaderCommunicationContext ctx = new LoaderCommunicationContext(port);

        long id = LoaderCommunicationInterface.getInstance().init(ctx);

        // add to execution and service map
        instanceServiceMap.put(id, Executors.newSingleThreadExecutor());
        instanceExecutionMap.put(id, new CommandExecution());

        if (id != 0) {
            ctx.setInstancePointer(id);
        }

        return id;
    }

    /**
     * {@inheritDoc}
     */
    public boolean cleanUp(long instanceId) {
        return LoaderCommunicationInterface.deInit(instanceId);
    }

    /**
     * {@inheritDoc}
     */
    public void sendRawData(long instanceId, byte[] rawData) {
        LoaderCommunicationInterface.write(rawData, rawData.length, 0, instanceId);
    }

    /**
     * {@inheritDoc}
     */
    public Object sendCommand(long instanceId, String command, Object[] args, ILoaderCommunicationListener listener) {

        if (canceledByTimer) {
            return null;
        }
        Method method = null;
        LCCommand cmd = null;
        Timer timer = null;

        method = getMethod(command);

        if (method == null) {
            logger.debug("Command" + command + " not supported by ME.");
            return null;
        }

        cmd = new LCCommand(method, args, command, instanceId, listener);

        // add command to the right
        instanceExecutionMap.get(instanceId).addToQueue(cmd);

        Future<LCCommand> cmdResult = null;

        try {
            cmdResult = instanceServiceMap.get(instanceId).submit(instanceExecutionMap.get(instanceId));

            // if this is not part of long time running commands or a loaderStartupStatus, we start
            // the GR timer
            if (LCCommandMappings.isGeneralResponseTimeoutEnabled(cmd.getCommandString())) {

                timer = new Timer();
                timer.schedule(new CorruptFlashTimer(cmdResult), LCTimeouts.getInstance().getTimeouts(
                    LCDefinitions.COMMAND_GR));
            }

            cmd = checkResult(cmd, cmdResult);

            // cancel timer
            if (LCCommandMappings.isGeneralResponseTimeoutEnabled(cmd.getCommandString())) {
                if (timer != null) {
                    timer.cancel();
                }
            }

        } catch (Exception ee) {
            logger.debug("Failed to get result for commmand: " + cmd.getCommandString());
        } finally {
            if (cmd != null && cmdResult != null && !cmdResult.isCancelled()) {
                return cmd.getResult();
            }
        }
        return null;
    }

    private LCCommand checkResult(LCCommand cmd, Future<LCCommand> cmdResult) {
        try {
            if (cmdResult != null) {
                cmd = cmdResult.get();
            }

        } catch (Exception ce) {
            if (canceledByTimer) {
                canceledByTimer = false;
                logger.debug("No General response for the command " + cmd.getCommandString()
                    + " was received from \nthe ME within the timeout of "
                    + LCTimeouts.getInstance().getTimeouts(LCDefinitions.COMMAND_GR) + " ms.");
            }
        }
        return cmd;
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
     * @author xdancho
     * 
     */
    public class CommandExecution implements Callable<LCCommand> {

        static final int QUEUE_SIZE = 10;

        boolean close = false;

        /**
         * @param cmd
         *            the command to add to queue
         */
        public void addToQueue(LCCommand cmd) {

            if (!EnvironmentProperties.isRunningInTestMode()) {
                logger.debug("Added command " + cmd.getCommandString() + " to queue");
            }
            synchronized (methodQueue) {
                methodQueue.add(cmd);
            }
        }

        /**
         * @param instanceId
         *            Instance ID
         */
        public void cleanUp(long instanceId) {
            synchronized (methodQueue) {
                Iterator<LCCommand> iterator = methodQueue.iterator();
                while (iterator.hasNext()) {
                    LCCommand next = iterator.next();
                    Object object = next.getArguments()[0];
                    if (object instanceof Long) {
                        if (((Long) object) == instanceId) {
                            iterator.remove();
                            System.out.println("Removed" + next.getCommandString());
                        }
                    }
                }
            }
        }

        private BlockingQueue<LCCommand> methodQueue = new ArrayBlockingQueue<LCCommand>(QUEUE_SIZE);

        /**
         * (non-Javadoc)
         * 
         * @see java.util.concurrent.Callable#call()
         * @return the completed command
         * @throws Exception
         *             exception
         */
        public LCCommand call() throws Exception {

            LCCommand cmd = null;
            try {
                synchronized (methodQueue) {
                    cmd = methodQueue.take();
                }

                if (!EnvironmentProperties.isRunningInTestMode()) {
                    logger.debug("removed: " + cmd.getCommandString() + " from queue");
                }
                LoaderCommunicationInterface.addEventListener((Long) cmd.getArguments()[0], cmd.getListener());

                Object result =
                    cmd.getMethod().invoke(LoaderCommunicationCommandHandler.lcInstance, cmd.getArguments());
                if (!EnvironmentProperties.isRunningInTestMode()) {
                    logger.debug("completed command " + cmd.getCommandString() + " from queue");
                }
                cmd.setResult(result);

            } catch (Exception e) {
                e.printStackTrace();
                logger.debug("error: " + e.getMessage());
            } finally {
                if (cmd != null) {
                    LoaderCommunicationInterface.getInstance().removeEventListener(cmd.getListener());
                }
            }

            return cmd;
        }

    }

    /**
     * @author xdancho
     */
    public class CorruptFlashTimer extends TimerTask {

        Future<LCCommand> task = null;

        /**
         * Constructor.
         * 
         * @param t
         *            Task
         */
        public CorruptFlashTimer(Future<LCCommand> t) {
            task = t;
        }

        /**
         * {@inheritDoc}
         */
        public void run() {

            try {
                task.cancel(true);
            } catch (Exception e) {
                e.printStackTrace();
            }

            canceledByTimer = true;
        }

    }

    /**
     * {@inheritDoc}
     */
    public long getTimeouts(String type) {
        return 0;
    }

    /**
     * {@inheritDoc}
     */
    public void setTimeouts(String type, long value) {
    }

    /**
     * {@inheritDoc}
     */
    public List<SupportedCommand> getSupportedCommands(long instancePointer) {
        return LoaderCommunicationInterface.getInstance().getContext(instancePointer).getSupportedCommands();
    }

    /**
     * {@inheritDoc}
     */
    public String getLoaderErrorDesc(int error) {
        return LoaderCommunicationInterface.getInstance().getLoaderErrorDesc(error);
    }
}
