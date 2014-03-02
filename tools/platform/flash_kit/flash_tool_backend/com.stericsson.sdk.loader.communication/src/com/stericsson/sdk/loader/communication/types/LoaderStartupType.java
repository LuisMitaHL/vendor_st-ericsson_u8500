package com.stericsson.sdk.loader.communication.types;

/**
 * @author xdancho
 * 
 */
public class LoaderStartupType extends LoaderCommunicationType {

    String loaderVersion = "";

    String protocolVersion = "";

    /**
     * @param status
     *            result
     * @param version
     *            loader version
     * @param versionSize
     *            loader version string size
     * @param protocol
     *            protocol version
     * @param protocolSize
     *            protocol version string size
     */
    public LoaderStartupType(int status, String version, int versionSize,
            String protocol, int protocolSize) {
        super(status);

        if (LoaderCommunicationType.checkString(version, versionSize)) {
            this.loaderVersion = version.substring(0, versionSize);
        }

        if (LoaderCommunicationType.checkString(protocol, protocolSize)) {
            this.protocolVersion = protocol.substring(0, protocolSize);
        }

    }

    /**
     * @return the loaderVersion
     */
    public String getLoaderVersion() {
        return loaderVersion;
    }

    /**
     * @return the protocolVersion
     */
    public String getProtocolVersion() {
        return protocolVersion;
    }

}
