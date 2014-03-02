package com.stericsson.sdk.equipment.m7x00.bsu;

/**
 * Description of a load module, contains module name and its manifest.
 * @author xadazim
 *
 */
public class LoadModuleDescription {
    private String moduleName;
    private LoadModuleManifest manifest;

    /**
     * Constructs load module description.
     * @param pModuleName module name
     * @param pManifest module manifest
     */
    public LoadModuleDescription(String pModuleName, LoadModuleManifest pManifest) {
        moduleName = pModuleName;
        manifest = pManifest;
    }

    /**
     * Gets module name.
     * @return module name.
     */
    public String getModuleName() {
        return moduleName;
    }

    /**
     * Gets module manifest.
     * @return module manifest
     */
    public LoadModuleManifest getManifest() {
        return manifest;
    }

    /**
     * 
     * {@inheritDoc}
     */
    public String toString() {
        String newline = System.getProperty("line.separator");
        StringBuilder result = new StringBuilder();
        result.append("Name=").append(moduleName).append(newline);
        result.append("Manifest=").append(newline).append(manifest).append(newline);
        return result.toString();
    }
}
