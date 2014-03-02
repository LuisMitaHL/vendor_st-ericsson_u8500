/************************* MPEG-2 NBC Audio Decoder **************************
 *                                                                           *
"This software module was originally developed by
AT&T, Dolby Laboratories, Fraunhofer Gesellschaft IIS and edited by
Yoshiaki Oikawa (Sony Corporaion),
Mitsuyuki Hatanaka (Sony Corporation),
in the course of development of the MPEG-2 NBC/MPEG-4 Audio standard ISO/IEC 13818-7,
14496-1,2 and 3. This software module is an implementation of a part of one or more
MPEG-2 NBC/MPEG-4 Audio tools as specified by the MPEG-2 NBC/MPEG-4
Audio standard. ISO/IEC  gives users of the MPEG-2 NBC/MPEG-4 Audio
standards free license to this software module or modifications thereof for use in
hardware or software products claiming conformance to the MPEG-2 NBC/MPEG-4
Audio  standards. Those intending to use this software module in hardware or
software products are advised that this use may infringe existing patents.
The original developer of this software module and his/her company, the subsequent
editors and their companies, and ISO/IEC have no liability for use of this software
module or modifications thereof in an implementation. Copyright is not released for
non MPEG-2 NBC/MPEG-4 Audio conforming products.The original developer
retains full right to use the code for his/her  own purpose, assign or donate the
code to a third party and to inhibit third party from using the code for non
MPEG-2 NBC/MPEG-4 Audio conforming products. This copyright notice must
be included in all copies or derivative works."
Copyright(c)1996.
 *                                                                           *
 ****************************************************************************/

#ifndef	_coupling_h_
#define	_coupling_h_

#if (CChans > 0)
int             cc_hufffac(Info * info, UCHAR * group, UCHAR * cc_cb_map, SHORT global_gain, SHORT *factors);

int 
getcc(MC_Info * mip, UCHAR * cc_wnd, Wnd_Shape * cc_wnd_shape,
      Float ** cc_coef, Float * cc_gain[CChans][Chans]);

    void            ind_coupling(MC_Info * mip, UCHAR * wnd, Wnd_Shape * wnd_shape, UCHAR * cc_wnd,
				                 Wnd_Shape * cc_wnd_shape, Float * cc_coef[CChans], Float * cc_state[CChans]);

    void            mix_cc(Info * info, Float * coef, Float * cc_coef, Float * cc_gain, int ind);
    void            init_cc(void);
    void            coupling(Info * info, MC_Info * mip, Float ** coef, Float ** cc_coef,
			                     Float * cc_gain[CChans][Chans], int ch, int cc_dom, int cc_ind);
#endif

#endif
