package com.stericsson.sdk.assembling.cli;

import java.io.File;
import java.io.IOException;
import java.io.PrintStream;
import java.io.PrintWriter;
import java.util.logging.Logger;

import org.apache.commons.cli.CommandLineParser;
import org.apache.commons.cli.Options;
import org.apache.commons.cli.ParseException;
import org.apache.commons.cli.PosixParser;

import com.stericsson.sdk.assembling.AssemblerException;
import com.stericsson.sdk.assembling.AssemblerFactory;
import com.stericsson.sdk.assembling.IAssembler;
import com.stericsson.sdk.assembling.IAssemblerFactory;
import com.stericsson.sdk.assembling.IAssemblerListener;
import com.stericsson.sdk.assembling.IAssemblerSettings;
import com.stericsson.sdk.assembling.internal.BasicFileValidator;
import com.stericsson.sdk.assembling.internal.u8500.U8500CommandLineOptions;
import com.stericsson.sdk.assembling.utilities.StreamCloser;

/**
 * This class is used to execute the command line interface tool for assembling software.
 * 
 * @author xolabju
 */
public class Assemble implements IAssemblerListener {

    /** to be used as first cli argument when assembling with u8500 as target platform */
    public static final String PLATFORM_U8500 = "u8500";

    private static final int SUCCESS = 0;

    private static final int ERROR = 1;

    private static final String ERROR_LOG_FILE = "assemble-tool-error.log";

    private Options commandLineOptions;

    private boolean verbose;

    private Logger logger;

    private static boolean taImg;

    private PrintStream printStream;

    /**
     * Entry point method
     * 
     * @param arguments
     *            Command line arguments
     */
    public static void main(String[] arguments) {
        CommandLineLogging.setupLogger();
        try {
            Logger logger = Logger.getLogger(Assemble.class.getName());
            PrintStream ps = new PrintStream(System.out, false, "UTF-8");

            try {
                File file = new File(getErrorLogFilePath());
                file.delete();
            } catch (Exception e) {
                System.err.println("Can't delete error log file: " + e.getMessage() + "\n");
            }

            System.exit(new Assemble(logger, ps).execute(new AssemblerFactory(), arguments, logger));
        } catch (Exception e) {
            PrintWriter pw = null;
            try {
                System.err.println("Assembling failed.\n\nError message: " + e.getMessage() + "\n");

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
                StreamCloser.close(pw);
            }
            System.exit(ERROR);
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

    /**
     * Constructor
     * 
     * @param l
     *            this logger will be used when we want to log information.
     * 
     * @param ps
     *            This print stream will be used to print information when you run verbose mode.
     */
    public Assemble(Logger l, PrintStream ps) {
        logger = l;
        printStream = ps;
    }

    /**
     * @param assemblerFactory
     *            a factory instance to use
     * 
     * @param arguments
     *            Command line arguments
     * 
     * @param l
     *            this logger will be used when we want to log information.
     * 
     * @return Error code (0 if no error)
     * @throws Exception
     *             Exception
     */
    public int execute(IAssemblerFactory assemblerFactory, String[] arguments, Logger l) throws Exception {

        int result = SUCCESS;

        ICommandLineProxy commandLine = new CommandLineProxy();
        CommandLineParser argumentParser = new PosixParser();
        IAssemblerSettings assemblerSettings;
        IAssembler assembler;

        try {

            // Create and parse command line options
            commandLineOptions = CommandLineOptionsFactory.createCommandLineOptions(arguments[0]);
            commandLine.setProxiedObject(argumentParser.parse(commandLineOptions, arguments));

            // Parse and handle initial arguments
            if (parseBaseArguments(assemblerFactory, commandLine, l)) {
                return result;
            }
            // Look for output file
            String outFile = getOutputFile(commandLine);

            // Create settings
            assemblerSettings = assemblerFactory.createAssemblerSettings(outFile, commandLine);
            if (assemblerSettings == null) {
                throw new AssemblerException("Failed to create assembler settings.");
            }
            // Create the signer and execute the signing
            assembler = assemblerFactory.createAssembler(assemblerSettings);
            if (assembler == null) {
                throw new AssemblerException("Failed to create assembler.");
            }
            assembler.addListener(this);
            assembler.assemble(assemblerSettings);
        } catch (Exception e) {
            println("");
            if (logger != null) {
                logger.severe(e.getMessage());
            }
            println("");
            throw e;
        }
        return result;
    }

    /**
     * @param msg
     *            message to be written to the print stream contained in the class.
     */
    public void println(String msg) {
        if (printStream == null) {
            return;
        }

        printStream.println(msg);
    }

    /**
     * @param signerFactory
     *            to create/fetch service
     * @param commandLine
     *            string
     * 
     * @return true if application should shutdown
     * 
     * @param l
     *            this logger will be used when we want to logg information.
     * 
     * @throws ParseException
     *             parse exception
     * 
     * @throws IOException
     *             no file found.
     */
    public boolean parseBaseArguments(IAssemblerFactory signerFactory, ICommandLineProxy commandLine, Logger l)
        throws ParseException, IOException {
        // Some basic options

        if (commandLine.hasOption(U8500CommandLineOptions.Option.HELP.getShortCMD())) {
            AssembleHelper.printHelp(commandLineOptions, printStream);
            return true;
        }

        if (commandLine.hasOption(U8500CommandLineOptions.Option.VERBOSE.getShortCMD())) {
            verbose = true;
        }

        setTrim(commandLine);

        missingArguments(commandLine, l);

        return false;
    }

    /**
     * Validates the options from a set of command line options. Use this to log the missing
     * required options.
     * 
     * @param commandLine
     *            a set of commands that has been specified.
     * 
     * @param l
     *            this logger will be used when we want to logg information.
     * 
     * @throws ParseException
     *             if commandLine lack a required option.
     */
    protected void missingArguments(ICommandLineProxy commandLine, Logger l) throws ParseException {
        boolean missingReqOptions = false;

        if (PLATFORM_U8500.equalsIgnoreCase(commandLine.getArgs()[0])) {
            missingReqOptions = missingU8500Arguments(commandLine, l);
        }

        if (missingReqOptions) {
            throw new ParseException("Missing one or more required parameters.");
        }
    }

    private boolean missingU8500Arguments(ICommandLineProxy commandLine, Logger l) throws ParseException {
        boolean missingReqOptions = false;
        for (U8500CommandLineOptions.Option i : U8500CommandLineOptions.Option.values()) {
            if (!(i.isRequired() && !commandLine.hasOption(i.getLongCMD()))) {
                continue;
            }

            if (l != null) {
                l.severe("Missing required param --" + i.getLongCMD() + ". Description: " + i.getDescription());
            }

            missingReqOptions = true;
        }
        return missingReqOptions;
    }

    private String getOutputFile(ICommandLineProxy commandLine) throws AssemblerException, ParseException {
        String[] args = commandLine.getArgs();
        if (args == null || args.length < 2) {
            throw new AssemblerException("You must specify an output file ");
        }
        String outFile = args[1];

        boolean overWrite = commandLine.hasOption(U8500CommandLineOptions.Option.FORCE.getShortCMD());

        BasicFileValidator.validateOutputFile(outFile, overWrite);

        return outFile;
    }

    /**
     * {@inheritDoc}
     */
    public void assemblerMessage(IAssembler assembler, String message) {

        if (logger != null) {
            logger.info(message);
        }

        if (verbose) {
            // Print message to standard output
            println(message);
        }

    }

    /**
     * Returns true if -r parameter is set.
     *
     * @return true if -r parameter is set
     */
    public static boolean getTrim() {
        return taImg;
    }

    /**
     *
     * @param commandLine .
     * @throws ParseException .
     */
    private static void setTrim(ICommandLineProxy commandLine) throws ParseException{
        if (commandLine.hasOption(U8500CommandLineOptions.Option.TRIM_FILE.getShortCMD())) {
            taImg = true;
        }else{
            taImg = false;
        }
    }
}
