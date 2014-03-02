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
import tools.common.exec.Args;
import tools.common.exec.exec;
import tools.common.exec.task;
import tools.common.exec.pipecolorlet.ErrorType;
import tools.common.configuration;

public class gcc extends preccompiler {
	public gcc() {
        ASM = configuration.getEnvDefault("ASM", "as");
        CC = configuration.getEnvDefault("CC", "gcc");
        CXX = configuration.getEnvDefault("CXX", "g++");
        AR = configuration.getEnvDefault("AR", "ar");
        EXELD = configuration.getEnvDefault("EXELD", "g++");
        LIBLD = configuration.getEnvDefault("LIBLD", "ld");
	}

    protected void compileC(Args argc, File src, File obj) throws compilerexception {
        argc.add("-c");
        argc.add("-o");
        argc.add(configuration.toMakePath(obj));
        argc.add(configuration.toMakePath(src));
    }
    
    protected void compileCxx(Args argc, File src, File obj) throws compilerexception {
        argc.add("-c");
        argc.add("-o");
        argc.add(configuration.toMakePath(obj));
        argc.add(configuration.toMakePath(src));
    }

    protected void compileAsm(Args argc, File src, File obj) throws compilerexception {
        argc.add("-o");
        argc.add(configuration.toMakePath(obj));
        argc.add(configuration.toMakePath(src));
    }

	public task link(File out, Set<String> files, ArrayList<task> predecessor) throws compilerexception {
        Args argc = new Args("LD", out.getPath());
		argc.add(EXELD);
		addList(LDFLAGS, argc);

		// Include files
        for(String file : files) {
			argc.add(file);
		}
        addList(GROUPLDFLAGS, argc);

		argc.add("-o");
		argc.add(configuration.toMakePath(out));

        out.getParentFile().mkdirs();
        task link = exec.startArgc(argc, ErrorType.AlaGnu, predecessor);
     
        return link;
	}

	public task library(File out, Set<String> files, ArrayList<task> predecessor) throws compilerexception {
        Args argc = new Args("LD", out.getPath());
		argc.add(LIBLD);
        argc.add("-r");
        addList(LIBLDFLAGS, argc);

		// Include files
        for(String file : files) {
			argc.add(file);
		}

        argc.add("-o");
        argc.add(configuration.toMakePath(out));

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
