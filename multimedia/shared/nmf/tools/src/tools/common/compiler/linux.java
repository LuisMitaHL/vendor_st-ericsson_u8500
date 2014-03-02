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
package tools.common.compiler;

import java.io.File;
import java.io.IOException;
import java.io.PrintStream;
import java.util.*;

import tools.common.compilerexception;
import tools.common.configuration;
import tools.common.configuration.TypeHeader;
import tools.common.exec.task;
import tools.conf.graph.component;

public class linux extends ccompiler 
{
    File mk;
	ArrayList<String> srcfiles;
	
	public void setDefaultFlags(component component) throws compilerexception 
	{
	    mk = configuration.getComponentTemporaryExt(component.type, "mk");
	    srcfiles = new ArrayList<String>();
	}
	public void setDefaultFlags(ccompiler parent)
	{
	    if(parent instanceof linux)
	    {
	        mk = ((linux)parent).mk;
	        srcfiles = ((linux)parent).srcfiles;
	    }
	}

	public String getPathToFile(File parent, File file) throws IOException 
	{
	    String path = "";

	    while(! file.getCanonicalPath().startsWith(parent.getCanonicalPath()))
	    {
	        path = path + "../";
	        parent = parent.getParentFile();
	    }
	    
	    String remain = file.getCanonicalPath().substring(parent.getCanonicalPath().length());
	    remain = remain.replace(File.separator, "/");
	    while(remain.charAt(0) == '/')
	        remain = remain.substring(1);
	    
	    return path + remain;
	}

	public task compile(File src, File obj, String speccflags) throws compilerexception 
	{
        try 
        {
            String path = getPathToFile(mk.getParentFile().getCanonicalFile(), src.getCanonicalFile());
            srcfiles.add(path);
        } 
        catch (IOException e1) 
        {
            // TODO Auto-generated catch block
            e1.printStackTrace();
        }
	    return null;
	}

    public boolean needRecompile(File obj, File dep) 
    {
        return true;
    }
    public void createDependenciesFile(File src, File dep) throws compilerexception 
    {
    }
    
    public task link(File out, Set<String> files, ArrayList<task> predecessor) throws compilerexception 
    {
        PrintStream mkout = configuration.forceRegeneration(mk, TypeHeader.makefile);
        mkout.println("# Component Sources");
        mkout.print("FILES = ");

        for(String path : srcfiles) 
        {
            mkout.println("\\");
            mkout.print("   " + path);
        }
        mkout.println();

        mkout.close();
        
	    return null;
	}

	public task library(File out, Set<String> files, ArrayList<task> predecessor) throws compilerexception 
	{
	    return link(out, files, predecessor);
	}
	
    public task archive(File out, Set<String> files, ArrayList<task> predecessor) throws compilerexception 
    {
        return link(out, files, predecessor);
    }

}
