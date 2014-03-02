/*
 * Copyright (C) ST-Ericsson SA 2010
 * Author: Chetan Nanda/chetan.nanda@stericsson.com for ST-Ericsson.
 * License terms: GNU General Public License (GPL), version 2.
 */

 /*
 * Purpose of this this driver is to read/write ISP 8500 page-elements 
 */
#include <linux/init.h>		/* Initiliasation support */
#include <linux/module.h>	/* Module support */
#include <linux/kernel.h>	/* Kernel support */
#include <linux/version.h>	/* Kernel version */
#include <linux/fs.h>		/* File operations (fops) defines */
#include <linux/cdev.h>		/* Charactor device support */
#include <linux/ioport.h>	/* Memory/device locking macros   */
#include <linux/errno.h>	/* Defines standard error codes */
#include <linux/miscdevice.h>
#include <linux/platform_device.h>
#include <linux/uaccess.h>
#include <linux/io.h>
#include "readHwReg_ioctl.h"
#include <mach/hardware.h>

//Base address, XP70 internal data memory
#define PICTOR_IN_XP70_TCDM_MEM_BASE_ADDR	(0x60000)

#define FW_TO_HOST_ADDR_MASK		(0x00001FFF)
#define FW_TO_HOST_ADDR_SHIFT		(0xD)
#define FW_TO_HOST_CLR_MASK		(0x3F)
#define SIA_ISP_MEM				(0x56000)
#define SIA_ISP_MEM_PAGE_REG			(0x54070)

#define SIA_SIA_BASE_ADDR                                   (0x54000)
#define SIA_ISP_MCU_SYS_ADDR0_OFFSET                        (SIA_SIA_BASE_ADDR + 0x40)
#define SIA_CLK_ENABLE_OFFSET	(0x20)

//Debug interface
#ifdef DEBUG
#define DEBUG_LOG(...) printk(KERN_ALERT "ReadHwReg-driver: " __VA_ARGS__)
#else
#define DEBUG_LOG(...)
#endif

//Access request
#define READHWREG_READ  0x0
#define READHWREG_WRITE  0x1

//Misc device struct for the driver
static struct miscdevice misc_dev;

//Minor number allocated to the driver
static int major_device_number;

//SIA base address, after ioremap
u32 g_siaBaseAdress;

//Driver can be used with one application at a time
unsigned char gOpened = 0;
	
/*
Struct DEBUG_PEInterface is assigned a fix address.
It is placed at the end of IDM before grab_notify struct

With this fixed address, this stuct can be accessed by HOST and can be used to send command
*/
typedef struct DEBUG_PEInterface
{
	unsigned int u32_PEAddress;	//PE address to be access
	unsigned int u32_PEVal;		//PE Value
	unsigned int u32_PESize;	//Size of PE
	unsigned int u32_PEAccess;	//Read/Write
	unsigned int u32_PEControl;	//Control/Status - for sending the command
	unsigned int u32_PEStatus;	//Control/Status - for sending the command
	unsigned int u32_dummy_1;	//Dummy1
	unsigned int u32_dummy_2;	//Dummy2
}DEBUG_PEInterface_ts;

volatile static DEBUG_PEInterface_ts debugPEIntf;

// readHwReg_open - 
// Open function for this driver
int readHwReg_open(struct inode *node, struct file *filp)
{
	if(gOpened)
	{
		DEBUG_LOG("Driver in used ...\n");
		return -1;
	}

	gOpened++;
	DEBUG_LOG("Device Opened ...");
	
	return 0;
}

// readHwReg_open - 
// Release function for this driver
int readHwReg_release(struct inode *node, struct file *filp)
{

	if(gOpened)
		gOpened--;	

	DEBUG_LOG("Device released ...");
	return 0;
}

//T1_to_ARM - to convert T1 address to address accessible by ARM
static u32 t1_to_arm(u32 t1_addr, void __iomem *smia_base_address,
		     u16 *p_mem_page)
{
	u16 mem_page_update = 0;
	mem_page_update = (t1_addr >> FW_TO_HOST_ADDR_SHIFT) &
			  FW_TO_HOST_CLR_MASK;

	if (mem_page_update != *p_mem_page) {
		/* Update sia_mem_page register */
		writew(mem_page_update, smia_base_address + SIA_ISP_MEM_PAGE_REG);
		*p_mem_page = mem_page_update;
	}

	return SIA_ISP_MEM + (t1_addr & FW_TO_HOST_ADDR_MASK);
}

//Process Read/Write page elements
static int process_readwritePEReq(sMsgBuf_t *data)
{
	unsigned int offset = 0;
	int i = 0, cnt = 0;
	unsigned int itval = 0;
	unsigned short mem_page = 0;
	unsigned int baseAddr = PICTOR_IN_XP70_TCDM_MEM_BASE_ADDR + 0x1FC0;

	//Convert address to PE adress	
	debugPEIntf.u32_PEAddress = (data->u32_address) & (0xFFFF);
	debugPEIntf.u32_PEAccess = data->u8_access;
	debugPEIntf.u32_PEVal = 0x0;
	debugPEIntf.u32_PESize= data->u32_dataSize;
	
	if(data->u8_access)//Write
		debugPEIntf.u32_PEVal = data->u32_data;
		
	debugPEIntf.u32_PEControl = debugPEIntf.u32_PEStatus +1;
	DEBUG_LOG("u32_PEAddress = %x, Access = %s\n",debugPEIntf.u32_PEAddress, data->u8_access?"WRITE":"READ");
	
	//Write the 'debugPEIntf' to XP70 TDCM memory
	for(i = baseAddr; i < (baseAddr + 64); i = i + 2 )
	{
		itval = t1_to_arm(i, (void __iomem *)g_siaBaseAdress, &mem_page);
		itval = ((u32) g_siaBaseAdress) + itval;

		/* Copy fw data in TCDM */
		writew((*((u16 *) &debugPEIntf + offset++)), itval);
	}

	//PE request has been sent to XP70 FW
	//Wait in while loop for completion of this request	
	while(1)
	{
		//Read back 'debugPEIntf' from XP70 TDCM memory
		offset = 0;
		mem_page = 0;
		for(i = baseAddr; i < (baseAddr + 64); i = i + 2 )
		{
			itval = t1_to_arm(i, (void __iomem *)g_siaBaseAdress, &mem_page);
			itval = ((u32) g_siaBaseAdress) + itval;

			/* Copy fw data in TCDM */
			(*((u16 *) &debugPEIntf + offset++)) = readw( itval);
		}

		//Check if the request has been processed by ISP FW
		if(debugPEIntf.u32_PEStatus == debugPEIntf.u32_PEControl)
		{
			//Request has been processed --break here			
			break;
		}

		//ISP FW has not yet processed the request ... retry
		printk("Waiting ... \n");
		cnt++;
		
		if(cnt > 10000)
			break;	
	}	

	//Update the data to be returned to application
	data->u32_data = debugPEIntf.u32_PEVal;
	return 0;
}
	
/* readHwReg_ioctl - 
** Driver IOCTL implementation
*/
long readHwReg_ioctl(struct file *filp, unsigned int cmd,  unsigned long arg)
{
	sMsgBuf_t data;
	long ret = 0;
	
	switch (cmd) 
	{
		case READHWREG_CAM_READREG:
		case READHWREG_CAM_WRITEREG:
		{
			unsigned int 	address;
   
			memset(&data, 0, sizeof(sMsgBuf_t));
				
			if (copy_from_user(&data, (sMsgBuf_t *)arg, sizeof(sMsgBuf_t))) 
			{
				printk("Could not copy data from userspace successfully\n");
				break;
			}
			DEBUG_LOG("READHWREG Driver: Read HwReg: 0x%4x",data.u32_address);

			if(!g_siaBaseAdress)
				return -ENOMEM;	

			address = g_siaBaseAdress + (data.u32_address - U8500_SIA_BASE);
			
            // TBD: Check data size is wither 1, 2 or 4
			
			// Check that the address to be accessed is aligned as per the data size
			if ((data.u32_dataSize != 1) && (address % data.u32_dataSize != 0))
			{
			    printk("Address is not aligned; address = 0x%x, alignment = 0x%x\n", address, data.u32_dataSize);
				ret = -EFAULT;
				break;
			}

			if (data.u8_access == READHWREG_READ)
			{
				// Read request
				DEBUG_LOG("%d %s: Offset = 0x%x, data = 0x%x\n", __LINE__, __FUNCTION__, address, data.u32_dataSize);
			
				if (data.u32_dataSize == 1)
				{
				    // TBD: Presumption is that tha address passed is 2-byte aligned
					data.u32_data = ((unsigned int) ioread16(address)) & 0x000000FF;
				}
				else if (data.u32_dataSize == 2)
				{
					data.u32_data = ((unsigned int) ioread16(address)) & 0x0000FFFF;
				}
				else
				{
					data.u32_data = (unsigned int) ioread32(address);
				}
				
				DEBUG_LOG("%d %s: Offset = 0x%x, size = %d, data = 0x%x\n", __LINE__, __FUNCTION__, address, data.u32_dataSize, data.u32_data);
			}
			else
			{
				// Write request
				DEBUG_LOG("%d %s: Offset = 0x%x, data = 0x%x\n", __LINE__, __FUNCTION__, address, data.u32_dataSize);
				
				if (data.u32_dataSize == 1)
				{
				    // TBD: Presumption is that tha address passed is 2-byte aligned
					iowrite16((data.u32_data & 0x000000FF), address);
				}
				else if (data.u32_dataSize == 2)
				{
					iowrite16((data.u32_data & 0x0000FFFF), address);
				}
				else
				{
					iowrite32(data.u32_data, address);
				}
			
				DEBUG_LOG("%d %s: Offset = 0x%x, size = %d, regval = 0x%x\n", __LINE__, __FUNCTION__, address, data.u32_dataSize, data.u32_dataSize);
			}
   
			if (copy_to_user((sMsgBuf_t *)arg,  &data, sizeof(sMsgBuf_t))) 
			{
				printk("Copy to userspace failed\n");
				ret = -EFAULT;
				break;
			}
        }
		
		break;
		
		case READHWREG_CAM_READPE:
		case READHWREG_CAM_WRITEPE:
		{
			memset(&data, 0, sizeof(sMsgBuf_t));
				
			if (copy_from_user(&data, (sMsgBuf_t *)arg, sizeof(sMsgBuf_t))) 
			{
				printk("Could not copy data from userspace successfully\n");
				break;
			}
			DEBUG_LOG(KERN_INFO "READHWREG Driver: Read HwReg: 0x%4x",data.u32_address);
			
			process_readwritePEReq(&data);

			if (copy_to_user((sMsgBuf_t *)arg,  &data, sizeof(sMsgBuf_t))) 
			{
				printk("Copy to userspace failed\n");
				ret = -EFAULT;
				break;
			}

		}
		break;
	}

	return ret;
}

static const struct file_operations readHwReg_fops = {
	.owner 	=	THIS_MODULE,
	.unlocked_ioctl 	=	readHwReg_ioctl,
	.open 	=	readHwReg_open,
	.release 	= 	readHwReg_release,
};

static int __init readHwReg_init(void)
{
	int err = 0;
	
	/* Register misc device */
	misc_dev.minor = MISC_DYNAMIC_MINOR;
	misc_dev.name = READHWREG_NAME;
	misc_dev.fops = &readHwReg_fops;
	err = misc_register(&misc_dev);
	
	if (err < 0) 
	{
		printk(KERN_INFO READHWREG_NAME " driver misc_register failed (%d)\n", err);
		return err;
	} 
	else 
	{
		major_device_number = err;
		printk(KERN_INFO READHWREG_NAME " driver initialized with minor=%d - Modified\n", misc_dev.minor);
	}

	//Driver registration completed successfuly

	//MAP sia address space onto kernel address space, This is needed for accessing the SIA registers
	g_siaBaseAdress = (u32) ioremap_nocache(U8500_SIA_BASE, 1024 * 1024);
	if (!g_siaBaseAdress) {
		printk("\n%d %s: IO-Remap of SIA base address failed\n",__LINE__,__FUNCTION__);
		return -ENOMEM;
	}

	return 0;
}

static void __exit readHwReg_exit(void)
{
	misc_deregister(&misc_dev);
	iounmap((void __iomem *)g_siaBaseAdress);
	
	printk(KERN_INFO READHWREG_NAME "driver unregistered\n");
}

module_init(readHwReg_init);
module_exit(readHwReg_exit);

MODULE_AUTHOR("STEricsson");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("U8500 ReadPE Camera driver");
