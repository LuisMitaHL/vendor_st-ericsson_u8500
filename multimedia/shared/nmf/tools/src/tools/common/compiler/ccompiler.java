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
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Iterator;
import java.util.List;
import java.util.Set;
import java.util.StringTokenizer;

import tools.common.compilerexception;
import tools.common.configuration;
import tools.common.configuration.Debug;
import tools.common.exec.Args;
import tools.common.exec.task;
import tools.conf.graph.component;

public abstract class ccompiler {
    public ccompiler() {
    }

    /*
     * Extension
     */
    // Elf exention indexes according LinkType
    public String elfExtension[] = new String[]
                                              {
            "bin",      // EXECUTABLE
            "lib",      // PARTIAL
            "a",        // ARCHIVE
            "so"        // DLL   
                                              };
    public String LIB = "lib";                      // Unit compilation
    
    public static String OBJ = "o";                        // Primitive 
    public static String S = "s", C = "c", CPP = "cpp";

    /**
     * Flags enumeration
     */
    public class TFlag {
        public ArrayList<String> data = new ArrayList<String>();
   
        public void setFlags(TFlag otherflags) {
            setFlags(otherflags.data);
        }

        public void setFlags(ArrayList<String> otherflags) {
            for(Iterator<String> i = otherflags.iterator(); i.hasNext(); ) {
                String mystr = i.next();
                if(!data.contains(mystr))
                    data.add(mystr);
            }
        }

        public void setFlags(String flagsstr) {
            if(flagsstr != null && "".equals(flagsstr) == false) {
                StringTokenizer st = new StringTokenizer(flagsstr);
                while (st.hasMoreTokens()) {
                    String tok = st.nextToken();
                    if(!data.contains(tok))
                        data.add(tok);
                }
            }
        }

        public void setFlagsSimple(String flagsstr) {
            data.add(flagsstr);
        }

        public boolean isEmptyFlags() {
            return data.isEmpty();
        }

        public void clearFlags() {
            data.clear();
        }

        public void dumpFlags(String name) {
            if(configuration.isDebugged(Debug.option)) {
                configuration.debug(Debug.option, name + "=" + Arrays.asList((Object[])data.toArray()));
            }
        }
    }

    public TFlag LDFLAGS = new TFlag();
    public TFlag LIBLDFLAGS = new TFlag();
    public TFlag GROUPLDFLAGS = new TFlag();
    public TFlag CPPFLAGS = new TFlag();
    public TFlag CFLAGS = new TFlag();
    public TFlag CXXFLAGS = new TFlag();
    public TFlag ASMFLAGS = new TFlag();

    public void setDefaultFlags(component component) throws compilerexception
    {
    }
    public void setDefaultFlags(ccompiler parent)
    {
    }

    /*
     * Add arguments on cmld line.
     */
    public static void addAsInclude(List<File> other, Args argc) {
        for(Iterator<File> i = other.iterator(); i.hasNext(); )
            argc.add("-I" + i.next().getPath());
    }
    public static void addAsInclude(File file, Args argc) {
        argc.add("-I" + file.getPath());
    }
    public static void addList(TFlag other, Args argc) {
        for(Iterator<String> i = other.data.iterator(); i.hasNext(); )
            argc.add(i.next());
    }

    /*
     * Compilation
     */
    public task compile(File src, File obj) throws compilerexception 
    {
        return compile(src, obj, null);
    }
    
    /*
     * Abstract methods for compilation
     */
    public abstract task compile(File src, File obj, String speccflags) throws compilerexception ;
    public abstract boolean needRecompile(File obj, File dep);
    public abstract void createDependenciesFile(File src, File dep) throws compilerexception;

    public abstract task link(File out, Set<String> files, ArrayList<task> predecessor) throws compilerexception;
    public abstract task library(File out, Set<String> files, ArrayList<task> predecessor) throws compilerexception;
    public abstract task archive(File out, Set<String> files, ArrayList<task> predecessor) throws compilerexception;

    /*
     * Abstract methods for type checking
     */
    public boolean isAvailabletype(String type) {
        return true;
    }
    public int sizeOfWord() {
        return 32;
    }
    public void declareStaticInterfaceMethod(PrintStream out, String signature) {
        out.println("extern " + signature + ";");
    }
    public void declareInterface(PrintStream out, String type, String itfname) {
        out.println("extern " + type + " " + itfname + ";");
    }

}
