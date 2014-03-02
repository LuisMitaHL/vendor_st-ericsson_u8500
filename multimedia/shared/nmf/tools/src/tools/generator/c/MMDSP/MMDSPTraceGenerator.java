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

import tools.common.astbasefile;
import tools.common.compilerexception;
import tools.common.configuration;
import tools.conf.ast.astoption;
import tools.conf.ast.astprimitive;
import tools.conf.ast.astprovide;
import tools.conf.ast.astrequire;
import tools.conf.ast.astsource;
import tools.conf.ast.astoption.OptionNames;
import tools.idl.ast.*;

public class MMDSPTraceGenerator implements tools.generator.api.TraceGeneratorInterface 
{
    Interface itf;
    
    public MMDSPTraceGenerator()
    {
        
    }
    
    public MMDSPTraceGenerator(Interface _itf) 
    {
        setItf(_itf);
    }
    
    public void setItf(Interface _itf) {
        this.itf = _itf;
    }
    
    public tools.conf.graph.binding.bc getPrimitive(astbasefile _declaration) throws compilerexception, IOException 
    {
        //astprimitive type = getAst();
        //return new tools.generator.c.bc.trace(_declaration, type);
        return null;
    }

    public astprimitive getAst() throws compilerexception, IOException 
    {
        astprimitive type = new astprimitive(null, "_tr." + itf.name);

        String smallname = itf.name.substring(itf.name.lastIndexOf('.')+1);
        astsource source = new astsource(
                configuration.getComponentTemporaryFile(type, "tr-" + smallname + ".c"));
        PrintStream outc = configuration.needRegeneration(itf.lastModifiedFile, source.file);
        if(outc != null) 
        {
            outc.println("/* Generated xtitrace for '" + itf.name + "' */");
            outc.println("#include <_tr/" + itf.name.replace('.', '/') + ".nmf>");
            outc.println();

            for(int j = 0; j < itf.methods.size(); j++)
            {
                Method md = (Method)itf.methods.get(j);

                outc.println(md.printMethodMETH() + " {");
                outc.println("  nmfTraceActivity(TRACE_ACTIVITY_CALL, (t_uint24)target.THIS, (t_uint24)target." + md.name + ");");
                outc.print("  target." + md.name+ "(");
                for(int k = 0; k < md.parameters.size(); k++) 
                {
                    Parameter fp = (Parameter)md.parameters.get(k);
                    if(k > 0) outc.print(",");
                    outc.println();
                    outc.print("    " + fp.name);
                }
                outc.println(");");
                outc.println("  nmfTraceActivity(TRACE_ACTIVITY_RETURN, (t_uint24)target.THIS, (t_uint24)target." + md.name + ");");
                outc.println("}");
                outc.println();
            }
        }

        // Fill the type
        type.addProvide(new astprovide(null, 0, 0, itf.name, "target"));
        type.addRequire(new astrequire(null, 0, 0, itf.name, "target"));
        type.sources.add(source);

        tools.idl.idlcompiler.Compile("rtos.api.ee", null);
        type.addRequire(new astrequire(null, 0, 0, "rtos.api.ee", "ee"));

        type.setOption(new astoption(OptionNames.CFLAGS, "-g -O3"));

        return type;
    }
}
