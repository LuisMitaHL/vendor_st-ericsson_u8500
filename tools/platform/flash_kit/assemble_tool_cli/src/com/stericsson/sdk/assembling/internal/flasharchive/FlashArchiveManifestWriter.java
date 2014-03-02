package com.stericsson.sdk.assembling.internal.flasharchive;

import java.io.BufferedWriter;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.util.List;
import java.util.ListIterator;

import com.stericsson.sdk.assembling.AssemblerException;
import com.stericsson.sdk.assembling.cli.Assemble;
import com.stericsson.sdk.assembling.internal.ConfigurationReader;
import com.stericsson.sdk.assembling.internal.u8500.U8500ConfigurationEntry;
import com.stericsson.sdk.assembling.internal.u8500.U8500ConfigurationEntryAttribute;
import com.stericsson.sdk.assembling.utilities.ContentTypeUtilities;

/**
 * Utility class for writing a manifest file
 *
 * @author etomjun
 */
public final class FlashArchiveManifestWriter {
    /** Manifest file entry name */
    public static final String DEFAULT_MANIFEST_NAME = "manifest.txt";

    /** The character set to use when writing manifest file */
    private static final String CHARSET = "ISO-8859-1";

    /** The manifest header identifier */
    private static final String MANIFEST_VERSION_ID = "STEFlash-Manifest-Version: ";

    /** Manifest version */
    private static final String VERSION = "2.0";

    /** Name identifier */
    private static final String NAME_ID = "Name: ";

    /** Content type identifier */
    private static final String CONTENT_TYPE_ID = "Content-Type: ";

    /**
     * @param entries
     *            Flash archive entries to write manifest for.
     * @param fileList
     *            A list of absolute path/filenames to entries that should be included.
     * @param output
     *            Output stream to write manifest to.
     * @throws AssemblerException
     *             if any assembler related errors occur
     */
    public static void write(List<U8500ConfigurationEntry> entries, List<String> fileList, ByteArrayOutputStream output)
        throws AssemblerException {
        OutputStreamWriter streamWriter = null;
        try {
            streamWriter = new OutputStreamWriter(output, CHARSET);
        } catch (IOException ioe) {
            throw new AssemblerException(ioe);
        }
        BufferedWriter bufferedWriter = new BufferedWriter(streamWriter);
        PrintWriter writer = new PrintWriter(bufferedWriter);

        writer.println(MANIFEST_VERSION_ID + VERSION);
        writer.println();

        ListIterator<U8500ConfigurationEntry> i = entries.listIterator();
        while (i.hasNext()) {
            U8500ConfigurationEntry entry = i.next();

            if (isEntryInFileList(entry, fileList) || entry.getSource() == null) {

                writer.println(NAME_ID + entry.getName());
                if (Assemble.getTrim()
                    && ContentTypeUtilities.checkContentSubType(entry.getType(), ConfigurationReader.SUBTYPE_INIT_AREA)) {
                    writer.println(CONTENT_TYPE_ID
                        + ContentTypeUtilities.makeContentType(ContentTypeUtilities.getPrefixOfContentType(entry
                            .getType()), ConfigurationReader.SUBTYPE_IMAGE));
                } else {
                    writer.println(CONTENT_TYPE_ID + entry.getType());
                }

                ListIterator<U8500ConfigurationEntryAttribute> j = entry.getAttributes();
                while (j.hasNext()) {
                    U8500ConfigurationEntryAttribute attribute = j.next();
                    writer.println(attribute.getName() + ": " + attribute.getValue());
                }
                writer.println();
            }
        }

        writer.flush();
    }

    /**
     * @param entry
     *            TBD
     * @param fileList
     *            TBD
     * @return TBD
     */
    private static boolean isEntryInFileList(U8500ConfigurationEntry entry, List<String> fileList) {
        boolean result = false;
        ListIterator<String> i = fileList.listIterator();
        while (i.hasNext()) {
            String filename = i.next();
            String symbolicFilename = extractSymbolicFilename(filename);

            if (symbolicFilename == null) {
                if (new File(filename).getName().equalsIgnoreCase(entry.getSource())) {
                    result = true;
                    break;
                }
            } else if (symbolicFilename.equalsIgnoreCase(entry.getSource())) {
                result = true;
                break;
            }

        }

        return result;
    }

    private static String extractSymbolicFilename(String filename) {
        String result = null;
        int p = filename.indexOf('=');

        if (p > 0) {
            result = filename.substring(0, p).trim();
        }

        return result;
    }

    /**
     * Constructor.
     */
    private FlashArchiveManifestWriter() {
    }
}
