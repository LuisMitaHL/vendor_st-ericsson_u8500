/**
 * 
 */
package com.stericsson.sdk.common;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.UnsupportedEncodingException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.channels.FileChannel;
import java.util.Date;
import java.util.HashMap;
import java.util.zip.ZipEntry;
import java.util.zip.ZipFile;

import com.stericsson.sdk.common.image.ITableOfContentsItem;

/**
 * @author cizovhel
 * 
 */
public class VersionParser {
    /** */
    private static final int BUFFER_SIZE = 256 * 1024;

    private static final String VERSION = "Version";

    private static final String REVISION = "Revision";

    private static final String COLON = ":";

    private static final String EQUALS = "=";

    private static final String DOT = ".";

    private static final String SEMICOLON = ";";

    private static final String VERSIONING = "____________Versioning____________";

    private static final String CUT_ID = "Cut ID";

    private static final String BIP = "Bip";

    private static final String BIB = "Bib";

    private static final String LINE_SEPARATOR = System.getProperty("line.separator");

    private static final String BUILD = "Build";

    private static final String DATE = "Date";

    private static final String MEM_INIT_VERSION_PREFIX_1 = "@#";

    private static final String MEM_INIT_VERSION_PREFIX_2 = "@(#)";

    private static final String MEM_INIT_VERSION_PREFIX_LABEL_1 = "MEM_INIT";

    private static final String MEM_INIT_VERSION_PREFIX_LABEL_2 = "SOC-SETTINGS";

    /** protected constructor */
    protected VersionParser() {

    }

    /**
     * Gives the instance of this singleton
     * 
     * @return VersionUtils
     * */
    public static VersionParser getInstance() {
        return new VersionParser();
    }

    /**
     * Parse versions of passed boot image file print it into version file and return its path
     * 
     * @param fileNames
     *            map of filenames and TOC-IDs
     * @param zipFile
     *            zip file which contains boot images
     * @param versionPath
     *            path to the version file
     * @return path to the version file
     * @throws IOException
     *             on file parsing errors
     * */
    public String parseVersion(HashMap<String, String> fileNames, ZipFile zipFile, String versionPath)
        throws IOException {
        if (versionPath == null) {
            return null;
        }

        FileOutputStream fout = null;
        FileChannel cout = null;
        try {
            fout = new FileOutputStream(versionPath);
            cout = fout.getChannel();

            parseXloader(fileNames, zipFile, cout);
            parseISSW(fileNames, zipFile, cout);
            parseMeminit(fileNames, zipFile, cout);
            parsePowerManagement(fileNames, zipFile, cout);
            parseModem(fileNames, zipFile, cout);
            parseIPL(fileNames, zipFile, cout);

            fout.flush();

        } catch (FileNotFoundException e) {
            throw new IOException(e.getMessage());
        } finally {
            if (cout != null) {
                try {
                    cout.close();
                } catch (IOException e) {
                    System.out.println("closing cout failed");
                    throw new IOException(e.getMessage());
                }
            }
            if (fout != null) {
                try {
                    fout.close();
                } catch (IOException e) {
                    System.out.println("closing fout failed");
                    throw new IOException(e.getMessage());
                }
            }
        }

        return versionPath;
    }

    private void parseXloader(HashMap<String, String> fileNames, ZipFile zipFile, FileChannel cout) throws IOException {
        ByteBuffer src;
        String version = "";

        String fileName = fileNames.get(ITableOfContentsItem.FILENAME_XLOADER);
        if (fileName == null) {
            return;
        }
        File bootImageFile = extractFile(fileName, zipFile);
        ByteBuffer buffer = getImage(bootImageFile);
        version = getXLoaderVersion(buffer.array(), false);
        src = ByteBuffer.allocate(version.getBytes("UTF-8").length);
        src.put(version.getBytes("UTF-8"));
        src.flip();
        cout.write(src);
    }

    /**
     * @param xLoader
     *            byte array
     * @param brpFormatted
     *            True if result string should be formatted according to BRP rules
     * @return xLoader version
     */
    public String getXLoaderVersion(byte[] xLoader, boolean brpFormatted) {
        if (xLoader == null || xLoader.length == 0) {
            return "";
        }

        ByteBuffer buffer;
        ByteBuffer sectionIdBuffer;
        byte[] sectionId = new byte[16];

        buffer = ByteBuffer.wrap(xLoader);
        buffer.position(8);
        buffer.get(sectionId, 0, 16);

        sectionIdBuffer = ByteBuffer.allocate(sectionId.length);
        sectionIdBuffer.put(sectionId);
        sectionIdBuffer.position(13);

        return fetchVersions(sectionIdBuffer, ITableOfContentsItem.FILENAME_XLOADER, brpFormatted);
    }

    private void parseISSW(HashMap<String, String> fileNames, ZipFile zipFile, FileChannel cout) throws IOException {
        ByteBuffer src;
        String version = "";

        String fileName = fileNames.get(ITableOfContentsItem.FILENAME_ISSW);
        if (fileName == null) {
            return;
        }
        File bootImageFile = extractFile(fileName, zipFile);
        ByteBuffer buffer = getImage(bootImageFile);

        version = getISSWVersion(buffer.array(), false);
        src = ByteBuffer.allocate(version.getBytes("UTF-8").length);
        src.put(version.getBytes("UTF-8"));
        src.flip();
        cout.write(src);

    }

    /**
     * @param issw
     *            byte array
     * @param brpFormatted
     *            True if result string should be formatted according to BRP rules
     * @return issw version
     */
    public String getISSWVersion(byte[] issw, boolean brpFormatted) {
        if (issw == null || issw.length == 0) {
            return "";
        }

        ByteBuffer buffer;
        ByteBuffer sectionIdBuffer;
        byte[] sectionId = new byte[16];

        buffer = ByteBuffer.wrap(issw);
        // 0x290 as a start address of customer header (CUST) for issw binary
        // plus 8 bytes as a start address of sectionID
        buffer.position(0x298);
        buffer.get(sectionId, 0, 16);

        sectionIdBuffer = ByteBuffer.allocate(sectionId.length);
        sectionIdBuffer.put(sectionId);
        sectionIdBuffer.position(13);

        return fetchVersions(sectionIdBuffer, ITableOfContentsItem.FILENAME_ISSW, brpFormatted);
    }

    private void parseMeminit(HashMap<String, String> fileNames, ZipFile zipFile, FileChannel cout) throws IOException {

        ByteBuffer src;

        String fileName = fileNames.get(ITableOfContentsItem.FILENAME_MEM_INIT);
        if (fileName == null) {
            return;
        }
        File bootImageFile = extractFile(fileName, zipFile);
        ByteBuffer buffer = getImage(bootImageFile);

        // MEM-INIT
        String version = getMemInitVersion(buffer.array(), false);
        src = ByteBuffer.allocate(version.getBytes("UTF-8").length);
        src.put(version.getBytes("UTF-8"));
        src.flip();
        cout.write(src);
    }

    /**
     * Method searches for version string prefix ("@#" or "@(#)") inside of MEM-INIT binary. Text
     * that follows this string and is terminated by zero byte or end of binary is considered as a
     * version string. If the version string starts with "MEM_INIT" or "SOC-SETTINGS" the method
     * ends immediately, otherwise it tries to find other instance of version string prefix and will
     * use the latest found. Method returns empty version string if no instance of version string
     * prefix is found.
     * 
     * @param memInit
     *            Byte array
     * @param brpFormatted
     *            True if result string should be formatted according to BRP rules
     * @return memInit version
     */
    public String getMemInitVersion(byte[] memInit, boolean brpFormatted) {
        String version = "";

        if (memInit == null || memInit.length == 0) {
            return version;
        }

        int nullBytePosition = memInit.length;
        for (int i = memInit.length - 1; i >= 0; i--) {
            if (memInit[i] == MEM_INIT_VERSION_PREFIX_1.charAt(0) || memInit[i] == MEM_INIT_VERSION_PREFIX_2.charAt(0)) {
                String text = "";
                try {
                    text = new String(memInit, i, nullBytePosition - i, "UTF-8");
                } catch (UnsupportedEncodingException e) {
                    // Should not get here. Every implementation of the Java platform is required to
                    // support UTF-8.
                    e.printStackTrace();
                }
                int prefixLength = getMemInitVersionPrefixLength(text);
                if (prefixLength > 0) {
                    version = text.substring(prefixLength);
                    if (version.startsWith(MEM_INIT_VERSION_PREFIX_LABEL_1)
                        || version.startsWith(MEM_INIT_VERSION_PREFIX_LABEL_2)) {
                        break;
                    }
                }
            }
            if (memInit[i] == 0) {
                nullBytePosition = i;
            }
        }

        StringBuffer versionBuffer = new StringBuffer();
        if (brpFormatted) {
            versionBuffer.append(version);
        } else {
            versionBuffer.append(ITableOfContentsItem.FILENAME_MEM_INIT + COLON + VERSIONING + LINE_SEPARATOR + version
                + SEMICOLON + LINE_SEPARATOR);
        }
        return versionBuffer.toString();
    }

    private int getMemInitVersionPrefixLength(String pText) {
        int prefixLength = 0;
        if (pText.startsWith(MEM_INIT_VERSION_PREFIX_1)) {
            prefixLength = MEM_INIT_VERSION_PREFIX_1.length();
        } else if (pText.startsWith(MEM_INIT_VERSION_PREFIX_2)) {
            prefixLength = MEM_INIT_VERSION_PREFIX_2.length();
        }
        return prefixLength;
    }

    private void parsePowerManagement(HashMap<String, String> fileNames, ZipFile zipFile, FileChannel cout)
        throws IOException {

        ByteBuffer src;
        String version = "";

        String fileName = fileNames.get(ITableOfContentsItem.FILENAME_PWR_MGT);
        if (fileName == null) {
            return;
        }
        File bootImageFile = extractFile(fileName, zipFile);
        ByteBuffer buffer = getImage(bootImageFile);

        // POWER MANAGEMENT
        version = getPwrMgtVersion(buffer.array(), false);
        src = ByteBuffer.allocate(version.getBytes("UTF-8").length);
        src.put(version.getBytes("UTF-8"));
        src.flip();
        cout.write(src);
    }

    /**
     * @param pwrmgt
     *            byte array
     * @param brpFormatted
     *            True if result string should be formatted according to BRP rules
     * @return power management version
     */
    public String getPwrMgtVersion(byte[] pwrmgt, boolean brpFormatted) {
        if (pwrmgt == null || pwrmgt.length == 0) {
            return "";
        }

        ByteBuffer buffer;
        ByteBuffer sectionIdBuffer;
        byte[] sectionId = new byte[16];

        buffer = ByteBuffer.wrap(pwrmgt);
        // gain pwrmgt version; has the same structure as in XLoder
        buffer.position(8);
        buffer.get(sectionId, 0, 16);
        sectionIdBuffer = ByteBuffer.allocate(sectionId.length);
        sectionIdBuffer.put(sectionId);

        sectionIdBuffer.position(13);

        return fetchVersions(sectionIdBuffer, ITableOfContentsItem.FILENAME_PWR_MGT, brpFormatted);
    }

    private void parseModem(HashMap<String, String> fileNames, ZipFile zipFile, FileChannel cout) throws IOException {
        ByteBuffer src;
        String version = "";

        /*
         * #MODEM_DELIVERY=0000 #MODEM_VARIANT=00
         */

        String fileName = fileNames.get(ITableOfContentsItem.FILENAME_MODEM);
        if (fileName == null) {
            return;
        }
        File bootImageFile = extractFile(fileName, zipFile);
        ByteBuffer buffer = getImage(bootImageFile);

        // MODEM
        version = getModemVersion(buffer.array());
        src = ByteBuffer.allocate(version.getBytes("UTF-8").length);
        src.put(version.getBytes("UTF-8"));
        src.flip();
        cout.write(src);
    }

    /**
     * @param pModem
     *            byte array
     * @return modem version
     */
    public String getModemVersion(byte[] pModem) {
        if (pModem == null || pModem.length == 0) {
            return "";
        }

        ByteBuffer buffer;
        // String version = "";
        StringBuffer versionBuffer = new StringBuffer();
        final String delivery = "MODEM_DELIVERY";
        final String variant = "MODEM_VARIANT";

        // MODEM_DELIVERY=0204
        int sizeOfModemDelivery = 0;
        int sizeOfModemVariant = 0;
        try {
            sizeOfModemDelivery = delivery.getBytes("UTF-8").length + "=".getBytes("UTF-8").length + 4;
            sizeOfModemVariant = variant.getBytes("UTF-8").length + "=".getBytes("UTF-8").length + 2;
        } catch (UnsupportedEncodingException e) {
            e.printStackTrace();
        }

        byte[] deliveryArray = new byte[sizeOfModemDelivery];
        byte[] variantArray = new byte[sizeOfModemVariant];

        buffer = ByteBuffer.wrap(pModem);

        while (buffer.hasRemaining()) {
            if (buffer.get() == '#') {
                buffer.get(deliveryArray, 0, sizeOfModemDelivery);
                try {
                    if (new String(deliveryArray, "UTF-8").startsWith(delivery)) {
                        versionBuffer.append(ITableOfContentsItem.FILENAME_MODEM + COLON + VERSIONING + LINE_SEPARATOR);
                        versionBuffer.append("#" + new String(deliveryArray, "UTF-8") + LINE_SEPARATOR);
                        break;
                    }
                } catch (UnsupportedEncodingException e) {
                    e.printStackTrace();
                }
            }
        }

        // gain modem variant
        while (buffer.hasRemaining()) {
            if (buffer.get() == '#') {
                buffer.get(variantArray, 0, sizeOfModemVariant);
                try {
                    if (new String(variantArray, "UTF-8").startsWith(variant)) {
                        versionBuffer.append("#" + new String(variantArray, "UTF-8") + LINE_SEPARATOR);
                        break;
                    }
                } catch (UnsupportedEncodingException e) {
                    e.printStackTrace();
                }
            }
        }

        return versionBuffer.toString();
    }

    private void parseIPL(HashMap<String, String> fileNames, ZipFile zipFile, FileChannel cout) throws IOException {
        ByteBuffer src;

        String fileName = fileNames.get(ITableOfContentsItem.FILENAME_IPL);
        if (fileName == null) {
            return;
        }
        File bootImageFile = extractFile(fileName, zipFile);
        ByteBuffer buffer = getImage(bootImageFile);

        // IPL
        String version = getIPLVersion(buffer.array());
        src = ByteBuffer.allocate(version.getBytes("UTF-8").length);
        src.put(version.getBytes("UTF-8"));
        src.flip();
        cout.write(src);
    }

    /**
     * @param pIpl
     *            byte array
     * @return ipl version
     */
    public String getIPLVersion(byte[] pIpl) {
        if (pIpl == null || pIpl.length == 0) {
            return "";
        }

        ByteBuffer buffer;
        buffer = ByteBuffer.wrap(pIpl);
        byte[] iplTimestamp = new byte[4];
        byte[] iplChecksum = new byte[4];
        // gain ipl version

        buffer.position(0x48); // 0x48 position of timestamp in unix format
        buffer.get(iplTimestamp, 0, 4);

        buffer.position(0x4c); // 0x4c position of checksum in ipl file
        buffer.get(iplChecksum, 0, 4);

        StringBuffer versionBuffer = new StringBuffer();
        versionBuffer.append(ITableOfContentsItem.FILENAME_IPL + COLON + VERSIONING + LINE_SEPARATOR);
        versionBuffer.append(BUILD + " " + HexUtilities.byteArrayToLong(iplChecksum) + " ");
        versionBuffer.append(DATE + COLON + "" + new Date(HexUtilities.byteArrayToLong(iplTimestamp) * 1000));

        return versionBuffer.toString();
    }

    private File extractFile(String fileToExtract, ZipFile zipFile) throws IOException {
        File result = null;
        ZipEntry entry;
        InputStream input = null;
        OutputStream output = null;
        int bytesRead;
        byte[] buffer = new byte[BUFFER_SIZE];

        try {
            entry = zipFile.getEntry(fileToExtract);
            if (entry != null) {
                input = zipFile.getInputStream(entry);
                result = File.createTempFile(fileToExtract, ".bin");
                result.deleteOnExit();
                output = new FileOutputStream(result);
                while ((bytesRead = input.read(buffer, 0, BUFFER_SIZE)) != -1) {
                    output.write(buffer, 0, bytesRead);
                }
            }
        } finally {
            if (input != null) {
                input.close();
            }

            if (output != null) {
                output.close();
            }
        }

        return result;
    }

    private ByteBuffer getImage(File bootImageFile) throws IOException {

        ByteBuffer buffer = ByteBuffer.allocate((int) bootImageFile.length());
        buffer.order(ByteOrder.LITTLE_ENDIAN);
        buffer.rewind();
        FileChannel fc = null;
        FileInputStream stream = null;

        try {
            stream = new FileInputStream(bootImageFile);
            fc = stream.getChannel();
            fc.read(buffer);
        } finally {
            if (fc != null) {
                try {
                    fc.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
            if (stream != null) {
                try {
                    stream.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }

        return buffer;
    }

    private String fetchVersions(ByteBuffer sectionIdBuffer, String inputFileName, boolean brpFormatted) {

        StringBuffer versionBuffer = new StringBuffer();
        // fetch cutId
        byte[] cutIdArray = new byte[4];
        sectionIdBuffer.position(0);
        sectionIdBuffer.get(cutIdArray, 0, 4);
        byte[] convertedCutId = LittleEndianByteConverter.reverse(cutIdArray);
        String cutId = HexUtilities.removeHexPrefix(HexUtilities.toHexString(convertedCutId));

        // fetch BIB
        byte[] bibArray = new byte[3];
        sectionIdBuffer.position(10);
        sectionIdBuffer.get(bibArray, 0, bibArray.length);
        String bib = HexUtilities.removeHexPrefix(HexUtilities.toHexString(bibArray));

        // fetch BIP
        String bip = HexUtilities.removeHexPrefix(HexUtilities.toHexString(sectionIdBuffer.get()));

        // fetch Major version (Revision)
        String majorRevision = HexUtilities.removeHexPrefix(HexUtilities.toHexString(sectionIdBuffer.get()));
        // fetch Major version (Revision)
        String minorRevision = HexUtilities.removeHexPrefix(HexUtilities.toHexString(sectionIdBuffer.get()));

        if (!brpFormatted) {
            versionBuffer.append(inputFileName);
            versionBuffer.append(COLON);
            versionBuffer.append(VERSIONING);
            versionBuffer.append(LINE_SEPARATOR);
            versionBuffer.append(CUT_ID);
            versionBuffer.append(EQUALS);
            versionBuffer.append(cutId);
            versionBuffer.append(LINE_SEPARATOR);
            versionBuffer.append(BIB);
            versionBuffer.append(" ");
            versionBuffer.append(VERSION);
            versionBuffer.append(EQUALS);
            versionBuffer.append(bib);
            versionBuffer.append(LINE_SEPARATOR);
            versionBuffer.append(BIP);
            versionBuffer.append(" ");
            versionBuffer.append(VERSION);
            versionBuffer.append(COLON);
            versionBuffer.append(bip);
            versionBuffer.append(" ");
            versionBuffer.append(REVISION);
            versionBuffer.append(COLON);
            versionBuffer.append(majorRevision);
            versionBuffer.append(DOT);
            versionBuffer.append(minorRevision);
            versionBuffer.append(SEMICOLON);
            versionBuffer.append(LINE_SEPARATOR);
        } else {
            versionBuffer.append(CUT_ID);
            versionBuffer.append(EQUALS);
            versionBuffer.append(cutId);
            versionBuffer.append(", ");
            versionBuffer.append(BIB);
            versionBuffer.append(": ");
            versionBuffer.append(VERSION);
            versionBuffer.append(EQUALS);
            versionBuffer.append(bib);
            versionBuffer.append(", ");
            versionBuffer.append(BIP);
            versionBuffer.append(": ");
            versionBuffer.append(VERSION);
            versionBuffer.append(EQUALS);
            versionBuffer.append(bip);
            versionBuffer.append(" ");
            versionBuffer.append(REVISION);
            versionBuffer.append(EQUALS);
            versionBuffer.append(majorRevision);
            versionBuffer.append(DOT);
            versionBuffer.append(minorRevision);
        }

        return versionBuffer.toString();
    }
}
