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

import java.io.*;
import java.util.ArrayList;

public class partitiontable 
{
	FileWriter m_fwriter;
	
	public partitiontable()
	{
		m_fwriter = null;
	}
	
	
	public int WrappingDeclaration(FileWriter fwriter, String filename)
	{
		String line1 = null;
		String line2 = null;
		String line3 = null;
		
		// TODO: see Linux Path formating
		String my_filename = filename; // = filename.substring(filename.lastIndexOf("\\")+1);
		// only get the name transformed according to objcopy syntax '.' --> '_'
		// toto.bin --> toto_bin
		my_filename = my_filename.replace('.', '_');
		my_filename = my_filename.replace('/', '_');
		my_filename = my_filename.replace('\\', '_');
		my_filename = my_filename.replace(':', '_');
		my_filename = my_filename.replace('-', '_');
		
		// The symbols created by objcopy are constructed from the original filename of the file
		line1 = String.format("extern char _binary_%1$s_start;\n", my_filename);
		line2 = String.format("extern char _binary_%1$s_end;\n", my_filename);
		line3 = String.format("extern char _binary_%1$s_size;\n\n", my_filename);
		
		try
		{
			fwriter.write(line1, 0, line1.length());
			fwriter.write(line2, 0, line2.length());
			fwriter.write(line3, 0, line3.length());
		}
		catch(IOException e)
		{
			System.out.println("error==="+e.getMessage());
	    	e.printStackTrace();		
		}			
		
		return line1.length()+line2.length()+line3.length();
	}
	
	
	
	/* This method will format a line and write it into the table of the C file 
	 * It takes in arguments:
	 * - the instance of the filewriter
	 * - The filename of the "binary" file which is converted by objcopy (full path)
	 * It returns the number of written characters
	 */
	
	public int WrappingLine(FileWriter fwriter, String filename, String objfilename)
	{
		String line = null;
		
		String my_trsfn_filename = objfilename;
		// only get the name transformed according to objcopy syntax '.' --> '_'
		// toto.bin --> toto_bin
		my_trsfn_filename = my_trsfn_filename.replace('.', '_');
		my_trsfn_filename = my_trsfn_filename.replace('/', '_');
		my_trsfn_filename = my_trsfn_filename.replace('\\', '_');
		my_trsfn_filename = my_trsfn_filename.replace(':', '_');
		my_trsfn_filename = my_trsfn_filename.replace('-', '_');
		
		
						
		// FIXME: Temporary
		String SymbianFilename = filename.replace('\\', '/');
		//SymbianFilename = SymbianFilename.replace('\\', '/');
		//SymbianFilename = SymbianFilename.
		
		// The symbols created by objcopy are constructed from the original filename of the file
		line = String.format("{\"%1$s\", &_binary_%2$s_start, &_binary_%2$s_end, &_binary_%2$s_size}", SymbianFilename, my_trsfn_filename);
				
		try
		{
			fwriter.write(line, 0, line.length());
		}
		catch(IOException e)
		{
			System.out.println("error==="+e.getMessage());
	    	e.printStackTrace();		
		}			
		
		return line.length();
	}
	
	public void CreateFileHeader(File fileH)
	{
		try
		{
			// Create the .H file from the path given
			fileH.createNewFile();
			
			// Creates the stream writer which will write to the file
			FileWriter fwriterH = new FileWriter(fileH);
			
            /*
             * Be careful, this structure is hardcoded in objfs component source!!!!!!
             */
			fwriterH.write("#ifndef _FSDESCRIPTOR_H\n");
			fwriterH.write("#define _FSDESCRIPTOR_H 1\n\n");
			fwriterH.write("typedef struct\n{\n");
			fwriterH.write("char	*c_filename;	// name of the file\n");
			fwriterH.write("char	*u32_fileStart; // address in memory of the beginning of the file\n");
			fwriterH.write("char	*u32_fileEnd;   // address of the end of the file\n");
			fwriterH.write("char     *u32_fileSize;   // size in byte of the file\n");
			fwriterH.write("}t_efs_entry;\n\n");
			fwriterH.write("extern t_efs_entry GFS_filesystem[];\n");
			fwriterH.write("extern int GFS_nb_entries;\n");
			fwriterH.write("\n#endif\n");
			
			
			fwriterH.close();			
		}
		catch(IOException e)
		{
			System.out.println("error==="+e.getMessage());
	    	e.printStackTrace();
	    }
		
	}
	
	/* This method will create a C file which will export a Table:
	 * This table will be the simple file system where we will find
	 * - the filename of the file
	 * - The start symbol of the obj file
	 * - The end symbol of the obj file
	 * - The size symbol of the binary file in the obj file
	 */
	public void CreateFileSystemTable(File fileC, int nb_entries, ArrayList<file2c.FILE> file_entries)
	{
		try
		{
			// Create the .C file from the path given
			fileC.createNewFile();
			
			// Creates the stream writer which will write to the file
			m_fwriter = new FileWriter(fileC);
						
			
			// Beginning of the file 
			//String prolog = String.format("#define NBFILES %1$d\n\n", nb_entries);
			String prolog = String.format("int GFS_nb_entries = %1$d;\n\n", nb_entries);
			m_fwriter.write("#include \"fsdescriptor.h\"\n");
			m_fwriter.write(prolog);
			
			// Declaring the symbols variables
			for (int i=0; i<nb_entries; i++)
			{
				WrappingDeclaration(m_fwriter, file_entries.get(i).sfile);						
			}
			
			// Beginning of the file 
			String prolog1 = String.format("t_efs_entry GFS_filesystem[%1$d] = \n{\n", nb_entries);
			m_fwriter.write(prolog1, 0, prolog1.length());
			
			//char filesystem[%1$d][4][%2$d] = \n{\n"
			// Filling the table
			for (int i=0; i<nb_entries; i++)
			{
				WrappingLine(m_fwriter, file_entries.get(i).ifile, file_entries.get(i).sfile);
				if(i!=nb_entries-1)
				{
					String end_line = ",\n";
					m_fwriter.write(end_line, 0, end_line.length());
				}				
			}
			
			// End of the file
			String end_prolog = "\n};\n";
			m_fwriter.write(end_prolog, 0, end_prolog.length());
			
			m_fwriter.flush();
			
			// Do not forget to close in order to flush the stream
			m_fwriter.close();
		}
		catch(IOException e)
		{
			System.out.println("error==="+e.getMessage());
	    	e.printStackTrace();
	    }
	}

}



