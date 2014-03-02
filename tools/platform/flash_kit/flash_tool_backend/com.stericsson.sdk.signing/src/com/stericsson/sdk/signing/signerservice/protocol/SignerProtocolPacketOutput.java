package com.stericsson.sdk.signing.signerservice.protocol;

import java.io.IOException;
import java.io.OutputStream;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.TimeUnit;

import org.apache.log4j.Logger;

import com.stericsson.sdk.signing.signerservice.SignerServiceException;

/**
 * The PacketOutput class represents a output stream of packaged signals used in the signing
 * protocol. This output channel is used to send packets to a connected peer on the network.
 */
public class SignerProtocolPacketOutput extends Thread {

    /**
     * Ref to logging facility.
     */
    private Logger logger;

    /**
     * Timeout for sending packets.
     */
    private static final long PACKET_TIMEOUT = 5000;

    /**
     * Output to write packets on.
     */
    private OutputStream output = null;

    /**
     * Lifetime control flag.
     */
    private boolean terminated = false;

    private LinkedBlockingQueue<SignerProtocolPacket> packetQueue;

    /**
     * Creates an output channel that can be used to send command and response packets.
     * 
     * @param os
     *            The stream to use for this output channel.
     */
    public SignerProtocolPacketOutput(OutputStream os) {
        super("SignerProtocolPacketOutput");
        logger = Logger.getLogger(getClass().getName());
        packetQueue = new LinkedBlockingQueue<SignerProtocolPacket>();
        output = os;
        start();
    }

    /**
     * Closes this output channel.
     * 
     * @throws IOException
     *             If the close operation fails.
     */
    private void close() throws IOException {
        output.close();
    }

    /**
     * Makes packet ap available for sending. The packet is sent when all previous packets en the
     * queue are sent.
     * 
     * @param packet
     *            The packet to send.
     * @throws SignerServiceException
     *             If sending the packet fails.
     */
    public void putPacket(SignerProtocolPacket packet) throws SignerServiceException {
        try {
            packetQueue.put(packet);
        } catch (InterruptedException e) {
            throw new SignerServiceException(e.getMessage());
        }
    }

    /**
     * Worker method for this channels thread. Polls the queue for outgoing packets and if packets
     * are found they get sent.
     */
    public void run() {
        try {
            SignerProtocolPacket packet = null;
            while (!terminated) {

                try {
                    packet = packetQueue.poll(PACKET_TIMEOUT, TimeUnit.MILLISECONDS);
                } catch (InterruptedException e) {
                    logger.debug("Packet pool timeout");
                }

                if (packet == null) {
                    continue;
                }

                if (!terminated) {
                    try {
                        output.write(packet.toArray());
                        output.flush();
                    } catch (Exception e) {
                        logger.warn("Failed to send signing protocol command packet! " + e.getMessage());
                    }
                }
            }
        } finally {
            try {
                close();
            } catch (IOException e) {
                logger.warn("Error while outputStream terminating!");
            }
        }
    }

    /**
     * Terminates this channel.
     */
    public void terminate() {
        if (!terminated) {
            terminated = true;
            try {
                join();
            } catch (InterruptedException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            }
        }
    }
}
