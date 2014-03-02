package com.stericsson.sdk.signing.cli.loadmodules;

import org.apache.commons.cli.Options;
import org.apache.commons.cli.ParseException;

/**
 * Use this class to setup the supported options and flags to be used by the CLI when signing list
 * of load modules software.
 * 
 * @author xtomzap
 * 
 */
public class LoadModulesCommandLineOptions extends Options {

    /***/
    private static final long serialVersionUID = 6516239075594482315L;

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
                throw new NullPointerException("NULL not valid as long command");
            } else if (commandDescription.equals("")) {
                throw new ParseException("Empty String not valid as long command");
            }
        }
    }

    private static Validator internalValidator = new Validator();

    /**
     * @author xtomzap
     */
    public enum Option {
        /***/
        HELP("h", "help", false, "will show you this help text", internalValidator),

        /***/
        VERBOSE("v", "verbose", false, "to enable visual output to stdout.", internalValidator),

        /***/
        CONFIGURATION("c", "configuration", true, "path to configuration xml file", internalValidator, true);


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
    public LoadModulesCommandLineOptions() throws ParseException, NullPointerException {
        super();

        // Make sure this configuration matches the one documented
        for (Option i : Option.values()) {
            this.addOption(i.getShortCMD(), i.getLongCMD(), i.takesArg(), i.getDescription());
        }
    }

}
