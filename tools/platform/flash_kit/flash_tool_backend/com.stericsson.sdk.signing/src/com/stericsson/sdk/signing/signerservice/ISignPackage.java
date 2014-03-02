package com.stericsson.sdk.signing.signerservice;

/**
 * Empty interface for sign package objects.
 * 
 * @author xtomlju
 */
public interface ISignPackage {

    /**
     * 
     */
    String MESP_NAME = "EMPHeaderPackage";

    /**
     * 
     */
    String PARAM_NAME_CHIPIDS = "CHIPIDS[]";

    /**
     * 
     */
    String PARAM_NAME_ENCRYPT_REQUIRED = "ENCRYPT_REQUIRED";

    /**
     * Flag for access rights.
     */
    String PARAM_NAME_CHIPALIASES = "CHIPALIASES";

    /**
     * 
     */
    String PARAM_NAME_PATH = "path";

    /**
     * 
     */
    String HID_FILENAME_NAME = "HID_filename";

    /**
     * 
     */
    String CHIP_CLASS_FILENAME_NAME = "CHIP_CLASS_filename";

    /**
     * 
     */
    String PACKAGE_TYPE_COPS_A2 = "COPS_A2";

    /**
     * 
     */
    String PACKAGE_TYPE_COPS_A2_AUTH_CA = "COPS_A2_AUTH_CA";

    /**
     * 
     */
    String PACKAGE_TYPE_COPS_A2_AUTH = "COPS_A2_AUTH";

    /**
     * Default name of index file.
     */
    String INDEX_MESP_FILENAME = "index.mesp";

    /** */
    String PACKAGE_MESP_NAME = "Package";

    /**
     * Default name of header mesp.
     */
    String HEADER_MESP_NAME = "EMPHeader";

    /**
     * Default name of cert mesp.
     */
    String CERTIFICATE_MESP_NAME = "Certificate";

    /**
     * Package name value id.
     */
    String PACKAGE_ALIAS_NAME = "alias";

    /**
     * Package parent value id.
     */
    String PACKAGE_PARENT_NAME = "parent";

    /**
     * Package type value id.
     */
    String PACKAGE_TYPE_NAME = "type";

    /**
     * Package create date value id.
     */
    String PACKAGE_CREATED_NAME = "created";

    /**
     * Package author value id.
     */
    String PACKAGE_CREATED_BY_NAME = "createdBy";

    /**
     * Package revision value id.
     */
    String PACKAGE_REVISION_NAME = "revision";

    /**
     * Package descr value id.
     */
    String PACKAGE_DESCRIPTION_NAME = "description";

    /**
     * Package version value id.
     */
    String HEADER_VERSION_NAME = "version";

    /**
     * Package header value id.
     */
    String HEADER_NAME_NAME = "name";

    /**
     * Package cert name value id.
     */
    String CERTIFICATE_NAME_NAME = "name";

    /**
     * Default name of cert file.
     */
    String CERTIFICATE_FILE_NAME = "Certificate.bin";

    /** */
    String PACKAGE_TYPE_U8500_ROOT = "U8500_ROOT";

    /** */
    String PACKAGE_TYPE_U8500_OEM1 = "U8500_OEM1";

    /** */
    String PACKAGE_TYPE_U8500_OEM2 = "U8500_OEM2";

    /** */
    String PACKAGE_TYPE_U8500_SMK = "U8500_SMK";

    /** */
    String PACKAGE_TYPE_U5500_ROOT = "U5500_ROOT";

    /** */
    String PACKAGE_TYPE_U5500_GENERIC = "U5500_GENERIC";

    /** */
    String PACKAGE_TYPE_U5500_AUTH = "U5500_AUTH";

    /** */
    String PACKAGE_TYPE_X509 = "X.509";

    /** */
    String PACKAGE_TYPE_U5500_CRK = "U5500_CRK";

    /** */
    String PACKAGE_TYPE_EMPTY_GENERIC = "EMPTY_GENERIC";

    /**
     * 
     * @return alias
     */
    String getAlias();

    /**
     * 
     * @return type
     */
    String getType();

    /**
     * 
     * @return parent
     */
    String getParent();

    /**
     * 
     * @return description
     */
    String getDescription();

    /**
     * 
     * @return revision
     */
    String getRevision();

}
