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

import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.PrintStream;
import java.util.ArrayList;
import java.util.Iterator;

import tools.common.*;
import tools.common.targetfactory.CodingStyle;
import tools.common.targetfactory.TargetNames;
import tools.common.exec.*;
import tools.conf.graph.graphfactory.factory;
import tools.generator.generatorfactory;
import tools.generator.api.EventGeneratorInterface;
import tools.generator.api.SkeletonGeneratorInterface;
import tools.generator.api.StubGeneratorInterface;
import tools.idl.ast.Interface;

class dobinding extends doelf {
    protected enum BCType {unknown, host2mpc, mpc2host, user2mpc, mpc2user, mpc2mpc, asynchronous, async, trace, itf};

    protected void CompileItf(
            BCType bctype, 
            ArrayList<Interface> interfaceNames) throws compilerexception, IOException {
        if(bctype == BCType.itf) 
        { 
            configuration.startContext();

            for(Iterator<Interface> i = interfaceNames.iterator(); i.hasNext(); ) {
                Interface itf = i.next();

                debug(Debug.step, "Generate " + itf.name + " Interface descriptor");
                generatorfactory.generateInterfaceDescriptor(itf, true, false);
            }

            configuration.stopContext();
        }
    }

    protected void CompileBC(
            BCType bctype, 
            ArrayList<Interface> interfaceNames) throws compilerexception, IOException, InterruptedException {
        for(Iterator<Interface> i = interfaceNames.iterator(); i.hasNext(); ) {
            Interface itf = i.next();

            // Generate template (sources)
            factory gf = new factory();
            if(bctype == BCType.asynchronous || bctype == BCType.async) 
            { 
                debug(Debug.step, "Generate " + itf.name + " ELF asynchronous binding component");
                configuration.startContext("_ev." + itf.name_);
                EventGeneratorInterface gen = targetfactory.getEvent();
                gen.setItf(itf);
                Compile(gf.wrapPrimitive(gen.registerAst(), "_ev." + itf.name), itf.lastModifiedFile);
                configuration.stopContext();
            } 
            else if(bctype == BCType.host2mpc || bctype == BCType.user2mpc) 
            {  
                debug(Debug.step, "Generate " + itf.name + " ELF user2mpc binding component");
                configuration.startContext("_sk." + itf.name_);
                SkeletonGeneratorInterface gensk = targetfactory.getSkeletonGenerator();
                gensk.setItf(itf);
                Compile(gf.wrapPrimitive(gensk.registerAst(), "_sk." + itf.name), itf.lastModifiedFile);
                configuration.stopContext();
            }
            else if(bctype == BCType.mpc2host || bctype == BCType.mpc2user)
            {  
                debug(Debug.step, "Generate " + itf.name + " ELF mpc2user binding component");
                configuration.startContext("_st." + itf.name_);
                StubGeneratorInterface genst = targetfactory.getStubGenerator();
                genst.setItf(itf);
                Compile(gf.wrapPrimitive(genst.registerAst(), "_st." + itf.name), itf.lastModifiedFile);
                configuration.stopContext();
            } 
            else if(bctype == BCType.mpc2mpc) 
            {  
                debug(Debug.step, "Generate " + itf.name + " ELF mpc2mpc binding component");
                configuration.startContext("_sk." + itf.name_);
                SkeletonGeneratorInterface gensk = targetfactory.getSkeletonGenerator();
                gensk.setItf(itf);
                Compile(gf.wrapPrimitive(gensk.registerAst(), "_sk." + itf.name), itf.lastModifiedFile);
                configuration.startContext("_st." + itf.name_);
                StubGeneratorInterface genst = targetfactory.getStubGenerator();
                genst.setItf(itf);
                Compile(gf.wrapPrimitive(genst.registerAst(), "_st." + itf.name), itf.lastModifiedFile);
                configuration.stopContext();
            } 
            else if(bctype == BCType.trace) 
            { 
                debug(Debug.step, "Generate " + itf.name + " ELF trace binding component");
                configuration.startContext("_tr." + itf.name_);
                Compile(gf.instanciate(new tools.generator.c.MMDSP.MMDSPTraceGenerator(itf).getAst()), itf.lastModifiedFile);
                configuration.stopContext();
           }
        }
        exec.waitProcesses();
    }
    
    protected void CompileHost(
            BCType bctype, 
            ArrayList<Interface> interfaceNames, 
            String prefixName) throws compilerexception, FileNotFoundException {
        // Check if we need host code
        String skelorstub;
        if(bctype == BCType.host2mpc || bctype == BCType.user2mpc)
            skelorstub = "stub";
        else if(bctype == BCType.mpc2host || bctype == BCType.mpc2user)
            skelorstub = "skel";
        else
            return;

        // Sanity check
        if(prefixName == null) {
            throw new compilerexception(error.BC_PREFIX_NOT_SET);
        }

        // Create file
        File outputFileName = getFinalFile("host" + File.separator + prefixName + "-" + bctype + "-" + skelorstub + "s." + targetfactory.style.SourceExtension);

        // Check that if dependencies ask for regeneration
        File lastModifiedFile = null;
        for(Iterator<Interface> i = interfaceNames.iterator(); i.hasNext(); ) {
            Interface itf = i.next();
      
            // Generate header for ARM
            generatorfactory.generateInterfaceDescriptor(itf, true, true);
           
            // Compute older file
            if(lastModifiedFile == null || 
                    itf.lastModifiedFile.lastModified() > lastModifiedFile.lastModified())
                lastModifiedFile = itf.lastModifiedFile;
       }

        // Eventual regeneration
        PrintStream out = configuration.needRegeneration(lastModifiedFile, outputFileName);
        if(out != null) {
            boolean firsttime = true;
            
            for(Iterator<Interface> i = interfaceNames.iterator(); i.hasNext(); ) {
                Interface itf = i.next();
                if(bctype == BCType.host2mpc || bctype == BCType.user2mpc) {
                    // Generate host stub
                    if(bctype == BCType.host2mpc) {
                        tools.generator.legacy.c.kernel.stub gen = new tools.generator.legacy.c.kernel.stub(itf, firsttime);
                        gen.Generate(out);
                    } else {
                        generatorfactory.generateStubOnCM(itf, firsttime, out);
                    }
                } else {
                    // Generate host skeleton
                    if(bctype == BCType.mpc2host) {
                        tools.generator.legacy.c.kernel.skel gen = new tools.generator.legacy.c.kernel.skel(itf, firsttime);
                        gen.Generate(out);
                    } else {
                        generatorfactory.generateSkeletonOnCM(itf, firsttime, out);
                    }
                }

                firsttime = false;
            }

            out.println("static t_nmf_" + skelorstub + "_function " + prefixName + "_" + skelorstub + "_functions[] = {");
            for(Iterator<Interface> i = interfaceNames.iterator(); i.hasNext(); ) {
                Interface itf = i.next();
                if(bctype == BCType.host2mpc || bctype == BCType.user2mpc) {
                    out.println("  {\"" + itf.name + "\", &INIT" + skelorstub + "_" + itf.name_ + "},");
                } else {
                    out.println("  {\"" + itf.name + "\", JT_" + itf.name_ + "},");
                }
            }
            out.println("};");
            out.println("");
            out.println("/*const */t_nmf_" + skelorstub + "_register " + prefixName + "_" + skelorstub + "_register = {");
            out.println("    NMF_BC_IN_" + targetfactory.style +",");
            out.println("    sizeof(" + prefixName + "_" + skelorstub + "_functions)/sizeof(t_nmf_" + skelorstub + "_function),");
            out.println("    " + prefixName + "_" + skelorstub + "_functions");
            out.println("};");
            out.close();
        }

    }

    public static void main(String args[]) {
        BCType bctype = BCType.unknown;
        String prefixName = null;
        ArrayList<Interface> interfaceNames = new ArrayList<Interface>();
        
        try {
            ArrayList<String> remain = parseOption(args, 
                    "dobinding", "<interfaces(s)>", 
                    "Generate code for <interface(s)>",
                    new option[]{
                    new option("-host2mpc", "", "Generate code for kernel client stub -> Mpc", validity.Vswitch),
                    new option("-mpc2host", "", "Generate code for Mpc -> kernel server skeleton", validity.Vswitch),
                    new option("-user2mpc", "", "Generate code for user client stub -> Mpc", validity.Vswitch),
                    new option("-mpc2user", "", "Generate code for Mpc -> user server skeleton", validity.Vswitch), 
                    new option("-mpc2mpc", "", "Generate code for Mpc -> Mpc", validity.Vswitch), 
                    new option("-asynchronous", "", "Generate code for asynchronous communication", validity.Vswitch), 
                    new option("-trace", "", "Generate code for trace communication", validity.Vswitch), 
                    new option("-itf", "", "Generate interface descriptor", validity.Vswitch), 
                    new option("-n", "prefixName", "Prefix name for generated file and symbol", validity.Voptional)});
            
            targetfactory.setTarget();
            CodingStyle origcs = targetfactory.style;

            for(Iterator<String> i = remain.iterator(); i.hasNext();) {
                String option = i.next();
                if("-n".equals(option)) {
                    if (i.hasNext())
                        prefixName = i.next();
                    else
                        throw new compilerexception(error.BC_PREFIX_NOT_SET);
                } else if(option.startsWith("-")) {
                    bctype = BCType.valueOf(option.substring(1));
                } else {
                    Interface itf = tools.idl.idlcompiler.Compile(option, null);
                    interfaceNames.add(itf);
                }
            }
            
            // Sanity checks
            if(bctype == BCType.unknown) {
                throw new compilerexception(error.BC_NOT_SET);
            }
            if (interfaceNames.size() == 0)
                throw new compilerexception(error.INVALID_ENVIRONMENT, "No interface specified");

            dobinding dobind = new dobinding();
            
            // Generate interface descriptor if necessary
            dobind.CompileItf(bctype, interfaceNames);

            // Generate BC for Host (could do nothing if not use on Host)
            generatorfactory.configure();
            dobind.CompileHost(bctype, interfaceNames, prefixName);
            
            // Generate BC for MPC
            if(targetfactory.TARGET == TargetNames.dsp16 || targetfactory.TARGET == TargetNames.dsp24) // Special hack since --cpp is used to distinguish C from CPP user interface
                targetfactory.style = CodingStyle.C;
            else
                targetfactory.style = origcs;
            targetfactory.setTarget();
            
            generatorfactory.configure();
            dobind.CompileBC(bctype, interfaceNames);

        } catch (IllegalArgumentException e) {
            error(e.getMessage());
            System.exit(255);
        } catch(IOException e) {
            error(e.getMessage());
            System.exit(255);
        } catch(compilerexception e) {
            error(e);
            for(int x=0; x < e.getStackTrace().length; x++) { 
                debug(Debug.error, "     " + e.getStackTrace()[x].toString()); 
            } 
            System.exit(e.exitNumber);
        } catch (InterruptedException e) {
            error(e.getMessage());
            System.exit(255);
        } catch (Exception e) {
            error(e.getMessage());
            System.exit(255);
        }
    }

}
