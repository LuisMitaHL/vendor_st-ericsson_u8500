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

import tools.common.astbasefile;
import tools.common.compilerexception;
import tools.common.configuration;
import tools.conf.confcompiler;
import tools.conf.ast.astattribute;
import tools.conf.ast.astoption;
import tools.conf.ast.astprimitive;
import tools.conf.ast.astprovide;
import tools.conf.ast.astrequire;
import tools.conf.ast.astsource;
import tools.conf.ast.astoption.OptionNames;
import tools.conf.graph.binding.bc;
import tools.generator.api.StubGeneratorInterface;
import tools.idl.ast.*;
import tools.idl.generator.stubclient;

public class MMDSPStubGenerator extends stubclient implements StubGeneratorInterface
{
    public astprimitive registerAst() throws compilerexception 
    {
        String compName = "_st." + itf.name;
        astprimitive type;

        type = (astprimitive)confcompiler.getPreviouslyCompile(compName);
        if(type == null)
        {
            type = new astprimitive(itf.lastModifiedFile, compName);

            String smallname = itf.name.substring(itf.name.lastIndexOf('.')+1);
            astsource source = new astsource(
                    configuration.getComponentTemporaryFile(type, "st-" + smallname + ".c"));
            PrintStream outc = configuration.needRegeneration(itf.lastModifiedFile, source.file);
            if(outc != null) 
            {
                outc.println("/* Generated dsp stub for '" + itf.name + "' */");
                outc.println("#include <_st/" + itf.name.replace('.', '/') + ".nmf>");
                outc.println("#include <inc/archi-wrapper.h>");
                outc.println();

                outc.println("t_nmf_fifo ATTR(FIFO) = (t_nmf_fifo)" + maxnb16 + ";");
                outc.println();

                for(int j = 0; j < itf.methods.size(); j++) 
                {
                    Method md = itf.methods.get(j);

                    /*
                     * Generate all method stubs for DSP side
                     */ 
                    outc.println("#pragma force_dcumode");
                    outc.println(md.printMethodMETH() + " {");
                    outc.println("  ENTER_CRITICAL_SECTION;");
                    outc.println("  {"); 
                    outc.println("    t_event_params_handle _xyuv_data = AllocEvent(ATTR(FIFO));");
                    for(int n = 0; n < needi[j]; n++)
                        outc.println("    int ___i" + n + ", ___j" + n +";");

                    // Marshall parameter
                    int nb16 = 0;
                    for(int k = 0; k < md.parameters.size(); k++) 
                    {
                        Parameter fp = md.parameters.get(k);
                        outc.println("    /* " + fp.name + " <" + fp.type.printType() + "> marshalling */");
                        nb16 += writeData(fp.type, fp.name, outc, Integer.toString(nb16), "    ", 0);
                    }

                    outc.println("    /* Event sending */");
                    outc.println("    PushEvent(ATTR(FIFO), _xyuv_data, " + j + ");");
                    outc.println("  }");
                    outc.println("  EXIT_CRITICAL_SECTION;");
                    outc.println("}");
                }
                outc.close();
            }

            // Fill the type
            tools.idl.idlcompiler.Compile("rtos.api.ee", null);
            tools.idl.idlcompiler.Compile("communication.api.communication", null);
            type.addRequire(new astrequire(null, 0, 0, "rtos.api.ee", "ee"));
            type.addRequire(new astrequire(null, 0, 0, "communication.api.communication", "eecom"));

            Interface itfcom = bc.getItfCom(itf);
            astrequire vb = new astrequire(null, 0, 0, itfcom.name, "coms");
            vb.virtualInterface = true;
            type.addRequire(vb);

            // Must be declared in first since hardcoded in CM_SYSCALL_AllocEvent
            type.addProvide(new astprovide(null, 0, 0, itf.name, "source"));

            type.addAttribute(new astattribute(null, 0, 0, "FIFO"));

            type.sources.add(source);

            type.setOption(new astoption(OptionNames.CFLAGS, "-g -O3"));

            confcompiler.registerComponent(type);
        }

        return type;
    }

    public bc getPrimitive(astbasefile declaration, String name) throws compilerexception 
    {
        assert false: "Not implemented";
        return null;
    }
}
