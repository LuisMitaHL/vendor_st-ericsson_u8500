package com.stericsson.sdk.cli;

import java.io.BufferedWriter;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.io.UnsupportedEncodingException;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Locale;
import java.util.SortedSet;
import java.util.TreeSet;

import org.apache.commons.cli.HelpFormatter;
import org.apache.commons.cli.Option;
import org.apache.commons.cli.OptionBuilder;
import org.apache.commons.cli.Options;

import com.stericsson.sdk.brp.AbstractCommand;
import com.stericsson.sdk.brp.CommandDescription;
import com.stericsson.sdk.brp.CommandFactory;
import com.stericsson.sdk.brp.CommandName;

/**
 * creates the CLI options
 * 
 * @author xdancho
 * 
 */
public class CLIOptions {

    HashMap<String, Options> commandMap = new HashMap<String, Options>();

    HashMap<String, Options> cliMap = new HashMap<String, Options>();

    SortedSet<String> commandMapSorted;

    static final String TIMEOUT = "t";

    static final String TIMEOUT_DESC =
        "Flash Tool CLI repeatedly reconnect to Flash Tool Backend until timeout occures.";

    static final String HELP = "help";

    static final String FTC = "console";

    static final String HELP_DESC = "help for the command";

    static final String FTC_DESC = "command propmt";

    static final String PORT = "port";

    static final String PORT_DESC = "which port to use";

    static final String HOST = "host";

    static final String HOST_DESC = "which host to use";

    static final String VERBOSE = "v";

    static final String VERBOSE_DESC = "set to verbose mode";

    static final String BARE = "b";

    static final String BARE_DESC = "prints result without formatting";

    /**
     * creates the options
     */
    public CLIOptions() {

        Options options = null;

        // CREATE ALL BRP COMMANDS
        for (CommandName cmd : CommandName.values()) {
            options = new Options();

            // always available
            OptionBuilder.isRequired(false);
            OptionBuilder.hasArg(false);
            OptionBuilder.withDescription(VERBOSE_DESC);
            options.addOption(OptionBuilder.create(VERBOSE));

            // help, always available
            OptionBuilder.isRequired(false);
            OptionBuilder.hasArg(false);
            OptionBuilder.withDescription(HELP_DESC);
            options.addOption(OptionBuilder.create(HELP));

            // port
            OptionBuilder.isRequired(false);
            OptionBuilder.hasArg(true);
            OptionBuilder.withDescription(PORT_DESC);
            options.addOption(OptionBuilder.create(PORT));

            // host
            OptionBuilder.isRequired(false);
            OptionBuilder.hasArg(true);
            OptionBuilder.withDescription(HOST_DESC);
            options.addOption(OptionBuilder.create(HOST));

            // timeout, always available
            OptionBuilder.isRequired(false);
            OptionBuilder.hasArg(true);
            OptionBuilder.withDescription(TIMEOUT_DESC);
            options.addOption(OptionBuilder.create(TIMEOUT));

            // bare
            OptionBuilder.isRequired(false);
            OptionBuilder.hasArg(false);
            OptionBuilder.withDescription(BARE_DESC);
            options.addOption(OptionBuilder.create(BARE));

            AbstractCommand aCmd = CommandFactory.createCommand(cmd);
            String[] arg = aCmd.getParameterNames();
            if (arg != null) {
                for (String argName : arg) {
                    if (AbstractCommand.PARAMETER_EQUIPMENT_ID.equalsIgnoreCase(argName)
                        || AbstractCommand.PARAMETER_SKIP_REDUNDANT_AREA.equalsIgnoreCase(argName)
                         || AbstractCommand.PARAMETER_INCLUDE_BAD_BLOCKS.equalsIgnoreCase(argName)
                        || AbstractCommand.PARAMETER_PERMANENT.equalsIgnoreCase(argName)) {
                        OptionBuilder.isRequired(false);
                    } else if (aCmd.getCommandName() == CommandName.BACKEND_SHUTDOWN_BACKEND) {
                        continue;
                    } else {
                        OptionBuilder.isRequired();
                    }

                    OptionBuilder.hasArg(true);
                    OptionBuilder.withArgName(argName);
                    OptionBuilder.withDescription(CommandDescription.getArgumentDescription(argName));
                    options.addOption(OptionBuilder.create(argName));
                }
            }
            commandMap.put(CommandName.getCLISyntax(cmd.name()).toLowerCase(Locale.getDefault()), options);
        }

        // help, always available
        options = new Options();
        OptionBuilder.hasArg(false);
        OptionBuilder.withDescription(HELP_DESC);
        options.addOption(OptionBuilder.create(HELP));

        cliMap.put(HELP, options);

        commandMapSorted = new TreeSet<String>(commandMap.keySet());

    }

    /**
     * get the options for a specific command
     * 
     * @param cmd
     *            the command to get options for
     * @return the Options
     */
    public Options getCommandOptions(String cmd) {
        Options opts = new Options();

        for (Iterator<?> it = commandMap.get(cmd).getOptions().iterator(); it.hasNext();) {
            opts.addOption((Option) it.next());
        }

        for (Iterator<?> it = cliMap.keySet().iterator(); it.hasNext();) {
            for (Iterator<?> it2 = cliMap.get(it.next()).getOptions().iterator(); it2.hasNext();) {
                opts.addOption((Option) it2.next());
            }
        }

        return opts;
    }

    /**
     * print help for a command
     * 
     * @param command
     *            the command to print help for
     */
    public void printHelpCommand(String command) {
        HelpFormatter formatter = new HelpFormatter();
        if (commandMap.get(command) != null) {
            formatter.printHelp(CommandDescription.getCommandDescription(command, true), command, commandMap
                .get(command), "", false);
        } else if (cliMap.get(command) != null) {
            formatter.printHelp(command, cliMap.get(command));
        }
    }

    /**
     * this will print usage for all commands
     * 
     * @throws UnsupportedEncodingException
     *             on errors
     */
    public void printHelpAll() throws UnsupportedEncodingException {
        HelpFormatter formatter = new HelpFormatter();
        System.out.println("\nAvailable commands:");
        formatter.setSyntaxPrefix("flashtool   ");

        // print BRP commands
        for (Iterator<?> it = commandMapSorted.iterator(); it.hasNext();) {
            String current = (String) it.next();

            formatter.printUsage(
                new PrintWriter(new BufferedWriter(new OutputStreamWriter(System.out, "UTF-8")), true),
                HelpFormatter.DEFAULT_WIDTH, current, commandMap.get(current));
            System.out.print("\n");

        }

        formatter.setSyntaxPrefix("flashtool   -");
        // print cli commands
        for (Iterator<?> it = cliMap.keySet().iterator(); it.hasNext();) {
            String current = (String) it.next();
            formatter.printUsage(
                new PrintWriter(new BufferedWriter(new OutputStreamWriter(System.out, "UTF-8")), true),
                HelpFormatter.DEFAULT_WIDTH, current, cliMap.get(current));
            System.out.print("\n");

        }
    }

    /**
     * tests if a command is available in the CLI
     * 
     * @param cmd
     *            the command to look up
     * @return if the command is available
     */
    public boolean isCommandAvailable(String cmd) {
        return commandMap.get(cmd) != null || cliMap.get(cmd) != null;
    }
}
