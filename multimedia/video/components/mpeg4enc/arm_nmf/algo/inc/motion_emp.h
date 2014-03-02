/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#ifndef MOTION_H
#define MOTION_H

#include "ast_defines.h"
#include "ast_types.h"
#include <stdio.h>
#include <stdlib.h>


//Extended area size
#define X_SIZE 17
#define Y_SIZE 17

#define X_SIZE_UV 9
#define Y_SIZE_UV 9
/* #include "defs_mot.h" */

#if ME_PROBES > 0
#define MAX_MV_NUMBER 32
typedef struct dump {
  ui16_t mb_x, mb_y;                                /* MB position x and y */
  ui16_t lambda;                                    /* lambda */  
  ui16_t MV_INDEX;                                  /* index for arrays in this struct */
  si16_t mv_x[MAX_MV_NUMBER], mv_y[MAX_MV_NUMBER];  /* MV components */
  ui16_t cost[MAX_MV_NUMBER];                       /* MV cost = SAD + SAD bias */
  ui16_t best_in_FIFO[MAX_MV_NUMBER];               /* flag set if best cost of MV in FIFO */
} ts_MEprobe, *tsp_MEprobe;
#endif

/* ******************************************************************* */
/* Varoius definitions */



/* Structure for Motion Vector */
typedef struct s_motion_vector
{
    si16_t    x,y;			/* Full per coordinates 	*/
    si8_t     hx,hy;     		/* Half pel coordinates 	*/
    ui8_t     valid;	       	/* Existence of vector          */
    ui32_t    sad;		/* SAD of the block associated to the vector */
    ui8_t     zerovector;		/* Set if zerovector is chosen  */
} motion_vector;

typedef struct motionvector {
  int x;         /* Horizontal comp. of mv            */
  int y;         /* Vertical comp. of mv              */
  int x_half;    /* Horizontal half-pel acc.          */
  int y_half;    /* Vertical half-pel acc.            */
  int min_error; /* Min error for this vector         */
  int Mode;      /* Necessary for adv. pred. mode     */
} MotionVector;

typedef motion_vector*	pmv_t;

/* 'Pixel" structure */
typedef struct s_pixel
{
    si16_t x,y;  /* Full pel coordinates */
    si16_t hx,hy; /* Half pel coordinates */
} pixel;


/* Macroblock types */
enum { mb_normal, mb_upper, mb_lower, mb_left, mb_right, mb_upper_left, mb_upper_right, mb_lower_left, mb_lower_right };

/* Block types */
enum { block_upper_left, block_upper_right, block_lower_left, block_lower_right };

/* Search scheme values */
enum { scheme_x, scheme_y, scheme_hx, scheme_hy };

/* Predictor types (ORIGINAL H.263+ version) */
/* enum { pred_space_left,pred_space_up,pred_temp_right,pred_temp_same }; */

/* Predictor types (MODIFIED for Mpeg-4) */
enum { pred_temp_same, pred_temp_right, pred_space_left, pred_space_up };
#define MAXRECURSIVEMVS 13

typedef struct
{
/* D.F. : Starting grid amplitude for refinement process*/        
  si16_t complete_refinement;

/* These are the predictor spaces:
the 'previous' space contains the temporal predictors
the 'actual' space contains the spatial predictors */
  pmv_t amv_prev_space;
  pmv_t amv_actl_space;
  motion_vector apv_preds[MAXRECURSIVEMVS];

  int RCONTROL; /* FP: implements the vop_rounding_type flag */

/* Dimensions of the frame and the search window */
  //pixel frame;
  //pixel searchwin_min;
  //pixel searchwin_max;
}Motion_Estimation_Data;


/* ******************************************************************* */
/* Function prototypes */

int mp4e_InitEstimate(ui32_t ui_dimx,ui32_t ui_dimy,ui32_t ui_dimb);
void mp4e_FreeEstimate (void);
void mp4e_NewSpace (ui32_t, ui32_t, ui32_t);

void mp4e_mv_SearchBest(ui8_t * pui8_source,ui8_t * pui8_area, motion_vector* amv_vect,pixel* coord, motion_vector* zero, ui32_t MaxValidNbr, pixel *frame, motion_vector* best_vector);
void mp4e_mv_SearchBest_umv(ui8_t * pui8_source,ui8_t * pui8_area, motion_vector* amv_vect,pixel* coord, motion_vector* zero, ui32_t MaxValidNbr, pixel *frame, motion_vector* best_vector);

motion_vector mp4e_mv_RefineDistance_umv (ui8_t * pui8_source, ui8_t * pui8_area, pixel* start, motion_vector* mv_displ, 
									  pixel* frame, ui32_t ui_limit_min, ui32_t ui_alfa, ui32_t delta_grid);

motion_vector mp4e_mv_RefineDistance (ui8_t * pui8_source, ui8_t * pui8_area, pixel* start, motion_vector* mv_displ, 
									  pixel* frame, ui32_t ui_limit_min, ui32_t ui_alfa, ui32_t delta_grid);



motion_vector mp4e_LimitRefinementVector (pixel* newPos, motion_vector* base_mv_v, pixel* coord, pixel* frame, unsigned int ui_limit_min, ui32_t umv);

/* Interface functions */
#ifdef MBTYPE_USE_MAD
int mp4e_ChooseMode (unsigned char *curr, int pels, int x_pos, int y_pos, int min_SAD);
#endif
#ifdef MBTYPE_USE_SAD
int mp4e_ChooseMode (int min_SAD);
#define SAD_THRESHOLD 4000
#endif
#ifdef MBTYPE_USE_MOTION_FIELD
int mp4e_ChooseMode ( int nMBwidth, int xMB, int yMB );
#define MV_NORM_ERROR_THRESHOLD 40
#endif
#ifdef MBTYPE_USE_MOTION_FIELD_AND_SAD
int mp4e_ChooseMode ( int nMBwidth, int xMB, int yMB, int min_SAD );
#define MV_NORM_ERROR_THRESHOLD 40
#define SAD_THRESHOLD 4000
#endif

int  mp4e_MacroBlockMotionEstimate(
			 int             MVx_tr[4],
			 int             MVy_tr[4],
			 unsigned char  *pui8_area1,
			 unsigned char  *pui8_area2,
			 unsigned char  *pui8_X,
			 unsigned int    ui_x,
			 unsigned int    ui_y,
			 unsigned int    ui_searchw,
			 unsigned int    ui_numb,
			 mp4_parameters *mp4_par
			);

/* Debugging functions */

void mp4e_PrintArea(ui8_t * area,ui32_t dim);
void mp4e_PrintVector(motion_vector v);


/* Other */
int mp4e_EvaluateMotionStrength(int,int *,int *,int);

void mp4e_StretchAllMV(ui32_t ui_dimx, ui32_t ui_dimy, ui32_t ui_dimb, si16_t stretch);
void mp4e_UnStretchAllMV(ui32_t ui_dimx, ui32_t ui_dimy, ui32_t ui_dimb, si16_t stretch);

//extern int RCONTROL;    /* FP: implements the vop_rounding_type flag */


#endif
