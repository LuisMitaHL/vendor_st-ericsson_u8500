package com.stericsson.sdk.signing.generic;

/**
 * U8500 specific signer settings
 * 
 * @author xolabju
 * 
 */
public interface IGenericSignerSettings {

    /***/
    String KEY_CUT_ID = "CUT_ID";

    /***/
    String KEY_SIGN_PACKAGE_TYPE = "SIGN_PACKAGE_TYPE";

    /***/
    String KEY_SW_TYPE = "SW_TYPE";

    /***/
    String KEY_HASH_BLOCK_SIZE = "HASH_BLOCK_SIZE";

    /***/
    String KEY_LOAD_ADDRESS = "LOAD_ADDRESS";

    /***/
    String KEY_START_ADDRESS = "START_ADDRESS";

    /***/
    String KEY_UNCHECKED_BLOB = "UNCHECKED_BLOB";

    /***/
    String KEY_PKA_FLAG = "PKA_FLAG";

    /***/
    String KEY_DMA_FLAG = "DMA_FLAG";

    /***/
    String KEY_SIGNED_HEADER_SIGNATURE_SIZE = "SIGNED_HEADER_SIGNATURE_SIZE";

    /***/
    String KEY_TEMP_AUTHENTICATION_CERTIFICATE_FILENAME = "TEMP_AUTHENTICATION_CERTIFICATE_FILENAME";

    /***/
    String KEY_ROOT_KEY_HASH_TYPE = "ROOT_KEY_HASH_TYPE";

    /***/
    String KEY_SIGNATURE_HASH_TYPE = "SIGNATURE_HASH_TYPE";

    /***/
    String KEY_PAYLOAD_HASH_TYPE = "PAYLOAD_HASH_TYPE";

    /***/
    String KEY_BUFFER_SIZE = "BUFFER_SIZE";

    /***/
    String KEY_SW_VERSION = "SW_VERSION";

    /***/
    String KEY_SIGNATURE_TYPE = "SIGNATURE_TYPE";

    /** */
    String KEY_MAJOR_BUILD_VERSION = "MAJOR_BUILD_VERSION";

    /** */
    String KEY_MINOR_BUILD_VERSION = "MINOR_BUILD_VERSION";

    /** */
    String KEY_FLAGS = "FLAGS";

    /** Sign key settings key. To hold an instance of ISignPackage */
    String KEY_SIGN_KEY = "SIGN_KEY";

}
