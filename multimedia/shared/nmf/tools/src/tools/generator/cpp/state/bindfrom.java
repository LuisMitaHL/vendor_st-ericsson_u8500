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
package tools.generator.cpp.state;

import java.io.PrintStream;

import tools.conf.graph.interfacereference;
import tools.idl.ast.Interface;

public abstract class bindfrom {
    protected Interface itf;
    protected interfacereference ir;

    public bindfrom(Interface _itf, interfacereference _ir) 
    {
        itf = _itf;
        ir = _ir;
    }

    public void getInterface(String itfname, PrintStream out) {}

    public abstract void bindFromUser(String itfname, PrintStream out);
    public abstract void unbindFromUser(String itfname, PrintStream out);
    
    public abstract void getComponentInterface(PrintStream out);

    public abstract void bindFromMPC(String itfname, PrintStream out);
    public abstract void unbindFromMPC(String itfname, PrintStream out);

}
