/**
 * 
 */
package com.stericsson.sdk.signing.ui.utils;

import java.io.File;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.preference.IPreferenceStore;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Shell;

import com.stericsson.sdk.common.ui.FlashKitPreferenceConstants;
import com.stericsson.sdk.signing.generic.GenericSoftwareType;
import com.stericsson.sdk.signing.signerservice.ISignPackage;
import com.stericsson.sdk.signing.signerservice.ISignPackageInformation;
import com.stericsson.sdk.signing.signerservice.ISignerService;
import com.stericsson.sdk.signing.signerservice.SignerService;
import com.stericsson.sdk.signing.ui.Activator;
import com.stericsson.sdk.signing.ui.jobs.RefreshSignPackagesJob;

/**
 * @author kapalpaw
 * 
 */
public final class SignedSoftwareUtils {

    private static final String SIGNED_PREFIX = "signed_";

    private SignedSoftwareUtils() {
    }

    /**
     * Creates sorted alphabetically list of {@link GenericSoftwareType} item names, excluding file
     * chunks.
     * 
     * @return string array of {@link GenericSoftwareType} names
     */
    public static String[] getPayloadTypeItems() {
        // Exclude U5500SoftwareType.AUTH_CHALLENGE, since it is only a chunk of the file
        final String[] items = new String[GenericSoftwareType.values().length - 1];
        int i = 0;
        for (GenericSoftwareType type : GenericSoftwareType.values()) {
            if (!GenericSoftwareType.AUTH_CHALLENGE.equals(type)) {
                items[i++] = type.getName();
            }
        }
        Arrays.sort(items);
        return items;
    }

    /**
     * 
     * @return the supported hash types
     */
    public static String[] getHashTypes() {
        return new String[] {
            "SHA-1", "SHA-256", "SHA-384", "SHA-512"};
    }

    /**
     * Gets index of given string element.
     * 
     * @param items
     *            array of strings
     * @param value
     *            element to be searched
     * @return index of the element in the array
     */
    public static int getElementIndex(final String[] items, final String value) {
        // Find "Flash Archive" element index
        int flashArchiveIndex = 0;
        for (String item : items) {
            if (value.equals(item)) {
                return flashArchiveIndex;
            }
            flashArchiveIndex++;
        }
        return 0;
    }

    /**
     * Returns {@link GenericSoftwareType} that matches given string name.
     * 
     * @param pSoftwareTypeName
     *            string containing {@link GenericSoftwareType} name
     * @return {@link GenericSoftwareType} element.
     */
    public static GenericSoftwareType getSofwareType(final String pSoftwareTypeName) {
        for (GenericSoftwareType type : GenericSoftwareType.values()) {
            if (type.getName().equals(pSoftwareTypeName)) {
                return type;
            }
        }
        return null;
    }

    /**
     * Returns selected element in the combo.
     * 
     * @param pCombo
     *            combo box containing {@link GenericSoftwareType} elements
     * @return {@link GenericSoftwareType} element.
     */
    public static GenericSoftwareType getSelectedElement(final Combo pCombo) {
        return getSofwareType(pCombo.getText());
    }

    /**
     * Gets sign package names from sign server
     * 
     * @param pShell
     *            the shell for error messages
     * @param pJob
     *            the job for sign packages refresh
     * @return string array of sign package names
     */
    public static String[] getAllSignPackageAliases(final Shell pShell, RefreshSignPackagesJob pJob) {
        final ArrayList<String> packageNames = new ArrayList<String>();

        ArrayList<ISignPackageInformation> signPackages = getSignPackagesInformations(pShell, pJob);
        if (signPackages != null) {
            for (ISignPackageInformation sp : signPackages) {
                if (ISignPackage.PACKAGE_TYPE_U5500_ROOT.equalsIgnoreCase(sp.getType())
                    || ISignPackage.PACKAGE_TYPE_X509.equalsIgnoreCase(sp.getType())
                    || ISignPackage.PACKAGE_TYPE_U5500_CRK.equalsIgnoreCase(sp.getType())) {
                    packageNames.add(sp.getName());
                }
            }
        }

        return packageNames.toArray(new String[packageNames.size()]);
    }

    /**
     * Gets authenticate sign package names from sign server
     * 
     * @param pShell
     *            the shell for error messages
     * @param pJob
     *            the job for sign packages refresh
     * @return string array of sign package names
     */
    public static String[] getAuthenticateSignPackageAliases(final Shell pShell, RefreshSignPackagesJob pJob) {
        final ArrayList<String> packageNames = new ArrayList<String>();

        ArrayList<ISignPackageInformation> signPackages = getSignPackagesInformations(pShell, pJob);
        if (signPackages != null) {
            for (ISignPackageInformation sp : signPackages) {
                if (ISignPackage.PACKAGE_TYPE_U5500_AUTH.equalsIgnoreCase(sp.getType())) {
                    packageNames.add(sp.getName());
                }
            }
        }

        return packageNames.toArray(new String[packageNames.size()]);
    }

    private static ArrayList<ISignPackageInformation> getSignPackagesInformations(final Shell pShell,
        RefreshSignPackagesJob pJob) {
        ArrayList<ISignPackageInformation> signPackagesToReturn = null;

        pJob.setUser(true);
        pJob.schedule();

        try {
            pJob.join();
            if (!pJob.getResult().isOK()) {
                return null;
            }
            signPackagesToReturn = pJob.getSignPackages();
        } catch (InterruptedException ie) {
            MessageDialog.openError(pShell, "Sign Packages Refresh",
                "Failed to refresh sign packages. Is the sign preferences set up correctly? Possible reason:\n"
                    + ie.getMessage());
        }
        return signPackagesToReturn;
    }

    /**
     * Gets default payload type for given payload file.
     * 
     * @param pPath
     *            Payload input file.
     * @return Payload type.
     */
    public static GenericSoftwareType getDefaultPayloadType(String pPath) {
        // Convenience method, select software type based on path else first (0) element
        Pattern zipPattern = Pattern.compile("(.*)\\.zip$");

        if (pPath.contains("xloader")) {
            return GenericSoftwareType.XLOADER;

        } else if (pPath.contains("issw")) {
            return GenericSoftwareType.ISSW;

        } else if (pPath.contains("loader")) {
            return GenericSoftwareType.LDR;

        } else if (pPath.contains("modem")) {
            return GenericSoftwareType.MODEM;

        } else if (pPath.contains("ipl")) {
            return GenericSoftwareType.IPL;

        } else if (pPath.contains("ta") || pPath.contains("trusted")) {
            return GenericSoftwareType.TRUSTED;

        } else if (zipPattern.matcher(pPath).matches()) { // If the payload file is .zip file
            return GenericSoftwareType.FLASH_ARCHIVE;

        } else {
            return GenericSoftwareType.values()[0];

        }
    }

    /**
     * Creates output path for given payload file.
     * 
     * @param path
     *            Payload input file.
     * @return Payload output file path.
     */
    public static String getDefaultOutputPath(String path) {

        Pattern binPattern = Pattern.compile("(.*)\\.bin$");

        String outputFilePath = path;

        // If the payload file is a .bin file
        if (binPattern.matcher(path).matches()) {

            // Replace file extension from .bin to .ssw
            String binExtension = "\\.bin$";
            Matcher m = Pattern.compile(binExtension).matcher(path);
            outputFilePath = m.replaceAll(".ssw");

        } else { // Add SIGNED_PREFIX to the name of the file to avoid overwriting the payload file

            final File file = new File(path);
            if (!file.exists()) {
                return null;
            }
            outputFilePath = "";
            File parentFile = file.getParentFile();
            if (parentFile != null) {
                outputFilePath += parentFile.getPath() + File.separator;
            }
            outputFilePath += SIGNED_PREFIX + file.getName();

        }

        return outputFilePath;
    }

    /**
     * Sets properties needed by signing mechanism
     */
    public static void setSystemVariables() {
        IPreferenceStore store = Activator.getDefault().getPreferenceStore();

        if (store != null) {
            System.setProperty(SignerService.ENV_SIGNER_SERVICE_SERVERS, store
                .getString(FlashKitPreferenceConstants.SIGNING_SIGN_SERVER));
            System.setProperty(ISignerService.ENV_SIGN_PACKAGE_ROOT, store
                .getString(FlashKitPreferenceConstants.SIGNING_SIGN_PACKAGE_ROOT));
            System.setProperty(ISignerService.ENV_LOCAL_SIGN_PACKAGE_ROOT, store
                .getString(FlashKitPreferenceConstants.SIGNING_LOCAL_SIGN_PACKAGE_ROOT));
            System.setProperty(ISignerService.ENV_LOCAL_SIGN_KEY_ROOT, store
                .getString(FlashKitPreferenceConstants.SIGNING_LOCAL_KEY_ROOT));
            System.setProperty(ISignerService.ENV_LOCAL_SIGN_KEYS_AND_PACKAGE_ROOT, store
                .getString(FlashKitPreferenceConstants.SIGNING_LOCAL_KEY_PACKAGES_ROOT));
        }
    }

    /**
     * Creates and returns refresh sign packages job
     * 
     * @return job
     */
    public static RefreshSignPackagesJob createRefreshSignPackagesJob() {
        boolean local =
            Activator.getDefault().getPreferenceStore().getBoolean(
                FlashKitPreferenceConstants.SIGNING_USE_LOCAL_SIGNING);
        boolean useCommonFolder =
                Activator.getDefault().getPreferenceStore().getBoolean(
                    FlashKitPreferenceConstants.SIGNING_USE_COMMONFOLDER);
        RefreshSignPackagesJob job = new RefreshSignPackagesJob(local, useCommonFolder);
        return job;
    }

}
