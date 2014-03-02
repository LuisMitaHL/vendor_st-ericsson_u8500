/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
#include <cm/os/api/repository_mgt_os.h>
#include <cm/proxy/common/repository/inc/repository.h>

t_cm_error cm_openComponent(const char *templateName, t_nmf_osal_fs_file_handle *file, void **data, t_cm_size *size, t_bool userspace)
{
    char filename[MAX_TEMPLATE_NAME_LENGTH+9]; /* +9 for filename extension */
    int i;

    /*
     * Create the filename where the component template must be found
     */
    for(i = 0; templateName[i] != 0 && i < MAX_TEMPLATE_NAME_LENGTH; i++) {
        filename[i] = (templateName[i] == '.') ? '/' : templateName[i];
    }

    if (i >= MAX_TEMPLATE_NAME_LENGTH)
        return CM_OUT_OF_LIMITS;

    filename[i++] = '.';
    filename[i++] = 'e';
    filename[i++] = 'l';
    filename[i++] = 'f';
    filename[i++] = '4';
    filename[i++] = 'n';
    filename[i++] = 'm';
    filename[i++] = 'f';
    filename[i++] = '\0';

    *file = OSAL_ReadFile(filename, data, size, userspace);
    if (*file == 0)
    {
        OSAL_Log("Error: CM_COMPONENT_NOT_FOUND: %s\n", (void *)templateName, 0, 0);
        return CM_COMPONENT_NOT_FOUND;
    }

    return CM_OK;
}

void cm_closeComponent(t_nmf_osal_fs_file_handle file)
{
    OSAL_CloseFile(file);
}

t_cm_error cm_pushComponent(int nb, char fileList[][MAX_INTERFACE_TYPE_NAME_LENGTH])
{
	int i;

	for(i = 0; i < nb; i++)
	{
	    if(fileList[i][0] != 0)
	    {
	        t_cm_error error;
	        t_nmf_osal_fs_file_handle file;
	        void *data;
	        t_cm_size size;

	        if((error = cm_openComponent(fileList[i], &file, &data, &size, 0)) != CM_OK)
	            return error;

	        error = CM_OS_PushComponent(fileList[i], data, size);

	        cm_closeComponent(file);

	        if(error != CM_OK)
	        {
	            for (i--; i>=0; i--)
	                if(fileList[i][0] != 0)
	                    CM_OS_ReleaseComponent(fileList[i]);

	            return error;
	        }
	    }
	}

	return CM_OK;
}

t_cm_error cm_openComponents(
        int nb,
        char fileList[][MAX_INTERFACE_TYPE_NAME_LENGTH],
        t_nmf_osal_fs_file_handle files[],
        void *datas[],
        t_cm_size sizes[],
        t_bool userspace)
{
    t_cm_error error;
    int n;

    for(n = 0; n < nb; n++)
    {
        // Set that component has not been loaded !!!
        files[n] = NULL;
        datas[n] = NULL;
        sizes[n] = 0;
    }

    for(n = 0; n < nb; n++)
    {
        if(fileList[n][0] != 0)
        {
            if((error = cm_openComponent(fileList[n], &files[n], &datas[n], &sizes[n], userspace)) != CM_OK)
            {
                for(; --n >= 0; )
                    cm_closeComponent(files[n]);
                return error;
            }
        }
    }

    return CM_OK;
}

void cm_closeComponents(
        int nb,
        t_nmf_osal_fs_file_handle files[])
{
    int n;

    for(n = 0; n < nb; n++)
    {
        if(files[n] != NULL)
            cm_closeComponent(files[n]);
    }
}
