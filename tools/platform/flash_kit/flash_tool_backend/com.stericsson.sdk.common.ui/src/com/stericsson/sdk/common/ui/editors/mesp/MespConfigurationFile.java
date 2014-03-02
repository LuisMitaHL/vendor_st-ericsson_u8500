package com.stericsson.sdk.common.ui.editors.mesp;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.Reader;
import java.io.StringReader;
import java.io.UnsupportedEncodingException;
import java.util.ArrayList;
import java.util.List;

import org.eclipse.core.runtime.IProgressMonitor;

import com.stericsson.sdk.common.configuration.mesp.MESPConfigurationException;
import com.stericsson.sdk.common.configuration.mesp.MESPConfigurationRecord;

/**
 * Instance of this class represents root node for the tree in MespEditor editor overview section.
 * This node is not visible and serves as container of children nodes.
 * 
 * @author xmicroh
 * 
 */
public class MespConfigurationFile implements IMespTreeItem {

    private List<MespConfigTreeItem> configRecords;

    private File configFile;

    /**
     * @param path
     *            Absolute path to mesp configuration file.
     */
    public MespConfigurationFile(String path) {
        configFile = new File(path);
        configRecords = new ArrayList<MespConfigTreeItem>();
        parse();
    }

    /**
     * Reload file and construct new children nodes accordingly.
     */
    public void doRevertToSaved() {
        configRecords = new ArrayList<MespConfigTreeItem>();
        parse();
    }

    /**
     * Construct new children node by parsing given string instead of configuration file.
     * 
     * @param pText
     *            String to parse records.
     */
    public void doUpdateFromString(String pText) {
        configRecords = new ArrayList<MespConfigTreeItem>();
        parse(pText);
    }

    /**
     * @return List of configuration records parsed from input.
     */
    public List<MespConfigTreeItem> getConfigRecords() {
        return configRecords;
    }

    /**
     * @param record
     *            Record to be added.
     * @return True if adding of record succeed, false otherwise.
     */
    public boolean addRecord(MespConfigTreeItem record) {
        return configRecords.add(record);
    }

    /**
     * @param record
     *            Record to be removed.
     * @return True if removal succeed, false otherwise.
     */
    public boolean removeRecord(MespConfigTreeItem record) {
        return configRecords.remove(record);
    }

    private void parse(String pText) {
        parse(new StringReader(pText));
    }

    private void parse() {
        try {
            parse(new InputStreamReader(new FileInputStream(configFile), "UTF-8"));
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        } catch (UnsupportedEncodingException e) {
            e.printStackTrace();
        }
    }

    private void parse(Reader r) {
        BufferedReader reader = null;

        try {
            reader = new BufferedReader(r);
            String line = null;
            while ((line = reader.readLine()) != null) {
                if (line.trim().length() == 0) {
                    continue;
                }
                configRecords.add(new MespConfigTreeItem(this, MESPConfigurationRecord.parse(line)));
            }
        } catch (IOException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        } catch (MESPConfigurationException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        } finally {
            try {
                if (reader != null) {
                    reader.close();
                }
            } catch (IOException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            }
        }
    }

    /**
     * @param monitor
     *            Perform saving of currently stored configuration records into file.
     */
    public void doSave(IProgressMonitor monitor) {
        String path = configFile.getAbsolutePath();
        try {
            doSave(monitor, path);
        } catch (IOException e) {
            // FIXME - show error dialog
            e.printStackTrace();
        }
    }

    private void doSave(IProgressMonitor monitor, String path) throws IOException {
        monitor.beginTask("Saving file '" + path + "'", configRecords.size());
        FileOutputStream fos = null;
        try {
            fos = new FileOutputStream(path);
            for (MespConfigTreeItem treeItem : configRecords) {
                fos.write((treeItem.getRecord().toString() + "\n").getBytes("UTF-8"));
                monitor.worked(1);
            }
        } finally {
            if (fos != null) {
                fos.close();
            }
            monitor.done();
        }
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public String toString() {
        return configFile.getAbsolutePath();
    }

    /**
     * {@inheritDoc}
     */
    public Object[] getChildren() {
        return configRecords.toArray();
    }

    /**
     * {@inheritDoc}
     */
    public Object getParent() {
        return null;
    }

    /**
     * {@inheritDoc}
     */
    public boolean hasChildren() {
        return configRecords.size() > 0;
    }

    /**
     * @return Return all configuration records in string representation.
     */
    public String getInputAsString() {
        StringBuilder sb = new StringBuilder();
        for (MespConfigTreeItem treeItem : configRecords) {
            sb.append(treeItem.getRecord().toString() + "\n");
        }
        return sb.toString();
    }

}
