package com.stericsson.sdk.assembling.internal.u8500;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.Collections;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.ListIterator;
import java.util.Locale;
import java.util.Vector;

import com.stericsson.sdk.assembling.AssemblerException;
import com.stericsson.sdk.assembling.IAssemblerSettings;
import com.stericsson.sdk.assembling.internal.ConfigurationReader;
import com.stericsson.sdk.assembling.utilities.ContentTypeUtilities;
import com.stericsson.sdk.assembling.utilities.HexUtilities;

/**
 * Class that assembles a binary image on the form [TOC] [image data 1] [image data ..] [image data
 * n]
 *
 * @author xolabju
 *
 */
public class U8500BinaryImageAssembler extends U8500Assembler {

    private U8500BinaryImage image;

    private void init(final String configuration, final String fileList) throws AssemblerException {
        image = new U8500BinaryImage();
        U8500ConfigurationReader parser =
            new U8500ConfigurationReader(configuration, ConfigurationReader.ELEMENT_NAME_RAM_IMAGE);
        entries = parser.getEntries();

        try {
            entryFileList = ConfigurationReader.getFileListFilenames(fileList);
        } catch (IOException e) {
            throw new AssemblerException(e);
        }
    }

    /**
     * Creates a RAM image from a (in the constructor) parsed configuration file.
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

        init(confFile, fileList);

        File imageFile;
        FileOutputStream output;
        ListIterator<U8500ConfigurationEntry> i;

        try {
            matchFileList();

            notifyMessage("Creating binary image...");

            imageFile = new File(outputFile);

            output = new FileOutputStream(imageFile);

            notifyMessage("Extracting boot images...");
            extractBootImages();

            notifyMessage("Assembling image entries...");

            i = entries.listIterator();
            while (i.hasNext()) {
                U8500ConfigurationEntry entry = i.next();
                ListIterator<String> j = entryFileList.listIterator();
                while (j.hasNext()) {
                    String filename = j.next();
                    String symbolicFilename = extractSymbolicFilename(filename);
                    String pathToFile = extractPathToFile(filename);

                    if (symbolicFilename == null) {
                        if (new File(filename).getName().equalsIgnoreCase(entry.getSource())) {
                            notifyMessage("Writing " + pathToFile + " entry as " + entry.getName() + " ("
                                + entry.getSource() + ")...");
                            updateImage(entry, pathToFile);
                        }
                    } else if (symbolicFilename.equalsIgnoreCase(entry.getSource())) {
                        notifyMessage("Writing " + symbolicFilename + "(" + pathToFile + ") entry as "
                            + entry.getName() + " (" + entry.getSource() + ")...");
                        updateImage(entry, pathToFile);
                    }
                }
            }

            notifyMessage("Writing image");

            U8500AssemblerHelper.writeImage(output, image);

            output.close();

            notifyMessage(image.toString());
            notifyMessage("Image " + outputFile + " sucessfully created!");
        } catch (IOException e) {
            throw new AssemblerException(e);
        }
    }

    private void updateImage(U8500ConfigurationEntry entry, String filePath) throws AssemblerException {
        byte[] entryData = U8500AssemblerHelper.readFromFile(filePath);
        U8500TableOfContentsItem tocItem = new U8500TableOfContentsItem();
        String tocId = entry.getAttributeValue(ConfigurationReader.ATTRIBUTE_TOC_ID);
        if (tocId == null) {
            throw new AssemblerException("Failed to get TOC ID for entry " + entry.getName());
        }
        tocItem.setFileName(tocId);
        tocItem.setSize(entryData.length);
        tocItem.setStartAddress(image.getLength());

        String flags = entry.getAttributeValue(ConfigurationReader.ATTRIBUTE_TOC_FLAGS);
        String loadAddress = entry.getAttributeValue(ConfigurationReader.ATTRIBUTE_TOC_LOAD_ADDRESS);
        String entryPoint = entry.getAttributeValue(ConfigurationReader.ATTRIBUTE_TOC_ENTRY_POINT);
        if (flags != null) {
            tocItem.setFlags(Long.decode(flags));
        }
        if (loadAddress != null) {
            tocItem.setLoadAddress(Long.decode(loadAddress));
        }
        if (entryPoint != null) {
            tocItem.setEntryPoint(Long.decode(entryPoint));
        }
        image.addImageItem(tocItem, entryData);
    }

    private void extractBootImages() throws AssemblerException {
        int pos = 0;
        HashMap<Integer, U8500ConfigurationEntry> newEntries = new HashMap<Integer, U8500ConfigurationEntry>();
        Vector<Integer> indexToRemove = new Vector<Integer>();
        Vector<String> newPaths = new Vector<String>();
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
                        // extract the bootimages into images
                        U8500BinaryImage subImage = U8500BinaryImage.createFromFile(pathToFile);
                        U8500TableOfContents toc = subImage.getTOC();
                        int tocIndex = 0;
                        // scan the TOC for data to add to the flash archive
                        for (U8500TableOfContentsItem tocItem : toc.getItems()) {
                            // It's use to substitution subtype image instead of bootimage.
                            // The first, it parse prefix from entry (it's necessary keep to the
                            // same prefix as original one).
                            // The second operation create a whole content type from a prefix and a
                            // subtype.
                            // This content is used to create a new configuration entry.
                            U8500ConfigurationEntry newEntry =
                                new U8500ConfigurationEntry(tocItem.getFileNameString(), ContentTypeUtilities
                                    .makeContentType(ContentTypeUtilities.getPrefixOfContentType(entry.getType()),
                                        ConfigurationReader.SUBTYPE_IMAGE));
                            newEntry.addAttribute(new U8500ConfigurationEntryAttribute(
                                ConfigurationReader.ATTRIBUTE_TOC_ID, tocItem.getFileNameString()));

                            // Source
                            newEntry
                                .setSource("parsed_" + newEntry.getName().toLowerCase(Locale.getDefault()) + ".bin");
                            // TOC-ID

                            // Additional TOC values
                            // load address
                            newEntry.addAttribute(new U8500ConfigurationEntryAttribute(
                                ConfigurationReader.ATTRIBUTE_TOC_LOAD_ADDRESS, HexUtilities.toHexString(tocItem
                                    .getLoadAddress(), true)));
                            // entry point
                            newEntry.addAttribute(new U8500ConfigurationEntryAttribute(
                                ConfigurationReader.ATTRIBUTE_TOC_ENTRY_POINT, HexUtilities.toHexString(tocItem
                                    .getEntryPoint(), true)));
                            // flags
                            newEntry.addAttribute(new U8500ConfigurationEntryAttribute(
                                ConfigurationReader.ATTRIBUTE_TOC_FLAGS, HexUtilities.toHexString(tocItem.getFlags(),
                                    true)));

                            File tempFile =
                                U8500AssemblerHelper.createTempFile(subImage.getImageData(tocItem.getFileNameString()),
                                    newEntry.getSource());
                            if (U8500TableOfContentsItem.FILENAME_ISSW.equals(tocItem.getFileNameString())) {
                                isswIndex = tocIndex + pos;
                            }
                            newEntries.put(tocIndex + pos, newEntry);
                            newPaths.add(tempFile.getAbsolutePath());
                            tocIndex++;
                        }
                        indexToRemove.add(pos);
                    }
                    if (U8500TableOfContentsItem.FILENAME_ISSW.equals(entry
                        .getAttributeValue(ConfigurationReader.ATTRIBUTE_TOC_ID))) {
                        isswIndex = pos;
                    }
                    if (ContentTypeUtilities.checkContentSubType(entry.getType(), ConfigurationReader.SUBTYPE_CRKC)) {
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
            entries.add(isswIndex + 1, crkcEntry);
        }
    }

    private void updateEntries(HashMap<Integer, U8500ConfigurationEntry> newEntries, Vector<Integer> indexToRemove,
        Vector<String> newPaths) {
        for (int i = indexToRemove.size(); i > 0; i--) {
            entries.remove((int) indexToRemove.get(i - 1));
        }

        List<Integer> temp = new Vector<Integer>();
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

}
