package com.stericsson.sdk.signing.generic;

/**
 * @author xtomzap
 * 
 */
public enum GenericSoftwareType {

    /** Authentication certificate (no payload type) */
    AUTH_CERT(10, -1, "Authentication certificate", "AUTH_CERT"),
    /** Initial secure software */
    ISSW(-1, -1, "Initial secure software (ISSW)", "ISSW"),
    /** Trusted application */
    TRUSTED(0, 0, "Trusted application", "TRUSTED"),
    /** PRCMU firmware */
    PRCMU(1, 1, "PRCMU firmware", "PRCMU"),
    /** Mem init */
    MEM_INIT(2, 2, "Mem init", "MEM_INIT"),
    /** X-loader */
    XLOADER(3, 3, "X-loader", "XLOADER"),
    /** OS-loader */
    OSLOADER(4, 4, "OS-loader", "OSLOADER"),
    /** APE normal world code */
    APE_NORMAL(5, 5, "APE normal world code", "APE_NORMAL"),
    /** Flash and customize loader */
    LDR(6, 6, "Flash and customize loader", "LDR"),
    /** Modem code */
    MODEM(7, 7, "Modem code", "MODEM"),
    /** Firmware over the air */
    FOTA(8, 8, "Firmware over the air", "FOTA"),
    /** Debug and test certificate */
    DNT(9, 9, "Debug and test certificate", "DNT"),
    /** Authentication challenge (no signing constraint) */
    AUTH_CHALLENGE(-1, -1, "Authentication challenge", "AUTH_CHALLENGE"),
    /***/
    IPL(11, 11, "GDFS and IPL", "GDFS and IPL"),
    /***/
    GDFS(11, 11, "GDFS and IPL", "GDFS and IPL"),
    /***/
    FLASH_ARCHIVE(12, 12, "Flash archive", "FLASH_ARCHIVE"),
    /** ITP */
    ITP(13, 13, "ITP", "ITP"),
    /** TVP LICENSE */
    TVP_LICENSE(14, 14, "TVP license", "TVP_LICENSE"),
    /** Customer Root Key Certificate */
    CRKC(-1, -1, "Customer root key certificate", "CRKC"),
    /** ELF load module */
    ELF(5, 12, "ELF load module", "ELF"),
    /** Field Return Authentication Certificate */
    FRAC(15, 15, "Field Return Authentication Certificate", "FRAC"),
    /** User space trusted application */
    USTA(16, 16, "User space trusted application", "USTA"),
    /** IMAD */
    IMAD(17, 17, "IMAD", "IMAD");


    private int signingConstraint;

    private int softwareId;

    private String name;

    private String type;

    private GenericSoftwareType(int pSigningConstraint, int pSoftwareId, String pName, String pType) {
        signingConstraint = pSigningConstraint;
        softwareId = pSoftwareId;
        name = pName;
        type = pType;
    }

    /**
     * @return position in sign constraints
     */
    public int getPosition() {
        return signingConstraint;
    }

    /**
     * @return software id
     */
    public int getSoftwareId() {
        return softwareId;
    }

    /**
     * @return GUI visible representation string
     */
    public String getName() {
        return name;
    }

    /**
     * @return Software type
     */
    public String getType() {
        return type;
    }
}
