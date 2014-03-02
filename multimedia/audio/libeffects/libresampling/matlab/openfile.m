function [fid]=openfile(filename);
fid = fopen(filename,'w');
fprintf(fid, '/*****************************************************************************/\n');
fprintf(fid, '/*\n');
fprintf(fid, ' * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.\n');
fprintf(fid, ' * This code is ST-Ericsson proprietary and confidential.\n');
fprintf(fid, ' * Any use of the code for whatever purpose is subject to\n');
fprintf(fid, ' * specific written permission of ST-Ericsson SA.\n');
fprintf(fid, ' *\n');
fprintf(fid, ' */\n');
fprintf(fid, '\n');
fprintf(fid, '/**\n');
fprintf(fid, ' * \file   %s\n',filename);
fprintf(fid, ' * \brief  \n');  
fprintf(fid, ' * \author ST-Ericsson\n');
fprintf(fid, ' */\n');
fprintf(fid, '/*****************************************************************************/\n');

