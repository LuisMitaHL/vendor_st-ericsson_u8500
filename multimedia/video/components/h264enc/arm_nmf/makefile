
# All target for lib , main and test
RVD = source /sw/st/gnu_compil/comp/arm/rvds/3.1/linux/RVDS31env.posh && rvdebug

#ERC_56
#STREAM = /prj/multimedia_shared/video/bitstreams/H264/CUSTOM/EC/rs_80x96_1copy.264

#ERC_61
#STREAM =/prj/multimedia_shared/video/bitstreams/H264/CUSTOM/EC/rs_80x96_partialcopy.264

#ERC_0ef
#STREAM =/prj/multimedia_shared/video/bitstreams/H264/CUSTOM/EC/akiyo_CIF_256000.264

# ERC_31
#STREAM = /prj/multimedia_shared/video/bitstreams//H264/CUSTOM/EC/news_CIF_128000.264

#ERC07
#STREAM = /prj/multimedia_shared/video/bitstreams/H264/CUSTOM/EC/carphone_QCIF_128000.264


#ERC 04
#STREAM    = /prj/multimedia_shared/video/bitstreams/H264/CUSTOM/EC/akiyo_QCIF_128000.264


STREAM = /prj/multimedia_shared/video/sequences_SUBQCIF/bowing_SUBQCIF_76_F7_5.yuv

all: lib-arm ddep # lib-armstub
clean-all: clean-lib-arm clean-ddep # clean-lib-armstub

test-gcc : main-gcc refcode test-only


test-only:
	echo " testing execution on linux"
	\rm -rf dut.log ref.log
	refcode/bin/h264enc $(STREAM) -digests -nodeblock > ref.log
	\cp out_yuv.digest  ref_yuv.digest
	test_appli/bin/$(target)/h264encnmf.exe $(STREAM) -digests > dut.log
	cmp ref_yuv.digest out_yuv.digest




test-all : main-gcc refcode
	echo " testing execution on linux"
	perl ./test_appli/script/testdec.pl  ./test_appli/script/data/all.lst



main-gcc : lib-gcc
	$(MAKE) -C test_appli -f makefile PLATFORM=$(target)-linux SELECTED_CONF=_gcc

clean-gcc : clean-lib-gcc
	$(MAKE) -C test_appli -f makefile clean PLATFORM=$(target)-linux SELECTED_CONF=_gcc



####	ARM/ Hamac DSP  related rules ######################
lib-gcc :
	$(MAKE) -C h264enc_gcc -f ../make-lib  PLATFORM=$(target)-linux SELECTED_CONF=_gcc

clean-lib-gcc :
	$(MAKE) -C h264enc_gcc -f ../make-lib  clean PLATFORM=$(target)-linux SELECTED_CONF=_gcc




##### ARM LIB rules ##########################
lib-arm :  #dsp_component
	$(MAKE) -C h264enc -f ../make-lib  

install-lib-arm :  
	$(MAKE) -C h264enc -f ../make-lib install

clean-lib-arm :  #clean-dsp_component
	$(MAKE) -C h264enc -f ../make-lib clean 


##### ARM STUB LIB rules ##########################

lib-armstub :  
	$(MAKE) -C ddep NO_HAMAC=1 SUFFIX=_stub
	$(MAKE) -C h264enc_stub  -f ../make-lib  SELECTED_CONF=_stub
	$(MAKE) -C proxynmf_stub -f ../make-ddep NO_HAMAC=1 SUFFIX=_stub

install-lib-armstub :  
	$(MAKE) -C h264enc_stub -f ../make-lib  SELECTED_CONF=_stub install
	$(MAKE) -C proxynmf_stub -f ../make-ddep NO_HAMAC=1 SUFFIX=_stub install

clean-lib-armstub :  
	$(MAKE) -C h264enc_stub -f ../make-lib  SELECTED_CONF=_stub clean
	$(MAKE) -C proxynmf_stub -f ../make-ddep NO_HAMAC=1 SUFFIX=_stub clean

ddep: lib-arm
	$(MAKE) -C ddep
	$(MAKE) -C proxynmf -f ../make-ddep SUFFIX=
	$(MAKE) -C proxynmf -f ../make-ddep SUFFIX=_hdtv

install-ddep:
	$(MAKE) -C proxynmf -f ../make-ddep SUFFIX= install
	$(MAKE) -C proxynmf -f ../make-ddep SUFFIX=_hdtv install

clean-ddep : clean-lib-arm #clean-lib-armstub 
	$(MAKE) -C proxynmf -f ../make-ddep SUFFIX= clean
	$(MAKE) -C proxynmf -f ../make-ddep SUFFIX=_hdtv clean


##### build all libs ##########################
libs:	 lib-arm # lib-armstub

clean-libs: clean-lib-arm # clean-lib-armstub
clean realclean: clean-arm clean-ddep # clean-armstub

install: install-lib-arm install-ddep # install-lib-armstub

################## Build all DSP components #######################################
dsp_component :
	$(MAKE) -C ../mpc/algo -f Makefile all  
	$(MAKE) -C ../mpc/ddep -f Makefile all  
	$(MAKE) -C ../mpc/adapter -f Makefile all  
	$(MAKE) -C ../../debug -f Makefile all  
#	$(MAKE) -C ../../resource_manager -f Makefile all  

clean-dsp_component : 
	$(MAKE) -C ../mpc/algo      -f Makefile clean  
	$(MAKE) -C ../mpc/ddep      -f Makefile clean  
	$(MAKE) -C ../mpc/adapter   -f Makefile clean  
	$(MAKE) -C ../../debug      -f Makefile clean  
#	$(MAKE) -C ../../resource_manager -f Makefile clean  
###########################################





###################################################################################
main-arm : lib-arm
	\rm -f     test_appli/src/configCM.c
	$(MAKE) -f makefile -C test_appli 

clean-arm : clean-lib-arm
	$(MAKE) -C test_appli -f makefile clean 

###################################################################################




####	ARM full stub related rules ######################

main-armstub : lib-armstub
	\rm -f     test_appli/src/configCM.c
	$(MAKE) -C test_appli -f makefile  SELECTED_CONF=_stub

clean-armstub : clean-lib-armstub
	$(MAKE) -C test_appli -f makefile clean  SELECTED_CONF=_stub

####Generation of refcode needed for Test execution ##############

refcode :
	$(MAKE) -C refcode

rvdm:
	$(RVD) -aws=- -nologo  -inc='./test_appli/script/rvd_marty.inc' --stdiolog='log.log'

rvd:
	$(RVD) -aws=- -nologo  -inc='./test_appli/script/rvd_$(shell whoami).inc' --stdiolog='log.log'

svp:
	#source /prj/multimedia_shared/common/TOOLS/Linux/internals/TLM/SVP/SVP-2.3-Linux-x86-SVP_8500_2.3.0.46-RedHatEnterpriseWS_4-x86_64-gcc_3.4.6/svp/svp.csh
	\rm -rf argument.txt
	echo $(STREAM) >> argument.txt
	tlm_run.exe -arm_lib ./test_appli/bin/cortexA9-think-rvct-svp8500_ed/h264encnmf.axf -stop_onexit -arm_opt "$(STREAM) -digests"


svp_linux: 
	perl test_appli/script/testsvp_linux.pl 
	make clean -C $(MMROOT)/linux/ramdisk 
	make -C $(MMROOT)/linux/ramdisk 

svp_linux_omx: 
	perl test_appli/script/testsvp_linux.pl omx
	make clean -C $(MMROOT)/shared/mmte
	make -C $(MMROOT)/shared/mmte
	make clean -C $(MMROOT)/linux/ramdisk 
	make -C $(MMROOT)/linux/ramdisk 

svpdebug_model:
	/sw/arm/system_generator/4.2/FastModelTools_4.2/ModelDebugger_4.2/bin/modeldebugger &
	\rm -rf argument.txt
	echo $(STREAM) >> argument.txt
	tlm_run.exe -arm_lib ./test_appli/bin/cortexA9-think-rvct-svp8500_ed/h264encnmf.axf -arm_opt "./build/host.axf SVASIA" -arm_debug -pv 2000 -wodbgv &
	#tlm_run.exe -arm_lib ./test_appli/bin/cortexA9-think-rvct-svp8500_ed/h264encnmf.axf -arm_opt "./build/host.axf SVASIA" -arm_debug &

svpdebug_t32:
	\rm -rf test_appli/argument.txt
	echo $(STREAM) >> test_appli/argument.txt
	make -C test_appli svpdebug_t32

.PHONY : rvd main-gcc lib-gcc test-gcc main-arm lib-arm test-arm refcode clean-gcc


