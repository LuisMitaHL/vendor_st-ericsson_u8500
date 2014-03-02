package com.stericsson.ftc;

import java.util.Collection;

import com.stericsson.sdk.cli.ClientSocket;
import com.stericsson.sdk.cli.CommandParser;
import com.stericsson.sdk.cli.command.BRPCommandExecutor;

/**
 * The <code>FTCAbstractCommand</code> is a abstract class representing console commands
 * (LS,CD,MKDIR, etc) used in <code>Flash Tool Console</code> (FTC). This class is used for setting
 * parameters from <code>FTCApplicationContext</code> which represents current state of application.
 * Class also provide parsing path for certain commands.
 * <p>
 * <blockquote>
 * 
 * <pre>
 * Example of used path formats:
 * ME path= Equipment-ID-\PATH\TO\IN\MOBILE\DEVICE
 *          Equipment-ID-/PATH/ON/MOBILE/DEVICE
 * Windows hard drive path = DISK:\PATH\TO\SOME\DIS
 * Linux hard drive path= /PATH/ON/LINUX/DRIVE
 * </pre>
 * 
 * </blockquote>
 * <p>
 * 
 * @author TSIKOR01
 * 
 */
public abstract class FTCAbstractCommand {

    /** 
     * application context
     */
    protected FTCApplicationContext context;

    private String[] command;

    /**
     * Method for checking input parameters, their count and correct type (if path is or is not
     * correct ME path, etc.). After checking parameters are provided to <code>execute</code>
     * method, which will execute specific command.
     * 
     * @param input
     *            String representing input from console.
     * @return Returns true if check was successful false otherwise.
     * @throws Exception
     *             Exception if there is no connection to the backend available
     */
    public abstract boolean setParameters(Collection<String> input) throws Exception;

    /**
     * Method which prints commands help.
     * <p>
     * <blockquote>
     * 
     * <pre>
     * Help format:
     * 
     * Command <SOME-PARAMETER>
     * - Description
     * </pre>
     * 
     * </blockquote>
     * <p>
     */
    public abstract void printHelp();

    /**
     * Method setting application context to currently called command, method is also checking
     * parameters provided in input context and returns true if parameters are correct.
     * 
     * @param inputcontext
     *            Application context from currently running instance of console.
     */
    public void setContext(FTCApplicationContext inputcontext) {
        this.context = inputcontext;
    }

    /**
     * Setter for parsed command which is prepared in <code>setParameters</code> method.
     * 
     * @param newCommand
     *            Array representing command.
     */
    protected void setCommand(String[] newCommand) {
        this.command = newCommand;
    }

    /**
     * Getter for parsed command
     * 
     * @return Returns command string
     */
    protected String[] getCommand() {
        return command;
    }

    /**
     * Method executing desired command.
     * 
     * @throws Exception
     *             Exception if there are problems with connection to backend.
     * 
     */
    public void execute() throws Exception {
        CommandParser cmdParser = new CommandParser();
        ClientSocket.getInstance().startSocket(context.getHost(), context.getPort());
        BRPCommandExecutor cmd = null;

        cmd = cmdParser.parse(getCommand(), false);

        if (cmd != null) {
            cmd.execute();
        }

    }
}
