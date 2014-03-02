package com.stericsson.sdk.equipment.io;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.channels.GatheringByteChannel;
import java.nio.channels.ReadableByteChannel;
import java.nio.channels.ScatteringByteChannel;
import java.nio.channels.WritableByteChannel;

import com.stericsson.sdk.equipment.io.port.IPort;

/**
 * @author xtomlju
 */
public class PortChannel implements ReadableByteChannel, WritableByteChannel, ScatteringByteChannel, GatheringByteChannel {

    private AbstractPort channelPort;

    /**
     * Constructor
     * 
     * @param port
     *            Port object
     */
    public PortChannel(IPort port) {
        channelPort = (AbstractPort)port;
        if (!port.isOpen()) {
            try {
                port.open();
            } catch (IOException e) {
                throw new RuntimeException("Openning communication port " + port.getPortName() + " failed!", e);
            }
        }
    }

    /**
     * {@inheritDoc}
     */
    public int read(ByteBuffer dst) throws IOException {
        if (!dst.hasArray()) {
            throw new IOException("ByteBuffer has no backing byte array");
        }
        int bytesRead = channelPort.read(dst.array(), dst.position(), dst.remaining());
        if (bytesRead > 0) {
            dst.position(dst.position() + bytesRead);
        }
        return bytesRead;
    }

    /**
     * {@inheritDoc}
     */
    public long read(ByteBuffer[] dsts) throws IOException {
        long bytesRead = 0L;
        for (ByteBuffer dst : dsts) {
            if (dst.hasRemaining()) {
                bytesRead += read(dst);
            } else {
                continue;
            }
            if (dst.hasRemaining()) {
                return bytesRead;
            }
        }
        return bytesRead;
    }

    /**
     * {@inheritDoc}
     */
    public long read(ByteBuffer[] dsts, int offset, int length) throws IOException {
        if (!((offset >= 0) && (length >= 0) && (offset + length <= dsts.length))) {
            throw new ArrayIndexOutOfBoundsException();
        }

        long bytesRead = 0L;
        for (int i = offset; i < length; i++) {
            if (dsts[i].hasRemaining()) {
                bytesRead += read(dsts[i]);
            } else {
                continue;
            }
            if (dsts[i].hasRemaining()) {
                return bytesRead;
            }
        }
        return bytesRead;
    }

    /**
     * {@inheritDoc}
     */
    public void close() throws IOException {
        if (channelPort.isOpen()) {
            channelPort.close();
        }
    }

    /**
     * {@inheritDoc}
     */
    public boolean isOpen() {
        return channelPort.isOpen();
    }

    /**
     * {@inheritDoc}
     */
    public int write(ByteBuffer src) throws IOException {
        if (!src.hasArray()) {
            throw new IOException("ByteBuffer has no backing byte array");
        }
        int bytesWritten = channelPort.write(src.array(), src.position(), src.remaining());
        if (bytesWritten > 0) {
            src.position(src.position() + bytesWritten);
        }
        return bytesWritten;
    }

    /**
     * {@inheritDoc}
     */
    public long write(ByteBuffer[] srcs) throws IOException {
        long bytesWritten = 0L;
        for (ByteBuffer src : srcs) {
            if (src.hasRemaining()) {
                bytesWritten += write(src);
            } else {
                continue;
            }
            if (src.hasRemaining()) {
                return bytesWritten;
            }
        }
        return bytesWritten;
    }

    /**
     * {@inheritDoc}
     */
    public long write(ByteBuffer[] srcs, int offset, int length) throws IOException {
        if (!((offset >= 0) && (length >= 0) && (offset + length <= srcs.length))) {
            throw new ArrayIndexOutOfBoundsException();
        }

        long bytesWritten = 0L;
        for (int i = offset; i < length; i++) {
            if (srcs[i].hasRemaining()) {
                bytesWritten += write(srcs[i]);
            } else {
                continue;
            }
            if (srcs[i].hasRemaining()) {
                return bytesWritten;
            }
        }
        return bytesWritten;
    }

}
