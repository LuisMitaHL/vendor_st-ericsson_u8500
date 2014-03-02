/**
 * 
 */
package com.stericsson.sdk.common.ui.wizards;

/**
 * Keys of settings values of Default Data Sheet Wizard. The settings of this wizard are stored in
 * IDialogSettings object which holds values in a similar way to a map - by a key-value pairing.
 * This enum serves as a key list for this map (precisely a toString() values of these enums).
 * 
 * @author xadazim
 * 
 */
public enum SettingsKeys {
    /**
     * Key for a value of "Select All Platforms" check box. Control is on the SelectFileWizardPage.
     */
    ALL_PLATFORMS_SELECTED,

    /**
     * Key for a value of "File" path (source excel file). Control is on the SelectFileWizardPage.
     */
    SOURCE_FILE,

    /**
     * Key for a value of "Create GDVAR file" check box. Control is on the OnePlatformOutputPage and
     * AllPlatformsOutputPage.
     */
    CREATE_GDVAR_FILE,

    /**
     * Key for a value of GDVAR file path. Control is on the OnePlatformOutputPage.
     */
    GDVAR_FILE,

    /**
     * Key for a value of "Create GDF file" check box. Control is on the OnePlatformOutputPage and
     * AllPlatformsOutputPage.
     */
    CREATE_GDF_FILE,

    /**
     * Key for a value of GDF file path. Control is on the OnePlatformOutputPage.
     */
    GDF_FILE,

    /**
     * Key for a value of "Merge with input GDF file" check box. Control is on the
     * OnePlatformOutputPage.
     */
    MERGE_INPUT_GDF_FILE,

    /**
     * Key for a value of GDF input (to be merged with) file path. Control is on the
     * OnePlatformOutputPage.
     */
    INPUT_GDF_FILE,

    /**
     * Key for a value of "Create EEVAR file" check box. Control is on the OnePlatformOutputPage and
     * AllPlatformsOutputPage.
     */
    CREATE_EEVAR_FILE,

    /**
     * Key for a value of EEVAR file path. Control is on the OnePlatformOutputPage.
     */
    EEVAR_FILE,

    /**
     * Key for a value of "Create EE file" check box. Control is on the OnePlatformOutputPage and
     * AllPlatformsOutputPage.
     */
    CREATE_EE_FILE,

    /**
     * Key for a value of EE file path. Control is on the OnePlatformOutputPage.
     */
    EE_FILE,

    /**
     * Key for a value of "Create product 19062 Excel file" check box. Control is on the
     * OnePlatformOutputPage and AllPlatformsOutputPage.
     */
    CREATE_EXCEL_FILE,

    /**
     * Key for a value of EXCEL file path. Control is on the OnePlatformOutputPage.
     */
    EXCEL_FILE,

    /**
     * Key for a value of "Show detailed report when finished" check box. Control is on the
     * OnePlatformOutputPage and AllPlatformsOutputPage.
     */
    GENERATE_REPORT,

    /**
     * Key of a boolean value which is set to true after a platform is chosen in the "Choose" combo
     * box. Control is on the SelectFileWizardPage.
     */
    CORRECT_PLATFORM_SELECTED,

    /**
     * Key for a value of "Output folder" path (text field). Control is on the
     * AllPlatformsOutputPage.
     */
    OUTPUT_DIR

}
