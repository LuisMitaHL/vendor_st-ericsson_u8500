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

import java.io.File;
import java.io.IOException;
import java.io.PrintStream;
import java.util.HashSet;
import java.util.LinkedHashSet;

import tools.common.compilerexception;
import tools.common.configuration;
import tools.conf.ast.astattribute;
import tools.conf.ast.astproperty;
import tools.conf.ast.astprovide;
import tools.conf.ast.astrequire;
import tools.conf.graph.component;
import tools.conf.graph.composite;
import tools.conf.graph.interfacereference;
import tools.conf.graph.primitive;
import tools.conf.graph.primitiveraw;
import tools.conf.graph.visitor;
import tools.conf.graph.binding.bc;
import tools.conf.graph.binding.proxy;
import tools.conf.graph.computer.lifecycle;
import tools.generator.cpp.state.automanaged;
import tools.generator.cpp.state.sharedvariable;
import tools.generator.cpp.state.variable;
import tools.generator.cpp.hostee.bc.*;
import tools.idl.idlcompiler;
import tools.idl.ast.Interface;
import tools.idl.ast.Method;
import tools.idl.generator.predefinemethod;

public class HostEEAssembleGenerator implements tools.generator.api.AssembleGeneratorInterface 
{
    enum Action {setShared, declare, 
        cpppreconstruct, cppconstruct, construct, destroy, start, stop, stop_flush,
        getInterface,
        bindFromUser, unbindFromUser, bindToUser, unbindToUser,
        bindToSMP, unbindToSMP, bindComponent, unbindComponent,
        getMPCInstance, getComponentInterface, bindToMPC, unbindToMPC, bindFromMPC, unbindFromMPC,
        cppdestroy};
    
    public LinkedHashSet<String> included = new LinkedHashSet<String>();
    public LinkedHashSet<String> sharedIncluded = new LinkedHashSet<String>();
    public LinkedHashSet<String> sharedVariables = new LinkedHashSet<String>();

    public int levelOfConstruction = 0;

    public primitive scToOverwrite = null;

    class initializerOfLink extends visitor {
        PrintStream out;
        Action action;
        // bool second = false;
        public initializerOfLink(PrintStream _out, Action _action) {
            out = _out;
            action = _action;
            if(action == Action.destroy || action == Action.stop || action == Action.stop_flush)
                reserveOrder = true;
        }
        
        protected void setAttribute(primitive primitive) throws compilerexception {
            // Declare Attributes
            for (astattribute attribute : primitive.getAttributes()) 
            {
                String value = primitive.getAttributeValue(attribute.name);
                if(value != null)
                    out.println("  _xyuv_" + primitive.getPathName_() + "." + attribute.name + " = " + value + ";");
            }
        }

        protected void setDependencies(primitiveraw primitive) throws compilerexception {
            // Bind require
            for (astrequire require : primitive.getRequires()) 
            {
                if(require.isStatic || require.virtualInterface)
                    continue;
                for(int i = 0; i < require.numberOfElement; i++) 
                {
                    String itfname = require.getInterfaceName(i);
                    interfacereference ir = primitive.lookupFcWithoutError(itfname);
                    String value;
                    if(ir == null)
                    {
                        // This is an unbinded interface
                        value = "0x0;";
                    } 
                    else if(ir.server == null)
                    {
                        // This is an external binding
                        value = "0x0;";
                    }
                    else if(ir.server instanceof proxystub)
                    {
                        ir = ir.server.lookupFcWithoutError("target");
                        assert ir != null;

                        // This is an external binding
                        value = "0x0;";
                    } 
                    else
                    {
                        astprovide provide = ir.server.getProvide(ir.itfname);
                        // This is an internal binding
                        if(provide.collection || ! "".equals(provide.methprefix)) {
                            value = "&_xyuv_" + ir.server.getPathName_() + "._xywrap_" + ir.getInterfaceName() + ";";
                        } else {
                            value = "&_xyuv_" + ir.server.getPathName_() + ";";
                        }
                    }
                    out.println("  _xyuv_" + primitive.getPathName_() + "." + itfname + 
                            " = (" + require.type.replace(".", "_") + "Descriptor*)" + value);
                }
            }
        }
        
        protected void travelRawPrimitive(primitiveraw primitive, Object data) throws compilerexception {     
            // Add specific initialization if need !
            switch (action) {
            case setShared: 
                if (primitive instanceof sharedvariable) {
                    ((sharedvariable)primitive).includeSharedFile(sharedIncluded);  
                    ((sharedvariable)primitive).declareShared(sharedVariables);
                }
                else if(! (primitive instanceof proxy))
                {
                    included.add(primitive.type.fullyname.replace(".", "/") + ".nmf");
                    
                    if(! (primitive instanceof bc) && primitive.priority.getOrdinal() == -1)
                    {
                        sharedVariables.add("t_sint32 " + primitive.priority.getValue() + ";"); // +"// " + primitive.getPathName_()); - this breaks the mechanism
                    }
                }
                break;
            case declare: 
                if (primitive instanceof variable) 
                {
                    ((variable)primitive).declarePrivate(out);
                }
                else if(! (primitive instanceof automanaged))
                {
                    if(primitive == scToOverwrite)
                        out.println("    " + primitive.type.fullyname_ + " &_xyuv_" + primitive.getPathName_() +";  // " + primitive.priority);
                    else 
                        out.println("    " + primitive.type.fullyname_ + " _xyuv_" + primitive.getPathName_() +";  // " + primitive.priority);
                }
                break;
            case cpppreconstruct: 
                if (primitive instanceof variable) 
                    ((variable)primitive).cpppreconstruct(out);
                break;
            case cppconstruct: 
                if (primitive instanceof variable)
                {
                    ((variable)primitive).cppconstruct(out);
                }
                
                if(! (primitive instanceof automanaged)) 
                {
                    String name = primitive.getPathName();
                    if(primitive.debugComment != null) 
                        name = name + " # " + primitive.debugComment;
                    out.println("  _xyuv_" + primitive.getPathName_() + ".name = \"" + name + "\";");
                    setDependencies(primitive);
                    
                    if(! (primitive instanceof proxy) && ! (primitive instanceof bc))
                    {
                        if(primitive.priority.getOrdinal() == -1)
                            out.println("  " + primitive.priority.getValue() +" = 1; // Normal is the default");
                    }

                    setAttribute((primitive)primitive);
                }
                
                break;
            case construct: 
                out.println("  // Construction of " + primitive.getPathName());
                if (primitive instanceof variable) 
                {
                    ((variable)primitive).construct(out);
                    out.println("  levelOfConstruction = " + ++levelOfConstruction + ";");
                }
                else 
                {
                    if(! (primitive instanceof proxy) && ! (primitive instanceof bc))
                    {
                        out.println("  _xyuv_" + primitive.getPathName_() + ".mutexHandle = (t_uint32) eeMutexCreate();");
                        out.println("  if(_xyuv_" + primitive.getPathName_() + ".mutexHandle == 0x0) {");
                        out.println("    error = NMF_NO_MORE_MEMORY;");
                        out.println("    goto out_on_error;");
                        out.println("  }");
                        out.println("  levelOfConstruction = " + ++levelOfConstruction + ";");

                        out.println("  if(signalComponentCreation(" + primitive.priority.getValue() + ")) {");
                        out.println("    error = NMF_NO_MORE_MEMORY;");
                        out.println("    goto out_on_error;");
                        out.println("  }");
                        out.println("  levelOfConstruction = " + ++levelOfConstruction + ";");
                    }
                    
                    if (primitive.getProvide("constructor") != null) 
                    {
                        out.println("  if((error = _xyuv_" + primitive.getPathName_() + ".construct()) != NMF_OK)");
                        out.println("    goto out_on_error;");
                    }
                    out.println("  levelOfConstruction = " + ++levelOfConstruction + ";");
                }
                out.println();
                break;
            case start: 
                if (primitive instanceof variable) 
                    ((variable)primitive).start(out);
                else if (primitive.getProvide("starter") != null) 
                    out.println("  _xyuv_" + primitive.getPathName_() + ".start();");
                break;
            case stop:
                if (primitive instanceof variable)
                    ((variable)primitive).stop(out);
                else if (primitive.getProvide("stopper") != null) 
                    out.println("  _xyuv_" + primitive.getPathName_() + ".stop();");
                break;
            case stop_flush:
                if(! (primitive instanceof proxy) && ! (primitive instanceof bc)) {
                    out.println("  _xyuv_" + primitive.getPathName_() + ".nmfState = NMF::Primitive::STATE_STOPPED_WAIT_FLUSH;");
                    out.println("    eeMutexLock((hMutex) _xyuv_" + primitive.getPathName_() + ".mutexHandle);");
                    out.println("    eeMutexUnlock((hMutex) _xyuv_" + primitive.getPathName_() + ".mutexHandle);");
                }
                if (primitive instanceof callbackasynchronous) {
                    ((callbackasynchronous)primitive).stopFlush(out);
                }
                break;
            case destroy:
                out.println("  // Destruction of " + primitive.getPathName());
                if (primitive instanceof variable) 
                {
                    out.println("  if(levelOfConstruction >= " + levelOfConstruction-- + ") {");
                    ((variable)primitive).destroy(out);
                    out.println("  }");
                }
                else 
                {
                    out.println("  if(levelOfConstruction >= " + levelOfConstruction-- + ") {");
                    if (primitive.getProvide("destructor") != null) 
                    {
                        out.println("    _xyuv_" + primitive.getPathName_() + ".destroy();");
                    }
                    out.println("  }");

                    if(! (primitive instanceof proxy) && ! (primitive instanceof bc))
                    {
                        out.println("  if(levelOfConstruction >= " + levelOfConstruction-- + ") {");
                        out.println("    signalComponentDestruction(" + primitive.priority.getValue() + ");");
                        out.println("  }");

                        out.println("  if(levelOfConstruction >= " + levelOfConstruction-- + ") {");
                        out.println("    eeMutexLock((hMutex) _xyuv_" + primitive.getPathName_() + ".mutexHandle);");
                        out.println("    eeMutexUnlock((hMutex) _xyuv_" + primitive.getPathName_() + ".mutexHandle);");
                        out.println("    eeMutexDestroy((hMutex) _xyuv_" + primitive.getPathName_() + ".mutexHandle);");
                        out.println("  }");
                    }
                }
                out.println();
                break;
            case getInterface:
                if (primitive instanceof callsynchronous) 
                    ((callsynchronous)primitive).getInterface(out);
                else if(primitive instanceof callasynchronous)
                    ((callasynchronous)primitive).getInterface(out);
                break;
                
            case bindFromUser:
                if (primitive instanceof callasynchronous) 
                    ((callasynchronous)primitive).bindFromUser(out);                
                break;
            case unbindFromUser:
                if (primitive instanceof callasynchronous) 
                    ((callasynchronous)primitive).unbindFromUser(out);                
                break;
            case bindToUser:
                if (primitive instanceof callbackasynchronous) 
                    ((callbackasynchronous)primitive).bindToUser(out);                
                break;
            case unbindToUser:
                if (primitive instanceof callbackasynchronous) 
                    ((callbackasynchronous)primitive).unbindToUser(out);                
                break;
                
            case getComponentInterface:
                if (primitive instanceof callasynchronous) 
                    ((callasynchronous)primitive).getComponentInterface(out);                
                break;
            case bindFromMPC:
                if (primitive instanceof callasynchronous) 
                    ((callasynchronous)primitive).bindFromMPC(out);                
                break;
            case unbindFromMPC:
                if (primitive instanceof callasynchronous) 
                    ((callasynchronous)primitive).unbindFromMPC(out);                
                break;
            case bindToMPC:
                if (primitive instanceof callbackasynchronous) 
                    ((callbackasynchronous)primitive).bindToMPC(out);                
                break;
            case unbindToMPC:
                if (primitive instanceof callbackasynchronous) 
                    ((callbackasynchronous)primitive).unbindToMPC(out);                
                break;
                
            case bindToSMP:
                if (primitive instanceof callbackasynchronous) 
                    ((callbackasynchronous)primitive).bindToSMP(out);                
                break;
            case unbindToSMP:
                if (primitive instanceof callbackasynchronous) 
                    ((callbackasynchronous)primitive).unbindToSMP(out);                
                break;
            case bindComponent:
                if (primitive instanceof callbackbase) 
                    ((callbackbase)primitive).bindComponent(out);                
                break;
            case unbindComponent:
                if (primitive instanceof callbackbase) 
                    ((callbackbase)primitive).unbindComponent(out);                
                break;
            case getMPCInstance:
                if(primitive instanceof proxycomp)
                    ((proxycomp)primitive).getMPCreference(out);                
                break;
            case cppdestroy:
               break;
            }
        }
    }
    
    enum tracerAction {instantiate, bind, unbind, destroy};

    class tracer extends visitor {
        PrintStream out;
        tracerAction action;
        // bool second = false;
        public tracer(PrintStream _out, tracerAction _action) {
            out = _out;
            action = _action;
            if(action == tracerAction.unbind || action == tracerAction.destroy)
                reserveOrder = true;
        }
        
        protected void travelRawPrimitive(primitiveraw primitive, Object data) throws compilerexception {     
            // Add specific initialization if need !
            switch (action) {
            case instantiate:
                if(! (primitive instanceof proxy) && ! (primitive instanceof bc))
                {
                    out.println("  nmfTraceComponent(TRACE_COMPONENT_COMMAND_ADD, " +
                            "(NMF::Primitive*)&_xyuv_" + primitive.getPathName_() + ", " +  
                            "_xyuv_" + primitive.getPathName_() + ".name, " +
                            "_xyuv_" + primitive.getPathName_() + ".templateName);");
                    
                    int itfidx = 0;
                    for (astprovide provide : primitive.getProvides()) 
                    {
                        Interface itf = idlcompiler.getPreviouslyCompile(provide.type);
                        
                        int itfmethidx = 0;
                        for(Method md : itf.methods) {
                            out.println("  nmfTraceInterfaceMethod(" +
                                    "(NMF::Primitive*)&_xyuv_" + primitive.getPathName_() + ", " +  
                                    "\"" + provide.methprefix + md.name + "\", " + 
                                    "(" + itfidx + " << 8 | " + itfmethidx + "));");
                            itfmethidx++;
                        }   
                        
                        itfidx++;
                    }
                }
                break;
            case bind:
                if(! (primitive instanceof proxy) && ! (primitive instanceof bc))
                {
                    for(astrequire require : primitive.getRequires())
                    {
                        if(require.isStatic || require.virtualInterface)
                            continue;
                        for(int i = 0; i < require.numberOfElement; i++) {
                            String itfname = require.getInterfaceName(i);
                            interfacereference ir = primitive.lookupFcWithoutError(itfname);
                            if(ir == null)
                            {
                                // This is an unbinded interface
                            } 
                            else 
                            {
                                if(ir.server == null)
                                {
                                    // External synchronous binding
                                }
                                else if(ir.server instanceof callbackasynchronous ||
                                        ir.server instanceof callbacksynchronous ||
                                        ir.server instanceof proxystub)
                                {
                                    ir = ir.server.lookupFcWithoutError("target");
                                    assert ir != null;

                                    // This is an external asynchronous binding (USER or DSP)
                                } 
                                else
                                {
                                    String command = "TRACE_BIND_COMMAND_BIND_SYNCHRONOUS";
                                    
                                    if(ir.server instanceof eventgenerator.eventprimitive) 
                                    {
                                        ir = ir.server.lookupFcWithoutError("target");
                                        assert ir != null;
                                        command = "TRACE_BIND_COMMAND_BIND_ASYNCHRONOUS";
                                    } 
                                    else if(ir.server instanceof tracegenerator.traceprimitive) 
                                    {
                                        ir = ir.server.lookupFcWithoutError("target");
                                        assert ir != null;
                                    }

                                    // This is an internal binding
                                    out.println("  nmfTraceBind(" + command + ", " + 
                                            "(NMF::Primitive*)&_xyuv_" + primitive.getPathName_() + ", " + 
                                            "\"" + itfname + "\", "+
                                            "(NMF::Primitive*)&_xyuv_" + ir.server.getPathName_() + ", " + 
                                            "\"" + ir.getInterfaceName() + "\");");
                                }
                            }
                        }
                    
                    }
                }
                break;
            case unbind:
                if(! (primitive instanceof proxy) && ! (primitive instanceof bc))
                {
                    for(astrequire require : primitive.getRequires())
                    {
                        if(require.isStatic || require.virtualInterface)
                            continue;
                        for(int i = 0; i < require.numberOfElement; i++) {
                            String itfname = require.getInterfaceName(i);
                            interfacereference ir = primitive.lookupFcWithoutError(itfname);
                            if(ir == null)
                            {
                                // This is an unbinded interface
                            } 
                            else 
                            {
                                if(ir.server == null)
                                {
                                    // External synchronous binding
                                }
                                else if(ir.server instanceof callbackasynchronous ||
                                        ir.server instanceof callbacksynchronous ||
                                        ir.server instanceof proxystub)
                                {
                                    ir = ir.server.lookupFcWithoutError("target");
                                    assert ir != null;

                                    // This is an external asynchronous binding (USER or DSP)
                                } 
                                else
                                {
                                    String command = "TRACE_BIND_COMMAND_UNBIND_SYNCHRONOUS";
                                    
                                    if(ir.server instanceof eventgenerator.eventprimitive) 
                                    {
                                        ir = ir.server.lookupFcWithoutError("target");
                                        assert ir != null;
                                        command = "TRACE_BIND_COMMAND_UNBIND_ASYNCHRONOUS";
                                    } 
                                    else if(ir.server instanceof tracegenerator.traceprimitive) 
                                    {
                                        ir = ir.server.lookupFcWithoutError("target");
                                        assert ir != null;
                                    }


                                    // This is an internal binding
                                    out.println("  nmfTraceBind(" + command + ", " + 
                                            "(NMF::Primitive*)&_xyuv_" + primitive.getPathName_() + ", " + 
                                            "\"" + itfname + "\", "+
                                            "(NMF::Primitive*)&_xyuv_" + ir.server.getPathName_() + ", " + 
                                            "\"" + ir.getInterfaceName() + "\");");
                                }
                            }
                        }
                    
                    }
                }
                break;
            case destroy:
                if(! (primitive instanceof proxy) && ! (primitive instanceof bc))
                {
                    out.println("  nmfTraceComponent(TRACE_COMPONENT_COMMAND_REMOVE, " +
                            "(NMF::Primitive*)&_xyuv_" + primitive.getPathName_() + ", " +  
                            "_xyuv_" + primitive.getPathName_() + ".name, " +
                            "_xyuv_" + primitive.getPathName_() + ".templateName);");
                }
                break;
            }
        }
    }
  

    /**
     * Export attributes
     * @author fassino
     *
     */
    class exportAttributes extends visitor {
        PrintStream out;
        public exportAttributes(PrintStream _out) {
            out = _out;
        }
      protected void travelPrimitive(primitive primitive, Object data) throws compilerexception {
            // Declare Attributes with pathname
            for (astattribute attribute : primitive.getAttributes()) {
                out.println("  if(compositeStrcmp(name, \"" + primitive.getPathName_() + "/" + attribute.name + "\") == 0) {");
                out.println("    *value = (t_uint32)_xyuv_" + primitive.getPathName_() + "." + attribute.name + ";");
                out.println("    return NMF_OK;");
                out.println("  }");
            }
        }
    }

    class exportProperties extends visitor 
    {
        PrintStream out;
        public exportProperties(PrintStream _out) {
            out = _out;
        }
        protected void travelCommon(component component, Object data) throws compilerexception 
        {
            for(final astproperty property : component.getProperties()) 
            {
                String pathname = component.getPathName();
                if(pathname != null)
                    pathname = pathname + "/" + property.name;
                else
                    pathname = property.name;
                out.println("  if(compositeStrcmp(name, \"" + pathname + "\") == 0) {");
                out.println("    compositeCpycmp(value, \"" + property.getBinValue() + "\", valuelen);");
                out.println("    return NMF_OK;");
                out.println("  }");
            }
        }
    }

    public void generate(component instance, File dstfile, lifecycle lccstate) throws compilerexception, IOException {
        assert instance instanceof composite;
        
        int nbOfPrim = 0;
        for(component prim : ((composite)instance).getFcSubComponents())
        {
            if(prim instanceof primitive)
            {
                scToOverwrite = (primitive)prim;
                nbOfPrim++;
            }
        }
        if(nbOfPrim > 1)
            scToOverwrite = null;
        
        PrintStream out = configuration.forceRegeneration(
                configuration.getComponentFinalExt(instance.type, "hpp"));

        out.println("/* '" + instance.type.fullyname_ + "' header library */");
        out.println("#if !defined(" + instance.type.fullyname_ + "_NMF)");
        out.println("#define " + instance.type.fullyname_ + "_NMF");
        out.println();
        
        (new initializerOfLink(out,  Action.setShared)).browseComponent(instance);

        out.println("#include <inc/type.h>");
        out.println("#include <cpp.hpp>");
        out.println("#ifndef HOST_ONLY");
        out.println("#include <cm/inc/cm.hpp>");
        out.println("#endif");
        for(final String include: sharedIncluded)
            out.println("#include <" + include + ">");
        HashSet<Interface> incls = new HashSet<Interface>();
        for (astprovide provide : instance.getProvides()) {
            Interface itf = idlcompiler.getPreviouslyCompile(provide.type);
            predefinemethod.GenerateInclude(itf, out, incls);
        }
        for (astrequire require : instance.getRequires()) {
            Interface itf = idlcompiler.getPreviouslyCompile(require.type);
            predefinemethod.GenerateInclude(itf, out, incls);
        }
        out.println();

        /**
         * Declare Composite class
         */
        out.println("class " + instance.type.fullyname_ + ": public NMF::Composite {");
        // Declare shared variable
        out.println("  public:"); 
        for(final String variable: sharedVariables)
            out.println("    " + variable);
        out.println("  protected:"); 
        out.println("    virtual ~" + instance.type.fullyname_ + "() {} // Protected, use " + instance.type.fullyname_ + "() instead !!!"); 
        out.println("};");
        out.println();

        out.println("/*");
        out.println(" * Component Factory");
        if(instance.type.singleton)
            out.println(" * Warning:  This is a singleton component");
        out.println(" */");
        if(scToOverwrite != null)
            out.println("class " + scToOverwrite.type.fullyname_ + ";");
        out.println("IMPORT_NMF_COMPONENT " + instance.type.fullyname_ + "* " + instance.type.fullyname_ + "Create(" +
                (scToOverwrite != null ? scToOverwrite.type.fullyname_ + " *pComp=0x0" : "void") + ");");
        out.println("IMPORT_NMF_COMPONENT void " + instance.type.fullyname_ + "Destroy(" + instance.type.fullyname_ + "*& instance);");
        out.println();

        out.println("#endif");
        out.close();

        /**
         * Generate C 
         */
        out = configuration.forceRegeneration(dstfile);
        
        out.println("/* Static composition of " + instance.type.fullyname + " */");
        out.println("#include <inc/type.h>");
        out.println();
        
        out.println("#define IN_NMF_ASSEMBLY");
        for(final String include : included) {
            out.println("#include <" + include + ">");
        }
        out.println("#include \"" + instance.type.fullyname.replace(".", "/") + ".hpp\"");
        out.println();

        // Generate static dependency
        Interface itf = tools.idl.idlcompiler.Compile("ee.api.trace", null);
        predefinemethod.GenerateInclude(itf, out, incls);
        for(final Method md : itf.methods) {
            out.println("extern \"C\" " +  md.printMethodLibrary() + ";");
        }
        out.println("extern \"C\" IMPORT_SHARED void* EEgetDistributionChannel(t_uint32 priority);");
        out.println("extern \"C\" IMPORT_SHARED t_uint32 signalComponentCreation(t_uint32 priority);");
        out.println("extern \"C\" IMPORT_SHARED t_uint32 signalComponentDestruction(t_uint32 priority);");
        out.println("typedef t_uint32 hMutex;");
        out.println("extern \"C\" IMPORT_SHARED hMutex eeMutexCreate();");
        out.println("extern \"C\" IMPORT_SHARED t_sint32 eeMutexDestroy(hMutex mutex);");
        out.println("extern \"C\" IMPORT_SHARED void eeMutexLock(hMutex mutex);");
        out.println("extern \"C\" IMPORT_SHARED t_sint32 eeMutexLockTry(hMutex mutex);");
        out.println("extern \"C\" IMPORT_SHARED void eeMutexUnlock(hMutex mutex);");
        out.println("enum bindType{BINDASYNC = 0, BINDSYNC = 1};");
        
        /*
         * Declare primitive component instance
         */
        out.println("/*");
        out.println(" * Declare composite class implementation");
        out.println(" */");
        out.println("class " + instance.type.fullyname_ + "Impl: public " + instance.type.fullyname_ + "{");
        out.println("  protected:");
        out.println("       t_sint16 levelOfConstruction;");
        out.println("       t_sint8 startNumber, constructedNumber;");
        (new initializerOfLink(out,  Action.declare)).browseComponent(instance);
        out.println();

        // Declare lifecycle methods
        out.println("  public:");
        out.println("    " + instance.type.fullyname_ + "Impl(" +
                (scToOverwrite != null ? scToOverwrite.type.fullyname_ + " *pComp" : "void") + ");");
        out.println("    virtual ~" + instance.type.fullyname_ + "Impl(void);");
        out.println("    virtual t_nmf_error construct(void);");
        out.println("    virtual void start(void);");
        out.println("    virtual void stop(void);");
        out.println("    virtual void stop_flush(void);");
        out.println("    virtual t_nmf_error destroy(void);");
        
        out.println("    virtual t_nmf_error bindFromUser(const char* name, unsigned int size, NMF::InterfaceReference* reference, unsigned int subpriority);");
        out.println("    virtual t_nmf_error unbindFromUser(const char* name);");
        out.println("    virtual t_nmf_error bindToUser(t_nmf_channel channel, const char* compositeitfname, void *clientContext, unsigned int size);");
        out.println("    virtual t_nmf_error unbindToUser(t_nmf_channel channel, const char* compositeitfname, void **clientContext);");
        out.println("    virtual t_nmf_error bindAsynchronous(const char* name, unsigned int size, NMF::Composite* target, const char* targetname, unsigned int subpriority);");
        out.println("    virtual t_nmf_error unbindAsynchronous(const char* name, NMF::Composite* target, const char* targetname);");
        out.println("    virtual t_nmf_error bindComponent(const char* name, NMF::Composite* target, const char* targetname);");
        out.println("    virtual t_nmf_error unbindComponent(const char* name, NMF::Composite* target, const char* targetname);");
        out.println("    virtual t_nmf_error readAttribute(const char* name, t_uint32 *value);");
        out.println("    virtual t_nmf_error getProperty(const char* name, char *value, t_uint32 valuelen);");
        out.println("    virtual t_nmf_component_handle getMPCComponentHandle(const char *nameinowner);");
        out.println("    virtual t_nmf_component_handle getComponentInterface(const char *compositeitfname, char *realitfname);");
        out.println("    virtual t_nmf_error bindFromMPC(t_nmf_component_handle client, const char* clientitfname, const char * compositeitfname, unsigned int size);");
        out.println("    virtual t_nmf_error unbindFromMPC(t_nmf_component_handle client, const char* clientitfname, const char * compositeitfname);");
        out.println("    virtual t_nmf_error bindToMPC(const char *compositeitfname, t_nmf_component_handle server, const char* serveritfname, unsigned int size);");
        out.println("    virtual t_nmf_error unbindToMPC(const char *compositeitfname, t_nmf_component_handle server, const char* serveritfname);");

        out.println("  protected:");
        out.println("    virtual t_nmf_error getInterface(const char* name, NMF::InterfaceReference* reference);");
        out.println("    virtual void dumpInstantiation();");
        out.println("    virtual void dumpDestruction();");
        
        out.println("    static void dumpInstantiationJumper(void* _this);");

        out.println("};");
        out.println();

        if(instance.type.singleton) {
            out.println("static " + instance.type.fullyname_ + "Impl* instanceSingleton = 0x0;");
            out.println("static int instanceReferenceCounter = 0;");
        }
        out.println("EXPORT_NMF_COMPONENT " + instance.type.fullyname_ + "* " + instance.type.fullyname_ + "Create(" +
                (scToOverwrite != null ? scToOverwrite.type.fullyname_ + " *pComp" : "void") + ") {");
        if(instance.type.singleton) {
            out.println("  if(instanceReferenceCounter++ == 0) {");
            out.println("    instanceSingleton = new " + instance.type.fullyname_ + "Impl(" +
                (scToOverwrite != null ? "pComp" : "") + ");");
            out.println("  }");
            out.println("  return instanceSingleton;");
        } else {
            out.println("  return new " + instance.type.fullyname_ + "Impl(" +
                    (scToOverwrite != null ? "pComp" : "") + ");");
        }
        out.println("}");
        out.println("EXPORT_NMF_COMPONENT  void " + instance.type.fullyname_ + "Destroy(" + instance.type.fullyname_ + "*& instance) {");
        if(instance.type.singleton) 
        {
            out.println("  NMF_ASSERT(instanceSingleton == (" + instance.type.fullyname_ + "Impl*)instance);");
            out.println("  if(--instanceReferenceCounter == 0) {");
            out.println("    delete instanceSingleton;");
            out.println("    instanceSingleton = 0x0;");
            out.println("  }");
        } else 
        {
            out.println("  delete ((" + instance.type.fullyname_ + "Impl*)instance);");
        }
        out.println("  instance = 0x0;");
        out.println("}");
        out.println();

        out.println("/*");
        out.println(" *Declare primitive component instance and methods");
        out.println(" */");

        out.println("" + instance.type.fullyname_ + "Impl::" + instance.type.fullyname_ + "Impl(" +
                (scToOverwrite != null ? scToOverwrite.type.fullyname_ + " *pComp" : "void") + "): ");
        out.print("    startNumber(0)");
        if(scToOverwrite != null)
        {
            out.println(",");
            out.print("    _xyuv_" + scToOverwrite.getPathName_() + "(pComp==NULL? *new " + scToOverwrite.type.fullyname_ + " : *pComp)");
        }
        (new initializerOfLink(out, Action.cpppreconstruct)).browseComponent(instance);
        out.println("{");
        out.println("  levelOfConstruction = 0;");
        out.println("  constructedNumber = 0;");
        (new initializerOfLink(out, Action.cppconstruct)).browseComponent(instance);
        out.println("}");
        out.println();

        out.println("" + instance.type.fullyname_ + "Impl::~" + instance.type.fullyname_ + "Impl() { ");
        if(scToOverwrite != null)
            out.println("  delete &_xyuv_" + scToOverwrite.getPathName_() + ";");
        (new initializerOfLink(out, Action.cppdestroy)).browseComponent(instance);
        out.println("}");
        out.println();
        
            out.println("t_nmf_error " + instance.type.fullyname_ + "Impl::construct(void) {");
            out.println("  t_nmf_error error = NMF_OK;");        // Always declared in order to keep compiler happy
            if(instance.type.singleton) 
            {
                out.println("  if(constructedNumber++ > 0)");
                out.println("    return error; // NMF_OK");
            }
            else
            {
                out.println("  if(constructedNumber++ > 0)");
                out.println("    return NMF_INVALID_COMPONENT_STATE_TRANSITION;");  
            }
            out.println("  if(levelOfConstruction != 0)");
            out.println("    return NMF_INVALID_COMPONENT_STATE_TRANSITION;");
            out.println();

            (new initializerOfLink(out, Action.construct)).browseComponent(instance);
            
            out.println();
            out.println("  nmfRegisterComposite((void*)this, (void*)" + instance.type.fullyname_ + "Impl::dumpInstantiationJumper);");
            out.println("  levelOfConstruction = " + ++levelOfConstruction + ";");
            out.println();
            out.println("  dumpInstantiation();");
            out.println("  return error;");
            out.println("out_on_error:");
            out.println("  destroy();");
            out.println("  return error;");
            out.println("}");
            out.println();

            out.println("void " + instance.type.fullyname_ + "Impl::start(void) {");
            out.println("  if(constructedNumber == 0)");
            out.println("    return;");
            out.println("  if(startNumber++ > 0)");
            out.println("    return;");
            (new initializerOfLink(out, Action.start)).browseComponent(instance);
            out.println("}");
            out.println();

            out.println("void " + instance.type.fullyname_ + "Impl::stop(void) {");
            out.println("  if(constructedNumber == 0)");
            out.println("    return;");
            out.println("  if(--startNumber > 0)");
            out.println("    return;");
            (new initializerOfLink(out, Action.stop)).browseComponent(instance);
            out.println("}");
            out.println();

            out.println("void " + instance.type.fullyname_ + "Impl::stop_flush(void) {");
            out.println("  stop();");
            out.println();
            (new initializerOfLink(out, Action.stop_flush)).browseComponent(instance);
            out.println("}");
            out.println();
            
            out.println("t_nmf_error " + instance.type.fullyname_ + "Impl::destroy(void) {");
            out.println("  if(--constructedNumber > 0)");
            out.println("    return NMF_OK;");
            out.println("  if(startNumber > 0)");
            out.println("    return NMF_COMPONENT_NOT_STOPPED;");
            out.println("  if(levelOfConstruction == 0)");
            out.println("    return NMF_INVALID_COMPONENT_STATE_TRANSITION;");
            out.println();
            out.println("  if(levelOfConstruction >= " + levelOfConstruction-- + ") ");
            out.println("    nmfUnregisterComposite((void*)this);");
            out.println();
            out.println("  dumpDestruction();");
            out.println();
            (new initializerOfLink(out, Action.destroy)).browseComponent(instance);
            out.println();
            out.println("  levelOfConstruction = 0;");
            out.println("  return NMF_OK;");
            out.println("}");
            out.println();

        out.println("t_nmf_error " + instance.type.fullyname_ + "Impl::getInterface(const char* name, NMF::InterfaceReference* reference) {");
        (new initializerOfLink(out,  Action.getInterface)).browseComponent(instance);
        out.println("  return NMF_NO_SUCH_PROVIDED_INTERFACE;");
        out.println("}");
        out.println();

        out.println("t_nmf_error " + instance.type.fullyname_ + "Impl::bindFromUser(const char* name, unsigned int size, NMF::InterfaceReference* reference, unsigned int subpriority) {");
        (new initializerOfLink(out,  Action.bindFromUser)).browseComponent(instance);
        out.println("  return NMF_NO_SUCH_PROVIDED_INTERFACE;");
        out.println("}");
        out.println();
        
        out.println("t_nmf_error " + instance.type.fullyname_ + "Impl::unbindFromUser(const char* name) {");
        (new initializerOfLink(out,  Action.unbindFromUser)).browseComponent(instance);
        out.println("  return NMF_NO_SUCH_PROVIDED_INTERFACE;");
        out.println("}");
        out.println();

        out.println("t_nmf_error " + instance.type.fullyname_ + "Impl::bindToUser(t_nmf_channel channel, const char* compositeitfname, void *clientContext, unsigned int size) {");
        out.println("  t_nmf_error error = NMF_NO_SUCH_REQUIRED_INTERFACE;");
        (new initializerOfLink(out,  Action.bindToUser)).browseComponent(instance);
        out.println("  return error;");
        out.println("}");
        out.println();

        out.println("t_nmf_error " + instance.type.fullyname_ + "Impl::unbindToUser(t_nmf_channel channel, const char* compositeitfname, void **clientContext) {");
        (new initializerOfLink(out,  Action.unbindToUser)).browseComponent(instance);
        out.println("  return NMF_NO_SUCH_REQUIRED_INTERFACE;");
        out.println("}");
        out.println();

        out.println("t_nmf_error " + instance.type.fullyname_ + "Impl::bindAsynchronous(const char* name, unsigned int size, NMF::Composite* target, const char* targetname, unsigned int subpriority) {");
        out.println("  t_nmf_error error = NMF_NO_SUCH_REQUIRED_INTERFACE;");
        out.println();
        out.println("  // Check if server was a MPC");
        out.println("  char realname[MAX_INTERFACE_NAME_LENGTH];");
        out.println("  t_nmf_component_handle server = target->getComponentInterface(targetname, realname);");
        out.println("  if(server != 0x0) ");
        out.println("    return bindToMPC(name, server, realname, size);");
        out.println();
        (new initializerOfLink(out,  Action.bindToSMP)).browseComponent(instance);
        out.println("  return error;");
        out.println("}");
        out.println();

        out.println("t_nmf_error " + instance.type.fullyname_ + "Impl::unbindAsynchronous(const char* name, NMF::Composite* target, const char* targetname) {");
        out.println("    char realname[MAX_INTERFACE_NAME_LENGTH];");
        out.println();
        out.println("  // Check if server was a MPC");
        out.println("    t_nmf_component_handle server = target->getComponentInterface(targetname, realname);");
        out.println("    if(server != 0x0) ");
        out.println("      return unbindToMPC(name, server, realname);");
        out.println();
        (new initializerOfLink(out,  Action.unbindToSMP)).browseComponent(instance);
        out.println("  return NMF_NO_SUCH_REQUIRED_INTERFACE;");
        out.println("}");
        out.println();

        out.println("t_nmf_error " + instance.type.fullyname_ + "Impl::bindComponent(const char* name, NMF::Composite* target, const char* targetname) {");
        out.println("  t_nmf_error error = NMF_NO_SUCH_REQUIRED_INTERFACE;");
        (new initializerOfLink(out,  Action.bindComponent)).browseComponent(instance);
        out.println("  return error;");
        out.println("}");
        out.println();

        out.println("t_nmf_error " + instance.type.fullyname_ + "Impl::unbindComponent(const char* name, NMF::Composite* target, const char* targetname) {");
        (new initializerOfLink(out,  Action.unbindComponent)).browseComponent(instance);
        out.println("  return NMF_NO_SUCH_REQUIRED_INTERFACE;");
        out.println("}");
        out.println();

        out.println("t_nmf_error " + instance.type.fullyname_ + "Impl::readAttribute(const char* name, t_uint32 *value) {");
        (new exportAttributes(out)).browseComponent(instance);
        out.println("  return NMF_NO_SUCH_ATTRIBUTE;");
        out.println("}");
        out.println();
        
        out.println("t_nmf_error " + instance.type.fullyname_ + "Impl::getProperty(const char* name, char *value, t_uint32 valuelen) {");
        (new exportProperties(out)).browseComponent(instance);
        out.println("  return NMF_NO_SUCH_PROPERTY;");
        out.println("}");
        out.println();

        out.println("t_nmf_component_handle " + instance.type.fullyname_ + "Impl::getMPCComponentHandle(const char *nameinowner) {");
        (new initializerOfLink(out,  Action.getMPCInstance)).browseComponent(instance);
        out.println("  return (t_nmf_component_handle)0x0;");
        out.println("}");
        
        out.println("t_nmf_component_handle " +instance.type.fullyname_ + "Impl:: getComponentInterface(const char *compositeitfname, char *realitfname) {");
        (new initializerOfLink(out,  Action.getComponentInterface)).browseComponent(instance);
        out.println("  return (t_nmf_component_handle)0x0;");
        out.println("}");

        out.println("t_nmf_error " + instance.type.fullyname_ + "Impl::bindFromMPC(t_nmf_component_handle client, const char* clientitfname, const char * compositeitfname, unsigned int size) {");
        out.println("#ifndef HOST_ONLY");
        (new initializerOfLink(out,  Action.bindFromMPC)).browseComponent(instance);
        out.println("#endif");
        out.println("  return NMF_NO_SUCH_PROVIDED_INTERFACE;");
        out.println("}");

        out.println("t_nmf_error " + instance.type.fullyname_ + "Impl::unbindFromMPC(t_nmf_component_handle client, const char* clientitfname, const char * compositeitfname) {");
        out.println("#ifndef HOST_ONLY");
        (new initializerOfLink(out,  Action.unbindFromMPC)).browseComponent(instance);
        out.println("#endif");
        out.println("  return NMF_NO_SUCH_PROVIDED_INTERFACE;");
        out.println("}");

        out.println("t_nmf_error " + instance.type.fullyname_ + "Impl::bindToMPC(const char *compositeitfname, t_nmf_component_handle server, const char* serveritfname, unsigned int size) {");
        out.println("  t_nmf_error error = NMF_NO_SUCH_REQUIRED_INTERFACE;");
        out.println("#ifndef HOST_ONLY");
        (new initializerOfLink(out,  Action.bindToMPC)).browseComponent(instance);
        out.println("#endif");
        out.println("  return error;");
        out.println("}");

        out.println("t_nmf_error " + instance.type.fullyname_ + "Impl::unbindToMPC(const char *compositeitfname, t_nmf_component_handle server, const char* serveritfname) {");
        out.println("#ifndef HOST_ONLY");
        (new initializerOfLink(out,  Action.unbindToMPC)).browseComponent(instance);
        out.println("#endif");
        out.println("  return NMF_NO_SUCH_REQUIRED_INTERFACE;");
        out.println("}");

        out.println("void " + instance.type.fullyname_ + "Impl::dumpInstantiation() {");
        (new tracer(out,  tracerAction.instantiate)).browseComponent(instance);
        (new tracer(out,  tracerAction.bind)).browseComponent(instance);
        out.println("}");
        out.println();

        out.println("void " + instance.type.fullyname_ + "Impl::dumpInstantiationJumper(void* _this) {");
        out.println("  " + instance.type.fullyname_ + "Impl* myself = (" + instance.type.fullyname_ + "Impl*)_this;");
        out.println("  myself->dumpInstantiation();");
        out.println("}");
        out.println();

        out.println("void " + instance.type.fullyname_ + "Impl::dumpDestruction() {");
        (new tracer(out,  tracerAction.unbind)).browseComponent(instance);
        (new tracer(out,  tracerAction.destroy)).browseComponent(instance);
        out.println("}");
        out.println();

        out.close();
    }

}
