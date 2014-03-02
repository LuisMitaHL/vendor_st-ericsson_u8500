package com.stericsson.sdk.signing.util;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.security.Key;
import java.security.KeyPair;
import java.security.PrivateKey;
import java.security.PublicKey;
import java.security.Security;
import java.security.interfaces.RSAPrivateKey;
import java.security.interfaces.RSAPublicKey;

import org.bouncycastle.jce.provider.BouncyCastleProvider;
import org.bouncycastle.openssl.PEMReader;
import org.bouncycastle.openssl.PEMWriter;

import com.stericsson.sdk.common.LittleEndianByteConverter;
import com.stericsson.sdk.signing.generic.cert.subparts.GenericKey;

/**
 * 
 * @author xolabju
 * 
 */
public final class PEMParser {

    private PEMParser() {
    }

    /**
     * Parses the RSAPublicKey from a PEM formatted key file
     * 
     * @param pemFile
     *            path to the PEM formatted RSA key
     * @return the parsed public key
     * @throws IOException
     *             on errors
     */
    public static RSAPublicKey parseRSAPublicKey(String pemFile) throws IOException {
        if (Security.getProvider("BC") == null) {
            Security.addProvider(new BouncyCastleProvider());
        }
        FileInputStream fis = null;
        BufferedReader br = null;
        RSAPublicKey rsaKey = null;
        PEMReader reader = null;

        try {
            fis = new FileInputStream(pemFile);
            br = new BufferedReader(new InputStreamReader(fis, "UTF-8"));
            reader = new PEMReader(br);
            Object obj = reader.readObject();

            if (obj == null) {
                throw new IOException("Failed to parse PEM file");
            }

            if (obj instanceof KeyPair) {
                KeyPair keyPair = (KeyPair) obj;
                PublicKey publicKey = keyPair.getPublic();
                if (publicKey instanceof RSAPublicKey) {
                    rsaKey = (RSAPublicKey) publicKey;
                }
            } else if (obj instanceof RSAPublicKey) {
                rsaKey = (RSAPublicKey) obj;
            } else {
                throw new IOException("Failed to parse PEM file");
            }
        } finally {
            if (reader != null) {
                try {
                    reader.close();
                } catch (IOException ioe) {
                    ioe.printStackTrace();
                }
            }
            if (br != null) {
                try {
                    br.close();
                } catch (IOException ioe) {
                    ioe.printStackTrace();
                }
            }
            if (fis != null) {
                try {
                    fis.close();
                } catch (IOException ioe) {
                    ioe.printStackTrace();
                }
            }
        }
        return rsaKey;
    }

    /**
     * Parses a PEM private key in text format into an RSAPrivateKey.
     * 
     * Example:
     * 
     * -----BEGIN RSA PRIVATE KEY-----
     * MIICWwIBAAKBgQC82bW4kxKIQhjuPHAth3x1vkhAvbR3Nh1rNbDl5m6jQdOzPSt7
     * QkcooSZCLCoEx3d8diFPXqjEFAdlLKnKnk60UBjleS5RUr87qPEW7yRpdtKN81u/
     * eTyzkUGkzcCxb/Ypvy57yOwdi+l0GW5YWxnltotkU5OXyso4TCUmGGCdDQIBAwKB
     * gH3meSW3YbAsEJ7S9XOvqE5+2tXTzaTOvkd5IJlEScIr4nd+HPzW2hsWGYFyxq3a
     * T6hOwN+UcIK4BO4dxocUNHcQAu5kw/xcHRNHE0m3zWOYwO87YOBjyCa0ZsO8I9L1
     * 8vIXGXXWwVi2fkVAND8YCfJHsEkqxT2i1fLjU5e39WHrAkEA5IUaEpTJOYvr1dmv
     * 0k7jLII5KVTXw/99Kt2ueuXmOTcqevkBxxpk+coWOLSYNyi7K1/uJ7fHn+Fr3Yz7
     * uk8T2QJBANOPZc9zjY8Hsuh6eIkhcOUu7fD1lx+Q/FfJ0LilDsXSlCGPST+vs4Bh
     * 9gBrYX/iPx+jCGuz87an4BmaxsohdlUCQQCYWLwMYzDRB/KOkR/hiezIVtDGOI/X
     * /6jHPnRR7pl7ehxR+1aEvENRMWQlzbrPcHzHlUlvz9pqlkfpCKfRig07AkEAjQpD
     * 36JeX1p3Rab7BhZLQ3SeoKO6FQtS5TE10G4J2TcNa7Tbf8p3quv5VZzrqpbUv8IF
     * nSKiecVAERHZ3BZO4wJAN4Zpzou5JAp8LSJ/6ZmKyAoojQ5o70Lx6pyq3XNQFQjJ
     * pHG7QmD4XYh0r0WXgPHj9iR8MtAA9gkr4TF8D7DHHA== -----END RSA PRIVATE KEY-----
     * 
     * @param filename
     *            The path to the private key text file
     * @return The RSAPrivateKey that is parsed from the input file.
     * @throws IOException
     *             If there is a problem parsing the key.
     */
    public static RSAPrivateKey parsePrivateKey(String filename) throws IOException {
        if (Security.getProvider("BC") == null) {
            Security.addProvider(new BouncyCastleProvider());
        }
        FileInputStream fis = null;
        BufferedReader br = null;
        PEMReader reader = null;
        try {
            fis = new FileInputStream(filename);
            br = new BufferedReader(new InputStreamReader(fis, "UTF-8"));
            reader = new PEMReader(br);
            Object obj = reader.readObject();
            if (obj == null || !(obj instanceof KeyPair)) {
                throw new IOException("Failed to parse private key from " + filename + ".");
            }
            KeyPair keyPair = (KeyPair) obj;
            PrivateKey privateKey = keyPair.getPrivate();
            return (RSAPrivateKey) privateKey;
        } finally {
            if (reader != null) {
                try {
                    reader.close();
                } catch (IOException ioe) {
                    ioe.getMessage();
                }
            }
            if (br != null) {
                try {
                    br.close();
                } catch (IOException ioe) {
                    ioe.getMessage();
                }
            }
            if (fis != null) {
                try {
                    fis.close();
                } catch (IOException ioe) {
                    ioe.getMessage();
                }
            }
        }
    }

    /**
     * Parses the U5500 formatted key from a PEM formatted key file
     * 
     * @param pemFile
     *            path to the PEM formatted RSA key
     * @return the parsed public key
     * @throws IOException
     *             on errors
     */
    public static GenericKey parseU5500Key(String pemFile) throws IOException {
        RSAPublicKey rsaKey = PEMParser.parseRSAPublicKey(pemFile);
        if (rsaKey == null) {
            throw new IOException("Failed to parse " + pemFile);
        }
        GenericKey key = new GenericKey();
        byte[] modulusWithSignBit = rsaKey.getModulus().toByteArray();
        byte[] modulus = new byte[modulusWithSignBit.length - 1];
        System.arraycopy(modulusWithSignBit, 1, modulus, 0, modulus.length);
        key.setModulus(LittleEndianByteConverter.reverse(modulus));
        key.setPublicExponent(rsaKey.getPublicExponent().intValue());
        key.setModulusSize((short) modulus.length);

        return key;
    }

    /**
     * @param f
     *            File to which key should be written.
     * @param key
     *            key which is written
     * @throws IOException
     *             if the file exists but is a directory rather than a regular file, does not exist
     *             but cannot be created, or cannot be opened for any other I/O error occurs.
     */
    public static void writeRSAkey(File f, Key key) throws IOException {
        FileOutputStream fos = new FileOutputStream(f);
        OutputStreamWriter fw = new OutputStreamWriter(fos, "UTF-8");
        PEMWriter pw = new PEMWriter(fw);
        pw.writeObject(key);
        pw.close();
        fw.close();
    }
}
