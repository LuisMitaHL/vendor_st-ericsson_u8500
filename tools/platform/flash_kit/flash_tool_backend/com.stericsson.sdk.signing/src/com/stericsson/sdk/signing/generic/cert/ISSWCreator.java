package com.stericsson.sdk.signing.generic.cert;
//package com.stericsson.sdk.signing.internal.u5500.cert;
//
//import java.io.File;
//import java.io.FileInputStream;
//import java.io.FileNotFoundException;
//import java.io.FileOutputStream;
//import java.io.IOException;
//
//import com.stericsson.sdk.signing.internal.u5500.HashType;
//import com.stericsson.sdk.signing.internal.u5500.SignatureType;
//import com.stericsson.sdk.signing.internal.u5500.cert.subparts.U5500ISSWCustRootKeyCert;
//import com.stericsson.sdk.signing.internal.u5500.cert.subparts.U5500ISSWCustomerPart;
//import com.stericsson.sdk.signing.internal.u5500.cert.subparts.U5500ISSWCustomerPartKey;
//import com.stericsson.sdk.signing.internal.u5500.cert.subparts.U5500ISSWRootKey;
//import com.stericsson.sdk.signing.internal.u5500.cert.subparts.U5500ISSWSecurityRomHeader;
//
//public class ISSWCreator {
//
//    public void create() throws IOException {
//        U5500ISSWCustomerFormatCertificate issw = new U5500ISSWCustomerFormatCertificate();
//        U5500ISSWSecurityRomHeader secRomHeader = issw.getSerRomHeader();
//        secRomHeader.setMagic(U5500Certificate.MAGIC_ISSW_CUSTOMER_REQUESTED);
//        secRomHeader.setRootKeySize(10); /////////////////////////////////////////////////////
//        secRomHeader.setCustomerPartSize(80);
//        secRomHeader.setUncheckedBlobSize(0);
//        secRomHeader.setSignatureSize(0);
//        secRomHeader.setSpeedUpMagic(U5500Certificate.MAGIC_ISSW_SPEEDUP);
//        secRomHeader.setSpeedUpMask(1);
//        secRomHeader.setSpeedUpData(new byte[256]);
//        secRomHeader.setSpeedUpPool(new byte[]{1,2,3,4});
//        secRomHeader.setSpeedUpSpare(new byte[]{5,6,7,8});
//        secRomHeader.setISSWHashType(HashType.SHA256_HASH);
//        secRomHeader.setHash(new byte[64]);
//        secRomHeader.setCodeLength(0);
//        secRomHeader.setRWDataLength(0);
//        secRomHeader.setZIDataLength(0);
//        secRomHeader.setISSWLoadLocation(8);
//        secRomHeader.setISSWStartAddr(9);
//        secRomHeader.setRootKeyType(SignatureType.RSASSA_PKCS_V1_5);
//        secRomHeader.setRootKeyHashType(HashType.SHA256_HASH);
//        secRomHeader.setSignatureHashType(HashType.SHA256_HASH);
//
//        U5500ISSWRootKey rootKey = issw.getRootKey();
//        rootKey.setPublicExponent(1);
//        short dummy = 2;
//        rootKey.setDummy(dummy);
//        short sizeModulus = 2;
//        rootKey.setModulusSize(sizeModulus);
//        rootKey.setModulus(new byte[]{1,1});
//
//        U5500ISSWCustomerPart customerPart = issw.getCustomerPart();
//        customerPart.setMagic(U5500Certificate.MAGIC_CUSTOMER_PART);
//        customerPart.setKeyListSize(44); /////////////////////////////////////////
//        customerPart.setNumberOfKeysInList(2);
//        short minorVersion = 3;
//        customerPart.setMinorBuildVersion(minorVersion);
//        short majorVersion = 3;
//        customerPart.setMajorBuildVersion(majorVersion);
//        customerPart.setUTCTime(2);
//        customerPart.setFlags(2);
//        customerPart.setSigningConstraints(new byte[]{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16});
//        customerPart.setSpare(0);
//
//        U5500ISSWCustomerPartKey key = new U5500ISSWCustomerPartKey();
//        key.setKeySize(18); ////////////////////////////////////////////////////
//        key.setKeyType(HashType.SHA256_HASH);
//        key.setExponent(1);
//        dummy = 2;
//        key.setDummy(dummy);
//        sizeModulus = 2;
//        key.setModulusSize(sizeModulus);
//        key.setModulus(new byte[]{1,1});
//
//        customerPart.addKey(key);
//
//        key = new U5500ISSWCustomerPartKey();
//        key.setKeySize(18); /////////////////////////////////////////////////////
//        key.setKeyType(HashType.SHA256_HASH);
//        key.setExponent(1);
//        dummy = 2;
//        key.setDummy(dummy);
//        sizeModulus = 2;
//        key.setModulusSize(sizeModulus);
//        key.setModulus(new byte[]{1,1});
//
//        customerPart.addKey(key);
//
//        byte[] isswData = issw.getData();
//
//        String strFilePath = "C:\\issw";
//
//        FileOutputStream fos = new FileOutputStream(strFilePath);
//        fos.write(isswData);
//        fos.close(); 
//
//    }
//
//    public void readAndWrite() throws Exception {
//        String strFilePathIn = "C:\\issw";
//        String strFilePathOut = "C:\\isswOut";
//
//        File inputFile = new File(strFilePathIn);
//        byte[] data = new byte[(int) inputFile.length()]; 
//        FileInputStream fin = new FileInputStream(strFilePathIn);
//        fin.read(data);
//
//        U5500ISSWCustomerFormatCertificate issw = new U5500ISSWCustomerFormatCertificate();
//        issw.setData(data);
//        FileOutputStream fos = new FileOutputStream(strFilePathOut);
//        byte[] isswData = issw.getData();
//        fos.write(isswData);
//        fos.close(); 
//    }
//    public static void main(String[] args) throws Exception {
////        new ISSWCreator().create();
//        new ISSWCreator().readAndWrite();
//        System.out.println("done");
//    }
//}
