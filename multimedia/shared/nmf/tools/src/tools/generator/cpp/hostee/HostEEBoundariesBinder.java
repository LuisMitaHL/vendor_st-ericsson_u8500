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
package tools.generator.cpp.hostee;

import java.io.IOException;

import tools.common.*;
import tools.conf.ast.*;
import tools.conf.graph.*;
import tools.conf.graph.graphfactory.ast2instance;
import tools.generator.api.BoundariesBinderInterface;
import tools.generator.cpp.hostee.bc.*;
import tools.idl.idlcompiler;
import tools.idl.ast.Interface;

public class HostEEBoundariesBinder extends visitor implements BoundariesBinderInterface {
    
    protected composite topcomposite;
    
    protected void travelRawPrimitive(primitiveraw primitive, Object data) throws compilerexception 
    {
        for(astrequire require : primitive.getRequires())
        {
            if(require.isStatic || require.virtualInterface)
                continue;
            for(int i = 0; i < require.numberOfElement; i++) 
            {
                String itfname = require.getInterfaceName(i);
                interfacereference ir = primitive.lookupFcWithoutError(itfname);
                
                if(ir != null && ir.server == null)
                {
                    // We are bound outside
                    component cb = topcomposite.getFcSubComponent(ir.getInterfaceName());
                    assert cb != null : "Output proxy not setted for " + itfname;
                    
                    if(primitive.getMPC() != null)
                    {
                        /*
                         * DSP -> Host Composite
                         * Binding: DSP -> Composite
                         * This binding can be optimized since we have no code to execute and thus event is not required
                         */
                        assert cb instanceof callbackasynchronous: "Only asynchronous binding allowed on MPC -> Composite";

                        Interface itf = idlcompiler.Compile(require.type, require);
                        ((callbackasynchronous)cb).clients.add(
                                new mpctoout(itf, primitive, itfname));
                    } 
                    else
                    {
                        if(cb instanceof callbackasynchronous)
                        {
                            ((callbackasynchronous)cb).clients.add(
                                    new smptoout(primitive, itfname));
                        }

                        ((callbackbase)cb).clients.add(
                                new callbackbase.subclient(primitive, itfname));
                    }
                }
            }
        }
    }

    public void browseComponent(component component) throws compilerexception 
    {
        topcomposite = (composite)component;

        for (astprovide provide : topcomposite.getProvides()) 
        {
            try 
            {
                Interface itf = idlcompiler.Compile(provide.type, provide);

                for(int i = 0; i < provide.numberOfElement; i++) 
                {
                    String itfname = provide.getInterfaceName(i);
                    interfacereference ir = topcomposite.getFcInterface(itfname);
                    primitiveraw bc;

                    if(itf.isAsynchronous())
                    {
                        if(ir.server.getMPC() == null)
                        {
                            bc = new callasynchronous(provide, itf, itfname,
                                    provide.name, (i == 0) ? provide.numberOfElement : -1, 
                                            new outtosmp(itf, ir));
                        } 
                        else
                        {
                            bc = new callasynchronous(provide, itf, itfname,
                                    provide.name, (i == 0) ? provide.numberOfElement : -1, 
                                            new outtompc(itf, ir));
                        }

                        for(final astbind sb : topcomposite.getBinds()) 
                        {
                            if("this".equals(sb.from) && itfname.equals(sb.fromitf)) 
                            {
                                topcomposite.addBind(new astbind(null, 0, 0, itfname, "target", sb.to, sb.toitf));
                                break;
                            }
                        }
                    } 
                    else
                    {
                        bc = new callsynchronous(provide, itf, itfname, ir);
                    }

                    // Add binding between callback and user 
                    bc.debugComment = "-> " + itfname;

                    ast2instance a2i = new ast2instance(bc);
                    a2i.createCanonical(null, null, null);
                    topcomposite.addFcControllerComponent(bc);
                }
            } catch (IOException e) {
                // TODO Auto-generated catch block
                throw new compilerexception(provide, e.getMessage());
            }
        }

        for (astrequire require : topcomposite.getRequires()) 
        {
            try 
            {
                Interface itf = idlcompiler.Compile(require.type, require);

                for(int i = 0; i < require.numberOfElement; i++) 
                {
                    String itfname = require.getInterfaceName(i);
                    primitiveraw bc;

                    if(itf.isAsynchronous())
                    {
                        bc = new callbackasynchronous(require, itf, itfname, 
                                require.name, (i == 0) ? require.numberOfElement : -1, 
                                        (new callbackasynchronousgenerator(itf)).getAstAsynchronous(), 
                                        require.optional); 
                    } 
                    else
                    {
                        bc = new callbacksynchronous(require, itf, itfname, 
                                require.name, (i == 0) ? require.numberOfElement : -1, 
                                        require.optional);
                    }

                    // Add binding between callback and user 
                    bc.debugComment = "" + itfname + " -> ";

                    ast2instance a2i = new ast2instance(bc);
                    a2i.createCanonical(null, null, null);
                    topcomposite.addFcControllerComponent(bc);
                }
            } catch (IOException e) {
                // TODO Auto-generated catch block
                throw new compilerexception(require, e.getMessage());
            }
        }

        tools.idl.idlcompiler.Compile("ee.api.trace", null);
        astrequire reqsched = new astrequire(null, 0, 0, "ee.api.trace", "trace");
        reqsched.isStatic = true;
        topcomposite.addRequire(reqsched); 

        super.browseComponent(topcomposite);
    }
}
