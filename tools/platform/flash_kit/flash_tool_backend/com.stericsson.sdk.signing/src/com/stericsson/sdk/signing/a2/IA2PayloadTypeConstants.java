package com.stericsson.sdk.signing.a2;

/**
 * This class is "PL_TYPE" field in A2 header.
 * PL_TYPE is a 4-byte field that contains the type of payload.
 * 
 * @author ezaptom
 *
 */
public interface IA2PayloadTypeConstants {

    /** */
    int TYPE_SW_IMAGE       = 0x00000000;
    /** */
    int TYPE_LOADER         = 0x00000001;
    /** */
    int TYPE_GENERIC        = 0x00000002;
    /** */
    int TYPE_ARCHIVE        = 0x00000003;
    /** */
    int TYPE_ELF            = 0x00000004;
    /** */
    int TYPE_TAR            = 0x00000100;

}
