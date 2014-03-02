Make sure that the following files is not copied to this folder:
general_gui_setting.cmm
general_path_setting.cmm



/ux500/software/common/prcmu_fw must be available due to the cpt scripts

Why is jtag clock set to 5MHz for ARM9

Debug through reset is not working with new TRACE32, enreset must be off to get it working

Modifications:

Added sys.config core command in 
5500_debug_config_ARM9
5500_debug_config_CEVA


\ to / in db5500_boot_emulation, db5500_top_script, init_5500

prcmufw_5500a0_ste.bin to prcmufw_5500A0_ste.bin