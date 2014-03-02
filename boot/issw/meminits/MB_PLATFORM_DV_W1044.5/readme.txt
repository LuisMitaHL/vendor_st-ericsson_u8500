
This folder contains memory initialization settings DB8500V2.0 (cutID B0) in binary, C source file and lauterbach CMM format


  * Compatible with PRCMU F/W version B0_V1.2.1, boot image 4.20.5 for DB8500V2.0
  * settings for:
	- toshiba ldDDR (4Gb) @400mhz

What’s new :
  => Update Lauterbach Trace32 scripts for DB8500V2:
    - Align with SOC scripts W1043.2 (V2.1.0)
    - Align with 8500-B0 PRCMUFW version B0_V1.2.1
    - Update scripts for DB8500V2

	
 Files:
	hrefp_v20.zip:
		hrefp_v20_db8500b0_secst:
			Contains MemInit source code and binaries for HREF+ V2.0 with locked db8500b0 (aka DB8500V2.0).
		hrefp_v20_db8500b0_unsec:
			Contains MemInit source code and binaries for HREF+ V2.0 with unlocked db8500b0 (aka DB8500V2.0).

	mop_v20.zip:
		mop_v20_db8500b0_secst:
			Contains MemInit source code and binaries for MOP V2.0 with locked db8500b0 (aka DB8500V2.0).
		mop_v20_db8500b0_unsec:
			Contains MemInit source code and binaries for MOP V2.0 with unlocked db8500b0 (aka DB8500V2.0).

	t32_connection.zip:
		t32_connection:
			Contains scripts to launch debug session with Lauterbach Trace32 on 
			all existing configurations (HREF/HREF+/MOP, locked/unlocked DB8500 ED/A0/A1/B0).
			Launch your session via 't32cde' shell script (needs csh interpreter) or dualdebug.bat


	Archives of the directories above, also available on CODEX
	https://codexstn.cro.st.com/file/showfiles.php?group_id=552 in MB_PLATFORM_DV_W1044.5 release


