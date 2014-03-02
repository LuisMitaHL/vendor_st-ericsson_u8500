/*
 * Copyright (C) ST-Ericsson SA 2010
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * - Neither the name of the ST-Ericsson SA nor the names of its contributors
 * may be used to endorse or promote products derived from this software
 * without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

package com.stericsson.bluetooth.ftp;

import android.util.Log;
import android.util.Xml;

import java.io.File;
import java.io.IOException;
import java.io.StringWriter;

import org.xmlpull.v1.XmlSerializer;

/** Provides interface to compose folder listing in XML format */
public class BluetoothFtpUtils {
    private static final String TAG = "BluetoothFtpFileListComposer";

    private static final String XML_CHARSET = "UTF-8";
    private static final String XML_DOCDECL = " folder-listing SYSTEM \"obex-folder-listing.dtd\"";
    private static final String XML_ROOT_TAG = "folder-listing";
    private static final String XML_PARENT_FOLDER_TAG = "parent-folder";
    private static final String XML_FOLDER_TAG = "folder";
    private static final String XML_FILE_TAG = "file";

    private BluetoothFtpUtils() {
    }

    public static String composeDirListing(String dir, boolean hasParent) {
        StringWriter writer = new StringWriter();
        File[] files = new File(dir).listFiles();

        if (files == null) {
            Log.e(TAG, "Cannot retrieve list of files");
            return null;
        }

        try {
            XmlSerializer xml = Xml.newSerializer();
            xml.setOutput(writer);

            xml.startDocument(XML_CHARSET, false);
            xml.docdecl(XML_DOCDECL);

            xml.startTag("", XML_ROOT_TAG);
            xml.attribute("", "version", "1.0");

            if (hasParent) {
                xml.startTag("", XML_PARENT_FOLDER_TAG);
                xml.endTag("", XML_PARENT_FOLDER_TAG);
            }

            for (File file : files) {
                String tag = file.isDirectory() ? XML_FOLDER_TAG : XML_FILE_TAG;

                xml.startTag("", tag);
                xml.attribute("", "name", file.getName());
                if (!file.isDirectory()) {
                    xml.attribute("", "size", new Long(file.length()).toString());
                    xml.attribute("", "modified", new Long(file.lastModified()).toString());
                }
                xml.endTag("", tag);
            }

            xml.endTag("", XML_ROOT_TAG);

            xml.endDocument();
        } catch (IOException e) {
            Log.e(TAG, "Exception when creating folder listing: " + e);
            return null;
        } catch (IllegalArgumentException e) {
            Log.e(TAG, "Exception when creating folder listing: " + e);
            return null;
        } catch (IllegalStateException e) {
            Log.e(TAG, "Exception when creating folder listing: " + e);
            return null;
        }

        return writer.toString();
    }
}
