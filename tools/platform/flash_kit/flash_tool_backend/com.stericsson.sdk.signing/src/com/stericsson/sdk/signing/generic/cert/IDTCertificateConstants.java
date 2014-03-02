package com.stericsson.sdk.signing.generic.cert;

/**
 * @author TSIKOR01
 * 
 */
public interface IDTCertificateConstants {

    /** * */
    int SIZE_MAGIC = 4;

    /** * */
    int SIZE_DT_CERT_SIZE = 4;

    /** * */
    int SIZE_SIGNATURE_SIZE = 4;

    /** * */
    int SIZE_SIGNATURE_HASH_TYPE = 4;

    /** * */
    int SIZE_SERIAL_NUMBER = 4;

    /** * */
    int SIZE_TYPE = 4;

    /** * */
    int SIZE_LEVEL = 1;

    /** * */
    int SIZE_SECURE_DEBUG = 1;

    /** * */
    int SIZE_APE_DEBUG = 1;

    /** * */
    int SIZE_MODEM_DEBUG = 1;

    /** * */
    int SIZE_PRCMU_DEBUG = 1;

    /** * */
    int SIZE_STM = 1;

    /** * */
    int SIZE_SPARE = 2;

    /** * */
    int SIZE_KEYS_TO_REPLACE = 4;

    /** * */
    int FIELD_MAGIC = 1;

    /** * */
    int FIELD_DT_CERT_SIZE = 2;

    /** * */
    int FIELD_SIGNATURE_SIZE = 3;

    /** * */
    int FIELD_SIGNATURE_HASH_TYPE = 4;

    /** * */
    int FIELD_SERIAL_NUMBER = 5;

    /** * */
    int FIELD_TYPE = 6;

    /** * */
    int FIELD_LEVEL = 7;

    /** * */
    int FIELD_SECURE_DEBUG = 8;

    /** * */
    int FIELD_APE_DEBUG = 9;

    /** * */
    int FIELD_MODEM_DEBUG = 10;

    /** * */
    int FIELD_PRCMU_DEBUG = 11;

    /** * */
    int FIELD_STM = 12;

    /** * */
    int FIELD_SPARE = 13;

    /** * */
    int FIELD_FLAGS = 14;

    /** * */
    int FIELD_RESERVED = 15;

    /** * */
    int FIELD_KEYS_TO_REPLACE = 16;

    /** * */
    int SIZE_FLAGS = 4;

    /** * */
    int SIZE_RESERVED = 32;
}
