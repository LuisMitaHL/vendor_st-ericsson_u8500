/*
* ============================================================================
*
*       Filename:  tatl01testfunc.c
*
*		Description:
*
*       Version:  1.0
*       Created:  5 May 2011
*       Revision:  none
*       Compiler:
*
*       Author:  VDY,
*       Copyright ST-Ericsson, 2011. All Rights Reserved
*
* ============================================================================
*/

#define TATL01TESTFUNC_C
#include"tatltest.h"
#include "tatl01testfunc.h"
#include <libab.h>
#include <libdb.h>
#undef TATL01TESTFUNC_C

DB_ACTION_REGISTER dbregister_action_data = { 0, 0, 0 };
AB_ACTION_REGISTER abregister_action_data = { 0, 0, 0 };

/*---------------------------------------------------------------------------*
* Procedure    : tatl_TestGetDthArray
*----------------------------------------------------------------------------*
* Description  : Get an Array
*----------------------------------------------------------------------------*
* Parameter(s) : dth_element *, the  DTH element
*----------------------------------------------------------------------------*
*
*----------------------------------------------------------------------------*
*
*----------------------------------------------------------------------------*
*
*----------------------------------------------------------------------------*
* Return Value : u32, TAT error
*---------------------------------------------------------------------------*/
int tatl_TestGetDthArray(struct dth_element *pl_elem,
			 struct dth_array *pl_array, int table_name)
{
	/* load source data */
	int col = pl_array->col;
	int row = pl_array->row;
	u8 *DestArray = (u8 *) pl_array->array;
	int ElementSize = tatl_ElementSize(pl_elem->type);
	int size = 0;
	u8 *pl_Source;
	int loop;
	int vl_Error = TAT_ERROR_OFF;

	if (ElementSize <= 0)
		return -1;

	switch (table_name) {
	case U8_1_ROW_1COL:
		pl_Source = (u8 *) u8_1_1;
		break;

	case U16_1_ROW_1COL:
		pl_Source = (u8 *) u16_1_1;
		break;

	case U32_1_ROW_1COL:
		pl_Source = (u8 *) u32_1_1;
		break;

	case U64_1_ROW_1COL:
		pl_Source = (u8 *) u64_1_1;
		break;

	case U8_2_ROW_2COL:
		pl_Source = (u8 *) u8_2_2;
		break;

	case U16_2_ROW_2COL:
		pl_Source = (u8 *) u16_2_2;
		break;

	case U32_2_ROW_2COL:
		pl_Source = (u8 *) u32_2_2;
		break;

	case U64_2_ROW_2COL:
		pl_Source = (u8 *) u64_2_2;
		break;

	default:
		return -1;
		break;
	}

	if (DestArray != NULL) {
		if (tatl_debug)
			printf("\n get ARRAY elem : col %d row %d",
			       pl_elem->cols, pl_elem->cols);

		if ((col == pl_elem->cols) && (row == pl_elem->rows)) {
			/* Access to the whole array. */
			int r = 0;
			int c = 0;

			if (tatl_debug)
				printf("\n get ARRAY : whole table");
			for (r = 0; (r < pl_elem->rows); r++) {
				for (c = 0; (c < pl_elem->cols);
				     c++, DestArray += ElementSize, pl_Source +=
				     ElementSize) {
					memcpy(DestArray, pl_Source,
					       ElementSize);
					if (tatl_debug) {
						for (loop = 0;
						     loop < ElementSize;
						     loop++) {
							printf
							    ("\n --------whole---------------");
							printf
							    ("\n srce row %d col %d data %02x:",
							     r, c,
							     *(pl_Source +
							       loop));
							printf
							    ("\n dest row %d col %d data %02x:",
							     r, c,
							     *(DestArray +
							       loop));
						}
					}
					size += ElementSize;
				}
			}

		} else if ((pl_array->col < pl_elem->cols)
			   && (pl_array->row < pl_elem->rows)) {
			/* fill the cell (row, col) at array[0] */
			int offset_data =
			    (row * pl_elem->cols + col) * ElementSize;

			if (tatl_debug)
				printf("\n get ARRAY : one element ");
			memcpy(DestArray, pl_Source + offset_data, ElementSize);
			if (tatl_debug) {
				printf("\n ---------one--------------");
				printf("\n  srce data %02x:",
				       *(pl_Source + offset_data));
				printf("\n  dest data %02x:",
				       *(DestArray + offset_data));
			}
			size += ElementSize;

		} else {
			size = -1;
			/* error: array col or/and row are out of range. */
			SYSLOG(LOG_ERR,
			       "error: cell (c:%d, r:%d) is out of range!\n",
			       pl_array->col, pl_array->row);
		}
	} else {
		printf("\n get ARRAY : Destination pointer NULL");
	}

	return vl_Error;
}

/*---------------------------------------------------------------------------*
* Procedure    : tatl_TestSetDthArray
*----------------------------------------------------------------------------*
* Description  : Set an array
* *----------------------------------------------------------------------------*
* Parameter(s) : dth_element *, the  DTH element
*----------------------------------------------------------------------------*
* Return Value : u32, TAT error
*---------------------------------------------------------------------------*/
int tatl_TestSetDthArray(struct dth_element *pl_elem,
			 struct dth_array *pl_array, int table_name)
{

	int col = pl_array->col;
	int row = pl_array->row;
	u8 *vl_Source = (u8 *) pl_array->array;
	int ElementSize = tatl_ElementSize(pl_elem->type);
	int size = 0;
	int loop;
	u8 *pl_Dest;
	int vl_Error = TAT_ERROR_OFF;

	if (ElementSize <= 0)
		return -1;

	switch (table_name) {
	case U8_1_ROW_1COL:
		pl_Dest = (u8 *) u8_1_1;
		break;

	case U16_1_ROW_1COL:
		pl_Dest = (u8 *) u16_1_1;
		break;

	case U32_1_ROW_1COL:
		pl_Dest = (u8 *) u32_1_1;
		break;

	case U64_1_ROW_1COL:
		pl_Dest = (u8 *) u64_1_1;
		break;

	case U8_2_ROW_2COL:
		pl_Dest = (u8 *) u8_2_2;
		break;

	case U16_2_ROW_2COL:
		pl_Dest = (u8 *) u16_2_2;
		break;

	case U32_2_ROW_2COL:
		pl_Dest = (u8 *) u32_2_2;
		break;

	case U64_2_ROW_2COL:
		pl_Dest = (u8 *) u64_2_2;
		break;

	default:
		return -1;
		break;
	}

	if (tatl_debug) {
		for (loop = 0; loop < ElementSize; loop++) {
			printf("\n input data : %d %02x:", loop,
			       *(((u8 *) vl_Source) + loop));
		}

		printf("\n set ARRAY : col %d row %d", col, row);
	}

	if (pl_Dest != NULL) {
		if ((col == pl_elem->cols) && (row == pl_elem->rows)) {
			/* Access to the whole array. */
			int r = 0;
			int c = 0;

			if (tatl_debug)
				printf("\n set ARRAY : whole table");
			for (r = 0; (r < pl_elem->rows); r++) {
				for (c = 0; (c < pl_elem->cols);
				     c++, pl_Dest += ElementSize, vl_Source +=
				     ElementSize) {
					memcpy(pl_Dest, vl_Source, ElementSize);
					if (tatl_debug) {
						for (loop = 0;
						     loop < ElementSize;
						     loop++) {
							printf
							    ("\n --------whole---------------");
							printf
							    ("\n srce row %d col %d data %02x:",
							     r, c,
							     *(((u8 *)
								vl_Source) +
							       loop));
							printf
							    ("\n dest row %d col %d data %02x:",
							     r, c,
							     *(((u8 *) pl_Dest)
							       + loop));
						}
					}
					size += ElementSize;
				}
			}
		} else if ((pl_array->col < pl_elem->cols)
			   && (pl_array->row < pl_elem->rows)) {
			/* fill the cell (row, col) at array[0] */
			int offset_data =
			    (row * pl_elem->cols + col) * ElementSize;

			if (tatl_debug)
				printf("\n set ARRAY : one element ");
			memcpy(pl_Dest + offset_data, vl_Source, ElementSize);
			if (tatl_debug) {
				for (loop = 0; loop < ElementSize; loop++) {
					printf("\n ---------one--------------");
					printf("\n  srce data %02x:",
					       *(((u8 *) vl_Source) +
						 offset_data + loop));
					printf("\n  dest data %02x:",
					       *(((u8 *) pl_Dest) +
						 offset_data + loop));
				}
			}
			size += ElementSize;

		} else {
			printf("\n set ARRAY : error");
			size = -1;
			/* error: array col or/and row are out of range. */
			SYSLOG(LOG_ERR,
			       "error: cell (c:%d, r:%d) is out of range!\n",
			       pl_array->col, pl_array->row);
		}
	}

	return vl_Error;
}

/*---------------------------------------------------------------------------*
* Procedure    : tatl_TestParam_Exec
*----------------------------------------------------------------------------*
* Description  : Execute the Cmd Action sent by DTH
*----------------------------------------------------------------------------*
* Parameter(s) : dth_element *, the  DTH element
*----------------------------------------------------------------------------*
* Return Value : u32, TAT error
*---------------------------------------------------------------------------*/
int tatl_TestParam_Exec(struct dth_element *elem)
{
	int vl_Error = -1;
	if (elem == NULL)
		return 0;

	if (vl_Error != 0)
		printf
		    ("tatl_TestParam_Exec : no executable element in test list \n");

	return vl_Error;
}

/*---------------------------------------------------------------------------*
* Procedure    : tatl_TestParam_Set
*----------------------------------------------------------------------------*
* Description  : Set the DTH Element Value Input Parameters
*----------------------------------------------------------------------------*
* Parameter(s) : dth_element *, Structure of DTH element
*		  void *, Value of Dth Element
*----------------------------------------------------------------------------*
* Return Value : u32, TAT error
*---------------------------------------------------------------------------*/
int tatl_TestParam_Set(struct dth_element *elem, void *Value)
{
	int vl_Error = TAT_ERROR_OFF;

	/* Arrays */
	if (elem->cols >= 1 && elem->rows >= 1)
		return tatl_TestSetDthArray(elem, (struct dth_array *)Value,
					    elem->user_data);

	if (elem->user_data == DEBUG_TATLTEST)
		tatl_debug = *(u8 *) Value;

	switch (elem->type) {

	case DTH_TYPE_U8:
		TU8[elem->user_data] = *(u8 *) Value;
		break;
	case DTH_TYPE_U8B:
		TU8B[elem->user_data] = *(u8 *) Value;
		break;
	case DTH_TYPE_S8:
		TS8[elem->user_data] = *(s8 *) Value;
		break;
	case DTH_TYPE_U16:
		TU16[elem->user_data] = *(u16 *) Value;
		break;
	case DTH_TYPE_U16B:
		TU16B[elem->user_data] = *(u16 *) Value;
		break;
	case DTH_TYPE_S16:
		TS16[elem->user_data] = *(s16 *) Value;
		break;
	case DTH_TYPE_U32:
		{
			u32 val = *(u32 *) Value;
			if(elem->user_data == ASYNC_EVENT) {
				/*Send an event to request a read if value is > 0*/
				if(val > 0)	{
					val--;
					/* send async event */
					libtatasync_msg_send(VALUE_CHANGED, elem->path);
				}
				asyncval = val;
			} else {
				TU32[elem->user_data] = val;
			}
		}
		break;
	case DTH_TYPE_U32B:
		TU32B[elem->user_data] = *(u32 *) Value;
		break;
	case DTH_TYPE_S32:
		TS32[elem->user_data] = *(s32 *) Value;
		break;
	case DTH_TYPE_U64:
		TU64[elem->user_data] = *(u64 *) Value;
		break;
	case DTH_TYPE_U64B:
		TU64B[elem->user_data] = *(u64 *) Value;
		break;
	case DTH_TYPE_S64:
		TS64[elem->user_data] = *(s64 *) Value;
		break;
	case DTH_TYPE_STRING:
		if ((char *)Value != NULL) {
			int len = strlen((char *)Value);
			if (TString[elem->user_data] != NULL) {
				if (len < TStringSize[elem->user_data]) {
					strncpy(TString[elem->user_data],
							(char *)Value, len);
					TString[elem->user_data][len] = '\0';
				}
			}
		}
		break;
	case DTH_TYPE_FLOAT:
		TF[elem->user_data] = *(float *)Value;
		break;
	default:
		printf("tatl_TestParam_Set problem with TYPE\n");
		vl_Error = TAT_ERROR_CASE;
		break;
	}
	return vl_Error;
}

/*---------------------------------------------------------------------------*
* Procedure    : tatl_TestParam_Get
*----------------------------------------------------------------------------*
* Description  : Get the DTH ELement Value Output Parameters
*----------------------------------------------------------------------------*
* Parameter(s) : dth_element *, Structure of DTH element
*		  void *, Value of Dth Element
*----------------------------------------------------------------------------*
* Return Value : u32, TAT error
*---------------------------------------------------------------------------*/
int tatl_TestParam_Get(struct dth_element *elem, void *Value)
{
	int vl_Error = TAT_ERROR_OFF;
	const char msg_alert[] = { "you successfully test alert notification" };

	/*struct dth_array *vl_array = (struct dth_array*)Value; */

	/* Arrays */
	if (elem->cols >= 1 && elem->rows >= 1) {
		if (tatl_debug) {
			printf("\n tatl_TestParam_Get dtharray %x",
			       (unsigned int)Value);
			printf("\n tatl_TestParam_Get dtharray.value %x",
			       (unsigned int)((struct dth_array *)(Value))->
			       array);
		}
		return tatl_TestGetDthArray(elem, (struct dth_array *)Value,
					    elem->user_data);
	}

	switch (elem->type) {

	case DTH_TYPE_U8:
		if (elem->rows == 0) {
			*((u8 *) Value) = TU8[elem->user_data];
		}		/*else {

				   vl_Error = tatl_TestGetDthArray(elem, vl_array, sizeof(u8), Value);
				   } */
		break;
	case DTH_TYPE_U8B:
		*((u8 *) Value) = TU8B[elem->user_data];
		break;
	case DTH_TYPE_S8:
		*((s8 *) Value) = TS8[elem->user_data];
		break;
	case DTH_TYPE_U16:
		*((u16 *) Value) = TU16[elem->user_data];
		break;
	case DTH_TYPE_U16B:
		*((u16 *) Value) = TU16B[elem->user_data];
		break;
	case DTH_TYPE_S16:
		*((s16 *) Value) = TS16[elem->user_data];
		break;
	case DTH_TYPE_U32:
		{
			u32 val = 0;
			if(elem->user_data == ASYNC_EVENT) {
				val = asyncval;
				/*Send an event to request a read if value is > 0*/
				if(val > 0)	{
					val--;
					asyncval--;
					/* send async event */
					libtatasync_msg_send(VALUE_CHANGED, elem->path);
				}
				else
				{
				  /* send async event */
				  printf("\n tatl_TestParam_Get ... %s",(char * )msg_alert);
				  libtatasync_msg_send_with_alert(NOTIFICATION_ALERT, elem->path, (char*)msg_alert);
				}
			} else {
				val = TU32[elem->user_data];
			}
			*(u32 *) Value = val;
		}
		break;
	case DTH_TYPE_U32B:
		*((u32 *) Value) = TU32B[elem->user_data];
		break;
	case DTH_TYPE_S32:
		*((s32 *) Value) = TS32[elem->user_data];
		break;
	case DTH_TYPE_U64:
		*((u64 *) Value) = TU64[elem->user_data];
		break;
	case DTH_TYPE_U64B:
		*((u64 *) Value) = TU64B[elem->user_data];
		break;
	case DTH_TYPE_S64:
		*((s64 *) Value) = TS64[elem->user_data];
		break;
	case DTH_TYPE_STRING:
		if (TString[elem->user_data] != NULL)
			strncpy((char *)Value, TString[elem->user_data],
				strlen(TString[elem->user_data])+1);
		else
			strncpy((char *)Value, "NULL", 5);
		break;
	case DTH_TYPE_FLOAT:
		*((float *)Value) = TF[elem->user_data];
		break;
	default:
		printf("tatl_TestParam_Get problem with TYPE\n");
		vl_Error = TAT_ERROR_CASE;
		break;
	}

	return vl_Error;
}

/*-----------------------------------*
 * Procedure    : tatl1_00ActDBAction_exec
 *------------------------------------------*
 * Description  : Execute the Cmd Action sent by DTH
 *------------------------------------------*
 * Parameter(s) : dth_element *, the  DTH element
 *------------------------------------------*
 * Return Value : u32, TAT error
 *------------------------------------------*/
int tatl1_00ActDBAction_exec(struct dth_element *elem)
{
	int vl_Error = 0;
	int reg_addr = 0, reg_value = 0;
	if (((uint32_t) elem->user_data) <= 0xFFFFFFFF) {
		if (dbregister_action_data.action == DB_READ) {
			/* Read register */
			reg_addr = dbregister_action_data.address;
			/*printf("Will read 0x%08x register\n", reg_addr); */
			db_u32_read(reg_addr, &dbregister_action_data.value);
			/* Get output value */
			printf("Action Read : 0x%08x in 0x%08x\n",
			       dbregister_action_data.value, reg_addr);
		} else {
			/* Write register */
			reg_addr = dbregister_action_data.address;
			reg_value = dbregister_action_data.value;
			printf("Action Write 0x%08x in  0x%08x\n", reg_value,
			       reg_addr);
			db_u32_write(reg_addr, reg_value);
		}
	} else {
		printf("tatl1_00ActDBAction_exec USERDATA not AVAILABLE\n");
		vl_Error = 1;
	}
	return vl_Error;
}

/*-----------------------------------*
 * Procedure    : tatl1_02ActDBParam_Set
 *------------------------------------------*
 * Description  : Set the DTH Element Value Input Parameters
 *------------------------------------------*
 * Parameter(s) : dth_element *, Structure of DTH element
 *		  void *, Value of Dth Element
 *------------------------------------------*
 * Return Value : u32, TAT error
 *------------------------------------------*/
int tatl1_02ActDBAction_Set(struct dth_element *elem, void *Value)
{
	int vl_Error = 0;
	switch (elem->user_data) {
	case ACT_DB_ACTION_REGISTER:
		dbregister_action_data.action = *(uint8_t *) Value;
		/*printf("Action Value = %i\n", *(uint8_t*)Value); */
		break;
	case ACT_DB_ADDRESS_REGISTER:
		dbregister_action_data.address = *(uint32_t *) Value;
		/*printf("Bitfield Value = 0x%x\n", *(uint32_t*)Value); */
		break;
	case ACT_DB_VALUE_REGISTER:
		dbregister_action_data.value = *(uint32_t *) Value;
		/*printf("Bitfield Value = 0x%x\n", *(uint32_t*)Value); */
		break;
	default:
		printf("tatl1_02ActDBAction_Set TYPE not AVAILABLE\n");
		vl_Error = -6;
		break;
	}
	return vl_Error;

}

/*-----------------------------------*
 * Procedure    : tatl1_03ActDBAction_Get
 *------------------------------------------*
 * Description  : Get the DTH ELement Value Output Parameters
 *------------------------------------------*
 * Parameter(s) : dth_element *, Structure of DTH element
 *		  void *, Value of Dth Element
 *------------------------------------------*
 * Return Value : u32, TAT error
 *------------------------------------------*/
int tatl1_03ActDBAction_Get(struct dth_element *elem, void *Value)
{
	int vl_Error = 0;

	switch (elem->user_data) {
	case ACT_DB_VALUE_REGISTER:
		*((uint32_t *) Value) = dbregister_action_data.value;
		/*printf("Bitfield Value = 0x%x\n", *(uint32_t*)Value); */
		break;
	default:
		printf("tatl1_03ActDBAction_Get TYPE not AVAILABLE\n");
		vl_Error = 1;
		break;
	}
	return vl_Error;
}

/*-----------------------------------*
 * Procedure    : tatl1_00ActABAction_exec
 *------------------------------------------*
 * Description  : Execute the Cmd Action sent by DTH
 *------------------------------------------*
 * Parameter(s) : dth_element *, the  DTH element
 *------------------------------------------*
 * Return Value : u32, TAT error
 *------------------------------------------*/
int tatl1_00ActABAction_exec(struct dth_element *elem)
{
	int vl_Error = 0;
	int reg_addr = 0, reg_value = 0;
	if (((uint32_t) elem->user_data) <= 0xFFFFFFFF) {
		if (abregister_action_data.action == AB_READ) {
			/* Read register */
			reg_addr = abregister_action_data.address;
			/*printf("Will read 0x%08x register\n", reg_addr); */
			vl_Error =
			    abxxxx_read(reg_addr,
					&abregister_action_data.value);
			/* Get output value */
			printf("Action Read : 0x%08x in 0x%08x\n",
			       abregister_action_data.value, reg_addr);
		} else {
			/* Write register */
			reg_addr = abregister_action_data.address;
			reg_value = abregister_action_data.value;
			printf("Action Write 0x%08x in  0x%08x\n", reg_value,
			       reg_addr);
			vl_Error =
			    abxxxx_write(reg_addr,
					 abregister_action_data.value);
		}
	} else {
		printf("tatl1_00ActABAction_exec USERDATA not AVAILABLE\n");
		vl_Error = 1;
	}
	return vl_Error;
}

/*-----------------------------------*
 * Procedure    : tatl1_02ActABParam_Set
 *------------------------------------------*
 * Description  : Set the DTH Element Value Input Parameters
 *------------------------------------------*
 * Parameter(s) : dth_element *, Structure of DTH element
 *		  void *, Value of Dth Element
 *------------------------------------------*
 * Return Value : u32, TAT error
 *------------------------------------------*/
int tatl1_02ActABAction_Set(struct dth_element *elem, void *Value)
{
	int vl_Error = 0;
	switch (elem->user_data) {
	case ACT_AB_ACTION_REGISTER:
		abregister_action_data.action = *(uint8_t *) Value;
		/*printf("Action Value = %i\n", *(uint8_t*)Value); */
		break;
	case ACT_AB_ADDRESS_REGISTER:
		abregister_action_data.address = *(uint32_t *) Value;
		/*printf("Bitfield Value = 0x%x\n", *(uint32_t*)Value); */
		break;
	case ACT_AB_VALUE_REGISTER:
		abregister_action_data.value = *(uint8_t *) Value;
		/*printf("Bitfield Value = 0x%x\n", *(uint32_t*)Value); */
		break;
	default:
		printf("tatl1_02ActABAction_Set TYPE not AVAILABLE\n");
		vl_Error = -6;
		break;
	}
	return vl_Error;

}

/*-----------------------------------*
 * Procedure    : tatl1_03ActABAction_Get
 *------------------------------------------*
 * Description  : Get the DTH ELement Value Output Parameters
 *------------------------------------------*
 * Parameter(s) : dth_element *, Structure of DTH element
 *		  void *, Value of Dth Element
 *------------------------------------------*
 * Return Value : u32, TAT error
 *------------------------------------------*/
int tatl1_03ActABAction_Get(struct dth_element *elem, void *Value)
{
	int vl_Error = 0;

	switch (elem->user_data) {
	case ACT_AB_VALUE_REGISTER:
		*((uint8_t *) Value) = abregister_action_data.value;
		/*printf("Bitfield Value = 0x%x\n", *(uint32_t*)Value); */
		break;
	default:
		printf("tatl1_03ActABAction_Get TYPE not AVAILABLE\n");
		vl_Error = 1;
		break;
	}
	return vl_Error;
}

/*-----------------------------------*
 * Procedure    : tatl_ElementSize
 *------------------------------------------*
 * Description  : Get the DTH ELement size
 *------------------------------------------*
 * Parameter(s) : type
 *------------------------------------------*
 * Return Value : int or -1
 *------------------------------------------*/
int tatl_ElementSize(int type)
{
	int size;

	switch (type) {

	case DTH_TYPE_U8:
	case DTH_TYPE_U8B:
	case DTH_TYPE_S8:
		size = 1;
		break;

	case DTH_TYPE_U16:
	case DTH_TYPE_U16B:
	case DTH_TYPE_S16:
		size = 2;
		break;

	case DTH_TYPE_U32:
	case DTH_TYPE_U32B:
	case DTH_TYPE_S32:
		size = 4;
		break;

	case DTH_TYPE_U64:
	case DTH_TYPE_U64B:
	case DTH_TYPE_S64:
		size = 8;
		break;

	default:
		size = -1;
		break;
	}

	if (tatl_debug)
		printf("\ntatl_ElementSize %d", size);

	return size;
}
