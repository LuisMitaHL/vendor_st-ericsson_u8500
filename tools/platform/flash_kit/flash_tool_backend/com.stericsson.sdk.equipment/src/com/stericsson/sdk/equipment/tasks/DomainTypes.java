package com.stericsson.sdk.equipment.tasks;

/**
 * Enumerator used for contain domain information.
 * 
 * @author mbodan01
 * 
 */
public enum DomainTypes {
    /** Service Domain */
    SERVICE(0, "Service"),
    /** Product Domain */
    PRODUCT(1, "Product"),
    /** Research and Development Domain */
    RANDD(2, "R&D"),
    /** Factory Domain */
    FACTORY(3, "Factory");

    private int domainId;

    private String domainName;

    /**
     * 
     * @param domainId
     *            -
     * @param domainName
     *            -
     */
    private DomainTypes(int pDomainId, String pDomainName) {
        domainId = pDomainId;
        domainName = pDomainName;
    }

    /**
     * Method returns the domain ID.
     * 
     * @return domain id
     */
    public int getDomainId() {
        return domainId;
    }

    /**
     * Method returns the domain name
     * 
     * @return domain name
     */
    public String getDomainName() {
        return domainName;
    }

    static DomainTypes getDomain(int id) {
        for (DomainTypes type : DomainTypes.values()) {
            if (type.getDomainId() == id) {
                return type;
            }
        }
        return null;
    }
}
