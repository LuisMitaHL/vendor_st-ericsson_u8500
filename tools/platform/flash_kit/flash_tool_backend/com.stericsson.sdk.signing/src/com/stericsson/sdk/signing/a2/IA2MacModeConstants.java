package com.stericsson.sdk.signing.a2;

/**
 * This class is "MODE" field in A2 header. 
 * The mode field contains the MAC calculation mode in A2. The MAC mode is the sum of ACCESS or
 * APPLICATION plus the suitable MAC type.
 * 
 * @author ezaptom
 *
 */
public interface IA2MacModeConstants {

    /** Signing for Access */
    byte ACCESS = 0x00;

    /** Constant for Application CPU */
    byte APPLICATION = 0x10;

    /** Mac configuration data. */
    byte MAC_CONFIG = 0x01;

    /** Mac 2 stage boot code. */
    byte MAC_2SBC = 0x02;

    /** Mac 3 stage boot code. */
    byte MAC_3SBC = 0x03;

    /** Mac SW. */
    byte MAC_SW = 0x04;

    /** Mac 4 stage boot code. */
    byte MAC_4SBC = 0x05;

    /** Software logging license MAC. */
    byte MAC_LICENSE = 0x06;

    /** Mac 7. */
    byte MAC_7 = 0x07;

    /** Mac 8. */
    byte MAC_8 = 0x08;
}

