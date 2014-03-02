package com.stericsson.sdk.signing.signerservice.local;

import java.io.File;
import java.io.FileNotFoundException;
import java.nio.ByteBuffer;
import java.util.HashMap;

import org.apache.log4j.Logger;

import com.stericsson.sdk.common.HexUtilities;
import com.stericsson.sdk.common.configuration.mesp.MESPConfiguration;
import com.stericsson.sdk.common.configuration.mesp.MESPConfigurationRecord;

/**
 * Class for handling encryption keys
 * 
 * @author xolabju
 * 
 */
public final class EncryptionKeyManager {

    /**
     * Size (bytes) of chip ID.
     */
    private static final int CHIP_ID_SIZE = 4;

    /**
     * Size (bytes) of keys.
     */
    private static final int KEY_SIZE = 4;

    /**
     * Size (bytes) of key buffer.
     */
    private static final int KEY_BUFFER_SIZE = 16;

    private static EncryptionKeyManager cInstance;

    /**
     * datastructure that contains the encryptedKeys.
     */
    private HashMap<Integer, ChipClass> chipClassMap;

    private static Logger logger = Logger.getLogger(EncryptionKeyManager.class.getName());;

    private EncryptionKeyManager() {
    }

    /**
     * Returns an instance of this class
     * 
     * @return an instance of EncryptionKeyManager
     */
    public static EncryptionKeyManager getInstance() {
        if (cInstance == null) {
            cInstance = new EncryptionKeyManager();
        }
        return cInstance;
    }

    /**
     * 
     * @param keyRoot
     *            the base directory of the encryption keys
     * @throws FileNotFoundException
     *             if the directory wasn't found
     */
    public void init(File keyRoot) throws FileNotFoundException {

        if (keyRoot == null) {
            throw new FileNotFoundException("Failed to locate encryption keys directory. keyroot is null");
        } else if (!keyRoot.exists() || !keyRoot.isDirectory()) {
            throw new FileNotFoundException("Failed to locate encryption keys directory " + keyRoot.getAbsolutePath());
        }
        logger.info("Initiating encryption keys. Root = " + keyRoot.getAbsolutePath());
        chipClassMap = new HashMap<Integer, ChipClass>();

        readEncryptedKeys(keyRoot); // find all the chipkeys
    }

    private void readEncryptedKeys(final File file) {
        if (!file.isDirectory()) {
            // the file points right to one file (initially)
            readEncryptionKeyContent(file);
        } else {
            // if it is a directory, we look through it and search for
            // suitable files and look throught other directory siblings
            File[] files = file.listFiles();
            for (int i = 0; i < files.length; i++) {
                if (files[i].isDirectory()) { // is it a directory?
                    readEncryptedKeys(files[i]);
                } else if (files[i].isFile()) { // is it a file?
                    readEncryptionKeyContent(files[i]);
                }
            }
        }
    }

    private void readEncryptionKeyContent(final File file) {
        try {
            MESPConfiguration config = MESPConfiguration.read((file));
            MESPConfigurationRecord[] allRecords = config.getAllRecords();
            if (allRecords == null) {
                return;
            }
            for (int i = 0; i < allRecords.length; i++) {
                // get one entry from the collection of mesp files
                MESPConfigurationRecord encryptionKeyEntry = allRecords[i];
                // get the chipclass entry
                String chipClassBuffer = encryptionKeyEntry.getValue("ChipClass");
                int chipClassNumber = 0;
                // search and decode the chipclass property
                if (chipClassBuffer == null) {
                    throw new IllegalArgumentException("Property \"ChipClass\" is not defined");
                } else {
                    chipClassNumber = new Integer(chipClassBuffer).intValue();
                }
                int cidNumber = 0;
                String cidBuffer = encryptionKeyEntry.getValue("Cid");
                if (cidBuffer == null) { // search and decode the cid
                    // property
                    throw new IllegalArgumentException("Property \"Cid\" is not defined");
                } else {
                    cidNumber = new Integer(cidBuffer).intValue();
                }
                int chipIdNumber = 0;
                // reads the chipid value
                String chipIdBuffer = encryptionKeyEntry.getValue("ChipId");
                if (chipIdBuffer == null) {
                    // search and decode the chipid property
                    throw new IllegalArgumentException("Property \"ChipId\" is not defined");
                } else {
                    byte[] chipIdIntBuffer = HexUtilities.toByteArray(chipIdBuffer, CHIP_ID_SIZE);
                    chipIdNumber = ByteBuffer.wrap(chipIdIntBuffer).getInt(0);
                }
                String alias = "";
                // reads the alias value
                String chipAliasBuffer = encryptionKeyEntry.getValue("Alias");

                if (chipAliasBuffer == null) {
                    // search and decode the chipid property
                    // Use chipIdNumber if alias is not available
                    alias = "0x" + Integer.toHexString(chipIdNumber);
                } else {
                    alias = chipAliasBuffer;
                }
                int[] key = new int[KEY_SIZE];
                String chipKeyBuffer = encryptionKeyEntry.getValue("ChipKey");
                // search and decode the key property
                if (chipKeyBuffer == null) {
                    throw new IllegalArgumentException("Property \"ChipKey\" is not defined");
                } else {
                    byte[] chipKeyIntBuffer = HexUtilities.toByteArray(chipKeyBuffer, KEY_BUFFER_SIZE);
                    byte[] temporaryKey = new byte[KEY_SIZE];
                    for (int j = 0; j < KEY_SIZE; j++) {
                        System.arraycopy(chipKeyIntBuffer, j * KEY_SIZE, temporaryKey, 0, KEY_SIZE);
                        key[j] = ByteBuffer.wrap(temporaryKey).getInt(0);
                    }
                }
                addEncryptionKey(chipClassNumber, cidNumber, chipIdNumber, alias, key);
            }
        } catch (Exception error) {
            logger.warn("Error parsing the chipkey " + file.getAbsolutePath() + ": " + error);
        }

    }

    /**
     * returns the ChipClass specified by the chipClassId.
     * 
     * @param chipClassId
     *            the class ID of the chip
     * @return the requested ChipClass
     */
    public ChipClass getEncryptionKey(final int chipClassId) {
        return chipClassMap.get(chipClassId);
    }

    private void addEncryptionKey(final int chipClassId, final int cidId, final int chipId, final String alias,
        final int[] encryptionKey) {
        ChipClass chipClass = chipClassMap.get(chipClassId);
        if (chipClass == null) {
            chipClass = new ChipClass(chipClassId);
            chipClassMap.put(chipClassId, chipClass);
        }
        chipClass.addCid(cidId);
        Cid cid = chipClass.getCid(cidId);
        cid.addChipId(chipId, alias, encryptionKey);
    }
}
