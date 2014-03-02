package com.stericsson.sdk.signing.cli.loadmodules;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import com.stericsson.sdk.signing.AbstractSigner;
import com.stericsson.sdk.signing.ISignerSettings;
import com.stericsson.sdk.signing.SignerException;
import com.stericsson.sdk.signing.cli.ISign;

/**
 * @author xtomzap
 * 
 */
public class LoadModulesSigner extends AbstractSigner {

    /** */
    private List<String> fileList;

    private List<LoadModuleEntry> loadModules;

    private Map<String, LoadModuleSignEntry> fileSignConfiguration;

    private static final String FILELIST_COMMENT_PREFIX = "#";

    /**
     * Constructor
     */
    public LoadModulesSigner() {
        fileSignConfiguration = new HashMap<String, LoadModuleSignEntry>();
    }

    /**
     * {@inheritDoc}
     */
    public void sign(ISignerSettings settings) throws SignerException {

        String fileListName = (String) settings.getSignerSetting(ISignerSettings.KEY_INPUT_FILE);
        String configFileName = (String) settings.getSignerSetting(LoadModulesSignerSettings.KEY_CONFIGURATION);
        String outputFolderName = (String) settings.getSignerSetting(LoadModulesSignerSettings.KEY_OUTPUT_FOLDER);

        try {
            checkOutputFolder(outputFolderName);
            parseFileList(fileListName);
            parseConfigFile(configFileName);
            matchFileList();

            ISign sign = (ISign) settings.getSignerSetting(LoadModulesSignerSettings.KEY_SIGN);

            for (String filePath : fileSignConfiguration.keySet()) {
                LoadModuleSignEntry actualSignEntry = fileSignConfiguration.get(filePath);
                String fullOutPathName = outputFolderName + System.getProperty("file.separator") + new File(filePath).getName();
                List<String> argumentsList = new ArrayList<String>();

                argumentsList.addAll(actualSignEntry.getArguments());
                argumentsList.add(filePath);
                argumentsList.add(fullOutPathName);

                String[] args = new String[argumentsList.size()];
                argumentsList.toArray(args);

                int result = sign.execute(args);
                if (result != 0) {
                    return;
                }
            }

        } catch (Exception e) {
            throw new SignerException(e);
        }
    }

    private void checkOutputFolder(String outputFolderName) throws IOException {
        File outputFolder = new File(outputFolderName);
        if (!outputFolder.exists()) {
            boolean success = outputFolder.mkdirs();
            if (!success) {
                throw new IOException("Cannot create " + outputFolderName + " directory");
            }
        }
    }

    private void parseConfigFile(String configFileName) throws SignerException {
        LoadModulesConfigurationReader reader = new LoadModulesConfigurationReader(configFileName);
        loadModules = reader.getLoadModules();
    }

    private void matchFileList() throws SignerException {
        boolean found = false;
        for (String filePath : fileList) {
            for (LoadModuleEntry loadModuleEntry : loadModules) {
                for (String configFileName : loadModuleEntry.getSourceList()) {
                    if (filesMatched(filePath, configFileName)) {
                        fileSignConfiguration.put(filePath, loadModuleEntry.getSignEntry());
                        found = true;
                        break;
                    }
                }
                if (found) {
                    break;
                }
            }
            if (!found) {
                throw new SignerException("Load module settings for " + filePath
                    + " could not be found in configuration file.");
            }
            found = false;
        }
    }

    private boolean filesMatched(String pListFileName, String pConfigFileName) {
        return new File(pListFileName).getName().equalsIgnoreCase(pConfigFileName);
    }

    private void parseFileList(String fileListName) throws IOException {
        BufferedReader lineReader = null;
        fileList = new ArrayList<String>();

        try {
            lineReader = new BufferedReader(new InputStreamReader(new FileInputStream(fileListName), "UTF-8"));
            String line = null;
            while ((line = lineReader.readLine()) != null) {
                if (!line.startsWith(FILELIST_COMMENT_PREFIX) && !line.trim().equalsIgnoreCase("")) {
                    fileList.add(line);
                }
            }
        } finally {
            lineReader.close();
        }
    }
}
