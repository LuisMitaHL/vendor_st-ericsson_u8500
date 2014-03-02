package com.stericsson.sdk.signing.ui.wizards.signedsoftware;

import org.apache.log4j.Logger;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.runtime.jobs.Job;

import com.stericsson.sdk.common.ui.FlashKitPreferenceConstants;
import com.stericsson.sdk.signing.ISigner;
import com.stericsson.sdk.signing.ISignerListener;
import com.stericsson.sdk.signing.ISignerSettings;
import com.stericsson.sdk.signing.SignerException;
import com.stericsson.sdk.signing.SignerFacade;
import com.stericsson.sdk.signing.SignerSettingsException;
import com.stericsson.sdk.signing.generic.GenericSoftwareType;
import com.stericsson.sdk.signing.generic.HashType;
import com.stericsson.sdk.signing.signerservice.ISignerService;
import com.stericsson.sdk.signing.u5500.U5500SignerSettings;
import com.stericsson.sdk.signing.ui.Activator;

/**
 * 
 * @author mielcluk
 * 
 */
public final class SignJobsUtils {

    private static Logger logger = Logger.getLogger(SignJobsUtils.class.getName());

    private SignJobsUtils() {
    };

    /**
     * Runs signing job for specified settings
     * 
     * @param pPayloadFilePath
     *            payload file path
     * @param pPayloadType
     *            payload type
     * @param pSignPackage
     *            sign package
     * @param pOutputFilePath
     *            output file path
     * @param pProject
     *            pProject of which payload file comes from.
     */
    public static void runSignJob(final String pPayloadFilePath, final GenericSoftwareType pPayloadType,
        final String pSignPackage, final String pOutputFilePath, final IProject pProject) {

        try {
            ISignerSettings settings = new U5500SignerSettings(pPayloadFilePath, pOutputFilePath);
            settings.setSignerSetting(ISignerSettings.KEY_SIGN_PACKAGE_ALIAS, pSignPackage);
            settings.setSignerSetting(U5500SignerSettings.KEY_SW_TYPE, pPayloadType);
            settings.setSignerSetting(U5500SignerSettings.KEY_ROOT_KEY_HASH_TYPE, getRootKeyHashType());
            settings.setSignerSetting(U5500SignerSettings.KEY_PAYLOAD_HASH_TYPE, getPayloadHashType());
            settings.setSignerSetting(U5500SignerSettings.KEY_SIGNATURE_HASH_TYPE, getSignatureHashType());
            runSignJob(settings, pProject);
        } catch (SignerException e) {
            logger.error("Failed to initialize settings for signing. " + e.getMessage());
        }

    }

    /**
     * Returns root key hash type, by default SHA1.
     * 
     * @return root key hash type
     */
    public static HashType getRootKeyHashType() {
        String rootKeyHashTypeStr =
            Activator.getDefault().getPreferenceStore().getString(
                FlashKitPreferenceConstants.SIGNING_ROOT_KEY_HASH_TYPE);
        return getHashType(rootKeyHashTypeStr);
    }

    /**
     * Returns payload hash type, by default SHA1.
     * 
     * @return root key hash type
     */
    public static HashType getPayloadHashType() {
        String payloadHashTypeStr =
            Activator.getDefault().getPreferenceStore().getString(
                FlashKitPreferenceConstants.SIGNING_PAYLOAD_HASH_TYPE);
        return getHashType(payloadHashTypeStr);
    }

    /**
     * Returns signature hash type, by default SHA1.
     * 
     * @return root key hash type
     */
    public static HashType getSignatureHashType() {
        String signatureHashTypeStr =
            Activator.getDefault().getPreferenceStore().getString(
                FlashKitPreferenceConstants.SIGNING_SIGNATURE_HASH_TYPE);
        return getHashType(signatureHashTypeStr);
    }

    /**
     * Returns root key hash type, by default SHA1.
     * @param hashTypeStr root key hash type given by string.
     * @return root key hash type
     */
    public static HashType getHashType(String hashTypeStr) {
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

    /**
     * Runs signing job for SigningWizardGeneralSettingsPage sign page
     * 
     * @param page
     *            the sign page
     */
    public static void runSignJob(final SigningWizardGeneralSettingsPage page) {
        runSignJob(page.getPayloadFilePath(), page.getPayloadType(), page.getSignPackage(), page.getOutputFilePath(),
            page.getPayloadFileProject());
    }

    /**
     * Runs signing job for specified settings
     * 
     * @param pSettings
     *            signer settings
     * @param pProject
     *            Project of which payload file comes from.
     */
    public static void runSignJob(final ISignerSettings pSettings, final IProject pProject) {

        final SignerFacade signerFacade = new SignerFacade();
        final boolean local =
            Activator.getDefault().getPreferenceStore().getBoolean(
                FlashKitPreferenceConstants.SIGNING_USE_LOCAL_SIGNING);
        final boolean useCommonFolder =
                Activator.getDefault().getPreferenceStore().getBoolean(
                    FlashKitPreferenceConstants.SIGNING_USE_COMMONFOLDER);
        try {
            pSettings.setSignerSetting(ISignerSettings.COMMON_FOLDER_KEY_AND_PACKAGES, useCommonFolder);
        } catch (SignerSettingsException e1) {
            logger.error(e1.getMessage());
        }

        Job job = new Job("Signed Software") {
            protected IStatus run(final IProgressMonitor monitor) {
                monitor.beginTask("Signing...", IProgressMonitor.UNKNOWN);

                final ISignerListener listener = new ISignerListener() {

                    public void signerMessage(ISigner signer, String pMessage) {
                        monitor.setTaskName(pMessage);
                    }

                    public void signerServiceMessage(ISignerService signerService, String pMessage) {
                        monitor.setTaskName(pMessage);
                    }
                };

                try {
                    signerFacade.sign(pSettings, listener, local);
                } catch (SignerException e) {
                    return new Status(IStatus.ERROR, "com.stericsson.sdk.signing.ui", "Signing of software failed", e);
                } finally {
                    monitor.done();
                }
                monitor.done();
                if (pProject != null) {
                    try {
                        pProject.refreshLocal(IResource.DEPTH_INFINITE, null);
                    } catch (CoreException e) {
                        SignJobsUtils.logger.error(e.getMessage());
                    }
                }

                return Status.OK_STATUS;
            }
        };

        job.setUser(true);
        job.schedule();

    }

}
