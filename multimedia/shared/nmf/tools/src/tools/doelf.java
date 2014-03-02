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
package tools;

import java.io.*;
import java.util.ArrayList;
import java.util.HashSet;

import tools.common.compilerexception;
import tools.common.configuration;
import tools.common.util;
import tools.common.targetfactory.CodingStyle;
import tools.common.targetfactory;
import tools.common.error;
import tools.common.exec.*;
import tools.conf.generator.dotter;
import tools.conf.generator.maker;
import tools.conf.generator.wrapper;
import tools.common.targetfactory.LinkType;
import tools.conf.ast.*;
import tools.conf.graph.*;
import tools.conf.graph.computer.lifecycle;
import tools.conf.graph.graphfactory.factory;
import tools.generator.generatorfactory;
import tools.generator.api.MetaDataGeneratorInterface;
import tools.idl.idlcompiler;
import tools.idl.ast.Interface;

public class doelf extends configuration {	

    class itfgenerator extends visitor 
    {
        HashSet<Interface> generated = new HashSet<Interface>();
        
        protected void generate(String type, boolean inFinalDirectory)
        {
            Interface itf = idlcompiler.getPreviouslyCompile(type);
            if(! generated.contains(itf)) 
            {
                generatorfactory.generateInterfaceDescriptor(itf, inFinalDirectory, false);
                generated.add(itf);
            }
        }
        
        protected void travel(component component, boolean inFinalDirectory)
        {
            // Generate provided interface descriptor
            for(astprovide provide : component.getProvides()) 
                generate(provide.type, inFinalDirectory);

            // Generate Required interface descriptor
            for (astrequire require : component.getRequires()) 
                generate(require.type, inFinalDirectory);
        }
        
        protected void travelRawPrimitive(primitiveraw primitive, Object data) 
        {
            travel(primitive, false);
        }
        
        public void browseComponent(component component, Object topData) throws compilerexception 
        {
            if(targetfactory.style == CodingStyle.CPP) 
                travel(component, true);

            super.browseComponent(component, topData);
        }

    }


    protected void Compile(component instance, File mostRecentFile) throws compilerexception, IOException 
    {
    
        
        lifecycle lccstate = new lifecycle(); 
        lccstate.browseComponent(instance);
        if(!(instance instanceof primitive) && targetfactory.linktype.uniqueLCCMethodName) {
            lccstate.needConstuctor = true;
            lccstate.needStarter = true;
            lccstate.needStopper = true;
            lccstate.needDestructor = true;
        }
            
        
        // Generate interface declarer
        (new itfgenerator()).browseComponent(instance);
        
        File dstfile = getComponentTemporaryFile(instance.type, instance.type.smallname + "-nmf." + targetfactory.style.SourceExtension);
        
        /*
        * Regenerate every glue file if one file description has been modified.
        * This is required since optimization can impact sub-composition.
        */
        debug(Debug.dep, dstfile.lastModified() + " < " + mostRecentFile.lastModified());
        if(dstfile.lastModified() < mostRecentFile.lastModified()) {
            debug(Debug.dep, "Regenerate GLUE " + dstfile + " (" + mostRecentFile + " changed)");

            /*
             * Generate sub-component wrapper files 
             * (stand-alone generation)
             */  
            configuration.debug(Debug.step, "Generate WRAP for '" + instance.type.fullyname + "'");
            wrapper wrp = new wrapper();
            wrp.browseComponent(instance);
            
            /*
             * Generate header description file (elf segment)
             * (global generation, if we are call do it)
             */
            MetaDataGeneratorInterface metatdatagen = targetfactory.getMetaDataGenerator();
            if(metatdatagen != null)
            {
                configuration.debug(Debug.step, "Generate META DATA for '" + instance.type.fullyname + "'");
                metatdatagen.Generate(instance, lccstate);
            }

            configuration.debug(Debug.step, "Generate ASSEMBLE for '" + instance.type.fullyname + "'");
            targetfactory.getAssembler().generate(instance, dstfile, lccstate);
            
            (new dotter()).Generate(instance, "full.");

            /*
             * Generate link command file
             */
            /*
            PrintStream outlks = configuration.needRegeneration(
                    instance.getMostRecentFile(), 
                    getComponentTemporaryFile(instance.type, instance.type.smallname + ".lks"));
            if(outlks != null) {
                for (Iterator<astrequire> j = instance.getRequires(); j.hasNext();) {
                    astrequire require = j.next();
                    Interface itf = idlcompiler.getPreviouslyCompile(require.type);
                    if(require.isStatic) {
                        for(int k = 0; k < itf.methods.size(); k++) {
                            Method md = (Method)itf.methods.get(k);
                            outlks.println("--defsym _" + md.name + "=0x0");
                        }
                    }
                }
            }
            */
        }
            
        /*
         * Compile components composition
         * (stand-alone generation)
         */  
        (new maker()).browseComponent(instance);

    }

    protected void Compile(String compositionname) throws compilerexception, IOException {
        debug(Debug.step, "Generate " + compositionname + " ELF component");

        startContext(longpath ? compositionname : util.getLastName(compositionname));

        generatorfactory.configure();

       // Instantiate graph
        factory factory = new factory();
        component instance = factory.instanciate(compositionname, true);

        // Sanity check
        if(targetfactory.staticPicCompilation && targetfactory.linktype != LinkType.EXECUTABLE)
            throw new compilerexception(instance.declaration, error.PIC_FOR_DYNAMIC_COMPONENT);
        
        // Compile them
        Compile(instance, factory.mostRecentFile);

        stopContext();
    }

    public static void main(String args[]) {
        doelf de = new doelf();

        try {
            ArrayList<String> remain = parseOption(args,
                    "doelf", "<component(s)>", 
                    "Compile binary <component(s)>",
                    new option[]{
                        new option("--executable", "",          "Generate executable", validity.Voptional),
                        new option("--dll", "",                 "Generate dynamic link library", validity.Voptional),
                        new option("--partial", "",             "Generate partial linked component", validity.Voptional),
                        new option("--archive", "",             "Generate archive component", validity.Voptional),
                        new option("--pic", "",                 "Generate PIC static executable component", validity.Voptional)});
            for(final String option : remain)
            {
                if("--partial".equals(option)) {
                    targetfactory.linktype = LinkType.PARTIAL;
                } else if("--archive".equals(option)) {
                    targetfactory.linktype = LinkType.ARCHIVE;
                } else if("--dll".equals(option)) {
                    targetfactory.linktype = LinkType.DLL;
                } else if("--executable".equals(option)) {
                    targetfactory.linktype = LinkType.EXECUTABLE;
                } else if("--pic".equals(option)) {
                    // PIC is special and also mean executable
                    targetfactory.linktype = LinkType.EXECUTABLE;
                    targetfactory.staticPicCompilation = true;
                } else {
                    targetfactory.setTarget();
                    de.Compile(option);
                }
            }
            
            exec.waitProcesses();
        } catch(compilerexception e) {
            error(e);
            for(int x=0; x < e.getStackTrace().length; x++) {  
                debug(Debug.error, "     " + e.getStackTrace()[x].toString()); 
            } 
            System.exit(e.exitNumber);
        } catch (IOException e) {
            error(e.getMessage());
            System.exit(255);
        } catch (InterruptedException e) {
            error(e.getMessage());
            System.exit(255);
        } catch (Exception e) {
            for(int x=0; x < e.getStackTrace().length; x++) {  
                error(e.getStackTrace()[x].toString()); 
            } 
            error(e.getMessage());
            System.exit(255);
        }
    }
    
}
