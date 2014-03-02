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
package tools.generator.c.MMDSP;

import java.io.IOException;
import java.io.PrintStream;
import java.util.HashMap;
import java.util.Set;

import tools.common.astbasefile;
import tools.common.compilerexception;
import tools.common.configuration;
import tools.common.targetfactory;
import tools.common.targetfactory.DomainNames;
import tools.common.targetfactory.TargetNames;
import tools.conf.confcompiler;
import tools.conf.ast.astattribute;
import tools.conf.ast.astoption;
import tools.conf.ast.astprimitive;
import tools.conf.ast.astprovide;
import tools.conf.ast.astrequire;
import tools.conf.ast.astsource;
import tools.conf.ast.astoption.OptionNames;
import tools.conf.graph.binding.bcevent;
import tools.conf.graph.component.SchedPriority;
import tools.generator.api.state.constructor;
import tools.generator.api.state.instantiator;
import tools.idl.ast.*;
import tools.idl.generator.eventbase;

public class MMDSPEventGenerator extends eventbase implements tools.generator.api.EventGeneratorInterface
{   
    public MMDSPEventGenerator()
    {
        
    }
    
    public MMDSPEventGenerator(Interface _itf) throws compilerexception 
    {
        setItf(_itf);
    }
    
    class eventprimitive extends bcevent implements constructor, instantiator 
    {
        public eventprimitive(astbasefile _declaration, 
                Interface _itf, 
                astprimitive _type,
                int _size,
                SchedPriority _priority) throws compilerexception, IOException {
            super(_declaration, _itf, _type, _size, _priority);
        }

        public void includeFile(Set<String> include) 
        {
            include.add(itf.nameslash + ".h");
            include.add("rtos/api/descriptor.idt");
        }
        public void declareSharedVariable(Set<String> variables)
        {
            /*
            if(instance.type.parsedFile != null &&
                    instance.type.parsedFile.getPath().endsWith(".arch")) {
                // We are in static deployment, 
                out.println("#pragma nopid");
            }
            out.println("extern int _sb_bases_section;");
             */

            if(targetfactory.domain == DomainNames.MMDSP) 
            {
                variables.add(/*"#pragma nopid" + configuration.eol + */"extern int _sb_bases_section;");
            }
        }

        public void declareInstance(PrintStream out) 
        {
            String THIS;
            if(targetfactory.domain == DomainNames.MMDSP)
                THIS = "&_sb_bases_section";
            else
                THIS = "(void*)0xC0BEEF";

            out.print("struct ");
            declareStruct(itf, out);
            String pathname_ = getPathName_();
            out.println(" " + pathname_ + "_FIFO[" + size + "] = {");
            for(int i = 0; i < size - 1; i++) 
                out.println("    {{&" + pathname_ + "_FIFO[" + (i+1) + "].event, 0, " + THIS + ", " + priority.getValue() + "},  },");
            out.println("    {{0, 0, " + THIS + ", " + priority.getValue() + "}, }");
            out.println("};");
            out.println("extern struct TEvent *" + type.fullyname_ + "_TOP;");
            out.println();
        }

        public void constructInstance(PrintStream out) 
        {
            String pathname_ = getPathName_();
            out.println("  " + type.fullyname_ + "_TOP = &" + pathname_ + "_FIFO[0].event;");
        }

        public void destroyInstance(PrintStream out)
        {
        }
    }
    

    public tools.conf.graph.binding.bcevent getPrimitive(astbasefile _declaration, 
            int size, 
            SchedPriority _priority,
            int _subpriority) throws compilerexception, IOException 
    {
        astprimitive type = registerAst();
        return new eventprimitive(_declaration, itf, type, size, _priority);
    }
    
    private static HashMap<Interface, astprimitive> registeredBCs = new HashMap<Interface, astprimitive>();

    public astprimitive registerAst() throws compilerexception, IOException 
    {
        astprimitive type = registeredBCs.get(itf);
        if(type == null) 
        {
            type = createAst();
            registeredBCs.put(itf, type);
        } 
        else 
        {
            type = new astprimitive(type);
        }
        
        return type;
    }
    
    protected astprimitive createAst() throws compilerexception 
    {
        String compName = "_ev." + itf.name;
        astprimitive type;

        type = (astprimitive)confcompiler.getPreviouslyCompile(compName);
        if(type == null)
        {
            type = new astprimitive(itf.lastModifiedFile, compName);

            String smallname = itf.name.substring(itf.name.lastIndexOf('.')+1);
            astsource source = new astsource(
                    configuration.getComponentTemporaryFile(type, "ev-" + smallname + ".c"));
            PrintStream outc = configuration.needRegeneration(itf.lastModifiedFile, source.file);
            if(outc != null) {
                outc.println("/* Generated asynchronous for '" + itf.name + "' */");
                outc.println("#include <_ev/" + itf.nameslash + ".nmf>");
                outc.println("#include <inc/archi-wrapper.h>");
                outc.println();

                outc.print("struct EVENT" + itf.name_ + " ");
                declareStruct(itf, outc);
                outc.println(";");
                outc.println();

                outc.println("/*");
                outc.println(" * The initialization value is the size of the fifo elem size which is read by CM to allocate fifo."); 
                outc.println(" * Thus the TOP reference the FIFO header when allocated.");
                outc.println(" */");
                outc.println("struct TEvent* ATTR(TOP) = (struct TEvent*)sizeof(struct EVENT" + itf.name_ + ");");
                outc.println();
                // Be careful here, potentially padding could have been added for 2 words integer length"
                // out.println("static struct EVENT" + itf.name_ + " FIFO[1];");

                for(int j = 0; j < itf.methods.size(); j++)
                {
                    Method md = (Method)itf.methods.get(j);

                    /*
                     * Reaction
                     */
                    outc.println("static void REAC" + md.name + 
                            "(struct EVENT" + itf.name_ + "* _xyuv_event) {");  
                    outc.println("  // Declare parameters on stack or registers");
                    int dim = 0, curdim;
                    for(int k = 0; k < md.parameters.size(); k++) 
                    {
                        Parameter fp = (Parameter)md.parameters.get(k);
                        outc.println("  " + fp.type.declareLocalVariable(fp.name) + ";");
                        dim = Math.max(dim, checkData(fp.type));
                    }

                    for(curdim = 0; curdim < dim; curdim++)
                        outc.println("  int ___i" + curdim +";");

                    outc.println("  // Copy parameters");
                    for(int k = 0; k < md.parameters.size(); k++)
                    {
                        Parameter fp = (Parameter)md.parameters.get(k);
                        readData(fp.type, fp.name, "_xyuv_event->data." + md.name + ".", outc, "  ", 0);
                    }
                    outc.println("  _xyuv_event->event.next = ATTR(TOP);");
                    outc.println("  ATTR(TOP) = &_xyuv_event->event;");
                    outc.println("  EMU_unit_maskit = 0;");
                    outc.println("  nmfTraceActivity(TRACE_ACTIVITY_START, (t_uint24)target.THIS, (t_uint24)target." + md.name + ");");
                    outc.print("  target." + md.name+ "(");
                    for(int k = 0; k < md.parameters.size(); k++)
                    {
                        Parameter fp = (Parameter)md.parameters.get(k);
                        if(k > 0) outc.print(", ");
                        outc.print(fp.type.passLocalVariableAsParameter(fp.name));
                    }
                    outc.println(");");
                    outc.println("  nmfTraceActivity(TRACE_ACTIVITY_END, (t_uint24)target.THIS, (t_uint24)target." + md.name + ");");
                    outc.println("}");

                    /*
                     * Posting
                     */
                    outc.println(md.printMethodMETH() + " {");
                    for(int n = 0; n < dim; n++)
                        outc.println("  int ___i" + n +";");
                    outc.println("  ENTER_CRITICAL_SECTION; // OK, if only user emit event");
                    outc.println("  {");
                    outc.println("    struct EVENT" + itf.name_ + "* _xyuv_event = (struct EVENT" + itf.name_ + "*)ATTR(TOP);");
                    outc.println("    if(_xyuv_event == 0) Panic(EVENT_FIFO_OVERFLOW, " + j + ");");
                    outc.println("    ATTR(TOP) = _xyuv_event->event.next;");
                    outc.println("    _xyuv_event->event.reaction = (void*)REAC" + md.name + ";");

                    if(md.parameters.size() != 0)
                    {
                        // Add argument method structure only if there are arguments
                        for(int k = 0; k < md.parameters.size(); k++) 
                        {
                            Parameter fp = (Parameter)md.parameters.get(k);
                            writeData(fp.type, fp.name, fp.name, "_xyuv_event->data." + md.name + ".", outc, "    ", 0);
                        }
                    }
                    outc.println("    nmfTraceActivity(TRACE_ACTIVITY_POST, (t_uint24)target.THIS, (t_uint24)target." + md.name + ");");
                    outc.println("    Sched_Event(&_xyuv_event->event);");
                    outc.println("  }");
                    outc.println("  EXIT_CRITICAL_SECTION;");

                    outc.println("}");
                }

                outc.println("");
                outc.close();
            }

            // Fill the type
            type.addRequire(new astrequire(null, 0, 0, itf.name, "target")); 

            tools.idl.idlcompiler.Compile("rtos.api.panic", null);
            tools.idl.idlcompiler.Compile("rtos.api.ee", null);
            type.addRequire(new astrequire(null, 0, 0, "rtos.api.panic", "panic"));
            type.addRequire(new astrequire(null, 0, 0, "rtos.api.ee", "ee"));

            type.addProvide(new astprovide(null, 0, 0, itf.name, "target"));
            type.addAttribute(new astattribute(null, 0, 0, "TOP"));
            type.sources.add(source);

            type.setOption(new astoption(TargetNames.dsp24, OptionNames.CFLAGS, "-g -O3"));
            type.setOption(new astoption(OptionNames.CFLAGS, "-g -O2"));

            confcompiler.registerComponent(type);
        }
        
        return type;
    }
      
    protected static void declareStruct(Interface itf, PrintStream out) 
    {
        // Check if the union is not empty, and don't declare it elsewhere
        boolean oneParameter = false;
        for(int j = 0; j < itf.methods.size(); j++) 
        {
            Method md = (Method)itf.methods.get(j);
            if(md.parameters.size() != 0) 
            {
                oneParameter = true;
                break;
            }
        }

        out.println("{");
        out.println("  struct TEvent event;");
        if(oneParameter) {
            out.println("  union {");
            for(int j = 0; j < itf.methods.size(); j++) 
            {
                Method md = itf.methods.get(j);
                if(md.parameters.size() != 0) 
                {
                    // Add argument method structure only if there are arguments
                    out.println("    struct {");
                    for(int k = 0; k < md.parameters.size(); k++) 
                    {
                        Parameter fp = (Parameter)md.parameters.get(k);
                        out.println("      " + fp.type.declareLocalVariable(fp.name) + ";");
                    }
                    out.println("    } " + md.name + ";");
                }
            }
            out.println("  } data;");
        }
        out.print("}");
    }
}
