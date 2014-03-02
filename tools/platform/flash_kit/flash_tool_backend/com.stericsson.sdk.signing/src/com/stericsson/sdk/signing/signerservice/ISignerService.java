package com.stericsson.sdk.signing.signerservice;

import java.io.InputStream;
import java.io.OutputStream;
import java.util.HashMap;

import com.stericsson.sdk.signing.generic.SignatureType;

/**
 * The ISignerService interface is the super interface for all remote signer service
 * implementations.
 * 
 * @author xtomlju
 */
public interface ISignerService {

    /** */
    String ENV_SIGN_PACKAGE_ROOT = "SIGNPACKAGEROOT";

    /** */
    String ENV_LOCAL_SIGN_KEY_ROOT = "LOCAL_KEYROOT";

    /** */
    String ENV_LOCAL_ENCRYPTION_KEY_ROOT = "LOCAL_ENCRYPTIONKEYROOT";

    /** */
    String ENV_LOCAL_SIGN_PACKAGE_ROOT = "LOCAL_SIGNPACKAGEROOT";

    /** */
    String ENV_LOCAL_SIGN_KEYS_AND_PACKAGE_ROOT = "LOCAL_SIGNKEYSANDPACKAGESROOT";

    /**
     * Connects to signer service
     * 
     * @param input
     *            Input stream for reading protocol packets
     * @param output
     *            Output stream for writing protocol packets
     * @throws SignerServiceException
     *             If a signer service related error occurred
     */
    void connect(InputStream input, OutputStream output) throws SignerServiceException;

    /**
     * Disconnects from the signer service, freeing any resources allocated during the session.
     */
    void disconnect();

    /**
     * Get a sign package from the remote signer service.
     * 
     * @param alias
     *            Alias for sign package
     * @param enableHelloHandshake
     *            true to perform handshake, false to just reauthorize
     * @return An interface to a sign package instance
     * @throws SignerServiceException
     *             If a signer service related error occurred
     */
    ISignPackage getSignPackage(String alias, boolean enableHelloHandshake) throws SignerServiceException;

    /**
     * Get a sign package from the remote signer service.
     *
     * @param pSignKeyPath
     *            sign key path
     * @param enableHelloHandshake
     *            true to perform handshake, false to just reauthorize
     * @throws SignerServiceException
     *             If a signer service related error occurred
     */
    void setSignKeyPath(String pSignKeyPath, boolean enableHelloHandshake) throws SignerServiceException;

    /**
     * Add a listener that receives notifications from the signer service
     * 
     * @param listener
     *            Listener to add
     */
    void addListener(ISignerServiceListener listener);

    /**
     * Remove specified listener from signer service object
     * 
     * @param listener
     *            Listener to remove
     */
    void removeListener(ISignerServiceListener listener);

    /**
     * Signs specified data and returns the signature
     * 
     * @param data
     *            Data to be signed
     * @return Signature of signed data
     * @throws SignerServiceException
     *             If a signer service related error occurred
     */
    byte[] signA2(byte[] data) throws SignerServiceException;

    /**
     * Signs specified data and returns the signature
     * 
     * @param data
     *            Data to be signed
     * @return Signature of signed data
     * @throws SignerServiceException
     *             If a signer service related error occurred
     */
    byte[] signU8500(byte[] data) throws SignerServiceException;

    /**
     * Signs specified data and returns the signature
     * 
     * @param data
     *            Data to be signed
     * @param signatureHashTypeAlgoritm signature Hash TypeAlgoritm
     * @param signatureType type of signature
     * @return Signature of signed data
     * @throws SignerServiceException
     *             If a signer service related error occurred
     */
    byte[] signU5500(byte[] data, int signatureHashTypeAlgoritm, SignatureType signatureType) throws SignerServiceException;

    /**
     * Signs specified data and returns the signature
     * 
     * @param data
     *            Data to be signed
     * @return Signature of signed data
     * @throws SignerServiceException
     *             If a signer service related error occurred
     */
    byte[] signX509(byte[] data) throws SignerServiceException;

    /**
     * Method to configure the signer service object
     * 
     * @throws SignerServiceException
     *             If a signer service related error occurred
     */
    void configure() throws SignerServiceException;

    /**
     * @param chipid
     *            Data to sign.
     * @param key
     *            The session key to encrypt.
     * @throws Exception
     *             if something goes wrong.
     * @return encrypted session key.
     */
    byte[] encryptSessionKey(int chipid, int[] key) throws Exception;

    /**
     * @throws SignerServiceException
     *             If a signer service related error occurred
     * @return MESPConfiguration instance
     */
    ISignPackageInformation[] getSignPackagesInformation() throws SignerServiceException;

    /**
     * @param enableHelloHandshake
     *            True allow hello handshake with sign server, false disable it.
     * @param includeSubPackages
     *            true to include sub packages, else false
     * 
     * @throws SignerServiceException
     *             If a signer service related error occurred
     * @return MESPConfiguration instance
     */
    ISignPackageInformation[] getSignPackagesInformation(boolean enableHelloHandshake, boolean includeSubPackages)
        throws SignerServiceException;

    /**
     * 
     * @param parent
     *            the parent package alias
     * @return a hashmap containing all sub packages
     * @throws SignerServiceException
     *             on errors
     */
    HashMap<String, String> getSubPackages(String parent) throws SignerServiceException;

    /**
     * Get a (sub) sign package alias for specified parent package alias and that matches the
     * specified exponent and modulus typically found in a certificate.
     * 
     * @param parentPackageAlias
     *            Sign package alias for parent sign package
     * @param exponent
     *            The exponent of the public key to search for
     * @param modulus
     *            The big-endian representation of the modulus of the public key to search for
     * @param encrRoot
     *            encrRoot
     * @throws SignerServiceException
     *             on errors
     * @return The alias of a sign package that matches the parent alias, exponent and modulus
     */
    Object getSubPackageAlias(String parentPackageAlias, int exponent, byte[] modulus, String encrRoot) throws SignerServiceException;
}
