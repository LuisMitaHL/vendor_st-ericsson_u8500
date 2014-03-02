/*****************************************************************************/
/**
*  ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file
* \brief   DTH server helper
* \author  ST-Ericsson
*/
/*****************************************************************************/

#ifndef DTH_DTHSRVHELPER_H_
#define DTH_DTHSRVHELPER_H_

#include  <dthfs/dth_filesystem.h>

struct dth_srvhelper {
	void *handle;
	void (*init_helper) (struct dth_directory *);
	void (*publish_verbose) (int);
	int (*publish_init) (const char *filename, char mode);
	void (*publish_set_module) (const char *);
	void (*publish_validate) (int);
	void (*publish_term) (void);

	int (*push_message) (int, const char *, ...);
	void (*reset_message_pump) (void);
	const char *(*get_message_pump) (void);

};

/**
 *
 * dth_type.
 *
 * Values that defines the type of a DTH element.
 *
 */
enum dth_type {
	/* values between 1 and 64 are reserved for bitfields parts size */
	DTH_BITFIELD_PART_MIN_SIZE = 1,
	DTH_BITFIELD_1_bits = 1,
	DTH_BITFIELD_2_bits = 2,
	DTH_BITFIELD_3_bits = 3,
	DTH_BITFIELD_4_bits = 4,
	DTH_BITFIELD_5_bits = 5,
	DTH_BITFIELD_6_bits = 6,
	DTH_BITFIELD_7_bits = 7,
	DTH_BITFIELD_8_bits = 8,
	DTH_BITFIELD_9_bits = 9,
	DTH_BITFIELD_10_bits = 10,
	DTH_BITFIELD_11_bits = 11,
	DTH_BITFIELD_12_bits = 12,
	DTH_BITFIELD_13_bits = 13,
	DTH_BITFIELD_14_bits = 14,
	DTH_BITFIELD_15_bits = 15,
	DTH_BITFIELD_16_bits = 16,
	DTH_BITFIELD_17_bits = 17,
	DTH_BITFIELD_18_bits = 18,
	DTH_BITFIELD_19_bits = 19,
	DTH_BITFIELD_20_bits = 20,
	DTH_BITFIELD_21_bits = 21,
	DTH_BITFIELD_22_bits = 22,
	DTH_BITFIELD_23_bits = 23,
	DTH_BITFIELD_24_bits = 24,
	DTH_BITFIELD_25_bits = 25,
	DTH_BITFIELD_26_bits = 26,
	DTH_BITFIELD_27_bits = 27,
	DTH_BITFIELD_28_bits = 28,
	DTH_BITFIELD_29_bits = 29,
	DTH_BITFIELD_30_bits = 30,
	DTH_BITFIELD_31_bits = 31,
	DTH_BITFIELD_32_bits = 32,
	DTH_BITFIELD_33_bits = 33,
	DTH_BITFIELD_34_bits = 34,
	DTH_BITFIELD_35_bits = 35,
	DTH_BITFIELD_36_bits = 36,
	DTH_BITFIELD_37_bits = 37,
	DTH_BITFIELD_38_bits = 38,
	DTH_BITFIELD_39_bits = 39,
	DTH_BITFIELD_40_bits = 40,
	DTH_BITFIELD_41_bits = 41,
	DTH_BITFIELD_42_bits = 42,
	DTH_BITFIELD_43_bits = 43,
	DTH_BITFIELD_44_bits = 44,
	DTH_BITFIELD_45_bits = 45,
	DTH_BITFIELD_46_bits = 46,
	DTH_BITFIELD_47_bits = 47,
	DTH_BITFIELD_48_bits = 48,
	DTH_BITFIELD_49_bits = 49,
	DTH_BITFIELD_50_bits = 50,
	DTH_BITFIELD_51_bits = 51,
	DTH_BITFIELD_52_bits = 52,
	DTH_BITFIELD_53_bits = 53,
	DTH_BITFIELD_54_bits = 54,
	DTH_BITFIELD_55_bits = 55,
	DTH_BITFIELD_56_bits = 56,
	DTH_BITFIELD_57_bits = 57,
	DTH_BITFIELD_58_bits = 58,
	DTH_BITFIELD_59_bits = 59,
	DTH_BITFIELD_60_bits = 60,
	DTH_BITFIELD_61_bits = 61,
	DTH_BITFIELD_62_bits = 62,
	DTH_BITFIELD_63_bits = 63,
	DTH_BITFIELD_64_bits = 64,
	DTH_BITFIELD_PART_MAX_SIZE = 64,

	DTH_TYPE_U8 = 65,		/**< u8[(n)[(m)]] .*/
	DTH_TYPE_S8,		/**<  s8[(n)[(m)]] .*/
	DTH_TYPE_U16,		/**<  u16[(n)[(m)]] .*/
	DTH_TYPE_S16,		/**<  s16[(n)[(m)]] .*/
	DTH_TYPE_U32,		/**<  u32[(n)[(m)]] .*/
	DTH_TYPE_S32,		/**<  s32[(n)[(m)]] .*/
	DTH_TYPE_U64,		/**<  u64[(n)[(m)]] .*/
	DTH_TYPE_S64,		/**<  s64[(n)[(m)]] .*/
	DTH_TYPE_FLOAT,		/**<  float .*/
	DTH_TYPE_STRING,	/**<  string .*/
	DTH_TYPE_FILE,		/**<  file .*/
	DTH_TYPE_DATA,		/**<  data .*/
	DTH_TYPE_U8B,		/**<  u8[(n)[(m)]] .*/
	DTH_TYPE_U16B,		/**<  u16[(n)[(m)]] .*/
	DTH_TYPE_U32B,		/**<  u32[(n)[(m)]] .*/
	DTH_TYPE_U64B,		/**<  u64[(n)[(m)]] .*/
	DTH_TYPE_ENUM_MAX,	/**<  maximum in enum definition. Must allways be the last one!*/
};

/**
 *
 * dth_element.
 *
 * Structure that containing all information to describe a DTH Element such
 * as items, bit fields, arrays and actions.
 *
 *  A DTH element might be composed of several such structures, if it has some

 *  sub-elements (e.g. for DTH Actions or Bitfields); in this case there is
 *  one structure for the main element, and one structure for each sub-element.
 *
 */
struct dth_element {
	char *path;  /**< the absolute path of this element.*/
	int type;  /**< any type defined.*/
	int cols; /**< the absolute path of this element.*/
	int rows; /**< the absolute path of this element.*/
	char *enumString;	/**< pointer to enum string or NULL if no enum.*/
	char *info; /**< pointer to info string, or NULL if no info.*/
	long long min;	/**< minimal allowed value; if both min and max are 0,
					  this means that none is signifiant.*/
	long long max; /**< maximal allowed value; if both min and max are 0,
					 this means that none is significant.*/
	int (*get) (struct dth_element *elem, void *value);
							   /**< pointer to callback used to
														 get element value.*/
	int (*set) (struct dth_element *elem, void *value);
							   /**< pointer to callback used to set element
														 value.*/
	int (*exec) (struct dth_element *elem);
					       /**< pointer to callback used to execute action.*/
	int user_data; /**< value defined by DTH Service.*/
	const void *default_value;  /**< default value of an element.*/
};

/**
 *
 * dth_array.
 *
 * Structure used to represent and acess the value of a DTH array.
 *
 *
 */
struct dth_array {
	int col; /**< number of colums to access in the array.*/
	int row; /**< number of lines to accessin the array.*/
	dth_s64 *array;	/**< area that contains a part of or whole DTH array.*/
};

/**
 *
 * dth_file_data.
 *
 * Structure used to represent and access the value of a DTH file.
 *
 *
 */
struct dth_file_data {
	char *pathname;
	unsigned int offset; /**< offset to access the file.*/
	unsigned int count;  /**< number of bytes to accessin the array.*/
	char *data; /**< area that contains the data bytes to be written or read.*/
};

/**
 * Load DTH services.
 * @param[in]	none
 * @retval 0 success.
 * @retval errorCode if an error occured while processing.
 */
int loaddthsrvhelper(void);

/**
 * Register a new DTH element into the DTH server file system.
 * @param[in] elem specifies the DTH element to register.
 * @retval 0 success.
 * @retval errorCode if an error occured while processing.
 */
int dth_register_element(struct dth_element *elem);

/**
 * Register a new DTH element into the DTH server file system.
 * @param[in] output document name.
 * @param[in] debug level.
 * @retval 0 success.
 * @retval errorCode if an error occured while processing.
 */
int genAPIdoc(const char *api_doc_filename, const int debuglevel);

/**
 * Callback to get a element value.
 * @param[in] elem specifies the DTH element.
 * @param[in] value specifies the pointer to the place
 * where to store the value.
 * @retval 0 success.
 * @retval errorCode if an error occured while processing.
 */
int (*get) (struct dth_element *elem, void *value);

/**
 * Callback to set a element value.
 * @param[in] elem specifies the DTH element.
 * @param[in] value specifies the pointer to the place where
 * to get the new value.
 * @retval 0 success.
 * @retval errorCode if an error occured while processing.
 */
int (*set) (struct dth_element *elem, void *value);

/**
 * Callback to execute a element action.
 * @param[in] elem specifies the DTH element.
 * @retval 0 success.
 * @retval errorCode if an error occured while processing.
 */
int (*exec) (struct dth_element *elem);


/**
 * Unregister a DTH element from the DTH virtual file system.
 * The function may be called in publishing mode but it will have no effect.
 * @param[in] elem specifies the DTH element to unregister.
 * @retval 0 success.
 * @retval 1 if the element was not found on server.
 * @retval -1 if an error occurred while processing.
 */
int dth_unregister_element(struct dth_element *elem);

/**
 * Unregister Unregister a file or directory as well as all its children from
 * the DTH virtual file system. The function may be called in publishing mode but
 * it will have no effect.
 * @param[in] path specifies the path of the branch to be removed.
 * @retval 0 success.
 * @retval 1 if the element was not found on server.
 * @retval -1 if an error occurred while processing.
 */
int dth_unregister_branch(const char *path);

/**
 * Tail message on stack and returns the number of chars really pushed in.
 * The message stack is limited so the returned number of
 * chars may be less than
 * \app_msg length.
 */
int dth_push_message(const int level, const char *pp_format, ...);

/**
 * Reset message pump
 */
void dth_reset_message_pump(void);

/* @caller 9pserver */
const char *dth_get_message_pump(void);

#endif /* DTH_SRVHELPER_H_ */
