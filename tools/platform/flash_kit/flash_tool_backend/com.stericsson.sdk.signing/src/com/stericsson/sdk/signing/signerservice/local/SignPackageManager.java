package com.stericsson.sdk.signing.signerservice.local;

import java.io.File;
import java.io.FileFilter;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.math.BigInteger;
import java.security.interfaces.RSAPrivateKey;
import java.util.Collections;
import java.util.Enumeration;
import java.util.Vector;
import java.util.zip.ZipFile;

import org.apache.log4j.Logger;

import com.stericsson.sdk.signing.signerservice.ISignPackage;
import com.stericsson.sdk.signing.signerservice.SignPackageFactory;
import com.stericsson.sdk.signing.signerservice.local.keys.Key2Path;

/**
 * Sign Package Manager class
 */
public final class SignPackageManager extends AbstractSignPackageManager{
//    private static SignPackageManager cInstance;

    private Vector<Key2Path> mKeyMappings;

    private static Logger logger = Logger.getLogger(SignPackageManager.class.getName());

    /**
     * 
     */
    public SignPackageManager() {
    }

    /**
     * Get key
     * 
     * @param keyAlias
     *            key alias
     * @return a key for given alias
     */
    public RSAPrivateKey getKey(String keyAlias) {
        Enumeration<Key2Path> keyMappingsEnum = mKeyMappings.elements();
        Key2Path keyAndSecret = (Key2Path) keyMappingsEnum.nextElement();
        while (keyMappingsEnum.hasMoreElements() && !keyAndSecret.getAlias().equalsIgnoreCase(keyAlias)) {
            keyAndSecret = (Key2Path) keyMappingsEnum.nextElement();
        }
        if (keyAndSecret.getAlias().equalsIgnoreCase(keyAlias)) {
            RSAPrivateKey pvk = keyAndSecret.getKey();
            return pvk;
        } else {
            return null;
        }
    }

    /**
     * Get package
     * 
     * @param keyAlias
     *            key alias
     * @return a package for given alias
     */
    public byte[] getPackage(String keyAlias) {
        Enumeration<Key2Path> keyMappingsEnum = mKeyMappings.elements();
        Key2Path keyAndSecret = (Key2Path) keyMappingsEnum.nextElement();
        while (keyMappingsEnum.hasMoreElements() && !keyAndSecret.getAlias().equalsIgnoreCase(keyAlias)) {
            keyAndSecret = (Key2Path) keyMappingsEnum.nextElement();
        }
        if (keyAndSecret.getAlias().equalsIgnoreCase(keyAlias)) {
            byte[] headerPack = keyAndSecret.getHeaderPackage();
            return headerPack;
        } else {
            return null;
        }
    }

    /**
     * Get package aliases
     * 
     * @return package aliases
     */
    public String[] getPackageAliases() {
        Vector<String> v = new Vector<String>();
        for (int i = 0; i < mKeyMappings.size(); i++) {
            v.addElement(mKeyMappings.elementAt(i).getAlias());
        }
        String[] aliases = new String[v.size()];
        v.copyInto(aliases);
        return aliases;
    }

    /**
     * Returns the path to where secret files should be placed (the same folder as the sign package)
     * for sign package.
     * 
     * @param keyAlias
     *            The alias of the Sign Package.
     * @return The path to where secret files should be placed.
     */
    public String getSecretPath(String keyAlias) {
        Enumeration<Key2Path> keyMappingsEnum = mKeyMappings.elements();
        Key2Path keyAndSecret = (Key2Path) keyMappingsEnum.nextElement();
        while (keyMappingsEnum.hasMoreElements() && !keyAndSecret.getAlias().equalsIgnoreCase(keyAlias)) {
            keyAndSecret = (Key2Path) keyMappingsEnum.nextElement();
        }
        if (keyAndSecret.getAlias().equalsIgnoreCase(keyAlias)) {
            return keyAndSecret.getSecretPath();
        } else {
            return null;
        }
    }

    /**
     * Returns the path to where secret files should be placed (the same folder as the sign package)
     * for sign package.
     * 
     * @param keyAlias
     *            The alias of the Sign Package.
     * @return The path to where secret files should be placed.
     */
    public String getKeyPath(String keyAlias) {
        for(Key2Path keyAndSecret : mKeyMappings) {
            if(keyAndSecret.getAlias().equalsIgnoreCase(keyAlias)) {
                return keyAndSecret.getKeyPath();
            }
        }
        return null;
    }

    /**
     * @param keyRoot
     *            root directory of the key files
     * @param secretRoot
     *            root directory of the secret files
     * @throws FileNotFoundException
     *             if any of the input directories are missing
     */
    public void init(File keyRoot, File secretRoot) throws FileNotFoundException {
        mKeyMappings = new Vector<Key2Path>();
        mapFolders(secretRoot, keyRoot, secretRoot);
        logger.info("Initiated local key mappings");
    }

    /**
     * Map folders
     * 
     * @param zipRoot
     *            zip root
     * @param keyBase
     *            key base
     * @param secretBase
     *            secret base
     * @throws FileNotFoundException
     *             if input folders are missing
     */
    protected void mapFolders(File zipRoot, File keyBase, File secretBase) throws FileNotFoundException {
        if (!zipRoot.isDirectory()) {
            String s = "Invalid SignPackage path: " + zipRoot.getAbsolutePath();
            throw new FileNotFoundException(s);
        }

        if (!keyBase.isDirectory()) {
            String s = "Invalid key path: " + keyBase.getAbsolutePath();
            throw new FileNotFoundException(s);
        }

        FileFilter dirs = new FileFilter() {
            public boolean accept(File f) {
                return f.isDirectory();
            }
        };

        FileFilter files = new FileFilter() {
            public boolean accept(File f) {
                return f.isFile();
            }
        };

        File[] subDirs = keyBase.listFiles(dirs);

        for (int i = 0; i < subDirs.length; i++) {
            File secretSub = new File(secretBase, subDirs[i].getName());
            if (!secretSub.isDirectory()) {
                logger.warn("Key directory " + subDirs[i].getAbsolutePath()
                    + " has no corresponding secret directory");
                continue;
            }
            mapFolders(zipRoot, subDirs[i], secretSub);
        }
        File[] keys = keyBase.listFiles(files);
        File[] zips = secretBase.listFiles(new FileFilter() {

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
        if (keys.length > 1) {
            handleTooManyKeys(keyBase, subDirs);
            return;
        } else if (keys.length < 1) {
            return;
        }
        if (zips.length < 1) {
            logger.info("No signing packages found in folder: " + secretBase.getName());
            return;
        }

        String keyPath = keys[0].getAbsolutePath();
        RSAPrivateKey pvk = null;
        ISignPackage sp;
        try {
            sp = SignPackageFactory.createSignPackage(zips[0]);
            pvk = createKey(keyPath, sp);
        } catch (Exception e) {
            logger.warn("Error parsing sign package " + zips[0].getAbsolutePath() + ": " + e.getMessage());
            return;
        }
        mapZip(zipRoot, secretBase, zips, pvk, sp, keyPath);
        Collections.sort(mKeyMappings);
    }

    /* (non-Javadoc)
     * @see com.stericsson.sdk.signing.signerservice.local.AbstractSignPackageManager#mapZip(java.io.File, java.io.File, 
     * java.io.File[], java.security.interfaces.RSAPrivateKey, com.stericsson.sdk.signing.signerservice.ISignPackage, 
     * java.lang.String)
     */
    /**
     * 
     * @param zipRoot zip Root
     * @param secretBase secret Base
     * @param zips zips
     * @param pvk pvk
     * @param sp sp
     * @param keyPath key Path
     */
    protected void mapZip(File zipRoot, File secretBase, File[] zips, RSAPrivateKey pvk, ISignPackage sp, String keyPath) {
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

                Key2Path keyAndSecret = new Key2Path(zipAlias, pvk, secretBase.getAbsolutePath(), buffer, keyPath);
                if (!mKeyMappings.contains(keyAndSecret)) {
                    mKeyMappings.addElement(keyAndSecret);
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

    private void handleTooManyKeys(File keyBase, File[] subDirs) {
        if (subDirs.length > 0) {
            logger.warn("Too many keys in folder: " + keyBase.getName() + ". Checking subfolders.");
        } else {
            logger.warn("Too many keys in folder: " + keyBase.getName() + ". Skipping folder.");
        }
    }

    /**
     * Not used
     * @param signPackage signPackage
     * @param modulus modulus
     * @param publicExponent publicExponent
     * @return null
     * @throws FileNotFoundException FileNotFoundException
     */
    @Override
    public RSAPrivateKey
            getKey(ISignPackage signPackage, BigInteger modulus, BigInteger publicExponent) throws FileNotFoundException {
        return null;
    }

    /**
     * Not used
     * @param signPackage signPackage
     * @param modulus modulus
     * @param publicExponent publicExponent
     * @return null
     * @throws FileNotFoundException FileNotFoundException
     */
    @Override
    public boolean
            getSignPackage(ISignPackage signPackage, BigInteger modulus, BigInteger publicExponent) throws FileNotFoundException {
        return false;
    }

}
