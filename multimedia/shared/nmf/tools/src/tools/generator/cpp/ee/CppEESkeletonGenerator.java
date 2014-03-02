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
package tools.generator.cpp.ee;

import java.io.IOException;
import java.io.PrintStream;
import java.util.Set;

import tools.common.astbasefile;
import tools.common.compilerexception;
import tools.common.configuration;
import tools.conf.confcompiler;
import tools.conf.ast.astprimitive;
import tools.conf.ast.astattribute;
import tools.conf.ast.astprovide;
import tools.conf.ast.astrequire;
import tools.conf.ast.astsource;
import tools.conf.graph.binding.bc;
import tools.generator.api.SkeletonGeneratorInterface;
import tools.generator.api.state.constructor;
import tools.generator.api.state.instantiator;
import tools.idl.ast.Interface;
import tools.idl.ast.Method;
import tools.idl.ast.Parameter;
import tools.idl.generator.stubserver;

public class CppEESkeletonGenerator extends stubserver implements SkeletonGeneratorInterface
{
    public class stubprimitive extends bc implements constructor, instantiator 
    {
        public stubprimitive(astbasefile _declaration,
                String _name,
                Interface _itf, 
                astprimitive _type) throws compilerexception, IOException {
            super(_declaration, _name, SchedPriority.Normal, _type);
        }

        public void includeFile(Set<String> include) 
        {
        }
        
        public void declareSharedVariable(Set<String> variables) 
        {
        }

        public void declareInstance(PrintStream out) 
        {
            out.println("  // Declare SKELETON");
        }

        public void constructInstance(PrintStream out) 
        {
            out.println("  // Construct SKELETON");
        }

        public void destroyInstance(PrintStream out) 
        {
        }
    }
    

    public bc getPrimitive(astbasefile _declaration, String _name) throws compilerexception 
    {
        try
        {
            astprimitive type = registerAst();
            return new stubprimitive(_declaration, _name, itf, type);
        } 
        catch (IOException e)
        {
            throw new compilerexception(_declaration, e.getMessage());
        }
    }
    
    public astprimitive registerAst() throws compilerexception, IOException
    {
        String compName = "_skl." + itf.name;
        astprimitive type;

        type = (astprimitive)confcompiler.getPreviouslyCompile(compName);
        if(type == null)
        {
            type = new astprimitive(itf.lastModifiedFile, compName);

            String smallname = itf.name.substring(itf.name.lastIndexOf('.')+1);

            PrintStream outh = configuration.needRegeneration(itf.lastModifiedFile, 
                    configuration.getComponentTemporaryFile(type, "inc/" + smallname + ".hpp"));
            if(outh != null) {
                outh.println("/* Generated skeleton bc for '" + itf.name + "' */");
                outh.println("#include <ee.hpp>");

                //define class
                outh.println("class " + type.fullyname_ + ": public " + type.fullyname_ + "Template {");
                outh.println("  public:");

                //menbers
                outh.println("    t_queue TOP;");
                outh.println("    t_nmf_fifo_id FIFO;");
                outh.println("    t_uint32 fifosize;");
                outh.println("    NMF::Primitive* targetTHIS;");
                outh.println("    Iee_api_queue Iqueue;");
                outh.println("    Iee_api_scheduler Ischeduler;");
                outh.println("    Iee_api_communication Icoms;");
                outh.println();

                //constructor
                outh.println("    " + type.fullyname_ + "() {");
                outh.println("      // TODO : This is a temporary fix to avoid crash in scheduler");
                outh.println("      // Need to discuss with JPF where  we take the semaphore");
                outh.println("      targetTHIS = (NMF::Primitive *) this;");
                if (maxnb16 == 0)
                    outh.println("      FIFO = (t_nmf_fifo_id) 4;");
                else
                    outh.println("      FIFO = (t_nmf_fifo_id)" + maxnb16 * 2 + ";");
                outh.println("      eeNetwork.getInterface(\"queue\", &Iqueue);"); //TODO : add assert on null
                outh.println("      eeNetwork.getInterface(\"scheduler\", &Ischeduler);"); //TODO : add assert on null
                outh.println("      eeNetwork.getInterface(\"coms\", &Icoms);"); //TODO : add assert on null
                outh.println("    }");
                outh.println();

                //methods for ee.api.bc
                outh.println("    virtual t_nmf_error createFifo(t_uint8 size, t_sint16 priority, t_sint16 subpriority);");
                outh.println("    virtual t_nmf_error destroyFifo();");
                outh.println();

                //methods for ee.api.reaction
                outh.println("    virtual void run(t_sched_event* pEvent);");
                outh.println();

                //methods for ee.api.skel
                outh.println("    virtual t_sched_remote_event *popEvent();");
                outh.println();

                outh.println("};");
                outh.println();

                outh.close();
            }

            astsource source = new astsource(
                    configuration.getComponentTemporaryFile(type, "sk-" + smallname + ".cpp"));
            PrintStream outc = configuration.needRegeneration(itf.lastModifiedFile, source.file);
            if(outc != null) 
            {
                outc.println("/* Generated skeleton for '" + itf.name + "' */");
                outc.println("#include <_skl/" + itf.name.replace('.', '/') + ".nmf>");
                outc.println();

                outc.println("#ifdef __EPOC32__");
                outc.println("NONSHARABLE_CLASS(" + type.fullyname_ + ");");
                outc.println("#endif");
                outc.println();

                // fifo itf
                outc.println("t_nmf_error " + type.fullyname_ + "::createFifo(t_uint8 size, t_sint16 priority, t_sint16 subpriority) {");
                outc.println("  if((TOP = Iqueue.create()) == 0x0)");
                outc.println("    return NMF_NO_MORE_MEMORY;");
                outc.println("  for(fifosize = 0; fifosize < size; fifosize++) {");
                outc.println("    t_sched_remote_event *pEvent = new t_sched_remote_event;");
                outc.println("    if(pEvent == 0x0) {");
                outc.println("      destroyFifo();");
                outc.println("      return NMF_NO_MORE_MEMORY;");
                outc.println("    }");
                outc.println("    pEvent->event.IReaction = (ee_api_reactionDescriptor *) this;");
                outc.println("    pEvent->event.mutexHandle = targetTHIS->mutexHandle;");
                outc.println("    pEvent->event.priority = priority;");
                outc.println("    pEvent->event.subpriority = subpriority;");
                outc.println("    Iqueue.push(TOP, (t_queue_link *)pEvent);");
                outc.println("  }");
                outc.println("  return NMF_OK;");
                outc.println("}");
                outc.println();
                outc.println("t_nmf_error " + type.fullyname_ + "::destroyFifo() {");
                outc.println("  if(TOP != 0x0) {");
                outc.println("    for(; fifosize--;) {");
                outc.println("      t_sched_remote_event *pEvent = (t_sched_remote_event *) Iqueue.pop(TOP);");
                outc.println("      if(pEvent == 0x0) { /*Panic(EVENT_FIFO_IN_USE, this, 0); */return NMF_COMPONENT_NOT_STOPPED; }");
                outc.println("      delete pEvent;");
                outc.println("    }");
                outc.println("    Iqueue.destroy(TOP);");
                outc.println("    TOP = 0;");
                outc.println("    return NMF_OK;");
                outc.println("  }");
                outc.println("  return NMF_INTERFACE_NOT_BINDED;");
                outc.println("}");
                outc.println();

                // reaction itf
                outc.println("void METH(run)(t_sched_event* pEvent) {");
                outc.println("  switch(pEvent->methodIndex) {");
                for(int j = 0; j < itf.methods.size(); j++) {
                    Method md = (Method)itf.methods.get(j);
                    outc.println("    case " + j + ":");
                    outc.println("    {");
                    outc.println("      t_sched_remote_event *_xyuv_event = (t_sched_remote_event *) pEvent;");
                    outc.println("      t_uint32 *_xyuv_data = (t_uint32 *) (t_uint32)_xyuv_event->data;");
                    // Declare parameter
                    for(int k = 0; k < md.parameters.size(); k++)
                    {
                        Parameter fp = (Parameter)md.parameters.get(k);
                        outc.println("      " + fp.type.declareLocalVariable(md.name + "_" + fp.name) + ";");
                    }
                    // Marshall parameter
                    int nb16 = 0;
                    for(int k = 0; k < md.parameters.size(); k++)
                    {
                        Parameter fp = (Parameter)md.parameters.get(k);
                        outc.println("      /* " + fp.name + " <" + fp.type.printType() + "> marshalling */");
                        nb16 += readData(fp.type, "    " + md.name + "_" + fp.name, 
                                outc, Integer.toString(nb16), "  ", 0);
                    }
                    // push event in queue
                    outc.println("      Iqueue.push(TOP, (t_queue_link *) pEvent);");
                    // Acknoledge param fifo
                    outc.println("      Icoms.acknowledgeEvent(ATTR(FIFO));");
                    // Call target method
                    outc.print("      target." + md.name+ "(");
                    for(int k = 0; k < md.parameters.size(); k++) {
                        Parameter fp = (Parameter)md.parameters.get(k);
                        if(k > 0) outc.print(", ");
                        outc.print(fp.type.passLocalVariableAsParameter(md.name + "_" + fp.name));
                    }
                    outc.println(");");
                    outc.println("    }");
                    outc.println("    break;");
                }
                outc.println("    default:");
                outc.println("      //TODO : add panic");
                outc.println("      break;");
                outc.println("  }");
                outc.println("}");
                outc.println();

                // skel.itf
                outc.println("t_sched_remote_event *METH(popEvent)() {");
                outc.println("  return (t_sched_remote_event *) Iqueue.pop(TOP);");
                outc.println("}");
                outc.println();

                outc.close();
            }

            // Fill the type
            tools.idl.idlcompiler.Compile("ee.api.skel", null);
            tools.idl.idlcompiler.Compile("ee.api.bc", null);
            tools.idl.idlcompiler.Compile("ee.api.reaction", null);
            tools.idl.idlcompiler.Compile("ee.api.scheduler", null);
            tools.idl.idlcompiler.Compile("ee.api.queue", null);
            tools.idl.idlcompiler.Compile("ee.api.communication", null);

            // require itf that is proxied
            type.addRequire(new astrequire(null, 0, 0, itf.name, "target"));
            type.sources.add(source);

            // provide itf
            type.addProvide(new astprovide(null, 0, 0, "ee.api.reaction", "reaction"));
            type.sources.add(source);
            type.addProvide(new astprovide(null, 0, 0, "ee.api.bc", "bcFifo"));
            type.sources.add(source);
            type.addProvide(new astprovide(null, 0, 0, "ee.api.skel", "skeleton"));
            type.sources.add(source);

            //require itf
            astrequire reqsched = new astrequire(null, 0, 0, "ee.api.scheduler", "scheduler");
            reqsched.optional = true;
            type.addRequire(reqsched); 
            reqsched = new astrequire(null, 0, 0, "ee.api.queue", "queue");
            reqsched.optional = true;
            type.addRequire(reqsched);
            reqsched = new astrequire(null, 0, 0, "ee.api.communication", "coms");
            reqsched.optional = true;
            type.addRequire(reqsched);

            //add attributes
            type.addAttribute(new astattribute(null, 0,0,"FIFO"));

            confcompiler.registerComponent(type);
        }

        return type;
    }
}
