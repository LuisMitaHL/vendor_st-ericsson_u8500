/*
 * javadefs.h
 *
 *  Created on: Jul 23, 2009
 *      Author: emicroh
 */

#ifndef JAVADEFS_H_
#define JAVADEFS_H_

#define UNDEF -1

#define EX_ARRAY_INDEX_OUT_OF_BOUND "java/lang/ArrayIndexOutOfBoundsException"
#define EX_NATIVE_EXCEPTION "com/stericsson/sdk/equipment/io/port/NativeException"
#define EX_IO_TIMEOUT_EXCEPTION "com/stericsson/sdk/equipment/io/usb/internal/IOTimeoutException"

#define CLASS_USB_NATIVE_HANDLER "com/stericsson/sdk/equipment/io/usb/internal/USBNativeHandler"
#define CLASS_USB_NATIVE_DEVICE "com/stericsson/sdk/equipment/io/usb/internal/USBNativeDevice"
#define CLASS_USB_DEVICE_KEY "com/stericsson/sdk/equipment/io/usb/internal/USBDeviceKey"

#define CLASS_DEVICE_DESCRIPTOR "com/stericsson/sdk/equipment/io/usb/internal/USBNativeDevice$DeviceDescriptor"
#define CLASS_USB_DESCRIPTOR "com/stericsson/sdk/equipment/io/usb/internal/USBNativeDevice$USBDescriptor"
#define CLASS_USB_CONF_DESCRIPTOR "com/stericsson/sdk/equipment/io/usb/internal/USBNativeDevice$USBConfigDescriptor"
#define CLASS_USB_INT_DESCRIPTOR "com/stericsson/sdk/equipment/io/usb/internal/USBNativeDevice$USBInterfaceDescriptor"
#define CLASS_USB_EP_DESCRIPTOR "com/stericsson/sdk/equipment/io/usb/internal/USBNativeDevice$USBEndpointDescriptor"

#define METHOD_NH_GET_INSTANCE "getInstance"
#define METHOD_NH_GET_INSTANCE_SIG "()Lcom/stericsson/sdk/equipment/io/usb/internal/USBNativeHandler;"
#define METHOD_NH_REGISTER_USB "registerUSB"
#define METHOD_NH_REGISTER_USB_SIG "(Lcom/stericsson/sdk/equipment/io/usb/internal/USBNativeDevice;)V"
#define METHOD_NH_USB_DEREGISTERED "usbDeregistered"
#define METHOD_NH_USB_DEREGISTERED_SIG "(Lcom/stericsson/sdk/equipment/io/usb/internal/USBDeviceKey;)V"
#define METHOD_NH_ACCEPT_USB_DEVICE "acceptUSBDevice"
#define METHOD_NH_ACCEPT_USB_DEVICE_SIG "(II)Z"
#define METHOD_NH_GET_INSTANCE "getInstance"
#define METHOD_NH_GET_INSTANCE_SIG "()Lcom/stericsson/sdk/equipment/io/usb/internal/USBNativeHandler;"

#define METHOD_ND_GET_KEY "getKey"
#define METHOD_ND_GET_KEY_SIG "()Lcom/stericsson/sdk/equipment/io/usb/internal/USBDeviceKey;"

#define METHOD_DK_GET_BUS_DIRNAME "getBusDirname"
#define METHOD_DK_GET_BUS_DIRNAME_SIG "()Ljava/lang/String;"
#define METHOD_DK_GET_DEV_FILENAME "getDeviceFilename"
#define METHOD_DK_GET_DEV_FILENAME_SIG "()Ljava/lang/String;"
#define METHOD_DK_GET_DEVNUM "getDevnum"
#define METHOD_DK_GET_DEVNUM_SIG "()I"
#define METHOD_DK_GET_VENDOR_ID "getVendorId"
#define METHOD_DK_GET_VENDOR_ID_SIG "()I"
#define METHOD_DK_GET_PRODUCT_ID "getProductId"
#define METHOD_DK_GET_PRODUCT_ID_SIG "()I"


#define METHOD_CONSTRUCTOR "<init>"
#define METHOD_CONST_DEFAULT_SIG "()V"
#define METHOD_CONST_USB_NATIVE_DEVICE_SIG "(Lcom/stericsson/sdk/equipment/io/usb/internal/USBNativeDevice$DeviceDescriptor;)V"
#define METHOD_CONST_USB_DEVICE_KEY_SIG "(ILjava/lang/String;Ljava/lang/String;II)V"

#define FN_DEVNUM "devNum"
#define FN_BUS_DIR_NAME "busDirName"
#define FN_DEVICE_FILE_NAME "deviceFileName"
#define FN_DEVICE_DESCRIPTOR "deviceDescriptor"
#define FN_USB_DESCRIPTOR "usbDescriptor"
#define FN_USB_CONF_DESCRIPTOR_ARRAY "configDescriptors"
#define FN_USB_IDESCRIPTOR_ARRAY "interfaces"
#define FN_USB_EPDESCRIPTOR_ARRAY "endpoints"

#define F_DEVICE_DESCRIPTOR_SIG "Lcom/stericsson/sdk/equipment/io/usb/internal/USBNativeDevice$DeviceDescriptor;"
#define F_USB_DESCRIPTOR_SIG "Lcom/stericsson/sdk/equipment/io/usb/internal/USBNativeDevice$USBDescriptor;"
#define F_USB_CONF_DESCRIPTOR_ARRAY_SIG "[Lcom/stericsson/sdk/equipment/io/usb/internal/USBNativeDevice$USBConfigDescriptor;"
#define F_USB_IDESCRIPTOR_ARRAY_SIG "[Lcom/stericsson/sdk/equipment/io/usb/internal/USBNativeDevice$USBInterfaceDescriptor;"
#define F_USB_EPDESCRIPTOR_ARRAY_SIG "[Lcom/stericsson/sdk/equipment/io/usb/internal/USBNativeDevice$USBEndpointDescriptor;"

//field names for usb_descriptor
#define FN_USBD_BLENGTH "bLength"
#define FN_USBD_BDESC_TYPE "bDescriptorType"
#define FN_USBD_BCDUSB "bcdUSB"
#define FN_USBD_BDEV_CLASS "bDeviceClass"
#define FN_USBD_BDEV_SUBCLASS "bDeviceSubClass"
#define FN_USBD_BDEV_PROT "bDeviceProtocol"
#define FN_USBD_BMAX_PKT_SIZE0 "bMaxPacketSize0"
#define FN_USBD_ID_VENDOR "idVendor"
#define FN_USBD_ID_PRODUCT "idProduct"
#define FN_USBD_MANUFACTURER "manufacturerDesc"
#define FN_USBD_PRODUCT "productDesc"
#define FN_USBD_SERIAL "serialNumberDesc"
#define FN_USBD_BCDDEV "bcdDevice"
#define FN_USBD_BNUM_CONF "bNumConfigurations"

//field names for usb_config_descriptor
#define FN_CONFD_BLENGTH "bLength"
#define FN_CONFD_BDESC_TYPE "bDescriptorType"
#define FN_CONFD_WTOT_LENGTH "wTotalLength"
#define FN_CONFD_BNUM_INT "bNumInterfaces"
#define FN_CONFD_BCONF_VAL "bConfigurationValue"
#define FN_CONFD_DESC "configurationDesc"
#define FN_CONFD_BM_ATTRS "bmAttributes"
#define FN_CONFD_MAX_POWER "maxPower"

//field names for usb_interface_descriptor
#define FN_USB_IDESC_BLENGTH "bLength"
#define FN_USB_IDESC_BDESC_TYPE "bDescriptorType"
#define FN_USB_IDESC_BINT_NUM "bInterfaceNumber"
#define FN_USB_IDESC_BALT_SETS "bAlternateSetting"
#define FN_USB_IDESC_BNUM_ENDPOINTS "bNumEndpoints"
#define FN_USB_IDESC_BINT_CLASS "bInterfaceClass"
#define FN_USB_IDESC_BINT_SUBCLASS "bInterfaceSubClass"
#define FN_USB_IDESC_BINT_PROT "bInterfaceProtocol"
#define FN_USB_IDESC_DESC "interfaceDesc"

//field names for usb_endpoint_descriptor
#define FN_USB_EPDESC_BLENGTH "bLength"
#define FN_USB_EPDESC_BDESC_TYPE "bDescriptorType"
#define FN_USB_EPDESC_BEP_ADDR "bEndpointAddress"
#define FN_USB_EPDESC_BM_ATTRS "bmAttributes"
#define FN_USB_EPDESC_WMAX_PKT_SIZE "wMaxPacketSize"
#define FN_USB_EPDESC_BINT "bInterval"
#define FN_USB_EPDESC_BREFRESH "bRefresh"
#define FN_USB_EPDESC_BSYNCH_ADDR "bSynchAddress"

#endif /* JAVADEFS_H_ */
