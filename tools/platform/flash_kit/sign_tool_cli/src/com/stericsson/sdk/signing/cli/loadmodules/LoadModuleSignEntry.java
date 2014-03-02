package com.stericsson.sdk.signing.cli.loadmodules;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 * @author xtomzap
 * Model class for sign entry in load modules configuration xml file. 
 */
public class LoadModuleSignEntry {

    private static final String SOFTWARE_TYPE_ELF = "-self"; 

    private String platform;

    private Map<String,String> attributes;

    /**
     * @param pPlatform specify HW platform type
     */
    public LoadModuleSignEntry(String pPlatform) {
        this.platform = pPlatform;
        attributes = new HashMap<String, String>();
    }

    /**
     * @return HW platform
     */
    public String getPlatform() {
        return platform;
    }

    /**
     * @param pPlatform HW platform
     */
    public void setPlatform(String pPlatform) {
        platform = pPlatform;
    }

    /**
     * Add xml configuration
     * @param attributeName xml attribute name
     * @param attributeValue xml attribute value
     */
    public void addAttribute(String attributeName, String attributeValue) {
        attributes.put(attributeName, attributeValue);
    }

    /**
     * @return prepared command line arguments
     */
    public List<String> getArguments() {
        List<String> attrs = new ArrayList<String>();

        attrs.add(platform);
        attrs.add(SOFTWARE_TYPE_ELF);

        for (String key : attributes.keySet()) {
            String value = attributes.get(key);
            if (value == null || value.trim().equals("")) {
                attrs.add(key);
            } else {
                if (key.startsWith("--")) {
                    attrs.add(key);
                    attrs.add(value);
                } else {
                    attrs.add(key + value);
                }
            }
        }

        return attrs;
    }
}
