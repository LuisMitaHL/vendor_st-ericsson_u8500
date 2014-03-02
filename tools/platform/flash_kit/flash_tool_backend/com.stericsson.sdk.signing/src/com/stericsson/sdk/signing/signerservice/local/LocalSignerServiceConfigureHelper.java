package com.stericsson.sdk.signing.signerservice.local;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.HashMap;
import java.util.Vector;

import org.apache.log4j.Logger;

import com.stericsson.sdk.common.configuration.mesp.MESPConfiguration;
import com.stericsson.sdk.common.configuration.mesp.MESPConfigurationRecord;
import com.stericsson.sdk.signing.ISignerSettings;
import com.stericsson.sdk.signing.a2.A2SignPackage;
import com.stericsson.sdk.signing.signerservice.ISignPackage;
import com.stericsson.sdk.signing.signerservice.ISignPackageInformation;
import com.stericsson.sdk.signing.signerservice.ISignerService;
import com.stericsson.sdk.signing.signerservice.SignPackageFactory;
import com.stericsson.sdk.signing.signerservice.SignerServiceException;
import com.stericsson.sdk.signing.signerservice.local.encryption.EncryptionAlgorithm;


/**
 * @author Xxvs0002
 *
 */
public final class LocalSignerServiceConfigureHelper {

    private static Logger logger = Logger.getLogger(LocalSignerServiceHelper.class.getName());

    /**
     */
    private LocalSignerServiceConfigureHelper(){}

    /**
     * @param useCommonFolder useCommonFolder
     * @param settings settings
     * @throws SignerServiceException SignerServiceException
     */
    public static void configureLocalSignerService(boolean useCommonFolder, ISignerSettings settings) 
        throws SignerServiceException{
        String keyRoot = getKeyRoot((String) null, settings);
        String pkgRoot = getPackageRoot((String) null, settings);
        String encrRoot = getEncryptionKeyRoot((String) null, settings);

        if(useCommonFolder){
            String keysAndPackagesRoot = getKeysAndPackagesRoot();

            if (keysAndPackagesRoot == null || keysAndPackagesRoot.trim().equals("")) {
                logger.error("The system property " + ISignerService.ENV_LOCAL_SIGN_KEYS_AND_PACKAGE_ROOT
                  + " is not set. To use the new way of singing where keys and packages are stored "
                  + "in same folder you must set this environment variable LOCAL_SIGNKEYSANDPACKAGESROOT to show on the folder"
                  + " where keys and packages are stored");
              throw new SignerServiceException("The system property " + ISignerService.ENV_LOCAL_SIGN_KEYS_AND_PACKAGE_ROOT
                  + " is not set.");
            }

            keyRoot = keysAndPackagesRoot;
        }
        if (encrRoot != null && !encrRoot.trim().equals("")) {
            try {
                EncryptionKeyManager.getInstance().init(new File(encrRoot));
            } catch (FileNotFoundException e) {
                logger.warn("Failed to init encryption keys. A2 loader encryption will be disabled: " + e.getMessage());
            }
        } else {
            logger.info(ISignerService.ENV_LOCAL_ENCRYPTION_KEY_ROOT
                + " is not set. Loader encryption will not be possible.");
        }

        if (pkgRoot == null || pkgRoot.trim().equals("")) {
            throw new SignerServiceException("The system property " + ISignerService.ENV_LOCAL_SIGN_PACKAGE_ROOT
                + " is not set.");
        }
        if (keyRoot == null || keyRoot.trim().equals("")) {
            throw new SignerServiceException("The system property " + ISignerService.ENV_LOCAL_SIGN_KEY_ROOT
                + " is not set.");
        }

        try {
            SignPackageManager.getInstance(useCommonFolder).init(new File(keyRoot), new File(pkgRoot));
        } catch (FileNotFoundException e) {
            throw new SignerServiceException(e.getMessage());
        }
    }

    /**
     * @param encrRoot
     *         passing info for overriding env variable.
     * @param settings
     *         passing info for overriding env variable.
     * @return encr root
     */
    protected static String getEncryptionKeyRoot(String encrRoot, ISignerSettings settings) {
        if (settings != null){
            encrRoot = settings.getSignerSetting(ISignerSettings.KEY_LOCAL_ENCRYPTION_KEY_ROOT)!=null 
                    ? settings.getSignerSetting(ISignerSettings.KEY_LOCAL_ENCRYPTION_KEY_ROOT).toString() : null;
        }
        if (encrRoot == null || encrRoot.trim().equals("")){
            encrRoot = System.getProperty(ISignerService.ENV_LOCAL_ENCRYPTION_KEY_ROOT);
        }

        return encrRoot == null || encrRoot.trim().equals("") ? System
            .getenv(ISignerService.ENV_LOCAL_ENCRYPTION_KEY_ROOT) : encrRoot;
    }

    /**
     * @param pkgRoot
     *         passing info for overriding env variable.
     * @param settings
     *         passing info for overriding env variable.
     * @return pkg root
     */
    protected static String getPackageRoot(String pkgRoot, ISignerSettings settings) {
        if (settings != null){
            pkgRoot = settings.getSignerSetting(ISignerSettings.KEY_LOCAL_SIGN_PACKAGE_ROOT)!=null 
                    ? settings.getSignerSetting(ISignerSettings.KEY_LOCAL_SIGN_PACKAGE_ROOT).toString() : null;
        }
        if (pkgRoot == null){
            pkgRoot = System.getProperty(ISignerService.ENV_LOCAL_SIGN_PACKAGE_ROOT);
        }
        return pkgRoot == null || pkgRoot.trim().equals("") ? System.getenv(ISignerService.ENV_LOCAL_SIGN_PACKAGE_ROOT)
            : pkgRoot;
    }

    /**
     * @param keyRoot
     *         passing info for overriding env variable.
     * @param settings
     *         passing info for overriding env variable.
     * @return key root
     */
    protected static String getKeyRoot(String keyRoot, ISignerSettings settings) {
        if (settings != null){
            keyRoot = settings.getSignerSetting(ISignerSettings.KEY_LOCAL_KEY_ROOT)!=null 
                    ? settings.getSignerSetting(ISignerSettings.KEY_LOCAL_KEY_ROOT).toString() : null;
        }
        if (keyRoot == null){
            keyRoot = System.getProperty(ISignerService.ENV_LOCAL_SIGN_KEY_ROOT);
        }

        return keyRoot == null || keyRoot.trim().equals("") ? System.getenv(ISignerService.ENV_LOCAL_SIGN_KEY_ROOT)
            : keyRoot;
    }

    /**
     * Get the environment variable for local keys and packages stored in common folder
     * @return keys and packages root
     */
    protected static String getKeysAndPackagesRoot() {
        String keyRoot = System.getProperty(ISignerService.ENV_LOCAL_SIGN_KEYS_AND_PACKAGE_ROOT);

        return keyRoot == null || keyRoot.trim().equals("") ? System.getenv(ISignerService.ENV_LOCAL_SIGN_KEYS_AND_PACKAGE_ROOT)
            : keyRoot;
    }

    /**
     * @param path path
     * @param abPack abPack
     * @return File
     * @throws SignerServiceException SignerServiceException
     */
    protected static File createTempFile(String path, byte[] abPack) throws SignerServiceException {
        File tempFile = null;
        FileOutputStream fos = null;
        try {
            tempFile = File.createTempFile("spkg", "zip");
            fos = new FileOutputStream(tempFile);
            fos.write(abPack);
            fos.close();
            tempFile.deleteOnExit();
        } catch (IOException ioe) {
            throw new SignerServiceException("Failed to read signpackage " + path + ": " + ioe.getMessage());
        } finally {
            if (fos != null) {
                try {
                    fos.close();
                } catch (IOException e) {
                    // ignore
                    e.getMessage();
                }
            }
        }
        return tempFile;
    }

    /**
     * @param entry entry
     * @param spack spack
     */
    protected static void handleA2SignPackage(MESPConfigurationRecord entry, ISignPackage spack, String encryptionRoot) {
        A2SignPackage a2sp = (A2SignPackage) spack;
        entry.setValue(ISignPackage.PACKAGE_TYPE_NAME, a2sp.getType()); // Sign packages
        int version = a2sp.getVersion();
        // adding chipid information to the signpackage
        String encrRoot = getEncryptionKeyRoot(encryptionRoot, null);
        try {
            if (encrRoot != null && !encrRoot.trim().equals("")) {
                ChipClass chipclass = EncryptionKeyManager.getInstance().getEncryptionKey(a2sp.getChipClass());
                if (chipclass != null) {
                    Vector<String> chipIds = new Vector<String>();
                    Vector<String> chipAliases = new Vector<String>();
                    Cid cid = chipclass.getCid(a2sp.getCertificateList().getSubjectCID());
                    handleChipId(entry, chipIds, chipAliases, cid);
                }
            }
        } catch (Exception e) {
            logger.error("Error retrieving the CID from Signpackage " + a2sp.getAlias());
            e.printStackTrace();
        }
        entry.setValue(ISignPackage.HEADER_VERSION_NAME, "" + version);
    }

    /**
     * @param entry entry
     * @param chipIds chipIds
     * @param chipAliases chipAliases
     * @param cid cid
     */
    protected static void handleChipId(MESPConfigurationRecord entry, Vector<String> chipIds, Vector<String> chipAliases,
            Cid cid) {
            if (cid != null) {
                int[] chip = cid.getChipIds();
                for (int j = 0; j < chip.length; j++) {
                    chipIds.add("0x" + Integer.toHexString(chip[j]));
                    chipAliases.add(cid.getChipId(chip[j]).getAlias());
                }
                entry.setArrayValue(ISignPackage.PARAM_NAME_CHIPIDS, chipIds.toArray(new String[chipIds.size()]));
                entry.setArrayValue(ISignPackage.PARAM_NAME_CHIPALIASES, chipAliases
                    .toArray(new String[chipAliases.size()]));
                entry.setValue(ISignPackage.PARAM_NAME_ENCRYPT_REQUIRED, "true");
            }
        }

   /**
     * {@inheritDoc}
     */
    static HashMap<String, String> getSubPackages(String parent) throws SignerServiceException {
        HashMap<String, String> map = new HashMap<String, String>();

        ISignPackageInformation[] signPackagesInformation = getSignPackagesInformation(false, true, null, false);
        if (signPackagesInformation == null) {
            throw new SignerServiceException("Failed to get sign packages information.");
        }
        for (ISignPackageInformation info : signPackagesInformation) {
            String infoParent = null;
            if (info != null) {
                infoParent = info.getParent();
            }
            if (infoParent != null && parent.equals(infoParent)) {
                map.put(info.getType(), info.getName());
            }
        }
        return map;
    }

    /**
     * {@inheritDoc}
     */
    public static ISignPackageInformation[] getSignPackagesInformation(boolean enableHelloHandshake, boolean includeSubPackages,
            String encrRoot, boolean useCommonFolder)
        throws SignerServiceException {
        String[] aliases = SignPackageManager.getInstance(useCommonFolder).getPackageAliases();
        MESPConfiguration mc = LocalSignerServiceHelper.getSignPackagesInformation(aliases, useCommonFolder, encrRoot);

        return LocalSignerServiceHelper.collectSignPackageInformations(includeSubPackages, mc);
    }

    /**
     * {@inheritDoc}
     */
    static byte[] encryptSessionKey(int chipid, int[] key, String currentSignPackage, boolean useCommonFolder) throws Exception {
        if (currentSignPackage == null) {
            throw new SignerServiceException("No sign package selected.");
        }
        byte[] encryptedSessionkey = null;
        ISignPackage pkg =
            SignPackageFactory.createSignPackage(SignPackageManager.getInstance(useCommonFolder).getPackage(currentSignPackage));
        if (pkg != null && pkg instanceof A2SignPackage) {
            A2SignPackage a2pkg = (A2SignPackage) pkg;
            ChipClass chipClass = EncryptionKeyManager.getInstance().getEncryptionKey(a2pkg.getChipClass());
            if (chipClass != null) {
                Cid cid = chipClass.getCid(a2pkg.getCertificateList().getSubjectCID());
                int[] secretKey = cid.getKey(chipid);
                ByteArrayOutputStream baos = new ByteArrayOutputStream();
                EncryptionAlgorithm.encryptPayload(32,
                    new ByteArrayInputStream(LocalSignerServiceHelper.intToByte(key)), secretKey, baos);
                encryptedSessionkey = baos.toByteArray();
            } else {
                throw new Exception("No encryption data found for signpackage " + currentSignPackage);
            }

        } else {
            throw new Exception("An A2 sign package must be selected to enable encryption.");
        }

        return encryptedSessionkey;
    }
}
