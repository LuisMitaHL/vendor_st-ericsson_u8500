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
import java.util.ArrayList;
import java.util.Set;

import tools.common.compilerexception;
import tools.common.configuration;
import tools.common.targetfactory;
import tools.common.targetfactory.CodingStyle;
import tools.common.exec.pipecolorlet.ErrorType;
import tools.common.exec.Args;
import tools.common.exec.exec;
import tools.common.exec.task;
import tools.cpp.cppparser;

public abstract class preccompiler extends ccompiler {
    public preccompiler() {
    }

    /*
     * Compiler
     */
    protected String ASM = "????";
    protected String CC = "????";
    protected String CXX = "????";
    protected String AR = "????";
    protected String EXELD = "????";
    protected String LIBLD = "????";

    protected ErrorType errorType = ErrorType.AlaGnu;

   /*
     * Abstract methods for compilation
     */
    protected abstract void compileC(Args argc, File src, File obj) throws compilerexception;
    protected abstract void compileAsm(Args argc, File src, File obj) throws compilerexception;
    protected abstract void compileCxx(Args argc, File src, File obj) throws compilerexception;

    public task compile(File src, File obj, String speccflags) throws compilerexception 
    {
        Args argc = new Args("CC", obj.getPath());
        obj.getParentFile().mkdirs();
        
        // Set compiler and compile option
        if(src.getAbsolutePath().endsWith("." + CPP) ||
                (src.getAbsolutePath().endsWith("." + C) && targetfactory.style == CodingStyle.CPP)) 
        {
            argc.add(CXX);
            addList(CXXFLAGS, argc);
            if(speccflags != null)
                argc.add(speccflags);
            addList(CPPFLAGS, argc);
            addAsInclude(configuration.sharedir, argc);
            addAsInclude(configuration.srcdir, argc);
            addAsInclude(configuration.tmpsrcdir, argc);
            addAsInclude(configuration.dstdir, argc);

            compileCxx(argc, src, obj);
        } else if(src.getAbsolutePath().endsWith("." + ASM)) 
        {
            argc.add(ASM);
            addList(ASMFLAGS, argc);
        
            compileAsm(argc, src, obj);
        } else  
        {
            argc.add(CC);
            addList(CFLAGS, argc);
            if(speccflags != null)
                argc.add(speccflags);
            addList(CPPFLAGS, argc);
            addAsInclude(configuration.sharedir, argc);
            addAsInclude(configuration.srcdir, argc);
            addAsInclude(configuration.tmpsrcdir, argc);
        
            compileC(argc, src, obj);
        }
        
        return exec.startArgc(argc, errorType);
    }
    
    public boolean needRecompile(File obj, File dep) {
        return cppparser.needRecompile(obj, dep);
    }
    public void createDependenciesFile(File src, File dep) throws compilerexception {
        cppparser.createDependenciesFile(src, dep);
    }

    public task archive(File out, Set<String> files, ArrayList<task> predecessor) throws compilerexception {
        throw new compilerexception("Archive not supported on this target!!!");
    }
}
