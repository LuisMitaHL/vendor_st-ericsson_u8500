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
package tools.generator.c;

import java.io.PrintStream;
import java.util.HashSet;

import tools.conf.ast.*;
import tools.conf.graph.*;
import tools.idl.ast.*;
import tools.idl.generator.predefinemethod;
import tools.idl.idlcompiler;
import tools.common.compilerexception;
import tools.common.configuration;
import tools.common.compiler.ccompiler;
import tools.common.compiler.compilerfactory;
import tools.common.targetfactory;
import tools.common.targetfactory.DomainNames;
import tools.common.targetfactory.LinkType;

public class CWrapGenerator implements tools.generator.api.WrapGeneratorInterface 
{
    protected ccompiler cc;
    
    public CWrapGenerator() throws compilerexception 
    {
        cc = compilerfactory.getRawComponent();
    }
    
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
    
    /**
     * Generate interface reference according binding and tools configuration
     * @param primitive
     * @param externalRequireName
     * @param itf
     * @return
     */
    protected StringBuffer generateInterfaceReference(primitive primitive, interfacereference itfref, Interface itf) 
    {

        if(itfref == null || itfref.server == null)
        { 
            // Null reference or external null reference
            return InterfaceReferenceStatic.GenerateNull(itf);
        }
        else
        {
            // Required solved statically
            return InterfaceReferenceStatic.GenerateNotNull(itf, itfref);
        }
    }

    /**
     * Declare header of server interface
     * @param out
     * @param primitive
     * @param require
     * @param itf
     */
    protected void declareServerComponent(PrintStream out, 
            interfacereference itfref, Interface itf) 
    {
        
        if(itfref == null || itfref.server == null)
        { 
            // Null reference or external null reference; nothing to declare
        }
        else 
        {
            // Required solved statically
            astprovide provide = itfref.server.getProvide(itfref.itfname);
            predefinemethod.Generate(itf, 
                    itfref.server.type.fullyname_, 
                    (provide.collection ? itfref.collectionIndex : "") + provide.methprefix, 
                    false, 
                    out);

            if(targetfactory.domain == DomainNames.MMDSP)
            {
                out.println("extern int _sb_bases_section;");
            }
        } 
    }
    
    public void generate(primitive primitive) 
    {
        astprimitive component = primitive.type;

        /*
         * Generated component .nmf file definition
         */
        PrintStream out = configuration.forceRegeneration(
                configuration.getComponentTemporaryExt(component, "nmf"));
        boolean itdeclared = false;
        out.println("/* '" + component.fullyname_ + "' component */");
        out.println("#if !defined(" + component.fullyname_ + "_NMF)");
        out.println("#define " + component.fullyname_ + "_NMF");
        out.println();

        out.println("#include <inc/type.h>");
        out.println("#include <c.h>");
        generateInclude(out, primitive);
        out.println();

        // Generate all provided meta-data
        for (astprovide provide :  primitive.getProvides())
        {
            Interface itf = idlcompiler.getPreviouslyCompile(provide.type);
            out.println("// '" + provide.name + "' provided interface");
            if (targetfactory.linktype == LinkType.EXECUTABLE && provide.interrupt)
            {
                // static deployment and interrupt routine
                // We need to wrap interrupt handler so it's connected statically with executive engine
                if(! itdeclared)
                {
                    out.println("#define EE_INTERRUPT static inline");
                    itdeclared = true;
                }
                out.println("EE_INTERRUPT void " + primitive.type.fullyname_ + "_" + 
                        provide.methprefix + itf.methods.get(0).name + "(void);");
                out.println("#pragma force_dcumode");
                out.println("_INTERRUPT void IT" + provide.line + "(void) {");
                out.println("   " + primitive.type.fullyname_ + "_" + 
                        provide.methprefix + itf.methods.get(0).name + "();");
                out.println("}");
            }
            else
            {
                // dynamic deployment or (static deployment and not interrupt)
                if(provide.interrupt) 
                {
                    if(! itdeclared) 
                    {
                        out.println("#define EE_INTERRUPT");
                        itdeclared = true;
                    }
                }
                
                /*
                 * Generate method header
                 */
                if(provide.unambiguous)
                    predefinemethod.GenerateUnanbiguous(itf, out);
                else
                    predefinemethod.Generate(itf, 
                            component.fullyname_, provide.methprefix, 
                            provide.collection, out);
                
                /*
                 * Declare referencable interface descriptor
                 */
                if(provide.referenced)
                {
                    if(provide.collection)
                    {
                        cc.declareInterface(out, "I" + itf.name_,
                                component.fullyname_ + "_" + provide.name + "[" + provide.numberOfElement + "]");
                        out.println("#define " + provide.name + " " +  component.fullyname_ + "_" + provide.name);
                    } 
                    else
                    {
                        cc.declareInterface(out, "I" + itf.name_,
                                component.fullyname_ + "_" + provide.name);
                        out.println("#define " + provide.name + " " +  component.fullyname_ + "_" + provide.name);
                    }
                }
            }
            out.println();
        }

        // Generate all required interfaces
        for (astrequire require : primitive.getRequires())
        {
            Interface itf = idlcompiler.getPreviouslyCompile(require.type);
            out.println("// '" + require.name + "' required interface");
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
            else if(isRequiredResolvedStatically(primitive, require)) 
            {
                // Intra composite binding (generate code that can be optimized)
                if(require.collection) 
                {
                    // Declare header of server interface
                    for(int n = 0; n < require.numberOfElement; n++) 
                    {
                        interfacereference ir = primitive.lookupFcWithoutError(require.name + "[" + n + "]");
                        declareServerComponent(out, ir, itf);
                    }

                    // Declare interface reference
                    out.print("static ");
                    if(targetfactory.domain != DomainNames.MMDSP)
                    {
                        // note: the const seam to break the MMDSP relocation mechanism which reference shared from private segment.
                        out.print("const ");
                    }
                    out.println("I" + itf.name_ + " " + require.name + 
                            "[" + require.numberOfElement + "] = {       // Collection");
                    for(int n = 0; n < require.numberOfElement; n++) 
                    {
                        interfacereference ir = primitive.lookupFcWithoutError(require.name + "[" + n + "]");
                        StringBuffer sb = generateInterfaceReference(primitive, ir, itf); 
                        out.println("  "+ sb.toString() + ",");
                    }
                    out.println("};");
                    out.println("#define " + require.name + "_N " +  require.numberOfElement);
                } 
                else 
                {
                    // Declare header of server interface
                    interfacereference ir = primitive.lookupFcWithoutError(require.name);
                    declareServerComponent(out, ir, itf);

                    // Declare interface reference
                    out.print("static ");
                    if(targetfactory.domain != DomainNames.MMDSP)
                    {
                        // note: the const seam to break the MMDSP relocation mechanism which reference shared from private segment.
                        out.print("const ");
                    }
                    out.println("I" + itf.name_ + " " + require.name + " = ");
                    StringBuffer sb = generateInterfaceReference(primitive, ir, itf); 
                    out.println("  "+ sb.toString() + ";");
                }
            } 
            else
            {
                // Extra composite binding
                if(require.collection) 
                {
                    cc.declareInterface(out, "I" + itf.name_,
                            component.fullyname_ + "_" + require.name + "[" + require.numberOfElement + "]");
                    out.println("#define " + require.name + "_N " +  require.numberOfElement);
                    out.println("#define " + require.name + " " +  component.fullyname_ + "_" + require.name);
                } 
                else
                {
                    cc.declareInterface(out, "I" + itf.name_,
                            component.fullyname_ + "_" + require.name);
                    out.println("#define " + require.name + " " +  component.fullyname_ + "_" + require.name);
                }
            }
            out.println();
        }

        // Declare properties
        for (final astproperty property : primitive.getProperties()) 
        {
            out.println("// '" + property.name + "' property");
            out.println("#define " + property.name + " " + property.getCValue());
            out.println();
        }

        // Declare Attributes
        for (astattribute attribute : primitive.getAttributes()) 
        {
            if(attribute.ctype != null)
            {
                out.println("// '" + attribute.name + "' attribute");
                out.println("extern " + attribute.ctype + " " + component.fullyname_ + "_" + attribute.name + ";");
                out.println();
            }
        }

        out.println("#define METH(x) " +  component.fullyname_ + "_ ## x");
        out.println("#define ATTR(x) " +  component.fullyname_ + "_ ## x");
        out.println();

        out.println("#endif");

        out.close();

        
        
        /*
         * Generated component C definition
         */
        out = configuration.forceRegeneration(
                configuration.getComponentTemporaryFile(component, component.smallname + "-wrp.c"));
        out.println("/* Primitive component " + component.fullyname + " */");
        out.println("#include <inc/type.h>");      
        generateInclude(out, primitive);
        out.println();

        HashSet<String> methodForCollection = new HashSet<String>();

        for(astprovide provide : primitive.getProvides()) 
        {
            Interface itf = idlcompiler.getPreviouslyCompile(provide.type);

            if(provide.collection) 
            {
                /*
                 * Declare provided interface methods for collection 
                 */
                out.println("// '" + provide.name + "' provided interface");

                predefinemethod.Generate(itf, 
                        primitive.type.fullyname_, 
                        provide.methprefix, 
                        provide.collection, 
                        out);

                for(int i = 0; i < provide.numberOfElement; i++) 
                {
                    for(int j = 0; j < itf.methods.size(); j++)
                    {
                        Method md = (Method)itf.methods.get(j);
                        String methodName = primitive.type.fullyname_ + "_" + i + provide.methprefix + md.name;
                        if(! methodForCollection.contains(methodName)) 
                        {
                            methodForCollection.add(methodName);
                            out.println(md.printMethod(primitive.type.fullyname_, i + provide.methprefix, false, false, false) + " {");
                            out.print("  ");
                            if(! (md.type instanceof TypeVoid))
                                    out.print("  return ");
                            out.print(primitive.type.fullyname_ + "_" + provide.methprefix + md.name + "(");
                            if(md.parameters.size() != 0) 
                            {
                                for(int k = 0; k < md.parameters.size(); k++)
                                {
                                    if(k >= 1) out.print(", ");
                                    Parameter fp = md.parameters.get(k);
                                    out.print(fp.name);
                                }
                                out.print(", " + i);
                            } else {
                                out.print(i);
                            }
                            out.println(");");
                            out.println("}");
                        }
                    }
                }
                out.println();
            } 

            if(provide.referenced)
            {
                /*
                 * Declare referencable interface descriptor
                 */
                StringBuffer sb;
                if(provide.collection) 
                {
                    // Declare header of server interface
                    for(int n = 0; n < provide.numberOfElement; n++)
                    {
                        interfacereference ir = primitive.lookupFcWithoutError(provide.name + "[" + n + "]");
                        declareServerComponent(out, ir, itf);
                    }

                    out.println("I" + itf.name_ + " " + 
                            primitive.type.fullyname_ + "_" + provide.name + 
                            "[" + provide.numberOfElement + "] = {       // Collection");
                    for(int n = 0; n < provide.numberOfElement; n++) 
                    {
                        interfacereference ir = primitive.getFcInterface(provide.name + "[" + n + "]");
                        sb = generateInterfaceReference(primitive, ir, itf);
                        out.println("  "+ sb.toString() + ",");
                    }
                    out.println("};");
                } else
                {
                    // Declare header of server interface (only relevant since external binding)
                    interfacereference ir = primitive.getFcInterface(provide.name);
                    declareServerComponent(out, ir, itf);

                    out.println("I" + itf.name_ + " " + 
                            primitive.type.fullyname_ + "_" + provide.name + " = ");
                    sb = generateInterfaceReference(primitive, ir, itf); 
                    out.println("  "+ sb.toString() + ";");
                }
                out.println();
            }
        }

        // Declare interface reference and configure them
        for (astrequire require : primitive.getRequires()) 
        {
            Interface itf = idlcompiler.getPreviouslyCompile(require.type);
            StringBuffer sb;

            out.println("// Interface reference for '" + primitive.type.fullyname + "'." + require.name + 
                    (require.optional ? " Optional" : "") + 
                    (require.isStatic ? " Static" : "") + 
                    (require.virtualInterface ? " Virtual" : ""));
            if(require.virtualInterface)
            {
                // Nothing to generate
            } 
            else if(require.isStatic) 
            {
                // Nothing to declare for static interface reference
            }
            else if(isRequiredResolvedStatically(primitive, require)) 
            {
                // Nothing to declare, static binding already done in .nmf
            }
            else 
            {
                if(require.collection)
                {
                    // Declare header of server interface
                    for(int n = 0; n < require.numberOfElement; n++)
                    {
                        interfacereference ir = primitive.lookupFcWithoutError(require.name + "[" + n + "]");
                        declareServerComponent(out, ir, itf);
                    }

                    out.println("I" + itf.name_ + " " + 
                            primitive.type.fullyname_ + "_" + require.name + 
                            "[" + require.numberOfElement + "] = {       // Collection");
                    for(int n = 0; n < require.numberOfElement; n++) 
                    {
                        interfacereference ir = primitive.lookupFcWithoutError(require.name + "[" + n + "]");
                        sb = generateInterfaceReference(primitive, ir, itf);
                        out.println("  "+ sb.toString() + ",");
                    }
                    out.println("};");
                } 
                else 
                {
                    // Declare header of server interface (only relevant since external binding)
                    interfacereference ir = primitive.lookupFcWithoutError(require.name);
                    declareServerComponent(out, ir, itf);

                    out.println("I" + itf.name_ + " " + 
                            primitive.type.fullyname_ + "_" + require.name + " = ");
                    sb = generateInterfaceReference(primitive, ir, itf); 
                    out.println("  "+ sb.toString() + ";");
                }
            }
            out.println();
        }

        // Declare Attributes and configure them
        for (astattribute attribute : primitive.getAttributes())
        {

            if(attribute.ctype == null) 
                continue;

            out.println("// '" + primitive.type.fullyname + "'." + attribute.name + " attribute");
            out.print(attribute.ctype + " " + primitive.type.fullyname_ + "_" + attribute.name);
            String value = primitive.getAttributeValue(attribute.name);
            if(value != null)
                out.print(" = " + value);
            out.println(";");
            out.println();
        }
    }
}
