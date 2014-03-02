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

// type of command
#define READHWREG_TYPE_HWREG	0x0
#define READHWREG_TYPE_PE	0x1

#endif