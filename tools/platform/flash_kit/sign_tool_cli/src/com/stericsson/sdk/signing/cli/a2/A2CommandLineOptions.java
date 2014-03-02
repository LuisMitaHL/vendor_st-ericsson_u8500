package com.stericsson.sdk.signing.cli.a2;

import java.util.Collections;
import java.util.Vector;

import org.apache.commons.cli.Options;
import org.apache.commons.cli.ParseException;

/**
 * Use this class to setup the supported options and flags to be used by the CLI when signing A2
 * software.
 * 
 * @author Daniel Ekelund <daniel.xd.ekelund@stericsson.com>
 * @author xtomlju
 */
public class A2CommandLineOptions extends Options {

    /***/
    public static final String SW_TYPE_IMAGE = "image";

    /***/
    public static final String SW_TYPE_LOADER = "loader";

    /***/
    public static final String SW_TYPE_PROLOGUE = "prologue";

    /***/
    public static final String SW_TYPE_GENERIC = "generic";

    /***/
    public static final String SW_TYPE_ARCHIVE = "archive";

    /***/
    public static final String SW_TYPE_TAR = "tar";

    /***/
    public static final String SW_TYPE_ELF = "elf";

    /***/
    public static final String SW_TYPE_FLASH = "flash";

    /***/
    private static Vector<String> supportedSoftwareTypes = new Vector<String>();
    static {
        Collections.addAll(supportedSoftwareTypes, SW_TYPE_IMAGE, SW_TYPE_LOADER, SW_TYPE_PROLOGUE, SW_TYPE_GENERIC,
            SW_TYPE_ARCHIVE, SW_TYPE_ELF, SW_TYPE_FLASH, SW_TYPE_TAR);
    }

    /***/
    public static final String TARGET_APP = "app";

    /***/
    public static final String TARGET_ACC = "acc";

    /***/
    private static Vector<String> supportedTargetCPU = new Vector<String>();
    static {
        Collections.addAll(supportedTargetCPU, TARGET_APP, TARGET_ACC);
    }

    /***/
    public static final String INTERACTIVE_STATIC = "static";

    /***/
    public static final String INTERACTIVE_INTERACTIVE = "interactive";

    /***/
    private static Vector<String> supportedInteractiveLoadBits = new Vector<String>();
    static {
        Collections.addAll(supportedInteractiveLoadBits, INTERACTIVE_STATIC, INTERACTIVE_INTERACTIVE);
    }

    /***/
    public static final String PTYPE_NAND = "nand";

    /***/
    public static final String PTYPE_NOR = "nor";

    private static Vector<String> supportedPayloadTypes = new Vector<String>();
    static {
        Collections.addAll(supportedPayloadTypes, "nand", "nor", "physical");
    }

    /***/
    public static final String HDR_SEC_AS_IS = "asIs";

    /***/
    public static final String HDR_SEC_REPLACE_SIGN = "replaceSign";

    /***/
    public static final String HDR_SEC_VERIFY_ONLY = "verifyOnly";

    /***/
    private static Vector<String> supportedHeaderSecurity = new Vector<String>();
    static {
        Collections.addAll(supportedHeaderSecurity, HDR_SEC_AS_IS, HDR_SEC_REPLACE_SIGN, HDR_SEC_VERIFY_ONLY);
    }

    /***/
    public static final String APP_SEC_ON = "on";

    /***/
    public static final String APP_SEC_OFF = "off";

    /***/
    public static final String APP_SEC_DATA_LOADING = "dataLoading";

    /***/
    public static final String APP_SEC_PAF_DEPENDENT = "PAFdependent";

    /***/
    private static Vector<String> supportedApplicationSecurity = new Vector<String>();
    static {
        Collections.addAll(supportedApplicationSecurity, APP_SEC_ON, APP_SEC_OFF, APP_SEC_DATA_LOADING,
            APP_SEC_PAF_DEPENDENT);
    }

    /***/
    public static final String ADDR_FORMAT_PAGES = "pages";

    /***/
    public static final String ADDR_FORMAT_4B = "4B";

    /***/
    private static Vector<String> supportedAddressFormat = new Vector<String>();
    static {
        Collections.addAll(supportedAddressFormat, ADDR_FORMAT_PAGES, ADDR_FORMAT_4B);
    }

    /***/
    public static final String MAC_MODE_CONFIG = "Config";

    /***/
    public static final String MAC_MODE_2NDSBC = "2ndSBC";

    /***/
    public static final String MAC_MODE_3RDSBC = "3rdSBC";

    /***/
    public static final String MAC_MODE_4THSBC = "4thSBC";

    /***/
    public static final String MAC_MODE_SW = "SW";

    /***/
    public static final String MAC_MODE_LICENCE = "Licence";

    /***/
    public static final String MAC_MODE_MAC7 = "Mac7";

    /***/
    public static final String MAC_MODE_MAC8 = "Mac8";

    /***/
    private static Vector<String> supportedMacMode = new Vector<String>();
    static {
        Collections.addAll(supportedMacMode, MAC_MODE_CONFIG, MAC_MODE_2NDSBC, MAC_MODE_3RDSBC, MAC_MODE_4THSBC,
            MAC_MODE_SW, MAC_MODE_LICENCE, MAC_MODE_MAC7, MAC_MODE_MAC8);
    }

    private static final long serialVersionUID = 8216076538493489053L;

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
     * @author Daniel Ekelund <daniel.xd.ekelund@stericsson.com>
     * @author xtomlju
     */
    public enum Option {
        /**
         * 
         */
        HELP("h", "help", false, "will show you this help text", internalValidator),

        /**
         * 
         */
        ENCRYPT("e", "encrypt", false, "encrypt the signed loader", internalValidator),

        /**
         * 
         */
        COMPRESS("c", "compress", false, "compress the signed loader", internalValidator),

        /**
         * 
         */
        TARGET("t", "target", true, "set target CPU, possible values: app, acc.", internalValidator, true),

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
        IN_FORMAT("i", "in-format", true, "set the kind of input file format.", internalValidator),

        /**
         * 
         */
        OUT_FORMAT("o", "out-format", true, "set the kind of input file format.", internalValidator),

        /**
         * 
         */
        DRYRUN("d", "dryrun", false, "to enable fake mode, with this enabled the software will never be signed.",
            internalValidator),

        /**
         * 
         */
        VERBOSE("v", "verbose", false, "to enable visual output to stdout.", internalValidator),

        /**
         * 
         */
        TEST("b", "test", false, "same as -f -v.", internalValidator),

        /**
         * 
         */
        FORCE("f", "force", true, "force overwrite if output already exists.", internalValidator),

        /**
         * 
         */
        MAC_MODE("m", "mac-mode", true,
            "set MAC mode, possible argument: Config, 2ndSBC, 3rdSBC, 4thSBC, SW, License, Mac7, Mac8.",
            internalValidator, true),

        /**
         * 
         */
        SW_TYPE("s", "sw-type", true,
            "set SW type, possible argument: image, loader, prologue, generic, archive, elf, flash.",
            internalValidator, true),

        /**
         * 
         */
        INTERACTIVE(null, "interactive", true, "set Interactive load bit, possible argument: static, interactive.",
            internalValidator, true),

        /**
         * 
         */
        APP_SEC(null, "app-sec", true,
            "set Application security, possible argument: on, off, dataLoading, PAFdependent.", internalValidator, true),

        /**
         * 
         */
        HDR_SEC(null, "hdr-sec", true,
            "set Header security, possible argument: asIs=0xdestAddr, replaceSign=0xdestAddr, verifyOnly.",
            internalValidator, true),

        /**
         * 
         */
        PTYPE(null, "ptype", true, "set payload type, possible argument: nand, nor, physical.", internalValidator),

        /**
         * 
         */
        ADDR_FORMAT(null, "addr-format", true, "set address format(NAND only), possible argument: pages, 4B.",
            internalValidator, true),

        /**
         * 
         */
        DEBUG(null, "debug", false, "enable debug in the signed file.", internalValidator),

        /**
         * 
         */
        ENABLE_ETX_REAL("x", "enable-etx-real", false, "enable the ETX level real time.", internalValidator),

        /**
         * 
         */
        DISABLE_ETX_REAL(null, "disable-etx-real", false, "disable the ETX level real time.", internalValidator),

        /**
         * 
         */
        ENABLE_ETX_HEADER("u", "enable-etx-header", false, "enable ETX level header updated.", internalValidator),

        /**
         * 
         */
        DISABLE_ETX_HEADER(null, "disable-etx-header", false, "disable ETX level header updated.", internalValidator),

        /**
         * 
         */
        SHORT_MAC(null, "short-mac", false, "enable short MAC header.", internalValidator),

        /**
         * 
         */
        ANTI_ROLLBACK("a", "anti-rollback*", false, "require anti-rollback.", internalValidator),

        /**
         * 
         */
        ALIGN(null, "align", false, "enable alignment.", internalValidator),

        /**
         * 
         */
        S3_LENGTH(null, "s3-length", true, "set S3 length, will only work if input filetype is a01.", internalValidator),

        /**
         * 
         */
        SW_VERSION(null, "sw-version", true, "SW version (dec.: 0..255).", internalValidator, true),

        /**
         * 
         * 
         */
        RC_FILE(null, "rc-file", true, "rc file (filename)", internalValidator),

        /**
         * 
         */
        LOCAL_SIGNING(null, "local", false, "disables the use of sign server and uses local sign packages and keys. "
            + "This requires LOCAL_KEYROOT and LOCAL_SIGNPACKAGEROOT system variables to be set. "
            + "To enable loader encryption, also LOCAL_ENCRYPTIONKEYROOT shall be set.", internalValidator, false);

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
    public A2CommandLineOptions() throws ParseException, NullPointerException {
        super();

        // Make sure this configuration matches the one documented
        for (Option i : Option.values()) {
            this.addOption(i.getShortCMD(), i.getLongCMD(), i.takesArg(), i.getDescription());
        }
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
     * Check if target CPU entered by user is supported
     * 
     * @param targetCPU
     *            target CPU
     * @return true if supported, else false
     */
    public static boolean isTargetCPUSupported(String targetCPU) {
        for (String item : supportedTargetCPU) {
            if (targetCPU.equalsIgnoreCase(item)) {
                return true;
            }
        }
        return false;
    }

    /**
     * Check if interactive Load Bit entered by user is supported
     * 
     * @param interactiveLoadBit
     *            interactive Load Bit
     * @return true if supported, else false
     */
    public static boolean isInteractiveLoadBitSupported(String interactiveLoadBit) {
        for (String item : supportedInteractiveLoadBits) {
            if (interactiveLoadBit.equalsIgnoreCase(item)) {
                return true;
            }
        }
        return false;
    }

    /**
     * Check if payload type entered by user is supported
     * 
     * @param payloadType
     *            payloadType
     * @return true if supported, else false
     */
    public static boolean isPayloadTypeSupported(String payloadType) {
        for (String item : supportedPayloadTypes) {
            if (payloadType.equalsIgnoreCase(item)) {
                return true;
            }
        }
        return false;
    }

    /**
     * Check if header security entered by user is supported
     * 
     * @param headerSecurity
     *            header Security
     * @return true if supported, else false
     */
    public static boolean isHeaderSecuritySupported(String headerSecurity) {
        for (String item : supportedHeaderSecurity) {
            if (headerSecurity.equalsIgnoreCase(item)) {
                return true;
            }
        }
        return false;
    }

    /**
     * Check if application security entered by user is supported
     * 
     * @param appSecurity
     *            application Security
     * @return true if supported, else false
     */
    public static boolean isApplicationSecuritySupported(String appSecurity) {
        for (String item : supportedApplicationSecurity) {
            if (appSecurity.equalsIgnoreCase(item)) {
                return true;
            }
        }
        return false;
    }

    /**
     * Check if address format entered by user is supported
     * 
     * @param addressFormat
     *            address format
     * @return true if supported, else false
     */
    public static boolean isAddressFormatSupported(String addressFormat) {
        for (String item : supportedAddressFormat) {
            if (addressFormat.equalsIgnoreCase(item)) {
                return true;
            }
        }
        return false;
    }

    /**
     * Check if mac mode entered by user is supported
     * 
     * @param macMode
     *            mac mode
     * @return true if supported, else false
     */
    public static boolean isMacModeSupported(String macMode) {
        for (String item : supportedMacMode) {
            if (macMode.equalsIgnoreCase(item)) {
                return true;
            }
        }
        return false;
    }

    /**
     * 
     * @return TBD
     */
    public static Vector<String> getSupportedTargetCPU() {
        return supportedTargetCPU;
    }

    /**
     * 
     * @return TBD
     */
    public static Vector<String> getSupportedSoftwareTypes() {
        return supportedSoftwareTypes;
    }

    /**
     * 
     * @return TBD
     */
    public static Vector<String> getSupportedInteractiveLoadBits() {
        return supportedInteractiveLoadBits;
    }

    /**
     * 
     * @return TBD
     */
    public static Vector<String> getSupportedApplicationSecurity() {
        return supportedApplicationSecurity;
    }

    /**
     * 
     * @return TBD
     */
    public static Vector<String> getSupportedAddressFormat() {
        return supportedAddressFormat;
    }

    /**
     * 
     * @return TBD
     */
    public static Vector<String> getSupportedHeaderSecurity() {
        Vector<String> supported = new Vector<String>();
        Collections.addAll(supported, "asIs=0xdestAddr", "replaceSign=0xdestAddr", "verifyOnly");
        return supported;
    }

    /**
     * 
     * @return TBD
     */
    public static Vector<String> getSupportedPayloadTypes() {
        return supportedPayloadTypes;
    }

    /**
     * 
     * @return TBD
     */
    public static Vector<String> getSupportedMacMode() {
        return supportedMacMode;
    }

    /**
     * 
     * @return TBD
     */
    public static Vector<String> getSupportedSoftwareVersion() {
        Vector<String> supported = new Vector<String>();
        Collections.addAll(supported, "decimal number: 0..255");
        return supported;
    }
}
