package com.stericsson.sdk.signing.cli;

import java.util.Locale;

import org.apache.commons.cli.ParseException;

import com.stericsson.sdk.common.HexUtilities;
import com.stericsson.sdk.signing.ISignerSettings;
import com.stericsson.sdk.signing.SignerException;
import com.stericsson.sdk.signing.SignerSettingsException;
import com.stericsson.sdk.signing.a2.A2AccessControlList;
import com.stericsson.sdk.signing.a2.A2Signer;
import com.stericsson.sdk.signing.a2.A2SignerSettings;
import com.stericsson.sdk.signing.a2.A2SoftwareVersion;
import com.stericsson.sdk.signing.a2.IA2HeaderDestinationAddressConstants;
import com.stericsson.sdk.signing.a2.IA2MacModeConstants;
import com.stericsson.sdk.signing.a2.IA2PayloadTypeConstants;
import com.stericsson.sdk.signing.cli.a2.A2CommandLineOptions;
import com.stericsson.sdk.signing.cli.loadmodules.LoadModulesCommandLineOptions;
import com.stericsson.sdk.signing.cli.loadmodules.LoadModulesSignerSettings;
import com.stericsson.sdk.signing.cli.m7x00.M7X00CommandLineOptions;
import com.stericsson.sdk.signing.cops.ICOPSSignerSettings;
import com.stericsson.sdk.signing.generic.GenericSignerSettings;
import com.stericsson.sdk.signing.generic.GenericSoftwareType;
import com.stericsson.sdk.signing.generic.HashType;
import com.stericsson.sdk.signing.util.BasicFileValidator;
import com.stericsson.sdk.signing.util.ListPackagesSignerSettings;

/**
 * This class goes throught user input from command line and validates it. If input is ok,
 * signerSettings is set with values from commandline.
 * 
 * @author ezaptom
 * 
 */
public class CommandLineSignerSettingsSetup {
    /**
     * 
     * @param commandLine
     *            commandLine
     * @param signerSettings
     *            signerSettings
     * @throws SignerException
     *             signerException
     * @throws ParseException
     *             parseException
     * @throws NullPointerException
     *             nullPointerException
     */
    public void setupSettings(ICommandLineProxy commandLine, ISignerSettings signerSettings) throws SignerException,
        NullPointerException, ParseException {

        if (signerSettings instanceof A2SignerSettings) {
            setupA2Settings(commandLine, signerSettings);
        } else if (signerSettings instanceof LoadModulesSignerSettings) {
            setupLoadModulesSettings(commandLine, signerSettings);
        } else if (signerSettings instanceof ListPackagesSignerSettings) {
            setupListPackagesSettings(commandLine, signerSettings);
        } else {
            setupSignerSettings(commandLine, signerSettings);
        }
    }

    private void setupLoadModulesSettings(ICommandLineProxy commandLine, ISignerSettings signerSettings)
        throws ParseException, SignerException {

        String baxPath = commandLine.getOptionValue(LoadModulesCommandLineOptions.Option.CONFIGURATION.getShortCMD());
        if (baxPath == null) {
            throw new SignerException("You must specify configuration file: -c, --configuration");
        }

        BasicFileValidator.validateInputFile(baxPath);

        signerSettings.setSignerSetting(LoadModulesSignerSettings.KEY_CONFIGURATION, baxPath);
    }

    private void setupSignerSettings(ICommandLineProxy commandLine, ISignerSettings signerSettings)
        throws ParseException, SignerException {

        String softwareType = commandLine.getOptionValue(GenericCommandLineOptions.GenericOption.SW_TYPE.getShortCMD());
        if (softwareType == null) {
            throw new SignerException("You must specify software type: -s, --sw-type");
        }

        if (!GenericCommandLineOptions.isSWTypeSupported(softwareType)) {
            throw new SignerException("Software type '" + softwareType + "' is not supported. Supported values: "
                + GenericCommandLineOptions.getSupportedSoftwareTypes());
        }

        setGenericSoftwareType(signerSettings, softwareType);

        overrideEnvVariableSettings(signerSettings,commandLine);

        long loadAddress = 0x0;
        if (commandLine.hasOption(GenericCommandLineOptions.GenericOption.LOAD_ADDRESS.getShortCMD())) {
            loadAddress =
                Long.decode(commandLine.getOptionValue(GenericCommandLineOptions.GenericOption.LOAD_ADDRESS
                    .getShortCMD()));
        }
        signerSettings.setSignerSetting(GenericSignerSettings.KEY_LOAD_ADDRESS, loadAddress);

        long startAddress = 0x0;
        if (commandLine.hasOption(GenericCommandLineOptions.GenericOption.START_ADDRESS.getShortCMD())) {
            startAddress =
                Long.decode(commandLine.getOptionValue(GenericCommandLineOptions.GenericOption.START_ADDRESS
                    .getShortCMD()));
        }
        signerSettings.setSignerSetting(GenericSignerSettings.KEY_START_ADDRESS, startAddress);

        if (commandLine.hasOption(GenericCommandLineOptions.GenericOption.ENABLE_DMA.getLongCMD())) {
            signerSettings.setSignerSetting(GenericSignerSettings.KEY_DMA_FLAG, true);
        } else {
            signerSettings.setSignerSetting(GenericSignerSettings.KEY_DMA_FLAG, false);
        }

        if (commandLine.hasOption(GenericCommandLineOptions.GenericOption.ENABLE_PKA.getLongCMD())) {
            signerSettings.setSignerSetting(GenericSignerSettings.KEY_PKA_FLAG, true);
        } else {
            signerSettings.setSignerSetting(GenericSignerSettings.KEY_PKA_FLAG, false);
        }

        String hashBlockSize =
            commandLine.getOptionValue(GenericCommandLineOptions.GenericOption.HASH_BLOCK_SIZE.getShortCMD());
        if ((hashBlockSize != null) && (!"".equals(hashBlockSize.trim()))) {
            int size = Integer.decode(hashBlockSize);
            signerSettings.setSignerSetting(GenericSignerSettings.KEY_HASH_BLOCK_SIZE, size);
        }

        HashType rootKeyHashType = HashType.SHA256_HASH;
        if (commandLine.hasOption(GenericCommandLineOptions.GenericOption.ROOT_KEY_HASH_TYPE.getShortCMD())) {
            String rootKeyHashTypeStr =
                commandLine.getOptionValue(GenericCommandLineOptions.GenericOption.ROOT_KEY_HASH_TYPE.getShortCMD());
            if (!GenericCommandLineOptions.isHashTypeSupported(rootKeyHashTypeStr)) {
                throw new SignerException("Root key hash type not supported: " + rootKeyHashTypeStr);
            }
            rootKeyHashType = getHashType(rootKeyHashTypeStr);
        }
        signerSettings.setSignerSetting(GenericSignerSettings.KEY_ROOT_KEY_HASH_TYPE, rootKeyHashType);

        setPayloadHashType(commandLine, signerSettings);

        setSignatureHashType(commandLine, signerSettings);

        // Adds .xml Output filename, this is used by M7X00 in elf signing,
        // if user forgot to specify xml-filename parameter filename is used instead
        String loadAdressXMLPath = (String) signerSettings.getSignerSetting(ISignerSettings.KEY_OUTPUT_FILE);
        if (commandLine.hasOption(M7X00CommandLineOptions.M7X00Option.LOAD_ADDRESS_XML.getShortCMD())) {
            loadAdressXMLPath =
                commandLine.getOptionValue(M7X00CommandLineOptions.M7X00Option.LOAD_ADDRESS_XML.getShortCMD());
        }
        signerSettings.setSignerSetting(GenericSignerSettings.KEY_LOAD_ADDRESS_XML, loadAdressXMLPath);

        setKeyHashPath(commandLine, signerSettings);

        setU5500BufferSize(commandLine, signerSettings);

        setSoftwareVersion(commandLine, signerSettings);

        setBuildVersionsAndFlags(commandLine, signerSettings);
    }

    private void setSignatureHashType(ICommandLineProxy commandLine, ISignerSettings signerSettings)
        throws SignerException, ParseException {
        HashType signatureHashType = HashType.SHA256_HASH;
        if (commandLine.hasOption(GenericCommandLineOptions.GenericOption.SIGNATURE_HASH_TYPE.getShortCMD())) {
            String signatureHashTypeStr =
                commandLine.getOptionValue(GenericCommandLineOptions.GenericOption.SIGNATURE_HASH_TYPE.getShortCMD());
            if (!GenericCommandLineOptions.isHashTypeSupported(signatureHashTypeStr)) {
                throw new SignerException("Signature hash type not supported: " + signatureHashTypeStr);
            }
            signatureHashType = getHashType(signatureHashTypeStr);
        }
        signerSettings.setSignerSetting(GenericSignerSettings.KEY_SIGNATURE_HASH_TYPE, signatureHashType);
    }

    private void setPayloadHashType(ICommandLineProxy commandLine, ISignerSettings signerSettings)
        throws SignerException, ParseException {
        HashType payloadHashType = HashType.SHA256_HASH;
        if (commandLine.hasOption(GenericCommandLineOptions.GenericOption.PAYLOAD_HASH_TYPE.getShortCMD())) {
            String payloadHashTypeStr =
                commandLine.getOptionValue(GenericCommandLineOptions.GenericOption.PAYLOAD_HASH_TYPE.getShortCMD());
            if (!GenericCommandLineOptions.isHashTypeSupported(payloadHashTypeStr)) {
                throw new SignerException("Payload hash type not supported: " + payloadHashTypeStr);
            }
            payloadHashType = getHashType(payloadHashTypeStr);
        }
        signerSettings.setSignerSetting(GenericSignerSettings.KEY_PAYLOAD_HASH_TYPE, payloadHashType);
    }

    private void setSoftwareVersion(ICommandLineProxy commandLine, ISignerSettings signerSettings)
        throws NumberFormatException, ParseException, SignerSettingsException {
        long softwareVersion = 0;
        if (commandLine.hasOption(GenericCommandLineOptions.GenericOption.SW_VERSION.getShortCMD())) {
            softwareVersion =
                Long.decode(commandLine
                    .getOptionValue(GenericCommandLineOptions.GenericOption.SW_VERSION.getShortCMD()));
        }
        signerSettings.setSignerSetting(GenericSignerSettings.KEY_SW_VERSION, softwareVersion);
    }

    private void setKeyHashPath(ICommandLineProxy commandLine, ISignerSettings signerSettings) throws ParseException,
        SignerException {
        String keyHashPath = null;
        if (commandLine.hasOption(GenericCommandLineOptions.GenericOption.KEY_HASH_FILE.getLongCMD())) {
            keyHashPath =
                commandLine.getOptionValue(GenericCommandLineOptions.GenericOption.KEY_HASH_FILE.getLongCMD());
        }
        signerSettings.setSignerSetting(ISignerSettings.KEY_HASH_FILE, keyHashPath);
    }

    private void setU5500BufferSize(ICommandLineProxy commandLine, ISignerSettings signerSettings)
        throws ParseException, SignerSettingsException {
        if (commandLine.hasOption(GenericCommandLineOptions.GenericOption.BUFFER_SIZE.getLongCMD())) {
            int bufferSize = 8 * 1024;
            String bufferSizeStr =
                commandLine.getOptionValue(GenericCommandLineOptions.GenericOption.BUFFER_SIZE.getLongCMD());
            if (bufferSizeStr != null) {
                if (bufferSizeStr.toLowerCase(Locale.getDefault()).endsWith("k")) {
                    bufferSizeStr = bufferSizeStr.substring(0, bufferSizeStr.length() - 1);
                    bufferSize = Integer.decode(bufferSizeStr) * 1024;
                } else if (bufferSizeStr.toLowerCase(Locale.getDefault()).endsWith("m")) {
                    bufferSizeStr = bufferSizeStr.substring(0, bufferSizeStr.length() - 1);
                    bufferSize = Integer.decode(bufferSizeStr) * 1024 * 1024;
                } else {
                    bufferSize = Integer.decode(bufferSizeStr);
                }
            }
            signerSettings.setSignerSetting(GenericSignerSettings.KEY_BUFFER_SIZE, bufferSize);
        }
    }

    private HashType getHashType(String hashTypeStr) {
        HashType type = HashType.SHA1_HASH;
        if ("SHA-1".equals(hashTypeStr)) {
            type = HashType.SHA1_HASH;
        } else if ("SHA-256".equals(hashTypeStr)) {
            type = HashType.SHA256_HASH;
        } else if ("SHA-384".equals(hashTypeStr)) {
            type = HashType.SHA384_HASH;
        } else if ("SHA-512".equals(hashTypeStr)) {
            type = HashType.SHA512_HASH;
        }
        return type;
    }

    private void setGenericSoftwareType(ISignerSettings signerSettings, String softwareType)
        throws SignerSettingsException {

        GenericSoftwareType swType = null;

        for (GenericSoftwareType type : GenericSoftwareType.values()) {
            if (type.name().equalsIgnoreCase(softwareType)) {
                swType = type;
                break;
            }
        }

        signerSettings.setSignerSetting(GenericSignerSettings.KEY_SW_TYPE, swType);
    }

    private void setBuildVersionsAndFlags(ICommandLineProxy commandLine, ISignerSettings signerSettings)
        throws NumberFormatException, NullPointerException, ParseException, SignerSettingsException {
        short majorBuildVersion = 0;
        if (commandLine.hasOption(GenericCommandLineOptions.GenericOption.MAJOR_BUILD_VERSION.getLongCMD())) {
            majorBuildVersion =
                Short.decode(
                    commandLine
                        .getOptionValue(GenericCommandLineOptions.GenericOption.MAJOR_BUILD_VERSION.getLongCMD()))
                    .shortValue();
        }
        signerSettings.setSignerSetting(GenericSignerSettings.KEY_MAJOR_BUILD_VERSION, majorBuildVersion);

        short minorBuildVersion = 0;
        if (commandLine.hasOption(GenericCommandLineOptions.GenericOption.MINOR_BUILD_VERSION.getLongCMD())) {
            minorBuildVersion =
                Short.decode(
                    commandLine
                        .getOptionValue(GenericCommandLineOptions.GenericOption.MINOR_BUILD_VERSION.getLongCMD()))
                    .shortValue();
        }
        signerSettings.setSignerSetting(GenericSignerSettings.KEY_MINOR_BUILD_VERSION, minorBuildVersion);

        int flags = 0;
        if (commandLine.hasOption(GenericCommandLineOptions.GenericOption.FLAGS.getLongCMD())) {
            flags =
                Long.decode(commandLine.getOptionValue(GenericCommandLineOptions.GenericOption.FLAGS.getLongCMD()))
                    .intValue();
        }
        signerSettings.setSignerSetting(GenericSignerSettings.KEY_FLAGS, flags);
    }

    /**
     * Setup all settings needed for A2 signing
     * 
     * @param commandLine
     * @param signerSettings
     * @throws SignerException
     * @throws NullPointerException
     * @throws ParseException
     */
    private void setupA2Settings(ICommandLineProxy commandLine, ISignerSettings signerSettings) throws SignerException,
        NullPointerException, ParseException {

        if (!commandLine.hasOption(A2CommandLineOptions.Option.DEBUG.getLongCMD())) {
            setA2Debug(false, signerSettings);
        } else {
            setA2Debug(true, signerSettings);
        }

        if (!commandLine.hasOption(A2CommandLineOptions.Option.TARGET.getShortCMD())) {
            throw new SignerException("You must specify a target CPU");
        }
        String targetCPU = commandLine.getOptionValue(A2CommandLineOptions.Option.TARGET.getShortCMD());
        if (!A2CommandLineOptions.isTargetCPUSupported(targetCPU)) {
            throw new SignerException("Target CPU '" + targetCPU + "' is not supported. Supported values: "
                + A2CommandLineOptions.getSupportedTargetCPU());
        } else {
            setA2TargetCPU(targetCPU, signerSettings);
        }

        if (!commandLine.hasOption(A2CommandLineOptions.Option.SW_TYPE.getShortCMD())) {
            throw new SignerException("You must specify a software type");
        }
        String softwareType = commandLine.getOptionValue(A2CommandLineOptions.Option.SW_TYPE.getShortCMD());
        if (!A2CommandLineOptions.isSWTypeSupported(softwareType)) {
            throw new SignerException("Software type '" + softwareType + "' is not supported. Supported values: "
                + A2CommandLineOptions.getSupportedSoftwareTypes());
        } else {
            setA2SWType(softwareType, signerSettings);
        }

        if (!commandLine.hasOption(A2CommandLineOptions.Option.DISABLE_ETX_REAL.getLongCMD())) {
            setA2ExtReal(true, signerSettings);
        } else {
            setA2ExtReal(false, signerSettings);
        }

        if (!commandLine.hasOption(A2CommandLineOptions.Option.DISABLE_ETX_HEADER.getLongCMD())) {
            setA2EtxHeader(true, signerSettings);
        } else {
            setA2EtxHeader(false, signerSettings);
        }

        if (!commandLine.hasOption(A2CommandLineOptions.Option.INTERACTIVE.getLongCMD())) {
            throw new SignerException("You must specify a interactive load bit");
        }
        String interactiveLoadBit = commandLine.getOptionValue(A2CommandLineOptions.Option.INTERACTIVE.getLongCMD());
        if (!A2CommandLineOptions.isInteractiveLoadBitSupported(interactiveLoadBit)) {
            throw new SignerException("Interactive Load Bit '" + interactiveLoadBit
                + "' is not supported. Supported values: " + A2CommandLineOptions.getSupportedInteractiveLoadBits());
        } else {
            setA2InteractiveLoadBit(interactiveLoadBit, signerSettings);
        }

        if (!commandLine.hasOption(A2CommandLineOptions.Option.APP_SEC.getLongCMD())) {
            throw new SignerException("You must specify a application security");
        }
        String applicationSecurity = commandLine.getOptionValue(A2CommandLineOptions.Option.APP_SEC.getLongCMD());
        if (!A2CommandLineOptions.isApplicationSecuritySupported(applicationSecurity)) {
            throw new SignerException("Application security type '" + applicationSecurity
                + "' is not supported. Supported values: " + A2CommandLineOptions.getSupportedApplicationSecurity());
        } else {
            setA2ApplicationSecurity(applicationSecurity, signerSettings);
        }

        // setupSignerSettings must continue in method setupSignerSettingsContinue, because check
        // style cyclomatic complexity
        setupA2SettingsContinue(commandLine, signerSettings);

    }

    private void setupA2SettingsContinue(ICommandLineProxy commandLine, ISignerSettings signerSettings)
        throws SignerException, NullPointerException, ParseException {

        if (!commandLine.hasOption(A2CommandLineOptions.Option.ADDR_FORMAT.getLongCMD())) {
            throw new SignerException("You must specify a address format");
        }
        String addressFormat = commandLine.getOptionValue(A2CommandLineOptions.Option.ADDR_FORMAT.getLongCMD());
        if (!A2CommandLineOptions.isAddressFormatSupported(addressFormat)) {
            throw new SignerException("Address format '" + addressFormat + "' is not supported. Supported values: "
                + A2CommandLineOptions.getSupportedAddressFormat());
        }
        setA2AddressFormat(addressFormat, signerSettings);

        if (!commandLine.hasOption(A2CommandLineOptions.Option.HDR_SEC.getLongCMD())) {
            throw new SignerException("You must specify a header security");
        }
        String headerSecurity = commandLine.getOptionValue(A2CommandLineOptions.Option.HDR_SEC.getLongCMD());
        setA2HeaderSecurity(headerSecurity, signerSettings);

        signerSettings.setSignerSetting(ICOPSSignerSettings.KEY_ACL_PTYPE,
            A2AccessControlList.PAYLOAD_TYPE_PHYSICAL_ADDRESS);

        if (commandLine.hasOption(A2CommandLineOptions.Option.PTYPE.getLongCMD())) {
            String payloadType = commandLine.getOptionValue(A2CommandLineOptions.Option.PTYPE.getLongCMD());
            if (!A2CommandLineOptions.isPayloadTypeSupported(payloadType)) {
                throw new SignerException("Payload type '" + payloadType + "' is not supported. Supported values: "
                    + A2CommandLineOptions.getSupportedPayloadTypes());
            }
            setA2PayloadType(payloadType, signerSettings);
        }

        if (!commandLine.hasOption(A2CommandLineOptions.Option.SHORT_MAC.getLongCMD())) {
            setA2ShortMac(false, signerSettings);
        } else {
            setA2ShortMac(true, signerSettings);
        }

        if (!commandLine.hasOption(A2CommandLineOptions.Option.MAC_MODE.getShortCMD())) {
            throw new SignerException("You must specify a mac mode");
        }
        String macMode = commandLine.getOptionValue(A2CommandLineOptions.Option.MAC_MODE.getShortCMD());
        if (!A2CommandLineOptions.isMacModeSupported(macMode)) {
            throw new SignerException("Mac mode '" + macMode + "' is not supported. Supported values: "
                + A2CommandLineOptions.getSupportedMacMode());
        }
        setA2MacMode(macMode, signerSettings);

        if (!commandLine.hasOption(A2CommandLineOptions.Option.ANTI_ROLLBACK.getShortCMD())) {
            setA2AntiRollBack(false, signerSettings);
        } else {
            setA2AntiRollBack(true, signerSettings);
        }

        if (!commandLine.hasOption(A2CommandLineOptions.Option.SW_VERSION.getLongCMD())) {
            throw new SignerException("You must specify a software version");
        }
        String swVersion = commandLine.getOptionValue(A2CommandLineOptions.Option.SW_VERSION.getLongCMD());
        setA2SwVersion(swVersion, signerSettings);

        // setupSignerSettingsContinue must continue in method setupSignerSettingsContinue2, because
        // check
        // style cyclomatic complexity
        setupA2SettingsContinue2(commandLine, signerSettings);
    }

    private void setupA2SettingsContinue2(ICommandLineProxy commandLine, ISignerSettings signerSettings)
        throws ParseException, SignerException {
        if (!commandLine.hasOption(A2CommandLineOptions.Option.COMPRESS.getShortCMD())) {
            setA2LoaderCompression(false, signerSettings);
        } else {
            setA2LoaderCompression(true, signerSettings);
        }

        if (!commandLine.hasOption(A2CommandLineOptions.Option.ENCRYPT.getShortCMD())) {
            setA2LoaderEncryption(false, signerSettings);
        } else {
            setA2LoaderEncryption(true, signerSettings);
        }

    }

    private void setA2SwVersion(String swVersion, ISignerSettings signerSettings) throws SignerException {
        try {
            int sVersion = Integer.parseInt(swVersion, 10);
            if ((sVersion < 0) || (sVersion > A2SoftwareVersion.MAX_SW_VERSION)) {
                throw new SignerException("Software version '" + swVersion + "' is not supported. Supported values: "
                    + A2CommandLineOptions.getSupportedSoftwareVersion());
            }
            signerSettings.setSignerSetting(ICOPSSignerSettings.KEY_SW_VERSION, sVersion);
        } catch (NumberFormatException e) {
            throw new SignerException("Software version '" + swVersion + "' is not supported. Supported values: "
                + A2CommandLineOptions.getSupportedSoftwareVersion());
        }
    }

    private void setA2AntiRollBack(boolean antiRollBack, ISignerSettings signerSettings) throws SignerException {
        if (antiRollBack) {
            signerSettings.setSignerSetting(ICOPSSignerSettings.KEY_SW_VERSION_ANTI_ROLL_REQUIRED,
                A2SoftwareVersion.ANTI_ROLLBACK_REQUIRED);
        } else {
            signerSettings.setSignerSetting(ICOPSSignerSettings.KEY_SW_VERSION_ANTI_ROLL_REQUIRED,
                A2SoftwareVersion.ANTI_ROLLBACK_NOT_REQUIRED);
        }
    }

    private void setA2MacMode(String macMode, ISignerSettings signerSettings) throws SignerException {
        if (macMode.equalsIgnoreCase(A2CommandLineOptions.MAC_MODE_CONFIG)) {
            signerSettings.setSignerSetting(ICOPSSignerSettings.KEY_MAC_MODE, IA2MacModeConstants.MAC_CONFIG);
        } else if (macMode.equalsIgnoreCase(A2CommandLineOptions.MAC_MODE_2NDSBC)) {
            signerSettings.setSignerSetting(ICOPSSignerSettings.KEY_MAC_MODE, IA2MacModeConstants.MAC_2SBC);
        } else if (macMode.equalsIgnoreCase(A2CommandLineOptions.MAC_MODE_3RDSBC)) {
            signerSettings.setSignerSetting(ICOPSSignerSettings.KEY_MAC_MODE, IA2MacModeConstants.MAC_3SBC);
        } else if (macMode.equalsIgnoreCase(A2CommandLineOptions.MAC_MODE_4THSBC)) {
            signerSettings.setSignerSetting(ICOPSSignerSettings.KEY_MAC_MODE, IA2MacModeConstants.MAC_4SBC);
        } else if (macMode.equalsIgnoreCase(A2CommandLineOptions.MAC_MODE_SW)) {
            signerSettings.setSignerSetting(ICOPSSignerSettings.KEY_MAC_MODE, IA2MacModeConstants.MAC_SW);
        } else if (macMode.equalsIgnoreCase(A2CommandLineOptions.MAC_MODE_LICENCE)) {
            signerSettings.setSignerSetting(ICOPSSignerSettings.KEY_MAC_MODE, IA2MacModeConstants.MAC_LICENSE);
        } else if (macMode.equalsIgnoreCase(A2CommandLineOptions.MAC_MODE_MAC7)) {
            signerSettings.setSignerSetting(ICOPSSignerSettings.KEY_MAC_MODE, IA2MacModeConstants.MAC_7);
        } else if (macMode.equalsIgnoreCase(A2CommandLineOptions.MAC_MODE_MAC8)) {
            signerSettings.setSignerSetting(ICOPSSignerSettings.KEY_MAC_MODE, IA2MacModeConstants.MAC_8);
        }

        setA2SwVersionMacMode(macMode, signerSettings);
    }

    private void setA2SwVersionMacMode(String macMode, ISignerSettings signerSettings) throws SignerException {
        if (macMode.equalsIgnoreCase(A2CommandLineOptions.MAC_MODE_2NDSBC)) {
            signerSettings.setSignerSetting(ICOPSSignerSettings.KEY_SW_VERSION_MAC_MODE,
                A2SoftwareVersion.ANTI_ROLLBACK_MODE_2SBC);
        } else if (macMode.equalsIgnoreCase(A2CommandLineOptions.MAC_MODE_3RDSBC)) {
            signerSettings.setSignerSetting(ICOPSSignerSettings.KEY_SW_VERSION_MAC_MODE,
                A2SoftwareVersion.ANTI_ROLLBACK_MODE_3SBC);
        } else if (macMode.equalsIgnoreCase(A2CommandLineOptions.MAC_MODE_4THSBC)) {
            signerSettings.setSignerSetting(ICOPSSignerSettings.KEY_SW_VERSION_MAC_MODE,
                A2SoftwareVersion.ANTI_ROLLBACK_MODE_4SBC);
        } else if (macMode.equalsIgnoreCase(A2CommandLineOptions.MAC_MODE_SW)) {
            signerSettings.setSignerSetting(ICOPSSignerSettings.KEY_SW_VERSION_MAC_MODE,
                A2SoftwareVersion.ANTI_ROLLBACK_MODE_SW);
        } else {
            signerSettings.setSignerSetting(ICOPSSignerSettings.KEY_SW_VERSION_MAC_MODE, 0);
        }
    }

    private void setA2ShortMac(boolean shortMac, ISignerSettings signerSettings) throws SignerException {
        if (shortMac) {
            signerSettings.setSignerSetting(ICOPSSignerSettings.KEY_ACL_SHORT_MAC,
                A2AccessControlList.SHORT_MAC_HEADER_ENABLE);
        } else {
            signerSettings.setSignerSetting(ICOPSSignerSettings.KEY_ACL_SHORT_MAC,
                A2AccessControlList.SHORT_MAC_HEADER_DISABLE);
        }
    }

    private void setA2PayloadType(String payloadType, ISignerSettings signerSettings) throws SignerException {

        if (payloadType.equalsIgnoreCase(A2CommandLineOptions.PTYPE_NAND)) {
            signerSettings.setSignerSetting(ICOPSSignerSettings.KEY_ACL_PTYPE, A2AccessControlList.PAYLOAD_TYPE_NAND);
        } else if (payloadType.equalsIgnoreCase(A2CommandLineOptions.PTYPE_NOR)) {
            signerSettings.setSignerSetting(ICOPSSignerSettings.KEY_ACL_PTYPE, A2AccessControlList.PAYLOAD_TYPE_NOR);
        }
    }

    private void setA2HeaderSecurity(String headerSecurity, ISignerSettings signerSettings) throws SignerException {
        if (headerSecurity.equalsIgnoreCase(A2CommandLineOptions.HDR_SEC_VERIFY_ONLY)) {
            signerSettings.setSignerSetting(ICOPSSignerSettings.KEY_ACL_HDR_SECURITY,
                A2AccessControlList.HEADER_ACTION_ONLY_VERIFY);
            signerSettings.setSignerSetting(ICOPSSignerSettings.KEY_HEADER_DESTINATION_ADDRESS,
                IA2HeaderDestinationAddressConstants.HEADER_DEST_ADDRESS);
        } else {
            int pos = headerSecurity.lastIndexOf('=');
            if (pos == -1) {
                throw new SignerException("Header security '" + headerSecurity
                    + "' is not supported. Supported values: " + A2CommandLineOptions.getSupportedHeaderSecurity());
            }

            int headerSecurityValue;
            String[] values = headerSecurity.split("=");

            if (HexUtilities.hasHexPrefix(values[1])) {
                if (HexUtilities.isHexDigit(values[1])) {
                    headerSecurityValue = Integer.parseInt(HexUtilities.removeHexPrefix(values[1]), 16);
                } else {
                    throw new SignerException("Invalid value for header security");
                }
            } else {
                throw new SignerException("Invalid value for header security");
            }

            if (values[0].equalsIgnoreCase(A2CommandLineOptions.HDR_SEC_AS_IS)) {
                signerSettings.setSignerSetting(ICOPSSignerSettings.KEY_ACL_HDR_SECURITY,
                    A2AccessControlList.HEADER_ACTION_STORE);
                signerSettings
                    .setSignerSetting(ICOPSSignerSettings.KEY_HEADER_DESTINATION_ADDRESS, headerSecurityValue);
            } else if (values[0].equalsIgnoreCase(A2CommandLineOptions.HDR_SEC_REPLACE_SIGN)) {
                signerSettings.setSignerSetting(ICOPSSignerSettings.KEY_ACL_HDR_SECURITY,
                    A2AccessControlList.HEADER_ACTION_STORE_REPLACE_SIGNATURE);
                signerSettings
                    .setSignerSetting(ICOPSSignerSettings.KEY_HEADER_DESTINATION_ADDRESS, headerSecurityValue);
            } else {
                throw new SignerException("Header security '" + headerSecurity
                    + "' is not supported. Supported values: " + A2CommandLineOptions.getSupportedHeaderSecurity());
            }
        }
    }

    private void setA2AddressFormat(String addressFormat, ISignerSettings signerSettings)
        throws SignerSettingsException {
        if (addressFormat.equalsIgnoreCase(A2CommandLineOptions.ADDR_FORMAT_PAGES)) {
            signerSettings.setSignerSetting(ICOPSSignerSettings.KEY_ACL_FORMAT,
                A2AccessControlList.ADDRESS_FORMAT_PAGES_ONLY);
        } else if (addressFormat.equalsIgnoreCase(A2CommandLineOptions.ADDR_FORMAT_4B)) {
            signerSettings.setSignerSetting(ICOPSSignerSettings.KEY_ACL_FORMAT,
                A2AccessControlList.ADDRESS_FORMAT_PAGES_AND_4_BYTES);
        }
    }

    private void setA2TargetCPU(String targetCPU, ISignerSettings signerSettings) throws SignerSettingsException {
        if (targetCPU.equalsIgnoreCase(A2CommandLineOptions.TARGET_ACC)) {
            signerSettings.setSignerSetting(ICOPSSignerSettings.KEY_ACL_TARGET_CPU,
                A2AccessControlList.TARGET_CPU_ACCESS);
        } else {
            signerSettings.setSignerSetting(ICOPSSignerSettings.KEY_ACL_TARGET_CPU,
                A2AccessControlList.TARGET_CPU_APPLICATION);
        }
    }

    private void setA2Debug(boolean debug, ISignerSettings signerSettings) throws SignerSettingsException {
        if (debug) {
            signerSettings.setSignerSetting(ICOPSSignerSettings.KEY_ACL_DEBUG,
                A2AccessControlList.DEBUG_ON_ACCESS_APPLICATION_SIDE_ENABLE);
        } else {
            signerSettings.setSignerSetting(ICOPSSignerSettings.KEY_ACL_DEBUG,
                A2AccessControlList.DEBUG_ON_ACCESS_APPLICATION_SIDE_DISABLE);
        }
    }

    private void setA2SWType(String softwareType, ISignerSettings signerSettings) throws SignerSettingsException {
        if (softwareType.equalsIgnoreCase(A2CommandLineOptions.SW_TYPE_LOADER)) {
            signerSettings.setSignerSetting(ICOPSSignerSettings.KEY_ACL_SW_TYPE, A2AccessControlList.SW_TYPE_LOADER);
            signerSettings.setSignerSetting(ICOPSSignerSettings.KEY_SW_TYPE, IA2PayloadTypeConstants.TYPE_LOADER);
            signerSettings.setSignerSetting(ISignerSettings.KEY_INPUT_FILE_TYPE, A2Signer.INPUT_FILE_TYPE_BIN);
        } else {
            signerSettings.setSignerSetting(ICOPSSignerSettings.KEY_ACL_SW_TYPE,
                A2AccessControlList.SW_TYPE_BOOT_CODE_OR_SW_IMAGE);

            if (softwareType.equalsIgnoreCase(A2CommandLineOptions.SW_TYPE_IMAGE)) {
                signerSettings.setSignerSetting(ICOPSSignerSettings.KEY_SW_TYPE, IA2PayloadTypeConstants.TYPE_SW_IMAGE);
            } else if (softwareType.equalsIgnoreCase(A2CommandLineOptions.SW_TYPE_ELF)) {
                signerSettings.setSignerSetting(ICOPSSignerSettings.KEY_SW_TYPE, IA2PayloadTypeConstants.TYPE_ELF);
            } else if (softwareType.equalsIgnoreCase(A2CommandLineOptions.SW_TYPE_GENERIC)) {
                signerSettings.setSignerSetting(ICOPSSignerSettings.KEY_SW_TYPE, IA2PayloadTypeConstants.TYPE_GENERIC);
            } else if (softwareType.equalsIgnoreCase(A2CommandLineOptions.SW_TYPE_ARCHIVE)) {
                signerSettings.setSignerSetting(ICOPSSignerSettings.KEY_SW_TYPE, IA2PayloadTypeConstants.TYPE_ARCHIVE);
            } else if (softwareType.equals(A2CommandLineOptions.SW_TYPE_TAR)) {
                signerSettings.setSignerSetting(ICOPSSignerSettings.KEY_SW_TYPE, IA2PayloadTypeConstants.TYPE_TAR);
            }
        }
    }

    private void setA2ExtReal(boolean extReal, ISignerSettings signerSettings) throws SignerSettingsException {
        if (extReal) {
            signerSettings
                .setSignerSetting(ICOPSSignerSettings.KEY_ACL_EXT_LEVEL, A2AccessControlList.ETX_LEVEL_ENABLE);
        } else {
            signerSettings.setSignerSetting(ICOPSSignerSettings.KEY_ACL_EXT_LEVEL,
                A2AccessControlList.ETX_LEVEL_DISABLE);
        }
    }

    private void setA2EtxHeader(boolean extHeader, ISignerSettings signerSettings) throws SignerSettingsException {
        if (extHeader) {
            signerSettings.setSignerSetting(ICOPSSignerSettings.KEY_ACL_ETX_LEVEL_IN_HEADER,
                A2AccessControlList.ETX_LEVEL_IN_HEADER_ENABLE);
        } else {
            signerSettings.setSignerSetting(ICOPSSignerSettings.KEY_ACL_ETX_LEVEL_IN_HEADER,
                A2AccessControlList.ETX_LEVEL_IN_HEADER_DISABLE);
        }
    }

    private void setA2InteractiveLoadBit(String interactiveLoadBit, ISignerSettings signerSettings)
        throws SignerSettingsException {
        if (interactiveLoadBit.equalsIgnoreCase(A2CommandLineOptions.INTERACTIVE_STATIC)) {
            signerSettings.setSignerSetting(ICOPSSignerSettings.KEY_ACL_INTERACTIVE_LOAD_BIT,
                A2AccessControlList.INTERACTIVE_LOAD_BIT_LOAD_IMAGE_STATIC);
        } else {
            signerSettings.setSignerSetting(ICOPSSignerSettings.KEY_ACL_INTERACTIVE_LOAD_BIT,
                A2AccessControlList.INTERACTIVE_LOAD_BIT_MODIFIED_INTERACTIVELY);
        }
    }

    private void setA2ApplicationSecurity(String applicationSecurity, ISignerSettings signerSettings)
        throws SignerSettingsException {
        if (applicationSecurity.equalsIgnoreCase(A2CommandLineOptions.APP_SEC_ON)) {
            signerSettings.setSignerSetting(ICOPSSignerSettings.KEY_ACL_APP_SEC,
                A2AccessControlList.ASF_APPLICATION_SECURITY_ALWAYS_ON);
        } else if (applicationSecurity.equalsIgnoreCase(A2CommandLineOptions.APP_SEC_OFF)) {
            signerSettings.setSignerSetting(ICOPSSignerSettings.KEY_ACL_APP_SEC,
                A2AccessControlList.ASF_APPLICATION_SECURITY_OFF);
        } else if (applicationSecurity.equalsIgnoreCase(A2CommandLineOptions.APP_SEC_DATA_LOADING)) {
            signerSettings.setSignerSetting(ICOPSSignerSettings.KEY_ACL_APP_SEC,
                A2AccessControlList.ASF_APPLICATION_SECURITY_ONLY_APPLIES_TO_DATA_LOADING);
        } else if (applicationSecurity.equalsIgnoreCase(A2CommandLineOptions.APP_SEC_PAF_DEPENDENT)) {
            signerSettings.setSignerSetting(ICOPSSignerSettings.KEY_ACL_APP_SEC,
                A2AccessControlList.ASF_APPLICATION_SECURITY_PAF_DEPENDENT);
        }
    }

    private void setA2LoaderCompression(boolean loaderCompression, ISignerSettings signerSettings)
        throws SignerException {
        if (loaderCompression) {
            signerSettings.setSignerSetting(ICOPSSignerSettings.KEY_LOADER_COMPRESSION, true);
        } else {
            signerSettings.setSignerSetting(ICOPSSignerSettings.KEY_LOADER_COMPRESSION, false);
        }
    }

    private void setA2LoaderEncryption(boolean loaderEncryption, ISignerSettings signerSettings) throws SignerException {
        if (loaderEncryption) {
            signerSettings.setSignerSetting(ICOPSSignerSettings.KEY_LOADER_ENCRYPTION, true);
        } else {
            signerSettings.setSignerSetting(ICOPSSignerSettings.KEY_LOADER_ENCRYPTION, false);
        }
    }

    /**
     * @param signerSettings signerSettings
     * @param commandLine commandLine
     * @throws SignerSettingsException SignerSettingsException
     * @throws ParseException ParseException
     */
    public void overrideEnvVariableSettings(ISignerSettings signerSettings,ICommandLineProxy commandLine) 
        throws SignerSettingsException, ParseException{
        if (commandLine.hasOption(GenericCommandLineOptions.GenericOption.SIGN_SERVERS.getShortCMD())){
             signerSettings.setSignerSetting(GenericSignerSettings.KEY_SIGN_SERVICE_SERVERS, 
                    commandLine.getOptionValue(GenericCommandLineOptions.GenericOption.SIGN_SERVERS.getShortCMD()));
        }
        if (commandLine.hasOption(GenericCommandLineOptions.GenericOption.SIGN_PACKAGE_ROOT.getShortCMD())){
            signerSettings.setSignerSetting(GenericSignerSettings.KEY_SIGN_PACKAGE_ROOT, 
                   commandLine.getOptionValue(GenericCommandLineOptions.GenericOption.SIGN_PACKAGE_ROOT.getShortCMD()));
       }
        if (commandLine.hasOption(GenericCommandLineOptions.GenericOption.LOCAL_SIGN_PACKAGE_ROOT.getShortCMD())){
            signerSettings.setSignerSetting(GenericSignerSettings.KEY_LOCAL_SIGN_PACKAGE_ROOT, 
                   commandLine.getOptionValue(GenericCommandLineOptions.GenericOption.LOCAL_SIGN_PACKAGE_ROOT.getShortCMD()));
       }
        if (commandLine.hasOption(GenericCommandLineOptions.GenericOption.LOCAL_KEY_ROOT.getShortCMD())){
            signerSettings.setSignerSetting(GenericSignerSettings.KEY_LOCAL_KEY_ROOT, 
                   commandLine.getOptionValue(GenericCommandLineOptions.GenericOption.LOCAL_KEY_ROOT.getShortCMD()));
       }
        if (commandLine.hasOption(GenericCommandLineOptions.GenericOption.LOCAL_ENCRYPTION_KEY_ROOT.getShortCMD())){
            signerSettings.setSignerSetting(GenericSignerSettings.KEY_LOCAL_ENCRYPTION_KEY_ROOT, 
                   commandLine.getOptionValue(GenericCommandLineOptions.GenericOption.LOCAL_ENCRYPTION_KEY_ROOT.getShortCMD()));
       }
    }

    private void setupListPackagesSettings(ICommandLineProxy commandLine, ISignerSettings signerSettings)
            throws ParseException, SignerException {

        overrideEnvVariableSettings(signerSettings, commandLine);
        }
}
