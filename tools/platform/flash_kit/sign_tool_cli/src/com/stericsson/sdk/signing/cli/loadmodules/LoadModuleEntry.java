package com.stericsson.sdk.signing.cli.loadmodules;

import java.util.ArrayList;
import java.util.List;

/**
 * Model class of load module entry from xml configuration file
 * @author xtomzap
 *
 */
public class LoadModuleEntry {

    private List<String> sourceList;

    private LoadModuleSignEntry signEntry;


    /***/
    public LoadModuleEntry() {
        sourceList = new ArrayList<String>();
    }

    /**
     * @param source load module
     */
    public void addSource(String source) {
        sourceList.add(source);
    }

    /**
     * @return list of source load modules
     */
    public List<String> getSourceList() {
        return sourceList;
    }

    /**
     * @return sign configuration entry
     */
    public LoadModuleSignEntry getSignEntry() {
        return signEntry;
    }

    /**
     * @param pSignEntry sign configuration entry
     */
    public void setSignEntry(LoadModuleSignEntry pSignEntry) {
        signEntry = pSignEntry;
    }
}
