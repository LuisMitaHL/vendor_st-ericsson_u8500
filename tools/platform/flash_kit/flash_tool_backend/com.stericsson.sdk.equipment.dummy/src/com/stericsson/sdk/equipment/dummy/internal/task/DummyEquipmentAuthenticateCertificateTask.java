package com.stericsson.sdk.equipment.dummy.internal.task;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.Locale;

import org.osgi.framework.ServiceReference;

import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.common.configuration.IConfigurationRecord;
import com.stericsson.sdk.common.configuration.IConfigurationService;
import com.stericsson.sdk.equipment.AbstractEquipmentTask;
import com.stericsson.sdk.equipment.EquipmentTaskResult;
import com.stericsson.sdk.equipment.IRuntimeExecutor;
import com.stericsson.sdk.equipment.dummy.Activator;
import com.stericsson.sdk.equipment.dummy.internal.DummyEquipment;
import com.stericsson.sdk.loader.communication.IAuthenticate;
import com.stericsson.sdk.loader.communication.LCDefinitions;
import com.stericsson.sdk.signing.generic.GenericSoftwareType;

/**
 * @author xtomlju
 */
public class DummyEquipmentAuthenticateCertificateTask extends AbstractEquipmentTask implements 
    IAuthenticate {

    private int resultCode;

    private String resultMessage;

    private DummyEquipment mobileEquipment;

    private String signPackageAlias;

    private IRuntimeExecutor runtimeExecutor;

    /**
     * @param equipment
     *            TBD
     */
    public DummyEquipmentAuthenticateCertificateTask(DummyEquipment equipment) {
        super(equipment);
        mobileEquipment = equipment;
        resultCode = ERROR_NONE;
        resultMessage = CommandName.getCLISyntax(getId()) + COMPLETED;
        runtimeExecutor = equipment.getRuntimeExecutor();
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
        int result = (Integer) mobileEquipment.sendCommand(LCDefinitions.METHOD_NAME_SYSTEM_AUTHENTICATE, new Object[] {
            LCDefinitions.AUTHENTICATE_CERTIFICATE, this}, this);
        if (result != 0) {
            resultCode = result;
            resultMessage = "Failed to authenticate";
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
            challengeFile = File.createTempFile("challange" + System.currentTimeMillis(), ".bin");
            signedChallengeFile = File.createTempFile("signedChallange" + System.currentTimeMillis(), ".bin");

            // Write challenge to file
            notifyTaskMessage("Preparing received challange");

            challengeFileOutput = new FileOutputStream(challengeFile);
            challengeFileOutput.write(challenge);

            executeSignTool(challengeFile, signedChallengeFile);

            // Read in signed file
            result = new byte[(int) signedChallengeFile.length()];
            signedChallengeFileInput = new FileInputStream(signedChallengeFile);
            int read = signedChallengeFileInput.read(result);
            if (read != result.length) {
                throw new IOException("Cannot read signed file: " + signedChallengeFile.getAbsolutePath());
            }

            notifyTaskMessage("Sending signed challange");

        } catch (IOException e) {
            e.printStackTrace();
        } catch (InterruptedException e) {
            e.printStackTrace();
        } finally {
            try {
                if (signedChallengeFileInput != null) {
                    signedChallengeFileInput.close();
                }
                if (challengeFileOutput != null) {
                    challengeFileOutput.close();
                }
            } catch (IOException e) {
                e.printStackTrace();
            }
            if (challengeFile != null) {
                challengeFile.delete();
            }
            if (signedChallengeFile != null) {
                signedChallengeFile.delete();
            }
        }

        // Read in signed file and return the byte array

        return result;
    }

    private void executeSignTool(File challengeFile, File signedChallengeFile) throws IOException, InterruptedException {
        ServiceReference serviceReference =
            Activator.getBundleContext().getServiceReference(IConfigurationService.class.getName());
        IConfigurationService service =
            (IConfigurationService) Activator.getBundleContext().getService(serviceReference);
        IConfigurationRecord record = service.getRecord("SignTool");

        boolean useLocalSigning = false;
        String recordValuePath = "";
        if (record != null) {
            useLocalSigning = record.getValue("UseLocalSigning").equalsIgnoreCase("true");
            recordValuePath = record.getValue("Path");
        }

        StringBuilder builder = new StringBuilder();

        boolean isWindows = System.getProperty("os.name").startsWith("Windows");

        if (isWindows) {
            builder.append("cmd /c ");
        } else {
            builder.append("sh ");
        }
        builder.append('"');
        builder.append(recordValuePath);
        builder.append(File.separatorChar);
        builder.append("sign-tool-u5500");
        if (isWindows) {
            builder.append(".bat");
        } else {
            builder.append(".sh");
        }
        builder.append('"');
        builder.append(" -s ");
        builder.append(GenericSoftwareType.AUTH_CHALLENGE.name().toLowerCase(Locale.getDefault()));
        builder.append(" -p ");
        builder.append(signPackageAlias);
        if (useLocalSigning) {
            builder.append(" --local");
        }
        builder.append(' ');
        builder.append(challengeFile.getAbsolutePath());
        builder.append(' ');
        builder.append(signedChallengeFile.getAbsolutePath());

        // Sign file using sign package argument
        notifyTaskMessage("Signing challange");
        System.out.println(builder.toString());
        runtimeExecutor.execute(builder.toString());
    }

    /**
     * {@inheritDoc}
     */
    public Object[] getControlKeys() {
        return null; // N/A
    }
}
