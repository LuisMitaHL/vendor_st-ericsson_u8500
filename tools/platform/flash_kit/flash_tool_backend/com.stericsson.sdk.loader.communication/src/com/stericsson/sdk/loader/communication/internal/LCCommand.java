package com.stericsson.sdk.loader.communication.internal;

import java.lang.reflect.Method;

import com.stericsson.sdk.loader.communication.ILoaderCommunicationListener;

/**
 * @author xdancho
 * 
 */
public class LCCommand {

    private Method method;

    private Object[] arguments;

    private Object result;

    private String command;

    private ILoaderCommunicationListener listener = null;

    /**
     * @param methodCMD
     *            the method for the command
     * @param args
     *            the arguments for the method
     * @param tag
     *            identifier for this command
     * @param startTimeOfCMD
     *            the start time for the command
     */
    LCCommand(Method methodCMD, Object[] args, String cmd, long instance, ILoaderCommunicationListener l) {
        method = methodCMD;
        addArguments(args, instance);
        command = cmd;
        listener = l;
    }

    /**
     * add arguments to use for this command
     * 
     * @param args
     *            the object array
     * @param instance
     *            the instance to use
     */
    public void addArguments(Object[] args, long instance) {

        if (args.length != 0) {
            arguments = new Object[args.length + 1];

            arguments[0] = instance;
            for (int i = 1; i < arguments.length; i++) {
                arguments[i] = args[i - 1];
            }

        } else {
            arguments = new Object[] {
                instance};
        }

    }

    /**
     * @return the command method
     */
    public Method getMethod() {
        return method;
    }

    /**
     * @return get the arguments for the command
     */
    public Object[] getArguments() {
        return arguments;
    }

    /**
     * @return result
     */
    public Object getResult() {
        return result;
    }

    /**
     * @param res
     *            result
     */
    public void setResult(Object res) {
        result = res;
    }

    /**
     * get the command as string
     * 
     * @return the command
     */
    public String getCommandString() {
        return command;
    }

    /**
     * @return TBD
     */
    public ILoaderCommunicationListener getListener() {
        return listener;
    }

}
