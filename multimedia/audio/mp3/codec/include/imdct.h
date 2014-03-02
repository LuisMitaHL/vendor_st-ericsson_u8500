
#ifndef _imdct_h_
#define _imdct_h_

#include "audiolibs_common.h"

#include "fft.h"
#include "mdct_tables.h"
#include "vector.h"


#define imdct_init_tables(base) audiotables_mdct_init(base)

extern void
imdct(Float *Spectra,	/* input array : Length_d2 elements */
      Float *Samples,	/* output array: Length elements    */
      int ln);          /* (2^(ln-1)) spectra (2**ln) points */

extern void
imdct_compress(Float *Spectra,	/* input array : Length_d2 elements */
	       Float *Samples,	/* output array: Length elements    */
	       int ln);          /* (2^(ln-1)) spectra (2**(ln-1)) points */

extern void
imdct_ramxy(Float *Spectra,	/* input array : Length_d2 elements */
	    Float *Samples,	/* output array: Length elements    */
	    Float YMEM *scratch_y, /* scratch_buffer in YMEM       */
	    int ln);            /* (2^(ln-1)) spectra (2**ln) points */

extern void
imdct_ramxy_scaled(Float *Spectra,	/* input array : Length_d2 elements */
	    Float *Samples,	/* output array: Length elements    */
	    Float YMEM *scratch_y, /* scratch_buffer in YMEM       */
	    int ln);            /* (2^(ln-1)) spectra (2**ln) points */
extern void
imdct_compress_ramxy(Float *Spectra,	/* input array : Length_d2 elements */
		     Float *Samples,	/* output array: Length elements    */
		     Float YMEM *scratch_y, /* scratch_buffer in YMEM       */
		     int ln);        /* (2^(ln-1)) spectra (2**ln-1) points */

#endif /* Do not edit below this line */

