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

import tools.conf.ast.astinterface;
import tools.conf.ast.astprovide;
import tools.conf.graph.interfacereference;
import tools.generator.cpp.state.bindfrom;
import tools.idl.ast.Interface;

import java.io.PrintStream;

public class outtosmp extends bindfrom 
{
    astprovide providesc;

    public outtosmp(Interface _itf, interfacereference _ir)
    {
        super(_itf, _ir);
        providesc = ir.server.getProvide(astinterface.getNameOfInterfaceCollection(ir.getInterfaceName()));
    }

    public void getInterface(String itfname, PrintStream out) 
    { 
        if(providesc.collection || ! "".equals(providesc.methprefix)) 
        {
            out.println("    *reference = (" +  itf.name_ + "Descriptor*)&_xyuv_" + ir.server.getPathName_() + "._xywrap_" + ir.getInterfaceName() + ";");        
        } 
        else 
        {
            out.println("    *reference = (" +  itf.name_ + "Descriptor*)&_xyuv_" + ir.server.getPathName_() + ";");        
        }
        out.println("    return NMF_OK;");
    }

    public void bindFromUser(String itfname, PrintStream out) 
    {
        out.println("    _xybc_" + itfname + ".destroyFifo();            // Destroy server previous binding if any");        
        out.println("    _xybc_" + itfname + ".connect(" + ir.server.getProvideIndex(ir.getInterfaceName()) + " << 8, &_xyuv_" + ir.server.getPathName_() + ");");
        out.println("    _xybc_" + itfname + ".name = \"User => " + itfname + " # " + ir.server.getPathName_()+ "[" + ir.server.type.fullyname + "]\";");
        out.println("    if(_xybc_" + itfname + ".createFifo(size, " + ir.server.priority.getValue() + ", subpriority) != NMF_OK)");        
        out.println("      return NMF_NO_MORE_MEMORY;");
        if(providesc.collection || ! "".equals(providesc.methprefix)) 
            out.println("    _xybc_" + itfname + ".target = (" + itf.name_ + "Descriptor*)&_xyuv_" + ir.server.getPathName_() + "._xywrap_" + ir.getInterfaceName() + ";");        
         else 
            out.println("    _xybc_" + itfname + ".target = (" + itf.name_ + "Descriptor*)&_xyuv_" + ir.server.getPathName_() + ";");        
        
        out.println("    *reference = &_xybc_" + itfname + ";");
    }

    public void unbindFromUser(String itfname, PrintStream out) 
    {
        out.println("    return _xybc_" + itfname + ".destroyFifo();");        
    }

    public void getComponentInterface(PrintStream out) {}

    public void bindFromMPC(String itfname, PrintStream out) 
    {
        out.println("    _xybc_" + itfname + ".destroyFifo();            // Destroy server previous binding if any");        
        out.println("    _xybc_" + itfname + ".connect(" + ir.server.getProvideIndex(ir.getInterfaceName()) + " << 8, &_xyuv_" + ir.server.getPathName_() + ");");
        out.println("    if(_xybc_" + itfname + ".createFifo(size, " + ir.server.priority.getValue() + ", 0) != NMF_OK)");        
        out.println("      return NMF_NO_MORE_MEMORY;");
        if(providesc.collection || ! "".equals(providesc.methprefix)) 
            out.println("    _xybc_" + itfname + ".target = (" + itf.name_ + "Descriptor*)&_xyuv_" + ir.server.getPathName_() + "._xywrap_" + ir.getInterfaceName() + ";");        
         else 
            out.println("    _xybc_" + itfname + ".target = (" + itf.name_ + "Descriptor*)&_xyuv_" + ir.server.getPathName_() + ";");        
        out.println("    return (t_nmf_error)CM_BindComponentToUser(");
        out.println("          (t_nmf_channel)EEgetDistributionChannel(" + ir.server.priority.getValue() + "),");
        out.println("          client,");
        out.println("          clientitfname,");
        out.println("          &_xybc_" + itfname +",");
        out.println("          size);");
    }

    public void unbindFromMPC(String itfname, PrintStream out) 
    {
        out.println("    void *handle" + hashCode() + ";");
        out.println("    _xybc_" + itfname + ".destroyFifo();");        
        out.println("    return CM_UnbindComponentToUser(");
        out.println("          (t_nmf_channel)EEgetDistributionChannel(" + ir.server.priority.getValue() + "),");
        out.println("          client,");
        out.println("          clientitfname,");
        out.println("          &handle" + hashCode() + ");");
    }


}
