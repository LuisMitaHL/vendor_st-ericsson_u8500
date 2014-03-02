/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 *
 * This code is part of the NMF Toolset.
 *
 * The NMF Toolset is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * The NMF Toolset is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with the NMF Toolset. If not, see <http://www.gnu.org/licenses/>.
 *
 */
package tools.common.compiler;

import java.io.File;
import java.io.PrintStream;
import java.util.*;

import tools.common.compilerexception;
import tools.common.targetfactory;
import tools.common.targetfactory.LinkType;
import tools.common.targetfactory.TargetNames;
import tools.common.exec.Args;
import tools.common.exec.exec;
import tools.common.exec.task;
import tools.common.exec.pipecolorlet.ErrorType;
import tools.common.configuration;
import tools.conf.graph.component;
import tools.conf.graph.component.Bases;
import tools.conf.graph.computer.stack;

public class ac extends preccompiler {

    protected TFlag MODEFLAGS = new TFlag();
    
    public ac() {
        elfExtension[LinkType.DLL.ordinal()] = "elf4nmf";
        elfExtension[LinkType.EXECUTABLE.ordinal()] = "elf4nmf";
        LIB = "a";
        S = "asm";

        CC = configuration.getEnvDefault("MMDSPAC", "ac");
        ASM = configuration.getEnvDefault("MMDSPAC", "ac");
        AR = configuration.getEnvDefault("MMDSPAR", "mmdsp-ar");
        EXELD = configuration.getEnvDefault("MMDSPLD", "mmdsp-ld");
        LIBLD = configuration.getEnvDefault("MMDSPLD", "mmdsp-ld");
    }

    public void setDefaultFlags(component component) throws compilerexception
    {
        if(targetfactory.TARGET == TargetNames.dsp16) {
            MODEFLAGS.setFlagsSimple("-mode16");
        }  

        if(targetfactory.linktype == LinkType.DLL) 
        {
            // Set compilation mode
            if(component.type.singleton)
                MODEFLAGS.setFlagsSimple("-nmf-static");
            else
                MODEFLAGS.setFlagsSimple("-nmf");

            // Set bases for component
            int no = 0;
            for (Bases base : Bases.values())  {
                if(component.isBaseSetted(base))
                    MODEFLAGS.setFlagsSimple(base  + "=" + (no++));
            }
        } 
        else 
        {
            // Set flags size
            stack st = new stack();
            st.browseComponent(component, null);
            if(st.minstack != 0)
                MODEFLAGS.setFlagsSimple("stack=" + st.minstack);

            // Set compilation mode
            if(targetfactory.staticPicCompilation)
                MODEFLAGS.setFlagsSimple("-pic");
        }
    }
    
    public void setDefaultFlags(ccompiler parent)
    {
        if(parent instanceof ac)
            MODEFLAGS.setFlags(((ac)parent).MODEFLAGS);
    }
    
    protected void compileC(Args argc, File src, File obj) throws compilerexception {
        addList(MODEFLAGS, argc);
        argc.add("-link");
        //argc.add("+k");
        argc.add("-o");
        argc.add(configuration.toMakePath(obj));
        argc.add(configuration.toMakePath(src));
    }
    
    protected void compileCxx(Args argc, File src, File obj) throws compilerexception {
        throw new compilerexception("C++ not supported on MMDSP!!!");
    }

    protected void compileAsm(Args argc, File src, File obj) throws compilerexception {
        argc.add("-link");
        argc.add("-o");
        argc.add(configuration.toMakePath(obj));
        argc.add(configuration.toMakePath(src));
    }

    public task link(File outelf4nmf, Set<String> files, ArrayList<task> predecessor) throws compilerexception {
        File outelf = new File(outelf4nmf.getPath().replaceFirst(".elf4nmf", ".elf"));

        Args argc = new Args("LD", outelf.getPath());
        argc.add(CC);
        addList(MODEFLAGS, argc);
        addList(LDFLAGS, argc);
        argc.add("-o");
        argc.add(configuration.toMakePath(outelf));
        argc.add("-pla");

        // Include files
        for(String file : files) {
            if(! file.endsWith("." + LIB))
                argc.add(file);
        }

        argc.add("link=--start-group");
        for(String file : files) {
            if(file.endsWith("." + LIB))
                argc.add("l=" + file);
        }
        addList(GROUPLDFLAGS, argc);
        argc.add("link=--end-group");
        outelf4nmf.getParentFile().mkdirs();
        task _task = exec.startArgc(argc, ErrorType.AlaGnu, predecessor);
        
        // In case we don't use -nmf -nmf-static option in MODEFLAGS we have to call the postpass 
        if (targetfactory.linktype == LinkType.EXECUTABLE)
        {
            argc = new Args("PP4NMF", outelf.getPath());
            argc.add("mmdsp-pp4nmf");
            argc.add(configuration.toMakePath(outelf));
            _task = exec.startArgc(argc, ErrorType.AlaGnu, _task);
        }

        /*
        argc = new ArrayList<String>();
        argc.add("/home/fassino/mmdsp-pp4nmf");
        argc.add(configuration.toMakePath(outelf));
        _task = exec.startArgc(Debug.always, argc, ErrorType.AlaGnu, _task);
         */
        
        argc = new Args("STRIP", outelf.getPath());
        argc.add("mmdsp-objcopy");
        argc.add("--no-adjust-warnings");
        argc.add("-I");
        argc.add("elf64-mmdsp");
        argc.add("-O");
        argc.add("elf64-mmdsp");
        argc.add("--strip-debug");
        argc.add("--strip-unneeded");
        argc.add("--remove-section");
        argc.add(".note");
        argc.add("--remove-section");
        argc.add("header_segment");
        argc.add("--remove-section");
        argc.add(".meminfo"); 
        argc.add(configuration.toMakePath(outelf4nmf));
        argc.add(configuration.toMakePath(outelf4nmf));
        _task = exec.startArgc(argc, ErrorType.AlaGnu, _task);

        return _task;
    }

    public task library(File out, Set<String> files, ArrayList<task> predecessor) throws compilerexception {
        Args argc = new Args("AR", out.getPath());
        argc.add(AR);
        argc.add("rc");
        argc.add(configuration.toMakePath(out));

        // Include files
        for(String file : files) {
            argc.add(file);
        }

        out.getParentFile().mkdirs();
        task link = exec.startArgc(argc, ErrorType.AlaGnu, predecessor);
     
        return link;
        /*
    argc = new ArrayList<String>();
    argc.add("ranlib");
    argc.add(configuration.toMakePath(out));

    exec.runArgc(Debug.always, argc);
         */
    }

    public boolean isAvailabletype(String type) {
        if("t_uint64".equals(type) || "t_sint64".equals(type))
            return false;
        if(targetfactory.TARGET == TargetNames.dsp16) {
            if("t_uint56".equals(type) || "t_sint56".equals(type))
                return false;
            if("t_uint48".equals(type) || "t_sint48".equals(type))
                return false;
        }
        return true;
    }
    public int sizeOfWord() {
        return 24;
    }
    
    public void declareStaticInterfaceMethod(PrintStream out, String signature) {
        out.println("#pragma absjmp");
        out.println("extern " + signature + ";");
    }
    public void declareInterface(PrintStream out, String type, String itfname) {
        out.println("extern " + type + " ");
        out.println("#pragma interface");
        out.println("  " + itfname + ";");
    }

}
