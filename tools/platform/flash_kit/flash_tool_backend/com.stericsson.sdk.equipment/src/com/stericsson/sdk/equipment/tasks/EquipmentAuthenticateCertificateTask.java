package com.stericsson.sdk.equipment.tasks;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;

import org.osgi.framework.ServiceReference;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.common.configuration.ConfigurationOption;
import com.stericsson.sdk.common.configuration.IConfigurationRecord;
import com.stericsson.sdk.common.configuration.IConfigurationService;
import com.stericsson.sdk.equipment.AbstractEquipmentTask;
import com.stericsson.sdk.equipment.AbstractLoaderCommunicationEquipment;
import com.stericsson.sdk.equipment.Activator;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.equipment.EquipmentType;
import com.stericsson.sdk.loader.communication.IAuthenticate;
import com.stericsson.sdk.loader.communication.LCDefinitions;
import com.stericsson.sdk.signing.ISignerSettings;
import com.stericsson.sdk.signing.SignerException;
import com.stericsson.sdk.signing.SignerFacade;
import com.stericsson.sdk.signing.generic.GenericSignerSettings;
import com.stericsson.sdk.signing.generic.GenericSoftwareType;
import com.stericsson.sdk.signing.u5500.U5500SignerSettings;
import com.stericsson.sdk.signing.u7x00.M7X00SignerSettings;
import com.stericsson.sdk.signing.u8500.U8500SignerSettings;

/**
 * @author xtomlju
 */
public class EquipmentAuthenticateCertificateTask extends AbstractEquipmentTask implements IAuthenticate {

    private int resultCode;

    private String resultMessage;

    private AbstractLoaderCommunicationEquipment mobileEquipment;

    private Object signPackageAlias;

    private int authenticationType;

    private static final String ERROR_PREFIX = "Failed to authenticate. Possible reason: ";

    /**
     * @param equipment
     *            TBD
     */
    public EquipmentAuthenticateCertificateTask(AbstractLoaderCommunicationEquipment equipment) {
        super(equipment);
        mobileEquipment = equipment;
        resultCode = ERROR_NONE;
        resultMessage = mobileEquipment.getLoaderCommunicationService().getLoaderErrorDesc(resultCode);
    }

    /**
     * {@inheritDoc}
     */
    public void cancel() {
        // TODO Auto-generated method stub

    }

    /**
     * {@inheritDoc}
     */
    public EquipmentTaskResult execute() {
        notifyTaskStart();
        if (!(resultCode == ERROR)) {
            resultCode = 0;
            int result =
                (Integer) mobileEquipment.sendCommand(LCDefinitions.METHOD_NAME_SYSTEM_AUTHENTICATE, new Object[] {
                    authenticationType, this}, this);
            if (resultCode == ERROR_NONE && result != 0) {
                resultCode = result;
                resultMessage =
                    ERROR_PREFIX + mobileEquipment.getLoaderCommunicationService().getLoaderErrorDesc(resultCode);
            }
        }
        return new EquipmentTaskResult(resultCode, resultMessage, null, false);
    }

    /**
     * {@inheritDoc}
     */
    public String getId() {
        return CommandName.SYSTEM_AUTHENTICATE_CERTIFICATE.name();
    }

    /**
     * {@inheritDoc}
     */
    public boolean isCancelable() {
        return false;
    }

    /**
     * {@inheritDoc}
     */
    public void setArguments(String[] arguments) {
        signPackageAlias = arguments[2];

        if (arguments[3].equals("null") || arguments[3].equalsIgnoreCase("false")) {
            setAuthenticationType(false);
        } else if (arguments[3].equalsIgnoreCase("true")) {
            setAuthenticationType(true);
        } else {
            resultMessage = "Parameter  \"permanent authentication\"  flag should be set only to true or false";
            resultCode = ERROR;
        }
    }

    private void setAuthenticationType(Boolean permanentAuthenticationFlag) {
        if (permanentAuthenticationFlag) {
            authenticationType = LCDefinitions.AUTHENTICATE_CERTIFICATE_PERSISTENT;
        } else {
            authenticationType = LCDefinitions.AUTHENTICATE_CERTIFICATE;
        }
    }

    /**
     * {@inheritDoc}
     */
    public byte[] getChallengeResponse(byte[] challenge) {
        byte[] result = null;
        File challengeFile = null;
        File signedChallengeFile = null;

        FileOutputStream challengeFileOutput = null;
        FileInputStream signedChallengeFileInput = null;
        try {
            challengeFile = File.createTempFile("challenge" + System.currentTimeMillis(), ".bin");
            signedChallengeFile = File.createTempFile("signedChallenge" + System.currentTimeMillis(), ".bin");
            signedChallengeFile.deleteOnExit();

            // Write challenge to file
            notifyTaskMessage("Preparing received challenge");

            challengeFileOutput = new FileOutputStream(challengeFile);
            challengeFileOutput.write(challenge);

            ServiceReference serviceReference =
                Activator.getBundleContext().getServiceReference(IConfigurationService.class.getName());
            IConfigurationService service =
                (IConfigurationService) Activator.getBundleContext().getService(serviceReference);
            IConfigurationRecord record = service.getRecord(ConfigurationOption.SIGN_TOOL);
            if (record == null) {
                throw new IOException("Failed to read configuration record: " + ConfigurationOption.SIGN_TOOL);
            }

            String useLocalSigningString = record.getValue(ConfigurationOption.USE_LOCAL_SIGNING);
            if (useLocalSigningString == null
                || useLocalSigningString.trim().equals("")
                || (!useLocalSigningString.trim().equalsIgnoreCase(Boolean.TRUE.toString()) && !useLocalSigningString
                    .trim().equalsIgnoreCase(Boolean.FALSE.toString()))) {
                throw new SignerException("Illegal local signing value specified in configuration record.");
            }

            boolean useLocalSigning = Boolean.parseBoolean(useLocalSigningString.trim());

            ISignerSettings settings = null;
            if (mobileEquipment.getEquipmentType() == EquipmentType.U5500) {
                settings =
                    new U5500SignerSettings(challengeFile.getAbsolutePath(), signedChallengeFile.getAbsolutePath());
            } else if (mobileEquipment.getEquipmentType() == EquipmentType.U8500) {
                settings =
                    new U8500SignerSettings(challengeFile.getAbsolutePath(), signedChallengeFile.getAbsolutePath());
            } else if (mobileEquipment.getEquipmentType() == EquipmentType.M7X00) {
                settings =
                    new M7X00SignerSettings(challengeFile.getAbsolutePath(), signedChallengeFile.getAbsolutePath());
            } else {
                throw new SignerException("Cannot create signer settings. Unsupported equipment type.");
            }

            settings.setSignerSetting(ISignerSettings.KEY_SIGN_PACKAGE_ALIAS, signPackageAlias);
            settings.setSignerSetting(GenericSignerSettings.KEY_SW_TYPE, GenericSoftwareType.AUTH_CHALLENGE);
            settings.setSignerSetting(ISignerSettings.COMMON_FOLDER_KEY_AND_PACKAGES, false);

            SignerFacade facade = new SignerFacade();
            facade.sign(settings, null, useLocalSigning);

            // Read in signed file
            result = new byte[(int) signedChallengeFile.length()];
            signedChallengeFileInput = new FileInputStream(signedChallengeFile);
            int read = signedChallengeFileInput.read(result);
            if (read < 0) {
                throw new IOException("Failed to read signed challenge");
            }

            notifyTaskMessage("Signed challenge created (" + +result.length + " bytes)");

        } catch (IOException e) {
            resultMessage = ERROR_PREFIX + e.getMessage();
            resultCode = -1;
            e.printStackTrace();
        } catch (SignerException e) {
            resultMessage = ERROR_PREFIX + e.getMessage();
            resultCode = -1;
            e.printStackTrace();
        } finally {
            finalizeGetChallengeResponse(challengeFile, signedChallengeFile, challengeFileOutput,
                signedChallengeFileInput);
        }

        // Read in signed file and return the byte array
        return result;
    }

    private void finalizeGetChallengeResponse(File challengeFile, File signedChallengeFile,
        FileOutputStream challengeFileOutput, FileInputStream signedChallengeFileInput) {
        if (challengeFileOutput != null) {
            try {
                challengeFileOutput.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
        if (signedChallengeFileInput != null) {
            try {
                signedChallengeFileInput.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
        if (challengeFile != null) {
            challengeFile.delete();
        }
        if (signedChallengeFile != null) {
            signedChallengeFile.delete();
        }
    }

    /**
     * {@inheritDoc}
     */
    public Object[] getControlKeys() {
        return null; // N/A
    }

    /**
     * {@inheritDoc}
     */
    public String toString() {
        return getId() + "@" + mobileEquipment;
    }
}
