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

import java.io.File;
import java.util.HashSet;

import tools.common.astbasefile;
import tools.common.compilerexception;
import tools.common.configuration;
import tools.common.error;
import tools.common.targetfactory;
import tools.common.targetfactory.CodingStyle;
import tools.conf.ast.astbind;
import tools.conf.ast.astinterface;
import tools.conf.ast.astprovide;
import tools.conf.ast.astrequire;
import tools.conf.graph.component;
import tools.conf.graph.composite;
import tools.conf.graph.visitor;
import tools.idl.ast.Interface;

public class checkinterface extends visitor {
    public File mostRecentFile = null;

    protected void travelCommon(component component, Object data) throws compilerexception {
        /*
         * Check provide
         */
        for(astprovide provide : component.getProvides()) {
            
            // Check that interface exists
            Interface itf = tools.idl.idlcompiler.Compile(provide.type, provide);
            mostRecentFile = configuration.getMostRecent(mostRecentFile, itf.file);

            String[][] association = new String[][] {
                {"constructor", "lifecycle.constructor"},
                {"destructor", "lifecycle.destructor"},
                {"starter", "lifecycle.starter"},
                {"stopper", "lifecycle.stopper"}
            };
            
            for(int i = 0; i < association.length; i++) {
                if(association[i][0].equals(provide.name) != association[i][1].equals(provide.type))
                    throw new compilerexception(provide, error.NAME_RESERVED_FOR_LIFECYCLE, association[i][0], association[i][1]);

                if(association[i][0].equals(provide.name)) {
                    if(provide.collection) 
                        throw new compilerexception(provide, error.LCC_WITH_COLLECTION, association[i][1]);
                    if(component instanceof composite)
                        throw new compilerexception(provide, error.LCC_ITF_WITH_COMPOSITE);
                }
            }

            // Check that require not provide twice with same name
            if(component.getProvide(provide.name) != provide)
                throw new compilerexception(provide, error.INTERFACE_PROVIDE_TWICE, 
                        provide.name);
            
            // Check that number of element of collection in ]0...255]
            if(provide.collection) {
                if(provide.numberOfElement <= 0 || provide.numberOfElement >= 256) {
                    throw new compilerexception(provide, error.COLLECTION_OUT_OF_RANGE,
                            provide.name);
                }
             }
            
            // Check that interrupt handler well formed
            if(provide.interrupt) {
                if(! "exception.handler".equals(provide.type))
                    throw new compilerexception(provide, error.IRQ_ITF_TYPE_WRONG);
                if(provide.collection) 
                    throw new compilerexception(provide, error.IRQ_ITF_CAN_BE_COLLECTION);
                switch(targetfactory.domain)
                {
                case MMDSP:
                    if(provide.line < 2 || provide.line > 31) 
                        throw new compilerexception(provide, error.IRQ_LINE_OUT_OF_RANGE, 2, 30, "2, 11, 20, 31");
                    if(provide.line == 2 || provide.line == 11 || provide.line == 20 || provide.line == 31) 
                        throw new compilerexception(provide, error.IRQ_LINE_OUT_OF_RANGE, 2, 30, "2, 11, 20, 31");
                    break;
                default:
                    if(component.getMPC() == null)
                        throw new compilerexception(provide, error.IRQ_NOT_SUPPORTED);
                }
            }
            
            // Check that prefix not used with composite component
            if(component instanceof composite && provide.methprefix.length() > 0)
                throw new compilerexception(provide, error.ITF_PREFIX_IN_COMPOSITE);
        }

        /*
         * Check provide
         */
        for (astrequire require : component.getRequires()) {

            // Check that interface exists
            Interface itf = tools.idl.idlcompiler.Compile(require.type, require);
            mostRecentFile = configuration.getMostRecent(mostRecentFile, itf.file);

            // Check that require not provide twice with same name
            if(component.getRequire(require.name) != require)
                throw new compilerexception(require, error.INTERFACE_REQUIRE_TWICE, 
                        require.name);

            // Check that static required interface type not required twice
            if(require.isStatic && component.getRequireByType(require.type) != require)
                throw new compilerexception(require, error.STATIC_INTERFACE_REQUIRE_TWICE, 
                        require.type);
            
            // Check that static required interface type not required twice
            if(require.isStatic && require.collection)
                throw new compilerexception(require, error.STATIC_INTERFACE_AS_COLLECTION);
            
            // Check that number of element of collection in ]0...255]
            if(require.collection) {
                if(require.numberOfElement <= 0 || require.numberOfElement >= 256) {
                    throw new compilerexception(require, error.COLLECTION_OUT_OF_RANGE,
                            require.name);
                }
            }
            
            if(require.isNMFLifeCycleInterface())
                throw new compilerexception(require, error.LCC_CANT_BE_REQUIRED, require.type);
        }
    }

    protected String checkRequire(astbasefile ctx, component client, String itfname) throws compilerexception {
        astrequire require = client.getRequire(astinterface.getNameOfInterfaceCollection(itfname));
        
        // Check that interface exists
        if(require == null)
            throw new compilerexception(ctx, error.COMPONENT_NOT_REQUIRE_INTERFACE,
                    client.type.fullyname, itfname);
        
        if(require.collection) {
            // Check that a array index is used for binding to a collection
            if(! itfname.contains("["))
                throw new compilerexception(ctx, error.WRONG_COLLECTION_USE,
                        client.type.fullyname, require.name);
            // And that the index not bigger than the collection
            if(astinterface.getIndexOfInterfaceCollection(itfname) >= require.numberOfElement)
                throw new compilerexception(ctx, error.COLLECTION_INDEX_OUT_OF_RANGE,
                        (require.numberOfElement-1), require.name);
        } else {
            // Check that a array is not used for accessible standard interface
            if(itfname.contains("["))
                throw new compilerexception(ctx, error.WRONG_COLLECTION_INDEX_USE,
                        require.name);
        }
        return require.type;
    }
    protected String checkProvide(astbasefile ctx, component server, String itfname)throws compilerexception {
        astprovide provide = server.getProvide(astinterface.getNameOfInterfaceCollection(itfname));

        // Check that interface exists
        if(provide == null)
            throw new compilerexception(ctx, error.COMPONENT_NOT_PROVIDE_INTERFACE,
                    server.type.fullyname, itfname);
     
        if(provide.collection) {
            // Check that a array index is used for binding to a collection
            if(! itfname.contains("["))
                throw new compilerexception(ctx, error.WRONG_COLLECTION_USE, 
                        server.type.fullyname, provide.name);
            // And that the index not bigger than the collection
            if(astinterface.getIndexOfInterfaceCollection(itfname) >= provide.numberOfElement)
                throw new compilerexception(ctx, error.COLLECTION_INDEX_OUT_OF_RANGE,
                        (provide.numberOfElement-1), provide.name);
        } else {
            // Check that a array is not used for accessible standard interface
            if(itfname.contains("["))
                throw new compilerexception(ctx, error.WRONG_COLLECTION_INDEX_USE,
                        provide.name);
        }
        return provide.type;
    }

    protected void travelComposite(composite composite, Object data) throws compilerexception {
        HashSet<String> requirebinded = new HashSet<String>();
        for(final astbind bind : composite.getBinds()) {
            // Check that client component exist
            component client = composite.getFcSubComponent(bind.from);
            if(client == null)
                throw new compilerexception(bind, error.SUBCOMPONENT_NOT_EXIST, 
                        bind.from, composite.type.fullyname);

            // and provide corresponding interface
            String fromtype;
            if(client == composite) {
                // Check that a composite provided interface is not require twice
                if(requirebinded.contains(bind.fromitf))
                    throw new compilerexception(bind, error.COMPOSITE_REQUIRE_ALREADY_BINDED,
                            bind.fromitf);
                requirebinded.add(bind.fromitf);
                
                fromtype = checkProvide(bind, client, bind.fromitf);
            } else {
                fromtype = checkRequire(bind, client, bind.fromitf);
            }
            
            // Check that server component exist
            component server = composite.getFcSubComponent(bind.to);
            if(server == null)
                throw new compilerexception(bind, error.SUBCOMPONENT_NOT_EXIST,
                        bind.to, composite.type.fullyname);

            // and provide corresponding interface
            String totype;
            if(server == composite) {
                // Check that provide not connected directly to require
                if(client == server)
                    throw new compilerexception(bind, error.PROVIDE_BIND_TO_REQUIRE);
                
                totype = checkRequire(bind, server, bind.toitf);
            } else {
                totype = checkProvide(bind, server, bind.toitf);
            }
            
            // Check that interface types are equals
            if(! fromtype.equals(totype))
                throw new compilerexception(bind, error.INTERFACE_MISMATCH,
                        fromtype, totype);
            
            // Check fifo size for asynchronous communications.
            if(bind.asynchronous && (bind.size <= 0 || bind.size > bind.MAX_COMMUNICATION_FIFO_SIZE))
                throw new compilerexception(bind, error.COMS_OUT_OF_FIFO_SIZE_LIMIT);
            
            if(bind.asynchronous && (bind.subpriority < 0 || bind.subpriority >= bind.MAX_COMMUNICATION_SUBPRIORITY))
                throw new compilerexception(bind, error.COMS_OUT_OF_SUBPRIORITY_LIMIT, bind.MAX_COMMUNICATION_SUBPRIORITY);

            // Not beautiful, must be moved elsewhere
            if(targetfactory.style == CodingStyle.CPP) {
                // Check if distributed communication goes through asynchronous if not relying composite boundary
                if(client.getMPC() != server.getMPC() &&
                        ! (server == composite && composite.owner == null) && 
                        ! (client == composite && composite.owner == null) && 
                        ! bind.asynchronous)
                    throw new compilerexception(bind, error.DISTRIBUTE_BIND_MUSTBE_ASYNCHRONOUS, 
                            client.getPathName() + ":" + client.getMPC(), 
                            server.getPathName() + ":" + server.getMPC());

                // Check if boundary composite call not asynchronous since already add by bindTo/From API
                if(server == composite && composite.owner == null && bind.asynchronous)
                    throw new compilerexception(bind, error.CALL_MUSTNOTBE_ASYNCHRONOUS, "CallBack");
                
                if(client == composite && composite.owner == null && bind.asynchronous)
                    configuration.warning(new compilerexception(bind, error.CALL_MUSTNOTBE_ASYNCHRONOUS, "Call"));
            }
        }
 
        super.travelComposite(composite, data);
    }

}
