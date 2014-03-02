package com.stericsson.sdk.signing.cli;

import java.util.ArrayList;
import java.util.Collection;
import java.util.List;
import java.util.Locale;
import java.util.Vector;

import org.apache.commons.cli.Option;
import org.apache.commons.cli.Options;
import org.apache.commons.cli.ParseException;

import com.stericsson.sdk.signing.generic.GenericSoftwareType;

/**
 * Use this class to setup the supported generic options and flags to be used by the CLI when
 * signing software.
 * 
 * @author xolabju
 * 
 */
public class GenericCommandLineOptions extends Options {

    /** */
    private static final long serialVersionUID = -1439788086043200343L;

    /***/
    private static Vector<String> supportedSoftwareTypes = new Vector<String>();
    static {
        for (GenericSoftwareType swType : GenericSoftwareType.values()) {
            supportedSoftwareTypes.add(swType.name().toLowerCase(Locale.getDefault()));
        }
    }

    /***/
    private static Vector<String> supportedHashTypes = new Vector<String>();
    static {
        supportedHashTypes.add("SHA-1");
        supportedHashTypes.add("SHA-256");
        supportedHashTypes.add("SHA-384");
        supportedHashTypes.add("SHA-512");
    }

    protected static class Validator {
        /**
         * 
         */
        public Validator() {

        }

        /**
         * Validates short command
         * 
         * @param shortCommand
         *            Short command.
         * @throws ParseException
         *             Parse exception.
         */
        public void validShortCMD(String shortCommand) throws ParseException {
            if (shortCommand != null && shortCommand.equals("")) {
                throw new ParseException("Empty Strings not valid as a short command, should be NULL value.");
            }
        }

        /**
         * Validates long command.
         * 
         * @param longCommand
         *            Long command.
         * @throws ParseException
         *             Parse exception.
         * @throws NullPointerException
         *             Null pointer exception.
         */
        public void validLongCMD(String longCommand) throws ParseException, NullPointerException {
            if (longCommand == null) {
                throw new NullPointerException("NULL not valid as long command");
            } else if (longCommand.equals("")) {
                throw new ParseException("Empty String not valid as long command");
            }
        }

        /**
         * Validates command description.
         * 
         * @param commandDescription
         *            Command description.
         * @throws ParseException
         *             Parse exception.
         * @throws NullPointerException
         *             Null pointer exception.
         */
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
     * @author xolabju
     */
    public enum GenericOption {
        /**
         * 
         */
        HELP("h", "help", false, "will show you this help text", internalValidator),

        /**
         * 
         */
        HASH_BLOCK_SIZE("b", "hash-block-size", true, "set flash archive hash block size", internalValidator),

        /**
         * 
         */
        PACKAGE("p", "sign-package", true, "set sign package to use.", internalValidator, true),

        /**
         * 
         */
        LIST("l", "list-packages", false, "list available sign packages", internalValidator),

        /**
         * 
         */
        VERBOSE("v", "verbose", false, "to enable visual output to stdout.", internalValidator),

        /**
         * 
         */
        FORCE("f", "force", false, "force overwrite if output already exists.", internalValidator),

        /**
         * 
         */
        SW_TYPE("s", "sw-type", true, "set SW type, possible arguments: " + getSupportedSoftwareTypesAsString() + ".",
            internalValidator, true),

        /**
         * 
         */
        LOCAL_SIGNING(null, "local", false, "disables the use of sign server and uses local sign packages and keys. "
            + "This requires LOCAL_KEYROOT and LOCAL_SIGNPACKAGEROOT system variables to be set.", internalValidator,
            false),

        /**
         * 
         */
        ENABLE_PKA("k", "enable-pka", false, "enable PKA acceleration", internalValidator, false),

        /**
         * 
         */
        ENABLE_DMA("m", "enable-dma", false, "enable DMA acceleration", internalValidator, false),

        /**
         * 
         */
        LOAD_ADDRESS("la", "load-address", true,
            "set load address of ISSW in secure RAM or load address for generic signed software", internalValidator,
            false),

        /**
             * 
             */
        ROOT_KEY_HASH_TYPE("r", "root-key-hash-type", true, "set root key hash type, possible arguments: "
            + getSupportedHashTypesAsString() + ".", internalValidator, false),

        /**
         * 
         */
        PAYLOAD_HASH_TYPE("ph", "payload-hash-type", true, "set payload hash type, possible arguments: "
            + getSupportedHashTypesAsString() + ".", internalValidator, false),

        /**
             * 
             */
        SIGNATURE_HASH_TYPE("sh", "signature-hash-type", true, "set signature hash type, possible arguments: "
            + getSupportedHashTypesAsString() + ".", internalValidator, false),
        /**
         * 
         */
        START_ADDRESS("sa", "start-address", true,
            "set start address of ISSW in secure RAM or startup address for generic signed software",
            internalValidator, false),

        /**
         * 
         */
        KEY_HASH_FILE(null, "key-hash", true, "specifies output path to key hash file", internalValidator, false),

        /**
          * 
         */
        BUFFER_SIZE(null, "buffer-size", true,
            "sets the buffer size to use when creating flash archives (in bytes). Valid prefixes are k and M, e.g. "
                + "128k or 1M. Default value is 256k", internalValidator, false),

        /**
         * 
         */
        SW_VERSION("sv", "sw-version", true, "set SW version", internalValidator, false),

        /**
         * 
         */
        MAJOR_BUILD_VERSION(null, "major-build-version", true, "set major build version of signed ISSW binary",
            internalValidator, false),

        /**
         * 
         */
        MINOR_BUILD_VERSION(null, "minor-build-version", true, "set minor build version of signed ISSW binary",
            internalValidator, false),

        /**
         * 
         */
        FLAGS(null, "flags", true, "set flags of signed ISSW binary", internalValidator, false),

        /**
         * 
         */
        SIGN_KEY("sk", "sign-key", true, "set sign key to use and override package.", internalValidator, false),

        /**
         * 
         */
        SIGN_SERVERS("ss", "sign-servers", true, 
                "Points to a sign server. Command line overwrites environment variables.",
                internalValidator, false),

        /**
         * 
         */
        SIGNATURE_TYPE("st", "signature-type", true, "set signature type of the sign key.", internalValidator, false),

        /**
         * 
         */
        SIGN_PACKAGE_ROOT("spr", "sign-package-root", true, 
                "Point to the share that contains the sign packages." 
                + "REQUIRED in Linux environment. Command line overwrites environment variables.", 
                internalValidator, false),

        /**
         * 
         */
        LOCAL_SIGN_PACKAGE_ROOT("lspr", "local-sign-package-root", true, 
                "Point to the local sign package root." 
                + " Command line overwrites environment variables.", 
                internalValidator, false),

        /**
         * 
         */
        LOCAL_KEY_ROOT("lkr", "local-key-root", true, 
                "Point to the local key root (used for software signing)." 
                + " Command line overwrites environment variables.", 
                internalValidator, false),

        /**
         * 
         */
        LOCAL_ENCRYPTION_KEY_ROOT("lekr", "local-encription-key-root", true, 
                "Point to the local encryption key root (used for loader encryption)." 
                + " Command line overwrites environment variables.", 
                internalValidator, false),

        /**
         * 
         */
        USE_COMMON_FOLDER("ucf", "use-common-folder", false, "specifies to use common folder for keys and packages", 
                internalValidator, false);
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
        private GenericOption(String shortCMD, String longCMD, boolean hasArg, String description, Validator val) {

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
        private GenericOption(String shortCMD, String longCMD, boolean hasArg, String description, Validator val,
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
    public GenericCommandLineOptions() throws ParseException, NullPointerException {
        super();

        // Make sure this configuration matches the one documented
        for (GenericOption i : GenericOption.values()) {
            this.addOption(i.getShortCMD(), i.getLongCMD(), i.takesArg(), i.getDescription());
        }
    }

    /**
     * Returns collection of all options including inherited ones.
     * 
     * @return Collection of all options including inherited ones.
     */
    @Override
    public Collection<Option> getOptions() {
        List<Option> options = new ArrayList<Option>();

        for (GenericOption o : GenericOption.values()) {
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

    /**
     * Check if software type entered by user is supported
     * 
     * @param swType
     *            software type
     * @return true if supported, else false
     */
    public static boolean isSWTypeSupported(String swType) {
        for (String item : supportedSoftwareTypes) {
            if (swType.equalsIgnoreCase(item)) {
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
        return supportedSoftwareTypes;
    }

    private static String getSupportedSoftwareTypesAsString() {
        StringBuffer buffer = new StringBuffer();

        for (int i = 0; i < (supportedSoftwareTypes.size() - 1); i++) {
            buffer.append(supportedSoftwareTypes.get(i) + ", ");
        }
        buffer.append(supportedSoftwareTypes.lastElement());

        return buffer.toString();
    }

    private static String getSupportedHashTypesAsString() {
        StringBuffer buffer = new StringBuffer();

        for (int i = 0; i < (supportedHashTypes.size() - 1); i++) {
            buffer.append(supportedHashTypes.get(i) + ", ");
        }
        buffer.append(supportedHashTypes.lastElement());

        return buffer.toString();
    }

    /**
     * Check if root key hash type entered by user is supported
     * 
     * @param hashType
     *            hash type
     * @return true if supported, else false
     */
    public static boolean isHashTypeSupported(String hashType) {
        for (String item : supportedHashTypes) {
            if (hashType.equalsIgnoreCase(item)) {
                return true;
            }
        }
        return false;
    }

}
