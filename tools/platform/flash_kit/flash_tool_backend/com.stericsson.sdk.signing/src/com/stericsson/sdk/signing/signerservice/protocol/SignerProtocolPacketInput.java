package com.stericsson.sdk.signing.signerservice.protocol;

import java.io.DataInputStream;
import java.io.EOFException;
import java.io.IOException;
import java.io.InputStream;
import java.net.SocketException;
import java.util.Arrays;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.TimeUnit;
import java.util.zip.CRC32;

import org.apache.log4j.Logger;

import com.stericsson.sdk.signing.signerservice.SignerServiceException;

/**
 * @author xtomlju
 */
public class SignerProtocolPacketInput extends Thread {

    private static final int HEADER_LENGTH = 9;

    private static final int MAX_PACKET_LENGTH = 100 * 1024;

    private static final int TAIL_LENGTH = 4;

    private DataInputStream input = null;

    private boolean terminated = false;

    private LinkedBlockingQueue<SignerProtocolPacket> packetQueue;

    private Logger logger;

    /**
     * Creates a new PacketInput reading from the given stream.
     * 
     * @param is
     *            The input stream to read from.
     */
    public SignerProtocolPacketInput(InputStream is) {
        super("SignerProtocolPacketInput");
        logger = Logger.getLogger(getClass().getName());
        packetQueue = new LinkedBlockingQueue<SignerProtocolPacket>();
        input = new DataInputStream(is);
        start();
    }

    /**
     * Closes this input.
     * 
     * @throws IOException
     *             If fails to close input stream.
     */
    private void close() throws IOException {
        input.close();
    }

    /**
     * Creates a packet and store it in the packet buffer.
     * 
     * @param command
     *            The packet command.
     * @param data
     *            The packet data.
     * @throws Exception
     *             If fails to create and store.
     */
    private void createAndStore(byte command, byte[] data) throws SignerServiceException {
        SignerProtocolPacket packet = SignerProtocolPacketFactory.createPacket(command, data);
        try {
            packetQueue.put(packet);
        } catch (InterruptedException e) {
            throw new SignerServiceException(e.getMessage());
        }
    }

    /**
     * Retrieves the next packet from the packet buffer. If no packet is available in buffer it will
     * wait the specified time before giving up.
     * 
     * @param timeout
     *            The time to wait for packet if not immediately available.
     * @return The next packet in the buffer.
     * @throws SignerServiceException
     *             If fails to get packet.
     */
    public SignerProtocolPacket getPacket(long timeout) throws SignerServiceException {

        SignerProtocolPacket packet = null;

        try {
            packet = packetQueue.poll(timeout, TimeUnit.MILLISECONDS);
        } catch (InterruptedException e) {
            throw new SignerServiceException(e.getMessage());
        }

        return packet;
    }

    /**
     * Worker method for this thread.
     */
    public void run() {

        byte[] header = new byte[HEADER_LENGTH];
        byte[] packetCRC = new byte[TAIL_LENGTH];
        byte[] calcCRC = new byte[TAIL_LENGTH];

        try {
            while (!terminated) {

                CRC32 crc = new CRC32();
                byte command = 0;
                int dataLength = 0;

                try {
                    input.readFully(header);

                    for (int i = 0; i < SignerProtocolPacket.HEADER_MAGIC_ARRAY.length; i++) {
                        if (header[i] != SignerProtocolPacket.HEADER_MAGIC_ARRAY[i]) {
                            logger.error("Invalid sign protocol packet header received. Terminating packet input.");
                            return;
                        }
                    }
                } catch (IOException e) {
                    if (e instanceof SocketException || e instanceof EOFException) {
                        logger.info("Nothing more to read from sign server. Closing");
                        return;
                    }
                }

                command = header[4];

                dataLength |= (((int) header[5]) & 0xFF) << 24;
                dataLength |= (((int) header[6]) & 0xFF) << 16;
                dataLength |= (((int) header[7]) & 0xFF) << 8;
                dataLength |= ((int) header[8]) & 0xFF;

                if (dataLength < 0 || dataLength > MAX_PACKET_LENGTH) {
                    logger.error("Invalid data length field in sign protocol packet, length: " + dataLength);
                    return;
                }

                byte[] data = new byte[dataLength];

                input.readFully(data);
                input.readFully(packetCRC);

                crc.update(header, 4, header.length - 4);
                crc.update(data, 0, data.length);

                long crcValue = crc.getValue();

                calcCRC[0] = (byte) ((crcValue >> 24) & 0xFF);
                calcCRC[1] = (byte) ((crcValue >> 16) & 0xFF);
                calcCRC[2] = (byte) ((crcValue >> 8) & 0xFF);
                calcCRC[3] = (byte) (crcValue & 0xFF);

                if (!Arrays.equals(calcCRC, packetCRC)) {
                    logger.error("Sign protocol packet CRC error!");
                    return;
                }

                createAndStore(command, data);
            }
        } catch (Exception e) {
            logger.error("Error while reading incoming packets, terminating packet input! " + e.getMessage());
        } finally {
            terminate();
        }
    }

    /**
     * Terminates this packet input.
     */
    public void terminate() {
        if (!terminated) { // only need to terminate once...
            terminated = true;
            try {
                close();
            } catch (IOException e) {
                logger.error("Error while inputStream terminating!");
            }
        }
    }
}
