package com.stericsson.ftc;

import java.io.File;

import com.stericsson.sdk.brp.AbstractCommand;
import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.cli.ClientSocket;
import com.stericsson.sdk.cli.CommandParser;
import com.stericsson.sdk.cli.command.BRPCommandExecutor;

/**
 * 
 * @author vsykal01
 * 
 *         This class representing the path on me/pc site.
 * */

public class FTCPath {

    /**
     * possible values USB0, COM1, portA, ...
     */
    private final String prefix;

    /**
     * path to file <i>c:/dirA/dirB</i><br />
     * It always contains linux folder separator '/'.<br />
     * It doesn't contain folder separator at the end if it isn't root folder of some volume:<br />
     * <i>c:/test/</i> isn't valid but <i>c:/</i> is valid<br />
     * 
     * Path to mobile equipment volume name is longer than windows volume name:
     * <i><b>/flash0/cabs0</b>:/afs</i>
     */
    private final String suffix;

    /**
     * Prefix determining that user is using PC
     */
    public static final String PC_PREFIX = "PC";

    /**
     * Separates prefix from the rest of the path
     */
    public static final String PREFIX_SEPARATOR = "-";

    /**
     * @param pPrefix
     *            Equipment identifier (COM1, USB0, ...), or loacal computer identifer (PC).
     * @param pPath
     *            Absolute path on device or on pc (example /tmp/bin).
     */
    public FTCPath(String pPrefix, String pPath) {
        prefix = pPrefix;
        suffix = pPath;
    }

    /**
     * @return True if it is ME path (not PC path).
     */
    public boolean isOnME() {
        return !prefix.equals(PC_PREFIX);
    }

    /**
     * @return the prefix
     */
    public String getPrefix() {
        return prefix;
    }

    /**
     * @return the suffix
     */
    public String getSuffix() {
        return suffix;
    }

    /**
     * @return path in format prefix-suffix same format as user input.
     */
    @Override
    public String toString() {
        return prefix + PREFIX_SEPARATOR + suffix;
    }

    /**
     * @return for me with prefix otherwise without prefix. Format which is used in BRP protocol.
     */
    public String getCommandString() {
        if (isOnME()) {
            return suffix;
        } else {
            return "PC:" + suffix;
        }
    }

    /**
     * @param context
     *            context for finding port and host if needed.
     * @return true if path is path to directory.
     * @throws Exception is thrown in case of some communication error with backend.
     */
    public boolean isDirectory(FTCApplicationContext context) throws Exception {
        return (isOnME() && mEIsDirectory(context)) || (!isOnME() && new File(suffix).isDirectory());
    }

    /**
     * This method is checking if path given in command can be reached using <code>BRPCommandExecutor's
     * FILE_SYSTEM_PROPERTIES</code> command, which will return true if it is directory.
     * 
     * @param context
     *            Contains connection information (host, port) to Backend.
     * @return true if path given is correct path to ME. 
     * @throws Exception
     *             Connection exception if Backend is not running.
     */
    private boolean mEIsDirectory(FTCApplicationContext context) throws Exception {
        CommandParser cmdParser = new CommandParser();

        BRPCommandExecutor cmd =
            cmdParser.parse(new String[] {
                CommandName.getCLISyntax(CommandName.FILE_SYSTEM_LIST_DIRECTORY.name()),
                "-" + AbstractCommand.PARAMETER_EQUIPMENT_ID, prefix, "-" + AbstractCommand.PARAMETER_TARGET_PATH,
                getCommandString()}, true);

        if (cmd != null) {
            ClientSocket.getInstance().startSocket(context.getHost(), context.getPort());
            return !cmd.execute();
        }

        return false;
    }

    /**
     * @param context
     *            context for finding port and host if needed.
     * @return true if file exist.
     * @throws Exception
     *             is thrown in case of some communication error with backend.
     */
    public boolean exist(FTCApplicationContext context) throws Exception {
        return (isOnME() && mEPathAvailable(context)) || (!isOnME() && pCPathAvailable());
    }

    /**
     * This method is checking if path given in command can be reached using <code>BRPCommandExecutor's
     * FILE_SYSTEM_PROPERTIES</code> command, which will return true if file/directory exists.
     * 
     * @param context
     *            Contains connection information (host, port) to Backend.
     * @return true if path given is correct path to ME.
     * @throws Exception
     *             Connection exception if Backend is not running.
     */
    private boolean mEPathAvailable(FTCApplicationContext context) throws Exception {
        CommandParser cmdParser = new CommandParser();

        BRPCommandExecutor cmd =
            cmdParser.parse(new String[] {
                CommandName.getCLISyntax(CommandName.FILE_SYSTEM_PROPERTIES.name()),
                "-" + AbstractCommand.PARAMETER_EQUIPMENT_ID, prefix, "-" + AbstractCommand.PARAMETER_TARGET_PATH,
                getCommandString()}, true);

        if (cmd != null) {
            ClientSocket.getInstance().startSocket(context.getHost(), context.getPort());
            return !cmd.execute();
        }

        return false;
    }

    /**
     * Method checking accessibility of directory located on hard drive.
     * 
     * @param directory
     *            Path of directory to check.
     * @return true if path is existing directory.
     */
    private boolean pCPathAvailable() {
        return (new File(suffix)).exists();
    }
}
