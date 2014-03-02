package com.stericsson.sdk.signing.signerservice;

import com.stericsson.sdk.common.configuration.mesp.MESPConfigurationRecord;

/**
 * @author xtomlju
 */
public class SignPackageInformation implements ISignPackageInformation {

    private String signPackageName;

    private String[] chipIdentifiers;

    private boolean encryptRequired;

    private String parent;

    private String type;

    /**
     * Constructor
     * 
     * @param record
     *            Sign package mesp configuration record
     */
    public SignPackageInformation(MESPConfigurationRecord record) {
        signPackageName = record.getValue(ISignPackage.PACKAGE_ALIAS_NAME);
        type = record.getValue(ISignPackage.PACKAGE_TYPE_NAME);
        parent = record.getValue(ISignPackage.PACKAGE_PARENT_NAME);

        Object chipIds = record.getArrayValue(ISignPackage.PARAM_NAME_CHIPIDS);
        if (chipIds instanceof String[]) {
            chipIdentifiers = (String[]) chipIds;
        } else if (chipIds instanceof String) {
            chipIdentifiers = new String[] {
                (String) chipIds};
        } else {
            chipIdentifiers = new String[0];
        }

        encryptRequired = "true".equalsIgnoreCase((String) record.getValue(ISignPackage.PARAM_NAME_ENCRYPT_REQUIRED));
    }

    /**
     * @return the sign package parent, or null if there is no parent
     */
    public String getParent() {
        return parent;
    }

    /**
     * @return the sign package type type
     */
    public String getType() {
        return type;
    }

    /**
     * @return Sign package name
     */
    public String getName() {
        return signPackageName;
    }

    /**
     * @return A list of chip identifiers
     */
    public String[] getChipIdentifiers() {
        return chipIdentifiers;
    }

    /**
     * @return True if encrypt should be required
     */
    public boolean isEncryptRequired() {
        return encryptRequired;
    }

}
