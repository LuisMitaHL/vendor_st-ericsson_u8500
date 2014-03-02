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
/*
 * Generated ELF header component description
 */
package tools.generator.cpp.ee;

import java.io.PrintStream;

import tools.common.compilerexception;
import tools.common.configuration;
import tools.common.compiler.ccompiler;
import tools.common.configuration.TypeHeader;
import tools.conf.ast.astprovide;
import tools.conf.ast.astrequire;
import tools.conf.graph.*;
import tools.conf.graph.computer.lifecycle;
import tools.generator.api.MetaDataGeneratorInterface;

public class CppEEMetaDataGenerator implements MetaDataGeneratorInterface 
{
    PrintStream outhdr;

    final String MAGIC_COMPONENT = "0x123";

    final int MAX_TEMPLATE_NAME_LENGTH = 128;

    private void genASM32(String value, String comment)
    {
        if(comment != null)
            outhdr.println("        DCD      " + value + " ; " + comment);
        else
            outhdr.println("        DCD      " + value);
    }
    
    private void genString(String name, int len) 
    {
        outhdr.println("        DCB     \"" + name + "\", 0");
        if(len - name.length() - 1 > 0)
            outhdr.println("        SPACE   " + (len - name.length() - 1));
    }

    public void Generate(component component, lifecycle lccstate) throws compilerexception 
    {
        outhdr = configuration.forceRegeneration(
                configuration.getComponentTemporaryFile(component.type, component.type.smallname + "-hdr." + ccompiler.S), 
                TypeHeader.RvctAsm);
        if(outhdr != null)
        {
            outhdr.println("        AREA nmf_header, NOALLOC");
            // ASSOC=NMF, 
            // SECTYPE=0x8000BEEF, 
            outhdr.println();
            
            for(astprovide provide : component.getProvides()) 
            {
                if(provide.isNMFLifeCycleInterface())
                    continue;
                String elfsymbolname = "P::" + provide.name + "::" + 
                        String.format("%03o", provide.collection ? provide.numberOfElement : 0) + "::" + 
                        provide.type;
                outhdr.println("        EXPORT |" + elfsymbolname + "| [PROTECTED,ELFTYPE=0]");
                outhdr.println("|" + elfsymbolname + "|");
            }
            outhdr.println();

            for(astrequire require : component.getRequires()) 
            {
                if(require.isNMFLifeCycleInterface())
                    continue;
                String elfsymbolname = "R::" + require.name + "::" + 
                        String.format("%03o", require.collection ? require.numberOfElement : 0) + "::" + 
                        require.type;
                outhdr.println("        EXPORT |" + elfsymbolname + "| [PROTECTED,ELFTYPE=0]");
                outhdr.println("|" + elfsymbolname + "|");
            }
            outhdr.println();

            outhdr.println("        END");
            
            /*
            outhdr.println("NmfHeader");
            genASM32(MAGIC_COMPONENT, "Magic Number");
            genASM32("0x" + Integer.toHexString((version.major << 16) | (version.minor << 8) | (version.patch)), "NMF Version");
            genString(component.type.fullyname, MAX_TEMPLATE_NAME_LENGTH);
            outhdr.println();
   
           // outhdr.println("        EXPORT NmfHeader");
            outhdr.println("        KEEP NmfHeader");
*/
            /*
            outhdr.println("#include <ee/inc/nmfheaderabi.h>");
            outhdr.println();
            
            outhdr.println("const t_elf_component_header NmfHeader __attribute__ ((section (\".nmf_header\"))) = {");
            outhdr.println("    MAGIC_COMPONENT, ");
            outhdr.println("    0x" + Integer.toHexString((version.major << 16) | (version.minor << 8) | (version.patch)) + ", ");
            outhdr.println("    \"" + component.type.fullyname + "\", ");
            outhdr.println("};");
*/

            outhdr.close();
        }
    }
}
