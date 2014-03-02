# -*- Mode: Makefile -*-
# vim:syntax=make:

#------------------------------
# Symbian prj file generation
#------------------------------

# GROUP_PATH is the top directory for the symbian project
# GROUP_NAME is the group folder name where project files are generated
# elf4nmf_EXPORT_DIR define the destination directory where elf4nmf file are exported

#replace / by \/ and remove trailling /
MYGROUP_PATH:= $(subst /,\/,$(shell echo $(GROUP_PATH) | sed 's/\/$$//'))
MMROOT_VIDEO:=$(MMROOT)/video
MYMMROOT_VIDEO:= $(subst /,\/,$(shell echo $(MMROOT_VIDEO) | sed 's/\/$$//'))
MM_GROUP_PATH:= $(subst $(MMROOT_VIDEO)/,,$(GROUP_PATH))
REL_GROUP_NAME:= $(shell echo $(GROUP_NAME) | sed 's/[^\/]*/../g')
MYREL_GROUP_NAME:= $(subst /,\/,$(shell echo $(REL_GROUP_NAME) | sed 's/\/$$//'))
REL_GROUP_PATH:= $(subst $(GROUP_PATH),$(REL_GROUP_NAME),$(PWD))



HEADER_STRING_GENERATED:= "// This file is generated automatically"
HEADER_STRING_LINE1:= "/*"
HEADER_STRING_LINE2:= " * Copyright (C) ST-Ericsson SA 2010. All rights reserved."
HEADER_STRING_LINE3:= " * This code is ST-Ericsson proprietary and confidential."
HEADER_STRING_LINE4:= " * Any use of the code for whatever purpose is subject to"
HEADER_STRING_LINE5:= " * specific written permission of ST-Ericsson SA."
HEADER_STRING_LINE6:= " */"

export SYMBIAN_SYSTEMINCLUDE +=	/epoc32/include/assp/nomadik               \
			/epoc32/include/assp/nomadik/nmf

export SYMBIAN_BLD_EXTRA_MMP
export SYMBIAN_USERINCLUDE_EXTRA
export SYMBIAN_SYSTEMLIB
export SYMBIAN_SYSTEMSTATICLIB
export SYMBIAN_EXTRA_MMP_FLAG
export SYMBIAN_EXTRA_MMP
export elf4nmf_EXPORT_DIR
export SYMBIAN_VARIANT_TAG

# Export configuration dirs
SymbExportConf:= video audio shared sharedhost ens nmf

export video_DESTDIR = osi/video/
video_EXPORT_HEADER = $(wildcard $(addsuffix /*.h*,$(video_EXPORTDIR)))
video_EXPORT_HEADER += $(wildcard $(addsuffix /*.idt,$(video_EXPORTDIR)))
export video_EXPORT_HEADER

export audio_DESTDIR = osi/audio/
export audio_EXPORT_HEADER = $(wildcard $(addsuffix /*.h*,$(audio_EXPORTDIR)))

export shared_DESTDIR = osi/shared/
export shared_EXPORT_HEADER = $(wildcard $(addsuffix /*.h*,$(shared_EXPORTDIR)))

export sharedhost_DESTDIR = osi/shared/host/
export sharedhost_EXPORT_HEADER = $(wildcard $(addsuffix /*.h*,$(sharedhost_EXPORTDIR)))

export nmf_DESTDIR = osi/shared/nmf/
export nmf_EXPORT_HEADER = $(wildcard $(addsuffix /*.h*,$(nmf_EXPORTDIR)))

export ens_DESTDIR = osi/ens/
export ens_EXPORT_HEADER = $(wildcard $(addsuffix /*.h*,$(ens_EXPORTDIR)))


export SYMBIAN_SUPPORTED_VARIANT_TAG = ST_8500ED ST_8500V1 ST_8500V2
export ST_8500ED_name = 8500ed
export ST_8500V1_name = 8500v1
export ST_8500V2_name = 8500v2

export SYMBIAN_VARIANT_NAME=$($(SYMBIAN_VARIANT_TAG)_name)

symb-prj: symb-bld-inf symb-dllmmp symb-iby

symb-check:
	@echo "Checking Symbian targets"
	@case $(MMROOT) in \
	  */) echo "ERROR: remove trailing slash from MMROOT variable"; exit 1 ;;   \
	esac
	@if [ -z $(MMROOT) ] ; then \
	  echo "ERROR: variable MMROOT is not defined"; exit 1 ; \
	fi
	@if [ -z $(GROUP_PATH) ] ; then \
	  echo "ERROR: variable GROUP_PATH is not defined"; exit 1 ; \
	fi
	@if [ -z $(GROUP_NAME) ] ; then \
	  echo "ERROR: variable GROUP_NAME is not defined"; exit 1 ; \
	fi
	@if [ -z $(SYMBIAN_PRJNAME) ] ; then \
	  echo "ERROR: variable SYMBIAN_PRJNAME is not defined"; exit 1 ; \
	fi
	@if [ ! -d $(GROUP_PATH) ] ; then \
	  echo "ERROR: directory defined by GROUP_PATH does not exist"; exit 1 ; \
	fi
	@if [ ! -d $(MMROOT_VIDEO) ] ; then \
	  echo "ERROR: no video directory in MMROOT"; exit 1 ; \
	fi
	@if [ ! -d $(GROUP_PATH)/$(GROUP_NAME) ] ; then \
	  mkdir -p $(GROUP_PATH)/$(GROUP_NAME); \
	fi

export IBY_FILE_NAME:= "st_"$(shell echo $(SYMBIAN_PRJNAME) | sed 's/^st_//')
export IBY_NAME_UC:= $(shell echo $(SYMBIAN_PRJNAME) | sed 's/^st_//' | sed 'y/abcdefghijklmnopqrstuvwxyz/ABCDEFGHIJKLMNOPQRSTUVWXYZ/')

symb-bld-inf: symb-check
	@echo "Generating Symbian file bld.inf"
	@rm -f bld.inf-tmp
	@echo $(HEADER_STRING_GENERATED) >> bld.inf-tmp
	@echo $(HEADER_STRING_LINE1) >> bld.inf-tmp
	@echo $(HEADER_STRING_LINE2) >> bld.inf-tmp
	@echo $(HEADER_STRING_LINE3) >> bld.inf-tmp
	@echo $(HEADER_STRING_LINE4) >> bld.inf-tmp
	@echo $(HEADER_STRING_LINE5) >> bld.inf-tmp
	@echo $(HEADER_STRING_LINE6) >> bld.inf-tmp
	@echo "" >> bld.inf-tmp
	@echo "PRJ_PLATFORMS" >> bld.inf-tmp
	@echo "DEFAULT -WINSCW" >> bld.inf-tmp
	@echo "" >> bld.inf-tmp
	@echo "PRJ_EXPORTS" >> bld.inf-tmp
	@echo "$(IBY_FILE_NAME).iby   /epoc32/rom/include/" >> bld.inf-tmp
	@echo "$(IBY_FILE_NAME)_$(SYMBIAN_VARIANT_NAME).iby   /epoc32/rom/include/" >> bld.inf-tmp
	@echo "" >> bld.inf-tmp
	@echo "PRJ_EXPORTS" >> bld.inf-tmp
	@for conf in $(SymbExportConf); do \
	   hd="$${conf}_EXPORT_HEADER"; \
	   d="$${conf}_DESTDIR"; \
	   eval headers=\$$$${hd} ; \
	   eval dest=\$$$${d} ; \
	   for h in $${headers}; do \
	      echo "$(REL_GROUP_PATH)/$${h}    $${dest}" >> bld.inf-tmp; \
	   done; \
	done
	@echo "" >> bld.inf-tmp
	@echo "PRJ_EXPORTS" >> bld.inf-tmp
	@if [ -z $(elf4nmf_EXPORT_DIR) ] ; then \
	   echo "   WARNING: Variable elf4nmf_EXPORT_DIR not defined. No file elf to export."; \
	else \
	   if [ -d $(GROUP_PATH)/$(BASE_REPOSITORY_MPC) ] ; then \
	      complist=`find $(GROUP_PATH)/$(BASE_REPOSITORY_MPC) -name "*.elf4nmf" | sed -e 's#$(GROUP_PATH)/$(BASE_REPOSITORY_MPC)/##g'` ; \
	      for comp in $$complist; do \
		   cfrom="$(REL_GROUP_NAME)/$(BASE_REPOSITORY_MPC)/$$comp"; \
		   cto="STE_VIDEO_DATA_EXPORT_PATH($(MM_GROUP_PATH)/$(BASE_REPOSITORY_MPC)/$$comp)"; \
		   echo "$$cfrom  $$cto" >> bld.inf-tmp ; \
	      done; \
	   else \
	      echo "   WARNING: No path $(GROUP_PATH)/$(BASE_REPOSITORY_MPC) with elf4nmf file to export."; \
	   fi; \
	fi
	@echo "" >> bld.inf-tmp
	@echo "PRJ_MMPFILES" >> bld.inf-tmp
	@for mmp in $(SYMBIAN_BLD_EXTRA_MMP); do \
		echo "$$mmp" >>  bld.inf-tmp ; \
	done
	@echo "$(SYMBIAN_PRJNAME)_$(SYMBIAN_VARIANT_NAME).mmp" >> bld.inf-tmp
	@mv bld.inf-tmp $(GROUP_PATH)/$(GROUP_NAME)/bld.inf

symb-iby: symb-check
	@echo "Generating Symbian iby file for platform $(SYMBIAN_VARIANT_NAME)" 
	@rm -f iby-tmp
	@echo $(HEADER_STRING_GENERATED) >> iby-tmp
	@echo $(HEADER_STRING_LINE1) >> iby-tmp
	@echo $(HEADER_STRING_LINE2) >> iby-tmp
	@echo $(HEADER_STRING_LINE3) >> iby-tmp
	@echo $(HEADER_STRING_LINE4) >> iby-tmp
	@echo $(HEADER_STRING_LINE5) >> iby-tmp
	@echo $(HEADER_STRING_LINE6) >> iby-tmp
	@echo "" >> iby-tmp
	@echo "#ifndef __ST_$(IBY_NAME_UC)_$(SYMBIAN_VARIANT_TAG)_IBY__" >> iby-tmp
	@echo "#define __ST_$(IBY_NAME_UC)_$(SYMBIAN_VARIANT_TAG)_IBY__" >> iby-tmp
	@echo "" >> iby-tmp
	@echo "#include <st_video.iby>" >> iby-tmp
	@echo "" >> iby-tmp
	@echo "file=ABI_DIR\\DEBUG_DIR\\$(SYMBIAN_PRJNAME)_$(SYMBIAN_VARIANT_NAME).dll      \\sys\\bin\\$(SYMBIAN_PRJNAME).dll" >> iby-tmp
	@echo "" >> iby-tmp
	@if [ -z $(elf4nmf_EXPORT_DIR) ] ; then \
	   echo "   WARNING: Variable elf4nmf_EXPORT_DIR not defined. No file elf exported."; \
	else \
	   if [ -d $(GROUP_PATH)/$(BASE_REPOSITORY_MPC) ] ; then \
	      complist=`find $(GROUP_PATH)/$(BASE_REPOSITORY_MPC) -name "*.elf4nmf" | sed -e 's#$(GROUP_PATH)/$(BASE_REPOSITORY_MPC)/##g'` ; \
	      for comp in $$complist; do \
		   cfrom="STE_VIDEO_DATA_PATH/$(MM_GROUP_PATH)/$(BASE_REPOSITORY_MPC)/$$comp"; \
		   cto="$(elf4nmf_EXPORT_DIR)/$$comp"; \
		   echo "data=$$cfrom   $$cto" >> iby-tmp ; \
	      done; \
	   else \
	      echo "   WARNING: No path $(GROUP_PATH)/$(BASE_REPOSITORY_MPC) with elf4nmf exported files."; \
	   fi; \
	fi
	@echo "" >> iby-tmp
	@echo "#endif" >> iby-tmp
	@mv iby-tmp $(GROUP_PATH)/$(GROUP_NAME)/$(IBY_FILE_NAME)_$(SYMBIAN_VARIANT_NAME).iby
	@echo "Generating Symbian multi variant iby file" 
	@rm -f iby-tmp
	@echo $(HEADER_STRING_GENERATED) >> iby-tmp
	@echo $(HEADER_STRING_LINE1) >> iby-tmp
	@echo $(HEADER_STRING_LINE2) >> iby-tmp
	@echo $(HEADER_STRING_LINE3) >> iby-tmp
	@echo $(HEADER_STRING_LINE4) >> iby-tmp
	@echo $(HEADER_STRING_LINE5) >> iby-tmp
	@echo $(HEADER_STRING_LINE6) >> iby-tmp
	@echo "" >> iby-tmp
	@echo "#ifndef __ST_$(IBY_NAME_UC)_IBY__" >> iby-tmp
	@echo "#define __ST_$(IBY_NAME_UC)_IBY__" >> iby-tmp
	@echo "" >> iby-tmp
	@ibyvariant=`echo $(SYMBIAN_SUPPORTED_VARIANT_TAG) $(SYMBIAN_VARIANT_TAG)| tr ' ' '\n' | sort | uniq` ; \
	for y in $${ibyvariant}; do \
		nm="$${y}_name"; \
		eval var_nm=\$$$${nm} ; \
		echo "#ifdef $${y}" >> iby-tmp ; \
		echo "#include <$(IBY_FILE_NAME)_$${var_nm}.iby>" >> iby-tmp ; \
		echo "#endif" >> iby-tmp ; \
		echo "" >> iby-tmp ; \
	done
	@echo "#endif" >> iby-tmp 
	@mv iby-tmp $(GROUP_PATH)/$(GROUP_NAME)/$(IBY_FILE_NAME).iby

symb-dllmmp: symb-check
	@echo "Generating Symbian project mmp file for platform $(SYMBIAN_VARIANT_NAME)" 
	@rm -f mmp-tmp
	@echo $(HEADER_STRING_GENERATED) >> mmp-tmp
	@echo $(HEADER_STRING_LINE1) >> mmp-tmp
	@echo $(HEADER_STRING_LINE2) >> mmp-tmp
	@echo $(HEADER_STRING_LINE3) >> mmp-tmp
	@echo $(HEADER_STRING_LINE4) >> mmp-tmp
	@echo $(HEADER_STRING_LINE5) >> mmp-tmp
	@echo $(HEADER_STRING_LINE6) >> mmp-tmp
	@echo "" >> mmp-tmp
	@echo "TARGET        $(SYMBIAN_PRJNAME)_$(SYMBIAN_VARIANT_NAME).dll" >> mmp-tmp
	@echo "TARGETTYPE    dll" >> mmp-tmp
	@echo "LINKAS        $(SYMBIAN_PRJNAME).dll" >> mmp-tmp
	@echo "EXPORTLIBRARY $(SYMBIAN_PRJNAME).lib" >> mmp-tmp
	@echo "" >> mmp-tmp
	@echo "CAPABILITY all -tcb" >> mmp-tmp
	@echo "UID 0x1000008d" >> mmp-tmp
	@echo "" >> mmp-tmp
	@for flag in $(SYMBIAN_EXTRA_MMP_FLAG); do \
		echo "$$flag" >> mmp-tmp ; \
	done
	@echo "" >> mmp-tmp
	@macrodef=`echo $(CPPFLAGS) | tr ' ' '\n' | sed -e '/^-[^D]/ d' -e 's/^-D//' | sort | uniq` ; \
	for m in $$macrodef; do \
		echo "MACRO $$m" >> mmp-tmp ; \
	done
	@echo "" >> mmp-tmp
	@echo "// These directories should be included by default" >> mmp-tmp
	@echo "OS_LAYER_SYSTEMINCLUDE_SYMBIAN" >> mmp-tmp
	@for inc in $(SYMBIAN_SYSTEMINCLUDE); do \
		echo "SYSTEMINCLUDE   $$inc" >> mmp-tmp ; \
	done
	@echo "" >> mmp-tmp
	@echo "// User include directories" >> mmp-tmp
	@includirs=`echo $(CPPFLAGS) | tr ' ' '\n' | sed -e '/^-[^I]/ d' -e 's/^-I//' | sort | uniq` ; \
	for inc in $$includirs; do \
		case $$inc in \
		  $(MMROOT_VIDEO)*) rel=`echo $(MM_GROUP_PATH) | sed -e 's/[^\/]*/../g'` ; \
	                      dir=`echo $$inc | sed -e 's/$(MYMMROOT_VIDEO)//g'`; \
	                      incpath="$(REL_GROUP_NAME)/$$rel$$dir" ;; \
	          /*) echo "   ERROR: Invalid Include Path $$inc not in MMROOT/video folder"; \
	              exit 1 ;;   \
		  *) incpath=$(REL_GROUP_PATH)/$$inc ;;    \
		esac; \
		if [ ! -d $(MMROOT_VIDEO)/$(MM_GROUP_PATH)/$(GROUP_NAME)/$$incpath ] ; then \
		  echo "   WARNING: unexisting include directory $$incpath" ; \
	        else \
		  abspath=`cd $(MMROOT_VIDEO)/$(MM_GROUP_PATH)/$(GROUP_NAME)/$$incpath && pwd` ; \
		  case $$abspath in \
		    $(MMROOT_VIDEO)*) echo "USERINCLUDE  $$incpath" >> mmp-tmp ;; \
	            *) echo "   ERROR: include path $$incpath not in MMROOT/video folder"; \
	               exit 1 ;;   \
		  esac; \
		fi ; \
	done
	@echo "" >> mmp-tmp
	@echo "// Extra user include directories" >> mmp-tmp
	@for hd in $(SYMBIAN_USERINCLUDE_EXTRA); do \
	  case $$hd in \
	    /*) echo "   ERROR: invalid include path $$hd (shall not be absolute path)"; \
	        exit 1 ;;   \
	  esac; \
	  if [ ! -d $(MMROOT_VIDEO)/$(MM_GROUP_PATH)/$(GROUP_NAME)/$$hd ] ; then \
	    echo "   ERROR: invalid extra include directory $$hd" ; \
	    exit 3; \
	  else \
	    abspath=`cd $(MMROOT_VIDEO)/$(MM_GROUP_PATH)/$(GROUP_NAME)/$$hd && pwd` ; \
	    case $$abspath in \
	      $(MMROOT_VIDEO)*) echo "USERINCLUDE  $$hd" >> mmp-tmp ;; \
	      *) echo "   ERROR: include path $$hd not in MMROOT/video folder"; \
	         exit 1 ;;   \
	    esac; \
	  fi ; \
	done
	@echo "" >> mmp-tmp
	@for vp in $(VPATH); do \
		case $$vp in \
		  $(MMROOT_VIDEO)*) rel=`echo $(MM_GROUP_PATH) | sed -e 's/[^\/]*/../g'` ; \
	                      dir=`echo $$vp | sed -e 's/$(MYMMROOT_VIDEO)//g'`; \
	                      srcpath="$(REL_GROUP_NAME)/$$rel$$dir" ;; \
	          /*) echo "   ERROR: Invalid VPATH not in MMROOT/video folder"; \
	              exit 1 ;;   \
		  *) srcpath=$(REL_GROUP_PATH)/$$vp ;; \
		esac; \
		found=0 ; \
		for f in $(LIBOBJ); do \
			if [ -f $$vp/$$f.c ] ; then \
				if [ $$found -eq 0 ] ; \
	                          then echo "" >> mmp-tmp ; \
	                          echo "SOURCEPATH $$srcpath" >> mmp-tmp ; \
	                          found=1; \
	                        fi; \
				echo "SOURCE $$f.c" >> mmp-tmp ; \
			fi ;\
			if [ -f $$vp/$$f.cpp ] ; then \
				if [ $$found -eq 0 ] ; \
	                          then echo "" >> mmp-tmp ; \
	                          echo "SOURCEPATH $$srcpath" >> mmp-tmp ; \
	                          found=1; \
	                          fi; \
				echo "SOURCE $$f.cpp" >> mmp-tmp ; \
			fi ;\
		done; \
	done
	@echo "" >> mmp-tmp
	@for stlib in $(SYMBIAN_SYSTEMSTATICLIB); do \
		echo "STATICLIBRARY   $$stlib" >> mmp-tmp ; \
	done
	@for lib in $(SYMBIAN_SYSTEMLIB); do \
		echo "LIBRARY   $$lib" >> mmp-tmp ; \
	done
	@echo "" >> mmp-tmp
	@for line in $(SYMBIAN_EXTRA_MMP); do \
		echo "$$line" >> mmp-tmp ; \
	done
	@mv mmp-tmp $(GROUP_PATH)/$(GROUP_NAME)/$(SYMBIAN_PRJNAME)_$(SYMBIAN_VARIANT_NAME).mmp

symb-libmmp: symb-check 
	@echo "Generating Symbian static library mmp file for platform $(SYMBIAN_VARIANT_NAME)" 
	@rm -f mmp-tmp
	@echo $(HEADER_STRING_GENERATED) >> mmp-tmp
	@echo $(HEADER_STRING_LINE1) >> mmp-tmp
	@echo $(HEADER_STRING_LINE2) >> mmp-tmp
	@echo $(HEADER_STRING_LINE3) >> mmp-tmp
	@echo $(HEADER_STRING_LINE4) >> mmp-tmp
	@echo $(HEADER_STRING_LINE5) >> mmp-tmp
	@echo $(HEADER_STRING_LINE6) >> mmp-tmp
	@echo "" >> mmp-tmp
	@echo "TARGET     $(SYMBIAN_PRJNAME)_$(SYMBIAN_VARIANT_NAME).lib" >> mmp-tmp
	@echo "TARGETTYPE lib" >> mmp-tmp
	@echo "" >> mmp-tmp
	@for flag in $(SYMBIAN_EXTRA_MMP_FLAG); do \
		echo "$$flag" >> mmp-tmp ; \
	done
	@macrodef=`echo $(CPPFLAGS) | tr ' ' '\n' | sed -e '/^-[^D]/ d' -e 's/^-D//' | sort | uniq` ; \
	for m in $$macrodef; do \
		echo "MACRO $$m" >> mmp-tmp ; \
	done
	@echo "" >> mmp-tmp
	@echo "// These directories should be included by default" >> mmp-tmp
	@echo "OS_LAYER_SYSTEMINCLUDE_SYMBIAN" >> mmp-tmp
	@for inc in $(SYMBIAN_SYSTEMINCLUDE); do \
		echo "SYSTEMINCLUDE   $$inc" >> mmp-tmp ; \
	done
	@echo "" >> mmp-tmp
	@echo "// User include directories" >> mmp-tmp
	@includirs=`echo $(CPPFLAGS) | tr ' ' '\n' | sed -e '/^-[^I]/ d' -e 's/^-I//' | sort | uniq` ; \
	for inc in $$includirs; do \
		case $$inc in \
		  $(MMROOT_VIDEO)*) rel=`echo $(MM_GROUP_PATH) | sed -e 's/[^\/]*/../g'` ; \
	                      dir=`echo $$inc | sed -e 's/$(MYMMROOT_VIDEO)//g'`; \
	                      incpath="$(REL_GROUP_NAME)/$$rel$$dir" ;; \
	          /*) echo "   ERROR: Invalid Include Path $$inc not in MMROOT/video folder"; \
	              exit 1 ;;   \
		  *) incpath=$(REL_GROUP_PATH)/$$inc ;;    \
		esac; \
		if [ ! -d $(MMROOT_VIDEO)/$(MM_GROUP_PATH)/$(GROUP_NAME)/$$incpath ] ; then \
		  echo "   WARNING: unexisting include directory $$incpath" ; \
	        else \
		  abspath=`cd $(MMROOT_VIDEO)/$(MM_GROUP_PATH)/$(GROUP_NAME)/$$incpath && pwd` ; \
		  case $$abspath in \
		    $(MMROOT_VIDEO)*) echo "USERINCLUDE  $$incpath" >> mmp-tmp ;; \
	            *) echo "   ERROR: include path $$incpath not in MMROOT/video folder"; \
	               exit 1 ;;   \
		  esac; \
		fi ; \
	done
	@echo "" >> mmp-tmp
	@echo "// Extra user include directories" >> mmp-tmp
	@for hd in $(SYMBIAN_USERINCLUDE_EXTRA); do \
	  case $$hd in \
	    /*) echo "   ERROR: invalid include path $$hd (shall not be absolute path)"; \
	        exit 1 ;;   \
	  esac; \
	  if [ ! -d $(MMROOT_VIDEO)/$(MM_GROUP_PATH)/$(GROUP_NAME)/$$hd ] ; then \
	    echo "   ERROR: invalid extra include directory $$hd" ; \
	    exit 3; \
	  else \
	    abspath=`cd $(MMROOT_VIDEO)/$(MM_GROUP_PATH)/$(GROUP_NAME)/$$hd && pwd` ; \
	    case $$abspath in \
	      $(MMROOT_VIDEO)*) echo "USERINCLUDE  $$hd" >> mmp-tmp ;; \
	      *) echo "   ERROR: include path $$hd not in MMROOT/video folder"; \
	         exit 1 ;;   \
	    esac; \
	  fi ; \
	done
	@echo "" >> mmp-tmp
	@for vp in $(VPATH); do \
		case $$vp in \
		  $(MMROOT_VIDEO)*) rel=`echo $(MM_GROUP_PATH) | sed -e 's/[^\/]*/../g'` ; \
	                      dir=`echo $$vp | sed -e 's/$(MYMMROOT_VIDEO)//g'`; \
	                      srcpath="$(REL_GROUP_NAME)/$$rel$$dir" ;; \
	          /*) echo "   ERROR: Invalid VPATH not in MMROOT/video folder"; \
	              exit 1 ;;   \
		  *) srcpath=$(REL_GROUP_PATH)/$$vp ;; \
		esac; \
		found=0 ; \
		for f in $(LIBOBJ); do \
			if [ -f $$vp/$$f.c ] ; then \
				if [ $$found -eq 0 ] ; \
	                          then echo "" >> mmp-tmp ; \
	                          echo "SOURCEPATH $$srcpath" >> mmp-tmp ; \
	                          found=1; \
	                        fi; \
				echo "SOURCE $$f.c" >> mmp-tmp ; \
			fi ;\
			if [ -f $$vp/$$f.cpp ] ; then \
				if [ $$found -eq 0 ] ; \
	                          then echo "" >> mmp-tmp ; \
	                          echo "SOURCEPATH $$srcpath" >> mmp-tmp ; \
	                          found=1; \
	                          fi; \
				echo "SOURCE $$f.cpp" >> mmp-tmp ; \
			fi ;\
		done; \
	done
	@echo "" >> mmp-tmp
	@echo "Search for Symbian NMF project mmh file"
	@for comp in $(SYMBIAN_COMPONENTS); do \
	   compname=`echo $$comp | sed -e 's#\.#/#g'` ; \
	   compdirname=`echo $$comp | sed -e 's#\.#_#g'` ; \
	   comppath=`find $(GROUP_PATH) -wholename "*/$$compdirname/*.mmh" | sed -e 's/$(MYGROUP_PATH)/$(MYREL_GROUP_NAME)/g'` ; \
	   echo "#include \"$$comppath\"" >> mmp-tmp ; \
	done
	@echo "" >> mmp-tmp
	@for line in $(SYMBIAN_EXTRA_MMP); do \
		echo "$$line" >> mmp-tmp ; \
	done
	@mv mmp-tmp $(GROUP_PATH)/$(GROUP_NAME)/$(SYMBIAN_PRJNAME)_$(SYMBIAN_VARIANT_NAME).mmp


.PHONY: symb-check symb-libmmp symb-dllmmh symb-dllmmp symb-iby symb-bld-inf symb-prj
