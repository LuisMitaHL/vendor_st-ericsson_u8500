
#ifndef armx_arm_h
#define armx_arm_h

#ifdef __cplusplus
extern "C"{
#endif

void    _box_mean_uint8_row_y_neon(unsigned char *p_src, unsigned short *p_dst, int param1, int* param2);

void    _box_mean_uint8_col_y_neon(unsigned short *p_src, unsigned char *p_dst, int param1, int* param2);

void    _box_mean_uint16_row_y_neon(unsigned short *p_src, unsigned short *p_dst, int param1, int* param2);

void    _box_mean_uint16_col_y_neon(unsigned short *p_src, unsigned short *p_dst,int param1, int* param2);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void    _box_mean_uint8_row_uv_neon(unsigned char *p_src, unsigned short *p_dst, int param1, int* param2);
    
void    _box_mean_uint8_col_uv_neon(unsigned short *p_src, unsigned char *p_dst, int param1, int* param2);
    
void    _box_mean_uint16_row_uv_neon(unsigned short *p_src, unsigned short *p_dst, int param1, int* param2);

void    _box_mean_uint16_col_uv_neon(unsigned short *p_src, unsigned short *p_dst,int param1, int* param2);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void    _calc_square_neon(unsigned char *p_src, unsigned short *p_dst, int param1, int* param2);

void    _calc_dst_neon(unsigned char *p_mean_a, unsigned short *p_mean_b, unsigned char *p_src, int *param);
    
void    _calc_ab_neon(unsigned short *p_mean_II, unsigned char *p_mean_I, unsigned char *p_lookup_a, int *param);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
int    _box_mean_uint8_y_neon(unsigned char *p_src, unsigned char *p_dst,int param1, int param2);
    
int    _box_mean_uint16_y_neon(unsigned short *p_src, unsigned short *p_dst,int param1, int param2);

int    _box_mean_uint8_uv_neon(unsigned char *p_src, unsigned char *p_dst,int param1, int param2);
    
int    _box_mean_uint16_uv_neon(unsigned short *p_src, unsigned short *p_dst,int param1, int param2);

void  _ZXCLAHEInterploateEntry_neon(int *pHistLU, int *pHistLB, int *pHistRU, int *param2);
#ifdef __cplusplus
}
#endif

#endif
