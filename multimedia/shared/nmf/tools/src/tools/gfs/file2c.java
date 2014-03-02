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
import java.io.FileFilter;
import java.util.ArrayList;
import java.util.Stack;
import java.util.regex.Pattern;

import tools.common.configuration;
import tools.common.configuration.Debug;

public class file2c {

    int m_position = 0;
    class FILE {
        public String ifile;
        //public String rfile;
        public String sfile;
        public File ofile;
        public FILE(String _if, /*String _rf,*/ String _sf, File _of) {
            ifile = _if;
            //rfile = _rf;
            sfile = _sf;
            ofile = _of;
        }
    }
    ArrayList<FILE> m_listFiles = new ArrayList<FILE>();
    boolean needRecompilation = false;

    /**
     * @param args
     */
    public static void main(String[] args) {
        new file2c().run(args);
    }

    public void run(String[] args)
    {
        // TODO Auto-generated method stub

        // TODO: JAVA BUG
        if(args[0].endsWith("\""))
            args[0]= args[0].substring(0, args[0].length()-1);

        String[] my_Args 	= parseOption(args);
        String linkerArg	= my_Args[3];
        String compiloArg 	= my_Args[2];
        String outputArg 	= my_Args[1];

        if (outputArg == null || my_Args.length == 2)
        {
            System.out.println("Parameters are incorrect \nYou need at least 1 valid input, 1 valid output folder, 1 valid Name for the filesystem\n");
            System.out.println("-I Input (Can be a whole folder, only a file, or [*.'extension'] to select some type of files in a selected folder");
            System.out.println("-O Output folder");
            System.out.println("-N Name of the generated table (which contains the path of the files which will be encapsulated in the HDD");
            System.out.println("-C Name (complete path) of the compiler which will be used to generate the harddisk.o");
            System.out.println("Default: arm-linux-gcc");
            System.out.println("-L Name (complete path) of the linker which will be used to generate the harddisk.o");
            System.out.println("Default: arm-linux-ld");
            
            return;
        }
        else
        {
            if (!outputArg.endsWith("\\") && !outputArg.endsWith("/"))
            {
                outputArg+="/";
            }


            // 4 for 4 parameters before inputs path
            for(int i = 4; i < my_Args.length; i++)
            {
                File input_file;
                String input_arg_pattern = null;

                my_Args[i] = my_Args[i].replace("\\", "/");
                if(my_Args[i].indexOf("[") >= 0)
                {
                    input_file = new File(my_Args[i].substring(0, my_Args[i].indexOf("[")-1));
                    input_arg_pattern = my_Args[i].substring(my_Args[i].indexOf("[")+1, my_Args[i].indexOf("]"));					
                    configuration.debug(Debug.env, "Dir = "+input_file+" - pattern = "+input_arg_pattern);
                } else {
                    input_file = new File(my_Args[i]);
                }

                File output_file = new File(outputArg);
                if(!output_file.exists())
                {
                    System.out.println("Warning : Output folder : " + output_file + " does not exist !");
                    System.out.println("Output folder : " + output_file + " will be created");
                    // Creates the directory named by this abstract pathname, including any necessary but nonexistent parent directories.
                    output_file.mkdirs();                   
                }

                try 
                {
                	//ProcessInput(input_file, "", output_file, input_arg_pattern, true);
                	
                    Stack<String> my_prefix = new Stack<String>();
                	ProcessInput2(input_file, my_prefix, output_file, input_arg_pattern, false);
                }
                catch (java.io.IOException e)
                {}
            }			
        }

        partitiontable table = new partitiontable();

        
        File H_filename = new File(outputArg + "fsdescriptor.h");
        if((! H_filename.exists()) || needRecompilation) {
            configuration.debug(Debug.step, "CreateFileHeader : fsdescriptor.h");

            table.CreateFileHeader(H_filename);
            
            needRecompilation = true;
        }
        
        File C_filename = new File(outputArg + "gfs_filesystem.c");
        if((! C_filename.exists()) || needRecompilation) {
            configuration.debug(Debug.step, "CreateFileSystemTable : gfs_filesystem.c");     

            table.CreateFileSystemTable(C_filename, m_listFiles.size(), m_listFiles);
            armtools.CompileCFile(C_filename, compiloArg);

            needRecompilation = true;
        }
        
        if(needRecompilation) {
            configuration.debug(Debug.step, "Generating Obj Filesystem!\n");

            armtools.CreateHDD(outputArg, m_listFiles, linkerArg);
        }


    }


    String[] parseOption(String[] cmdLine)
    {
        // No more than 100 options
        String[] temp = new String[100];
        temp[0]=null;// Filesystem table name
        temp[1]=null;//	Output name
        temp[2]=null;// compiler name 
        temp[3]=null;//	linker name
        int input = 4;

        try {
        for (int i = 0;; i++)
        {
            //        System.out.println("cmdLine = "+cmdLine[i]);
            if (cmdLine[i].indexOf("-I") >= 0)
            {
                temp[input++] = cmdLine[++i];
            }
            else if (cmdLine[i].indexOf("-O") >= 0)
            {
                temp[1] = cmdLine[++i];
            }
            else if (cmdLine[i].indexOf("-N") >= 0)
            {
                temp[0] = cmdLine[++i];
            }
            else if (cmdLine[i].indexOf("-C") >= 0)
            {
                temp[2] = cmdLine[++i];
            }
            else if (cmdLine[i].indexOf("-L") >= 0)
            {
                temp[3] = cmdLine[++i];
            }
        }
        } catch ( Exception e ) {}

        String[] returnTab = new String[input];
        for(int i = 0; i < input; i++)
        {
            returnTab[i]=temp[i];
        }

        return returnTab;
    }

    class FilePattern implements FileFilter{
        Pattern pattern;
        public FilePattern (String pattern) {
            pattern = pattern.replaceAll("\\*", ".*");
            this.pattern = Pattern.compile(pattern);
        }
        public boolean accept(File dir) {
            if (dir.isDirectory()) {
                return true;
            } else {
                return pattern.matcher(dir.getName()).matches();
            }
        }
    }

    void ProcessInput2(File file, Stack<String> prefix_list, File outputdir, String pattern, boolean strip) throws java.io.IOException
    {
    	file = file.getAbsoluteFile();// normally is already given in absolutepath

        if(file.isDirectory())
        {
        	File files[] = file.listFiles( new FilePattern(pattern));
            if(files != null) 
            {
            	prefix_list.push(file.getName());
                for	(int i =0; i<files.length; i++) {
                	ProcessInput2(files[i], prefix_list, outputdir, pattern, strip);
                }
                prefix_list.pop();
            }
        }
        else if (file.isFile())
        {
            String prefix_o = "";
            String prefix_i = "";
            
            // we don't want the first one
            for (int i = 1; i < prefix_list.size(); i++)
            {
            	prefix_o = prefix_o + "_" + prefix_list.get(i);
            	prefix_i = prefix_i + "/" + prefix_list.get(i);
            }
            
        	File of = new File(outputdir, prefix_o + "_" + file.getName() + ".o");
            String fs_if = prefix_i + "/" + file.getName();
            if(fs_if.startsWith("/"))
            	fs_if = fs_if.substring(1); 
            
            if(strip && file.getName().endsWith(".elf4nmf")) {
                
            	File strippedfile = new File(outputdir, prefix_o + "_" + file.getName() + ".strip");
                needRecompilation |= armtools.CreateObjFile(file, of, strippedfile);
                m_listFiles.add(new FILE(fs_if, strippedfile.getCanonicalPath(), of));                
            } else {
                needRecompilation |= armtools.CreateObjFile(file, of, null);
                m_listFiles.add(new FILE(fs_if, file.getCanonicalPath(), of));                
            }
        }
    }	

}
