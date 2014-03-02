package com.stericsson.sdk.signing.signerservice.local;

import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.math.BigInteger;
import java.security.interfaces.RSAPrivateKey;
import java.util.HashMap;

import org.apache.log4j.Logger;

import com.stericsson.sdk.signing.ISignerSettings;
import com.stericsson.sdk.signing.generic.GenericSignPackage;
import com.stericsson.sdk.signing.generic.SignatureType;
import com.stericsson.sdk.signing.signerservice.AbstractSignerService;
import com.stericsson.sdk.signing.signerservice.ISignPackage;
import com.stericsson.sdk.signing.signerservice.ISignPackageInformation;
import com.stericsson.sdk.signing.signerservice.SignPackageFactory;
import com.stericsson.sdk.signing.signerservice.SignerServiceException;
import com.stericsson.sdk.signing.signerservice.SignerServiceHelper;
import com.stericsson.sdk.signing.signerservice.local.encryption.U5500Encrypter;
import com.stericsson.sdk.signing.signerservice.local.keys.KeyParser;


/**
 * Class for signing using local sign packages and keys
 * 
 * @author xolabju
 * 
 */
public class LocalSignerService extends AbstractSignerService {

    private Logger logger;

    private String currentSignPackage;
    private String signKeyPath;

    private boolean useCommonFolder;

    private ISignPackage currentSignPcg;

    private BigInteger currentKeyModulus;

    private BigInteger currentKeyExponent;

    private ISignerSettings settings;

    /**
     * Constructor
     * @param useCommonFolderParam useCommonFolderParam
     * @param genericSignerSettings
     *            passing info for overriding env variables.
     */
    public LocalSignerService(boolean useCommonFolderParam, ISignerSettings genericSignerSettings){
        logger = Logger.getLogger(getClass().getName());
        this.useCommonFolder = useCommonFolderParam;
        settings = genericSignerSettings;
    }

    /**
     * {@inheritDoc}
     */
    public void configure() throws SignerServiceException {
        logger.info("Configuring local signer service");
        LocalSignerServiceHelper.configureLocalSignerService(useCommonFolder, settings);
    }

    /**
     * {@inheritDoc}
     */
    public void connect(InputStream input, OutputStream output) throws SignerServiceException {
        // ignored

    }

    /**
     * {@inheritDoc}
     */
    public void disconnect() {
        // ignored

    }

    /**
     * {@inheritDoc}
     */
    public byte[] encryptSessionKey(int chipid, int[] key) throws Exception {
        return LocalSignerServiceConfigureHelper.encryptSessionKey(chipid, key, currentSignPackage, useCommonFolder);
    }

    /**
     * {@inheritDoc}
     */
    public ISignPackage getSignPackage(String alias, boolean enableHelloHandshake) throws SignerServiceException {

        ISignPackage signPackage =
            SignPackageFactory.createSignPackage(SignPackageManager.getInstance(useCommonFolder).getPackage(alias));
        if (signPackage == null) {
            throw new SignerServiceException("Failed to get sign package " + alias);
        }
        currentSignPackage = signPackage.getAlias();
        return signPackage;
    }

    /**
     * {@inheritDoc}
     */
    public void setSignKeyPath(String pSignKeyPath, boolean enableHelloHandshake) throws SignerServiceException {

        if (pSignKeyPath == null || "".equalsIgnoreCase(pSignKeyPath)) {
            throw new SignerServiceException("Failed to set sign key path " + pSignKeyPath);
        }
        signKeyPath = pSignKeyPath;
    }

    /**
     * {@inheritDoc}
     */
    public ISignPackageInformation[] getSignPackagesInformation() throws SignerServiceException {
        return getSignPackagesInformation(true, false);
    }

    /**
     * {@inheritDoc}
     */
    public ISignPackageInformation[] getSignPackagesInformation(boolean enableHelloHandshake, boolean includeSubPackages)
        throws SignerServiceException {
        String ecrRoot = "";
        return LocalSignerServiceConfigureHelper.getSignPackagesInformation(enableHelloHandshake, 
                includeSubPackages, ecrRoot, useCommonFolder);
    }

    /**
     * {@inheritDoc}
     */
    public HashMap<String, String> getSubPackages(String parent) throws SignerServiceException {
        return LocalSignerServiceConfigureHelper.getSubPackages(parent);
    }

    /**
     * {@inheritDoc}
     */
    public byte[] signA2(byte[] data) throws SignerServiceException {
        if (currentSignPackage == null) {
            throw new SignerServiceException("No sign package selected.");
        }
        RSAPrivateKey key = getPrivateKey();
        if (key == null) {
            throw new SignerServiceException("Failed to get key for " + currentSignPackage + ".");
        }
        return LocalSignerServiceHelper.signA2helper(data, key);
    }

    /**
     * {@inheritDoc}
     */
    public byte[] signU8500(byte[] data) throws SignerServiceException {
        if (currentSignPackage == null) {
            throw new SignerServiceException("No sign package selected.");
        }

        RSAPrivateKey rsaPrivateKey = getPrivateKey();

        return LocalSignerServiceHelper.signU8500Helper(data, rsaPrivateKey);
    }

    /**
     * {@inheritDoc}
     */
    public Object getSubPackageAlias(String pkgName, int exponent, byte[] modulus, String encrRoot) {
        BigInteger bigExponent = BigInteger.valueOf(exponent);
        BigInteger bigModulus = new BigInteger(modulus);
        if(useCommonFolder){
            try {
                ISignPackage signPackage = getSignPackage(pkgName, false);
                currentSignPcg = signPackage;
                currentKeyModulus = bigModulus;
                currentKeyExponent = bigExponent;

                boolean keyFound = AbstractSignPackageManager.getInstance(useCommonFolder).
                        getSignPackage(signPackage, bigModulus, bigExponent);

                return (keyFound) ? new GenericSignPackage(null, null, ISignPackage.PACKAGE_TYPE_EMPTY_GENERIC, null) : null;
            } catch (SignerServiceException e) {
                logger.error(e.getMessage());
            } catch (FileNotFoundException e) {
                logger.error(e.getMessage());
            }
        }else{
            return LocalSignerServiceHelper.getSubPackageAlias(pkgName, bigExponent, bigModulus, useCommonFolder, encrRoot);
        }
        return null;
    }

    /**
     * {@inheritDoc}
     */
    public byte[] signU5500(byte[] data, int signatureHashTypeAlgoritm, SignatureType signatureType)
        throws SignerServiceException {

        RSAPrivateKey crtKey;
        if (signKeyPath != null) {
            try {
                crtKey = KeyParser.parsePEMPrivateKey(signKeyPath);
            } catch (Exception e) {
                throw new SignerServiceException("Sign Key "+ signKeyPath + " not valid.");
            }
        } else {
            if (currentSignPackage == null) {
                throw new SignerServiceException("No sign package selected.");
            }
            crtKey = getPrivateKey();
        }

        if (signatureType == SignatureType.RSASSA_PKCS_V1_5) {
            return new U5500Encrypter().calculateU5500Signature(data, crtKey, signatureHashTypeAlgoritm);
        } else if (signatureType == SignatureType.RSASSA_PPS) {
            return new U5500Encrypter().calculateU5500PSSSignature(data, crtKey, signatureHashTypeAlgoritm);
        } else {
            throw new SignerServiceException("Selected signature type is not supported.");
        }
    }

    private RSAPrivateKey getPrivateKey(){
        RSAPrivateKey rsaPrivateKey = null;
        try{
            if(useCommonFolder){
                rsaPrivateKey = AbstractSignPackageManager.getInstance(useCommonFolder).
                        getKey(currentSignPcg, currentKeyModulus, currentKeyExponent);
            }else{
                rsaPrivateKey = AbstractSignPackageManager.getInstance(useCommonFolder).getKey(currentSignPackage);
            }
        }catch (IOException ioe) {
            logger.error("Cannot get the private key for the alias " + currentSignPackage);
        }

        return rsaPrivateKey;
    }
    /**
     * {@inheritDoc}
     */
    public byte[] signX509(byte[] data) throws SignerServiceException {
        return SignerServiceHelper.calculateRSAPKCS1Signature(data, currentSignPackage, useCommonFolder);
    }

    /**
     * @return Alias of current sign package
     */
    public String getCurrentSignPackageAlias() {
        return currentSignPackage;
    }
}
