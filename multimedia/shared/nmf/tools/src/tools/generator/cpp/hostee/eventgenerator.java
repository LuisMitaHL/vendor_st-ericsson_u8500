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
import tools.conf.graph.binding.bcevent;
import tools.conf.graph.component.SchedPriority;
import tools.generator.cpp.state.variable;
import tools.idl.ast.*;
import tools.idl.generator.eventbase;

public class eventgenerator extends eventbase implements tools.generator.api.EventGeneratorInterface 
{
    public class eventprimitive extends bcevent implements variable {
        protected int subpriority = 0;

        public eventprimitive(astbasefile _declaration, 
                Interface _itf, 
                astprimitive _type,
                int _size,
                SchedPriority _priority,
                int _subpriority) throws compilerexception, IOException {
            super(_declaration, _itf, _type, _size, _priority);
            subpriority = _subpriority;
        }

        public void declarePrivate(PrintStream out) {
            out.println("    " + type.fullyname_ + " _xyuv_" + getPathName_() + "; // " + debugComment);
        }

        public void cpppreconstruct(PrintStream out) {
        }

        public void cppconstruct(PrintStream out) {

        }

        public void construct(PrintStream out) 
        {
            interfacereference ir = lookupFcWithoutError("target");
            
            out.println("  _xyuv_" + getPathName_() + ".mutexHandle = (t_uint32) eeMutexCreate();");
            out.println("  if(_xyuv_" + getPathName_() + ".mutexHandle == 0x0) {");
            out.println("    error = NMF_NO_MORE_MEMORY;");
            out.println("    goto out_on_error;");
            out.println("  }");
            
            if(ir != null && ir.server != null)
            {
            	out.println("  _xyuv_" + getPathName_() + ".connect(" + ir.server.getProvideIndex(ir.itfname) + " << 8, &_xyuv_" + ir.server.getPathName_() + ");");
            	out.println("  _xyuv_" + getPathName_() + ".createFifo(" + size + ", " + priority.getValue() + ", " + subpriority + ");");
            }
        }

        public void start(PrintStream out) {
        }

        public void stop(PrintStream out) {
        }

        public void destroy(PrintStream out) 
        {
            out.println("    _xyuv_" + getPathName_() + ".destroyFifo();");        
            out.println("    eeMutexLock((hMutex) _xyuv_" + getPathName_() + ".mutexHandle);");
            out.println("    eeMutexUnlock((hMutex) _xyuv_" + getPathName_() + ".mutexHandle);");
            out.println("    eeMutexDestroy((hMutex) _xyuv_" + getPathName_() + ".mutexHandle);");
        }
    }

    public tools.conf.graph.binding.bcevent getPrimitive(
            astbasefile _declaration, 
            int size, 
            SchedPriority _priority,
            int _subpriority) throws compilerexception, IOException {
        astprimitive type = registerAst();
        return new eventprimitive(_declaration, itf, type, size, _priority, _subpriority);
    }
    
    public astprimitive registerAst() throws compilerexception, IOException {
        astprimitive type = new astprimitive(null, "_evl." + itf.name);

        String smallname = itf.name.substring(itf.name.lastIndexOf('.')+1);
        astsource source = new astsource(
                configuration.getComponentUniqueTemporaryFile(type, "ev-" + smallname + ".cpp"));
        PrintStream outc = configuration.needRegeneration(itf.lastModifiedFile, source.file);
        if(outc != null) {
            outc.println("/* Generated asynchronous for '" + itf.name + "' */");
            outc.println("#include <_evl/" + itf.nameslash + ".nmf>");
            outc.println();
            
            outc.println("#ifdef __EPOC32__");
            outc.println("NONSHARABLE_CLASS(" + type.fullyname_ + ");");
            outc.println("#endif");
            outc.println();
            
            outc.println("t_nmf_error " + type.fullyname_ + "::createFifo(t_uint8 size, t_sint16 _priority, t_sint16 _subpriority) {");
            outc.println("  if((TOP = nmfQueueCreate()) == 0x0)");
            outc.println("    return NMF_NO_MORE_MEMORY;");
            outc.println("  priority = _priority;");
            outc.println("  subpriority = _subpriority;");
            outc.println("  for(fifosize = 0; fifosize < size; fifosize++) {");
            outc.println("    t_xxx_event *pEvent = new t_xxx_event;");
            outc.println("    if(pEvent == 0x0) {");
            outc.println("      destroyFifo();");
            outc.println("      return NMF_NO_MORE_MEMORY;");
            outc.println("    }");
            outc.println("    pEvent->event.THIS = this;");
            outc.println("    pEvent->event.mutexHandle = targetTHIS->mutexHandle;");
            outc.println("    pEvent->event.priority = priority;");
            outc.println("    pEvent->event.subpriority = subpriority;");
            outc.println("    nmfQueuePush(TOP, (t_queue_link *)pEvent);");
            outc.println("  }");
            outc.println("  return NMF_OK;");
            outc.println("}");
            outc.println();
            
            outc.println("t_nmf_error " + type.fullyname_ + "::destroyFifo() {");
            outc.println("  if(TOP != 0x0) {");
            outc.println("    t_uint32 retries = 30;");
            outc.println();
            outc.println("    while(fifosize && retries) {");
            outc.println("      t_xxx_event *pEvent;");
            outc.println();
            outc.println("      if ((pEvent = (t_xxx_event *) nmfQueuePop(TOP)) == 0x0) {");
            outc.println("        if (targetTHIS->nmfState == NMF::Primitive::STATE_DEFAULT) {");
            outc.println("          //legacy behavior");
            outc.println("          Panic(EVENT_FIFO_IN_USE, this, (t_uint32)this->name); return NMF_COMPONENT_NOT_STOPPED;");
            outc.println("        }");
            outc.println("      } else {");
            outc.println("        delete pEvent;");
            outc.println("        fifosize--;");
            outc.println("        continue;");
            outc.println("      }");
            outc.println();
            outc.println("      if ((pEvent = (t_xxx_event *) nmfSchedulerRemove(this, priority, subpriority)) != 0x0) {");
            outc.println("        delete pEvent;");
            outc.println("        fifosize--;");
            outc.println("        continue;");
            outc.println("      }");
            outc.println();
            outc.println("      nmfSchedulerWait();");
            outc.println("      retries--;");
            outc.println("    }");
            outc.println();
            outc.println("    if (fifosize != 0) {");
            outc.println("      Panic(EVENT_FIFO_IN_USE, this, 0); return NMF_COMPONENT_NOT_STOPPED;");
            outc.println("    }");
            outc.println();
            outc.println("    nmfQueueDestroy(TOP);");
            outc.println("    TOP = 0;");
            outc.println("    return NMF_OK;");
            outc.println("  }");
            outc.println("  return NMF_INTERFACE_NOT_BINDED;");
            outc.println("}");
            outc.println();

            for(int j = 0; j < itf.methods.size(); j++) {
                Method md = (Method)itf.methods.get(j);

                /*
                 * Reaction
                 */
                outc.println("static void REAC" + md.name + "(t_sched_event *pSchedulerEvent) {");
                outc.println("  " + type.fullyname_ + "::t_xxx_event *_xyuv_event = (" + type.fullyname_ + "::t_xxx_event *) pSchedulerEvent;");
                outc.println("  " + type.fullyname_ + " *self = (" + type.fullyname_ + " *)_xyuv_event->event.THIS;");
                outc.println("  // Declare parameters on stack or registers");
                int dim = 0, curdim;
                for(int k = 0; k < md.parameters.size(); k++) {
                    Parameter fp = (Parameter)md.parameters.get(k);
                    outc.println("  " + fp.type.declareLocalVariable(fp.name) + ";");
                    dim = Math.max(dim, checkData(fp.type));
                }

                for(curdim = 0; curdim < dim; curdim++)
                    outc.println("  int ___i" + curdim +";");

                outc.println("  // Copy parameters");
                for(int k = 0; k < md.parameters.size(); k++) {
                    Parameter fp = (Parameter)md.parameters.get(k);
                    readData(fp.type, fp.name, "_xyuv_event->data." + md.name + ".", outc, "  ", 0);
                }
                outc.println("  nmfQueuePush(self->TOP, (t_queue_link *) _xyuv_event);");
                outc.println("  // Check target state and flush evtl");
                outc.println("  if (self->targetTHIS->nmfState == NMF::Primitive::STATE_STOPPED_WAIT_FLUSH)");
                outc.println("    return;");
                
                outc.println("  nmfTraceActivity(TRACE_ACTIVITY_START, self->targetTHIS, self->itfidx | " + j + ");");
                outc.print("  self->target." + md.name+ "(");
                for(int k = 0; k < md.parameters.size(); k++) {
                    Parameter fp = (Parameter)md.parameters.get(k);
                    if(k > 0) outc.print(", ");
                    outc.print(fp.type.passLocalVariableAsParameter(fp.name));
                }
                outc.println(");");
                outc.println("  nmfTraceActivity(TRACE_ACTIVITY_END, self->targetTHIS, self->itfidx | " + j + ");");
                outc.println("}");

                /*
                 * Posting
                 */
                outc.println(md.printMethodMETH() + " {");
                for(int n = 0; n < dim; n++)
                    outc.println("  int ___i" + n +";");
                outc.println("  t_xxx_event *_xyuv_event;");
                outc.println();
                outc.println("  // Check target state and flush evtl");
                outc.println("  if (targetTHIS->nmfState ==  NMF::Primitive::STATE_STOPPED_WAIT_FLUSH)");
                outc.println("    return;");
                outc.println();
                outc.println("  _xyuv_event = (t_xxx_event *) nmfQueuePop(TOP);");

                outc.println("  if(_xyuv_event == 0) { Panic(EVENT_FIFO_OVERFLOW, this, (t_uint32)this->name) ; return; }");
                outc.println("  _xyuv_event->event.pReaction = (void*)REAC" + md.name + ";");
                if(md.parameters.size() != 0) {
                    // Add argument method structure only if there are arguments
                    for(int k = 0; k < md.parameters.size(); k++) {
                        Parameter fp = (Parameter)md.parameters.get(k);
                        writeData(fp.type, fp.name, fp.name, "_xyuv_event->data." + md.name + ".", outc, "    ", 0);
                    }
                }
                outc.println("  nmfTraceActivity(TRACE_ACTIVITY_POST, targetTHIS, itfidx | " + j + ");");

                outc.println("  nmfSchedulerSchedEvent(&_xyuv_event->event);");

                outc.println("}");
            }

            PrintStream outh = configuration.needRegeneration(itf.lastModifiedFile, 
                    configuration.getComponentTemporaryFile(type, "inc/" + smallname + ".hpp"));
            if(outh != null) {
                outh.println("/* Generated event bc for '" + itf.name + "' */");

                outh.println("class " + type.fullyname_ + ": public " + type.fullyname_ + "Template {");
                outh.println("  public:");
                declareStruct(itf, outh);
                outh.println();

                outh.println("    " + type.fullyname_ + "() {");
                outh.println("      TOP = 0x0;");
                outh.println("      fifosize = 0;");
                outh.println("      itfidx = 0;");
                outh.println("      priority = 0;");
                outh.println("      subpriority = 0;");
                outh.println("    }");
                outh.println();
                
                outh.println("    t_queue TOP;");
                outh.println("    t_uint32 fifosize;");
                outh.println("    unsigned int itfidx;");                
                outh.println("    NMF::Primitive* targetTHIS;");
                outh.println("    t_uint16 priority;");
                outh.println("    t_uint16 subpriority;");

                outh.println();   
                outh.println("    t_nmf_error createFifo(t_uint8 size, t_sint16 priority, t_sint16 subpriority);");
                outh.println("    t_nmf_error destroyFifo();");
                outh.println();
                outh.println("    void connect(unsigned int _itfidx, NMF::Primitive* _targetTHIS) {");
                outh.println("      itfidx = _itfidx;");
                outh.println("      targetTHIS = _targetTHIS;");
                outh.println("    }");
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

        tools.idl.idlcompiler.Compile("ee.api.scheduler", null);
        tools.idl.idlcompiler.Compile("ee.api.queue", null);
        tools.idl.idlcompiler.Compile("ee.api.trace", null);
        tools.idl.idlcompiler.Compile("ee.api.panic", null);
        astrequire reqsched = new astrequire(null, 0, 0, "ee.api.scheduler", "scheduler");
        reqsched.isStatic = true;
        type.addRequire(reqsched); 
        reqsched = new astrequire(null, 0, 0, "ee.api.queue", "queue");
        reqsched.isStatic = true;
        type.addRequire(reqsched); 
        reqsched = new astrequire(null, 0, 0, "ee.api.trace", "trace");
        reqsched.isStatic = true;
        type.addRequire(reqsched); 
        reqsched = new astrequire(null, 0, 0, "ee.api.panic", "panic");
        reqsched.isStatic = true;
        type.addRequire(reqsched); 

        type.addProvide(new astprovide(null, 0, 0, itf.name, "target"));
        type.sources.add(source);

        type.setOption(new astoption(OptionNames.CFLAGS, "-g -O3"));

        return type;
    }

    public void declareStruct(Interface itf, PrintStream out) {
        // Check if the union is not empty, and don't declare it elsewhere
        boolean oneParameter = false;
        for(int j = 0; j < itf.methods.size(); j++) {
            Method md = (Method)itf.methods.get(j);
            if(md.parameters.size() != 0) {
                oneParameter = true;
                break;
            }
        }

        out.println("    typedef struct _t_xxx_event {");
        out.println("      t_sched_event event;");
        if(oneParameter) {
            out.println("      union {");
            for(int j = 0; j < itf.methods.size(); j++) {
                Method md = itf.methods.get(j);
                if(md.parameters.size() != 0) {
                    // Add argument method structure only if there are arguments
                    out.println("        struct {");
                    for(int k = 0; k < md.parameters.size(); k++) {
                        Parameter fp = (Parameter)md.parameters.get(k);
                        out.println("          " + fp.type.declareLocalVariable(fp.name) + ";");
                    }
                    out.println("        } " + md.name + ";");
                }
            }
            out.println("      } data;");
        }
        //out.println("    EVENT() {}");
        out.println("    } t_xxx_event;");
    }

}
