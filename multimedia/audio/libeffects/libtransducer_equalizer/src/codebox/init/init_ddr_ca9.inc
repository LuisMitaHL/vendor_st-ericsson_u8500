// -----------------------------------------------------------------
// Script file for EB SDRAM Initialization for RVD
// -----------------------------------------------------------------

// cmd set config mode
setmem /w 0x10018004 = 0x00000004

//  -pl340 ID config
setmem /w 0x10018100 = 0x00000003

// ddr setup 
//  -pl340 init 
setmem /w 0x10018010 = 0x00000353

// cas latency 
setmem /w 0x10018014 = 0x00000005
// t_dqss
setmem /w 0x10018018 = 0x00000001
// t_mrd
setmem /w 0x1001801c = 0x00000002
// t_ras
setmem /w 0x10018020 = 0x00000005
// t_rc
setmem /w 0x10018024 = 0x00000008
// t_rcd
setmem /w 0x10018028 = 0x00000003
// t_rfc
setmem /w 0x1001802c = 0x0000018f
// t_rp
setmem /w 0x10018030 = 0x00000003
// t_rrd
setmem /w 0x10018034 = 0x00000002
// t_wr
setmem /w 0x10018038 = 0x00000002
// t_wtr
setmem /w 0x1001803c = 0x00000002
// t_xp
setmem /w 0x10018040 = 0x00000001
// t_xsr
setmem /w 0x10018044 = 0x00000018
// t_esr
setmem /w 0x10018048 = 0x00000018

//  -pl340 config 
//  1 chip //0x6c8009

// burst of 2
// setmem /w 0x1001800c = 0x006c802A

// burst of 4
// setmem /w 0x1001800c = 0x006d002A

// burst of 8
setmem /w 0x1001800c = 0x006d802A

// chip_cfg0
setmem /w 0x10018200 = 0x000000f0 

//  -pl340 initmem 
// nop
setmem /w 0x10018008 = 0x000c0000
// pre
setmem /w 0x10018008 = 0x00000000
// emr	was 0x90000 (DLL enable)   , 0x90001 (DLL disable)
setmem /w 0x10018008 = 0x00090001
// mr
setmem /w 0x10018008 = 0x00080123
// pa
setmem /w 0x10018008 = 0x00000000
// ar
setmem /w 0x10018008 = 0x00040000
// ar
setmem /w 0x10018008 = 0x00040000
// mr
setmem /w 0x10018008 = 0x00080023

// delay start 
dump /w 0x10018000..0x10018014

// delay end 

// cmd
setmem /w 0x10018004 = 0x00000000

//  ddr setup complete 

// Remap
setmem /w 0x10001000 = 0x00000309
