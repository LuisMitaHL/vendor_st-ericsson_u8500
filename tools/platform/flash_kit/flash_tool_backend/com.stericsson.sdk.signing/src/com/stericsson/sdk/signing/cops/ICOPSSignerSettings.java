package com.stericsson.sdk.signing.cops;


/**
 * COPS specific signer settings
 * 
 * @author xtomlju
 */
public interface ICOPSSignerSettings {

    /***/
    String KEY_ACL_SW_TYPE = "ACL_SW_TYPE";

    /***/
    String KEY_ACL_DEBUG = "DEBUG";

    /***/
    String KEY_ACL_TARGET_CPU = "TARGET_CPU";

    /***/
    String KEY_ACL_EXT_LEVEL = "ACL_EXT_LEVEL";

    /***/
    String KEY_ACL_ETX_LEVEL_IN_HEADER = "ACL_ETX_LEVEL_IN_HEADER";

    /***/
    String KEY_ACL_INTERACTIVE_LOAD_BIT = "ACL_INTERACTIVE_LOAD_BIT";

    /***/
    String KEY_ACL_APP_SEC = "ACL_APP_SEC";

    /***/
    String KEY_ACL_FORMAT = "ACL_FORMAT";

    /***/
    String KEY_ACL_HDR_SECURITY = "ACL_HDR_SECURITY";

    /***/
    String KEY_HEADER_DESTINATION_ADDRESS = "HDR_SECURITY_DEST_ADDRESS";

    /***/
    String KEY_ACL_PTYPE = "ACL_PTYPE";

    /***/
    String KEY_ACL_SHORT_MAC = "ACL_SHORT_MAC";

    /***/
    String KEY_SW_TYPE = "SW_TYPE";

    /***/
    String KEY_MAC_MODE = "MAC_MODE";

    /***/
    String KEY_SW_VERSION_MAC_MODE = "SW_VERSION_MAC_MODE";

    /***/
    String KEY_SW_VERSION_ANTI_ROLL_REQUIRED = "SW_VERSION_ANTI_ROLL_REQUIRED";

    /***/
    String KEY_SW_VERSION = "SW_VERSION";

    /***/
    String KEY_LOADER_COMPRESSION = "LOADER_COMPRESION";

    /***/
    String KEY_LOADER_ENCRYPTION = "LOADER_ENCRYPTION";

    /***/
    String KEY_KEYS_CATALOGUE = "KEYS_CATALOGUE";

    /***/
    String KEY_ENCRYPT_REQUIRED = "ENCRYPT_REQUIRED";

    /***/
    String KEY_CHIPIDS = "CHIPIDS";

    /***/
    String KEY_RANDOM_KEY_VALUE = "RANDOM_KEY_VALUE";

}
