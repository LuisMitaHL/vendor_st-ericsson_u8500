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
/*
 * Generated ELF header component description
 */
package tools.conf.generator;

import java.io.*;
import java.util.*;

import tools.conf.ast.*;
import tools.conf.ast.astoption.OptionNames;
import tools.conf.graph.*;
import tools.conf.graph.binding.bc;
import tools.common.*;
import tools.common.compiler.*;
import tools.common.targetfactory.CodingStyle;
import tools.common.targetfactory.DomainNames;
import tools.common.exec.task;

public class maker extends visitor {
    public maker() {
        travelVirtual = false;
    }
    
    class parentinfo {
        ccompiler cc;
        LinkedHashSet<String> files;
        ArrayList<task> predecessors;
        HashSet<String> sourceToCompiles;
        parentinfo(ccompiler _cc) {
            cc = _cc;
            files = new LinkedHashSet<String>();
            predecessors = new ArrayList<task>();
            sourceToCompiles = new HashSet<String>();
        }
        parentinfo(ccompiler _cc, parentinfo other) {
            cc = _cc;
            files = other.files;
            predecessors = other.predecessors;
            sourceToCompiles = other.sourceToCompiles;
        }
        public boolean needCompile(astprimitive sourceToCompile) {
            if(! sourceToCompiles.contains(sourceToCompile.fullyname.intern())) {
                sourceToCompiles.add(sourceToCompile.fullyname.intern());
                return  true;
            }
            return false;
        }
    }

    protected void predefine(component instance, ccompiler cc) throws compilerexception  {
        if(instance instanceof bc) {
            // CFLAGS = (BC_CFLAGS == null)? local::CFLAGS : BC_CFLAGS
            String BC_CFLAGS = configuration.getEnvDefault("BC_CFLAGS", null);
            cc.CFLAGS.clearFlags();
            if(BC_CFLAGS != null) {
                cc.CFLAGS.setFlags(BC_CFLAGS);
            } else {
                cc.CFLAGS.setFlags(instance.getOption(targetfactory.TARGET, OptionNames.CFLAGS));
            }
        } else {
            // CPPFLAGS = parent::CPPFLAGS + local::CPPFLAGS
            cc.CPPFLAGS.setFlags(instance.getOption(targetfactory.TARGET, OptionNames.CPPFLAGS));

            // CPPFLAGS += -Iincdir
            String incdir = instance.getOption(targetfactory.TARGET, OptionNames.incdir);
            if(incdir != null) {
                StringTokenizer st = new StringTokenizer(incdir, ";");
                while (st.hasMoreTokens()) {
                    cc.CPPFLAGS.setFlagsSimple("-I" + instance.type.parsedFile.getParentFile().getPath() + File.separator + st.nextToken());
                }
            }
            
            // CFLAGS = (local::OVERWRITTEN_CFLAGS == null)?((parent.CFLAGS empty) ? local::CFLAGS : parent.CFLAGS): local::OVERWRITTEN_CFLAGS ;
            String OVERWRITTEN_CFLAGS = instance.getOption(targetfactory.TARGET, OptionNames.OVERWRITTEN_CFLAGS);
            if (OVERWRITTEN_CFLAGS == null)
            {
                if(cc.CFLAGS.isEmptyFlags())
                    cc.CFLAGS.setFlags(instance.getOption(targetfactory.TARGET, OptionNames.CFLAGS));
            }
            else
            {
                cc.CFLAGS.clearFlags();
                cc.CFLAGS.setFlags(OVERWRITTEN_CFLAGS);
            }

            // CFLAGS += local::FIXED_CFLAGS
            cc.CFLAGS.setFlags(instance.getOption(targetfactory.TARGET, OptionNames.FIXED_CFLAGS));
        }
        cc.CPPFLAGS.dumpFlags("CPPFLAGS");
        cc.CFLAGS.dumpFlags("CFLAGS");
        cc.CXXFLAGS.dumpFlags("CXXFLAGS");
        cc.LDFLAGS.dumpFlags("LDFLAGS");
        cc.LIBLDFLAGS.dumpFlags("LIBLDFLAGS");
        cc.GROUPLDFLAGS.dumpFlags("GROUPLDFLAGS");
    }

    
    protected void compileCommon(component instance, parentinfo top) throws compilerexception {      
        astcomponent component = instance.type;
        
        // Include and generate assembled file
        File outsrcwrp = getComponentTemporaryFile(component, component.smallname + "-nmf." + targetfactory.style.SourceExtension);
        File outobjwrp = getComponentBinaryFile(component, component.smallname + "-nmf." + ccompiler.OBJ);
        File outdepwrp = getComponentBinaryFile(component, component.smallname + "-nmf.d");
        if(top.cc.needRecompile(outobjwrp, outdepwrp)) {
            top.predecessors.add(top.cc.compile(outsrcwrp, outobjwrp));
            top.cc.createDependenciesFile(outsrcwrp, outdepwrp);
        }
        top.files.add(toMakePath(outobjwrp));
    }
    
    protected void compileElf(component instance, parentinfo top) throws compilerexception 
    {       
        /*
         * Generate the elf file
         */
        File outelf = getComponentFinalExt(instance.type, top.cc.elfExtension[targetfactory.linktype.ordinal()]);

        if(targetfactory.domain == DomainNames.MMDSP || targetfactory.domain == DomainNames.EECpp)
        {
            // compile asm header file if generated 
            File outhdr = getComponentTemporaryFile(instance.type, instance.type.smallname + "-hdr." + ccompiler.S);
            if(outhdr.exists()) 
            {
                File outobjhdr = getComponentBinaryFile(instance.type, instance.type.smallname + "-hdr." + ccompiler.OBJ);

                if(outobjhdr.lastModified() < outhdr.lastModified()) 
                {
                    top.predecessors.add(top.cc.compile(outhdr, outobjhdr));
                }
                top.files.add(toMakePath(outobjhdr));
            }
        } 
        
        // Create dependencies list and compile common part
        compileCommon(instance, top);

        if(top.predecessors.size() > 0 || ! outelf.exists()) 
        {
            switch (targetfactory.linktype) 
            {
            case EXECUTABLE:
            case DLL:
                top.cc.link(outelf, top.files, top.predecessors);
                break;
            case PARTIAL:
                top.cc.library(outelf, top.files, top.predecessors);
                break;
            case ARCHIVE:
                top.cc.archive(outelf, top.files, top.predecessors);
                break;
            }
       }
    }

    protected void travelPrimitive(primitive primitive, Object data) throws compilerexception {
        parentinfo parent = (parentinfo)data;

        astprimitive component = primitive.type;
        // Do not include twice same primitive source !
        if(parent.needCompile(component)) {
            ccompiler cc = compilerfactory.getCCompiler(parent.cc);

            predefine(primitive, cc);

            ArrayList<task> predecessors = new ArrayList<task>();
            LinkedHashSet<String> files = new LinkedHashSet<String>();

            // Include and generate assembled file
            if(targetfactory.style == CodingStyle.C) {
                File outsrcwrp = getComponentTemporaryFile(component, component.smallname + "-wrp.c");
                File outobjwrp = getComponentBinaryFile(component, component.smallname + "-wrp." + ccompiler.OBJ);
                File outdepwrp = getComponentBinaryFile(component, component.smallname + "-wrp.d");
                if(cc.needRecompile(outobjwrp, outdepwrp)) {
                    predecessors.add(cc.compile(outsrcwrp, outobjwrp));
                    cc.createDependenciesFile(outsrcwrp, outdepwrp);
                }
                files.add(toMakePath(outobjwrp));
            }

            // Compile component sources
            for (final astsource source : component.sources) 
            {
                String smallsource = source.file.getName().substring(0, source.file.getName().lastIndexOf('.'));
                File dep = getComponentBinaryFile(component, smallsource + ".d");
                File obj = getComponentBinaryFile(component, smallsource + "." + ccompiler.OBJ);
                File src = source.file;

                // Recompile if source older that most IDL/CDL file include in build
                // or if C dependencies changes
                if(cc.needRecompile(obj, dep)) {
                    String fileCFLAGS = source.CFLAGS;

                    // CPPFLAGS += -D__MYFILE__=\"`echo <abs-pathname> | sed -r "s,(.*)/multimedia/(.*),\2,g"`\"
                    final String removeBefore = "/multimedia/";
                    String filename = source.file.getAbsolutePath();
                    int idx = filename.indexOf(removeBefore);
                    
                    if(idx != -1)
                        filename = filename.substring(idx + removeBefore.length());
                    
                    fileCFLAGS = (fileCFLAGS != null ? fileCFLAGS + " " : "") +
                            "-D__MYFILE__=\"" + filename + "\"";

                    predecessors.add(cc.compile(src, obj, fileCFLAGS));
                    cc.createDependenciesFile(src, dep);
                }
                files.add(toMakePath(obj));
            }

            // Link component library
            // Since RVCT don't handle path name, create a unique file to do that !!!!
            File outlibfile = getComponentTemporaryUniqueFile(component, cc.LIB);
            if((predecessors.size() > 0) || ! outlibfile.exists()) 
            {
                task link = cc.library(outlibfile, files, predecessors);
                parent.predecessors.add(link);
            }
            parent.files.add(toMakePath(outlibfile));
        }
    }

    protected void travelComposite(composite composite, Object data) throws compilerexception {
        parentinfo parent = (parentinfo)data;
        
        ccompiler cc = compilerfactory.getCCompiler(parent.cc);
        
        predefine(composite, cc);
        parentinfo newdata = new parentinfo(cc, parent);
        super.travelComposite(composite, newdata);

        /* 
         * TODO This code could be used when module filename for libraries
         * could understand directory. (Actually it is only generated, not used).

         File outlibfile = getComponentDstBin(component, component.smallname + "." + LIB);
         ArrayList<String> libraries = new ArrayList<String>();
         for (Iterator<component> i = composite.getFcSubComponents(); i.hasNext();) {
         component scc = i.next();
         astcomponent sc = scc.type;
         String lib = toMakePath(getComponentDstBin(sc, sc.smallname + "." + LIB)).intern();
         if(! libraries.contains(lib)) {
         libraries.add(lib);
        // Add library
         }
         }
         $(AR) +s l=$@ $^
         */
        
    }

    public void browseComponent(component component) throws compilerexception {
        ccompiler cc = compilerfactory.getCCompiler(component);

        // LDFLAGS += local::LDFLAGS
        cc.LDFLAGS.setFlags(component.getOption(targetfactory.TARGET, OptionNames.LDFLAGS));

        // LIBLDFLAGS += local::LIBLDFLAGS
        cc.LIBLDFLAGS.setFlags(component.getOption(targetfactory.TARGET, OptionNames.LIBLDFLAGS));

        // GROUPLDFLAGS += local::GROUPLDFLAGS
        cc.GROUPLDFLAGS.setFlags(component.getOption(targetfactory.TARGET, OptionNames.GROUPLDFLAGS));

        parentinfo top = new parentinfo(cc);
        super.browseComponent(component, top);

        compileElf(component, top);
    }

}
