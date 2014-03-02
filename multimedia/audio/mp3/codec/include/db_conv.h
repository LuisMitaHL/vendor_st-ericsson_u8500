

#ifndef _db_conv_h_
#define _db_conv_h_

#ifndef _NMF_MPC_ 
/*
  dB to linear conversion
  
  input parameter : gaindB is in Q8 representation
  ouput_parameter : linear gain in [0.5,0.9999] with shift.
   
  
  principle : gain_linear(6,02dB) = 2
  => gain_linear(x) = gain_linear(x' + y*6.02) = gain_linear(x')*2^y with 0<x'<6.02dB
  process :
          gaindB is put in [0;6,02dB[ with sucessive add (or sub) of 6.02dB.
		  the number of add (or sub) is store in shift.
		  Once gaindB is in the range [0;6.02dB] its value is read in the convsersion table tab_gain
		  tab_gain is computed at compilation time according the NB_STEP value (defined in dBconv_include.h)
		  gain_linear(x) = tab_gain[x*(NB_STEP/6.02)].   STEP_FACT = NB_STEP / 6.02dB
		  
  if gaindB = -128dB (0xFF8000) then the function return 0;
  
*/
MMshort get_gain_from_dB(MMshort gaindB, MMshort *shift_res);


/*
  linear to dB conversion

  input parameter : linear gain with shift.
  ouput_parameter : gain in dB in Q8 representation
 */
MMshort get_dB_from_gain(MMshort gainl, MMshort shift);
#endif // _NMF_MPC_

#endif /* _db_conv_h_ */
