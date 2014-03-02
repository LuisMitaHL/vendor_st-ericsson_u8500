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
package tools.generator.cpp;

import java.io.PrintStream;
import java.util.HashSet;

import tools.conf.ast.*;
import tools.conf.ast.astsource.SourceType;
import tools.conf.graph.*;
import tools.idl.ast.*;
import tools.idl.generator.predefinemethod;
import tools.idl.idlcompiler;
import tools.common.compilerexception;
import tools.common.configuration;
import tools.common.compiler.ccompiler;
import tools.common.compiler.compilerfactory;

public class CppWrapGenerator implements tools.generator.api.WrapGeneratorInterface 
{
    
    protected void generateInclude(PrintStream out, primitive primitive) 
    {
        HashSet<Interface> incls = new HashSet<Interface>();

        for (astprovide provide : primitive.getProvides())
        {
            Interface itf = idlcompiler.getPreviouslyCompile(provide.type);
            predefinemethod.GenerateInclude(itf, out, incls);
        }
        for (astrequire require : primitive.getRequires()) 
        {
            Interface itf = idlcompiler.getPreviouslyCompile(require.type);
            predefinemethod.GenerateInclude(itf, out, incls);
        }
    }

    protected boolean isRequiredResolvedStatically(primitive primitive, astrequire require) 
    {
        if(require.collection) 
        {
            for(int n = 0; n < require.numberOfElement; n++) 
            {
                interfacereference ir = primitive.lookupFcWithoutError(require.name + "[" + n + "]");
                if (ir == null || ir.server == null)
                {
                    // Extra composite binding
                    return false;
                }
            }
        }
        else 
        {
            interfacereference ir = primitive.lookupFcWithoutError(require.name);
             if (ir == null || ir.server == null) 
             {
                // Extra composite binding
                  return false;
            }
        }
        return true;
    }
    
    public void generate(primitive primitive) throws compilerexception 
    {
        astprimitive ast = primitive.type;
        ccompiler cc = compilerfactory.getRawComponent();

        /*
         * Generated component .nmf file definition
         */
        PrintStream out = configuration.forceRegeneration(
                configuration.getComponentTemporaryExt(ast, "nmf"));
        out.println("/* '" + ast.fullyname_ + "' component */");
        out.println("#if !defined(" + ast.fullyname_ + "_NMF)");
        out.println("#define " + ast.fullyname_ + "_NMF");
        out.println();

        out.println("#include <inc/type.h>");
        out.println("#include <cpp.hpp>");
        generateInclude(out, primitive);
        out.println();

        // Generate class
        String templateName = ast.fullyname_ + ((ast.getSourceType() == SourceType.CPPCource) ? "Template" : "");
        String virtalPrefix = (ast.getSourceType() == SourceType.CPPCource) ? " = 0" : "";
        out.print("class " + templateName + ": public NMF::Primitive");
        HashSet<String> set = new HashSet<String>();
        for (astprovide provide : primitive.getProvides()) 
        {
            if(! provide.collection && "".equals(provide.methprefix)) 
            {
                if(! set.contains(provide.type))
                {
                    out.println(",");
                    out.println("  // '" + provide.name + "' provided interface");
                    out.print("  public " + provide.type.replace(".", "_") + "Descriptor");
                    set.add(provide.type);
                }
            }
        }
        out.println();
        out.println("{");
                
        // Generate all provided meta-data
        set = new HashSet<String>();
        for (astprovide provide : primitive.getProvides()) 
        {
            Interface itf = idlcompiler.getPreviouslyCompile(provide.type);
            out.println("  // '" + provide.name + "' provided interface");

            /*
             * Declare method that must be implemented by developers
             */
            if(provide.collection || ! "".equals(provide.methprefix)) 
            {
                out.println("  protected:");
                out.println("    class " + provide.name + "Wrapper" + ": public " + itf.name_ + "Descriptor {");
                out.println("      public:");
                out.println("        " + templateName + " *reference;");
                if(provide.collection) 
                    out.println("        int index;");
                for(Method md : itf.methods) 
                {
                    out.println("        virtual " + md.printMethodCpp("", false) + " {");
                    out.print("          ");
                    if(! (md.type instanceof TypeVoid))
                        out.print("return ");
                    out.print("reference->" + provide.methprefix + md.name + "(");
                    boolean second = false;
                    for (Parameter fp : md.parameters) 
                    {
                        if(second)
                            out.print(", ");
                        out.print(fp.name);
                        second = true;
                    }
                    if(provide.collection) 
                        out.print(", index");
                    out.println(");");
                    out.println("      }");
                }
                out.println("    };");
                for(Method md : itf.methods) 
                {
                    out.println("    virtual " + md.printMethodCpp(provide.methprefix, provide.collection) + virtalPrefix + ";");
                }   
                out.println("  public:");
                if(provide.collection) 
                {
                    out.println("    static const t_uint8 " + provide.name + "_N = " +  provide.numberOfElement + ";");
                } 
                out.println("    " + provide.name + "Wrapper _xywrap_" + provide.name + 
                        (provide.collection ?  "[" + provide.numberOfElement + "]" : "") + ";");
                out.println();
            } 
            else 
            {
                if(! set.contains(provide.type))
                {
                    out.println("  public:");
                    for(Method md : itf.methods) 
                    {
                        out.println("    virtual " + md.printMethodCpp(provide.methprefix, provide.collection) + virtalPrefix + ";");
                    }   
                    set.add(provide.type);
                }
            }
            
            if(provide.referenced) 
            {
                out.println("  protected:");
                out.println("    I" + itf.name_ + " " + provide.name + 
                        (provide.collection ?  "[" + provide.numberOfElement + "]" : "") + ";");
                out.println();
            }
            out.println();
        }

        // Generate all required interfaces
        boolean foundOneStaticRequire = false;
        for (astrequire require : primitive.getRequires()) 
        {
            Interface itf = idlcompiler.getPreviouslyCompile(require.type);
            
            if(require.virtualInterface) 
            {
                // Nothing to generate
            } 
            else if(require.isStatic) 
            {
                foundOneStaticRequire = true;
            } 
            else 
            {
                out.println("  // '" + require.name + "' required interface");
                
                // Extra composite binding
                out.println("  public:");
                if(require.collection) 
                {
                    out.println("    I" + itf.name_ + " " + require.name + "[" + require.numberOfElement + "];");
                    out.println("    static const t_uint8 " + require.name + "_N = " +  require.numberOfElement + ";");
                }
                else 
                {
                    out.println("    I" + itf.name_ + " " + require.name + ";");
                }

                out.println();
            }
        }

        // Declare properties
        for (astproperty property : primitive.getProperties()) 
        {
            out.println("    // '" + property.name + "' property");
            out.println("    #define " + property.name + " " + property.getCValue());
            out.println();
        }

        // Declare Attributes
        for (astattribute attribute : primitive.getAttributes()) 
        {
            if(attribute.ctype != null) 
            {
                out.println("  public:");
                out.println("    // '" + attribute.name + "' attribute");
                out.println("    " + attribute.ctype + " " + attribute.name + ";");
                out.println();
            }
        }

        /*
         * Declare constructor
         */
        out.println("  public:");
        out.println("    " + templateName + "(): Primitive(\"" + primitive.type.fullyname + "\") {");
        // Set provide 
        for (astprovide provide : primitive.getProvides()) 
        {
            if(provide.collection) 
            {
                for(int n = 0; n < provide.numberOfElement; n++)
                {
                    out.println("      _xywrap_" + provide.name + "[" + n + "].reference = this;");
                    out.println("      _xywrap_" + provide.name + "[" + n + "].index = " + n +";");
                    if(provide.referenced) 
                        out.println("      " + provide.name + " = (" + provide.type.replace(".", "_") + "Descriptor*)&_xywrap_" + provide.name + "[" + n + "];");
                }
            } 
            else if(! "".equals(provide.methprefix)) 
            {
                out.println("      _xywrap_" + provide.name + ".reference = this;");
                if(provide.referenced) 
                    out.println("      " + provide.name + " = (" + provide.type.replace(".", "_") + "Descriptor*)&_xywrap_" + provide.name + ";");
            } 
            else 
            {
                if(provide.referenced) 
                    out.println("      " + provide.name + " = (" + provide.type.replace(".", "_") + "Descriptor*)this;");
            }
        }
        out.println("    }");

        out.println("};");
        out.println();

        if(ast.getSourceType() == SourceType.CPPCource) 
        {
            out.println("#include <" + ast.fullyname.replace(".", "/") + "/inc/" +ast.smallname + ".hpp>");
            out.println();
        }

        out.println("#ifndef IN_NMF_ASSEMBLY");
        out.println("#define METH(x) " +  ast.fullyname_ + "::x");
        out.println("#define ATTR(x) x");
        out.println("#endif");
        out.println();

        // Declare static required interface
        if(foundOneStaticRequire) 
        {
            out.println("extern \"C\" {");
            for (astrequire require : primitive.getRequires()) 
            {
                Interface itf = idlcompiler.getPreviouslyCompile(require.type);

                if(require.virtualInterface)
                {
                    // Nothing to generate
                } 
                else if(require.isStatic)
                {
                    for(final Method md : itf.methods) 
                    {
                        cc.declareStaticInterfaceMethod(out, md.printMethodLibrary());
                    }
                }
                out.println();
            }
            out.println("}");
            out.println();
       }

        out.println("#endif");

        out.close();
    }
}
