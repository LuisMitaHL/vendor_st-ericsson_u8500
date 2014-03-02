package com.stericsson.sdk.signing.cli;

import org.apache.commons.cli.ParseException;

import com.stericsson.sdk.common.HexUtilities;
import com.stericsson.sdk.signing.ISignerFacade;
import com.stericsson.sdk.signing.ISignerSettings;
import com.stericsson.sdk.signing.SignerException;
import com.stericsson.sdk.signing.SignerFacade;
import com.stericsson.sdk.signing.SignerSettingsException;
import com.stericsson.sdk.signing.a2.A2SignerSettings;
import com.stericsson.sdk.signing.cli.a2.A2CommandLineOptions;
import com.stericsson.sdk.signing.cli.loadmodules.LoadModulesFacade;
import com.stericsson.sdk.signing.cli.loadmodules.LoadModulesSignerSettings;
import com.stericsson.sdk.signing.generic.IGenericSignerSettings;
import com.stericsson.sdk.signing.generic.SignatureType;
import com.stericsson.sdk.signing.l9540.L9540SignerSettings;
import com.stericsson.sdk.signing.u5500.U5500SignerSettings;
import com.stericsson.sdk.signing.u7x00.M7X00SignerSettings;
import com.stericsson.sdk.signing.u8500.U8500SignerSettings;
import com.stericsson.sdk.signing.util.ListPackagesSignerSettings;

/**
 * This factory class is used to create different types of signer based on some properties.
 * 
 * @author xtomlju
 */
public class SignerFactory implements ISignerFactory {

    /**
     * Constructor, will initiated necessery data, i.e. the logger for this class.
     */
    public SignerFactory() {
    }

    /**
     * Create a signer settings object
     * 
     * @param arguments
     *            Any additional arguments needed to the signer settings object
     * @param cmdLineProxy
     *            settings that was set by the commandline.
     * 
     * @return An ISignerSettings interface
     * 
     * @throws SignerException
     *             If a signer setting related error occurred
     * @throws ParseException
     *             parseException
     * @throws NullPointerException
     *             nullPointerException
     */
    public ISignerSettings createSignerSettings(Object[] arguments, ICommandLineProxy cmdLineProxy)
        throws SignerException, NullPointerException, ParseException {

        if (cmdLineProxy.hasOption(A2CommandLineOptions.Option.LIST.getLongCMD())) {
            return createListPackagesSignerSettings(arguments, cmdLineProxy);
        } else if (Sign.SIGN_A2.equalsIgnoreCase(cmdLineProxy.getArgs()[0])) {
            return createA2SignerSettings(arguments, cmdLineProxy);
        }else if (Sign.SIGN_L9540.equalsIgnoreCase(cmdLineProxy.getArgs()[0])) {
            return createL9540SignerSettings(arguments, cmdLineProxy);
        } else if (Sign.SIGN_U8500.equalsIgnoreCase(cmdLineProxy.getArgs()[0])) {
            return createU8500SignerSettings(arguments, cmdLineProxy);
        } else if (Sign.SIGN_U5500.equalsIgnoreCase(cmdLineProxy.getArgs()[0])) {
            return createU5500SignerSettings(arguments, cmdLineProxy);
        } else if (Sign.SIGN_M7X00.equalsIgnoreCase(cmdLineProxy.getArgs()[0])) {
            return createM7X00SignerSettings(arguments, cmdLineProxy);
        } else if (Sign.SIGN_LOAD_MODULES.equalsIgnoreCase(cmdLineProxy.getArgs()[0])) {
            return createLoadModulesSignerSettings(arguments, cmdLineProxy);
        } else {
            return null;
        }
    }

    private ISignerSettings createLoadModulesSignerSettings(Object[] arguments, ICommandLineProxy cmdLineProxy)
        throws SignerException, NullPointerException, ParseException {

        LoadModulesSignerSettings newSettings =
            new LoadModulesSignerSettings((String) arguments[0], (String) arguments[1]);

        new CommandLineSignerSettingsSetup().setupSettings(cmdLineProxy, newSettings);

        return newSettings;
    }

    private ISignerSettings createM7X00SignerSettings(Object[] arguments, ICommandLineProxy cmdLineProxy)
        throws SignerException, ParseException {

        // Get the sign package alias from the command line
        String signPackageAlias = getSignPackageAlias(cmdLineProxy);

        M7X00SignerSettings newSettings = new M7X00SignerSettings((String) arguments[0], (String) arguments[1]);

        newSettings.setSignerSetting(ISignerSettings.KEY_SIGN_PACKAGE_ALIAS, signPackageAlias);

        new CommandLineSignerSettingsSetup().setupSettings(cmdLineProxy, newSettings);

        return newSettings;
    }

    private ISignerSettings createU5500SignerSettings(Object[] arguments, ICommandLineProxy cmdLineProxy)
        throws SignerException, ParseException {

        // Get the sign package alias from the command line
        String signPackageAlias = getSignPackageAlias(cmdLineProxy);

        U5500SignerSettings newSettings = new U5500SignerSettings((String) arguments[0], (String) arguments[1]);

        newSettings.setSignerSetting(ISignerSettings.KEY_SIGN_PACKAGE_ALIAS, signPackageAlias);

        new CommandLineSignerSettingsSetup().setupSettings(cmdLineProxy, newSettings);

        return newSettings;
    }

    private ISignerSettings createU8500SignerSettings(Object[] arguments, ICommandLineProxy cmdLineProxy)
        throws ParseException, SignerException, SignerSettingsException {


        U8500SignerSettings newSettings = new U8500SignerSettings((String) arguments[0], (String) arguments[1]);

        if (cmdLineProxy.hasOption(GenericCommandLineOptions.GenericOption.SIGN_KEY.getShortCMD())) {

            // Get the sign key path from the command line
            String signKeyPath = getSignKeyPath(cmdLineProxy);
            // Store important objects into signer settings
            newSettings.setSignerSetting(IGenericSignerSettings.KEY_SIGN_KEY, signKeyPath);
            newSettings.setSignerSetting(IGenericSignerSettings.KEY_SIGNATURE_TYPE, getSignatureType(cmdLineProxy));
        } else {
            // Get the sign package alias from the command line
            String signPackageAlias = getSignPackageAlias(cmdLineProxy);
            // Store important objects into signer settings
            newSettings.setSignerSetting(ISignerSettings.KEY_SIGN_PACKAGE_ALIAS, signPackageAlias);
        }
        new CommandLineSignerSettingsSetup().setupSettings(cmdLineProxy, newSettings);

        return newSettings;
    }

    private ISignerSettings createL9540SignerSettings(Object[] arguments, ICommandLineProxy cmdLineProxy)
    throws ParseException, SignerException, SignerSettingsException {


    L9540SignerSettings newSettings = new L9540SignerSettings((String) arguments[0], (String) arguments[1]);

    if (cmdLineProxy.hasOption(GenericCommandLineOptions.GenericOption.SIGN_KEY.getShortCMD())) {

        // Get the sign key path from the command line
        String signKeyPath = getSignKeyPath(cmdLineProxy);
        // Store important objects into signer settings
        newSettings.setSignerSetting(IGenericSignerSettings.KEY_SIGN_KEY, signKeyPath);
        newSettings.setSignerSetting(IGenericSignerSettings.KEY_SIGNATURE_TYPE, getSignatureType(cmdLineProxy));
    } else {
        // Get the sign package alias from the command line
        String signPackageAlias = getSignPackageAlias(cmdLineProxy);
        // Store important objects into signer settings
        newSettings.setSignerSetting(ISignerSettings.KEY_SIGN_PACKAGE_ALIAS, signPackageAlias);
    }
    new CommandLineSignerSettingsSetup().setupSettings(cmdLineProxy, newSettings);

    return newSettings;
    }

    private ISignerSettings createA2SignerSettings(Object[] arguments, ICommandLineProxy cmdLineProxy)
        throws ParseException, SignerException, SignerSettingsException {
        // Get the sign package alias from the command line or resource file

        String signPackageAlias = getSignPackageAlias(cmdLineProxy);

        A2SignerSettings newSettings = new A2SignerSettings((String) arguments[0], (String) arguments[1]);

        // Store important objects into signer settings
        newSettings.setSignerSetting(ISignerSettings.KEY_SIGN_PACKAGE_ALIAS, signPackageAlias);

        new CommandLineSignerSettingsSetup().setupSettings(cmdLineProxy, newSettings);

        return newSettings;
    }

    private ISignerSettings createListPackagesSignerSettings(Object[] arguments, ICommandLineProxy cmdLineProxy)
            throws ParseException, SignerException, SignerSettingsException {

            ListPackagesSignerSettings newSettings = new ListPackagesSignerSettings();

            new CommandLineSignerSettingsSetup().setupSettings(cmdLineProxy, newSettings);

            return newSettings;
        }

    /**
     * @param commandLine
     *            will pick the signpackage param from this set.
     * 
     * @return the alias for the signpackage.
     * 
     * @throws ParseException
     *             TBD
     * @throws SignerException
     *             If commandline doesn't specify sign package.
     */
    public String getSignPackageAlias(ICommandLineProxy commandLine) throws ParseException, SignerException {

        String signPackageAlias;

        if (commandLine.hasOption(A2CommandLineOptions.Option.PACKAGE.getShortCMD())) {
            signPackageAlias = commandLine.getOptionValue(A2CommandLineOptions.Option.PACKAGE.getShortCMD());
        } else {
            throw new SignerException("You must specify a sign package");
        }

        return signPackageAlias;
    }

    /**
     * @param commandLine
     *            will pick the signkey param from this set.
     * 
     * @return the path for the signkey.
     * 
     * @throws ParseException
     *             TBD
     * @throws SignerException
     *             If commandline doesn't specify sign key.
     */
    public String getSignKeyPath(ICommandLineProxy commandLine) throws ParseException, SignerException {

        String signKeyPath;

        if (commandLine.hasOption(GenericCommandLineOptions.GenericOption.SIGN_KEY.getShortCMD())) {
            signKeyPath = commandLine.getOptionValue(GenericCommandLineOptions.GenericOption.SIGN_KEY.getShortCMD());
        } else {
            throw new SignerException("You must specify a sign key path");
        }

        return signKeyPath;
    }

    /**
     * @param commandLine
     *            will pick the signaturetype param from this set.
     * 
     * @return the SignatureType.
     * 
     * @throws ParseException
     *             TBD
     */
    private SignatureType getSignatureType(ICommandLineProxy commandLine) throws ParseException, SignerException {

        String signatureTypeString = null;
        int signatureTypeInt = 0;
        SignatureType signatureType = SignatureType.RSASSA_PKCS_V1_5;
        if (commandLine.hasOption(GenericCommandLineOptions.GenericOption.SIGNATURE_TYPE.getShortCMD())) {
            signatureTypeString = commandLine.getOptionValue(
                GenericCommandLineOptions.GenericOption.SIGNATURE_TYPE.getShortCMD());
            signatureTypeInt = HexUtilities.parseHexDigit(signatureTypeString);
            signatureType = SignatureType.getByValue(signatureTypeInt);
        }
        if (signatureType == null) {
            return SignatureType.RSASSA_PKCS_V1_5;
        }
        return signatureType;
    }

    /**
     * {@inheritDoc}
     */
    public ISignerFacade createSignerFacade(String facadeType) {
        if (facadeType.equalsIgnoreCase(Sign.SIGN_LOAD_MODULES)) {
            return new LoadModulesFacade();
        } else {
            return new SignerFacade();
        }
    }
}
