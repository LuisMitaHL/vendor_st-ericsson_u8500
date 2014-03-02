
#ifdef SSR

extern int     const FT_MEM     aac_a_lngain[GAIN_TABLE_SIZE];
extern Float   const FT_MEM     aac_gc_gain_table[89+32];
extern Float   const FT_MEM     aac_ssr_tab_cos[NBANDS*NBANDS];
extern Float   const FT_MEM     aac_a_pqfproto[NPQFTAPS];

extern const   Float *aac_gc_gain_table_pointer;
#endif /* SSR */














