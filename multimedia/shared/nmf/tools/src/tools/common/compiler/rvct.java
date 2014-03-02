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
import java.util.*;

import tools.common.compilerexception;
import tools.common.targetfactory;
import tools.common.exec.Args;
import tools.common.exec.exec;
import tools.common.exec.task;
import tools.common.exec.pipecolorlet.ErrorType;
import tools.common.targetfactory.LinkType;
import tools.common.configuration;

public class rvct extends preccompiler {
    public rvct() {
        elfExtension[LinkType.EXECUTABLE.ordinal()] = "axf";

        ASM = configuration.getEnvDefault("ASM", "armasm");
        CC = configuration.getEnvDefault("CC", "armcc");
        CXX = configuration.getEnvDefault("CXX", "armcpp");
        AR = configuration.getEnvDefault("AR", "armar");
        EXELD = configuration.getEnvDefault("EXELD", "armlink");
        
        errorType = ErrorType.AlaArm;
    }

    protected void compileC(Args argc, File src, File obj) throws compilerexception {
        if(targetfactory.linktype == LinkType.DLL)
        {
            argc.add("--no_rtti");
            argc.add("--no_exceptions");
            argc.add("--apcs=/fpic");
            //argc.add("--no_hide_all"); // Suppress warning "L6780W: STV_HIDDEN visibility removed from symbol"
            argc.add("--library_interface=aeabi_clib");
            argc.add("--dllimport_runtime");
        } 
        argc.add("-c");
        argc.add("-o");
        argc.add(configuration.toMakePath(obj));
        argc.add(configuration.toMakePath(src));
    }
    
    protected void compileCxx(Args argc, File src, File obj) throws compilerexception {
        compileC(argc, src, obj);
    }

    protected void compileAsm(Args argc, File src, File obj) throws compilerexception {
        argc.add("-o");
        argc.add(configuration.toMakePath(obj));
        argc.add(configuration.toMakePath(src));
    }

    public task link(File out, Set<String> files, ArrayList<task> predecessor) throws compilerexception {
        Args argc = new Args("LD", out.getPath());
        argc.add(EXELD);

        if(targetfactory.linktype == LinkType.DLL)
        {
            // This work with __declspec (dllimport) extern [but not "C"]
            //argc.add("--shared");
            //argc.add("--fpic");
            
            argc.add("--bpabi"); // Imply --reloc but in compatible way
            argc.add("--dll");  // Relocatable instead of executable
           // argc.add("--pltgot=none"); -> Not possible until address space between runtime & component to big, wait VM !!!
            argc.add("--no_scanlib"); // To not include lib
         //   argc.add("--symver_soname"); // ??
         //   argc.add("--no_debug");
            argc.add("--edit");
            argc.add("/home/fassino/steering");
            argc.add("--override_visibility"); // Allow with no_hide_all to hide symbol with __declspec(notshared
//            argc.add("--no_locals");
        } 
        else 
        {
            addList(LDFLAGS, argc);
            
            argc.add("--noremove");
        }

        // Include files
        for(String file : files) {
            argc.add(file);
        }
        addList(GROUPLDFLAGS, argc);

        argc.add("-o");
        argc.add(configuration.toMakePath(out));

        out.getParentFile().mkdirs();
        task link = exec.startArgc(argc, ErrorType.AlaArm, predecessor);
     
        return link;
    }

    public task library(File out, Set<String> files, ArrayList<task> predecessor) throws compilerexception {
        Args argc = new Args("LD", out.getPath());
        argc.add(EXELD);
        addList(LIBLDFLAGS, argc);
        argc.add("--partial");

        // Include files
        for(String file : files) {
            argc.add(file);
        }
        addList(GROUPLDFLAGS, argc);

        argc.add("-o");
        argc.add(configuration.toMakePath(out));

        out.getParentFile().mkdirs();
        task link = exec.startArgc(argc, ErrorType.AlaArm, predecessor);
     
        return link;
    }
    
    public task archive(File out, Set<String> files, ArrayList<task> predecessor) throws compilerexception {
        Args argc = new Args("AR", out.getPath());
        argc.add(AR);
        argc.add("-r");

        argc.add(configuration.toMakePath(out));

        // Include files
        for(String file : files) {
            argc.add(file);
        }

        out.getParentFile().mkdirs();
        task link = exec.startArgc(argc, ErrorType.AlaArm, predecessor);
     
        return link;
    }

}
