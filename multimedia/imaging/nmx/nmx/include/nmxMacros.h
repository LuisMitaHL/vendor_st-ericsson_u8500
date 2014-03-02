/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*!
	CVS Id    $Id$

	\file	  nmxMacros.h
 
	\author   Luigi Arnone luigi.arnone@stericsson.com

*/

#ifndef H_nmxMacros
#define H_nmxMacros 1

#define NMX_GET_BIT_U8(u8,idx) ((u8>>idx)&1)
#define NMX_SET_BIT_U8(u8,idx,b) ((u8 & (255^(1<<idx))) | ((b&1)<<idx))

/* Copy a nmxMatrix structure */
#define NMX_COPY_MATRIX_DS(mati_,mato_) { \
		nmxDimension_t ndims; \
		nmxSize_t *dimsi, *dimso; \
		nmxStride_t *ski, *sko; \
		ndims = (mati_)->ndims; \
		dimsi = (mati_)->dims; \
		ski = (mati_)->strides; \
		(mato_)->ndims = ndims; \
		dimso = (mato_)->dims; \
		sko = (mato_)->strides; \
		for(;ndims;ndims--) { \
			*dimso++ = *dimsi++; \
			*sko++ = *ski++; \
		} \
	}

/* From a nmxMatrix returns rows and cols */
#define NMX_GET_MATRIX_2DIMS(mat_,rows_,cols_) { \
		nmxDimension_t ndims; \
		nmxSize_t *dims; \
		ndims = (mat_)->ndims; \
		dims = (mat_)->dims; \
		rows_ = dims[0]; \
		cols_ = 1; \
		if(ndims>1) cols_ = dims[1]; \
	}

/* Set rows and cols for nmxMatrix */
#define NMX_SET_MATRIX_2DIMS(mat_,rows_,cols_) { \
		nmxSize_t *dims; \
		(mat_)->ndims = 2; \
		dims = (mat_)->dims; \
		dims[0] = (nmxSize_t)rows_; \
		dims[1] = (nmxSize_t)cols_; \
	}

/* From a nmxMatrix returns rows, cols and p */
#define NMX_GET_MATRIX_3DIMS(mat_,rows_,cols_,p_) { \
		nmxDimension_t ndims; \
		nmxSize_t *dims; \
		ndims = (mat_)->ndims; \
		dims = (mat_)->dims; \
		rows_ = dims[0]; \
		cols_ = 1; \
		if(ndims>1) cols_ = dims[1]; \
		p_ = 1; \
		if(ndims>2) p_ = dims[2]; \
	}

/* Set rows, cols and p for nmxMatrix */
#define NMX_SET_MATRIX_3DIMS(mat_,rows_,cols_,p_) { \
		nmxSize_t *dims; \
		(mat_)->ndims = 3; \
		dims = (mat_)->dims; \
		dims[0] = (nmxSize_t)rows_; \
		dims[1] = (nmxSize_t)cols_; \
		dims[2] = (nmxSize_t)p_; \
	}

/* From a nmxMatrix returns strides 0 1 */
#define NMX_GET_MATRIX_2STRIDES(mat_,sk0_,sk1_) { \
		nmxDimension_t ndims; \
		nmxStride_t *strides; \
		ndims = (mat_)->ndims; \
		strides = (mat_)->strides; \
		sk0_ = strides[0]; \
		sk1_ = 1; \
		if(ndims>1) sk1_ = strides[1]; \
	}

/* Set strides 0 1 for nmxMatrix */
#define NMX_SET_MATRIX_2STRIDES(mat_,sk0_,sk1_) { \
		nmxStride_t *strides; \
		strides = (mat_)->strides; \
		strides[0] = (nmxStride_t)sk0_; \
		strides[1] = (nmxStride_t)sk1_; \
	}

/* From a nmxMatrix returns strides 0 1 2 */
#define NMX_GET_MATRIX_3STRIDES(mat_,sk0_,sk1_,sk2_) { \
		nmxDimension_t ndims; \
		nmxStride_t *strides; \
		ndims = (mat_)->ndims; \
		strides = (mat_)->strides; \
		sk0_ = strides[0]; \
		sk1_ = 1; \
		if(ndims>1) sk1_ = strides[1]; \
		sk2_ = 1; \
		if(ndims>2) sk2_ = strides[2]; \
	}

/* Set strides 0 1 2 for nmxMatrix */
#define NMX_SET_MATRIX_3STRIDES(mat_,sk0_,sk1_,sk2_) { \
		nmxStride_t *strides; \
		strides = (mat_)->strides; \
		strides[0] = (nmxStride_t)sk0_; \
		strides[1] = (nmxStride_t)sk1_; \
		strides[2] = (nmxStride_t)sk2_; \
	}

/* Get rows cols and strides for nmxMatrix */
#define NMX_GET_MATRIX_2DS(mat_,d0_,d1_,sk0_,sk1_) {\
		NMX_GET_MATRIX_2DIMS(mat_,d0_,d1_); \
		NMX_GET_MATRIX_2STRIDES(mat_,sk0_,sk1_); \
	}

/* Get data pointer, rows, cols and strides for nmxMatrix */
#define NMX_GET_MATRIX_P2DS(mat_,ty_,pd_,d0_,d1_,sk0_,sk1_) {\
		pd_ = (ty_*)((mat_)->pdata); \
		NMX_GET_MATRIX_2DS(mat_,d0_,d1_,sk0_,sk1_); \
	}

/* Set rows cols and default strides for nmxMatrix */
#define NMX_SET_MATRIX_2D(mat_,d0_,d1_) {\
		NMX_SET_MATRIX_2DIMS(mat_,d0_,d1_); \
		NMX_SET_MATRIX_2STRIDES(mat_,1,d0_); \
	}

/* Set rows cols and strides for nmxMatrix */
#define NMX_SET_MATRIX_2DS(mat_,d0_,d1_,sk0_,sk1_) {\
		NMX_SET_MATRIX_2DIMS(mat_,d0_,d1_); \
		NMX_SET_MATRIX_2STRIDES(mat_,sk0_,sk1_); \
	}

/* Set data pointer, rows, cols and default strides for nmxMatrix */
#define NMX_SET_MATRIX_P2D(mat_,pd_,d0_,d1_) {\
		(mat_)->pdata = (void*)(pd_); \
		(mat_)->etype = NMX_MAKE_TYPE(0,sizeof(*pd_)); \
		NMX_SET_MATRIX_2D(mat_,d0_,d1_); \
	}

/* Set data pointer, rows, cols and strides for nmxMatrix */
#define NMX_SET_MATRIX_P2DS(mat_,pd_,d0_,d1_,sk0_,sk1_) {\
		(mat_)->pdata = (void*)(pd_); \
		(mat_)->etype = NMX_MAKE_TYPE(0,sizeof(*pd_)); \
		NMX_SET_MATRIX_2DS(mat_,d0_,d1_,sk0_,sk1_); \
	}

/* Get rows cols and strides for nmxMatrix */
#define NMX_GET_MATRIX_3DS(mat_,d0_,d1_,d2_,sk0_,sk1_,sk2_) {\
		NMX_GET_MATRIX_3DIMS(mat_,d0_,d1_,d2_); \
		NMX_GET_MATRIX_3STRIDES(mat_,sk0_,sk1_,sk2_); \
	}

/* Get data pointer, rows, cols and strides for nmxMatrix */
#define NMX_GET_MATRIX_P3DS(mat_,ty_,pd_,d0_,d1_,d2_,sk0_,sk1_,sk2_) {\
		pd_ = (ty_*)((mat_)->pdata); \
		NMX_GET_MATRIX_3DS(mat_,d0_,d1_,d2_,sk0_,sk1_,sk2_); \
	}

/* Set rows cols and default strides for nmxMatrix */
#define NMX_SET_MATRIX_3D(mat_,d0_,d1_,d2_) {\
		NMX_SET_MATRIX_3DIMS(mat_,d0_,d1_,d2_); \
		NMX_SET_MATRIX_3STRIDES(mat_,1,d0_,d0_*d1_); \
	}

/* Set rows cols and strides for nmxMatrix */
#define NMX_SET_MATRIX_3DS(mat_,d0_,d1_,d2_,sk0_,sk1_,sk2_) {\
		NMX_SET_MATRIX_3DIMS(mat_,d0_,d1_,d2_); \
		NMX_SET_MATRIX_3STRIDES(mat_,sk0_,sk1_,sk2_); \
	}

/* Set data pointer, rows, cols and defaut strides for nmxMatrix */
#define NMX_SET_MATRIX_P3D(mat_,pd_,d0_,d1_,d2) {\
		(mat_)->pdata = (void*)(pd_); \
		(mat_)->etype = NMX_MAKE_TYPE(0,sizeof(*pd_)); \
		NMX_SET_MATRIX_3D(mat_,d0_,d1_,d2); \
	}

/* Set data pointer, rows, cols and strides for nmxMatrix */
#define NMX_SET_MATRIX_P3DS(mat_,pd_,d0_,d1_,d2,sk0_,sk1_,sk2_) {\
		(mat_)->pdata = (void*)(pd_); \
		(mat_)->etype = NMX_MAKE_TYPE(0,sizeof(*pd_)); \
		NMX_SET_MATRIX_3DS(mat_,d0_,d1_,d2,sk0_,sk1_,sk2_); \
	}

/*  */
#define NMX_GET_VECTOR_ITEM(pdata_,stride_,idx_) ((pdata_) + ((idx_) * (stride_)))
#define NMX_GET_VECTOR_VALUE(pdata_,stride_,idx_) *NMX_GET_VECTOR_ITEM(pdata_,stride_,idx_)
#define NMX_SET_VECTOR_VALUE(pdata_,stride_,idx_,value_) *NMX_GET_VECTOR_ITEM(pdata_,stride_,idx_) = (value_)

/*  */
#define NMX_GET_ELEMENT_2D(mat_,type_,i0_,i1_,e_) { \
		type_ *pdata; \
		pdata = (type_ *)mat_->pdata; \
		e_ = pdata[(i0_)*mat_->strides[0] + (i1_)*mat_->strides[1]]; \
	}

/* preset the default values */
#define NMX_GET_2ELEMENS(vector_,type_,e0_,e1_) { \
		uint32_t u32tmp_; \
		type_ *ptmp_; \
		if(NMX_IS_EMPTY(vector_)) { \
			e0_ = (type_)0; \
			e1_ = (type_)0; \
		} else { \
			u32tmp_ = (vector_)->ndims; \
			if(u32tmp_>1) u32tmp_ = (vector_)->dims[0] * (vector_)->dims[1]; \
			else u32tmp_ = (vector_)->dims[0]; \
			ptmp_ = (type_*)(vector_)->pdata; \
			if(u32tmp_ > 0) e0_ = ptmp_[0]; \
			if(u32tmp_ > 1) e1_ = ptmp_[1]; \
		} \
	}

#define NMX_GET_3ELEMENS(vector_,type_,e0_,e1_,e2_) { \
		uint32_t u32tmp_; \
		type_ *ptmp_; \
		if(NMX_IS_EMPTY(vector_)) { \
			e0_ = (type_)0; \
			e1_ = (type_)0; \
			e2_ = (type_)0; \
		} else { \
			u32tmp_ = (vector_)->ndims; \
			if(u32tmp_>1) u32tmp_ = (vector_)->dims[0] * (vector_)->dims[1]; \
			else u32tmp_ = (vector_)->dims[0]; \
			ptmp_ = (type_*)(vector_)->pdata; \
			if(u32tmp_ > 0) e0_ = ptmp_[0]; \
			if(u32tmp_ > 1) e1_ = ptmp_[1]; \
			if(u32tmp_ > 2) e2_ = ptmp_[2]; \
		} \
	}

#define NMX_GET_4ELEMENS(vector_,type_,e0_,e1_,e2_,e3_) { \
		uint32_t u32tmp_; \
		type_ *ptmp_; \
		if(NMX_IS_EMPTY(vector_)) { \
			e0_ = (type_)0; \
			e1_ = (type_)0; \
			e2_ = (type_)0; \
			e3_ = (type_)0; \
		} else { \
			u32tmp_ = (vector_)->ndims; \
			if(u32tmp_>1) u32tmp_ = (vector_)->dims[0] * (vector_)->dims[1]; \
			else u32tmp_ = (vector_)->dims[0]; \
			ptmp_ = (type_*)(vector_)->pdata; \
			if(u32tmp_ > 0) e0_ = ptmp_[0]; \
			if(u32tmp_ > 1) e1_ = ptmp_[1]; \
			if(u32tmp_ > 2) e2_ = ptmp_[2]; \
			if(u32tmp_ > 3) e3_ = ptmp_[3]; \
		} \
	}

#define NMX_INIT_EMPTY(empty_) { \
		NMX_SET_MATRIX_P2D(empty_,0,0,0); \
		(empty_)->etype = NMX_MAKE_TYPE(0,0); \
	}

#define NMX_IS_EMPTY(mat_) (((mat_)!=NMX_NULL(nmxMatrix*)) ? (((mat_)->pdata!=NMX_NULL(void*)) ? ((mat_)->dims[0]*(mat_)->dims[1]) : 0) : 0)

#define NMX_INIT_SCALAR(scalar_,var_) { \
		NMX_SET_MATRIX_P2D(scalar_,&var_,1,1); \
		(scalar_)->etype = NMX_MAKE_TYPE(0,sizeof(var_)); \
	}

#define NMX_GET_SCALAR(scalar_,type_) (*((type_*)(scalar_)->pdata))

#define NMX_SET_SCALAR(scalar_,type_,val_) { \
		*((type_*)(scalar_)->pdata) = (type_)(val_); \
	}

#define NMX_TRANSPOSE(mat_) { \
		int32_t tmp; \
		NMX_SWAP((mat_)->dims[0],(mat_)->dims[1],tmp); \
		NMX_SWAP((mat_)->strides[0],(mat_)->strides[1],tmp); \
	}

#endif /* H_nmxMacros */


