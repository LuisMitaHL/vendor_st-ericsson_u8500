package com.stericsson.sdk.assembling.internal.u8500;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.Collections;
import java.util.HashMap;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;
import java.util.ListIterator;
import java.util.Locale;
import java.util.zip.ZipOutputStream;

import com.stericsson.sdk.assembling.AssemblerException;
import com.stericsson.sdk.assembling.IAssemblerSettings;
import com.stericsson.sdk.assembling.internal.ConfigurationReader;
import com.stericsson.sdk.assembling.internal.flasharchive.MinimizedJFFSImageEntryWriter;
import com.stericsson.sdk.assembling.internal.flasharchive.MinimizedSparseEXT4ImageEntryWriter;
import com.stericsson.sdk.assembling.internal.preflash.PreflashUtil;
import com.stericsson.sdk.assembling.internal.preflash.SRecordConverter;
import com.stericsson.sdk.assembling.utilities.ContentTypeUtilities;
import com.stericsson.sdk.assembling.utilities.HexUtilities;
import com.stericsson.sdk.assembling.utilities.StreamCloser;
import com.stericsson.sdk.common.gdfs.GDFSBinaryFileWriter;
import com.stericsson.sdk.common.image.ITableOfContentsItem;

/**
 * The FlashArchiveBuilder class can create a flash archive using input from a configuration file
 * and the paths/filenames in a file list file.
 *
 * Current design/implementation of a flash archive is a ZIP archive with the following layout:
 *
 * [root] signature.sf archive.zip
 *
 * The actually contents that is going to be flashed is inside the archive.zip entry.
 *
 * A manifest file is located in the root of the archive.zip entry is used by the loader to locate
 * flashable entries and (also depending on the Content-Type) where to put these entries in the
 * flash memory.
 *
 * @author xolabju
 */
public class U8500FlashArchiveAssembler extends U8500Assembler {

    private static final String VERSION_FILE = "version.txt";

    private File versionFile = null;

    private void init(final String configuration, final String fileList, final IAssemblerSettings settings)
        throws AssemblerException {
        U8500ConfigurationReader parser =
            new U8500ConfigurationReader(configuration, ConfigurationReader.ELEMENT_NAME_FLASH_ARCHIVE);
        entries = parser.getEntries();
        versionFile = new File(VERSION_FILE);
        versionFile.deleteOnExit();

        try {
            entryFileList = ConfigurationReader.getFileListFilenames(fileList);
        } catch (IOException e) {
            throw new AssemblerException(e);
        }

        Object bufferSetting = settings.get(IAssemblerSettings.KEY_BUFFER_SIZE);
        if (bufferSetting != null) {
            int bufferSettingInt = (Integer) bufferSetting;
            U8500AssemblerHelper.setBufferSize(bufferSettingInt);
        }

        Object alignmentSetting = settings.get(IAssemblerSettings.KEY_ALIGNMENT_SIZE);
        if (alignmentSetting != null) {
            int alignmentSettingInt = (Integer) alignmentSetting;
            U8500AssemblerHelper.setAlignmentSize(alignmentSettingInt);
        }
    }

    /**
     * Creates a flash archive from a (in the constructor) parsed configuration file.
     *
     * @param settings
     *            the assembler settings
     * @throws AssemblerException
     *             In case of an assembler error.
     */
    public void assemble(IAssemblerSettings settings) throws AssemblerException {
        String confFile = (String) settings.get(IAssemblerSettings.KEY_CONFIGURATION_FILE);
        String fileList = (String) settings.get(IAssemblerSettings.KEY_FILE_LIST_FILE);
        String outputFile = (String) settings.get(IAssemblerSettings.KEY_OUTPUT_FILE);
        List<U8500ConfigurationEntry> removedPreflashEntries = new LinkedList<U8500ConfigurationEntry>();
        List<String> removedPreflashFileNames = new LinkedList<String>();
        List<U8500ConfigurationEntry> addedPreflashEntries = new LinkedList<U8500ConfigurationEntry>();
        List<U8500ConfigurationEntry> gdfsEntries = new LinkedList<U8500ConfigurationEntry>();

        init(confFile, fileList, settings);

        File archiveFile;
        ZipOutputStream output = null;
        ListIterator<U8500ConfigurationEntry> i;

        try {
            matchFileList();

            notifyMessage("Creating archive...");

            archiveFile = new File(outputFile);

            output = new ZipOutputStream(new FileOutputStream(archiveFile));

            notifyMessage("Extracting boot images...");
            extractBootImages();

            notifyMessage("Writing archive entries...");

            List<U8500ConfigurationEntry> writtenEntries = new LinkedList<U8500ConfigurationEntry>();
            i = entries.listIterator();
            while (i.hasNext()) {
                U8500ConfigurationEntry entry = i.next();
                ListIterator<String> j = entryFileList.listIterator();
                while (j.hasNext()) {
                    String filename = j.next();
                    String symbolicFilename = extractSymbolicFilename(filename);
                    String pathToFile = extractPathToFile(filename);
                    if (ContentTypeUtilities.checkContentSubType(entry.getType(), ConfigurationReader.SUBTYPE_PREFLASH)) {
                        removedPreflashEntries.add(entry);
                        notifyMessage("Writing preflash entry " + entry.getName());
                        addedPreflashEntries.addAll(writePreflashImageEntry(archiveFile, output, entry, pathToFile));
                        removedPreflashFileNames.add(filename);
                    } else {
                        if ((symbolicFilename != null && symbolicFilename.equalsIgnoreCase(entry.getSource()))
                            || new File(filename).getName().equalsIgnoreCase(entry.getSource())) {
                            if (ContentTypeUtilities.checkContentSubType(entry.getType(),
                                ConfigurationReader.SUBTYPE_GDFS)) {
                                notifyMessage("Writing GDFS entry " + entry.getName());
                                gdfsEntries.add(writeGDFSEntry(archiveFile, output, entry, pathToFile));
                            } else {
                                notifyMessage("Writing " + pathToFile + " entry as " + entry.getName() + " ("
                                    + entry.getSource() + ")...");
                                writtenEntries.addAll(writeEntry(archiveFile, output, entry, pathToFile));
                                updateVersionFile(entry, pathToFile, versionFile.getAbsolutePath());
                            }
                        }
                    }
                }
            }
            writtenEntries.removeAll(removedPreflashEntries);
            entryFileList.removeAll(removedPreflashFileNames);
            for (U8500ConfigurationEntry e : addedPreflashEntries) {
                writtenEntries.add(e);
                entryFileList.add(e.getSource());
            }

            for (U8500ConfigurationEntry e : gdfsEntries) {
                writtenEntries.add(e);
            }

            notifyMessage("Writing archive manifest");

            byte[] manifest = U8500AssemblerHelper.writeManifest(archiveFile, output, writtenEntries, entryFileList);

            notifyMessage("Archive manifest: \n" + new String(manifest, "UTF-8"));

            handleVersioning(archiveFile, output);

            notifyMessage("Flash archive: " + outputFile + " sucessfully created!");
        } catch (Exception e) {
            throw new AssemblerException(e);
        } finally {
            StreamCloser.close(output);
        }
    }

    private List<U8500ConfigurationEntry> writeEntry(File archiveFile, ZipOutputStream output,
        U8500ConfigurationEntry entry, String pathToFile) throws AssemblerException {
        File file = new File(pathToFile);
        if (file.length() > 0xFFFFFFFFL) {
            throw new AssemblerException("Archive entry '" + entry.getName() + "' is larger than 4 GB");
        }

        // If skipping of unused blocks is enabled for the entry, it will be written as a minimized
        // JFFS image, otherwise it will be written as a minimized sparse EXT4 image or as a generic
        // file entry.
        if (entry.isSkipUnusedBlocks()) {
            return MinimizedJFFSImageEntryWriter.writeEntry(archiveFile, output, entry, pathToFile);
        } else if (MinimizedSparseEXT4ImageEntryWriter.isSparseEXT4Image(pathToFile)) {
            return MinimizedSparseEXT4ImageEntryWriter.writeEntry(archiveFile, output, entry, pathToFile);
        } else {
            U8500AssemblerHelper.writeEntry(archiveFile, output, entry, pathToFile);
            List<U8500ConfigurationEntry> entries = new LinkedList<U8500ConfigurationEntry>();
            entries.add(entry);
            return entries;
        }
    }

    private U8500ConfigurationEntry writeGDFSEntry(File archiveFile, ZipOutputStream output,
        U8500ConfigurationEntry entry, String pathToFile) throws AssemblerException {
        File file = new File(pathToFile);
        File binaryFile = null;

        try {
            binaryFile = File.createTempFile("gdfs", null);
            binaryFile.deleteOnExit();

            GDFSBinaryFileWriter.writeGDFSBinaryFile(binaryFile, file);
        } catch (Exception e) {
            throw new AssemblerException(e);
        }

        U8500AssemblerHelper.writeEntry(archiveFile, output, entry, binaryFile.getAbsolutePath());

        return entry;
    }

    private void handleVersioning(File archiveFile, ZipOutputStream output) throws Exception {
        if (new File(VERSION_FILE).exists()) {
            notifyMessage("Generating version information");
            U8500AssemblerHelper.attachVersioning(VERSION_FILE, archiveFile, output);
        }
    }

    private void extractBootImages() throws AssemblerException {
        int pos = 0;
        HashMap<Integer, U8500ConfigurationEntry> newEntries = new HashMap<Integer, U8500ConfigurationEntry>();
        LinkedList<Integer> indexToRemove = new LinkedList<Integer>();
        LinkedList<String> newPaths = new LinkedList<String>();
        Iterator<U8500ConfigurationEntry> entryIter = entries.iterator();
        int isswIndex = 0;
        U8500ConfigurationEntry crkcEntry = null;
        // search the entries for bootimages
        while (entryIter.hasNext()) {
            U8500ConfigurationEntry entry = entryIter.next();
            ListIterator<String> fileIter = entryFileList.listIterator();
            while (fileIter.hasNext()) {
                String filename = fileIter.next();
                String symbolicFilename = extractSymbolicFilename(filename);
                String pathToFile = extractPathToFile(filename);
                if ((symbolicFilename == null ? new File(filename).getName() : symbolicFilename).equalsIgnoreCase(entry
                    .getSource())) {
                    if (ContentTypeUtilities
                        .checkContentSubType(entry.getType(), ConfigurationReader.SUBTYPE_BOOTIMAGE)) {
                        U8500BinaryImage image = U8500BinaryImage.createFromFile(pathToFile);
                        U8500TableOfContents toc = image.getTOC();
                        String[] fileNameParts = extractFileNameParts(entry.getName());
                        // extract the bootimages into images
                        if (entry.getAttributeValue(ConfigurationReader.ATTRIBUTE_TOC_ID) == null) {
                            int tocIndex = 0;
                            // scan the TOC for data to add to the flash archive
                            for (U8500TableOfContentsItem tocItem : toc.getItems()) {
                                U8500ConfigurationEntry newEntry = getBootImageSubEntry(entry, tocItem, fileNameParts);
                                File tempFile =
                                    U8500AssemblerHelper.createTempFile(image.getImageData(tocItem.getFileNameString()),
                                        newEntry.getName());
                                isswIndex = setIndexIfIssw(tocItem.getFileNameString(), tocIndex, pos, isswIndex);
                                long len = tempFile.length();
                                U8500ConfigurationEntryAttribute attrSize =
                                    new U8500ConfigurationEntryAttribute(ConfigurationReader.ATTRIBUTE_NAME_SIZE,
                                        HexUtilities.toHexString(len));
                                newEntry.addAttribute(attrSize);
                                newEntries.put(tocIndex + pos, newEntry);
                                newPaths.add(tempFile.getAbsolutePath());
                                tocIndex++;
                            }
                            indexToRemove.add(pos);
                        } else if (U8500TableOfContentsItem.FILENAME_ISSW.equals(entry
                                .getAttributeValue(ConfigurationReader.ATTRIBUTE_TOC_ID)) 
                                || U8500TableOfContentsItem.FILENAME_XLOADER.equals(entry
                                        .getAttributeValue(ConfigurationReader.ATTRIBUTE_TOC_ID))) {

                            U8500TableOfContentsItem tocItem = toc.getItem(entry.
                                    getAttributeValue(ConfigurationReader.ATTRIBUTE_TOC_ID));
                            U8500ConfigurationEntry newEntry = getBootImageSubEntry(entry, tocItem, fileNameParts);
//                          revert back to the start address set in config xml file 
                            String targetAttr = entry.getAttribute(ConfigurationReader.ATTRIBUTE_NAME_TARGET).getValue();
                            newEntry.getAttribute(ConfigurationReader.ATTRIBUTE_NAME_TARGET).setValue(targetAttr);
                            File tempFile =
                                U8500AssemblerHelper.createTempFile(image.getImageData(tocItem.getFileNameString()),
                                    newEntry.getName());
                            long len = tempFile.length();
                            U8500ConfigurationEntryAttribute attrSize =
                                new U8500ConfigurationEntryAttribute(ConfigurationReader.ATTRIBUTE_NAME_SIZE,
                                    HexUtilities.toHexString(len));
                            newEntry.addAttribute(attrSize);
                            newEntries.put(pos, newEntry);
                            newPaths.add(tempFile.getAbsolutePath());
                            indexToRemove.add(pos);
                        }
                    }
                    if (U8500TableOfContentsItem.FILENAME_ISSW.equals(entry
                        .getAttributeValue(ConfigurationReader.ATTRIBUTE_TOC_ID))) {
                        long isswLen = getFileSize(pathToFile);
                        U8500ConfigurationEntryAttribute attrSize =
                            new U8500ConfigurationEntryAttribute(ConfigurationReader.ATTRIBUTE_NAME_SIZE, HexUtilities
                                .toHexString(isswLen));
                        entry.addAttribute(attrSize);

                        isswIndex = pos;
                    }
                    if (ContentTypeUtilities.checkContentSubType(entry.getType(), ConfigurationReader.SUBTYPE_CRKC)
                        || U8500TableOfContentsItem.FILENAME_CRKC.equals(entry
                            .getAttributeValue(ConfigurationReader.ATTRIBUTE_TOC_ID))) {
                        long crkcLen = getFileSize(pathToFile);
                        U8500ConfigurationEntryAttribute attrSize =
                            new U8500ConfigurationEntryAttribute(ConfigurationReader.ATTRIBUTE_NAME_SIZE, HexUtilities
                                .toHexString(crkcLen));
                        entry.addAttribute(attrSize);

                        crkcEntry = entry;
                        indexToRemove.add(pos);
                    }
                }
            }
            pos++;
        }

        // update the entries
        updateEntries(newEntries, indexToRemove, newPaths);

        // CRKC entry must be placed right after ISSW entry
        if (crkcEntry != null) {
            // getting ISSW start address and length
            U8500ConfigurationEntry isswEntry = entries.get(isswIndex);
            String isswLen = isswEntry.getAttribute(ConfigurationReader.ATTRIBUTE_NAME_SIZE).getValue();
            int iisswLen = HexUtilities.hexStringToInt(isswLen);
            String isswStart = isswEntry.getAttribute(ConfigurationReader.ATTRIBUTE_NAME_TARGET).getValue();
            int iisswStart = HexUtilities.hexStringToInt(isswStart.substring(isswStart.indexOf(",") + 2));

            // counting CRKC start address
            String crkcTargetAttr = crkcEntry.getAttribute(ConfigurationReader.ATTRIBUTE_NAME_TARGET).getValue();
            String crkcStart = HexUtilities.toHexString(iisswStart + iisswLen, true);
            crkcTargetAttr =
                crkcTargetAttr.replace(crkcTargetAttr.substring(crkcTargetAttr.indexOf(",") + 2), crkcStart);
            crkcEntry.getAttribute(ConfigurationReader.ATTRIBUTE_NAME_TARGET).setValue(crkcTargetAttr);

            // counting XLoader start address
            U8500ConfigurationEntry xloaderEntry = entries.get(isswIndex + 1);
            String xloaderTargetAttr = xloaderEntry.getAttribute(ConfigurationReader.ATTRIBUTE_NAME_TARGET).getValue();
            String crkcLen = crkcEntry.getAttribute(ConfigurationReader.ATTRIBUTE_NAME_SIZE).getValue();
            int icrkcLen = HexUtilities.hexStringToInt(crkcLen);
            String xloaderStart = HexUtilities.toHexString(iisswStart + iisswLen + icrkcLen, true);
            xloaderTargetAttr =
                xloaderTargetAttr
                    .replace(xloaderTargetAttr.substring(xloaderTargetAttr.indexOf(",") + 2), xloaderStart);
            xloaderEntry.getAttribute(ConfigurationReader.ATTRIBUTE_NAME_TARGET).setValue(xloaderTargetAttr);

            entries.add(isswIndex + 1, crkcEntry);

        }
    }

//  Helper method to get around max allowed nested if-then
    private int setIndexIfIssw(String fileName, int tocIndex, int pos, int isswIndex) {
        if (U8500TableOfContentsItem.FILENAME_ISSW.equals(fileName)) {
            return tocIndex + pos;
        } else {
            return isswIndex;
        }
    }

    private U8500ConfigurationEntry getBootImageSubEntry(U8500ConfigurationEntry bootImageEntry,
        U8500TableOfContentsItem tocItem, String[] fileNameParts) throws AssemblerException {
        // It's use to substitution subtype image instead of bootimage.
        // The first, it parse prefix from entry (it's necessary keep to the
        // same prefix as original one).
        // The second operation create a whole content type from a prefix and a
        // subtype.
        // This content is used to create a new configuration entry.
        U8500ConfigurationEntry newEntry =
            new U8500ConfigurationEntry(fileNameParts[0] + "_"
                + tocItem.getFileNameString().toLowerCase(Locale.getDefault()) + fileNameParts[1], ContentTypeUtilities
                .makeContentType(ContentTypeUtilities.getPrefixOfContentType(bootImageEntry.getType()),
                    ConfigurationReader.SUBTYPE_IMAGE));

        // Source
        newEntry.setSource(newEntry.getName());
        // TOC-ID
        newEntry.addAttribute(new U8500ConfigurationEntryAttribute(ConfigurationReader.ATTRIBUTE_TOC_ID, tocItem
            .getFileNameString()));
        // VERSION-ID
        newEntry.addAttribute(new U8500ConfigurationEntryAttribute(ConfigurationReader.ATTRIBUTE_VERSION_ID, tocItem
            .getFileNameString()));
        // Target
        U8500ConfigurationEntryAttribute targetAttr =
            createTargetAttribute(bootImageEntry.getAttribute(ConfigurationReader.ATTRIBUTE_NAME_TARGET), tocItem
                .getStartAddress());
        if (targetAttr != null) {
            newEntry.addAttribute(targetAttr);
        }
        // SIZE-MODE PartitionSize
        U8500ConfigurationEntryAttribute sizeModeAttr = createSizeModeAttribute(bootImageEntry.getAttribute(ConfigurationReader.ATTRIBUTE_SIZE_MODE), tocItem.getStartAddress());
        if (sizeModeAttr != null){
        	newEntry.addAttribute(sizeModeAttr);
        }
        // Additional TOC values
        if (U8500TableOfContentsItem.FILENAME_NORMAL.equals(tocItem.getFileNameString())) {
            newEntry.addAttribute(new U8500ConfigurationEntryAttribute(ConfigurationReader.ATTRIBUTE_TOC_LOAD_ADDRESS,
                HexUtilities.toHexString(tocItem.getLoadAddress(), true)));
            newEntry.addAttribute(new U8500ConfigurationEntryAttribute(ConfigurationReader.ATTRIBUTE_TOC_ENTRY_POINT,
                HexUtilities.toHexString(tocItem.getEntryPoint(), true)));
        } else if (U8500TableOfContentsItem.FILENAME_PRODUCTION.equals(tocItem.getFileNameString())) {
            newEntry.addAttribute(new U8500ConfigurationEntryAttribute(ConfigurationReader.ATTRIBUTE_TOC_ENTRY_POINT,
                HexUtilities.toHexString(tocItem.getEntryPoint(), true)));

        }

        return newEntry;
    }

    private void updateEntries(HashMap<Integer, U8500ConfigurationEntry> newEntries, List<Integer> indexToRemove,
        List<String> newPaths) {
        for (int i = indexToRemove.size(); i > 0; i--) {
            entries.remove((int) indexToRemove.get(i - 1));
        }

        List<Integer> temp = new LinkedList<Integer>();
        temp.addAll(newEntries.keySet());
        Collections.sort(temp);

        for (Integer key : temp) {
            if (key.intValue() <= (entries.size() - 1)) {
                entries.add(key, newEntries.get(key));
            } else {
                entries.add(newEntries.get(key));
            }
        }
        entryFileList.addAll(newPaths);

    }

    private U8500ConfigurationEntryAttribute createTargetAttribute(U8500ConfigurationEntryAttribute attribute,
        int address) {
        U8500ConfigurationEntryAttribute newAttr = null;

        if (attribute != null) {
            newAttr = new U8500ConfigurationEntryAttribute(attribute.getName());
            String attrValue = attribute.getValue();
            String newValue = "";
            String target = "";
            if (attrValue != null) {
                String[] split = attrValue.split(",");
                if (split != null && split.length > 0) {
                    target = split[0];
                }
            }
            newValue = target + ", " + HexUtilities.toHexString(address, true);
            newAttr.setValue(newValue);
        }

        return newAttr;
    }

    private U8500ConfigurationEntryAttribute createSizeModeAttribute(U8500ConfigurationEntryAttribute attribute,
            int address) {
            U8500ConfigurationEntryAttribute newAttr = null;
            if (attribute != null) {
                newAttr = new U8500ConfigurationEntryAttribute(attribute.getName());
                newAttr.setValue(attribute.getValue());
            }
            return newAttr;
        }

    private String[] extractFileNameParts(String fileName) {
        String[] parts = new String[] {
            "", ""};
        if (fileName != null && fileName.length() > 0) {
            int dotPos = fileName.lastIndexOf(".");
            if (dotPos != -1) {
                parts[0] = fileName.substring(0, dotPos);
                parts[1] = fileName.substring(dotPos, fileName.length());
            } else {
                parts[0] = fileName;
            }
        }
        return parts;
    }

    private long getFileSize(String filePath) {
        File file = new File(filePath);
        if (file.exists()) {
            return file.length();
        }
        return 0;
    }

    /**
     * Writes the preflash image entry
     *
     * @param archiveFile
     *            Output archive file
     * @param output
     *            the zip output stream
     * @param entry
     *            the preflash entry to write
     * @param a01File
     *            the preflash image file name
     * @throws AssemblerException
     *             on errors
     * @throws IOException
     *             on errors
     * @return the entries created by converting and splitting the preflash image
     */
    public static List<U8500ConfigurationEntry> writePreflashImageEntry(File archiveFile, ZipOutputStream output,
        U8500ConfigurationEntry entry, String a01File) throws AssemblerException, IOException {
        SRecordConverter converter = new SRecordConverter();
        List<U8500ConfigurationEntry> newEntries = new LinkedList<U8500ConfigurationEntry>();
        String[] splitA01 = PreflashUtil.splitA01(a01File);
        if (splitA01 == null || splitA01.length == 0) {
            throw new AssemblerException("Failed to split file " + a01File);
        }
        String target = U8500AssemblerHelper.getTarget(entry);
        for (String a01part : splitA01) {
            String entryName = new File(a01part).getName();
            int dotPos = entryName.lastIndexOf(".");
            if (dotPos == -1) {
                entryName += ".bin";
            } else {
                entryName = entryName.substring(0, dotPos) + ".bin";
            }
            U8500ConfigurationEntry splitEntry = new U8500ConfigurationEntry(entryName, entry.getType());
            String convertedA01Split = converter.convertA01ToBin(splitEntry, a01part);
            splitEntry.setSource(new File(convertedA01Split).getName());
            long address = 0;
            address = converter.getFirstAddressInS3File(a01part, 0x00000000, 0xffffffffL);
            U8500ConfigurationEntryAttribute targetAttr =
                new U8500ConfigurationEntryAttribute(ConfigurationReader.ATTRIBUTE_NAME_TARGET);
            String hexAddress = HexUtilities.toHexString(address);
            targetAttr.setValue(target + ", " + HexUtilities.toHexString(hexAddress, 4));
            splitEntry.addAttribute(targetAttr);
            U8500ConfigurationEntryAttribute sizeAttr =
                new U8500ConfigurationEntryAttribute(ConfigurationReader.ATTRIBUTE_NAME_SIZE);
            String sizeHex = HexUtilities.toHexString(new File(convertedA01Split).length());
            sizeAttr.setValue(HexUtilities.toHexString(sizeHex, 4));
            splitEntry.addAttribute(sizeAttr);
            newEntries.add(splitEntry);
            String convertedSplitFilename = new File(convertedA01Split).getAbsolutePath();
            U8500AssemblerHelper.writeEntry(archiveFile, output, splitEntry, convertedSplitFilename);
        }
        return newEntries;
    }

    private void updateVersionFile(U8500ConfigurationEntry entry, String filePath, String versionFilePath)
        throws IOException {
        if (supportsVersioning(entry)) {
            U8500AssemblerHelper.updateVersionFile(entry, filePath, versionFilePath);
        }

    }

    private boolean supportsVersioning(U8500ConfigurationEntry entry) {
        U8500ConfigurationEntryAttribute versionId = entry.getAttribute(ConfigurationReader.ATTRIBUTE_VERSION_ID);
        if (versionId == null) {
            return false;
        }
        if (ITableOfContentsItem.FILENAME_XLOADER.equalsIgnoreCase(versionId.getValue())
            || ITableOfContentsItem.FILENAME_ISSW.equalsIgnoreCase(versionId.getValue())
            || ITableOfContentsItem.FILENAME_MEM_INIT.equalsIgnoreCase(versionId.getValue())) {
            return true;
        } else if (ITableOfContentsItem.FILENAME_PWR_MGT.equalsIgnoreCase(versionId.getValue())
            || ITableOfContentsItem.FILENAME_MODEM.equalsIgnoreCase(versionId.getValue())
            || ITableOfContentsItem.FILENAME_IPL.equalsIgnoreCase(versionId.getValue())) {
            return true;
        }

        return false;
    }
}
