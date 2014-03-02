How to import new CPT delivery :
--------------------------------
under LINUX: 
unzip ../CPT_Debug_Trace_scripts_vXXXXXXX-2012
chmod -R +w  *
dos2unix `find . | grep -v xls`

=> Merge the modifications
  (to be abble to keep patch on last CPT delivery if necessary)

=========
 WARNING : Make sure that the following files is not copied to this folder
=========
general_gui_setting.cmm
general_path_setting.cmm
