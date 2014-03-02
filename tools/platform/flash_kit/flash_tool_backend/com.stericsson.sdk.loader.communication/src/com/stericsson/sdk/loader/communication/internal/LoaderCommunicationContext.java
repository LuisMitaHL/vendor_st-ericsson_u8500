package com.stericsson.sdk.loader.communication.internal;

import java.util.ArrayList;
import java.util.List;

import com.stericsson.sdk.equipment.io.port.ILCPort;
import com.stericsson.sdk.equipment.io.port.IPort;
import com.stericsson.sdk.loader.communication.types.subtypes.SupportedCommand;

/**
 * 
 * @author xdancho
 * 
 */
public class LoaderCommunicationContext {

    private static final long serialVersionUID = 1337955332493554534L;

    LoaderCommunicationCommandHandler cmdHandler;

    static final int TRUE = 1;

    ILCPort port;

    /** */
    List<SupportedCommand> supportedCommands;

    private long instancePointer;

    /**
     * @param p TBD
     */
    public LoaderCommunicationContext(IPort p) {
        port = (ILCPort)p;
    }

    /**
     * @return TBD
     */
    public ILCPort getPort() {
        return port;
    }

    /**
     * @param id Instance pointer
     */
    public void setInstancePointer(long id) {
        instancePointer = id;
    }

    /**
     * @return Instance pointer
     */
    public long getInstancePointer() {
        return instancePointer;
    }

    /**
     * get supported commands
     * 
     * @return list of supported commands
     */
    public List<SupportedCommand> getSupportedCommands() {
        return supportedCommands;
    }

    /**
     * set the supported commands
     * 
     * @param cmdList
     *            a list of supported commands
     */
    public void setSupportedCommands(List<SupportedCommand> cmdList) {
        supportedCommands = new ArrayList<SupportedCommand>(cmdList.size());

        for (int i = 0; i < cmdList.size(); i++) {
            supportedCommands.add(cmdList.get(i));
        }
    }

    /**
     * check if a command is permitted
     * 
     * @param group
     *            group nbr
     * @param command
     *            command nbr
     * @return if the command is permitted
     */
    public boolean isCommandPermitted(int group, int command) {
        // TODO Auto-generated method stub

        if (supportedCommands == null) {
            return false;
        }

        for (SupportedCommand s : supportedCommands) {
            if (s.getGroup() == group) {
                if (s.getCommand() == command) {
                    if (s.getPermitted() == TRUE) {
                        return true;
                    }
                }
            }
        }
        return false;
    }

    /**
     * check if the command group is supported
     * 
     * @param group
     *            the group
     * @return true if the command group is supported
     */
    public boolean isCommandGroupSupported(int group) {

        if (supportedCommands == null) {
            return false;
        }

        for (SupportedCommand s : supportedCommands) {
            if (s.getGroup() == group) {
                if(s.getPermitted() == TRUE) {
                    return true;
                }
            }
        }
        return false;
    }

}
