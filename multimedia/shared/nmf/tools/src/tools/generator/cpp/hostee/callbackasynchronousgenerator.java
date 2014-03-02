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

import tools.common.compilerexception;
import tools.common.configuration;
import tools.conf.ast.astoption;
import tools.conf.ast.astprimitive;
import tools.conf.ast.astprovide;
import tools.conf.ast.astrequire;
import tools.conf.ast.astsource;
import tools.conf.ast.astoption.OptionNames;
import tools.idl.ast.*;
import tools.idl.generator.eventbase;

public class callbackasynchronousgenerator extends eventbase /*implements callbackgenerator*/ {
    /*
    public static primitiveraw getPrimitive(
            astbasefile _declaration,
            Interface _itf,
            String _itfname,
            boolean _optional) throws compilerexception, IOException {
        callbackasynchronous cba = new callbackasynchronous(_itf);
        astprimitive type = cba.getAstAsynchronous();
        return new tools.generator.cpp.bc.callbackasynchronous(_declaration, _itf, _itfname, type, _optional);
    }
*/
    public callbackasynchronousgenerator(Interface _itf) throws compilerexception 
    {
        setItf(_itf);
    }

    public astprimitive getAstAsynchronous() throws compilerexception, IOException 
    {
        astprimitive type = new astprimitive(null, "_cbl." + itf.name);

        String smallname = itf.name.substring(itf.name.lastIndexOf('.')+1);
        astsource source = new astsource(
                configuration.getComponentUniqueTemporaryFile(type, "cb-" + smallname + ".cpp"));
        PrintStream outc = configuration.needRegeneration(itf.lastModifiedFile, source.file);
        if(outc != null) {
            outc.println("/* Generated asynchronous for '" + itf.name + "' */");
            outc.println("#include <_cbl/" + itf.nameslash + ".nmf>");
            outc.println();

            outc.println("#ifdef __EPOC32__");
            outc.println("NONSHARABLE_CLASS(" + type.fullyname_ + ");");
            outc.println("#endif");
            outc.println();
            
            outc.println("t_nmf_error " + type.fullyname_ + "::createFifo(t_nmf_channel channel, t_uint8 _size, void *clientContext) {");
            outc.println("  if (nmfChannelAddRef(channel) != NMF_OK)");
            outc.println("    return NMF_INVALID_PARAMETER;");
            outc.println("  this->channel = channel;");
            outc.println("  if((TOP = nmfQueueCreate()) == 0x0) {");
            outc.println("    nmfChannelRemoveRef(channel);");
            outc.println("    return NMF_NO_MORE_MEMORY;");
            outc.println("  }");
            outc.println("  for(int i = 0; i < _size; i++) {");
            outc.println("    t_xxx_event *pEvent = new t_xxx_event;");
            outc.println("    if(pEvent == 0x0) {");
            outc.println("      destroyFifo(0x0);");
            outc.println("      nmfChannelRemoveRef(channel);");
            outc.println("      return NMF_NO_MORE_MEMORY;");
            outc.println("    }");
            outc.println("    pEvent->event.msg.type = EE_CB_ITF_MSG;");
            outc.println("    pEvent->event.THIS = this;");
            outc.println("    pEvent->event.clientContext = clientContext;");
            outc.println("    pEvent->event.channel = channel;");
            outc.println("    nmfQueuePush(TOP, &pEvent->event.msg.event);");
            outc.println("  }");
            outc.println("  size = _size;");

            outc.println("  return NMF_OK;");
            outc.println("}");
            outc.println();

            outc.println("t_nmf_error " + type.fullyname_ + "::destroyFifo(void **clientContext) {");
            outc.println("  t_uint32 retries = 30;");
            outc.println("  if(TOP != 0x0) {");
            outc.println("    while(size && retries)");
            outc.println("    {");
            outc.println("      t_xxx_event *pEvent = (t_xxx_event *) nmfQueuePop(TOP);");
            outc.println("      if(pEvent != 0x0){");
            outc.println("        delete pEvent;");
            outc.println("        size--;");
            outc.println("        continue;");
            outc.println("      }");
            outc.println("      t_queue_link *pMsg = nmfRemoveMessageFromChannel(channel);");
            outc.println("      if(pMsg != 0x0){");
            outc.println("        delete pMsg;");
            outc.println("        size--;");
            outc.println("        continue;");
            outc.println("      }");
            outc.println("      retries--;");
            outc.println("    }");
            outc.println("    if (size != 0) {");
            outc.println("      if(clientContext != 0x0) *clientContext = (void*)0XDEADBEEF;");
            outc.println("      return NMF_COMPONENT_NOT_STOPPED;");
            outc.println("    }");

            outc.println("    nmfQueueDestroy(TOP);");
            outc.println("    TOP = 0;");
            outc.println("    nmfChannelRemoveRef(channel);");
            outc.println("    return NMF_OK;");
            outc.println("    }");
            outc.println("    return NMF_INTERFACE_NOT_BINDED;");

            outc.println("}");
            outc.println();


            for(int j = 0; j < itf.methods.size(); j++) {
                Method md = (Method)itf.methods.get(j);

                /*
                 * Reaction
                 */
                outc.println("static void REAC" + md.name + "(t_callback_event *pCallbackEvent, void *cbClass) {");
                outc.println("  " + type.fullyname_ + "::t_xxx_event *_xyuv_event = (" + type.fullyname_ + "::t_xxx_event *) pCallbackEvent;");
                outc.println("  " + type.fullyname_ + " *self = (" + type.fullyname_ + " *)_xyuv_event->event.THIS;");
                outc.println("  " + itf.name_ + "Descriptor *target = (" + itf.name_ + "Descriptor*) cbClass;");
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
                outc.println("  nmfQueuePush(self->TOP, &_xyuv_event->event.msg.event);");
                outc.println("  if (self->nmfState == NMF::Primitive::STATE_STOPPED_WAIT_FLUSH)");
                outc.println("    return;");
                outc.print("  target->" + md.name+ "(");
                for(int k = 0; k < md.parameters.size(); k++) {
                    Parameter fp = (Parameter)md.parameters.get(k);
                    if(k > 0) outc.print(", ");
                    outc.print(fp.type.passLocalVariableAsParameter(fp.name));
                }
                outc.println(");");
                outc.println("}");

                /*
                 * Posting
                 */
                outc.println(md.printMethodMETH() + " {");
                for(int n = 0; n < dim; n++)
                    outc.println("  int ___i" + n +";");
                outc.println("  " + type.fullyname_ + " *self=0x0;");
                outc.println("  t_xxx_event *_xyuv_event = (t_xxx_event *) nmfQueuePop(TOP);");
                outc.println("  if(_xyuv_event == 0) { Panic(EVENT_FIFO_OVERFLOW, this, (t_uint32)this->name); return; }");
                outc.println("  self = (" + type.fullyname_ + " *)_xyuv_event->event.THIS;");
                outc.println("  _xyuv_event->event.pReaction = (void*)REAC" + md.name + ";");
                outc.println("  if (self->nmfState == NMF::Primitive::STATE_STOPPED_WAIT_FLUSH)");
                outc.println("    return;");
                if(md.parameters.size() != 0) {
                    // Add argument method structure only if there are arguments
                    for(int k = 0; k < md.parameters.size(); k++) {
                        Parameter fp = (Parameter)md.parameters.get(k);
                        writeData(fp.type, fp.name, fp.name, "_xyuv_event->data." + md.name + ".", outc, "    ", 0);
                    }
                }
                outc.println("  nmfPushMessageInChannel(_xyuv_event->event.channel, &_xyuv_event->event.msg.event);");

                outc.println("}");
            }

            PrintStream outh = configuration.needRegeneration(itf.lastModifiedFile, 
                    configuration.getComponentTemporaryFile(type, "inc/" + smallname + ".hpp"));
            if(outh != null) {
                outh.println("/* Generated callback bc for '" + itf.name + "' */");

                outh.println("class " + type.fullyname_ + ": public " + type.fullyname_ + "Template {");
                outh.println("  public:");
                declareStruct(itf, outh);
                outh.println();

                outh.println("    " + type.fullyname_ + "() {");
                outh.println("      TOP = 0x0;");
                outh.println("      channel = 0x0;");
                outh.println("      size = 0;");
                outh.println("    }");
                outh.println();

                outh.println("    t_queue TOP;");
                outh.println("    t_nmf_channel channel;");
                outh.println("    t_uint8 size;");

                outh.println();   
                outh.println("    t_nmf_error createFifo(t_nmf_channel channel, t_uint8 _size, void *clientContext);");
                outh.println("    t_nmf_error destroyFifo(void **clientContext);");
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
        astrequire output = new astrequire(null, 0, 0, itf.name, "target");
        output.virtualInterface = true;
        type.addRequire(output); 

        tools.idl.idlcompiler.Compile("ee.api.queue", null);
        tools.idl.idlcompiler.Compile("ee.api.callback", null);
        tools.idl.idlcompiler.Compile("ee.api.panic", null);
       astrequire reqsched = new astrequire(null, 0, 0, "ee.api.queue", "queue");
        reqsched.isStatic = true;
        type.addRequire(reqsched);
        reqsched = new astrequire(null, 0, 0, "ee.api.callback", "callback");
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
        out.println("      t_callback_event event;");
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
