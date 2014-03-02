package com.stericsson.sdk.assembling.internal.u8500;

import java.util.Collections;
import java.util.Vector;

import org.apache.commons.cli.Options;
import org.apache.commons.cli.ParseException;

/**
 * Use this class to setup the supported options and flags to be used by the CLI when assembling
 * U8500 software.
 * 
 * @author xolabju
 * 
 */
public class U8500CommandLineOptions extends Options {

    private static final long serialVersionUID = 1673929056009515770L;

    /***/
    public static final String TYPE_FLASH_ARCHIVE = "flash_archive";

    /***/
    public static final String TYPE_RAM_IMAGE = "ram_image";

    /***/
    public static final String TYPE_PREFLASH_IMAGE = "preflash_image";

    /***/
    private static Vector<String> supportedTypes = new Vector<String>();
    static {
        Collections.addAll(supportedTypes, TYPE_FLASH_ARCHIVE, TYPE_RAM_IMAGE, TYPE_PREFLASH_IMAGE);
    }

    static class Validator {
        public void validShortCMD(String shortCommand) throws ParseException {
            if (shortCommand != null && shortCommand.equals("")) {
                throw new ParseException("Empty Strings not valid as a short command, should be NULL value.");
            }
        }

        public void validLongCMD(String longCommand) throws ParseException, NullPointerException {
            if (longCommand == null) {
                throw new NullPointerException("NULL not valid as long command");
            } else if (longCommand.equals("")) {
                throw new ParseException("Empty String not valid as long command");
            }
        }

        public void validDescription(String commandDescription) throws ParseException, NullPointerException {
            if (commandDescription == null) {
                throw new NullPointerException("NULL not valid as description");
            } else if (commandDescription.equals("")) {
                throw new ParseException("Empty String not valid as description");
            }
        }
    }

    private static Validator internalValidator = new Validator();

    /**
     * @author xolabju
     */
    public enum Option {
        /**
         * 
         */
        HELP("h", "help", false, "show this help text", internalValidator),

        /**
         * 
         */
        OUTPUT_TYPE("t", "type", true, "set the type of software to assemble, possible values: "
            + "flash_archive | ram_image | preflash_image", internalValidator, true),

        /**
         * 
         */
        CONFIGURATION("c", "configuration", true, "set the XML configuration file to use", internalValidator, true),

        /**
         * 
         */
        FILE_LIST("l", "file_list", true, "set the file list to use", internalValidator, true),

        /**
         * 
         */
        VERBOSE("v", "verbose", false, "enable visual output to stdout", internalValidator),

        /**
         * 
         */
        FORCE("f", "force", false, "force overwrite if output file already exists", internalValidator),

        /**
         * 
         */
        TRIM_FILE("r", "trim_file", false, "include Trim Area file in flash archive", internalValidator),

        /**
         *
         */
        BUFFER_SIZE(null, "buffer-size", true,
            "set the buffer size to use when creating flash archive (in bytes), valid suffixes are k and M (e.g. "
                + "128k or 1M), default value is 8k", internalValidator, false),

        /**
         *
         */
        ALIGNMENT_SIZE(null, "alignment-size", true,
            "set the alignment size of flash archive entries (in bytes), default value is 512", internalValidator);

        /**
         * 
         */

        /*
         * @see Option(String shortCMD, String longCMD, boolean hasArg, String description,
         * Validator val)
         */
        private String shortCommand;

        /*
         * @see Option(String shortCMD, String longCMD, boolean hasArg, String description,
         * Validator val)
         */
        private String longCommand;

        /*
         * @see Option(String shortCMD, String longCMD, boolean hasArg, String description,
         * Validator val)
         */
        private boolean hasArgument;

        /*
         * @see Option(String shortCMD, String longCMD, boolean hasArg, String description,
         * Validator val)
         */
        private Validator validator;

        /*
         * @see Option(String shortCMD, String longCMD, boolean hasArg, String description,
         * Validator val)
         */
        private String commandDescription;

        /*
         * If this option should be required by the user.
         */
        boolean required;

        /**
         * @param shortCMD
         *            a short command for the command, ie "-f".
         * @param longCMD
         *            a long command for the command, ie "--force".
         * @param hasArg
         *            set this to true if the command takes an argument. Default: false.
         * @param description
         *            a longer description related to the option.
         * @param validator
         *            a dependency injection of a argument validator.
         */
        private Option(String shortCMD, String longCMD, boolean hasArg, String description, Validator val) {

            init(shortCMD, longCMD, hasArg, description, val, false);
        }

        /**
         * @param shortCMD
         *            a short command for the command, ie "-f".
         * @param longCMD
         *            a long command for the command, ie "--force".
         * @param hasArg
         *            set this to true if the command takes an argument. Default: false.
         * @param description
         *            a longer description related to the option.
         * @param validator
         *            a dependency injection of a argument validator.
         * @param req
         *            if this option is required to be specified by the user.
         */
        private Option(String shortCMD, String longCMD, boolean hasArg, String description, Validator val, boolean req) {
            init(shortCMD, longCMD, hasArg, description, val, req);
        }

        /**
         * @param shortCMD
         *            a short command for the command, ie "-f".
         * @param longCMD
         *            a long command for the command, ie "--force".
         * @param hasArg
         *            set this to true if the command takes an argument. Default: false.
         * @param description
         *            a longer description related to the option.
         * @param validator
         *            a dependency injection of a argument validator.
         * @param req
         *            if this option is required to be specified by the user.
         */
        private void init(String shortCMD, String longCMD, boolean hasArg, String description, Validator val,
            boolean req) {

            shortCommand = null;
            longCommand = null;
            hasArgument = false;
            commandDescription = null;
            validator = null;

            shortCommand = shortCMD;
            longCommand = longCMD;
            hasArgument = hasArg;
            commandDescription = description;
            validator = val;
            required = req;
        }

        /**
         * @return true if the user most specify the param at run time.
         */
        public boolean isRequired() {
            return required;
        }

        /**
         * @return the short command for the option, ie -f.
         * @throws ParseException
         *             parseException
         */
        public String getShortCMD() throws ParseException {
            validator.validShortCMD(shortCommand);
            return shortCommand;
        }

        /**
         * @return the short command for the option, ie --force.
         * @throws ParseException
         *             parseException
         * @throws NullPointerException
         *             nullpointerException
         */
        public String getLongCMD() throws ParseException, NullPointerException {
            validator.validLongCMD(longCommand);
            return longCommand;
        }

        /**
         * @return true if the option takes arguments, false otherwise.
         */
        public boolean takesArg() {
            return hasArgument;
        }

        /**
         * @return a description of the command.
         * @throws ParseException
         *             parseException
         * @throws NullPointerException
         *             nullpointerException
         */
        public String getDescription() throws ParseException, NullPointerException {
            validator.validDescription(commandDescription);
            return commandDescription;
        }
    }

    /**
     * Constructor, will initiate the parent class and add options to the parent.
     * 
     * @throws ParseException
     *             parseException
     * @throws NullPointerException
     *             nullpointerException
     */
    public U8500CommandLineOptions() throws ParseException, NullPointerException {
        super();

        // Make sure this configuration matches the one documented
        for (Option i : Option.values()) {
            this.addOption(i.getShortCMD(), i.getLongCMD(), i.takesArg(), i.getDescription());
        }
    }

    /**
     * Check if assembly type entered by user is supported
     * 
     * @param type
     *            assembly type
     * @return true if supported, else false
     */
    public static boolean isOutputTypeSupported(String type) {
        if (type == null) {
            return false;
        }
        for (String item : supportedTypes) {
            if (type.equalsIgnoreCase(item)) {
                return true;
            }
        }
        return false;
    }

    /**
     * 
     * @return TBD
     */
    public static Vector<String> getSupportedSoftwareTypes() {
        return supportedTypes;
    }

}
