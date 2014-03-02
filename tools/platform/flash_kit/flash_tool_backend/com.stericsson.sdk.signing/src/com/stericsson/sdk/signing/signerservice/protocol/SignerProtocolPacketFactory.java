package com.stericsson.sdk.signing.signerservice.protocol;

import com.stericsson.sdk.signing.signerservice.SignerServiceException;

/**
 * @author qdaneke
 * 
 */
public final class SignerProtocolPacketFactory {

    private static final int[] RECOGNIZED_COMMANDS =
        {
            SignerProtocolPacket.COMMAND_LOGIN_REQUEST, SignerProtocolPacket.COMMAND_LOGIN_ACCEPT,
            SignerProtocolPacket.COMMAND_SECRET_REQUEST, SignerProtocolPacket.COMMAND_SECRET_RESPONSE,
            SignerProtocolPacket.COMMAND_RANDOMNUMBER_REQUEST, SignerProtocolPacket.COMMAND_RANDOMNUMBER_RESPONSE,
            SignerProtocolPacket.COMMAND_HEADERPACKAGE_RESPONSE, SignerProtocolPacket.COMMAND_HEADERPACKAGE_REQUEST,
            SignerProtocolPacket.COMMAND_HELLO, SignerProtocolPacket.COMMAND_DATA_A2_ENCRYPTION_REQUEST,
            SignerProtocolPacket.COMMAND_DATA_A2_ENCRYPTION_RESPONSE,
            SignerProtocolPacket.COMMAND_KEYCATALOGUE_REQUEST, SignerProtocolPacket.COMMAND_KEYCATALOGUE_RESPONSE,
            SignerProtocolPacket.COMMAND_DATA_A2_ENCRYPTION_SESSIONKEY_REQUEST,
            SignerProtocolPacket.COMMAND_DATA_A2_ENCRYPTION_SESSIONKEY_RESPONSE,
            SignerProtocolPacket.COMMAND_DATA_U8500_ENCRYPTION_REQUEST,
            SignerProtocolPacket.COMMAND_DATA_U8500_ENCRYPTION_RESPONSE,
            SignerProtocolPacket.COMMAND_SUB_PACKAGE_BY_KEY_REQUEST,
            SignerProtocolPacket.COMMAND_SUB_PACKAGE_BY_KEY_RESPONSE,
            SignerProtocolPacket.COMMAND_DATA_U5500_ENCRYPTION_REQUEST_SHA1withRSA,
            SignerProtocolPacket.COMMAND_DATA_U5500_ENCRYPTION_REQUEST_SHA256withRSA,
            SignerProtocolPacket.COMMAND_DATA_U5500_ENCRYPTION_REQUEST_SHA384withRSA,
            SignerProtocolPacket.COMMAND_DATA_U5500_ENCRYPTION_REQUEST_SHA512withRSA,
            SignerProtocolPacket.COMMAND_DATA_U5500_ENCRYPTION_RESPONSE,
            SignerProtocolPacket.COMMAND_DATA_PKCS1_ENCRYPTION_REQUEST,
            SignerProtocolPacket.COMMAND_DATA_PKCS1_ENCRYPTION_RESPONSE,
            SignerProtocolPacket.COMMAND_USE_COMMON_FOLDER_REQUEST};

    /**
     * Utility method to test if a packet is of a specific command type
     * 
     * @param packet
     *            Packet to test
     * @param command
     *            Command to match packet instance to
     * @return True if packet is of corresponding packet class specified by command
     */
    public static boolean isPackageCommand(SignerProtocolPacket packet, byte command) {
        if (packet == null) {
            return false;
        }

        return (packet.getCommand() == command);
    }

    /**
     * Factory method to create a signer protocol packet for a specified command and data
     * 
     * @param command
     *            Command to create corresponding signer protocol packet for
     * @param data
     *            Data for the packet
     * @return A signer protocol packet instance
     * @throws SignerServiceException
     *             If an signer service related error occurred
     */
    public static SignerProtocolPacket createPacket(byte command, byte[] data) throws SignerServiceException {

        for (int c : RECOGNIZED_COMMANDS) {
            if (c == command) {
                return new SignerProtocolPacket(command, data);
            }

        }

        throw new SignerServiceException("Invalid signer protocol command: " + command);
    }

    private SignerProtocolPacketFactory() {

    }
}
