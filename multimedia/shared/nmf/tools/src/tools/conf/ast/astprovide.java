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

import tools.common.compilerexception;
import tools.common.error;

public class astprovide extends astinterface {
	public String methprefix = "";

	public boolean unambiguous = false;
    public boolean referenced = false;

	// For interrupt
	public boolean interrupt = false;
	public int line;
	
	public astprovide(File _file, int line, int column, String _type, String _name) {
		super(_file, line, column, _type, _name);
	}
	
	protected boolean isCLetter(char ch) {
	    return Character.isLetter(ch) || ch == '_';
	}
	
	protected boolean isCLetterDigit(char ch) {
        return Character.isLetter(ch) || Character.isDigit(ch) || ch == '_';
    }
	
	public void setPrefix(String prefix) throws compilerexception {
	    // Sanity check
	    if(prefix.length() == 0) 
	        throw new compilerexception(this, error.ITF_PREFIX_EMPTY);
	    if(! isCLetter(prefix.charAt(0)))
	            throw new compilerexception(this, error.ITF_PREFIX_MUST_START_LETTER, prefix);
	    for(int i = 1; i < prefix.length(); i++)
	        if(! isCLetterDigit(prefix.charAt(i)))
	            throw new compilerexception(this, error.ITF_INVALID_PREFIX, prefix);
	    
	    methprefix = prefix;
	}
}
