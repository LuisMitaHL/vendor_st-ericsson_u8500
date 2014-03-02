
#ifndef _NVM_H_
#define _NVM_H_

#define CAM_DRV_NVM_MAX_NUM_OF_LS_AREAS		(19*11)		// maximum LS areas count, actual counts on each direction (H & V) have to be odd numbers (so that a "central" area is there
#define CAM_DRV_NVM_MAX_NUM_OF_LS_TABLES	5			// maximum LS characterizations (illumint + focus position + zoom position combination) count
#define CAM_DRV_NVM_MAX_SENS_DATA_NUM		5			// maximum WB characterizations (illumint) count     
#define CAM_DRV_NVM_MAX_DEFECT_NUM			500			// maximum sensor's defects count
#define CAM_DRV_NVM_MAX_AF_LENS_POS			10			// maximum number of characterized lens positions


/** Structure for specifying the LSC data */

typedef struct
{
    uint16 r;											// red channel
    uint16 gr;											// green red channel
    uint16 b;											// blue channel
    uint16 gb;											// green blue channel
}
CAM_DRV_NVM_LSC_DATA_T;


/** Structure for specifying the LS table */

typedef struct
{
    uint16 lsc_data_offset;								// Byte offset to lsc data from start of this object e.g.
														// CAM_DRV_NVM_LS_TABLE_T* ls_table = (CAM_DRV_NVM_LS_TABLE_T*) some_address;
														// CAM_DRV_NVM_LSC_DATA_T* p_lsc_data = (CAM_DRV_NVM_LSC_DATA_T*) ((char *) ls_table + ls_table.lsc_data_offset);

    uint16 x_value_x1000;								// Illumination data of measurement conditions, 1000 times X-coordinate in CIE 1931
    uint16 y_value_x1000;								// Illumination data of measurement conditions, 1000 times Y-coordinate in CIE 1931

    uint8 x_division;									// number of areas in horizontal
    uint8 y_division;									// number of areas in vertical
    uint8 ch;											// number of color channels: 3 or 4
    uint8 center_bit_depth;								// Size (no. of bits) for center entry
    uint8 other_bit_depth;								// Size (no. of bits) for other entries, if same as center_bit_depth then values are absolute, otherwise relative to center values

    uint8 object_distance;								// lens position info of measurement conditions, Object distance

    uint8 zoom_factor;									// lens position info of measurement conditions, zoom factor
    uint8 padding1;

}
CAM_DRV_NVM_LS_TABLE_T;


/** Structure for specifying the Sensitivity data */

typedef struct
{
    CAM_DRV_NVM_LSC_DATA_T sens;						// Sensitivity of the image center area
    uint16 x_value_x1000;								// Illumination data of measurement conditions: 1000 times X- coordinate in CIE 1931 (x,y)
    uint16 y_value_x1000;								// Illumination data of measurement conditions: 1000 times Y-coordinate in CIE 1931 (x,y)
}
CAM_DRV_NVM_SENSITIVITY_DATA_T;


/** Structure for specifying the Defect couplet type1 */

typedef struct
{
    uint16 x;											// X co-ordinates of first pixel in a couplet
    uint16 y;											// Y co-ordinates of first pixel in a couplet

}
CAM_DRV_NVM_DEFECT_COUPLET_TYPE1_T;


/** Structure for specifying the Defect couplet type2 */

typedef struct
{
    uint16 x;                          					// X co-ordinates of first pixel in a couplet
    uint16 y;                          					// Y co-ordinates of first pixel in a couplet
    uint8 dir;                         					// Direction of 2nd pixel relative to first: 0= right, 1= bottom-right, 2= bottom, 3= bottom-left
    uint8 padding1;
    uint8 padding2;
    uint8 padding3;

}
CAM_DRV_NVM_DEFECT_COUPLET_TYPE2_T;


/** Structure for specifying the Defect line */

typedef struct
{
    uint16 XorY;                          				// Line/column number of defective line/column
    uint8 type;                          				// 0= line defect, 1= column defect
    uint8 padding1;
    uint8 padding2;
    uint8 padding3;
}
CAM_DRV_NVM_DEFECT_LINE_T;


/** Structure for specifying the LS data block */

typedef struct
{
    uint16 ls_measured_lp_nums_offset;         			// Offset from beginning of this object to ls_measured_temp_num times 8bit entries each indicating in
                          								// how many lens positions values are measured in each of temperatures
    uint16 ls_comTable_offset;         					// Offset from beginning of this object to ls_table_num times CAM_DRV_NVM_LS_TABLE_T entries
    uint8 ls_measured_temp_num;         				// in how many colour temperatures LS values are measured
    uint8 ls_table_num;         						// total number of CAM_DRV_NVM_LS_TABLE_T items present at p_ls_comTable, value = p_ls_measured_lp_nums[0] + ....
         												// + p_ls_measured_lp_nums[ls_measured_temp_num-1]
    uint8 padding1;
    uint8 padding2;
}
CAM_DRV_NVM_LS_DATA_BLOCK_T;


/** Structure for specifying the focus data block */

typedef struct
{
    uint16 C_F_object_distances_offset;					// Offset from beginning of this object to 16-bit values. A 16-bit number describing the object distance in mm for each measured position from infinity(excluding) to near end (excluding)
    uint16 additional_af_data_offset;					// Offset from beginning of this objec to additional AF data, if any. 0 if no other data available.

	uint16 positions_horizontal[CAM_DRV_NVM_MAX_AF_LENS_POS];	// In total there are 10 times 16 bit fields, out of which total_positions_h number of 16 bit values are valid, each corresponding to sensor reading or control
         														// word of different steps from far to near. Values are absolute. NULL if data is not available
    

    uint16 positions_upward[CAM_DRV_NVM_MAX_AF_LENS_POS];		// In total there are 10 times 16 bit fields, out of which total_positions_u number of 16 bit values are valid, each corresponding to sensor reading or control
         														// word of different steps from far to near. Values are absolute. NULL if data is not available


    uint16 positions_downward[CAM_DRV_NVM_MAX_AF_LENS_POS];		// In total there are 10 times 16 bit fields, out of which total_positions_d number of 16 bit values are valid, each corresponding to sensor reading or control
         														// word of different steps from far to near. Values are absolute. NULL if data is not available


    uint16 obj_dis_availability_bitmask;				// Which data is available at p_C_F_object_distances. Each set bit of this bitmask indicates presence of corresponding field as specified by AF_positions_availability_bitmask
         												// Available positions are stored at p_C_F_object_distances in same order as appear in CAM_DRV_NVM_AF_POSITIONS_ORDER_E.

    uint16 size_of_additional_af_data;					// Size of data (bytes) pointed by p_additional_af_data. If available and needed, this data needs to be parsed by client.

    uint16 fine_step_im;								// Number of units of Focus control word that corresponds to 5% MTF drop in focus change register or fine step movement in infinity to macro direction.

    uint16 fine_step_mi;								// Number of units of Focus control word that corresponds to 5% MTF drop in focus change register or fine step movement in macro to infinity direction.


    uint16 availability_bitmask_h;						// Which data is available at p_positions_horizontal. Each set bit indicates presence of corresponding field as indicated by AF_positions_availability_bitmask
         												// Available positions are stored at p_positions_horizontal in same order as appear in CAM_DRV_NVM_AF_POSITIONS_ORDER_E.
    uint16 availability_bitmask_u;						// Which data is available at p_positions_upward. Each set bit indicates presence of corresponding field as indicated by AF_positions_availability_bitmask
         												// Available positions are stored at p_positions_upward in same order as appear in CAM_DRV_NVM_AF_POSITIONS_ORDER_E.
    uint16 availability_bitmask_d;						// Which data is available at p_positions_downward. Each set bit indicates presence of corresponding field as indicated by AF_positions_availability_bitmask
         												// Available positions are stored at p_positions_downward in same order as appear in CAM_DRV_NVM_AF_POSITIONS_ORDER_E.

    uint8 pos_unit_relation_to_cw;						// How many focus change units one reading of position register corresponds to, 0 if not available

    uint8 total_positions_h;							// How many positions stored for horizontal orientation

    uint8 total_positions_u;							// How many positions stored for upward orientation

    uint8 total_positions_d;							// How many positions stored for downward orientation


}
CAM_DRV_NVM_AF_DATA_BLOCK_T;


/** Structure for specifying the Sensitivity data block */

typedef struct
{
    uint16 sens_offset;									// Offset from beginning of this object to sensitivity data, sensitivity_colour_temps number of CAM_DRV_NVM_SENSITIVITY_DATA_T values
    uint8 sensitivity_colour_temps;						// in how many colour temperatures values are measured
    uint8 padding1;
}
CAM_DRV_NVM_SENSITIVITY_DATA_BLOCK_T;


/** Structure for specifying the defects data block */

typedef struct
{
    /**<  Defect data
     Type1 only provides co-ordinates of first pixel in a couplet, no information on other pixel
     Type2 provides co-ordinates of first pixel as well as direction of other pixel relative to first */

    uint16 couplet_type1_map_offset;					// Offst from beginning of this object to num_of_couplet_type1_defects times
														// CAM_DRV_NVM_DEFECT_COUPLET_TYPE1_T type of data
    uint16 couplet_type2_map_offset;					// Offst from beginning of this object to num_of_couplet_type2_defects times
														// CAM_DRV_NVM_DEFECT_COUPLET_TYPE2_T type of data
    uint16 line_map_offset;								// Offst from beginning of this object to num_of_line_defects times
														// CAM_DRV_NVM_DEFECT_LINE_T type of data

    uint16 num_of_couplet_type1_defects;				// Number of defects of type 1

    uint16 num_of_couplet_type2_defects;				// Number of defects of type 2

    uint16 num_of_line_defects;							// Number of full line or column defects
}
CAM_DRV_NVM_DEFECT_DATA_BLOCK_T;


/** Structure for specifying the mechanical shutter / ND filter data block */

typedef struct
{
    uint16 shut_delay_us;								// shutter delay in micro seconds, 0 if no shutter
    uint16 ND_trans_x100;								// ND filter transparency, % value multiplied by 100, 0 if no ND filter

}
CAM_DRV_NVM_ND_MS_DATA_BLOCK_T;


/** Structure for specifying the black offset data */

typedef struct
{
    uint8 r;											// Red channel
    uint8 gr;											// Green red channel
    uint8 b;											// Blue channel
    uint8 gb;											// Green blue channel
}
CAM_DRV_NVM_SPATIAL_PEDESTAL_DATA_T;


/** Structure for specifying the black offset table */

typedef struct
{
    uint16 spatial_pedestal_data_offset;				// Offset from beginning of this object to  width * height times CAM_DRV_NVM_SPATIAL_PEDESTAL_DATA_T data

    uint8 width;										// width of table (matrix)
    uint8 height;										// height of table (matrix)
    uint8 analog_gain;									// analog gain value for which this table's values are valid
    uint8 padding1;
    uint8 padding2;
    uint8 padding3;

}
CAM_DRV_NVM_SPATIAL_PEDESTAL_TABLE_T;


/** Structure for specifying the black offset data block */

typedef struct
{
    uint16 spatial_pedestal_table_offset;				// Offset from beginning of this object to  number_of_tables times CAM_DRV_NVM_SPATIAL_PEDESTAL_TABLE_T data
    uint8 number_of_tables;								// number of spatial pedestal tables at spatial_pedestal_table, 0 if data not available
    uint8 padding1;

}
CAM_DRV_NVM_SPATIAL_PEDESTAL_TABLES_BLOCK_T;

typedef struct
{
	uint16 width;
	uint16 height;
}
CAM_DRV_NVM_SENS_RESOLUTION_T;


/** Structure for specifying NVM data */

typedef struct
{
    CAM_DRV_NVM_SENS_RESOLUTION_T sens_resolution;							// Maximum sensor resolution
    CAM_DRV_NVM_AF_DATA_BLOCK_T af_data;									// Focus lens related NVM data
    CAM_DRV_NVM_LS_DATA_BLOCK_T ls_data;									// Lens shading related NVM data
    CAM_DRV_NVM_SENSITIVITY_DATA_BLOCK_T sensitivity_data;					// Sensitivity related NVM data
    CAM_DRV_NVM_DEFECT_DATA_BLOCK_T defect_data;							// Defect pixels related NVM data
    CAM_DRV_NVM_ND_MS_DATA_BLOCK_T nd_ms_data;								// ND filter and Shutter related NVM data
    CAM_DRV_NVM_SPATIAL_PEDESTAL_TABLES_BLOCK_T spatial_pedestal_data;		// spatial pedestal data tables

    uint32 checksum;														// stored checksum
    uint32 checksum_calculated;												// calculated checksum

    uint32 number_of_errors;
    uint32 checksum2;
    uint32 checksum2_calculated;

    uint32 decoder_status;													// ecoding err codes if any

    uint32 total_data_size;													// Total size of data, sum of all fields in all structures

    /**< if a certain subblock of data is not present then set corresponding offset to 0 */
    
    uint16 offset_af_data_object_distances;									// Offset from beginning of CAM_DRV_NVM_T to uint16 type object distance data */
    uint16 offset_af_additional_af_data;									// Offset from beginning of CAM_DRV_NVM_T to additional af data, type not specified */
    uint16 offset_ls_no_of_lens_positions;									// Offset from beginning of CAM_DRV_NVM_T to uint8 type data measured_lp_nums */
    uint16 offset_com_tables;												// Offset from beginning of CAM_DRV_NVM_T to CAM_DRV_NVM_LS_TABLE_T type data */
    uint16 offset_ls_lsc_data;												// Offset from beginning of CAM_DRV_NVM_T to CAM_DRV_NVM_LSC_DATA_T type data */
    uint16 offset_sensitivity_data;											// Offset from beginning of CAM_DRV_NVM_T to CAM_DRV_NVM_SENSITIVITY_DATA_T type data */
    uint16 offset_defect_couplet_type1;										// Offset from beginning of CAM_DRV_NVM_T to CAM_DRV_NVM_DEFECT_COUPLET_TYPE1_T type data */
    uint16 offset_defect_couplet_type2;										// Offset from beginning of CAM_DRV_NVM_T to CAM_DRV_NVM_DEFECT_COUPLET_TYPE2_T type data */
    uint16 offset_defect_line;												// Offset from beginning of CAM_DRV_NVM_T to CAM_DRV_NVM_DEFECT_LINE_T type data */
    uint16 offset_pedestal_table;											// Offset from beginning of CAM_DRV_NVM_T to CAM_DRV_NVM_SPATIAL_PEDESTAL_TABLE_T type data */
    uint16 offset_pedestal_data;											// Offset from beginning of CAM_DRV_NVM_T to CAM_DRV_NVM_SPATIAL_PEDESTAL_DATA_T type data */

    uint8 padding1;
    uint8 padding2;

}
CAM_DRV_NVM_T;

#endif /* _NVM_H_ */
