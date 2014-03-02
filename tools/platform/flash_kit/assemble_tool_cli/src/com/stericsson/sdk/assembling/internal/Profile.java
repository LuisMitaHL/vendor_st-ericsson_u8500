package com.stericsson.sdk.assembling.internal;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.LinkedList;
import java.util.List;
import java.util.Locale;

import com.stericsson.sdk.assembling.AssemblerException;

/**
 * @author ezaptom
 * 
 */
public class Profile {

    /** */
    private List<MESPConfigurationRecord> profileRecords;

    /** */
    public static final String PRE_FLASH_TOOL = "PreFlashTool";

    /** */
    public static final String PATH_LINUX = "Linux";

    /** */
    public static final String PATH_WINDOWS = "Windows";

    /** */
    public static final String AT_SIGN_PROFILES = "@profiles";

    /** */
    public static final String PROFILES_ROOT = "profiles.root";

    /**
     * @param pPath
     *            path to profile
     * @throws AssemblerException
     *             if profile file doesn't exist
     * @throws IOException
     *             If an I/O error occurred
     */
    public Profile(String pPath) throws AssemblerException, IOException {
        profileRecords = new LinkedList<MESPConfigurationRecord>();
        read(pPath);
    }

    private void read(String pPath) throws AssemblerException, IOException {
        pPath = translatePath(pPath);
        BasicFileValidator.validateInputFile(pPath);

        BufferedReader reader =
            new BufferedReader(new InputStreamReader(new FileInputStream(new File(pPath)), "UTF-8"));
        String line;
        try {
            line = reader.readLine();
            while ((line != null) && (!"".equals(line))) {
                profileRecords.add(MESPConfigurationRecord.parse(line));
                line = reader.readLine();
            }
        } catch (MESPConfigurationException e) {
            IOException ioe = new IOException("Could not read profile");
            ioe.initCause(e);
            throw ioe;
        } finally {
            reader.close();
        }
    }

    /**
     * Translates path to absolute.
     * 
     * @param pPath
     *            path
     * @return absolute path
     */
    protected static String translatePath(String pPath) {
        if (pPath.startsWith(AT_SIGN_PROFILES)) {
            String profilesRoot = System.getProperty(PROFILES_ROOT);
            if (profilesRoot == null) {
                profilesRoot = "";
            }

            pPath = pPath.replace("@profiles", profilesRoot);
        }
        return pPath;
    }

    /**
     * @return path to PreFlashTool
     */
    public String getPreFlashToolPath() {
        String value = null;
        for (MESPConfigurationRecord record : profileRecords) {
            if (PRE_FLASH_TOOL.equalsIgnoreCase(record.getName())) {
                String os = System.getProperty("os.name");
                value =
                    record
                        .getValue(os.toLowerCase(Locale.getDefault()).contains("windows") ? PATH_WINDOWS : PATH_LINUX);
                if (value != null) {
                    break;
                }
            }
        }
        return value;
    }
}
