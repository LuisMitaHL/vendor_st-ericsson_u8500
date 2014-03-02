#	
LMM       =  make -C /work1/1220/vendor/st-ericsson/multimedia/linux/build -f lbp.mk DIRS_TO_INSTALL=" " 
LMM_CMD   =  mm-all
LMM_PARAMS=  BUILD_IMAGING=true 
LMM_DEBUG =  DEBUG=1 FORCE_EXPORT_OF_SOURCES=1	

Targets=osi_toolbox omx_toolbox wom wrapper bellagio fakesink

all:$(Targets)

.PHONY: build_lmm 

osi_toolbox: DIRS_TO_BUILD="imaging/img_common/osi_toolbox"
osi_toolbox: build_lmm

omx_toolbox: DIRS_TO_BUILD="imaging/img_common/omx_toolbox"
omx_toolbox: build_lmm


wom: DIRS_TO_BUILD="imaging/img_common/wom"
wom: build_lmm
	
fakesink: DIRS_TO_BUILD="imaging/img_valid_omx/omx_components/fakesink"
fakesink: build_lmm

	
mmte: DIRS_TO_BUILD="shared/mmte"
mmte: build_lmm
	
ens: DIRS_TO_BUILD="shared/ens"
ens: build_lmm

imgens: DIRS_TO_BUILD="imaging/img_common/img_ens"
imgens: build_lmm

wrapper: DIRS_TO_BUILD="imaging/img_common/img_enswrapper"
wrapper: build_lmm

bellagio: DIRS_TO_BUILD="linux/bellagio_ste_omxloader"
bellagio: LMM_DEBUG=
bellagio: build_lmm

all: $(foreach Target, $(Targets), $(Target))
	@echo rebuild done

n: Targets= wom wrapper bellagio fakesink
n: all


run: LMM_CMD=lbp-ramdisk
run: build_lmm
	../../../../../svp_remote_tools/bin/launch_svp.sh
	
build_lmm:
	$(LMM)  DIRS_TO_BUILD=$(DIRS_TO_BUILD) $(LMM_PARAMS) $(LMM_DEBUG) $(LMM_CMD)

build_lmm_%:
	$(LMM)  DIRS_TO_BUILD="$*" $(LMM_PARAMS) $(LMM_DEBUG) $(LMM_CMD)







