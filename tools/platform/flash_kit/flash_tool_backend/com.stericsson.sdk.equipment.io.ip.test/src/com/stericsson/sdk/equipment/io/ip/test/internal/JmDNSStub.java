package com.stericsson.sdk.equipment.io.ip.test.internal;

import java.io.IOException;
import java.net.Inet4Address;
import java.net.Inet6Address;
import java.net.InetAddress;
import java.util.Enumeration;
import java.util.Map;

import javax.jmdns.JmDNS;
import javax.jmdns.ServiceEvent;
import javax.jmdns.ServiceInfo;
import javax.jmdns.ServiceListener;
import javax.jmdns.ServiceTypeListener;

import com.stericsson.sdk.equipment.io.ip.configuration.DNSConfiguration;

/**
 * @author eolabor
 *
 */
public class JmDNSStub {

    private boolean listenerAdded = false;

    private String serviceName;

    private String hostadress;

    private ServiceEvent event = new ServiceEvent(this) {

        /**
         *
         */
        private static final long serialVersionUID = 1L;

        @Override
        public String getType() {
            return DNSConfiguration.SERVICE_TYPE;
        }

        @Override
        public String getName() {
            return serviceName;
        }

        @Override
        public ServiceInfo getInfo() {
            return serviceInfo;
        }

        @Override
        public JmDNS getDNS() {
            return jmDNS;
        }
    };

    private JmDNS jmDNS = new JmDNS() {

        @Override
        public void unregisterService(ServiceInfo arg0) {
            // This is just a stub.

        }

        @Override
        public void unregisterAllServices() {
            // This is just a stub.

        }

        @Override
        public void requestServiceInfo(String arg0, String arg1, boolean arg2, long arg3) {
            // This is just a stub.

        }

        @Override
        public void requestServiceInfo(String arg0, String arg1, long arg2) {
            // This is just a stub.

        }

        @Override
        public void requestServiceInfo(String arg0, String arg1, boolean arg2) {
            // This is just a stub.

        }

        @Override
        public void requestServiceInfo(String arg0, String arg1) {
            // This is just a stub.

        }

        @Override
        public void removeServiceTypeListener(ServiceTypeListener arg0) {
            // This is just a stub.

        }

        @Override
        public void removeServiceListener(String arg0, ServiceListener arg1) {
            // This is just a stub.

        }

        @Override
        public boolean registerServiceType(String arg0) {
            // This is just a stub.
            return false;
        }

        @Override
        public void registerService(ServiceInfo arg0) throws IOException {
            // This is just a stub.

        }

        @Override
        public void printServices() {
            // This is just a stub.

        }

        @Override
        public Map<String, ServiceInfo[]> listBySubtype(String arg0, long arg1) {
            // This is just a stub.
            return null;
        }

        @Override
        public Map<String, ServiceInfo[]> listBySubtype(String arg0) {
            // This is just a stub.
            return null;
        }

        @Override
        public ServiceInfo[] list(String arg0, long arg1) {
            // This is just a stub.
            return null;
        }

        @Override
        public ServiceInfo[] list(String arg0) {
            ServiceInfo[] infos = {
                serviceInfo};
            return infos;
        }

        @Override
        public ServiceInfo getServiceInfo(String arg0, String arg1, boolean arg2, long arg3) {
            // This is just a stub.
            return null;
        }

        @Override
        public ServiceInfo getServiceInfo(String arg0, String arg1, boolean arg2) {
            // This is just a stub.
            return null;
        }

        @Override
        public ServiceInfo getServiceInfo(String arg0, String arg1, long arg2) {
            // This is just a stub.
            return null;
        }

        @Override
        public ServiceInfo getServiceInfo(String arg0, String arg1) {
            // This is just a stub.
            return null;
        }

        @Override
        public String getName() {
            // This is just a stub.
            return null;
        }

        @Override
        public InetAddress getInterface() throws IOException {
            // This is just a stub.
            return null;
        }

        @Override
        public String getHostName() {
            // This is just a stub.
            return null;
        }

        @Override
        public void addServiceTypeListener(ServiceTypeListener arg0) throws IOException {
            // This is just a stub.

        }

        @Override
        public void addServiceListener(String arg0, ServiceListener arg1) {
            System.out.println("Listener Added");
            listenerAdded = true;
        }

        public void close() {
            // This is just a stub.
        }
    };

    private ServiceInfo serviceInfo = new ServiceInfo() {

        @Override
        public void setText(Map<String, ?> arg0) throws IllegalStateException {
            // This is just a stub.

        }

        @Override
        public void setText(byte[] arg0) throws IllegalStateException {
            // This is just a stub.

        }

        @Override
        public boolean isPersistent() {
            // This is just a stub.
            return false;
        }

        @Override
        public boolean hasData() {
            // This is just a stub.
            return false;
        }

        @Override
        public int getWeight() {
            // This is just a stub.
            return 0;
        }

        @Override
        public String getURL(String arg0) {
            // This is just a stub.
            return null;
        }

        @Override
        public String getURL() {
            // This is just a stub.
            return null;
        }

        @Override
        public String getTypeWithSubtype() {
            // This is just a stub.
            return null;
        }

        @Override
        public String getType() {
            // This is just a stub.
            return null;
        }

        @Override
        public String getTextString() {
            // This is just a stub.
            return null;
        }

        @Override
        public byte[] getTextBytes() {
            // This is just a stub.
            return null;
        }

        @Override
        public String getSubtype() {
            // This is just a stub.
            return null;
        }

        @Override
        public String getServer() {
            // This is just a stub.
            return null;
        }

        @Override
        public Map<Fields, String> getQualifiedNameMap() {
            // This is just a stub.
            return null;
        }

        @Override
        public String getQualifiedName() {
            // This is just a stub.
            return null;
        }

        @Override
        public String getProtocol() {
            // This is just a stub.
            return null;
        }

        @Override
        public String getPropertyString(String arg0) {
            // This is just a stub.
            return null;
        }

        @Override
        public Enumeration<String> getPropertyNames() {
            // This is just a stub.
            return null;
        }

        @Override
        public byte[] getPropertyBytes(String arg0) {
            // This is just a stub.
            return null;
        }

        @Override
        public int getPriority() {
            // This is just a stub.
            return 0;
        }

        @Override
        public int getPort() {
            // This is just a stub.
            return 0;
        }

        @Override
        public String getNiceTextString() {
            String dummyPath = "path=/dummy/path";
            return dummyPath;
        }

        @Override
        public String getName() {
            return serviceName;
        }

        @Override
        public InetAddress getInetAddress() {
            // This is just a stub.
            return null;
        }

        @Override
        public Inet6Address getInet6Address() {
            // This is just a stub.
            return null;
        }

        @Override
        public Inet4Address getInet4Address() {
            // This is just a stub.
            return null;
        }

        @Override
        public String getHostAddress() {
            return hostadress;
        }

        @Override
        public String getDomain() {
            // This is just a stub.
            return null;
        }

        @Override
        public String getApplication() {
            // This is just a stub.
            return null;
        }

        @Override
        public InetAddress getAddress() {
            // This is just a stub.
            return null;
        }
    };

    /**
     * @return the ServiceEvent
     */
    public ServiceEvent getEvent() {
        return event;
    }

    /**
     * @return the JmDNS
     */
    public JmDNS getJmDNS() {
        return jmDNS;
    }

    /**
     * @return boolean Listener added
     */
    public boolean getListnerAdded() {
        return listenerAdded;
    }

    /**
     * @return the serviceName
     */
    public String getServiceName() {
        return serviceName;
    }

    /**
     * @param pServiceName
     *            the serviceName to set
     */
    public void setServiceName(String pServiceName) {
        this.serviceName = pServiceName;
    }

    /**
     * @param pHostadress
     *            the host address to set
     */
    public void setHostadress(String pHostadress) {
        this.hostadress = pHostadress;
    }
}
