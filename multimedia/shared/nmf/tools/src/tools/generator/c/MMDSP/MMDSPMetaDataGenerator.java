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
package tools.generator.c.MMDSP;

import java.io.PrintStream;
import java.util.*;

import tools.version;
import tools.common.compilerexception;
import tools.common.configuration;
import tools.common.targetfactory;
import tools.common.compiler.ccompiler;
import tools.common.targetfactory.LinkType;
import tools.conf.ast.*;
import tools.conf.graph.*;
import tools.conf.graph.component.Bases;
import tools.conf.graph.computer.lifecycle;
import tools.conf.graph.computer.stack;
import tools.generator.api.MetaDataGeneratorInterface;
import tools.idl.ast.*;
import tools.idl.idlcompiler;

public class MMDSPMetaDataGenerator implements MetaDataGeneratorInterface 
{
    final int COLLECTION_REQUIRE = 1; 
    final int OPTIONAL_REQUIRE = 2;
    final int STATIC_REQUIRE = 4;
    final int VIRTUAL_REQUIRE = 8;
    final int INTRINSEC_REQUIRE = 16;

    final int COLLECTION_PROVIDE = 1; 
    final int VIRTUAL_PROVIDE = 2;
    
    final String MAGIC_COMPONENT = "0x123";
    final String MAGIC_SINGLETON = "0x321";
    final String MAGIC_FIRMWARE = "0x456";
    
    String CNP = "_"; /* Prefix of geenrated C Name */
    
    PrintStream outhdr;

    ArrayList<String> values = new ArrayList<String>();

    /*
     * Helper methods
     */
    private void genComment(String comment) {
        outhdr.println("/* " + comment + " */");
    }
    private void genASM32(String value, String comment) {
        if(comment != null)
            outhdr.println("\t.word " + value + " /* " + comment + " */");
        else
            outhdr.println("\t.word " + value);
    }
    private void genASM32(String value) {
        outhdr.println("\t.word " + value);
    }
    private void genASM16(String value) {
        outhdr.println("\t.hword " + value);
    }
    private void genASM8(String value) {
        outhdr.println("\t.byte " + value);
    }
    private void genASM8(String value, String comment) {
        outhdr.println("\t.byte " + value + " /* " + comment + " */");
    }
    
    private void genImport(String symbol) {
        outhdr.println("\t.extern _" + symbol);
    }

    private HashMap<String, String> globalString = new HashMap<String, String>();
    private void referenceString(String str) {
        String label = "N" + str.replace('/', '_').replace('.', '_').replace('?', '_');
        int number = 0;
        String existing;
        do {
            existing = globalString.get(label + number);
            if(existing == null) 
                break;                  // Label not already registered
            if(existing.equals(str))    
                break;                  // Label correspond to same string
            number++;                   // Choose another label
        } while(true);
        globalString.put(label + number, str);
        genASM32(label + number);
    }
    
    /**
     * Import symbols visitor
     * @author fassino
     *
     */
    class Counter extends visitor {
        int NumberOfAttributes = 0;
        int NumberOfProperties = 0;
        protected void travelCommon(component component, Object data) throws compilerexception {
            // Import property -> nothing to do here, only here for counting
            for(final astproperty property : component.getProperties()) {
                NumberOfProperties++;
            }
        }
        protected void travelPrimitive(primitive primitive, Object data) throws compilerexception {
            // Import Attribute symbols with pathname
            for (astattribute attribute : primitive.getAttributes()) {
                NumberOfAttributes++;
            }
       }
    }
    
    /**
     * Export attributes
     * @author fassino
     *
     */
    class exportAttributes extends visitor {
        protected void travelPrimitive(primitive primitive, Object data) throws compilerexception {
            // Declare Attributes with pathname
            for (astattribute attribute : primitive.getAttributes()) {
                String pathname = primitive.getPathName();
                if(pathname != null)
                    referenceString(pathname + "/" + attribute.name);
                else 
                    referenceString(attribute.name);
                genASM32(CNP + primitive.type.fullyname_ + "_" + attribute.name);
            }
        }
    }

    /**
     * Export properties
     * @author fassino
     *
     */
    class exportProperties extends visitor {
        protected void travelCommon(component component, Object data) throws compilerexception {
            // Declare Properties with pathname
            for(final astproperty property : component.getProperties()) {
                String pathname = component.getPathName();
                if(pathname != null)
                    referenceString(pathname + "/" + property.name);
                else 
                    referenceString(property.name);
                String value = property.getBinValue();
                values.add(value);
                genASM32("V" + values.indexOf(value));
            }
        }
    }

    /**
     * 
     * @author fassino
     *
     */
    class exportRequiresCounter extends visitor {
        int NumberOfClient = 0;
        String name;
        public exportRequiresCounter(String _name) {
            name = _name;
        }
        protected void travelPrimitive(primitive primitive, Object data) throws compilerexception {
            for (astrequire require : primitive.getRequires()) 
            {
                for(int i = 0; i < require.numberOfElement; i++) {
                    String itfname = require.getInterfaceName(i);
                    interfacereference ir = primitive.lookupFcWithoutError(itfname);
                    if (ir != null && 
                            ir.server == null && 
                            ir.getInterfaceName().equals(name)) {
                        NumberOfClient++;
                    }
                }
            }
        }
    }
    class exportRequires extends visitor {
        String name;
        public exportRequires(String _name) {
            name = _name;
        }
        protected void travelPrimitive(primitive primitive, Object data) throws compilerexception {
            for (astrequire require : primitive.getRequires()) 
            {
                for(int i = 0; i < require.numberOfElement; i++) {
                    String itfname = require.getInterfaceName(i);
                    interfacereference ir = primitive.lookupFcWithoutError(itfname);
                    if (ir != null && 
                            ir.server == null && 
                            ir.getInterfaceName().equals(name)) { // This is a external binding
                        Interface itf = idlcompiler.getPreviouslyCompile(require.type);
                        genASM32(CNP + primitive.type.fullyname_ + "_" + require.name + "+" + (i * (itf.methods.size()+1)));
                    }
                }
            }
        }
    }


    /**
     * 
     */
    public void Generate(component component, lifecycle lccstate) throws compilerexception {
        outhdr = configuration.forceRegeneration(
                configuration.getComponentTemporaryFile(component.type, component.type.smallname + "-hdr." + ccompiler.S));
        if(outhdr == null) 
            return;
        
        if(targetfactory.linktype == LinkType.EXECUTABLE) 
        {
            if(! targetfactory.staticPicCompilation)
            {
                // We compile the EE: Reserve address for the THIS & Load map
                outhdr.println(".section .this_section, \"a\"");
                outhdr.println(".global THIS");
                outhdr.println("THIS:");
                outhdr.println("    .zero 1");
                outhdr.println(".global _LOADMAP");
                outhdr.println("_LOADMAP:");
                outhdr.println("	.zero 1");
            }
            else
            {
                // We compile for the MMDSP
                // PATCH: Do not generate nmf_segment section in this case, this break simulation
                return;
            }
        }
        else 
        {
            // Declare RAM Bases in  XRAM (must be at beginning)
            outhdr.println(".section .bases_section, \"wa\""); 
            for (Bases base : Bases.values()) {
                if(base.elfSymbolPointer == null && component.isBaseSetted(base)) {
                    outhdr.println("    .data " + base.elfSymbolStart + " /* " + base + "*/");
                }
            }
            outhdr.println();
            
            // Declare ROM bases in XROM (could be everywhere in  the segment)
            outhdr.println(".section .this_section, \"a\"");
            for (Bases base : Bases.values()) {
                if(base.elfSymbolPointer != null && component.isBaseSetted(base)) {
                    outhdr.println(".global " + base.elfSymbolPointer);
                    outhdr.println(base.elfSymbolPointer + ": .data " + base.elfSymbolStart + " /* " + base + "*/");
                }
            }
            outhdr.println();
        }

        outhdr.println(".section nmf_segment, \"\"");
        outhdr.println(".align 2");
        outhdr.println();

        int NumberOfProvide = 0;
        int NumberOfRequire = component.getRequireSize();

        /*
         * Count different think
         */
        Counter eas = new Counter();
        eas.browseComponent(component);
        
        for(astprovide provide : component.getProvides()) {
            // Count provide (while removing lifecycle interface)
            if(provide.isNMFLifeCycleInterface())
                continue;
            NumberOfProvide++;
        }
        
        /*
         * Component Header
         */
        genComment("t_elf_component_header");
        outhdr.println("ENTRY:");
        genASM32(targetfactory.linktype == LinkType.EXECUTABLE ? MAGIC_FIRMWARE : (component.type.singleton ? MAGIC_SINGLETON : MAGIC_COMPONENT), "Magic Number");
        genASM32("0x" + Integer.toHexString((version.major << 16) | (version.minor << 8) | (version.patch)), "NMF Version");
        referenceString(component.type.fullyname);
        
        if(targetfactory.linktype == LinkType.EXECUTABLE) {
            genASM32((lccstate.needConstuctor) ? CNP + "construct" : "0xFFFFFFFF", "Constructor");
            genASM32((lccstate.needStarter) ? CNP + "start" : "0xFFFFFFFF", "Starter");
            genASM32((lccstate.needStopper) ? CNP + "stop" : "0xFFFFFFFF", "Stopper");
            genASM32((lccstate.needDestructor) ? CNP + "destroy" : "0xFFFFFFFF", "Destructor");

        } else {
            genASM32((lccstate.needConstuctor) ? CNP + component.type.fullyname_ + "_construct" : "0xFFFFFFFF", "Constructor");
            genASM32((lccstate.needStarter) ? CNP + component.type.fullyname_ + "_start" : "0xFFFFFFFF", "Starter");
            genASM32((lccstate.needStopper) ? CNP + component.type.fullyname_ + "_stop" : "0xFFFFFFFF", "Stopper");
            genASM32((lccstate.needDestructor) ? CNP + component.type.fullyname_ + "_destroy" : "0xFFFFFFFF", "Destructor");
        }
        
        stack st = new stack();
        st.browseComponent(component);
        genASM32(String.valueOf(st.minstack), "Stack");

        genASM32(String.valueOf(eas.NumberOfAttributes), "Attributes");
        genASM32((eas.NumberOfAttributes > 0) ? "ATTRIBUTES" : "0");
        genASM32(String.valueOf(eas.NumberOfProperties), "Properties");
        genASM32((eas.NumberOfProperties > 0) ? "PROPERTIES" : "0");
        genASM32(String.valueOf(NumberOfProvide), "Provides");
        genASM32((NumberOfProvide > 0) ? "PROVIDES" : "0");
        genASM32(String.valueOf(NumberOfRequire), "Requires");
        genASM32((NumberOfRequire > 0) ? "REQUIRES" : "0");
        outhdr.println();
        
        /*
         * Export Attributes
         */
        if(eas.NumberOfAttributes > 0) {
            genComment("t_elf_attribute");
            outhdr.println("ATTRIBUTES:");
            (new exportAttributes()).browseComponent(component);
            outhdr.println();
        }

        /*Tracking defects for installation of Nomadik Toolset
         * Export Properties
         */
        if(eas.NumberOfProperties > 0) {
            genComment("t_elf_property");
            outhdr.println("PROPERTIES:");
            (new exportProperties()).browseComponent(component);
            outhdr.println();
        }

        LinkedHashSet<Interface> itfToDesribe = new LinkedHashSet<Interface>();
        
        /*
         * Export Provide interfaces
         */
        if(NumberOfProvide > 0) {
            genComment("t_elf_provided_interface");
            outhdr.println("PROVIDES:");
            for(astprovide provide : component.getProvides()) 
            {
                if(provide.isNMFLifeCycleInterface())
                    continue;
                Interface itf = idlcompiler.getPreviouslyCompile(provide.type);
                itfToDesribe.add(itf);
                outhdr.println("    /* " + provide.name + "<" + provide.type + ">*/");
                referenceString(provide.name);
                genASM8(String.valueOf(
                        (provide.collection ? COLLECTION_PROVIDE : 0) | 
                        (provide.virtualInterface ? VIRTUAL_PROVIDE : 0)), "Info");
                genASM8(provide.interrupt ? String.valueOf(provide.line) : "0", "Interrupt line");
                genASM8(String.valueOf(provide.numberOfElement), "Number of Element");
                genASM8("0", "Padding");
                genASM32("ITF" + provide.type.replace(".", "_"));
                for(int i = 0; i < provide.numberOfElement; i++) {
                    String itfname = provide.getInterfaceName(i);
                    interfacereference ir = component.getFcInterface(itfname);

                    astprovide serverprovide = ir.server.getProvide(ir.itfname);
                    for (int k = 0; k < itf.methods.size(); k++) {
                        Method md = (Method) itf.methods.get(k);
                        genASM32(CNP + (serverprovide.unambiguous ? "" : (ir.server.type.fullyname_ + "_")) + 
                                (serverprovide.collection ? ir.collectionIndex : "") + 
                                serverprovide.methprefix + md.name,
                                k == 0 ? "index=" +  i : null);
                    }
                }
            }
            outhdr.println();
        }
        
        /*
         * Export Requires
         */
       if(NumberOfRequire > 0) {
           genComment("t_elf_required_interface");
            outhdr.println("REQUIRES:");
            for (astrequire require : component.getRequires()) 
            {
                Interface itf = idlcompiler.getPreviouslyCompile(require.type);
                itfToDesribe.add(itf);
                outhdr.println("    /* " + require.name + "<" + require.type + ">*/");
                referenceString(require.name);
                genASM8(String.valueOf(
                        (require.collection ? COLLECTION_REQUIRE : 0) | 
                        (require.optional ? OPTIONAL_REQUIRE : 0) | 
                        (require.isStatic ? STATIC_REQUIRE : 0) |
                        (require.virtualInterface ? VIRTUAL_REQUIRE : 0) |
                        (require.isIntrinsec ? INTRINSEC_REQUIRE : 0)), "Info");
                genASM8(String.valueOf(require.numberOfElement), "Elements number");
                genASM8("0", "Padding");
                genASM8("0");
                genASM32("ITF" + require.type.replace(".", "_"));
                if(! require.isStatic && ! require.virtualInterface) {
                    for(int i = 0; i < require.numberOfElement; i++) {
                        String itfname = require.getInterfaceName(i);

                        exportRequiresCounter eric = new exportRequiresCounter(itfname);
                        eric.browseComponent(component);
                        genASM32(String.valueOf(eric.NumberOfClient), "NumberOfClient, index=" + i);

                        exportRequires eri = new exportRequires(itfname);
                        eri.browseComponent(component);
                    }
                } 
            }
            outhdr.println();
        }
        
        /*
         * Declare interfaces
         */
       genComment("t_elf_interface_description");
        for (final Interface itf : itfToDesribe) 
        {
            outhdr.println("ITF" + itf.name.replace(".", "_") + ":");
            referenceString(itf.name);
            genASM8(String.valueOf(itf.methods.size()));
            genASM8("0");
            genASM8("0");
            genASM8("0");
            for (int k = 0; k < itf.methods.size(); k++) {
                referenceString(itf.methods.get(k).name);
            }
        }
        outhdr.println();

        /*
         * Declare Strings and values
         */
        genComment("Strings");
        for (final String label : globalString.keySet()) 
        {
            outhdr.println(label + ":");
            outhdr.println("\t.string \"" + globalString.get(label) + "\\0\"");
        }
        outhdr.println();

        genComment("Values");
        for (int i = 0; i < values.size(); i++) {
            String value = values.get(i);
            outhdr.println("V" + i + ":");
            outhdr.println("\t.string \"" + value + "\\0\"");
        }
        outhdr.close();
    }
}
