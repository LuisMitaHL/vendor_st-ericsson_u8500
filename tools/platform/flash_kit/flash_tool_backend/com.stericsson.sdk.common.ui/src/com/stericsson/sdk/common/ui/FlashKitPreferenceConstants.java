package com.stericsson.sdk.common.ui;

import org.eclipse.jface.dialogs.MessageDialogWithToggle;

import com.stericsson.sdk.common.configuration.system.SystemProperties;

/**
 * Constant definitions for plug-in preferences
 */
public final class FlashKitPreferenceConstants {

    private FlashKitPreferenceConstants() {
    }

    /** */
    public static final String BACKEND_LCD_FILE = SystemProperties.BACKEND_LCD_FILE;

    /** */
    public static final String BACKEND_LCM_FILE = SystemProperties.BACKEND_LCM_FILE;

    /** */
    public static final String BACKEND_CONFIGURATION_FILE = SystemProperties.BACKEND_CONFIGURATIONS_ROOT;

    /** */
    public static final String BACKEND_CONFIGURATION_FILE_SHORTCUT = "@configurations";

    /** */
    public static final String BACKEND_LOADER_PATH = SystemProperties.BACKEND_LOADER_ROOT;

    /** */
    public static final String BACKEND_LOADER_PATH_SHORTCUT = "@loaders";

    /** */
    public static final String BACKEND_PROFILE_PATH = SystemProperties.BACKEND_PROFILES_ROOT;

    /** */
    public static final String BACKEND_PROFILE_PATH_SHORTCUT = "@profiles";

    /** */
    public static final String BACKEND_USB_ENABLED = "usbEnabled";

    /** */
    public static final String BACKEND_ACCEPT_CLIENTS = "acceptClients";

    /** */
    public static final String BACKEND_ACCEPT_EQUIPMENT = "acceptEquipment";

    /** */
    public static final String BACKEND_ACTIVE_PROFILE = "activeProfile";

    /** */
    public static final String BACKEND_BRP_PORT = SystemProperties.BACKEND_BRP_PORT;

    /** */
    public static final String EQUIPMENT_EDITOR_DISCONNECT =
        "com.stericsson.sdk.equipment.ui.editors.equipmenteditor.disconnect";

    /** */
    public static final String EQUIPMENT_EDITOR_DISCONNECT_CLOSE = MessageDialogWithToggle.ALWAYS;

    /** */
    public static final String EQUIPMENT_EDITOR_DISCONNECT_NO_CLOSE = MessageDialogWithToggle.NEVER;

    /** */
    public static final String EQUIPMENT_EDITOR_DISCONNECT_ASK = MessageDialogWithToggle.PROMPT;

    /** */
    public static final String SIGNING_USE_LOCAL_SIGNING = "useLocaLSigning";

    /** */
    public static final String SIGNING_SIGN_SERVER = "signServer";

    /** */
    public static final String SIGNING_SIGN_PACKAGE_ROOT = "signPackageRoot";

    /** */
    public static final String SIGNING_LOCAL_SIGN_PACKAGE_ROOT = "localSignPackageRoot";

    /** */
    public static final String SIGNING_LOCAL_KEY_ROOT = "localKeyRoot";

    /** */
    public static final String SIGNING_USE_DEFAULT_SIGN_PACKAGE = "useDefaultSignPackage";

    /** */
    public static final String SIGNING_DEFAULT_SIGN_PACKAGE = "defaultSignPackage";

    /** */
    public static final String SIGNING_ROOT_KEY_HASH_TYPE = "rootKeyHashType";

    /** */
    public static final String SIGNING_PAYLOAD_HASH_TYPE = "payloadHashType";

    /** */
    public static final String SIGNING_SIGNATURE_HASH_TYPE = "signatureHashType";

    /** */
    public static final String SIGNING_USE_COMMONFOLDER = "useCommonFolder";

    /** */
    public static final String SIGNING_LOCAL_KEY_PACKAGES_ROOT = "localKeyPackagesRoot";
}
