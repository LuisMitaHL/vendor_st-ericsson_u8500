package com.stericsson.sdk.equipment.m7x00.bsu;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.channels.FileChannel;
import java.util.Collection;
import java.util.LinkedList;

import org.apache.log4j.Logger;

import com.stericsson.sdk.common.EnvironmentProperties;
import com.stericsson.sdk.common.FileUtils;
import com.stericsson.sdk.equipment.io.port.IPort;
import com.stericsson.sdk.equipment.m7x00.Activator;
import com.stericsson.sdk.equipment.m7x00.internal.M7x00Equipment;
import com.stericsson.sdk.loader.communication.ILoaderCommunicationListener;
import com.stericsson.sdk.loader.communication.LCDefinitions;

/**
 * Utility class for BSU compatibility checking. The functionality covers:
 * <ul>
 * <li>fetching load modules from a device</li>
 * <li>performing a compatibility checks between given load module and modules installed on a device
 * </ul>
 * 
 * @author xadazim
 * 
 */
public enum BSUUtils {

    /**
     * The only instance.
     */
    INSTANCE;

    private static final String ENCODING = "UTF-8";

    private static final String LINE_SEPARATOR = "\n";

    private static final Logger log = Logger.getLogger(BSUUtils.class);

    /**
     * Enum containing all the paths on ME that shall be scanned when looking for load modules.
     * 
     * @author xadazim
     */
    public enum BSUPath {
        /**
         * PSU load modules path.
         */
        PSU(":/boot/"),

        /**
         * DPU load modules path.
         */
        DPU(":/bin/");

        private String path;

        /**
         * Returns path on the ME. The path contains trailing "/" (always ends with "/").
         * 
         * @return path on the ME
         */
        public String getPath() {
            return path;
        }

        private BSUPath(String pPath) {
            path = pPath;
        }
    }

    /**
     * Results of BSU host manager (BSUHostManager.exe) invocation.
     */
    public static final class BSUResult {
        private String text;

        private int exitCode;

        /**
         * Constructs bsu host manager process results object.
         * 
         * @param pText
         *            bsu host manager standard output text
         * @param pExitCode
         *            bsu host manager exit code
         */
        public BSUResult(String pText, int pExitCode) {
            text = pText;
            exitCode = pExitCode;
        }

        /**
         * Gets standard output of bsu host manager process.
         * 
         * @return standard output of bsu host manager process
         */
        public String getText() {
            return text;
        }

        /**
         * Gets exit code of bsu host manager process.
         * 
         * @return exit code of bsu host manager process
         */
        public int getExitCode() {
            return exitCode;
        }
    }

    /**
     * Fetches information about load modules installed in all the paths given in BSUPath enum.
     * 
     * @see BSUUtils#getLoadModules(M7x00Equipment, String)
     * @see BSUUtils.BSUPath
     * 
     * @param pEquipment
     *            equipment from which load modules shall be fetched
     * @return collection of load modules descriptions
     * @throws Exception
     *             in case of LCD error or parsing error
     */
    public static Collection<LoadModuleDescription> getLoadModules(M7x00Equipment pEquipment) throws Exception {
        LinkedList<LoadModuleDescription> result = new LinkedList<LoadModuleDescription>();
        for (BSUPath p : BSUPath.values()) {
            result.addAll(getLoadModules(pEquipment, p.getPath()));
        }
        return result;
    }

    /**
     * Fetches information about load modules installed in given path on given equipment. Calls
     * "fileSystemReadLoadModuleManifests" LCD command to fetch the information. LCD stores this
     * information as a regular file. Only one file is created for all the load modules that are
     * installed on the ME, this file contains load module names and manifests of all the modules
     * concatenated one after another. This method reads the file, parses it and returns as a
     * collection of LoadModuleDescriptions. The file that was created on the disk is removed
     * afterwards.
     * 
     * @param pEquipment
     *            equipment from which load modules shall be fetched
     * @param pMEPath
     *            path on the equipment which should be scanned for the load modules
     * @return collection of load modules descriptions
     * @throws Exception
     *             if LCD command has failed or if the manifests file that was created could not be
     *             parsed
     */
    public static Collection<LoadModuleDescription> getLoadModules(M7x00Equipment pEquipment, String pMEPath)
        throws Exception {
        Collection<LoadModuleDescription> result;
        File manifestsFile = null;
        try {
            manifestsFile = File.createTempFile("manifestsFile", "");

            fetchManifestsFile(pEquipment, manifestsFile, pMEPath);
            result = parseManifestsFile(manifestsFile);
        } finally {
            if (manifestsFile != null) {
                manifestsFile.delete();
            }
        }

        return result;
    }

    /**
     * Checks compatibility of given load module with load modules that are installed on ME. The
     * compatibility is verified with the usage of external tool "BSUHostManager.exe".
     * 
     * @param pEquipment
     *            ME that is to be scanned for installed load modules against which the
     *            compatibility should be checked
     * @param pLoadModule
     *            load module which compatibility should be checked
     * @return true if modules are compatible, false otherwise
     * @throws Exception
     *             in case manifests could not be fetched from ME or could not be parsed or
     *             BSUHostManager.exe could not be called or given load module could not be read
     * 
     */
    public static BSUResult checkCompatibility(M7x00Equipment pEquipment, File pLoadModule) throws Exception {
        if (!pLoadModule.canRead()) {
            throw new Exception("Load module file " + pLoadModule.getAbsolutePath() + " could not be read");
        }

        // fetch manifests from ME, parse and split
        Collection<LoadModuleDescription> installedModules = getLoadModules(pEquipment);

        // create temp folder and store in it manifests of the load modules that are installed on ME
        File tmpDir = null;
        StringBuilder bsuArgs = new StringBuilder();
        bsuArgs.append(pLoadModule + " ");
        try {
            tmpDir = FileUtils.createTmpDir("me_load_modules", "");
            for (LoadModuleDescription desc : installedModules) {
                File f = new File(tmpDir, desc.getModuleName() + ".xml");
                bsuArgs.append("\"" + f.getAbsolutePath() + "\" ");
                desc.getManifest().writeToFile(f);
            }
        } finally {
            if (tmpDir != null) {
                try {
                    FileUtils.removeFilesInDir(tmpDir);
                } catch (IOException ex) {
                    log.warn("Could not remove temporary directory", ex);
                }
                tmpDir.delete();
            }
        }

        // call bsu host manager
        BSUResult result = callBSUHostManager(bsuArgs.toString());

        // return bsu results
        return result;
    }

    /**
     * Parses manifests file created by LCD. This file is created by
     * "fileSystemReadLoadModuleManifests" LCD command.
     * 
     * @param pManifestsFile
     *            file to be parsed
     * @return contents of the file as collection of LoadModuleDescriptions
     * @throws Exception
     *             in case of parsing error
     */
    private static Collection<LoadModuleDescription> parseManifestsFile(File pManifestsFile) throws Exception {
        LinkedList<LoadModuleDescription> descriptions = new LinkedList<LoadModuleDescription>();

        ByteBuffer bb = ByteBuffer.allocate((int) pManifestsFile.length());
        bb.order(ByteOrder.LITTLE_ENDIAN);

        FileChannel inputChannel = null;
        FileInputStream inputStream = null;
        try {
            inputStream = new FileInputStream(pManifestsFile);
            inputChannel = inputStream.getChannel();
            inputChannel.read(bb);
        } finally {
            if (inputChannel != null) {
                inputChannel.close();
            }
            if (inputStream != null) {
                inputStream.close();
            }
        }

        bb.rewind();

        while (bb.remaining() > 0) {
            // read name size
            int nameSize = bb.getInt();

            // read name
            byte[] nameBytes = new byte[nameSize];
            bb.get(nameBytes);
            String name = new String(nameBytes, ENCODING);

            // read manifest size
            int manifestSize = bb.getInt();

            // read manifest
            byte[] manifestBytes = new byte[manifestSize];
            bb.get(manifestBytes);

            LoadModuleDescription descr = new LoadModuleDescription(name, new LoadModuleManifest(manifestBytes));
            descriptions.add(descr);
        }

        return descriptions;
    }

    /**
     * Creates a file containing all manifests from load modules installed on ME in given directory.
     * Calls "fileSystemReadLoadModuleManifests" LCD command.
     * 
     * @param pEquipment
     *            equipment from which manifests shall be fetched
     * @param pManifestsFile
     *            where to store fetched manifests
     * @param pMEPath
     *            path on ME which should be scanned for load modules (containing manifests)
     * @throws Exception
     *             if LCD command has failed
     */
    private static void fetchManifestsFile(M7x00Equipment pEquipment, File pManifestsFile, String pMEPath)
        throws Exception {
        pManifestsFile.createNewFile(); // get manifests from ME
        Object resultObject =
            pEquipment.sendCommand(LCDefinitions.METHOD_NAME_FS_READ_LOAD_MODULE_MANIFESTS, new Object[] {
                pManifestsFile.getAbsolutePath(), pMEPath}, new ILoaderCommunicationListener() {
                public void loaderCommunicationProgress(long pTotalBytes, long pTransferredBytes) {
                }

                public void loaderCommunicationMessage(String pTitle, String pMessage, boolean pLogOnly) {
                }

                public void loaderCommunicationError(IPort pPort, String pMessage) {
                }
            });
        if (resultObject == null) {
            throw new Exception("Could not invoke " + LCDefinitions.METHOD_NAME_FS_READ_LOAD_MODULE_MANIFESTS
                + " method");
        }
        Integer result = (Integer) resultObject;
        if (result != 0) {
            throw new Exception("Reading of load module manifests has failed ("
                + LCDefinitions.METHOD_NAME_FS_READ_LOAD_MODULE_MANIFESTS + " LCD command), "
                + pEquipment.getLoaderCommunicationService().getLoaderErrorDesc(result));
        }
    }

    /**
     * Calls BSUHostManager.exe, returns results of the process.
     * 
     * @param pArguments
     *            runtime arguments that should be passed to BSUHostManager.exe
     * @return result code of the process and standard output
     * @throws Exception
     *             if BSUHostManager could not be executed
     */
    private static BSUResult callBSUHostManager(String pArguments) throws Exception {

        if ((!EnvironmentProperties.isRunningInTestMode()) && (!EnvironmentProperties.isWindows())) {
            throw new Exception("BSUHostManager can be run only on Windows.");
        }

        Process process;
        int result;
        BufferedReader processReader;
        String processLine;

        String command = Activator.getBSUExeFile().getAbsolutePath() + " " + pArguments;
        log.info("Starting BSU host manager: " + command);
        if (EnvironmentProperties.isRunningInTestMode() && (!EnvironmentProperties.isWindows())) {
            return new BSUResult("File compatibility.txt result: Compatibility check failed.", 0);
        } else {
            process = Runtime.getRuntime().exec(command);
            processReader = new BufferedReader(new InputStreamReader(process.getInputStream(), ENCODING));

            StringBuilder outputBuilder = new StringBuilder();
            while ((processLine = processReader.readLine()) != null) {
                outputBuilder.append(processLine + LINE_SEPARATOR);
            }
            result = process.waitFor();
            return new BSUResult(outputBuilder.toString(), result);
        }
    }
}
