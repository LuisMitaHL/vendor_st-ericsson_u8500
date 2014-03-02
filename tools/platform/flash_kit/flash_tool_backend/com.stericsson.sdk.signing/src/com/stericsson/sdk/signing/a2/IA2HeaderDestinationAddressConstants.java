package com.stericsson.sdk.signing.a2;

/**
 * This class is "HD_DEST_ADDRESS" in A2 header. It's value depends on the ACL header security
 * value. HD_DEST_ADDRESS is a 4-byte field that contains the destination address where the header is
 * to be stored in flash. If the second byte of ACL has bits 5-6 set to 11 (verifyOnly) the address is
 * 0xFFFFEEEE.
 * 
 * @author ezaptom
 * 
 */
public interface IA2HeaderDestinationAddressConstants {
    /***/
    int HEADER_DEST_ADDRESS = 0xFFFFEEEE;

}
