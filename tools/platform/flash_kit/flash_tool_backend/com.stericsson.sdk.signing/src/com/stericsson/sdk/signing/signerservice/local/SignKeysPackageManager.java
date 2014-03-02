package com.stericsson.sdk.signing.signerservice.local;

import java.io.File;
import java.io.FileFilter;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.math.BigInteger;
import java.security.interfaces.RSAPrivateKey;
import java.util.Arrays;
import java.util.HashMap;
import java.util.Set;
import java.util.zip.ZipFile;

import org.apache.log4j.Logger;

import com.stericsson.sdk.signing.signerservice.ISignPackage;
import com.stericsson.sdk.signing.signerservice.SignPackageFactory;
import com.stericsson.sdk.signing.signerservice.local.keys.Key2Path;


/**
 * @author xxvs0002
 *
 */
public class SignKeysPackageManager extends AbstractSignPackageManager{
//    private static SignKeysPackageManager cInstance;

    private HashMap<String, Key2Path> mPackagesMappings;
//    private HashMap<String, HashMap<String, Key2Path>> mKeysMappings;

    private static Logger logger = Logger.getLogger(SignKeysPackageManager.class.getName());
    /**
     * Empty constructor
     */
    public SignKeysPackageManager() {
    }

    /* (non-Javadoc)
     * @see com.stericsson.sdk.signing.signerservice.local.AbstractSignPackageManager#init(java.io.File, 
     * java.io.File, java.io.File)
     */
    /**
     * @param keysPackagesRoot
     *            root directory of the common folder where keys and packages are stored
     * @param notUsed notUsed
     * @throws FileNotFoundException
     *             if any of the input directories are missing
     */
    public void init(File keysPackagesRoot, File notUsed) throws FileNotFoundException {
        mPackagesMappings = new HashMap<String, Key2Path>();
        if (!keysPackagesRoot.isDirectory()) {
            String s = "Invalid SignPackagesAndKeys path: " + keysPackagesRoot.getAbsolutePath();
            throw new FileNotFoundException(s);
        }
        mapFolders(keysPackagesRoot, null, keysPackagesRoot);
        logger.info("Initiated local key mappings");
    }

    /**
     * Map folders
     * 
     * @param keysPackagesRoot
     *            keysPackagesRoot root
     * @param file1
     *            key base
     * @param subDir
     *            secret base
     * @throws FileNotFoundException
     *             if input folders are missing
     */
    protected void mapFolders(File keysPackagesRoot, File file1, File subDir) throws FileNotFoundException {
        FileFilter dirs = new FileFilter() {
            public boolean accept(File f) {
                return f.isDirectory();
            }
        };

        File[] subDirs = keysPackagesRoot.listFiles(dirs);

        for (int i = 0; i < subDirs.length; i++) {
          File subDirPath = new File(subDir, subDirs[i].getName());
          if (subDirs[i].isDirectory()) {
            mapFolders(subDirs[i], null, subDirPath);
          }
        }

        File[] zips = keysPackagesRoot.listFiles(new FileFilter() {

            public boolean accept(File f) {
                ZipFile testZip = null;
                try {
                    testZip = new ZipFile(f);
                } catch (Exception e) {
                    return false;
                } finally {
                    if (testZip != null) {
                        try {
                            testZip.close();
                        } catch (IOException e) {
                            e.getMessage();
                        }
                    }
                }
                return true;
            }
        });

        if (zips.length < 1) {
            logger.info("No signing packages found in folder: " + keysPackagesRoot.getName());
            return;
        }

        ISignPackage sp;
        try {
            sp = SignPackageFactory.createSignPackage(zips[0]);
        } catch (Exception e) {
            logger.warn("Error parsing sign package " + zips[0].getAbsolutePath() + ": " + e.getMessage());
            return;
        }
        mapZip(keysPackagesRoot, subDir, zips, null, sp, null, mPackagesMappings);
//        Collections.sort(mKeyMappings);
    }

    /**
     * Map zip.
     * @param zipRoot zip Root
     * @param subDir subDir
     * @param zips zips
     * @param pvk pvk
     * @param sp sp
     * @param hashMapToLoad hashMapToLoad
     * @param keyPath key Path
     */
    protected void mapZip(File zipRoot, File subDir, File[] zips, RSAPrivateKey pvk, ISignPackage sp, String keyPath, 
            HashMap<String, Key2Path> hashMapToLoad) {
        for (int i = 0; i < zips.length; i++) {
            FileInputStream fis = null;
            ZipFile zipper = null;
            try {
                zipper = new ZipFile(zips[i]);
                int zipBaseNameOffset = zipRoot.getAbsolutePath().length() + 1; // +1
                // for
                // removal
                // of
                // slash

                File f = new File(zips[i].getAbsolutePath());
                fis = new FileInputStream(f);
                byte[] buffer = new byte[(int) f.length()];

                int read = fis.read(buffer, 0, buffer.length);
                if (read != buffer.length) {
                    logger.warn("Error reading full contents of SignPackage: " + f.getAbsolutePath());
                    continue;
                }
                sp = SignPackageFactory.createSignPackage(zipper);
                String zipAlias = null;
                if (sp != null) {
                    zipAlias = sp.getAlias();
                }
                if (zipAlias == null || zipAlias.equals("")) {
                    zipAlias = zips[i].getAbsolutePath().substring(zipBaseNameOffset).replace('\\', '/'); // Keep
                    // it
                    // the
                    // same
                }

                Key2Path keyAndSecret = new Key2Path(zipAlias, null, subDir.getAbsolutePath(), buffer, keyPath);
                if (!hashMapToLoad.containsKey(zipAlias)) {
                    hashMapToLoad.put(zipAlias, keyAndSecret);
                }
            } catch (Exception e) {
                logger.warn(e.getMessage());
                continue;
            } finally {
                if (fis != null) {
                    try {
                        fis.close();
                    } catch (IOException e) {
                        logger.warn(e.getMessage());
                    }
                }
                if (zipper != null) {
                    try {
                        zipper.close();
                    } catch (IOException e) {
                        logger.warn(e.getMessage());
                    }
                }
            }
        }
    }

    /**
     * Returns a generic sign package for particular key
     * @param signPackage
     *          sign package
     * @param modulus
     *          modulus of the public key
     * @param publicExponent
     *          exponent of the public key
     * @return Empty Generic sign package
     * @throws FileNotFoundException 
     *          If the path to the keys and packages is not valid
     */
    public boolean getSignPackage(ISignPackage signPackage, 
            BigInteger modulus, BigInteger publicExponent) throws FileNotFoundException{

            RSAPrivateKey pvk = getKey(signPackage, modulus, publicExponent);
            if(pvk != null){
                return true;
            }
            return false;
    }

    /**
     * @param signPackage sign package
     * @param modulus modulus of the public key
     * @param publicExponent exponent of the public key
     * @return the private key
     * @throws FileNotFoundException
     *              If the path to the keys and packages is not valid
     */
    public RSAPrivateKey getKey(ISignPackage signPackage, 
            BigInteger modulus, BigInteger publicExponent) throws FileNotFoundException{

        Key2Path key2Path = mPackagesMappings.get(signPackage.getAlias());
        if(key2Path == null){
            logger.error("Package with alias " + signPackage.getAlias() + " doesn't exist");
            return null;
        }else{
            File keysDir = new File(key2Path.getSecretPath());
            if (!keysDir.isDirectory()) {
                String s = "Invalid SignPackagesAndKeys path: " + keysDir.getAbsolutePath();
                throw new FileNotFoundException(s);
            }

            FileFilter files = new FileFilter() {
                public boolean accept(File f) {
                    return f.isFile();
                }
            };

            File[] keys = keysDir.listFiles(files);

            boolean keyFound = false;

            RSAPrivateKey pvk;
            for(File keyFile : keys){
                String keyPath = keyFile.getAbsolutePath();
                try{
                    pvk = createKey(keyPath, signPackage);
                    keyFound = compareKeys(pvk, modulus, publicExponent);
                    if(keyFound){
                        return pvk;
                    }
                }catch(IOException ioe){
                    logger.warn("Error creating private key for file " + keyPath);
                }
            }

            return null;
        }
    }

    /* (non-Javadoc)
     * @see com.stericsson.sdk.signing.signerservice.local.AbstractSignPackageManager#getPackage(java.lang.String)
     */
    /**
     * @param keyAlias key Alias
     * @return a package for given alias
     */
    @Override
    public byte[] getPackage(String keyAlias) {
        Key2Path key2Path = mPackagesMappings.get(keyAlias);
        if(key2Path != null){
            return key2Path.getHeaderPackage();
        }
        return null;
    }

    /**
     * @param alias Alias of the package
     * @param exponent exponent
     * @param modulus modulus
     * @return returns the header of the package
     */
    public byte[] generatePackage(String alias, Integer exponent, byte[] modulus){

        return null;
    }

    /* (non-Javadoc)
     * @see com.stericsson.sdk.signing.signerservice.local.AbstractSignPackageManager#getPackageAliases()
     */
    /**
     * @return package aliases
     */
    @Override
    public String[] getPackageAliases() {
        Set<String> pckAliases = mPackagesMappings.keySet();
        Object[] objectArray = pckAliases.toArray();
        String[] arrayToOrder = Arrays.copyOf(objectArray, objectArray.length, String[].class);
        Arrays.sort(arrayToOrder);
        return arrayToOrder;
    }

    /**
     * @param keyAlias key Alias
     * @return The secret path
     */
    @Override
    public String getSecretPath(String keyAlias){
        Key2Path key2Path = mPackagesMappings.get(keyAlias);
        if(key2Path != null){
            return key2Path.getSecretPath();
        }
        return null;
    }

    /**
     * Not used
     * @param keyAlias keyAlias
     * @return null
     */
    @Override
    public String getKeyPath(String keyAlias) {
        return null;
    }

    /**
     * Not used
     * @param keyAlias keyAlias
     * @return null
     */
    @Override
    public RSAPrivateKey getKey(String keyAlias) {
        return null;
    }

}
