package com.stericsson.sdk.signing.cli;

import java.io.File;
import java.io.PrintStream;
import java.io.PrintWriter;

import org.apache.commons.cli.CommandLineParser;
import org.apache.commons.cli.Options;
import org.apache.commons.cli.ParseException;
import org.apache.commons.cli.PosixParser;
import org.apache.log4j.Logger;

import com.stericsson.sdk.signing.ISigner;
import com.stericsson.sdk.signing.ISignerFacade;
import com.stericsson.sdk.signing.ISignerListener;
import com.stericsson.sdk.signing.ISignerSettings;
import com.stericsson.sdk.signing.SignerException;
import com.stericsson.sdk.signing.SoftwareAlreadySignedException;
import com.stericsson.sdk.signing.cli.GenericCommandLineOptions.GenericOption;
import com.stericsson.sdk.signing.cli.a2.A2CommandLineOptions;
import com.stericsson.sdk.signing.generic.GenericSoftwareType;
import com.stericsson.sdk.signing.generic.IGenericSignerSettings;
import com.stericsson.sdk.signing.signerservice.ISignPackageInformation;
import com.stericsson.sdk.signing.signerservice.ISignerService;

/**
 * This class is used to execute the command line interface tool for signing various files.
 * 
 * @author xtomlju
 * @author Daniel Ekelund <daniel.xd.ekelund@stericsson.com>
 */
public class Sign implements ISignerListener, ISign {

    /** to be used as first cli argument when signing a2 */
    public static final String SIGN_A2 = "a2";

    /** to be used as first cli argument when signing l9540 */
    public static final String SIGN_L9540 = "l9540";

    /** to be used as first cli argument when signing u8500 */
    public static final String SIGN_U8500 = "u8500";

    /** to be used as first cli argument when signing u5500 */
    public static final String SIGN_U5500 = "u5500";

    /** to be used as first cli argument when signing m7x00 */
    public static final String SIGN_M7X00 = "m7x00";

    /** to be used as first cli argument when signing load modules */
    public static final String SIGN_LOAD_MODULES = "load-modules";

    private static final int SUCCESS = 0;

    private static final int ERROR = 1;

    private static final String ERROR_LOG_FILE = "sign-tool-error.log";

    private Options commandLineOptions;

    private boolean verbose;

    private Logger logger;

    private PrintStream printStream;

    /**
     * Entry point method
     * 
     * @param arguments
     *            Command line arguments
     */
    public static void main(String[] arguments) {
        try {
            Logger logger = Logger.getLogger(Sign.class.getName());
            PrintStream ps = new PrintStream(System.out, false, "UTF-8");

            try {
                File file = new File(getErrorLogFilePath());
                file.delete();
            } catch (Exception e) {
                System.err.println("Can't delete error log file: " + e.getMessage() + "\n");
            }

            System.exit(new Sign(logger, ps).execute(new SignerFactory(), arguments, logger));
        } catch (Exception e) {
            PrintWriter pw = null;
            try {
                System.err.println("Signing failed.\n\nError message: " + e.getMessage() + "\n");

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
     *            this logger will be used when we want to logg information.
     * 
     * @param ps
     *            This print stream will be used to print information when you run verbose mode.
     */
    public Sign(Logger l, PrintStream ps) {
        logger = l;
        printStream = ps;
    }

    /**
     * @param arguments
     *            the arguments
     * @return result
     * @throws Exception
     *             Exception
     */
    public int execute(String[] arguments) throws Exception {
        return execute(new SignerFactory(), arguments, logger);
    }

    /**
     * @param signerFactory
     *            Signer factory instance to use
     * 
     * @param arguments
     *            Command line arguments
     * 
     * @param l
     *            this logger will be used when we want to logg information.
     * 
     * @return Error code (0 if no error)
     * @throws Exception
     *             Exception
     */
    public int execute(ISignerFactory signerFactory, String[] arguments, Logger l) throws Exception {
        int result = SUCCESS;

        ICommandLineProxy commandLine = new CommandLineProxy();
        CommandLineParser argumentParser = new PosixParser();
        ISignerSettings signerSettings = null;

        try {

            // Create and parse command line options
            if (arguments.length == 0) {
                printStream.append("No arguments given, possible usage is:\n\n");
                commandLineOptions = CommandLineOptionsFactory.createCommandLineOptions(Sign.SIGN_A2);
                SignHelper.printHelp(commandLineOptions, Sign.SIGN_A2, printStream);
                printStream.append("\n\n");
                commandLineOptions = CommandLineOptionsFactory.createCommandLineOptions(Sign.SIGN_L9540);
                SignHelper.printHelp(commandLineOptions, Sign.SIGN_L9540, printStream);
                printStream.append("\n\n");
                commandLineOptions = CommandLineOptionsFactory.createCommandLineOptions(Sign.SIGN_U8500);
                SignHelper.printHelp(commandLineOptions, Sign.SIGN_U8500, printStream);
                printStream.append("\n\n");
                commandLineOptions = CommandLineOptionsFactory.createCommandLineOptions(Sign.SIGN_U5500);
                SignHelper.printHelp(commandLineOptions, Sign.SIGN_U5500, printStream);
                printStream.append("\n\n");
                commandLineOptions = CommandLineOptionsFactory.createCommandLineOptions(Sign.SIGN_M7X00);
                SignHelper.printHelp(commandLineOptions, Sign.SIGN_M7X00, printStream);
                printStream.append("\n\n");
                commandLineOptions = CommandLineOptionsFactory.createCommandLineOptions(Sign.SIGN_LOAD_MODULES);
                SignHelper.printHelp(commandLineOptions, Sign.SIGN_LOAD_MODULES, printStream);
                return ERROR;
            }

            commandLineOptions = CommandLineOptionsFactory.createCommandLineOptions(arguments[0]);
            commandLine.setProxiedObject(argumentParser.parse(commandLineOptions, arguments));

            // Parse and handle initial arguments
            if (parseBaseArguments(signerFactory, commandLine, l, signerSettings)) {
                return result;
            }
            // Look for input and output files
            String[] fileArguments = commandLine.getArgs();
            checkFileArguments(fileArguments);

            // Create specialized signer settings based on the sign package and the input filename
            signerSettings = signerFactory.createSignerSettings(new Object[] {
                fileArguments[1], fileArguments[2]}, commandLine);

            if (signerSettings == null) {
                throw new SignerException("Failed to create signer settings.");
            }

            boolean useCommonFolderForKeysAndPackages = 
                    (commandLine.hasOption(GenericCommandLineOptions.GenericOption.USE_COMMON_FOLDER.getShortCMD()) 
                    || commandLine.hasOption(GenericCommandLineOptions.GenericOption.USE_COMMON_FOLDER.getLongCMD()))
                            && (checkSoftwareType(signerSettings));


            signerSettings.setSignerSetting(ISignerSettings.COMMON_FOLDER_KEY_AND_PACKAGES, useCommonFolderForKeysAndPackages);

            ISignerFacade facade = signerFactory.createSignerFacade(arguments[0]);
            if (facade == null) {
                throw new Exception("Signer facade is null");
            }
            facade.sign(signerSettings, this, commandLine.hasOption(A2CommandLineOptions.Option.LOCAL_SIGNING
                .getLongCMD()));
        } catch (SoftwareAlreadySignedException e) {
            logInfoMessage(e.getMessage());
        } catch (Exception e) {
            logErrorMessage(e.getMessage());
            throw e;
        }
        return result;
    }

    private boolean checkSoftwareType(ISignerSettings signerSettings){
        boolean allowedSoftwareTypesForCommonFolder = true;

        try{
            Object swTypeObject = signerSettings.getSignerSetting(IGenericSignerSettings.KEY_SW_TYPE);

            if(swTypeObject != null && swTypeObject instanceof GenericSoftwareType 
                    && (GenericSoftwareType) swTypeObject == GenericSoftwareType.CRKC){
                GenericSoftwareType swType = (GenericSoftwareType) swTypeObject;
                if(swType == GenericSoftwareType.CRKC){
                    allowedSoftwareTypesForCommonFolder = false;
                }
            }
        }catch(Exception e){
            logger.error(e.getMessage());
        }
        return allowedSoftwareTypesForCommonFolder;
    }

    private void logInfoMessage(String message) {
        println("");
        if (logger != null) {
            logger.info(message);
            println(message);
        }
        println("");
    }

    private void logErrorMessage(String message) {
        println("");
        if (logger != null) {
            logger.error(message);
        }
        println("");
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
     * @param signerSettings
     *         passing info for overriding env variables. 
     * 
     * @throws ParseException
     *             parse exception
     * 
     * @throws SignerException
     *             TBD
     */
    public boolean parseBaseArguments(ISignerFactory signerFactory, ICommandLineProxy commandLine, 
             Logger l, ISignerSettings signerSettings)
        throws ParseException, SignerException {
        // Some basic options
        if (commandLine.hasOption(A2CommandLineOptions.Option.HELP.getShortCMD())) {
            SignHelper.printHelp(commandLineOptions, commandLine.getArgs()[0], printStream);
            return true;
        }

        // Handle "list sign packages" if option is available and return
        if (commandLine.hasOption(A2CommandLineOptions.Option.LIST.getLongCMD())) {
            signerSettings = signerFactory.createSignerSettings(new Object[]{}, commandLine);
            handleListSignPackages(signerFactory, commandLine.hasOption(A2CommandLineOptions.Option.LOCAL_SIGNING
                .getLongCMD()), commandLine.hasOption(GenericOption.USE_COMMON_FOLDER.getLongCMD()), signerSettings);
            return true;
        }

        if (commandLine.hasOption(A2CommandLineOptions.Option.VERBOSE.getShortCMD())) {
            verbose = true;
        }

        validateArguments(commandLine, l);

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
    public void validateArguments(ICommandLineProxy commandLine, Logger l) throws ParseException {
        CommandLineOptionsValidator.validateArguments(commandLine, l);
    }

    /**
     * {@inheritDoc}
     */
    public void signerMessage(ISigner signer, String message) {
        if (verbose) {
            // Print message to standard output
            println(message);
        }
    }

    /*
     * (non-Javadoc)
     * 
     * @see com.stericsson.sdk.signing.ISignerServiceListener#signerServiceMessage
     * (com.stericsson.sdk .signing.ISignerService, java.lang.String)
     */
    /**
     * @param signer
     *            Signer reporting progress
     * @param message
     *            Message from the signer service
     */
    public void signerServiceMessage(ISignerService signer, String message) {
        if (verbose) {
            println(message);
        }
    }

    private void handleListSignPackages(ISignerFactory signerFactory, boolean local, 
            boolean common, ISignerSettings signerSettings) throws SignerException {

        ISignerFacade facade = signerFactory.createSignerFacade("");
        if (facade == null) {
            throw new SignerException("Failed to create signer facade");
        }

        ISignPackageInformation[] signPackagesInformation = facade.getSignPackages(this, local, common, signerSettings);

        if (signPackagesInformation != null && signPackagesInformation.length > 0) {

            String message = "Listing " + signPackagesInformation.length + " available sign packages";

            signerServiceMessage(null, message);

            for (ISignPackageInformation info : signPackagesInformation) {
                println(info.getName());
            }
        } else {
            println("No available sign packages found");
        }
    }

    private void checkFileArguments(String[] fileArguments) throws SignerException {
        if (fileArguments.length < 3) {
            throw new SignerException("You must specify input and output file " + fileArguments.length);
        }
    }
}
