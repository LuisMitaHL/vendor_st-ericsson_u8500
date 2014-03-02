package com.stericsson.sdk.signing.generic;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;

import org.apache.log4j.Logger;

import com.stericsson.sdk.common.LittleEndianByteConverter;
import com.stericsson.sdk.signing.ISignerSettings;
import com.stericsson.sdk.signing.SignerException;
import com.stericsson.sdk.signing.SignerSettingsException;
import com.stericsson.sdk.signing.generic.cert.GenericAuthenticationCertificate;
import com.stericsson.sdk.signing.generic.cert.GenericISSWCertificate;
import com.stericsson.sdk.signing.generic.cert.subparts.GenericISSWCustomerPartKey;
import com.stericsson.sdk.signing.generic.cert.subparts.GenericKey;
import com.stericsson.sdk.signing.signerservice.ISignPackage;
import com.stericsson.sdk.signing.signerservice.ISignerService;
import com.stericsson.sdk.signing.signerservice.SignerServiceException;

/**
 * Help class for U5500 signing
 * 
 * @author xolabju
 * 
 */
public final class GenericSignerHelper {

    private static Logger logger = Logger.getLogger(GenericSignerHelper.class.getName());

    private GenericSignerHelper() {
    }

    /**
     * Get sign package
     * @param signerSettings
     *            signer settings
     * @param signerService
     *            signer service
     * @return u8500 signpackage
     * @throws SignerException
     *             e
     * @throws FileNotFoundException 
     *              If the folder with keys and packages doesn't exist.
     */
    public static GenericSignPackage getSignPackage(GenericSignerSettings signerSettings, ISignerService signerService)
        throws SignerException, FileNotFoundException {
        GenericSignPackage pkg = null;
        String pkgName = (String) signerSettings.getSignerSetting(ISignerSettings.KEY_SIGN_PACKAGE_ALIAS);

        Object pkgObject = signerSettings.getSignerSetting(ISignerSettings.KEY_SIGN_PACKAGE);
        if (pkgObject == null || !(pkgObject instanceof GenericSignPackage)) {
            throw new SignerException("Software must be signed using appropriate sign packages. You have selected a "
                + (pkgObject == null ? "N/A" : pkgObject.getClass().getName()));
        }
        pkg = (GenericSignPackage) signerSettings.getSignerSetting(ISignerSettings.KEY_SIGN_PACKAGE);
        GenericSoftwareType swType =
            (GenericSoftwareType) signerSettings.getSignerSetting(IGenericSignerSettings.KEY_SW_TYPE);
        if (pkg == null) {
            throw new SignerException("Undefined Generic Sign Package.");
        }
        if (swType == null) {
            throw new SignerException("Undefined software type.");
        }

        if (swType == GenericSoftwareType.AUTH_CHALLENGE) {
            String authenticationCertificateFilename =
                createAuthenticationCertificateFile(pkg.getCertificate().getData());
            signerSettings.setSignerSetting(IGenericSignerSettings.KEY_TEMP_AUTHENTICATION_CERTIFICATE_FILENAME,
                authenticationCertificateFilename);

            GenericAuthenticationCertificate certificate = (GenericAuthenticationCertificate) pkg.getCertificate();
            signerSettings.setSignerSetting(IGenericSignerSettings.KEY_SIGNED_HEADER_SIGNATURE_SIZE,
                (short) certificate.getModulusSize());
        } else if ((swType != GenericSoftwareType.ISSW) && (swType != GenericSoftwareType.AUTH_CHALLENGE)
            && (swType != GenericSoftwareType.CRKC)) {

            pkg = getGenericSignPackage(signerSettings, signerService, pkg, pkgName, swType);
        } else if (swType != GenericSoftwareType.CRKC) {
            GenericISSWCertificate certificate = (GenericISSWCertificate) pkg.getCertificate();
            signerSettings.setSignerSetting(IGenericSignerSettings.KEY_SIGNED_HEADER_SIGNATURE_SIZE,
                (short) certificate.getRootKey().getModulus().length);

            //Set the key properties for later use
            useCommonFolder(signerService, signerSettings, certificate.getRootKey(), pkgName);
        }

        return pkg;
    }

    private static void useCommonFolder(ISignerService signerService, GenericSignerSettings signerSettings, 
            GenericKey key, String pkgName) throws SignerServiceException{
        Boolean useCommonFolder = (Boolean)signerSettings.getSignerSetting(ISignerSettings.COMMON_FOLDER_KEY_AND_PACKAGES);

        if(useCommonFolder != null && useCommonFolder){
            signerService.getSubPackageAlias(pkgName, key.getPublicExponent(), 
                    LittleEndianByteConverter.reverse(key.getModulus()),
                    (signerSettings.getSignerSetting(GenericSignerSettings.KEY_LOCAL_ENCRYPTION_KEY_ROOT)!= null) 
                    ? signerSettings.getSignerSetting(GenericSignerSettings.KEY_LOCAL_ENCRYPTION_KEY_ROOT).toString() : null);
        }
    }

    /**
     * method extracted to avoid checkstyle errors :X
     * @throws FileNotFoundException 
     */
    private static GenericSignPackage getGenericSignPackage(GenericSignerSettings signerSettings,
        ISignerService signerService, GenericSignPackage pkg, String pkgName, GenericSoftwareType swType)
        throws SignerException, SignerServiceException, SignerSettingsException, FileNotFoundException {
        GenericISSWCertificate certificate = (GenericISSWCertificate) pkg.getCertificate();
        GenericISSWCustomerPartKey key = certificate.getKey(swType);
        Boolean useCommonFolder = (Boolean)signerSettings.getSignerSetting(ISignerSettings.COMMON_FOLDER_KEY_AND_PACKAGES);

        if (key == null) {
            throw new SignerException("Could not get key from certificate based on software type " + swType);
        }

//      Check if the chosen payload hash type has not been revoked.
        int revokedHashTypes = ((key.getKeyTypeWithRevocationHashTypes() & 0xFF00) >> 8);
        HashType payloadHashType = (HashType)signerSettings.getSignerSetting(IGenericSignerSettings.KEY_PAYLOAD_HASH_TYPE);
        if ((revokedHashTypes & payloadHashType.getHashType()) != 0) {
            throw new SignerException("The specified payload hash type " + payloadHashType.name() + " has been revoked.");
        }

        ISignPackage signPackage;
        if(useCommonFolder != null && useCommonFolder){
            signPackage = (ISignPackage)signerService.getSubPackageAlias(pkgName, key.getExponent(), 
                    LittleEndianByteConverter.reverse(key.getModulus()), 
                    (signerSettings.getSignerSetting(GenericSignerSettings.KEY_LOCAL_ENCRYPTION_KEY_ROOT)!= null) 
                    ? signerSettings.getSignerSetting(GenericSignerSettings.KEY_LOCAL_ENCRYPTION_KEY_ROOT).toString() : null);
        }else{
            // certificates are in little-endian order. default representation of big integer arrays
            // are in big-endian. Reverse to get value in big-endian
            String signPackageAlias =
                (String)signerService.getSubPackageAlias(pkgName, key.getExponent(), LittleEndianByteConverter.reverse(key
                    .getModulus()), (signerSettings.getSignerSetting(GenericSignerSettings.KEY_LOCAL_ENCRYPTION_KEY_ROOT)!= null) 
                    ? signerSettings.getSignerSetting(GenericSignerSettings.KEY_LOCAL_ENCRYPTION_KEY_ROOT).toString() : null);
            if (signPackageAlias == null || signPackageAlias.trim().equals("")) {
                throw new SignerException("The specified sign package does not allow " + swType + " signing.");
            }
            signPackage = signerService.getSignPackage(signPackageAlias, false);
        }

        if (signPackage instanceof GenericSignPackage) {
            pkg = (GenericSignPackage) signPackage;
        } else {
            throw new SignerException("Returned sign package is not usable for signing of this kind of software.");
        }

        signerSettings.setSignerSetting(IGenericSignerSettings.KEY_SIGNED_HEADER_SIGNATURE_SIZE, (short) key
            .getModulus().length);
        return pkg;
    }

    private static String createAuthenticationCertificateFile(byte[] data) throws SignerException {

        File file = null;
        FileOutputStream fileOutput = null;

        try {
            file = File.createTempFile("auth" + System.currentTimeMillis(), "cert");
            if (file != null) {
                file.deleteOnExit();
                fileOutput = new FileOutputStream(file);
                fileOutput.write(data);
                return file.getAbsolutePath();
            }
        } catch (IOException e) {
            throw new SignerException(e.getMessage());
        } finally {
            if (fileOutput != null) {
                try {
                    fileOutput.close();
                } catch (IOException e) {
                    logger.warn(e.getMessage());
                }
            }
        }

        return null;
    }

    /**
     * 
     * @param uncheckedBlob
     *            path to unchecked blob
     * @return the blob as bytes
     * @throws SignerException
     *             on errors
     */
    public static byte[] readUncheckedBlob(String uncheckedBlob) throws SignerException {

        return GenericSignerPropertiesHelper.readUncheckedBlob(uncheckedBlob);
    }
}
