package com.stericsson.sdk.equipment.io.usb.internal;

class USBDeviceKey {

    private String busDirname;

    private String deviceFilename;

    private int devnum;

    private int vendorId;

    private int productId;

    public USBDeviceKey(int dnum, String dirname, String filename, int vid, int pid) {
        this.devnum = dnum;
        this.busDirname = dirname;
        this.deviceFilename = filename;
        this.vendorId = vid;
        this.productId = pid;
    }

    public int getDevnum() {
        return devnum;
    }

    public String getBusDirname() {
        return busDirname;
    }

    public String getDeviceFilename() {
        return deviceFilename;
    }

    public int getVendorId() {
        return vendorId;
    }

    public int getProductId() {
        return productId;
    }

    @Override
    public int hashCode() {
        return devnum << 24 ^ vendorId << 16 ^ productId << 8 ^ busDirname.hashCode();
    }

    @Override
    public boolean equals(Object o) {
        if ((o == null) || !(o instanceof USBDeviceKey)) {
            return false;
        } else {
            USBDeviceKey key = (USBDeviceKey) o;
            return devnum == key.devnum && vendorId == key.vendorId && productId == key.productId
                && busDirname.equals(key.busDirname);
        }
    }

}
