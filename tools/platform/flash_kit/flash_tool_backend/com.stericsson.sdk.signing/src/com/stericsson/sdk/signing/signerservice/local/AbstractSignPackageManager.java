package com.stericsson.sdk.signing.signerservice.local;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.math.BigInteger;
import java.security.interfaces.RSAPrivateCrtKey;
import java.security.interfaces.RSAPrivateKey;
import java.util.Arrays;

import com.stericsson.sdk.signing.a2.A2SignPackage;
import com.stericsson.sdk.signing.generic.GenericSignPackage;
import com.stericsson.sdk.signing.signerservice.ISignPackage;
import com.stericsson.sdk.signing.signerservice.local.keys.KeyParser;
import com.stericsson.sdk.signing.tvp.TVPSignPackage;

/**
 * Abstract class for SignPackageManager
 * @author xxvs0002
 *
 */
public abstract class AbstractSignPackageManager{

    private static SignPackageManager cInstanceSignPackageManager;
    private static SignKeysPackageManager cInstanceSignKeysPackageManager;
    /**
     * Returns an instance of this class
     * @param useCommonFolder useCommonFolder
     * @return an instance of SignPackageManager if use mirror structure for keys and packages or 
     *          return instance of SignKeysPackageManager if use common folder where keys and packages are stored
     */
    public static AbstractSignPackageManager getInstance(boolean useCommonFolder) {
        if(useCommonFolder){
            if (cInstanceSignKeysPackageManager == null) {
                cInstanceSignKeysPackageManager = new SignKeysPackageManager();
            }
            return cInstanceSignKeysPackageManager;
        }else{
            if (cInstanceSignPackageManager == null) {
                cInstanceSignPackageManager = new SignPackageManager();
            }
            return cInstanceSignPackageManager;
        }
    }

    /**
     * @param filename filename
     * @param sp sp
     * @return private key
     * @throws IOException IOException
     */
    protected RSAPrivateKey createKey(String filename, ISignPackage sp) throws IOException {
        if (sp instanceof A2SignPackage) {
            return createA2Key(filename);
        } else if (sp instanceof GenericSignPackage) {
            return createU5500Key(filename);
        } else if (sp instanceof TVPSignPackage) {
            return createX509Key(filename);
        }
        throw new IOException("Sign package is not a valid type");
    }

    private RSAPrivateKey createU5500Key(String filename) throws IOException {
        return KeyParser.parsePEMPrivateKey(filename);
    }

    /**
     * Creates and returns an RSAPrivateKey from a file with file name filename. The file must be a
     * text-file that is an A2 private key.
     * 
     * @param filename
     *            - The path name of the file.
     * @return The RSAPrivateKey that was created from the specified file.
     * @throws IOException
     *             if any errors occur.
     */
    private RSAPrivateKey createA2Key(String filename) throws IOException {
        return KeyParser.parseA2PrivateKey(filename);
    }

    /**
     * Creates and returns an RSAPrivateKey from a file with file name filename. The key must be a
     * PKCS#8 or PKCS#1 encoded private key.
     * 
     * @param filename
     *            - The path name of the file.
     * @return The RSAPrivateKey that was created from the specified file.
     * @throws IOException
     *             if any errors occur.
     */
    private RSAPrivateKey createX509Key(String filename) throws IOException {
        FileInputStream fis = null;
        try {
            fis = new FileInputStream(filename);
            return KeyParser.parseX509Key(fis);
        } finally {
            if (fis != null) {
                try {
                    fis.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }
    }

    /**
     * Get key
     * 
     * @param keyAlias
     *            key alias
     * @return a key for given alias
     */
    public abstract RSAPrivateKey getKey(String keyAlias);

    /**
     * @param signPackage sign package
     * @param modulus modulus of the public key
     * @param publicExponent exponent of the public key
     * @return the private key
     * @throws FileNotFoundException
     *              If the path to the keys and packages is not valid
     */
    public abstract RSAPrivateKey getKey(ISignPackage signPackage, 
            BigInteger modulus, BigInteger publicExponent) throws FileNotFoundException;
    /**
     * Get package
     * 
     * @param keyAlias
     *            key alias
     * @return a package for given alias
     */
    public abstract byte[] getPackage(String keyAlias);

    /**
     * Get package aliases
     * 
     * @return package aliases
     */
    public abstract String[] getPackageAliases();

    /**
     * Returns the path to where secret files should be placed (the same folder as the sign package)
     * for sign package.
     * 
     * @param keyAlias
     *            The alias of the Sign Package.
     * @return The path to where secret files should be placed.
     */
    public abstract String getSecretPath(String keyAlias);

    /**
     * Returns the path to where secret files should be placed (the same folder as the sign package)
     * for sign package.
     * 
     * @param keyAlias
     *            The alias of the Sign Package.
     * @return The path to where secret files should be placed.
     */
    public abstract String getKeyPath(String keyAlias);

    /**
     * @param keyRoot
     *            root directory of the key files
     * @param secretRoot
     *            root directory of the secret files
     * @throws FileNotFoundException
     *             if any of the input directories are missing
     */
    public abstract void init(File keyRoot, File secretRoot) throws FileNotFoundException;

    /**
     * Map folders
     * 
     * @param zipRoot
     *            zip root
     * @param keyBase
     *            key base
     * @param secretBase
     *            secret base
     * @throws FileNotFoundException If the file is not found
     */
    protected abstract void mapFolders(File zipRoot, File keyBase, File secretBase) throws FileNotFoundException;


    /**
     * 
     * @param key
     *            key #1
     * @param modulus
     *            modulus of key #2
     * @param publicExponent
     *            public exponent of key #2
     * @return true if the keys are equal, else false
     */
    static boolean compareKeys(RSAPrivateKey key, BigInteger modulus, BigInteger publicExponent) {
        if (key != null && key instanceof RSAPrivateCrtKey) {
            if (modulus != null && publicExponent != null) {
                RSAPrivateCrtKey crtKey = (RSAPrivateCrtKey) key;
                byte[] byteArray = crtKey.getModulus().toByteArray();
                byte[] unsignedByteArray = new byte[byteArray.length - 1];
                System.arraycopy(byteArray, 1, unsignedByteArray, 0, unsignedByteArray.length);
                return Arrays.equals(unsignedByteArray, modulus.toByteArray())
                    && publicExponent.equals(crtKey.getPublicExponent());
            }
        }
        return false;
    }

    /**
     * @param signPackage signPackage
     * @param modulus modulus
     * @param publicExponent publicExponent
     * @return GenericSignPackage
     * @throws FileNotFoundException FileNotFoundException
     */
    public abstract boolean getSignPackage(ISignPackage signPackage, 
            BigInteger modulus, BigInteger publicExponent) throws FileNotFoundException;
}
