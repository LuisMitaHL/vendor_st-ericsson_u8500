package com.stericsson.ftc;

import java.util.Locale;

import com.stericsson.ftc.command.CDCommand;
import com.stericsson.ftc.command.CHMODCommand;
import com.stericsson.ftc.command.CPCommand;
import com.stericsson.ftc.command.DevCommand;
import com.stericsson.ftc.command.FSPCommand;
import com.stericsson.ftc.command.FSVPropertiesCommand;
import com.stericsson.ftc.command.FormatCommand;
import com.stericsson.ftc.command.LSCommand;
import com.stericsson.ftc.command.MKDIRCommand;
import com.stericsson.ftc.command.MVCommand;
import com.stericsson.ftc.command.RMCommand;

/**
 * Factory creating command using abstract command.
 * 
 * @author tsikor01
 * 
 */
public enum FTCCommandFactory {

    /** Change directory command */
    CD(CDCommand.class),
    /** List directory */
    LS(LSCommand.class),
    /** Listing available devices and drives */
    DEV(DevCommand.class),
    /** Move file */
    MV(MVCommand.class),
    /** Remove file */
    RM(RMCommand.class),
    /** Copy file */
    CP(CPCommand.class),
    /** Create directory */
    MKDIR(MKDIRCommand.class),
    /** Get FileSystem properties */
    PROPERTIES(FSPCommand.class),
    /** Get FileSystem volume properties */
    VOLUME(FSVPropertiesCommand.class),
    /** Format volume */
    FORMAT(FormatCommand.class),
    /** Change access */
    CHMOD(CHMODCommand.class);

    private Class<? extends FTCAbstractCommand> clazz;

    FTCCommandFactory(Class<? extends FTCAbstractCommand> pClazz) {
        clazz = pClazz;
    }

    /**
     * Factory method for creating command
     * 
     * @param request
     *            Requested command
     * @return FTCAbstractCommand instance
     */
    public static FTCAbstractCommand createCommand(String request) {
        FTCAbstractCommand command = null;
        try {
            Class<? extends FTCAbstractCommand> pClazz = valueOf(request.toUpperCase(Locale.getDefault())).clazz;
            command = pClazz.newInstance();
        } catch (Exception e) {
            System.out.println("Error while handling command " + request + "!");
        }
        return command;
    }
}
