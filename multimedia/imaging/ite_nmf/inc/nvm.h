/**********************************************************************
*
* 
* Trident SCF - ISA CORE UK
*
*
*          %name:           cam_drv.h %
*          %version:        3 %
*          %created_by:     aazhar %
*          %date_created:   Mon Feb 22 11:43:09 2010 %
*
*
* 
*
**********************************************************************/
/******************************************************************/
/* This file contains nvm data structure to be used as reference  */
/* by host.                                                       */  
/******************************************************************/

#ifndef _NVM_H_
#define _NVM_H_

/** CONSTANTS */
#define CAM_DRV_NVM_MAX_NUM_OF_LS_AREAS 19*11   /**< max smia++ spec area size is 20x12, but need to be odd number in H&V! */
#define CAM_DRV_NVM_MAX_NUM_OF_LS_TABLES 5    /**< max smia++ spec ls tables possible */
#define CAM_DRV_NVM_MAX_SENS_DATA_NUM 5             /**< max smia++ spec sensitivity data numbers */         

#define CAM_DRV_NVM_START_OF_LENS_POSITIONS 102  /**< Used for swordfish NVM parsing, start of lens position */
#define CAM_DRV_NVM_SWORDFISH_LENS_POSITIONS 4   /**< Used for swordfish NVM parsing, number of lens positions */
#define CAM_DRV_NVM_START_OF_SENS_DATA 17        /**< Used for swordfish NVM parsing, start of sensitivity data */

#define LLA_LSC_GRID_HORIZ_COUNT          9 //EEPROM Map document Page 18
#define LLA_LSC_GRID_VERTI_COUNT          7 //EEPROM Map document Page 18

#define LLA_RGBGrGb_CALIBRATED            0x4             /* Number of channels calibrated is 4 (R, G, B, Gb/Gr) */
#define LLA_RGB_CALIBRATED                0x3             /* Number of channels calibrated is 3 (R, G, B) */


/** Enum specifying AF config */
typedef enum
{
	// af_config1 8bits
	CAM_DRV_NVM_H_DATA_EXIST		= 0x01,	// Relative_Hx registers exist
	CAM_DRV_NVM_U_DATA_EXIST		= 0x02,  // Relative_Ux registers exist
	CAM_DRV_NVM_D_DATA_EXIST		= 0x04,  // Relative_Dx registers exist
	CAM_DRV_NVM_H_SOC_EXIST			= 0x08,  // Start_of_controllability_H register exist
	CAM_DRV_NVM_U_SOC_EXIST			= 0x10,  // Start_of_controllability_U register exist
	CAM_DRV_NVM_D_SOC_EXIST			= 0x20,  // Start_of_controllability_D register exist
	CAM_DRV_NVM_SOC_IS_16BIT		= 0x40,  // Start_of_controllability are 16bit=1, 8bit=0
	CAM_DRV_NVM_AF_CONFIG2_EXIST	= 0x80,	// Af_config2 exist

	// af_config2 8bits
	CAM_DRV_NVM_VALIDITY_REGS_FOR_U_D	= 0x01,
	CAM_DRV_NVM_POS_UNIT_REG_EXIST		= 0x02,
	CAM_DRV_NVM_NO_ADD_REGS_EXIST		= 0x04,
	CAM_DRV_NVM_FINESTEP_REGS_EXIST		= 0x08,
	CAM_DRV_NVM_FINESTEP_IS_8BIT		= 0x10,
	CAM_DRV_NVM_FINESTEP_IS_DEF2		= 0x20,
	CAM_DRV_NVM_AF_CONFIG3_EXIST		= 0x80,
	
	// af_config3 8 bits
	CAM_DRV_NVM_AF_CONFIG4_EXIST_H		= 0x01,
	CAM_DRV_NVM_AF_CONFIG4_EXIST_U		= 0x02,
	CAM_DRV_NVM_AF_CONFIG4_EXIST_D		= 0x04,
	CAM_DRV_NVM_OBJECT_DIST_C_EXIST		= 0x08,
	CAM_DRV_NVM_OBJECT_DIST_D_EXIST		= 0x10,
	CAM_DRV_NVM_OBJECT_DIST_E_EXIST		= 0x20,
	CAM_DRV_NVM_OBJECT_DIST_F_EXIST		= 0x40
}CAM_DRV_NVM_AF_CONFIG_E;

/** Enum specifying AF positions */
typedef enum
{
	CAM_DRV_NVM_SOC			= 0,	/**<  start of controllability  
										next are from prev. stored position */
	CAM_DRV_NVM_REL_TO_A	= 1,	/**<   to A   cw difference */
	CAM_DRV_NVM_REL_TO_B	= 2,	/**<   to B   cw difference */
	CAM_DRV_NVM_REL_TO_INF	= 3,	/**<   to INF cw difference */
	CAM_DRV_NVM_REL_TO_C	= 4,	/**<   to C   cw difference */
	CAM_DRV_NVM_REL_TO_D    = 5,	/**<   to D   cw difference */
	CAM_DRV_NVM_REL_TO_E	= 6,	/**<   to E   cw difference */
	CAM_DRV_NVM_REL_TO_F	= 7,	/**<   to F   cw difference */
	CAM_DRV_NVM_REL_TO_EOC	= 8,	/**<   to end cw difference */
	CAM_DRV_NVM_MAX_CW_NUM  = 10
}CAM_DRV_NVM_AF_POSITIONS_E;

/** Enum specifying content of NVM data */
typedef enum 
{
	CAM_DRV_NVM_MS_EXIST		= 0x80,            /**< Mechanical shutter data exists in NVM */
	CAM_DRV_NVM_ND_EXIST		= 0x40,            /**<  ND Filter data exists in NVM */
	CAM_DRV_NVM_SENS_EXIST		= 0x20,					 /**<  Sensitivity data exists in NVM */
	CAM_DRV_NVM_LS_EXIST		= 0x10,            /**< Lens Shading data exists in NVM */
	CAM_DRV_NVM_CONFIG2_EXIST	= 0x08,          /**< Config2 exists in NVM */
	CAM_DRV_NVM_DEFECT_EXIST	= 0x04,					 /**< Defect data exists in NVM */	
	CAM_DRV_NVM_AF_EXIST		= 0x02,						 /**< Autofocus data exists in NVM */	
	CAM_DRV_NVM_ZOOM_EXIST		= 0x01					 /**< Zoom data exists in NVM */		
} CAM_DRV_NVM_CONTENT_E;


/** Enum specifying NVM data error codes */
typedef enum
{
	CAM_DRV_NVM_MS_ADDR_NG			= 0x0001,
	CAM_DRV_NVM_ND_ADDR_NG			= 0x0002,
	CAM_DRV_NVM_SENS_ADDR_NG		= 0x0004,
	CAM_DRV_NVM_SENS_CONFIG_NG		= 0x0008,
	CAM_DRV_NVM_LS_ADDR_NG			= 0x0010,
	CAM_DRV_NVM_LS_DATA_NG			= 0x0020,
	CAM_DRV_NVM_ZOOM_ADDR_NG		= 0x0040,
	CAM_DRV_NVM_ZOOM_DATA_NG		= 0x0080,
	CAM_DRV_NVM_DEFECT_ADDR_NG		= 0x0100,
	CAM_DRV_NVM_DEFECT_DATA_NG		= 0x0200,
	CAM_DRV_NVM_AF_ADDR_NG			= 0x0400,
	CAM_DRV_NVM_CHECKSUM_ADDR_NG	= 0x0800,
	CAM_DRV_NVM_CHECKSUM_NG			= 0x1000,
	CAM_DRV_NVM_NUM_ERR_ADDR_NG		= 0x2000,
	CAM_DRV_NVM_MAP_VERSION_NG		= 0x4000,
	CAM_DRV_NVM_LS_WIDHGT_NG		= 0x8000
}CAM_DRV_NVM_CONTENT_CONFIG_E;

/** Maximum possible defect numbers */
#define CAM_DRV_NVM_MAX_DEFECT_NUM 500

#define CAM_DRV_NVM_MAX_AF_LENS_POS 10

/** Enum for specifying the AF positions */
typedef enum
{
    SOC_exist       = 0x0001,           /**<  Identifier for start of controllability, availability_bitmask_* uses these identifiers  */
    A_exist         = 0x0002,           /**<  Identifier for A, availability_bitmask_* uses these identifiers  */
    B_exist         = 0x0004,           /**<  Identifier for B, availability_bitmask_* uses these identifiers  */
    INF_exist       = 0x0008,           /**<  Identifier for Infinity, availability_bitmask_* uses these identifiers  */
    C_exist         = 0x0010,           /**<  Identifier for C,availability_bitmask_* uses these identifiers  */
    D_exist         = 0x0020,           /**<  Identifier for D, availability_bitmask_* uses these identifiers  */
    E_exist         = 0x0040,           /**<  Identifier for E, availability_bitmask_* uses these identifiers  */
    F_exist         = 0x0080,           /**<  Identifier for F, availability_bitmask_* uses these identifiers  */
    EOC_exist       = 0x0100,           /**<  Identifier for end of controllability, availability_bitmask_* uses these identifiers  */
}AF_positions_availability_bitmask;

/** Structure for specifying resolution of the sensor */
typedef struct {
    uint16 width;                       /**< Width in pixels */
    uint16 height;                      /**< Height in pixels */
} CAM_DRV_NVM_SENS_RESOLUTION_T;

/** Structure for specifying the LSC data */
typedef struct
{
    uint16 r;                           /**< Red pixels */
    uint16 gr;                          /**< Green red pixels */
    uint16 b;                           /**< Blue pixels */
    uint16 gb;                          /**< Green blue pixels */
}CAM_DRV_NVM_LSC_DATA_T;

/** Structure for specifying the LS table */
typedef struct
{
    //CAM_DRV_NVM_LSC_DATA_T *p_lsc_data; /**< pointing to (x_division * y_division) number of entries of CAM_DRV_NVM_LSC_DATA_T */
    uint16 lsc_data_offset;             /**< Byte offset to lsc data from start of this object e.g.
                                             CAM_DRV_NVM_LS_TABLE_T* ls_table = (CAM_DRV_NVM_LS_TABLE_T*) some_address;
                                             CAM_DRV_NVM_LSC_DATA_T* p_lsc_data = (CAM_DRV_NVM_LSC_DATA_T*) (ls_table + ls_table.lsc_data_offset);
                                             (x_division * y_division) number of entries of CAM_DRV_NVM_LSC_DATA_T */

    uint16 x_value_x1000;               /**< Illumination data of measurement conditions, 1000 times X-coordinate in CIE 1931 */
    uint16 y_value_x1000;               /**< Illumination data of measurement conditions, 1000 times Y-coordinate in CIE 1931 */

    uint8 x_division;                   /**< number of areas in horizontal */
    uint8 y_division;                   /**< number of areas in vertical */
    uint8 ch;                           /**< number of color channels: 3 or 4 */
    uint8 center_bit_depth;             /**< Size (no. of bits) for center entry */
    uint8 other_bit_depth;              /**< Size (no. of bits) for other entries, if same as center_bit_depth then values are absolute, otherwise relative to center values */

    uint8 object_distance;              /**< lens position info of measurement conditions, Object distance */

    uint8 zoom_factor;                  /**< lens position info of measurement conditions, zoom factor */
    uint8 padding1;

}CAM_DRV_NVM_LS_TABLE_T;

/** Structure for specifying the Sensitivity data */
typedef struct
{
    CAM_DRV_NVM_LSC_DATA_T sens;        /**<  Sensitivity of the image center area */
    uint16 x_value_x1000;               /**<  Illumination data of measurement conditions: 1000 times X- coordinate in CIE 1931 (x,y) */
    uint16 y_value_x1000;               /**<  Illumination data of measurement conditions: 1000 times Y-coordinate in CIE 1931 (x,y) */
}CAM_DRV_NVM_SENSITIVITY_DATA_T;


/** Structure for specifying the Defect couplet type1 */
typedef struct
{
    uint16 x;                           /**< X co-ordinates of first pixel in a couplet */
    uint16 y;                           /**< Y co-ordinates of first pixel in a couplet  */

}CAM_DRV_NVM_DEFECT_COUPLET_TYPE1_T;

/** Structure for specifying the Defect couplet type2 */
typedef struct
{
    uint16 x;                           /**< X co-ordinates of first pixel in a couplet */
    uint16 y;                           /**< Y co-ordinates of first pixel in a couplet */
    uint8 dir;                          /**< Direction of 2nd pixel relative to first: 0= right, 1= bottom-right, 2= bottom, 3= bottom-left */
    uint8 padding1;
    uint8 padding2;
    uint8 padding3;

}CAM_DRV_NVM_DEFECT_COUPLET_TYPE2_T;

/** Structure for specifying the Defect line */
typedef struct
{
    uint16 XorY;                        /**<  Line/column number of defective line/column */
    uint8 type;                         /**< 0= line defect, 1= column defect */
    uint8 padding1;
    uint8 padding2;
    uint8 padding3;
}CAM_DRV_NVM_DEFECT_LINE_T;


typedef struct
{
    //uint8* p_ls_measured_lp_nums;           /**< pointer to 8bit entries each indicating in how many lens positions values are measured in each of temperatures */
    uint16 ls_measured_lp_nums_offset;      /**< Offset from beginning of this object to 8ls_measured_temp_num times bit entries each indicating in
                                                 how many lens positions values are measured in each of temperatures */
    //CAM_DRV_NVM_LS_TABLE_T *p_ls_comTable;
    uint16 ls_comTable_offset;              /**< Offset from beginning of this object to ls_table_num times CAM_DRV_NVM_LS_TABLE_T entries */
    uint8 ls_measured_temp_num;             /**< in how many colour temperatures LS values are measured */
    uint8 ls_table_num;                     /**< total number of CAM_DRV_NVM_LS_TABLE_T items present at p_ls_comTable, value = p_ls_measured_lp_nums[0] + ....
                                                 + p_ls_measured_lp_nums[ls_measured_temp_num-1] */

    uint8 padding1;
    uint8 padding2;
}CAM_DRV_NVM_LS_DATA_BLOCK_T;


typedef struct
{
    // AF data
    //uint16 *p_C_F_object_distances;         /**< Pointer to 16-bit values. A 16-bit number describing the object distance in mm for each measured position from infinity(excluding) to near end (excluding) */
    uint16 C_F_object_distances_offset;     /**< Offset from beginning of this object to 16-bit values. A 16-bit number describing the object distance in mm for each measured position from infinity(excluding) to near end (excluding) */
    //void *p_additional_af_data;             /**< Additional AF data, if any. NULL if no other data available. */
    uint16 additional_af_data_offset;       /**< Offset from beginning of this objec to additional AF data, if any. 0 if no other data available. */

                                            /**< In total there are 10 times 16 bit fields, out of which total_positions_h number of 16 bit values are valid, each corresponding to sensor reading or control
                                            word of different steps from far to near. Values are absolute. NULL if data is not available */
    uint16 positions_horizontal[CAM_DRV_NVM_MAX_AF_LENS_POS];

                                            /**< In total there are 10 times 16 bit fields, out of which total_positions_u number of 16 bit values are valid, each corresponding to sensor reading or control
                                            word of different steps from far to near. Values are absolute. NULL if data is not available */
    uint16 positions_upward[CAM_DRV_NVM_MAX_AF_LENS_POS];

                                            /**< tIn total there are 10 times 16 bit fields, out of which total_positions_d number of 16 bit values are valid, each corresponding to sensor reading or control
                                            word of different steps from far to near. Values are absolute. NULL if data is not available */
    uint16 positions_downward[CAM_DRV_NVM_MAX_AF_LENS_POS];

    uint16 obj_dis_availability_bitmask;    /**< Which data is available at p_C_F_object_distances. Each set bit of this bitmask indicates presence of corresponding field as specified by AF_positions_availability_bitmask
                                            Available positions are stored at p_C_F_object_distances in same order as appear in CAM_DRV_NVM_AF_POSITIONS_ORDER_E. */

    uint16 size_of_additional_af_data;      /**< Size of data (bytes) pointed by p_additional_af_data. If available and needed, this data needs to be parsed by client. */

    uint16 fine_step_im;                    /**< Number of units of Focus control word that corresponds to 5% MTF drop in focus change register or fine step movement in infinity to macro direction. */

    uint16 fine_step_mi;                    /**< Number of units of Focus control word that corresponds to 5% MTF drop in focus change register or fine step movement in macro to infinity direction. */


    uint16 availability_bitmask_h;          /**< Which data is available at p_positions_horizontal. Each set bit indicates presence of corresponding field as indicated by AF_positions_availability_bitmask
                                            Available positions are stored at p_positions_horizontal in same order as appear in CAM_DRV_NVM_AF_POSITIONS_ORDER_E. */
    uint16 availability_bitmask_u;          /**< Which data is available at p_positions_upward. Each set bit indicates presence of corresponding field as indicated by AF_positions_availability_bitmask
                                            Available positions are stored at p_positions_upward in same order as appear in CAM_DRV_NVM_AF_POSITIONS_ORDER_E. */
    uint16 availability_bitmask_d;          /**< Which data is available at p_positions_downward. Each set bit indicates presence of corresponding field as indicated by AF_positions_availability_bitmask
                                            Available positions are stored at p_positions_downward in same order as appear in CAM_DRV_NVM_AF_POSITIONS_ORDER_E. */

    uint8 pos_unit_relation_to_cw;          /**< How many focus change units one reading of position register corresponds to, 0 if not available */

    uint8 total_positions_h;                /**< How many positions stored for horizontal orientation */

    uint8 total_positions_u;                /**< How many positions stored for upward orientation */

    uint8 total_positions_d;                /**< How many positions stored for downward orientation */


} CAM_DRV_NVM_AF_DATA_BLOCK_T;

typedef struct
{
    /**<  Sensor sensitivity data */
    //CAM_DRV_NVM_SENSITIVITY_DATA_T *p_sens; /**< pointer to sensitivity data, sensitivity_colour_temps number of CAM_DRV_NVM_SENSITIVITY_DATA_T values */
    uint16 sens_offset;                     /**< Offset from beginning of this object to sensitivity data, sensitivity_colour_temps number of CAM_DRV_NVM_SENSITIVITY_DATA_T values */
    uint8 sensitivity_colour_temps;         /**< in how many colour temperatures values are measured */
    uint8 padding1;
    //uint8 pading2;
    //uint8 pading3;

} CAM_DRV_NVM_SENSITIVITY_DATA_BLOCK_T;


typedef struct
{
    /**<  Defect data
     Type1 only provides co-ordinates of first pixel in a couplet, no information on other pixel
     Type2 provides co-ordinates of first pixel as well as direction of other pixel relative to first */

    //CAM_DRV_NVM_DEFECT_COUPLET_TYPE1_T *p_couplet_type1_map;
    uint16 couplet_type1_map_offset;        /**< Offst from beginning of this object to num_of_couplet_type1_defects times
                                                 CAM_DRV_NVM_DEFECT_COUPLET_TYPE1_T type of data */
    //CAM_DRV_NVM_DEFECT_COUPLET_TYPE2_T *p_couplet_type2_map;
    uint16 couplet_type2_map_offset;        /**< Offst from beginning of this object to num_of_couplet_type2_defects times
                                                 CAM_DRV_NVM_DEFECT_COUPLET_TYPE2_T type of data */
    //CAM_DRV_NVM_DEFECT_LINE_T *p_line_map;
    uint16 line_map_offset;                 /**< Offst from beginning of this object to num_of_line_defects times
                                                 CAM_DRV_NVM_DEFECT_LINE_T type of data */

    uint16 num_of_couplet_type1_defects;    /**<  Number of defects of type 1 */

    uint16 num_of_couplet_type2_defects;    /**<  Number of defects of type 2 */

    uint16 num_of_line_defects;             /**<  Number of full line or column defects */

    //uint8 padding1;
    //uint8 padding2;

} CAM_DRV_NVM_DEFECT_DATA_BLOCK_T;

typedef struct
{
    /**< shutter delay in micro seconds, 0 if no shutter */
    uint16 shut_delay_us;

    /**< ND filter transparency, % value multiplied by 100, 0 if no ND filter */
    uint16 ND_trans_x100;

} CAM_DRV_NVM_ND_MS_DATA_BLOCK_T;

/** Structure for specifying the LSC data */
typedef struct
{
    uint8 r;                           /**< Red channel */
    uint8 gr;                          /**< Green red channel */
    uint8 b;                           /**< Blue channel */
    uint8 gb;                          /**< Green blue channel */
} CAM_DRV_NVM_SPATIAL_PEDESTAL_DATA_T;

typedef struct
{
    /**< 8bit absolute entries of spatial pedestal table, always width * height * sizeof(CAM_DRV_NVM_SPATIAL_PEDESTAL_DATA_T) */
    //CAM_DRV_NVM_SPATIAL_PEDESTAL_DATA_T* spatial_pedestal_table;
    uint16 spatial_pedestal_data_offset;   /**< Offset from beginning of this object to  width * height times CAM_DRV_NVM_SPATIAL_PEDESTAL_DATA_T data */

    /**< width of table (matrix) */
    uint8 width;
    /**< height of table (matrix) */
    uint8 height;
    /**< analog gain value for which this table's values are valid */
    uint8 analog_gain;
    uint8 padding1;
    uint8 padding2;
    uint8 padding3;

} CAM_DRV_NVM_SPATIAL_PEDESTAL_TABLE_T;

typedef struct
{
    /**< pointer to spatial pedestal tables */
    //CAM_DRV_NVM_SPATIAL_PEDESTAL_TABLE_T* spatial_pedestal_table;
    uint16 spatial_pedestal_table_offset;   /**< Offset from beginning of this object to  number_of_tables times CAM_DRV_NVM_SPATIAL_PEDESTAL_TABLE_T data */

    /**< number of spatial pedestal tables at spatial_pedestal_table, 0 if data not available */
    uint8 number_of_tables;
    uint8 padding1;
    //uint8 padding2;
    //uint8 padding3;

} CAM_DRV_NVM_SPATIAL_PEDESTAL_TABLES_BLOCK_T;

/** Structure for specifying NVM data in SMIA++ format, VERSION 2.0 */
typedef struct
{
    /**< Maximum sensor resolution */
    CAM_DRV_NVM_SENS_RESOLUTION_T sens_resolution;

    /** Focus lens related NVM data */
    CAM_DRV_NVM_AF_DATA_BLOCK_T af_data;

    /** Lens shading related NVM data */
    CAM_DRV_NVM_LS_DATA_BLOCK_T ls_data;

    /** Sensitivity related NVM data */
    CAM_DRV_NVM_SENSITIVITY_DATA_BLOCK_T sensitivity_data;

    /** Defect pixels related NVM data */
    CAM_DRV_NVM_DEFECT_DATA_BLOCK_T defect_data;

    /** ND filter and Shutter related NVM data */
    CAM_DRV_NVM_ND_MS_DATA_BLOCK_T nd_ms_data;

    /**< spatial pedestal data tables */
    CAM_DRV_NVM_SPATIAL_PEDESTAL_TABLES_BLOCK_T spatial_pedestal_data;

    /**<  Zoom data
     TBD - Not under consideration at the moment */

    /*  Checksum */
    uint32 checksum;                        /**< stored checksum */
    uint32 checksum_calculated;             /**< calculated checksum */

    /**<  Error correction TBD */
    uint32 number_of_errors;
    uint32 checksum2;
    uint32 checksum2_calculated;

    uint32 decoder_status;                  /**< decoding err codes if any */

    uint32 total_data_size;                 /**< Total size of data, sum of all fields in all structures */

    /**< if a certain subblock of data is not present then set corresponding offset to 0 */
    uint16 offset_af_data_object_distances; /**< Offset from beginning of CAM_DRV_NVM_T to uint16 type object distance data */
    uint16 offset_af_additional_af_data;    /**< Offset from beginning of CAM_DRV_NVM_T to additional af data, type not specified */
    uint16 offset_ls_no_of_lens_positions;  /**< Offset from beginning of CAM_DRV_NVM_T to uint8 type data measured_lp_nums */
    uint16 offset_com_tables;               /**< Offset from beginning of CAM_DRV_NVM_T to CAM_DRV_NVM_LS_TABLE_T type data */
    uint16 offset_ls_lsc_data;              /**< Offset from beginning of CAM_DRV_NVM_T to CAM_DRV_NVM_LSC_DATA_T type data */
    uint16 offset_sensitivity_data;         /**< Offset from beginning of CAM_DRV_NVM_T to CAM_DRV_NVM_SENSITIVITY_DATA_T type data */
    uint16 offset_defect_couplet_type1;     /**< Offset from beginning of CAM_DRV_NVM_T to CAM_DRV_NVM_DEFECT_COUPLET_TYPE1_T type data */
    uint16 offset_defect_couplet_type2;     /**< Offset from beginning of CAM_DRV_NVM_T to CAM_DRV_NVM_DEFECT_COUPLET_TYPE2_T type data */
    uint16 offset_defect_line;              /**< Offset from beginning of CAM_DRV_NVM_T to CAM_DRV_NVM_DEFECT_LINE_T type data */
    uint16 offset_pedestal_table;           /**< Offset from beginning of CAM_DRV_NVM_T to CAM_DRV_NVM_SPATIAL_PEDESTAL_TABLE_T type data */
    uint16 offset_pedestal_data;            /**< Offset from beginning of CAM_DRV_NVM_T to CAM_DRV_NVM_SPATIAL_PEDESTAL_DATA_T type data */

    uint8 padding1;
    uint8 padding2;

} CAM_DRV_NVM_T;

#endif /* _NVM_H_ */
