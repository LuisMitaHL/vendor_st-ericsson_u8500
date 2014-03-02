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
package tools.generator.cpp.hostee.bc;

import java.io.IOException;
import java.io.PrintStream;
import java.util.ArrayList;

import tools.common.astbasefile;
import tools.common.compilerexception;
import tools.conf.ast.astprimitive;
import tools.generator.cpp.state.bindto;
import tools.idl.ast.*;

public class callbackasynchronous extends callbackbase {
    public ArrayList<bindto> clients = new ArrayList<bindto>();

    public callbackasynchronous(astbasefile _declaration, 
            Interface _itf, 
            String _itfname, String _name, int _indextodeclare,
            astprimitive _type,
            boolean _optional) throws compilerexception, IOException {
        super(_declaration, _itf, _itfname, _optional, _name, _indextodeclare, _type);
    }

    public void stopFlush(PrintStream out) {

		out.println("  if ((!_xyitfcb_" + itfname + ".IsNullInterface())&&(_xyitfcb_" + itfname + ".getReference()!=(void*)0xBEEF)&&(!_xyitfcbbt_" + itfname + ")) {");
        out.println("    _cbl_" + itf.name_ + "* cbl = (_cbl_" + itf.name_ + "*)_xyitfcb_" + itfname + ".getReference();");
        out.println("    cbl->nmfState = NMF::Primitive::STATE_STOPPED_WAIT_FLUSH; //no exec lock?");
        out.println("  }");
    }

    public void bindToUser(PrintStream out) 
    {
        String clientsName = "";
        boolean isFirstClient = true;
        out.println("  if(compositeStrcmp(compositeitfname, \"" + itfname + "\") == 0) {");
        out.println("    _cbl_" + itf.name_ + "* cbl;");

        if(! optional) 
        {
            out.println("    if(startNumber > 0)");
            out.println("      return NMF_COMPONENT_NOT_STOPPED;");
        }
        out.println("    if(! _xyitfcb_" + itfname + ".IsNullInterface())");
        out.println("      return NMF_INTERFACE_ALREADY_BINDED;");

        out.println("    cbl  = new _cbl_" + itf.name_ + ";");
        out.println("    if(cbl == 0x0)");        
        out.println("      return NMF_NO_MORE_MEMORY;");
        out.println("    cbl->name = \"" + debugComment + "\";");

        out.println("    error = cbl->createFifo(channel, size, clientContext);");        
        out.println("    if(error != NMF_OK) {");
        out.println("      delete cbl;");        
        out.println("      return NMF_NO_MORE_MEMORY;");
        out.println("    }");

        ArrayList<bindto> clientsdone = new ArrayList<bindto>();
        for(bindto client : clients)
        {
            if (isFirstClient) {
                clientsName += client.bindToUser(itfname, out);
                isFirstClient = false;
            } else {
                clientsName += " or " + client.bindToUser(itfname, out);
            }

            out.println("    if(error != NMF_OK) {");
            for(bindto clientdone : clientsdone)
            {
                clientdone.unbindToUser(itfname, out);
            }
            out.println("      cbl->destroyFifo(&clientContext);");        
            out.println("      delete cbl;");
            out.println("      return error;");
            out.println("    }");

            clientsdone.add(0, client);
        }
        out.println("    cbl->name = \"" + itfname + " => User # " + clientsName + "\";");
        
        out.println("    _xyitfcb_" + itfname + " = cbl;");        
        out.println("    return NMF_OK;");
        out.println("  }");
    }

    public void unbindToUser(PrintStream out) 
    { 
        out.println("  if(compositeStrcmp(compositeitfname, \"" + itfname + "\") == 0) {");
        out.println("    _cbl_" + itf.name_ + "* cbl;");

        if(! optional) 
        {
            out.println("    if(startNumber > 0)");
            out.println("      return NMF_COMPONENT_NOT_STOPPED;");
        }
        out.println("    if(_xyitfcb_" + itfname + ".IsNullInterface())");
        out.println("      return NMF_INTERFACE_NOT_BINDED;");

        out.println("    cbl  = (_cbl_" + itf.name_ + "*)_xyitfcb_" + itfname + ".getReference();");

        for(bindto client : clients)
        {
            client.unbindToUser(itfname, out);
        }

        out.println("    t_nmf_error error = cbl->destroyFifo(clientContext);");
        out.println("    if(error == NMF_OK)");
        out.println("      delete cbl;");
        out.println("    _xyitfcb_" + itfname + " = 0x0;");        
        out.println("    return error;");
        out.println("  }");
    }

    public void bindToSMP(PrintStream out) 
    {
        out.println("  if(compositeStrcmp(name, \"" + itfname + "\") == 0) {");

        if(! optional) 
        {
            out.println("    if(startNumber > 0)");
            out.println("      return NMF_COMPONENT_NOT_STOPPED;");
        }
        out.println("    if(! _xyitfcb_" + itfname + ".IsNullInterface())");
        out.println("      return NMF_INTERFACE_ALREADY_BINDED;");

        out.println("    error = target->bindFromUser(targetname, size, &_xyitfcb_" + itfname + ", subpriority);");
        out.println("    if(error != NMF_OK)");
        out.println("      return error;");

        ArrayList<bindto> clientsdone = new ArrayList<bindto>();
        for(bindto client : clients)
        {
            if(client.bindToSMP(itfname, out))
            {
                out.println("    if(error != NMF_OK) {");
                for(bindto clientdone : clientsdone)
                {
                    clientdone.unbindToSMP(out);
                }
                out.println("      return error;");
                out.println("    }");
            }

            clientsdone.add(0, client);
        }

        out.println("    return error;");
        out.println("  }");
    }

    public void unbindToSMP(PrintStream out)
    {
        out.println("  if(compositeStrcmp(name, \"" + itfname + "\") == 0) {");
        
        if(! optional) 
        {
            out.println("    if(startNumber > 0)");
            out.println("      return NMF_COMPONENT_NOT_STOPPED;");
        }
        out.println("    if(_xyitfcb_" + itfname + ".IsNullInterface())");
        out.println("      return NMF_INTERFACE_NOT_BINDED;");
        
        out.println("    target->unbindFromUser(targetname);");

        for(bindto client : clients)
        {
            client.unbindToSMP(out);
        }
        
        out.println("    _xyitfcb_" + itfname + " = 0x0;");        
        out.println("    return NMF_OK;");
        out.println("  }");
    }

    public void bindToMPC(PrintStream out)
    {
        out.println("  if(compositeStrcmp(compositeitfname, \"" + itfname + "\") == 0) {");
        
        if(! optional) 
        {
            out.println("    if(startNumber > 0)");
            out.println("      return NMF_COMPONENT_NOT_STOPPED;");
        }
        out.println("    if(! _xyitfcb_" + itfname + ".IsNullInterface())");
        out.println("      return NMF_INTERFACE_ALREADY_BINDED;");

        out.println("    _xyitfcb_" + itfname + " = (" + itf.name_ + "Descriptor*)0xBEEF;");

        ArrayList<bindto> clientsdone = new ArrayList<bindto>();
        for(bindto client : clients)
        {
            client.bindToMPC(out);

            out.println("    if(error != NMF_OK) {");
            for(bindto clientdone : clientsdone)
            {
                clientdone.unbindToMPC(out);
            }
            out.println("      return error;");
            out.println("    }");

            clientsdone.add(0, client);
        }
        
        out.println("    return error;");
        out.println("  }");
    }
    
    public void unbindToMPC(PrintStream out) 
    {
        out.println("  if(compositeStrcmp(compositeitfname, \"" + itfname + "\") == 0) {");

        if(! optional) 
        {
            out.println("    if(startNumber > 0)");
            out.println("      return NMF_COMPONENT_NOT_STOPPED;");
        }
        out.println("    if(_xyitfcb_" + itfname + ".IsNullInterface())");
        out.println("      return NMF_INTERFACE_NOT_BINDED;");

        for(bindto client : clients)
        {
            client.unbindToMPC(out);
        }

        out.println("    _xyitfcb_" + itfname + " = 0x0;");        
        out.println("    return NMF_OK;");
        out.println("  }");
    }
}
