package com.stericsson.sdk.signing.cli.m7x00;

import java.util.ArrayList;
import java.util.Collection;
import java.util.List;

import org.apache.commons.cli.Option;
import org.apache.commons.cli.ParseException;

import com.stericsson.sdk.signing.cli.GenericCommandLineOptions;

/**
 * @author xtomzap
 * 
 */
public class M7X00CommandLineOptions extends GenericCommandLineOptions {

    /** */
    private static final long serialVersionUID = 5211649556514364382L;

    private static Validator internalValidator = new Validator();

    /**
     * @author xolabju
     */
    public enum M7X00Option {
        /**
         * 
         */
        LOAD_ADDRESS_XML("xml", "load-xml", true, "specifies name and location for XML containing TOC-LoadAdress",
            internalValidator);

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
        private M7X00Option(String shortCMD, String longCMD, boolean hasArg, String description, Validator val) {

            this(shortCMD, longCMD, hasArg, description, val, false);
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
        private M7X00Option(String shortCMD, String longCMD, boolean hasArg, String description, Validator val,
            boolean req) {
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
    public M7X00CommandLineOptions() throws ParseException, NullPointerException {
        super();

        // Make sure this configuration matches the one documented
        for (M7X00Option i : M7X00Option.values()) {
            this.addOption(i.getShortCMD(), i.getLongCMD(), i.takesArg(), i.getDescription());
        }
    }

    /**
     * 
     * {@inheritDoc}
     */
    @Override
    public Collection<Option> getOptions() {
        List<Option> options = new ArrayList<Option>();
        options.addAll(super.getOptions());

        for (M7X00Option o : M7X00Option.values()) {
            try {
                Option option = new Option(o.getShortCMD(), o.getLongCMD(), o.takesArg(), o.getDescription());
                option.setRequired(o.isRequired());
                options.add(option);
            } catch (Exception e) {
                // Should not get here since options are validated in constructor.
                e.printStackTrace();
            }
        }

        return options;
    }

}
