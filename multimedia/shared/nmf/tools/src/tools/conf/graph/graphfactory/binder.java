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
package tools.conf.graph.graphfactory;

import java.io.IOException;
import java.util.*;

import tools.common.*;
import tools.conf.ast.*;
import tools.conf.graph.*;
import tools.conf.graph.binding.*;
import tools.generator.api.EventGeneratorInterface;
import tools.generator.api.TraceGeneratorInterface;
import tools.idl.idlcompiler;
import tools.idl.ast.Interface;

public class binder extends visitor {
    
    protected void travelPrimitive(primitive primitive, Object data) throws compilerexception {
        // We've got nothing to do for primitive !
    }

    protected void travelComposite(composite composite, Object data) throws compilerexception 
    {
        super.travelComposite(composite, data);

        debug(Debug.comp, "Bind composite " + composite.getPathName());

        ArrayList<primitiveraw> ncs = new ArrayList<primitiveraw>();
        
        // Create binding
        int nbbc = 0;
        ArrayList<astbind> newb = new ArrayList<astbind>();
        for(final astbind bind : composite.getBinds()) {
            debug(Debug.comp, "  " + bind.from + "." + bind.fromitf + 
                    " -> " + bind.to + "." + bind.toitf + 
                    (bind.asynchronous ? " async" : "") + 
                    (bind.trace ? " trace" : ""));

            component client = composite.getFcSubComponent(bind.from);
            component server = composite.getFcSubComponent(bind.to);
            String clientmpc = client.getMPC();
            String servermpc = server.getMPC();

            String itftype;
            String toitfname = astinterface.getNameOfInterfaceCollection(bind.toitf);
            if(server == composite) {
                itftype = composite.getRequire(toitfname).type;
            } else {
                itftype = server.getProvide(toitfname).type;
            }

            // Create binding component if necessary
            try {
                if(clientmpc == null && servermpc != null)
                {
                    Interface itf = idlcompiler.Compile(itftype, bind);
                    
                    if(client != composite)
                    {
                        tools.generator.cpp.hostee.bc.proxystub bc = new tools.generator.cpp.hostee.bc.proxystub(bind,
                                itf,
                                client == composite ? null : client, bind.fromitf, 
                                        server, bind.toitf,
                                        bind.size);
                        bc.debugComment = bind.from + "." + bind.fromitf + "->" + bind.to + "." + bind.toitf;
                        ncs.add(bc);

                        // Add binding between client and stub
                        newb.add(new astbind(null, 0, 0, bind.from, bind.fromitf, bc.nameinowner, "target"));

                        // Replace binding between stub and remote component 
                        bind.from = bc.nameinowner;
                        bind.fromitf = "target";
                    }
                } 
                else if(clientmpc != null && servermpc == null) 
                {
                    /*
                     * DSP ->  Host Composite size
                     */
                    Interface itf = idlcompiler.Compile(itftype, bind);
                    
                    if(server != composite)
                    {
                        /*
                         * DSP -> HOST Component
                         * Binding: DSP -> skel -> event -> Component 
                         */
                        EventGeneratorInterface gen = targetfactory.getEvent();
                        gen.setItf(itf);
                        bcevent event =  gen.getPrimitive(
                                new astbasefile(bind.parsedFile, bind.line, bind.column), 
                                bind.size,
                                server.priority,
                                bind.subpriority);
                        event.debugComment = bind.from + "." + bind.fromitf + "->" + bind.to + "." + bind.toitf;
                        ncs.add(event);

                        // Add binding between event to server component
                        newb.add(new astbind(null, 0, 0, event.nameinowner, "target", bind.to, bind.toitf));

                        tools.generator.cpp.hostee.bc.proxyskel skel = new tools.generator.cpp.hostee.bc.proxyskel(bind, 
                                itf,
                                client, bind.fromitf, 
                                event, "target",
                                bind.size);
                        skel.debugComment = bind.from + "." + bind.fromitf + "->" + bind.to + "." + bind.toitf;
                        ncs.add(skel);

                        // Add binding between skeleton and event
                        newb.add(new astbind(null, 0, 0, skel.nameinowner, "target", event.nameinowner, "target"));

                        // Replace binding between client component and skeleton
                        bind.to = skel.nameinowner;
                        bind.toitf = "target";
                    }
                } 
                else if(clientmpc != null && servermpc != null) 
                {
                    tools.generator.cpp.hostee.bc.proxybind bc = new tools.generator.cpp.hostee.bc.proxybind(       
                            new astcontent(bind.parsedFile, bind.line, bind.column, "_virtual", "v" + (nbbc++)),
                            client, bind.fromitf, server, bind.toitf, 
                            bind.size, bind.asynchronous);
                    ncs.add(bc);
                } 
                else if(clientmpc == null && servermpc == null && bind.asynchronous) 
                {
                    Interface itf = idlcompiler.Compile(itftype, bind);
                    EventGeneratorInterface gen = targetfactory.getEvent();
                    gen.setItf(itf);
                    bcevent bc =  gen.getPrimitive(
                            new astbasefile(bind.parsedFile, bind.line, bind.column), 
                            bind.size,
                            server.priority,
                            bind.subpriority);
                    bc.debugComment = bind.from + "." + bind.fromitf + "->" + bind.to + "." + bind.toitf + " [" + bind.size + "]";
                    ncs.add(bc);

                    // Add binding between client sub component and binding component
                    newb.add(new astbind(null, 0, 0, bind.from, bind.fromitf, bc.nameinowner, "target"));
                    
                    // Replace binding client sub-component by binding component
                    bind.from = bc.nameinowner;
                    bind.fromitf = "target";
                } 
                else if(clientmpc == null && servermpc == null && bind.trace) 
                {
                    Interface itf = idlcompiler.Compile(itftype, bind);
                    TraceGeneratorInterface gen = targetfactory.getTrace();
                    gen.setItf(itf);
                    bc bc =  gen.getPrimitive(
                            new astbasefile(bind.parsedFile, bind.line, bind.column));
                    bc.debugComment = bind.from + "." + bind.fromitf + "->" + bind.to + "." + bind.toitf + " [" + bind.size + "]";
                    ncs.add(bc);

                    // Add binding between client sub component and binding component
                    newb.add(new astbind(null, 0, 0, bind.from, bind.fromitf, bc.nameinowner, "target"));
                    
                    // Replace binding client sub-component by binding component
                    bind.from = bc.nameinowner;
                    bind.fromitf = "target";
                } 
            } catch(IOException e) {
                throw new compilerexception(bind, e.getMessage());
            }
        }

        // This delayed adding was need to avoid java.util.ConcurrentModificationException exception
        for(final astbind newbind : newb)
            composite.addBind(newbind);

        // Add created component
        for (final primitiveraw comp : ncs) {
            ast2instance a2i = new ast2instance(comp);
            a2i.createCanonical(null, null, null);
            composite.addFcControllerComponent(comp);
        }
    }
    
    public void browseComponent(component component) throws compilerexception 
    {
        super.browseComponent(component);
    }
}
