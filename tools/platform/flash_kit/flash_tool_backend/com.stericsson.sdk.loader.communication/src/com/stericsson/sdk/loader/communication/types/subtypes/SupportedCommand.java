package com.stericsson.sdk.loader.communication.types.subtypes;

/**
 * @author xdancho
 * 
 */
public class SupportedCommand {

    int cmdGroup = 0;

    int cmdCommand = 0;

    int cmdPermitted = 0;

    /**
     * @param group
     *            cmd group
     * @param command
     *            cmd
     * @param permitted
     *            is permitted
     */
    public SupportedCommand(int group, int command, int permitted) {
        this.cmdGroup = group;
        this.cmdCommand = command;
        this.cmdPermitted = permitted;
    }

    /**
     * @return the group
     */
    public int getGroup() {
        return cmdGroup;
    }

    /**
     * @return the command
     */
    public int getCommand() {
        return cmdCommand;
    }

    /**
     * @return the permitted
     */
    public int getPermitted() {
        return cmdPermitted;
    }

}
