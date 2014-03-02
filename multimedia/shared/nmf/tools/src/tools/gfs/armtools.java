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
package tools.gfs;

import java.io.File;
import java.util.ArrayList;
import java.util.Iterator;

import tools.common.exec.Args;
import tools.common.exec.exec;
import tools.common.exec.pipecolorlet.ErrorType;

public class armtools 
{
	public static String strCmdHDD = "";
	public String[] c_filenames;
	public static ArrayList<String> m_listFiles;
	
	public armtools (String[] args)
	{
		c_filenames = args;
	}
	
	public static boolean CreateObjFile(File inputfile, File outputfile, File stripfile)
	{
	    if(inputfile.lastModified() > outputfile.lastModified()) {
            // Recompile only if necessary!

	        Args argc;
	        
	        try
	        {
	            System.out.println("CreateObjFile Output File : " + outputfile.getName());

	            if(stripfile != null) {
	                argc = new Args("STRIP", stripfile.getPath());

	                argc.add("mmdsp-objcopy");
                    argc.add("--no-adjust-warnings");
	                argc.add("-I");
	                argc.add("elf64-big"); 
	                argc.add("-O");
	                argc.add("elf64-big"); 
	                argc.add("--strip-debug");
	                argc.add("--remove-section");
	                argc.add(".note");
	                argc.add("--remove-section");
	                argc.add("header_segment");
	                argc.add(inputfile.getCanonicalPath());
	                argc.add(stripfile.getCanonicalPath());

	                exec.runArgc(argc, ErrorType.AlaGnu);
	            } else {
	                stripfile = inputfile;
	            }

	            argc = new Args("BIN", outputfile.getPath());
	            argc.add("arm-linux-objcopy");
	            argc.add("-I");
	            argc.add("binary"); 
	            argc.add("-O");
	            argc.add("elf32-littlearm");
	            argc.add("-B");
	            argc.add("arm");
	            argc.add(stripfile.getCanonicalPath());
	            argc.add(outputfile.getCanonicalPath());

	            exec.runArgc(argc, ErrorType.AlaGnu);
	        }
	        catch(Exception e)
	        {
	            System.out.println("error==="+e.getMessage());
	            e.printStackTrace();
	        }
            
            return true;
	    }
        
        return false;
	}
	
	public static String CompileCFile(File file, String compilo_name)
	{
		String str_cmd = null;
			
		try
	    {
			String outputName = file.getParent() + "/" + file.getName().substring(0, file.getName().lastIndexOf(".c")) + ".o";
			
			Args argc = new Args("CC", file.getName().substring(0, file.getName().lastIndexOf(".c")) + ".o");
            if(compilo_name == null)
            {	
            	argc.add("arm-linux-gcc");
            }
            else
            {
            	System.out.println("name of compiler : "+compilo_name);
            	argc.add(compilo_name);
            }
            argc.add("-g");
            argc.add("-c");
            argc.add(file.getPath());
            argc.add("-o"); 
            argc.add(outputName);
            exec.runArgc(argc, ErrorType.AlaGnu);
            
            // Strip .ARM.attributes section to avoid troubles in link stage
            argc = new Args("STRIP", file.getName().substring(0, file.getName().lastIndexOf(".c")) + ".o");
            argc.add("arm-linux-objcopy");
            argc.add("-R");
            argc.add(".ARM.attributes");
            argc.add(outputName);
            exec.runArgc(argc, ErrorType.AlaGnu);
	    }
		catch(Exception e)
	    {
	    	System.out.println("error==="+e.getMessage());
	    	e.printStackTrace();
	    }
		return str_cmd;
	}
	
	public static void CreateHDD (String s_root, ArrayList<file2c.FILE> file_entries, String linkerArg)
	{
        try
        {
            Args argc = new Args("LD", "harddisk.o");
            if(linkerArg == null)
            {
            	argc.add("arm-linux-ld");
            	argc.add("-r");
            }
            else
            {
            	System.out.println("name of linker : "+linkerArg);
            	argc.add(linkerArg);
            	argc.add("--partial");
            }
            argc.add("-o");
            argc.add(s_root+"/harddisk.o");
            for(Iterator<file2c.FILE> i = file_entries.iterator(); i.hasNext(); ) {
                file2c.FILE e = i.next();
                argc.add(e.ofile.getPath());
            }
            argc.add(s_root+"/gfs_filesystem.o");
                
            exec.runArgc(argc, ErrorType.AlaGnu);
        }
        catch (Exception e)
        {
            System.out.println("error==="+e.getMessage());
            e.printStackTrace();
        }
	}

}
