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
import tools.common.exec.task;
import tools.conf.graph.component;

public class symbian extends ccompiler 
{
    File mmh;
	ArrayList<String> srcfiles;
	
	public void setDefaultFlags(component component) throws compilerexception 
	{
	    mmh = configuration.getComponentTemporaryExt(component.type, "mmh");
	    srcfiles = new ArrayList<String>();
	}
	public void setDefaultFlags(ccompiler parent)
	{
	    if(parent instanceof symbian)
	    {
	        mmh = ((symbian)parent).mmh;
	        srcfiles = ((symbian)parent).srcfiles;
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
            String path = getPathToFile(mmh.getParentFile().getCanonicalFile(), src.getCanonicalFile());
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
        PrintStream mmpout = configuration.forceRegeneration(mmh);
        mmpout.println("// Component Sources");

        String oldpath = "X";
        for(String path : srcfiles) 
        {
            String newpath = path.substring(0, path.lastIndexOf("/"));
            String filename = path.substring(path.lastIndexOf("/") + 1);
            if(! oldpath.equals(newpath)) 
            {
                mmpout.println();

                String stringtosearch = "../shared/ens";
                int idx = newpath.indexOf(stringtosearch);
                if(idx != -1)
                {
                    String remainpath = newpath.substring(idx + stringtosearch.length());
                    mmpout.println("#ifdef STE_MM_OSI_SHARED_EXPORT_PATH");
                    mmpout.println("SOURCEPATH STE_MM_OSI_SHARED_EXPORT_PATH/ens" + remainpath);
                    mmpout.println("#else");
                    mmpout.println("SOURCEPATH " + newpath);
                    mmpout.println("#endif");
                } 
                else
                {
                    mmpout.println("SOURCEPATH " + newpath);
                }
                oldpath = newpath;
            }
            mmpout.println("SOURCE " + filename);
        }
        
        mmpout.close();
        
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
