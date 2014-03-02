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
package tools.conf.generator;

import java.io.File;
import java.io.PrintStream;
import java.util.ArrayList;

import tools.common.compilerexception;
import tools.common.configuration;
import tools.conf.ast.astbind;
import tools.conf.ast.astinterface;
import tools.conf.ast.astprovide;
import tools.conf.ast.astrequire;
import tools.conf.graph.*;
import tools.idl.idlcompiler;
import tools.idl.ast.Interface;

public class dotter extends configuration 
{

    public class dotterinstance extends visitor 
    {
        PrintStream out;

        public dotterinstance(PrintStream _out) 
        {
            out = _out;
        }

        protected String displayItf(String name)
        {
            return astinterface.getNameOfInterfaceCollection(name);
        }

        protected void travelRawPrimitive(primitiveraw primitive, Object data) throws compilerexception 
        {
            String dec = (String)data; 

            out.println(dec + primitive.getPathName_() + 
                    " [label=\"" + primitive.nameinowner /*+ "\\n<" + primitive.type.fullyname + ">:" + primitive.debugComment +*/ + "\"" +
                    ", shape=component, style=filled" +
                    ", fontsize=8" + 
                    (primitive.type.parsedFile != null ? ", URL=\"" + primitive.type.parsedFile.getAbsolutePath() + "\"" : "") + "]");

            for(astprovide provide : primitive.getProvides())
            {
                if(! provide.type.startsWith("lifecycle."))
                {
                    Interface itf = idlcompiler.getPreviouslyCompile(provide.type);
                    out.println(dec + primitive.getPathName_() + "_P" + provide.name + 
                            " [label = \"" + provide.name + "\", shape=point]");
                    out.println(dec + primitive.getPathName_() + "_P" + provide.name + " -> " + primitive.getPathName_() +" [" +
                            "fontsize=8, style = \"bold\", " + 
                            "label = \"" + provide.name + "\", " + 
                            "URL=\"" + itf.file.getAbsolutePath() + "\"" + 
                    "]");
                }
            }
/*
            for (astrequire require : primitive.getRequires()) 
            {
                Interface itf = idlcompiler.getPreviouslyCompile(require.type);

                for(int i = 0; i < require.numberOfElement; i++) 
                {
                    String itfname = require.getInterfaceName(i);
                    interfacereference ir = primitive.lookupFcWithoutError(itfname);

                    if (ir != null && ir.server != null)
                    {
                        out.print(primitive.getPathName_() + " -> ");
                        out.print(ir.server.getPathName_() + "_P" + displayItf(ir.itfname));
                        if(itf != null)
                        {
                            out.print(" [" +
                                    "URL=\"" + itf.file.getAbsolutePath() + "\"" + 
                                    "]");
                        }
                        out.println();
                    }
                }
            }
            */
        }
        
        class bindst{
            String from, to;
            int number; //  binding collection number
            public bindst(String _from, String _to)
            {
                from = _from ; to = _to; 
                number = 1;
            }
        }
        protected void addBind(ArrayList<bindst>list, String from, String to)
        {
            for(bindst b: list)
            {
                if(from.equals(b.from) && to.equals(b.to))
                {
                    b.number++;
                    return ;
                }
            }
            list.add(new bindst(from, to));
        }

        protected void travelComposite(composite composite, Object data) throws compilerexception 
        {
            String dec = (String)data; 

            if(((String)data).length() == 4)
            {
                out.println(dec + "subgraph cluster" + composite.getPathName_() + " {");
                out.println(dec + "  label = \"" + (
                        composite.nameinowner == null ? "" : (composite.nameinowner + "\\n")) + 
                        "<" + composite.type.fullyname + ">\"");
                out.println(dec + "  shape=box");
                out.println(dec + "  fontsize=9");
                out.println(dec + "  URL=\"" + composite.type.parsedFile.getAbsolutePath() + "\"");
            }

            if(((String)data).length() == 4)
                out.println(dec + "  { rank=source;");
            for(astprovide provide : composite.getProvides())
            {
                if(! provide.type.startsWith("lifecycle."))
                {
                    out.println(dec + composite.getPathName_() + "_P" + provide.name + 
                            " [label = \"" + provide.name + "\", shape=point]");
                }
            }
            if(((String)data).length() == 4)
                out.println(dec + "  }");
            if(((String)data).length() == 4)
                out.println(dec + "  { rank=sink;");
            for(astrequire require : composite.getRequires())
            {
                if(! require.type.startsWith("lifecycle."))
                {
                    out.println(dec + composite.getPathName_() + "_R" + require.name + 
                            " [label = \"" + require.name + "\", shape=point]");
                }
            }
            if(((String)data).length() == 4)
                out.println(dec + "  }");

            super.travelComposite(composite, dec + "  ");

            ArrayList<bindst>list = new ArrayList<bindst>();
            for(astbind bind : composite.getBinds())
            {
                String src, dst;
                
                if("this".equals(bind.from))
                    src = composite.getPathName_() + "_P" + astinterface.getNameOfInterfaceCollection(bind.fromitf);
                else
                {
                    component client = composite.getFcSubComponent(bind.from);
                    if(client instanceof composite)
                        src = client.getPathName_() + "_R" + astinterface.getNameOfInterfaceCollection(bind.fromitf);
                    else
                        src = client.getPathName_();
                }
                
                if("this".equals(bind.to))
                    dst = composite.getPathName_() + "_R" + astinterface.getNameOfInterfaceCollection(bind.toitf);
                else
                {
                    component server = composite.getFcSubComponent(bind.to);
                    dst = server.getPathName_() + "_P" + astinterface.getNameOfInterfaceCollection(bind.toitf);
                }

                addBind(list, src, dst);
            }
            
            for(bindst b: list)
            {
                out.println(dec + "" + b.from + " -> " + b.to + " [arrowhead=none" +
                        (b.number > 1 ? (",label =" + b.number + ",fontsize=7") : "") + 
                        "]");
            }


            if(((String)data).length() == 4)
            {
                out.println(dec + "}");
            }
        }
    }

    public void Generate(component component, String suffix) throws compilerexception 
    {
        File dotfile = getComponentTemporaryExt(component.type, suffix + "dot");
        PrintStream out = forceRegeneration(dotfile);

        out.println("/*");
        out.println(" Generate gif file through this dot command:");
        out.println(" dot " + dotfile.getPath() + " -Tgif -o" + component.type.smallname + ".gif" + " -Tcmapx -o" + component.type.smallname + ".map");
        out.println(" echo '<img src=\"" + component.type.smallname + ".gif\" usemap=\"#" + component.type.smallname + "\">' > " + component.type.smallname + ".html");
        out.println(" cat " + component.type.smallname + ".map >> " + component.type.smallname + ".html");
        out.println(" */");

        out.println("digraph " + component.type.smallname);
        out.println("{");       

        out.println("  rankdir=LR");
        out.println("  ranksep=0.2");
        
    (new dotterinstance(out)).browseComponent(component, "  ");

        out.println("}");
        out.close();
        
    }
}
