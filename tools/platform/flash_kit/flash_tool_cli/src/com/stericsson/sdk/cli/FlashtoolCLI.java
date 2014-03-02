package com.stericsson.sdk.cli;

import java.io.File;
import java.io.PrintWriter;
import java.net.ConnectException;
import java.util.Locale;

import org.apache.commons.cli.CommandLine;
import org.apache.commons.cli.Parser;
import org.apache.commons.cli.PosixParser;

import com.stericsson.ftc.FTCReader;
import com.stericsson.sdk.cli.command.BRPCommandExecutor;

/**
 * flashtool main
 * 
 * @author xdancho
 * 
 */
public class FlashtoolCLI {

    static final int DEFAULT_TIMEOUT = 0;

    static final int DEFAULT_PORT = 8088;

    /** */
    public static final int EXIT_ERROR_CONNECT = 2;

    /** */
    public static final int EXIT_ERROR = 1;

    /** */
    public static final int EXIT_NORMAL = 0;

    private static final String ERROR_LOG_FILE = "flash-tool-error.log";

    static final String DEFAULT_HOST = "localhost";

    static FlashtoolCLI instance = null;

    private String host = "";

    private int port = 8088;

    private int timeout = 0;

    static int exitCode = EXIT_NORMAL;

    /** */
    public static final int EXIT_ERROR_MISSING = 2;
    /**
     * @param code
     *            Exit code
     */
    public static void setExitCode(int code) {
        exitCode = code;
    }

    /**
     * main
     * 
     * @param args
     *            args provided
     */
    public static void main(String[] args) {

        try {
            File file = new File(getErrorLogFilePath());
            file.delete();
        } catch (Exception e) {
            System.err.println("Can't delete error log file: " + e.getMessage() + "\n");
        }

        instance = new FlashtoolCLI();
        instance.start(args);

        System.exit(exitCode);
    }

    /**
     * start flashtool CLI
     * 
     * @param args
     *            the args provided
     */
    public void start(String[] args) {

        try {
            if (args.length == 0) {
                printOnNewLine("Missing command. Use option -help to get list of available commands.");
                return;
            }

            // -gdfs --> -storage_id gdfs
            // -ta --> -storage_id ta
            if (isGDFS(args[0])) {
                args = transformParameters(args);
            }

            boolean isAvailable = false;
            boolean containsHelp = false;

            if (!args[0].equals("console".toLowerCase(Locale.getDefault()))) {
                // parse cli options
                CLIOptions options = new CLIOptions();

                // test command available

                if (options.isCommandAvailable(args[0])) {
                    isAvailable = true;

                }

                // test for '-help'
                containsHelp = testHelp(args, containsHelp);

                // if the command is not recognized and -help is available.
                if (!isAvailable) {
                    if (containsHelp) {
                        options.printHelpAll();
                        return;
                    }

                    System.out.println("Command: " + args[0]
                        + " is not recognized, use option -help to get available commands");
                    return;
                } else {
                    if (containsHelp) {
                        options.printHelpCommand(args[0]);
                        return;
                    }
                }

                Parser cliParser = new PosixParser();
                CommandLine line = cliParser.parse(options.getCommandOptions(args[0]), args);
                args = setConnection(line, args);
                executeCommand(args);

            } else {
                FTCReader prompt = new FTCReader(args);
                prompt.start();
            }
        } catch (ConnectException e) {
            printOnNewLine("Connection refused. Check that flash tool backend is running on " + host + ":" + port
                + ".\n");
            handleException(e);
            setExitCode(EXIT_ERROR_CONNECT);
        } catch (Exception e) {
            printOnNewLine("Command failed.\n");
            handleException(e);
            setExitCode(EXIT_ERROR);
        } finally {
            ClientSocket.getInstance().stopSocket();
        }

    }

    /**
     * Returns path of the error log file.
     * 
     * @return Path of the error log file.
     * @throws NullPointerException
     *             Null pointer exception.
     * @throws SecurityException
     *             Security exception.
     */
    public static String getErrorLogFilePath() throws NullPointerException, SecurityException {
        // path to user directory
        String path = System.getProperty("user.dir");
        // adds file separator + log-file name
        path += System.getProperty("file.separator") + ERROR_LOG_FILE;
        return path;
    }

    private void handleException(Exception e) {
        PrintWriter pw = null;
        try {
            System.err.println("Error message: " + e.getMessage() + "\n");

            File file = new File(getErrorLogFilePath());

            pw = new PrintWriter(file, "UTF-8");
            pw.println("Exception stack trace:\n");
            e.printStackTrace(pw);
            pw.flush();

            System.err.println("Error log file: " + file.getAbsolutePath() + "\n");
        } catch (Exception ex) {
            System.err.println("Can't create error log file: " + ex.getMessage() + "\n\nException stack trace:\n");
            e.printStackTrace();
        } finally {
            if (pw != null) {
                pw.close();
            }
        }
    }

    private String[] setConnection(CommandLine line, String[] args) {

        if (line.hasOption(CLIOptions.HOST)) {
            host = line.getOptionValue(CLIOptions.HOST);
            args = removeFromArgs(args, CLIOptions.HOST);
        } else {
            host = DEFAULT_HOST;
        }

        if (line.hasOption(CLIOptions.PORT)) {
            String strPort = line.getOptionValue(CLIOptions.PORT);
            args = removeFromArgs(args, CLIOptions.PORT);
            port = Integer.parseInt(strPort);

        } else {
            port = DEFAULT_PORT;
        }

        if (line.hasOption(CLIOptions.TIMEOUT)) {
            String strTimeout = line.getOptionValue(CLIOptions.TIMEOUT);
            args = removeFromArgs(args, CLIOptions.TIMEOUT);
            timeout = Integer.parseInt(strTimeout);

        } else {
            timeout = DEFAULT_TIMEOUT;
        }
        return args;
    }

    private void executeCommand(String[] args) throws Exception {
        boolean connected = false;
        long startTime = System.currentTimeMillis();
        do {
            try {
                printConnectingMessage(startTime, timeout);
                ClientSocket.getInstance().startSocket(host, port);
                connected = true;
                CommandParser cmdParser = new CommandParser();
                BRPCommandExecutor cmd = cmdParser.parse(args, false);
                if (cmd != null) {
                    if (cmd.execute()) {
                        if(cmd.isMissingLibrary()){
                            setExitCode(EXIT_ERROR_MISSING);
                        }else{
                            setExitCode(EXIT_ERROR);
                        }
                    }
                } else {
                    setExitCode(EXIT_ERROR);
                }

            } catch (Exception e) {
                if (connected) {
                    throw e;
                }
                if (timeoutOccured(startTime, timeout)) {
                    throw e;
                }
            }
        } while (!connected && !timeoutOccured(startTime, timeout));
    }

    private void printConnectingMessage(long pStartTime, int pTimeout) {
        if (pTimeout > 0) {
            long currentTime = System.currentTimeMillis();
            long currentTimeout = ((currentTime - pStartTime) / 1000);
            if (currentTimeout == 1) {
                printOnSameLine("Connecting ... timeout " + (currentTimeout) + " second.");
            } else {
                printOnSameLine("Connecting ... timeout " + (currentTimeout) + " seconds.");
            }
        }
    }

    private void printOnSameLine(String message) {
        System.out.print("\r" + message + "\t\t");
    }

    private void printOnNewLine(String message) {
        System.out.println("\n" + message);
    }

    private boolean timeoutOccured(long pStartTime, int pTimeout) {
        long currentTime = System.currentTimeMillis();

        if (((currentTime - pStartTime) / 1000) < pTimeout) {
            return false;
        }
        return true;
    }

    private boolean testHelp(String[] args, boolean containsHelp) {
        for (String s : args) {
            if (s.contains(CLIOptions.HELP)) {
                containsHelp = true;
            }
        }
        return containsHelp;
    }

    private String[] removeFromArgs(String[] args, String arg) {

        String[] resArgs = new String[args.length - 2];
        int outputIndex = 0;
        int inputIndex = 0;
        for (int i = 0; i < resArgs.length; i++) {
            if (args[i].equalsIgnoreCase("-" + arg)) {
                inputIndex += 2;
            }
            resArgs[outputIndex] = args[inputIndex];
            outputIndex++;
            inputIndex++;
        }

        return resArgs;

    }

    private String[] transformParameters(String[] args) {
        String[] temp = new String[args.length + 1];
        boolean found = false;
        int j = 0;
        for (int i = 0; i < args.length; i++) {
            if (args[i].equalsIgnoreCase("-gdfs") && !found) {
                temp[j] = "-storage_id";
                j++;
                temp[j] = "gdfs";
                found = true;
            } else if (args[i].equalsIgnoreCase("-ta") && !found) {
                temp[j] = "-storage_id";
                j++;
                temp[j] = "ta";
                found = true;
            } else {
                temp[j] = args[i];
            }
            j++;

        }
        if (!found) {
            return args;
        }
        return temp;
    }

    private boolean isGDFS(String arg) {
        String[] gdfsCommand =
            {
                "write_global_data_set", "read_global_data_set", "erase_global_data_set", "write_global_data_unit",
                "read_global_data_unit"};

        for (String cmd : gdfsCommand) {
            if (cmd.equalsIgnoreCase(arg)) {
                return true;
            }
        }
        return false;
    }
}
