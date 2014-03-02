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
package tools.generator.cpp.hostee;

import java.io.IOException;
import java.io.PrintStream;

import tools.common.astbasefile;
import tools.common.compilerexception;
import tools.common.configuration;
import tools.conf.ast.astoption;
import tools.conf.ast.astprimitive;
import tools.conf.ast.astprovide;
import tools.conf.ast.astrequire;
import tools.conf.ast.astsource;
import tools.conf.ast.astoption.OptionNames;
import tools.conf.graph.interfacereference;
import tools.conf.graph.binding.bc;
import tools.generator.cpp.state.variable;
import tools.idl.ast.*;

public class tracegenerator implements tools.generator.api.TraceGeneratorInterface {
    Interface itf;
    
    public void setItf(Interface _itf) {
        this.itf = _itf;
    }
    
    public class traceprimitive extends bc implements variable {
        interfacereference ir;
        
        public traceprimitive(astbasefile _declaration, 
                astprimitive _type) throws compilerexception, IOException {
            super(_declaration, SchedPriority.Normal, _type);
        }

        public void declarePrivate(PrintStream out) {
            out.println("    " + type.fullyname_ + " _xyuv_" + getPathName_() + "; // " + debugComment);
        }

        public void cpppreconstruct(PrintStream out) {
            ir = lookupFcWithoutError("target");
            
            if(ir != null && ir.server != null)
            {
            	out.println(",");
            	out.print("    _xyuv_" + getPathName_() + "(" + ir.server.getProvideIndex(ir.itfname) + " << 8, &_xyuv_" + ir.server.getPathName_() + ")");
            }
        }

        public void cppconstruct(PrintStream out) {
        }

        public void construct(PrintStream out) 
        {
            out.println("  _xyuv_" + getPathName_() + ".mutexHandle = (t_uint32) eeMutexCreate();");
            out.println("  if(_xyuv_" + getPathName_() + ".mutexHandle == 0x0) {");
            out.println("    error = NMF_NO_MORE_MEMORY;");
            out.println("    goto out_on_error;");
            out.println("  }");
        }

        public void start(PrintStream out) {
        }

        public void stop(PrintStream out) {
        }

        public void destroy(PrintStream out) 
        {
            out.println("    eeMutexLock((hMutex) _xyuv_" + getPathName_() + ".mutexHandle);");
            out.println("    eeMutexUnlock((hMutex) _xyuv_" + getPathName_() + ".mutexHandle);");
            out.println("    eeMutexDestroy((hMutex) _xyuv_" + getPathName_() + ".mutexHandle);");
        }
    }
    
    public tools.conf.graph.binding.bc getPrimitive(
            astbasefile _declaration) throws compilerexception, IOException {
        astprimitive type = getAst();
        return new traceprimitive(_declaration, type);
    }
    
    public astprimitive getAst() throws compilerexception, IOException {
        astprimitive type = new astprimitive(null, "_trl." + itf.name);

        String smallname = itf.name.substring(itf.name.lastIndexOf('.')+1);
        astsource source = new astsource(
                configuration.getComponentUniqueTemporaryFile(type, "tr-" + smallname + ".cpp"));
        PrintStream outc = configuration.needRegeneration(itf.lastModifiedFile, source.file);
        if(outc != null) {
            outc.println("/* Generated snchronous trace for '" + itf.name + "' */");
            outc.println("#include <_trl/" + itf.nameslash + ".nmf>");
            outc.println();
            
            outc.println("#ifdef __EPOC32__");
            outc.println("NONSHARABLE_CLASS(" + type.fullyname_ + ");");
            outc.println("#endif");
            outc.println();
            
            for(int j = 0; j < itf.methods.size(); j++) {
                Method md = (Method)itf.methods.get(j);

                outc.println(md.printMethodMETH() + " {");
                outc.println("  nmfTraceActivity(TRACE_ACTIVITY_CALL, targetTHIS, itfidx | " + j + ");");
                outc.print("  target." + md.name+ "(");
                for(int k = 0; k < md.parameters.size(); k++) {
                    Parameter fp = (Parameter)md.parameters.get(k);
                    if(k > 0) outc.print(",");
                    outc.print(fp.name);
                }
                outc.println(");");
                outc.println("  nmfTraceActivity(TRACE_ACTIVITY_RETURN, targetTHIS, itfidx | " + j + ");");
                outc.println("}");
            }

            PrintStream outh = configuration.needRegeneration(itf.lastModifiedFile, 
                    configuration.getComponentTemporaryFile(type, "inc/" + smallname + ".hpp"));
            if(outh != null) {
                outh.println("/* Generated synchronous trace bc for '" + itf.name + "' */");

                outh.println("class " + type.fullyname_ + ": public " + type.fullyname_ + "Template {");
                outh.println("  public:");

                outh.println("    " + type.fullyname_ + "(unsigned int _itfidx, NMF::Primitive* _targetTHIS): itfidx(_itfidx), targetTHIS(_targetTHIS) {");
                outh.println("    }");
                outh.println();
                
                outh.println("    const unsigned int itfidx;");                
                outh.println("    NMF::Primitive* targetTHIS;");                
                outh.println();   
                outh.println("  protected:");
                for(Method md : itf.methods) {
                    outh.println("    virtual " + md.printMethodCpp("", false) + ";");
                }
                outh.println("};");
            }

            outc.println("");
            outc.close();
        }

        // Fill the type
        type.addRequire(new astrequire(null, 0, 0, itf.name, "target")); 

        tools.idl.idlcompiler.Compile("ee.api.trace", null);
        astrequire reqsched = new astrequire(null, 0, 0, "ee.api.trace", "trace");
        reqsched.isStatic = true;
        type.addRequire(reqsched); 

        type.addProvide(new astprovide(null, 0, 0, itf.name, "target"));
        type.sources.add(source);

        type.setOption(new astoption(OptionNames.CFLAGS, "-g -O3"));

        return type;
    }
}
