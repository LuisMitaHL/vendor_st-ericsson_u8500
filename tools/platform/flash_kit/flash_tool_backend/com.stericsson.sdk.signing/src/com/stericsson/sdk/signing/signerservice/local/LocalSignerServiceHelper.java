package com.stericsson.sdk.signing.signerservice.local;

import java.io.File;
import java.math.BigInteger;
import java.security.interfaces.RSAPrivateKey;
import java.util.LinkedList;
import java.util.List;

import org.apache.log4j.Logger;

import com.stericsson.sdk.common.configuration.mesp.MESPConfiguration;
import com.stericsson.sdk.common.configuration.mesp.MESPConfigurationRecord;
import com.stericsson.sdk.signing.ISignerSettings;
import com.stericsson.sdk.signing.a2.A2SignPackage;
import com.stericsson.sdk.signing.generic.GenericSignPackage;
import com.stericsson.sdk.signing.signerservice.ISignPackage;
import com.stericsson.sdk.signing.signerservice.ISignPackageInformation;
import com.stericsson.sdk.signing.signerservice.SignPackageFactory;
import com.stericsson.sdk.signing.signerservice.SignPackageInformation;
import com.stericsson.sdk.signing.signerservice.SignerServiceException;
import com.stericsson.sdk.signing.signerservice.local.encryption.A2Encrypter;
import com.stericsson.sdk.signing.signerservice.local.encryption.U8500Encrypter;
import com.stericsson.sdk.signing.tvp.TVPSignPackage;

/**
 * 
 * @author xolabju
 * 
 */
public final class LocalSignerServiceHelper {

    private static Logger logger = Logger.getLogger(LocalSignerServiceHelper.class.getName());

    private LocalSignerServiceHelper() {

    }

    /**
     * 
     * @param aliases
     *            sign package aliases
     * @param useCommonFolder
     *            use commonf folder where keys and packages are store
     * @return mesp configuration
     * @throws SignerServiceException
     *             on errors
     */
    public static MESPConfiguration getSignPackagesInformation(String[] aliases, 
            boolean useCommonFolder, String encrRoot) throws SignerServiceException {
        MESPConfiguration mc = new MESPConfiguration();
        for (int i = 0; i < aliases.length; i++) {
            String path = SignPackageManager.getInstance(useCommonFolder).getSecretPath(aliases[i]);
            MESPConfigurationRecord entry = new MESPConfigurationRecord();
            entry.setName(ISignPackage.MESP_NAME);
            entry.setValue(ISignPackage.PACKAGE_ALIAS_NAME, aliases[i]);
            entry.setValue(ISignPackage.PARAM_NAME_PATH, path);
            entry.setValue(ISignPackage.PARAM_NAME_ENCRYPT_REQUIRED, "false");
            byte[] abPack = SignPackageManager.getInstance(useCommonFolder).getPackage(aliases[i]);
            File tempFile = createTempFile(path, abPack);
            ISignPackage spack = SignPackageFactory.createSignPackage(tempFile);

            if (spack instanceof A2SignPackage) {
                handleA2SignPackage(entry, spack, encrRoot);
            } else if (spack instanceof GenericSignPackage) {
                entry.setValue(ISignPackage.PACKAGE_TYPE_NAME, spack.getType());
                String parent = ((GenericSignPackage) spack).getParent();
                if (parent != null && !parent.trim().equals("")) {
                    entry.setValue(ISignPackage.PACKAGE_PARENT_NAME, parent);
                }
            } else if (spack instanceof TVPSignPackage) {
                entry.setValue(ISignPackage.PACKAGE_TYPE_NAME, spack.getType());
            }
            mc.addConfigurationRecord(entry);
        }
        return mc;
    }

    private static void handleA2SignPackage(MESPConfigurationRecord entry, ISignPackage spack, String encryptionRoot) {
        LocalSignerServiceConfigureHelper.handleA2SignPackage(entry, spack, encryptionRoot);
    }

    private static File createTempFile(String path, byte[] abPack) throws SignerServiceException {
        return LocalSignerServiceConfigureHelper.createTempFile(path, abPack);
    }

    /**
     * 
     * @param includeSubPackages
     *            true/false
     * @param mc
     *            tbd
     * @return pkg info
     */
    protected static ISignPackageInformation[] collectSignPackageInformations(boolean includeSubPackages,
        MESPConfiguration mc) {
        List<ISignPackageInformation> signPackageInformation = new LinkedList<ISignPackageInformation>();

        MESPConfigurationRecord[] signPackageRecords = mc.getRecords(new String[] {
            ISignPackage.MESP_NAME});
        for (MESPConfigurationRecord record : signPackageRecords) {
            SignPackageInformation pkgInfo = new SignPackageInformation(record);
            if ((pkgInfo.getParent() == null || (pkgInfo.getParent() != null && includeSubPackages))) {
                signPackageInformation.add(pkgInfo);
            }
        }
        return signPackageInformation.toArray(new ISignPackageInformation[signPackageInformation.size()]);
    }

    /**
     * converts int array to byte array
     * 
     * @param src
     *            int array
     * @return byte array
     */
    public static byte[] intToByte(int[] src) {
        int srcLength = src.length;
        byte[] dst = new byte[srcLength << 2];

        for (int i = 0; i < srcLength; i++) {
            int x = src[i];
            int j = i << 2;
            dst[j++] = (byte) ((x >>> 0) & 0xff);
            dst[j++] = (byte) ((x >>> 8) & 0xff);
            dst[j++] = (byte) ((x >>> 16) & 0xff);
            dst[j++] = (byte) ((x >>> 24) & 0xff);
        }
        return dst;
    }

    static String getSubPackageAlias(String parent, BigInteger bigExponent, BigInteger bigModulus, 
        boolean useCommonFolder, String encrRoot) {

        String[] aliases = SignPackageManager.getInstance(useCommonFolder).getPackageAliases();
        MESPConfiguration packages = null;
        try {
            packages = getSignPackagesInformation(aliases, false, encrRoot);
        } catch (SignerServiceException e) {
            logger.error("Failed to get sign packages information: " + e.getMessage());
        }
        if (packages != null) {
            MESPConfigurationRecord[] allRecords = packages.getAllRecords();
            if (allRecords == null) {
                return null;
            }
            for (MESPConfigurationRecord pkg : allRecords) {
                String pkgParent = pkg.getValue(ISignPackage.PACKAGE_PARENT_NAME);
                if (pkgParent != null && pkgParent.equals(parent)) {
                    String pkgName = pkg.getValue(ISignPackage.PACKAGE_ALIAS_NAME);
                    if (SignPackageManager.compareKeys(SignPackageManager.getInstance(false).getKey(pkgName), bigModulus,
                        bigExponent)) {
                        return pkgName;
                    }
                }
            }
        }

        return null;
    }

    static void configureLocalSignerService(boolean useCommonFolder, ISignerSettings settings) throws SignerServiceException{
        LocalSignerServiceConfigureHelper.configureLocalSignerService(useCommonFolder, settings);
    }

    static byte[] signA2helper(byte[] data, RSAPrivateKey key){
        return new A2Encrypter().calculateA2Signature(data, key);
    }

    static byte[] signU8500Helper(byte[] data, RSAPrivateKey key) throws SignerServiceException{
        return new U8500Encrypter().calculateU8500Signature(data, key);
    }
}
