/* appReadHwReg 
	- Application that help in debugging by read/write SIA hw reg over adb connection
	- 	
   
   AUTHOR: Chetan Nanda (chetan.nanda@stericsson.com)
*/

#ifndef __APP_READ_HW_REG_H__
#define __APP_READ_HW_REG_H__

//Access request
#define READHWREG_READ  0x0
#define READHWREG_WRITE  0x1

// Status of last command
#define READHWREG_STATUS_OK	0x0
#define READHWREG_STATUS_FAILED	0x1

/* Message structure between client and server
*/
typedef struct
{
	unsigned int u32_address;
	unsigned int u32_data;
	unsigned int u32_dataSize;
	unsigned char u8_access;
	unsigned char u8_status;
}sMsgBuf_t;

#endif