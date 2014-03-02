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
package tools.conf.ast;

import java.io.File;

import tools.common.astbasefile;

public class astbind extends astbasefile implements Comparable<astbind> {
	public String from, fromitf;
	public String to, toitf;
	
	public astbind(File _file, int line, int column, 
			String _from, String _fromitf,
			String _to, String _toitf) {
		super(_file, line, column);
		this.from = _from;
		this.to = _to;
		
		// Serialize the name
		String name;
		int index;
		
        name = astinterface.getNameOfInterfaceCollection(_fromitf);
        index = astinterface.getIndexOfInterfaceCollection(_fromitf);
        if(index != -1)
            name = name + "[" + index + "]";
		this.fromitf = name;
		
        name = astinterface.getNameOfInterfaceCollection(_toitf);
        index = astinterface.getIndexOfInterfaceCollection(_toitf);
        if(index != -1)
            name = name + "[" + index + "]";
        this.toitf = name;
	}

	public astbind(astbind other) {
	    super(other.parsedFile, other.line, other.column);
	    this.from = other.from;
	    this.fromitf = other.fromitf;
	    this.to = other.to;
	    this.toitf = other.toitf;
	    this.asynchronous = other.asynchronous;
	    this.size = other.size;
	    this.trace = other.trace;
        this.subpriority = other.subpriority;
	}

	public int compareTo(astbind o) {
	    int ret = this.to.compareTo(o.to);
		if (ret == 0)
		{
			ret = this.toitf.compareTo(o.toitf);
		}
		return ret;
        // If this < o, return a negative value
        // If this = o, return 0
        // If this > o, return a positive value
    }
	
	// For event binding component
	public final int MAX_COMMUNICATION_FIFO_SIZE = 256;
	public final int MAX_COMMUNICATION_SUBPRIORITY = 4;

	public boolean asynchronous = false;
	public int size = 4;
	public int subpriority = 0;

	// For trace
	public boolean trace = false;

}
