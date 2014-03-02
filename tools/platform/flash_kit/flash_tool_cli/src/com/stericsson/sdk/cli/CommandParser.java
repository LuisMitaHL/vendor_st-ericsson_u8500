package com.stericsson.sdk.cli;

import java.util.Locale;

import org.apache.commons.cli.CommandLine;
import org.apache.commons.cli.CommandLineParser;
import org.apache.commons.cli.MissingOptionException;
import org.apache.commons.cli.ParseException;
import org.apache.commons.cli.PosixParser;

import com.stericsson.sdk.cli.command.BRPCommandExecutor;
import com.stericsson.sdk.brp.AbstractCommand;
import com.stericsson.sdk.brp.CommandFactory;
import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.brp.InvalidSyntaxException;

/**
 * handles the parsing of args to a BRPCommand
 * 
 * @author xdancho
 * 
 */
public class CommandParser {

    CommandLineParser parser = new PosixParser();

    CLIOptions options = new CLIOptions();

    /**
     * command parser
     * 
     * @param args
     *            the args provided
     * @param silent
     *            Command will not print resoult
     * @return a BRPCommand
     */
    public BRPCommandExecutor parse(String[] args, boolean silent) {

        try {
            boolean verbose = false; // default
            boolean bare = false; // default

            CommandLine cmd = parser.parse(options.getCommandOptions(args[0]), args);

            // CLI OPTIONS
            if (cmd.hasOption(CLIOptions.VERBOSE)) {
                verbose = true;
            }

            if (cmd.hasOption(CLIOptions.BARE)) {
                bare = true;
            }

            // BRP COMMANDS
            // locate enum
            CommandName cmdName = null;
            for (CommandName enumName : CommandName.values()) {
                if (CommandName.getCLISyntax(enumName.name()).equals(args[0].toLowerCase(Locale.getDefault()))) {
                    cmdName = enumName;
                    break;
                }
            }
            if (cmdName == null) {
                throw new ParseException("Failed to parse command " + args[0] + ".");
            }

            AbstractCommand aCmd = CommandFactory.createCommand(cmdName);
            String[] argNames = aCmd.getParameterNames();
            String[] aCmdArgs = null;

            if (argNames != null) {
                aCmdArgs = new String[argNames.length];
            }

            aCmd = createCommand(cmd, cmdName, argNames, aCmdArgs);

            if (aCmd == null) {
                throw new ParseException("Failed to parse command " + args[0] + ".");
            }
            BRPCommandExecutor brpCmd = new BRPCommandExecutor(aCmd.getCommandString(), silent);
            brpCmd.setVerbose(verbose);
            brpCmd.setBare(bare);
            return brpCmd;

        } catch (Exception e) {
            if (e instanceof MissingOptionException || e instanceof ParseException
                || e instanceof InvalidSyntaxException) {
                options.printHelpCommand(args[0]);
            } else {
                System.out.println(e.getMessage());
                System.out.print("\nException technical details:\n");
                e.printStackTrace(System.out);
            }
            return null;
        }

    }

    private AbstractCommand createCommand(CommandLine cmd, CommandName cmdName, String[] argNames, String[] aCmdArgs)
        throws InvalidSyntaxException {
        AbstractCommand aCmd;
        if (aCmdArgs != null) {
            // required args
            for (int i = 0; i < argNames.length; i++) {
                if (AbstractCommand.PARAMETER_EQUIPMENT_ID.equalsIgnoreCase(argNames[i])
                    && cmd.getOptionValue(argNames[i]) == null) {
                    aCmdArgs[i] = AbstractCommand.EQUIPMENT_ID_CURRENT;
                } else if (cmdName == CommandName.BACKEND_SHUTDOWN_BACKEND
                    && AbstractCommand.PARAMETER_MESSAGE.equalsIgnoreCase(argNames[i])) {
                    aCmdArgs[i] = createShutdownString();
                } else {
                    aCmdArgs[i] = cmd.getOptionValue(argNames[i]);
                }
            }

            aCmd = CommandFactory.createCommand(cmdName, aCmdArgs);
        } else {
            aCmd = CommandFactory.createCommand(cmdName.name());
        }
        return aCmd;
    }

    private String createShutdownString() {
        return "Backend shutdown requested by " + System.getProperty("user.name");
    }
}
