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

import java.io.PrintStream;

import tools.generator.api.SkeletonGeneratorInterface;
import tools.idl.ast.*;
import tools.idl.generator.stubserver;
import tools.common.astbasefile;
import tools.common.compilerexception;
import tools.common.configuration;
import tools.common.error;
import tools.conf.confcompiler;
import tools.conf.ast.astattribute;
import tools.conf.ast.astoption;
import tools.conf.ast.astprimitive;
import tools.conf.ast.astprovide;
import tools.conf.ast.astrequire;
import tools.conf.ast.astsource;
import tools.conf.ast.astoption.OptionNames;
import tools.conf.graph.binding.bc;

public class MMDSPSkeletonGenerator extends stubserver implements SkeletonGeneratorInterface
{
    
    protected int stackSizeWordMMDSP(Type type) throws compilerexception 
    {
        if(type.typedefinition != null)
        {
            return stackSizeWordMMDSP(type.typedefinition);
        }
        else if (type instanceof TypePrimitive) 
        {
            TypePrimitive primitive = (TypePrimitive) type;
            if(primitive.packedsize <= 24)
                return 1;
            else if(primitive.packedsize <= 48)
                return 2;
            else {
                throw new compilerexception(type, error.COMS_TOO_BIG_TYPE);
            }
        }
        else if (type instanceof TypePointer) 
        {
            return 1;
        } 
        else if (type instanceof typereference) 
        {
            return 1;
        }
        else if (type instanceof structure) 
        {
            int nb24 = 0;
            structure ts = (structure)type;
            for(structentry se : ts.members) {
                nb24 += stackSizeWordMMDSP(se.type);
            }
            return nb24;
        } 
        else if (type instanceof enumeration)
        {
            // Handle enum as 32bits
            return 1;
        }
        else if (type instanceof typedef) 
        {
            typedef td = (typedef)type;
            return stackSizeWordMMDSP(td.type);
        } 
        else if (type instanceof typearray) 
        {
            typearray array = (typearray) type;
            return stackSizeWordMMDSP(array.type) * array.sizeInteger;
        } 
        else if (type instanceof typeconst) 
        {
            typeconst cst = (typeconst)type;
            return stackSizeWordMMDSP(cst.type);
        } 
        else 
        {
            throw new compilerexception(type, error.COMS_UNKNOWN_TYPE, type);
        }
    }

    protected int paramSizeWordMMDSP(Type type) throws compilerexception 
    {
        if(type.typedefinition != null)
        {
            return paramSizeWordMMDSP(type.typedefinition);
        }
        else if (type instanceof TypePrimitive) 
        {
            TypePrimitive primitive = (TypePrimitive) type;
            if(primitive.packedsize <= 24)
                return 1;
            else if(primitive.packedsize <= 48)
                return 2;
            else {
                throw new compilerexception(type, error.COMS_TOO_BIG_TYPE);
            }
        }
        else if (type instanceof TypePointer) 
        {
            return 1;
        } 
        else if (type instanceof typereference) 
        {
            typereference tr = (typereference) type;
            return paramSizeWordMMDSP(tr.type);
        }
        else if (type instanceof structure) 
        {
            int nb24 = 0;
            structure ts = (structure)type;
            for(structentry se : ts.members) {
                nb24 += paramSizeWordMMDSP(se.type);
            }
            return nb24;
        } 
        else if (type instanceof enumeration)
        {
            // Handle enum as 32bits
            return 1;
        }
        else if (type instanceof typedef) 
        {
            typedef td = (typedef)type;
            return paramSizeWordMMDSP(td.type);
        } 
        else if (type instanceof typearray) 
        {
            typearray array = (typearray) type;
            return paramSizeWordMMDSP(array.type) * array.sizeInteger;
        } 
        else if (type instanceof typeconst) 
        {
            typeconst cst = (typeconst)type;
            return paramSizeWordMMDSP(cst.type);
        } 
        else 
        {
            throw new compilerexception(type, error.COMS_UNKNOWN_TYPE, type);
        }
    }

    public astprimitive registerAst() throws compilerexception 
    {
        String compName = "_sk." + itf.name;
        astprimitive type;

        type = (astprimitive)confcompiler.getPreviouslyCompile(compName);
        if(type == null)
        {
            type = new astprimitive(itf.lastModifiedFile, compName);

            // Compute stack size
            boolean needToPutInDDRMeth[] = new boolean[itf.methods.size()];
            int stack = 0;
            boolean needToPutInDDR = false;
            for(int j = 0; j < itf.methods.size(); j++) 
            {
                Method md = (Method)itf.methods.get(j);

                int stacksize = 0;
                for(int k = 0; k < md.parameters.size(); k++)
                {
                    Parameter fp = (Parameter)md.parameters.get(k);

                    int paramsize = paramSizeWordMMDSP(fp.type);                
                    if(paramsize > 32)
                        needToPutInDDRMeth[j] = needToPutInDDR = true;
                    else
                        stacksize += paramsize; // For declaring parameter

                    stacksize += stackSizeWordMMDSP(fp.type); // For passing parameter in stack (reference == 1 here)
                }
                stack = Math.max(stack, stacksize);
            }

            String smallname = itf.name.substring(itf.name.lastIndexOf('.')+1);
            astsource source = new astsource(
                    configuration.getComponentTemporaryFile(type, "sk-" + smallname + ".c"));
            PrintStream outc = configuration.needRegeneration(itf.lastModifiedFile, source.file);
            if(outc != null) 
            {
                outc.println("/* Generated dsp skeleton for '" + itf.name + "' */");
                outc.println(" // STACK computed = " + stack);
                outc.println("#include <_sk/" + itf.name.replace('.', '/') + ".nmf>");
                outc.println("#include <inc/archi-wrapper.h>");
                outc.println("#include <stwdsp.h>");
                outc.println();

                outc.println("struct TEvent* ATTR(TOP) = (struct TEvent*)sizeof(t_remote_event);");
                // We don't want that ATTR(FIFO) is set to zero. We don't want that because in that
                // case FIFO address is located behind TOP address, and in that case traceSched() in
                // IT11 send wrong trace information.
                if (maxnb16 == 0)
                {
                    outc.println("t_nmf_fifo ATTR(FIFO) = (t_nmf_fifo)1;");
                }
                else 
                {
                    outc.println("t_nmf_fifo ATTR(FIFO) = (t_nmf_fifo)" + maxnb16 + ";");
                }
                outc.println();

                for(int j = 0; j < itf.methods.size(); j++) 
                {
                    Method md = (Method)itf.methods.get(j);


                    // In case of too huge stack size, put parameters on external memory
                    if(needToPutInDDRMeth[j])
                    {
                        for(int k = 0; k < md.parameters.size(); k++)
                        {
                            Parameter fp = (Parameter)md.parameters.get(k);
                            outc.println("static EXTMEM24 " + fp.type.declareLocalVariable(md.name + "_" + fp.name) + ";");
                        }
                    }

                    outc.println("#pragma force_dcumode");
                    outc.println("void METH(" + md.name + ")(t_remote_event *_xyuv_event) {");
                    outc.println("  t_event_params_handle _xyuv_data = _xyuv_event->data;");
                    for(int n = 0; n < needi[j]; n++)
                        outc.println("    int ___i" + n + ", ___j" + n +";");

                    // Declare parameter
                    if(! needToPutInDDRMeth[j])
                    {
                        for(int k = 0; k < md.parameters.size(); k++)
                        {
                            Parameter fp = (Parameter)md.parameters.get(k);
                            outc.println("  " + fp.type.declareLocalVariable(md.name + "_" + fp.name) + ";");
                        }
                    }

                    // Marshall parameter
                    int nb16 = 0;
                    for(int k = 0; k < md.parameters.size(); k++)
                    {
                        Parameter fp = (Parameter)md.parameters.get(k);
                        outc.println("    /* " + fp.name + " <" + fp.type.printType() + "> marshalling */");
                        nb16 += readData(fp.type, md.name + "_" + fp.name, 
                                outc, Integer.toString(nb16), "  ", 0);
                    }

                    outc.println("  /* Server calling */");
                    outc.println("  _xyuv_event->event.next = ATTR(TOP);");
                    outc.println("  ATTR(TOP) = _xyuv_event;");
                    outc.println("  AcknowledgeEvent(ATTR(FIFO));");
                    outc.println("  EMU_unit_maskit = 0;");
                    outc.println("  nmfTraceActivity(TRACE_ACTIVITY_START, (t_uint24)target.THIS, (t_uint24)target." + md.name + ");");
                    outc.print("  target." + md.name+ "(");

                    // Call target
                    for(int k = 0; k < md.parameters.size(); k++) 
                    {
                        Parameter fp = (Parameter)md.parameters.get(k);
                        if(k > 0) outc.print(", ");
                        outc.print(fp.type.passLocalVariableAsParameter(md.name + "_" + fp.name));
                    }
                    outc.println(");");

                    outc.println("  nmfTraceActivity(TRACE_ACTIVITY_END, (t_uint24)target.THIS, (t_uint24)target." + md.name + ");");
                    outc.println("}");
                    outc.println();
                }

                outc.close();
            }

            /* 
             * Fill the type
             */
            // Must be declared in first since hardcoded in CM_SYSCALL_AllocEvent
            type.addRequire(new astrequire(null, 0, 0, itf.name, "target"));

            tools.idl.idlcompiler.Compile("rtos.api.ee", null);
            tools.idl.idlcompiler.Compile("communication.api.communication", null);
            type.addRequire(new astrequire(null, 0, 0, "rtos.api.ee", "ee"));
            type.addRequire(new astrequire(null, 0, 0, "communication.api.communication", "eecom"));

            Interface itfcom = bc.getItfCom(itf);
            astprovide vb = new astprovide(null, 0, 0, itfcom.name, "coms");
            vb.virtualInterface = true;
            type.addProvide(vb);

            type.addAttribute(new astattribute(null, 0,0,"TOP"));
            type.addAttribute(new astattribute(null, 0,0,"FIFO"));

            type.sources.add(source);

            type.setOption(new astoption(OptionNames.CFLAGS, "-g -O3"));
            if(stack > 32)
                type.setOption(new astoption(OptionNames.stack, Integer.toString(stack)));

            if(needToPutInDDR)
                type.setOption(new astoption(OptionNames.memories, "EXT24RAM"));

            confcompiler.registerComponent(type);
        }

        return type;
    }
    
    protected int create32(PrintStream out, String typein, String typeout, int packedsize, boolean signed, String nb16)
    {
	    if (packedsize <= 8) 
	    {
            if(signed)
                out.print("(" + typeout + ")_xyuv_data[" + nb16 + "]");
            else
                out.print("(" + typeout + ")(_xyuv_data[" + nb16 + "] & 0xFF)");
            return 1;
        }
	    else if (packedsize <= 16) 
	    {
		    if(signed)
		        out.print("(" + typeout + ")_xyuv_data[" + nb16 + "]");
		    else
		        out.print("(" + typeout + ")(_xyuv_data[" + nb16 + "] & 0xFFFF)");
			return 1;
		} 
	    else if (packedsize <= 32) 
	    {
			out.print("(" + typeout + ")ext16to32((" + typein + ")_xyuv_data[" + nb16 + "+1], (" + typein + ")_xyuv_data[" + nb16 + "])");
			return 2;
		} 
		return 0;
	}

    public bc getPrimitive(astbasefile declaration, String name) throws compilerexception 
    {
        assert false: "Not implemented";
        return null;
    }
}
