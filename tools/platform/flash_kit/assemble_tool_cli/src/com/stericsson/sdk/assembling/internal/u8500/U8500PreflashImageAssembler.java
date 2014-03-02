package com.stericsson.sdk.assembling.internal.u8500;

import java.io.BufferedReader;
import java.io.File;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.Iterator;
import java.util.ListIterator;
import java.util.Locale;

import com.stericsson.sdk.assembling.AssemblerException;
import com.stericsson.sdk.assembling.IAssemblerSettings;
import com.stericsson.sdk.assembling.internal.ConfigurationReader;
import com.stericsson.sdk.assembling.internal.Profile;
import com.stericsson.sdk.assembling.utilities.ContentTypeUtilities;
import com.stericsson.sdk.assembling.utilities.StreamCloser;

/**
 * Class that assembles a preflash image (S-Records) from a flash archive and an optional gdfs file
 *
 * @author xolabju
 *
 */
public class U8500PreflashImageAssembler extends U8500Assembler {

    private Profile profile = null;

    private static final String PREFLASH_OUTPUT_FILE = "preflash.bin";

    private void init(final String configuration, final String fileList) throws AssemblerException {
        U8500ConfigurationReader parser =
            new U8500ConfigurationReader(configuration, ConfigurationReader.ELEMENT_NAME_PREFLASH_IMAGE);
        entries = parser.getEntries();
        profile = parser.getProfile();
        if (profile == null) {
            throw new AssemblerException("Failed to read profile from configuration");
        }

        try {
            entryFileList = ConfigurationReader.getFileListFilenames(fileList);
        } catch (IOException e) {
            throw new AssemblerException(e);
        }
    }

    /**
     * Creates a RAM image from a (in the constructor) parsed configuration file.
     *
     * @param settings
     *            the assembler settings
     * @throws AssemblerException
     *             In case of an assembler error.
     */
    public void assemble(IAssemblerSettings settings) throws AssemblerException {
        String confFile = (String) settings.get(IAssemblerSettings.KEY_CONFIGURATION_FILE);
        String fileList = (String) settings.get(IAssemblerSettings.KEY_FILE_LIST_FILE);
        String outputFile = (String) settings.get(IAssemblerSettings.KEY_OUTPUT_FILE);

        init(confFile, fileList);

        ListIterator<U8500ConfigurationEntry> i;

        try {
            matchFileList();

            notifyMessage("Creating Motorola S-Records...");

            notifyMessage("Locating external preflash tool...");
            File preflashTool = locatePreflashTool();
            if (preflashTool == null || !preflashTool.isFile()) {
                throw new AssemblerException(
                    "Failed to locate preflash tool. Make sure that the profile entered in the configuration file "
                        + "contains a valid path to the preflash tool. (e.g " + Profile.PRE_FLASH_TOOL + " "
                        + Profile.PATH_WINDOWS + "=\"c:\\path_to\\preflash_tool.exe\" " + Profile.PATH_LINUX
                        + "=\"/path_to/preflash_tool\"" + ")");
            }

            notifyMessage("Locating GDFS entry...");
            String gdfs = extractGDFS();

            notifyMessage("Calling external preflash tool...");

            String cmdPrefix = getCmdPrefix();

            i = entries.listIterator();
            while (i.hasNext()) {
                U8500ConfigurationEntry entry = i.next();
                ListIterator<String> j = entryFileList.listIterator();
                while (j.hasNext()) {
                    String filename = j.next();
                    String symbolicFilename = extractSymbolicFilename(filename);
                    String pathToFile = extractPathToFile(filename);

                    if (symbolicFilename == null) {
                        if (new File(filename).getName().equalsIgnoreCase(entry.getSource())) {
                            executeCommand(cmdPrefix + preflashTool.getAbsolutePath() + " "
                                + new File(pathToFile).getAbsolutePath() + " " + (gdfs == null ? "" : gdfs));
                        }
                    } else if (symbolicFilename.equalsIgnoreCase(entry.getSource())) {
                        executeCommand(cmdPrefix + preflashTool.getAbsolutePath() + " "
                            + new File(pathToFile).getAbsolutePath() + " " + (gdfs == null ? "" : gdfs));
                    }
                }
            }
            File preflashFile = new File(PREFLASH_OUTPUT_FILE);
            preflashFile.renameTo(new File(outputFile));
            notifyMessage("Preflash image " + outputFile + " sucessfully created!");
        } catch (Exception e) {
            throw new AssemblerException(e);
        }
    }

    private void executeCommand(String cmd) throws AssemblerException {
        notifyMessage("Executing command: " + cmd);
        int result;
        Runtime rt = Runtime.getRuntime();
        BufferedReader processReader = null;
        try {
            StringBuffer sBuffer = new StringBuffer();

            String processLine;
            String separator = System.getProperty("line.separator");
            Process exec = null;

            exec = rt.exec(cmd);

            processReader = new BufferedReader(new InputStreamReader(exec.getInputStream(), "UTF-8"));
            while ((processLine = processReader.readLine()) != null) {
                sBuffer.append(processLine + separator);
            }
            try {
                result = exec.waitFor();
                if (result != 0 || sBuffer.toString().contains("Error:")) {
                    throw new AssemblerException(
                        "Failed to created Motorola S-Records from input: Preflash tool returned " + result + "\n"
                            + sBuffer.toString());
                } else {
                    notifyMessage(sBuffer.toString());
                }

            } catch (InterruptedException e) {
                throw new AssemblerException(e.getMessage());
            }
        } catch (IOException e) {
            throw new AssemblerException(e.getMessage());
        } finally {
            StreamCloser.close(processReader);
        }
    }

    private String getCmdPrefix() {
        String prefix = null;
        String os = System.getProperty("os.name");
        if (os != null && os.toLowerCase(Locale.getDefault()).contains("windows")) {
            prefix = "cmd /c ";
        } else {
            prefix = "";
        }
        return prefix;
    }

    private String extractGDFS() throws AssemblerException {
        boolean found = false;
        String filename = null;
        Iterator<U8500ConfigurationEntry> entryIterator = entries.iterator();
        Iterator<String> filelistIterator = entryFileList.iterator();
        while (entryIterator.hasNext()) {
            U8500ConfigurationEntry entry = entryIterator.next();
            if (ContentTypeUtilities.checkContentSubType(entry.getType(), ConfigurationReader.SUBTYPE_GDFS)) {
                while (filelistIterator.hasNext()) {
                    String next = filelistIterator.next();
                    String symbolicName = extractSymbolicFilename(next);
                    if (symbolicName == null && new File(next).getName().equals(entry.getSource())) {
                        found = true;
                        filename = next;
                    } else if (symbolicName != null && entry.getSource().equalsIgnoreCase(symbolicName)) {
                        found = true;
                        filename = extractPathToFile(next);
                    }
                    if (found) {
                        filelistIterator.remove();
                        entryIterator.remove();
                        return new File(filename).getAbsolutePath();
                    }
                }
            }
        }
        return null;
    }

    private File locatePreflashTool() {
        File preflashTool = null;
        String preFlashToolPath = profile.getPreFlashToolPath();
        if (preFlashToolPath != null) {
            preflashTool = new File(preFlashToolPath);
        }
        return preflashTool;
    }

}
