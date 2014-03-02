/*****************************************************************************/
/**
*  ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   This module provides routines to manage ab dth functions
* \author  ST-Ericsson
*/
/*****************************************************************************/

#include "dthab.h"
#include "tatlab.h"

char * filename = "/var/local/tat/tatlab/AB.xml";
struct dth_element *p_BF_elements;
int v_NbParams = 0;

int dth_init_service()
{
	int vl_result = 0; 
	int result = 0, i = 0;
 	struct dth_element *ptr = NULL;

	result = count_elements2(filename) + 4; //one for Version, two for menus, one null
	//printf("AB8500 : Counted %d elements\n", result);
	p_BF_elements = dth_element_alloc2(result);
	printf("AB8500 : Allocated %i elements\n", result);

	ptr = p_BF_elements;
	/* Version element */
	strcpy((char*)ptr->path, "/DTH/AB/VERSION/Version");
	ptr->type = DTH_TYPE_STRING;
	ptr->enumString = NULL;
	ptr->info = NULL;
	ptr->get = get_version;

	v_NbParams = dth_xml_init_service(p_BF_elements, filename, (ptf_get)DthAB_get, (ptf_set)DthAB_set, (ptf_exec)DthAB_exec);
	//printf("AB8500 : Created %d elements\n\n", v_NbParams);	
	//printf("AB8500 : Saved %d elements\n\n", result);

    	if(v_NbParams > 0)
    	{
		/*printf("So there are %d elements to register\n\n", result);*/
        	ptr = p_BF_elements;
        	for(i = 0; i < v_NbParams-1; i++)
        	{
			/*if (strncmp(ptr->path, "/DTH/AB/config", 14) == 0)
            		printf("Element[%d] ->  path:%s, type:%i, cols:%i, rows:%i, enum:%s, info:%s, min:%lld, max:%lld, user_data: %d, get: %d, set:%d, exec:%d\n", i,ptr->path, ptr->type, ptr->cols, ptr->rows, ptr->enumString, ptr->info, ptr->min, ptr->max, ptr->user_data, (int)(ptr->get), (int)(ptr->set), (int)(ptr->exec)); */
            		vl_result = dth_register_element(ptr);
            		if(vl_result < 0)
			{
				printf("Can't register element! \n");
				printf("Element[%d] ->  path:%s, type:%i, cols:%i, rows:%i, enum:%s, info:%s, min:%lld, max:%lld, user_data: %d, get: %d, set:%d, exec:%d\n", i,ptr->path, ptr->type, ptr->cols, ptr->rows, ptr->enumString, ptr->info, ptr->min, ptr->max, ptr->user_data, (int)(ptr->get), (int)(ptr->set), (int)(ptr->exec));
				printf("vl_result = %d", vl_result);
				break ;
			}
            		ptr++;
        	}
    	}
	return vl_result ;
}

void dth_uninit_service()
{
    if ( NULL != p_BF_elements ) {

#if 0
        /* unregister parameters */
        struct dth_element *param = p_BF_elements;
        int i;
        for ( i = 0; i < v_NbParams; i++ )
        {
            dth_unregister_element(param);
            param ++;
        }
#endif
        free(p_BF_elements);
        p_BF_elements = NULL;
    }
}

int DthAB_exec(struct dth_element *elem)
{
	int result = 0;
    result = tatl1_00ActAB_exec(elem);
	return result;
}

int DthAB_set(struct dth_element *elem, void *value)
{
	int result = 0;
    result = tatl1_02ActAB_Set(elem, value);
	return result;
}

int DthAB_get(struct dth_element *elem, void *value)
{
	int result = 0;
    result = tatl1_03ActAB_Get(elem, value);
	return result;
}

int count_elements2(char* filename)
{
	FILE *file = NULL;
	int nmb = 0;
	char s_buffer[MAX_CHAR_PER_LINE];

	if ((file = fopen(filename, "r")) == NULL)
        {
            printf("fail to open file %s \n", filename);	
            nmb = -1;	
        }
	else
	{
		memset(s_buffer, 0,MAX_CHAR_PER_LINE) ;
		fgets (s_buffer, MAX_CHAR_PER_LINE, file);
		while (!feof(file))
    		{
        		fgets (s_buffer, MAX_CHAR_PER_LINE, file);
        		if (( strncmp(s_buffer, "<action", 7) == 0) || ( strncmp(s_buffer, "<argument", 7) == 0) )
        		{
            			nmb++;
        		}
    		}
	}
	fclose(file);
	return nmb;
}

struct dth_element * dth_element_alloc2(int nb_element) 
{

    int i;
    struct dth_element *p_elements = NULL;
    struct dth_element *copie = NULL;

    p_elements= calloc(nb_element ,sizeof(struct dth_element));
    if(p_elements == NULL)	
        goto alloc_error ;
    copie = p_elements ;

    for(i=0; i<nb_element; i++)
    {
        p_elements->path = calloc(MAX_LOCAL_STRING_SIZE,sizeof(char));
        if(p_elements->path == NULL)
            goto alloc_error ;

        p_elements->type = 0 ;
        p_elements->cols = 0 ;
        p_elements->rows = 0 ;

        p_elements->enumString = calloc(7000,sizeof(char));
        if(p_elements->enumString == NULL)
            goto alloc_error ;

        p_elements->info = calloc(MAX_LOCAL_STRING_SIZE,sizeof(char));
        if(p_elements->info == NULL)
            goto alloc_error ;

        p_elements->min = 0 ;
        p_elements->max = 0 ;
        p_elements->get = NULL ;
        p_elements->set = NULL ;
        p_elements->exec = NULL ;
        p_elements->user_data = 0 ;
        p_elements->default_value =  NULL ;
        p_elements++ ;
    }	

    p_elements = copie ;
    return p_elements;

alloc_error :
    if( p_elements != NULL){
        free(p_elements);
    }
    return NULL;	
}

int get_version(struct dth_element *elem, void *value)
{
    int vl_len = 32;
    char version[32+1];
		if (elem == NULL)
		return 0;
    
    char command[80];	
    command[0]='\0';
    FILE * fp;
    strcat(command, "md5sum ");
    strcat(command, filename);
    strcat(command, " | awk '{print $1}'");	
    fp = popen(command, "r");
    fgets(version, sizeof(version), fp);
    pclose(fp);

    version[vl_len] = '\0';
    strncpy((char*)value, version, vl_len);	
  
    return 0;
}
