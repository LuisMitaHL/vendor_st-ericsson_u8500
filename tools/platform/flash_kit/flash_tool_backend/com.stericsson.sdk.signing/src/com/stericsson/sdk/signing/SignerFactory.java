package com.stericsson.sdk.signing;

import java.security.SecureRandom;

import com.stericsson.sdk.signing.a2.A2Signer;
import com.stericsson.sdk.signing.a2.A2SignerSettings;
import com.stericsson.sdk.signing.cops.ICOPSSignerSettings;
import com.stericsson.sdk.signing.generic.GenericSignerSettings;
import com.stericsson.sdk.signing.generic.GenericSoftwareType;
import com.stericsson.sdk.signing.l9540.L9540Signer;
import com.stericsson.sdk.signing.l9540.L9540SignerSettings;
import com.stericsson.sdk.signing.signerservice.ISignPackageInformation;
import com.stericsson.sdk.signing.signerservice.ISignerService;
import com.stericsson.sdk.signing.signerservice.local.encryption.EncryptionAlgorithm;
import com.stericsson.sdk.signing.u5500.U5500Signer;
import com.stericsson.sdk.signing.u5500.U5500SignerSettings;
import com.stericsson.sdk.signing.u7x00.M7X00Signer;
import com.stericsson.sdk.signing.u7x00.M7X00SignerSettings;
import com.stericsson.sdk.signing.u8500.U8500Signer;
import com.stericsson.sdk.signing.u8500.U8500SignerSettings;

/**
 * @author xtomzap
 *
 */
public final class SignerFactory {

    private SignerFactory() {

    }

    /**
     * @param settings signer settings
     * @return signer
     * @throws SignerException if anything goes wrong
     */
    public static ISigner createSigner(ISignerSettings settings) throws SignerException {
        if (settings instanceof A2SignerSettings) {
            return createA2Signer(settings);
        } else if (settings instanceof U5500SignerSettings) {
            return handleU5550Signer();
        } else if (settings instanceof U8500SignerSettings) {
            return new U8500Signer();
        } else if (settings instanceof L9540SignerSettings) {
            return new L9540Signer();
        } else if (settings instanceof M7X00SignerSettings) {
            return new M7X00Signer();
        }
        return null;
    }

    private static A2Signer createA2Signer(ISignerSettings settings) throws SignerException {
        Object compressObj = settings.getSignerSetting(ICOPSSignerSettings.KEY_LOADER_COMPRESSION);
        Object encryptObj = settings.getSignerSetting(ICOPSSignerSettings.KEY_LOADER_ENCRYPTION);
        boolean encrypt = encryptObj != null && encryptObj instanceof Boolean && (Boolean) encryptObj;
        boolean compress = compressObj != null && compressObj instanceof Boolean && (Boolean) compressObj;
        if (!compress) {
            settings.setSignerSetting(ICOPSSignerSettings.KEY_LOADER_COMPRESSION, false);
        }
        if (!encrypt) {
            settings.setSignerSetting(ICOPSSignerSettings.KEY_LOADER_ENCRYPTION, false);
        }
        if (encrypt || compress) {
            configureSettingsForLoaderEncryption(settings);
        }
        return new A2Signer();
    }

    /**
     * method to avoid checkstyle errors o_O
     */
    private static ISigner handleU5550Signer() {
        return new U5500Signer();
        // The old way of signing TVP license uses dedicated signer class.
        // return isTVP ? new TVPLicenseSigner() : new U5500Signer();
    }

    /**
     * @param signerSettings
     *            will save settings to this object, method requires that sign package alias is set
     *            in this object.
     * @param service
     *            uses this service to fetch information regarding the sign package.
     * @throws SignerException
     *             TBD
     */
    private static void configureSettingsForLoaderEncryption(ISignerSettings signerSettings) throws SignerException {
        ISignerService service = null;
        Object serviceObj = signerSettings.getSignerSetting(ISignerSettings.KEY_SIGNER_SERVICE);
        if (serviceObj != null && serviceObj instanceof ISignerService) {
            service = (ISignerService) serviceObj;
        } else {
            throw new SignerException("Failed to get signer service object");
        }

        ISignPackageInformation signPackageInformation = null;
        ISignPackageInformation[] signPackagesInformation = service.getSignPackagesInformation(false, false);
        if (signPackagesInformation == null || signPackagesInformation.length == 0) {
            throw new SignerException("Failed to get sign packages information.");
        }
        String signPackageAlias = (String) signerSettings.getSignerSetting(ISignerSettings.KEY_SIGN_PACKAGE_ALIAS);

        for (ISignPackageInformation info : signPackagesInformation) {
            if (info.getName().equalsIgnoreCase(signPackageAlias)) {
                signPackageInformation = info;
                break;
            }
        }

        if (signPackageInformation == null) {
            throw new SignerException("No sign package with name " + signPackageAlias + " found");
        }

        signerSettings.setSignerSetting(ICOPSSignerSettings.KEY_CHIPIDS, signPackageInformation.getChipIdentifiers());
        signerSettings.setSignerSetting(ICOPSSignerSettings.KEY_ENCRYPT_REQUIRED, signPackageInformation
            .isEncryptRequired());
        signerSettings.setSignerSetting(ICOPSSignerSettings.KEY_RANDOM_KEY_VALUE, getRandomKeyValue());
    }

    /**
     * @return TBD
     * @throws SignerException
     *             TBD
     */
    private static int[] getRandomKeyValue() throws SignerException {

        int[] randomKey = new int[4];
        SecureRandom sr;
        try {
            sr = SecureRandom.getInstance("SHA1PRNG");
        } catch (Exception e) {
            throw new SignerException("SHA1PRNG don't exist!");
        }
        sr.setSeed(sr.generateSeed(10));

        for (int i = 0; i < 4; ++i) {
            byte[] tempkey = new byte[4];
            sr.nextBytes(tempkey);
            randomKey[i] = EncryptionAlgorithm.fromBytesToInt(tempkey);
        }

        return randomKey;
    }

}
