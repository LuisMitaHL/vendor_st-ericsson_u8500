package com.stericsson.sdk.loader.communication.test;

import java.lang.reflect.Method;

import junit.framework.TestCase;

import org.osgi.framework.ServiceReference;

import com.stericsson.sdk.equipment.io.port.ILCPort;
import com.stericsson.sdk.equipment.io.port.IPort;
import com.stericsson.sdk.equipment.io.port.PortException;
import com.stericsson.sdk.loader.communication.ILoaderCommunicationService;

/**
 * @author xdancho
 * 
 */
public class ReaderWriterTest extends TestCase {

    static ILoaderCommunicationService service = null;

    private static void setService(ILoaderCommunicationService lcService) {
        ReaderWriterTest.service = lcService;
    }

    static long instancePointer = 0;

    /**
     * @return the instancePointer
     */
    private static long getInstancePointer() {
        return instancePointer;
    }

    /**
     * @param ip
     *            the instancePointer to set
     */
    private static void setInstancePointer(long ip) {
        ReaderWriterTest.instancePointer = ip;
    }

    static IPort port = new ILCPort() {

        final Integer portId = IPort.PortIdGenerator.INSTANCE.getNextId();

        public int write(byte[] buffer, int offset, int length, int timeout) throws PortException {
            return length;
        }

        public int write(byte[] buffer, int offset, int length) throws PortException {
            return length;
        }

        public int write(byte[] buffer, int timeout) throws PortException {
            return buffer.length;
        }

        public int write(byte[] buffer) throws PortException {
            return buffer.length;
        }

        public int write(int byteValue, int timeout) throws PortException {
            return 1;
        }

        public int write(int byteValue) throws PortException {
            return 1;
        }

        public int read(byte[] buffer, int offset, int length, int timeout) throws PortException {
            for (int i = 0; i < length; i++) {
                buffer[offset + 1] = (byte) 0xFF;
            }
            return length;
        }

        public int read(byte[] buffer, int offset, int length) throws PortException {
            return read(buffer, offset, length, 1000);
        }

        public int read(byte[] buffer, int timeout) throws PortException {
            return read(buffer, 0, buffer.length, timeout);
        }

        public int read(byte[] buffer) throws PortException {
            read(buffer, 1000);
            return 0;
        }

        public int read(int timeout) throws PortException {
            return read();
        }

        public int read() throws PortException {
            return 0;
        }

        public void open() throws PortException {
        }

        public boolean isOpen() {
            return false;
        }

        public String getPortName() {
            return null;
        }

        public void close() throws PortException {
        }

        public Integer getPortIdentifier() {
            return portId;
        }
    };

    private static final String INTERFACE_CLASS_NAME =
        "com.stericsson.sdk.loader.communication.internal.LoaderCommunicationInterface";

    // read(int length, long dataPointer, long instancePointer)
    private static final String READ_METHOD_NAME = "read";

    // write(byte[] data, int length, long dataPointer, long instancePointer)
    private static final String WRITE_METHOD_NAME = "write";

    // private static String USE_PRINTOUTS_FIELD_NAME = "useCommunicationPrintouts";
    // private static String DISABLE_CHUNK_FIELD_NAME = "disableChunkPrintouts";

    private static final String GET_INSTANCE = "getInstance";

    private static Method readMethod;

    private static Method writeMethod;

    private static Object lcInstance;

    private static Class<?> lcInterfaceClass;

    private static void setLcInterfaceClass(Class<?> clazz) {
        ReaderWriterTest.lcInterfaceClass = clazz;
    }

    private static void setReadMethod(Method method) {
        ReaderWriterTest.readMethod = method;
    }

    private static void setWriteMethod(Method method) {
        ReaderWriterTest.writeMethod = method;
    }

    private static void setInstance(Object instance) {
        ReaderWriterTest.lcInstance = instance;
    }

    /**
     * test reader writer
     */
    public void testReaderWriter() {
        ServiceReference sr = null;
        try {
            sr =
                Activator.getBundleContext().getServiceReferences(ILoaderCommunicationService.class.getName(),
                    "(type=normal)")[0];

            setService((ILoaderCommunicationService) Activator.getBundleContext().getService(sr));

            ReaderWriterTest.setInstancePointer(service.initialize(port));

            setLcInterfaceClass(Class.forName(INTERFACE_CLASS_NAME));
            Method getInstance = lcInterfaceClass.getDeclaredMethod(GET_INSTANCE);

            setInstance(getInstance.invoke(null, new Object[] {}));

            setWriteMethod(lcInterfaceClass.getDeclaredMethod(WRITE_METHOD_NAME, byte[].class, int.class, long.class,
                long.class));
            setReadMethod(lcInterfaceClass.getDeclaredMethod(READ_METHOD_NAME, int.class, long.class, long.class));

            // //enable printouts
            // Field usePrintouts = lcInterfaceClass.getDeclaredField(USE_PRINTOUTS_FIELD_NAME);
            // Field disableChunk = lcInterfaceClass.getDeclaredField(DISABLE_CHUNK_FIELD_NAME);
            //
            // usePrintouts.setAccessible(true);
            // disableChunk.setAccessible(true);
            //
            // usePrintouts.setBoolean(lcInstance, Boolean.TRUE);
            // disableChunk.setBoolean(lcInstance, Boolean.FALSE);

            // read(int length, long dataPointer, long instancePointer)
            ReaderWriterTest.readMethod.invoke(lcInstance, new Object[] {
                10, 1L, getInstancePointer()});

            // write(byte[] data, int length, long dataPointer, long instancePointer)
            ReaderWriterTest.writeMethod.invoke(lcInstance, new Object[] {
                new byte[10], 10, 1L, getInstancePointer()});

            service.cleanUp(getInstancePointer());
        } catch (Exception e) {
            e.printStackTrace();
        }

    }
}
