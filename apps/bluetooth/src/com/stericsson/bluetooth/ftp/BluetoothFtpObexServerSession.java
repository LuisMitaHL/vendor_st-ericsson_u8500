/*
 * Copyright (c) 2008-2009, Motorola, Inc.
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
 * - Neither the name of the Motorola, Inc. nor the names of its contributors
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

import android.content.Context;
import android.content.Intent;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.os.PowerManager;
import android.os.PowerManager.WakeLock;
import android.util.Log;
import android.webkit.MimeTypeMap;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Arrays;
import java.util.Stack;

import javax.obex.HeaderSet;
import javax.obex.ObexTransport;
import javax.obex.Operation;
import javax.obex.ResponseCodes;
import javax.obex.ServerRequestHandler;
import javax.obex.ServerSession;

/** Handles requests for OBEX server */
public class BluetoothFtpObexServerSession extends ServerRequestHandler {
    private static final String TAG = "BluetoothFtpObexServerSession";
    private static final boolean D = Constants.DEBUG;
    private static final boolean V = Constants.VERBOSE;

    private static final byte[] FTP_TARGET = new byte[] {
            (byte)0xf9, (byte)0xec, 0x7b, (byte)0xc4, (byte)0x95, 0x3c, 0x11, (byte)0xd2,
            (byte)0x98, 0x4e, 0x52, 0x54, 0x00, (byte)0xdc, (byte)0x9e, 0x09
    };

    private Handler mCallback = null;

    private ObexTransport mTransport;

    private Context mContext;

    private ServerSession mSession;

    private WakeLock mWakeLock;

    private String mRootDir = null;

    private Stack<String> mCurrentDir = null;

    public BluetoothFtpObexServerSession(Handler callback, Context context, ObexTransport transport) {
        mCallback = callback;
        mContext = context;
        mTransport = transport;
        PowerManager pm = (PowerManager) mContext.getSystemService(Context.POWER_SERVICE);
        mWakeLock = pm.newWakeLock(PowerManager.PARTIAL_WAKE_LOCK, TAG);

        setupRootDir();
        if (D) Log.d(TAG, "Initialized root directory to " + mRootDir);
    }

    public void start() {
        mWakeLock.acquire();
        try {
            mSession = new ServerSession(mTransport, this, null);
        } catch (IOException e) {
            Log.e(TAG, "Exception when creating session: " + e);
        }
    }

    public void stop() {
        if (mSession != null) {
            try {
                mSession.close();
                mTransport.close();
            } catch (IOException e) {
                Log.e(TAG, "Exception when closing session: " + e);
            }
        }
        mSession = null;
    }

    @Override
    public int onConnect(HeaderSet request, HeaderSet reply) {
        if (V) Log.v(TAG, "onConnect called");

        if (mRootDir == null) {
            return ResponseCodes.OBEX_HTTP_INTERNAL_ERROR;
        }

        try {
            byte[] target = (byte[])request.getHeader(HeaderSet.TARGET);

            if (target == null) {
                Log.e(TAG, "Rejecting connection due to CONNECT with no TARGET given");
                return ResponseCodes.OBEX_HTTP_NOT_ACCEPTABLE;
            }

            if (!Arrays.equals(target,  FTP_TARGET)) {
                Log.e(TAG, "Rejecting connection due to CONNECT with invalid TARGET: " +
                                                                            target.toString());
                return ResponseCodes.OBEX_HTTP_NOT_ACCEPTABLE;
            }
            reply.setHeader(HeaderSet.WHO, target);
        } catch (IOException e) {
            Log.e(TAG, "Exception when handling CONNECT: " + e);
            return ResponseCodes.OBEX_HTTP_INTERNAL_ERROR;
        }

        mCurrentDir = new Stack<String>();

        return ResponseCodes.OBEX_HTTP_OK;
    }

    @Override
    public void onDisconnect(HeaderSet req, HeaderSet resp) {
        if (V) Log.v(TAG, "onDisconnect called");

        resp.responseCode = ResponseCodes.OBEX_HTTP_OK;
    }

    @Override
    public void onClose() {
        if (V) Log.v(TAG, "onClose called");

        if (mWakeLock.isHeld()) {
            mWakeLock.release();
        }

        if (mCallback != null) {
            Message msg = Message.obtain(mCallback);
            msg.what = BluetoothFtpService.MSG_SERVERSESSION_CLOSED;
            msg.sendToTarget();
       }
    }

    @Override
    public int onGet(Operation op) {
        if (V) Log.v(TAG, "onGet called");

        try {
            HeaderSet request = op.getReceivedHeader();
            String name = (String) request.getHeader(HeaderSet.NAME);
            String type = (String) request.getHeader(HeaderSet.TYPE);
            File path;

            if (D) Log.d(TAG, "GET request: name=" + name + ", type=" + type + ", cwd=" + getCurrentDir());

            if (name == null) {
                path = new File(getCurrentDir());
            } else {
                path = new File(getCurrentDir(), name);
            }

            if (!path.exists()) {
                Log.i(TAG, "Path does not exist: " + path.getAbsolutePath());
                return ResponseCodes.OBEX_HTTP_NOT_FOUND;
            }

            // we only support x-obex/folder-listing
            if (type != null && type.startsWith("x-obex/")) {
                if (!type.equals("x-obex/folder-listing")) {
                    return ResponseCodes.OBEX_HTTP_NOT_IMPLEMENTED;
                }

                if (!path.isDirectory()) {
                    return ResponseCodes.OBEX_HTTP_BAD_METHOD;
                }

                return sendFileList(op, path, !mCurrentDir.empty());
            }

            // now we know request should be to pull file
            if (path.isDirectory()) {
                return ResponseCodes.OBEX_HTTP_BAD_METHOD;
            }

            return sendFile(op, path);
        } catch (IOException e) {
            Log.e(TAG, "Exception when handling GET: " + e);
        }

        return ResponseCodes.OBEX_HTTP_INTERNAL_ERROR;
    }

    private int sendFileList(Operation op, File path, boolean hasParent) {
        if (V) Log.v(TAG, "Sending folder listing of " + path.getAbsolutePath() +
                                                                    " (" + hasParent + ")");

        try {
            String list = BluetoothFtpUtils.composeDirListing(path.getAbsolutePath(), hasParent);

            OutputStream stream = op.openOutputStream();
            stream.write(list.getBytes());
            stream.flush();
        } catch (IOException e) {
            Log.e(TAG, "Exception when handling folder listing: " + e);
            return ResponseCodes.OBEX_HTTP_INTERNAL_ERROR;
        }

        return ResponseCodes.OBEX_HTTP_OK;
    }

    private int sendFile(Operation op, File file) {
        FileInputStream istream = null;
        OutputStream ostream = null;

        if (D) Log.d(TAG, "Sending file " + file.getAbsolutePath());

        try {
            istream = new FileInputStream(file);
            ostream = op.openOutputStream();
            int packetSize = op.getMaxPacketSize();
            byte[] buffer = new byte[packetSize];

            long length = file.length();
            long position = 0;

            long time = System.currentTimeMillis();

            while (position < length) {
                int read = istream.read(buffer, 0, packetSize);

                if (read < 0) {
                    break;
                }

                ostream.write(buffer, 0, read);

                position += read;

                if (V) Log.v(TAG, "Sent " + position + " out of " + length + " bytes");
            }

            time = System.currentTimeMillis() - time;
            if (D) Log.d(TAG, String.format("Sent %d bytes in %.3f seconds (%.2fKBps)",
                                length, ((float)time / 1000.0), ((float)length / (float)time)));
        } catch (IOException e) {
            Log.e(TAG, "Exception when sending file: " + e);
            return ResponseCodes.OBEX_HTTP_INTERNAL_ERROR;
        } finally {
            try {
                if (istream != null) {
                    istream.close();
                }
            } catch (IOException e) { }
        }

        return ResponseCodes.OBEX_HTTP_OK;
    }

    @Override
    public int onPut(Operation op) {
        if (V) Log.v(TAG, "onPut called");

        try {
            HeaderSet request = op.getReceivedHeader();
            String name = (String)request.getHeader(HeaderSet.NAME);

            if (D) Log.d(TAG, "PUT request: name=" + name + ", cwd=" + getCurrentDir());

            if (name == null || name.isEmpty()) {
                Log.i(TAG, "Rejecting file with empty name");
                return ResponseCodes.OBEX_HTTP_BAD_REQUEST;
            } else if (name == "." || name == "..") {
                Log.i(TAG, "Rejecting file with forbidden name");
                return ResponseCodes.OBEX_HTTP_FORBIDDEN;
            } else {
                File file = new File(getCurrentDir(), name);

                int result = receiveFile(op, file);
                if (result != ResponseCodes.OBEX_HTTP_OK) {
                    Log.e(TAG, "Error when receiving file");
                    return result;
                }

                sendScanRequestIntent(file, Constants.SCAN_TYPE_NEW);
            }
        } catch (IOException e) {
            Log.e(TAG, "Exception when handling PUT: " + e);
            return ResponseCodes.OBEX_HTTP_INTERNAL_ERROR;
        }

        return ResponseCodes.OBEX_HTTP_OK;
    }

    private int receiveFile(Operation op, File file) {
        FileOutputStream ostream = null;
        InputStream istream = null;
        long length = op.getLength();
        long position = 0;

        if (D) Log.d(TAG, "Receiving file " + file.getAbsolutePath());

        try {
            ostream = new FileOutputStream(file);
            istream = op.openInputStream();
            int packetSize = op.getMaxPacketSize();
            byte[] buffer = new byte[packetSize];

            long time = System.currentTimeMillis();

            while (position < length) {
                int read = istream.read(buffer, 0, packetSize);

                if (read < 0) {
                    break;
                }

                ostream.write(buffer, 0, read);

                position += read;

                if (V) Log.v(TAG, "Received " + position + " out of " + length + " bytes");
            }

            time = System.currentTimeMillis() - time;
            if (D) Log.d(TAG, String.format("Received %d bytes in %.3f seconds (%.2fKBps)",
                                length, ((float)time / 1000.0), ((float)length / (float)time)));
        } catch (IOException e) {
            Log.e(TAG, "Exception when receiving file: " + e);
            return ResponseCodes.OBEX_HTTP_INTERNAL_ERROR;
        } finally {
            try {
                if (ostream != null) {
                    ostream.close();
                }
            } catch (IOException e) { }

            // remove incomplete file
            if (length != position) {
                file.delete();
            }
        }

        return ResponseCodes.OBEX_HTTP_OK;
    }

    @Override
    public int onSetPath(HeaderSet request, HeaderSet reply, boolean backup, boolean create) {
        if (V) Log.v(TAG, "onSetPath called");

        int result = ResponseCodes.OBEX_HTTP_INTERNAL_ERROR;
        int flags = (backup ? 0x01 : 0x00) | (create ? 0x00 : 0x02);

        try {
            String name = (String) request.getHeader(HeaderSet.NAME);

            if (D) Log.d(TAG, "SETPATH request: name=" + name + ", flags=" + flags + ", cwd=" + getCurrentDir());

            switch (flags) {
            case 0x00: // create new folder
                if (name == null || name.length() == 0) {
                    Log.i(TAG, "Folder not created (empty name forbidden)");
                    result = ResponseCodes.OBEX_HTTP_FORBIDDEN;
                    break;
                }

                if (name.equals(".") || name.equals("..")) {
                    Log.i(TAG, "Folder not created (name forbidden)");
                    result = ResponseCodes.OBEX_HTTP_FORBIDDEN;
                    break;
                }

                File newDir = new File(getCurrentDir(), name);
                if (newDir.exists() || newDir.mkdir()) {
                    mCurrentDir.push(name);
                    result = ResponseCodes.OBEX_HTTP_OK;
                } else {
                    Log.i(TAG, "Cannot create folder " + newDir.getAbsolutePath());
                    result = ResponseCodes.OBEX_HTTP_UNAUTHORIZED;
                }
                break;

            case 0x02: // set current folder (root or forward)
                if (name == null) {
                    mCurrentDir.clear();
                    if (V) Log.v(TAG, "Set current folder to root");
                    result = ResponseCodes.OBEX_HTTP_OK;
                } else {
                    File path = new File(getCurrentDir(), name);
                    if (path.exists()) {
                        mCurrentDir.push(name);
                        if (V) Log.v(TAG, "Set current folder");
                        result = ResponseCodes.OBEX_HTTP_OK;
                    } else {
                        Log.i(TAG, "Path does not exist: " + path.getAbsolutePath());
                        result = ResponseCodes.OBEX_HTTP_NOT_FOUND;
                    }
                }
                break;

            case 0x03: // set current folder (backward)
                if (mCurrentDir.isEmpty()) {
                    Log.i(TAG, "Folder not set, already in root");
                    result = ResponseCodes.OBEX_HTTP_NOT_FOUND;
                } else {
                    mCurrentDir.pop();
                    if (V) Log.v(TAG, "Set current folder back to parent");
                    result = ResponseCodes.OBEX_HTTP_OK;
                }
                break;

            default:
                Log.e(TAG, "Unsupported flags combination");
            }
        } catch (IOException e) {
            Log.e(TAG, "Exception when handling SETPATH: " + e);
        }

        return result;
    }

    @Override
    public int onDelete(HeaderSet request, HeaderSet reply) {
        if (V) Log.v(TAG, "onDelete called");

        int result = ResponseCodes.OBEX_HTTP_INTERNAL_ERROR;

        try {
            String name = (String)request.getHeader(HeaderSet.NAME);

            if (D) Log.d(TAG, "PUT request (delete): name=" + name + ", cwd=" + getCurrentDir());

            result = deleteFile(new File(getCurrentDir(), name));
        } catch (IOException e) {
            Log.e(TAG, "Exception when handling PUT (delete): " + e);
        }

        return result;
    }

    private void setupRootDir() {
        try {
            BufferedReader reader = new BufferedReader(new FileReader(Constants.ROOT_CONTAINER_FILE));
            mRootDir = reader.readLine();
        } catch (IOException e) {
            // Nothing to do here, just ignore and mRootDir will be set to external storage
        }
        if (mRootDir == null) {
            mRootDir = Environment.getExternalStorageDirectory().getPath();
        }
        mRootDir = mRootDir.trim();
        if (!mRootDir.endsWith("/")) {
            mRootDir += "/";
        }
    }

    private String getCurrentDir() {
        StringBuilder s = new StringBuilder(mRootDir);

        for (String dir : mCurrentDir) {
            s.append(dir);
            s.append(Constants.PATH_SEPARATOR);
        }

        return s.toString();
    }

    private int deleteFile(File path) {
        if (D) Log.d(TAG, "Deleting file " + path.getAbsolutePath());

        if (!path.exists()) {
            return ResponseCodes.OBEX_HTTP_NOT_FOUND;
        }

        boolean isDirectory = path.isDirectory();

        // for directories delete its contents first
        if (isDirectory) {
            for (File f : path.listFiles()) {
                int result = deleteFile(f);
                if (result != ResponseCodes.OBEX_HTTP_OK) {
                    return result;
                }
            }
        }

        if (!path.delete()) {
            Log.i(TAG, "Failed to delete " + path.getAbsolutePath());
            return ResponseCodes.OBEX_HTTP_UNAUTHORIZED;
        }

        if (!isDirectory) {
            sendScanRequestIntent(path, Constants.SCAN_TYPE_DELETED);
        }

        return ResponseCodes.OBEX_HTTP_OK;
    }

    private void sendScanRequestIntent(File file, int scanType) {
        String scanPath = file.getAbsolutePath();
        String mimeType = getMimeType(scanPath);

        Intent intent = new Intent(Constants.ACTION_SCAN_REQUEST);
        intent.putExtra(Constants.EXTRA_SCANTYPE, scanType);
        intent.putExtra(Constants.EXTRA_SCANPATH, scanPath);
        intent.putExtra(Constants.EXTRA_MIMETYPE, mimeType);
        mContext.sendBroadcast(intent);

        if (V) Log.v(TAG, "Scan intent sent for " + scanPath + " with MIME type " + mimeType);
    }

    private static String getMimeType(String path) {
        String type = null;

        if (path != null) {
            File file = new File(path);
            String name = file.getName();

            int idx = name.lastIndexOf(".");
            if (idx != -1) {
                String ext = name.substring(idx + 1).toLowerCase();
                type = MimeTypeMap.getSingleton().getMimeTypeFromExtension(ext);
            }
        }

        if (V) Log.v(TAG, "MIME type for " + path + " guessed form extension: " + type);

        return type;
    }
}
