/******************************************************************************
 Copyright (c) 2009-2011, ST-Ericsson SA
 All rights reserved.
 
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:
 
   1. Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials
      provided with the distribution.
   3. Neither the name of the ST-Ericsson SA nor the names of its
      contributors may be used to endorse or promote products
      derived from this software without specific prior written
      permission.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 OF THE POSSIBILITY OF SUCH DAMAGE.
 ******************************************************************************/

/********************************************************************************
*Notes:		- 8 point FDCT based on modified BEYONG GI LEE					*
*			fast algorithm  (11 multiplies, 29 adds)							*
*			- 20 bit data, 16 bit coefficient for 8 point FDCT					*
*			- 16 bit data for transposition memory								*
*			- 9 bit input, 12 bit output										*
********************************************************************************/
#include "types.h"
#ifdef __NMF
#include <t1xhv_retarget.idt>
#endif


#include "jpegenc_fdct_20_16.h"

/*   Multiplication   */

#define mult(X,C) ((((X>>12)*C) + (((X&0x00000fff)*C)>>12) + 2)>>2)

/*   Addition and division by 4   */

#define adddiv4(X,Y) ((X+Y)>>2)

/*   Substraction and division by 4   */

#define subdiv4(X,Y) ((X-Y)>>2)

/*   Addition and division by 2   */

#define adddiv2(X,Y) ((X+Y)>>1)

/*   Substraction and division by 2   */

#define subdiv2(X,Y) ((X-Y)>>1)

/*   One Dimentional Forward DCT   */

/*   DCT8(X)

  t_sint32 X[8] ;
*/

void jpegenc_DCT8(t_sint32 * X) ;

void jpegenc_DCT8(t_sint32 * X)
{
	t_sint32   F[8] ;

	/*   Stage 1 : result in F   */

	F[7] = subdiv4 (X[2] , X[5]) ;
	F[6] = adddiv4 (X[2] , X[5]) ;
	F[5] = subdiv4 (X[3] , X[4]) ;
	F[4] = adddiv4 (X[3] , X[4]) ;
	F[3] = subdiv4 (X[1] , X[6]) ;
	F[2] = adddiv4 (X[1] , X[6]) ;
	F[1] = subdiv4 (X[0] , X[7]) ;
	F[0] = adddiv4 (X[0] , X[7]) ;

	/*   Stage 2 : result in F   */

	/*           F[7] = mult (F[7] ,  3686L ) ;
	F[5] = mult (F[5] , 10498L ) ;
	F[3] = mult (F[3] ,  2463L ) ;
	F[1] = mult (F[1] ,  2088L ) ; */
	F[7] = mult (F[7] , 14745L ) ;
	F[5] = mult (F[5] , 41991L ) ;
	F[3] = mult (F[3] ,  9852L ) ;
	F[1] = mult (F[1] ,  8352L ) ;

	/*   Stage 3 : result in X   */

	X[7] = F[3] - F[7] ;
	X[6] = F[3] + F[7] ;
	X[5] = F[1] - F[5] ;
	X[4] = F[1] + F[5] ;
	X[3] = F[2] - F[6] ;
	X[2] = F[2] + F[6] ;
	X[1] = F[0] - F[4] ;
	X[0] = F[0] + F[4] ;

	/*   Stage 4 : result in X   */

	/*           X[7] = mult (X[7] , 7568L ) ;
	X[5] = mult (X[5] , 3135L ) ;
	X[3] = mult (X[3] , 7568L ) ;
	X[1] = mult (X[1] , 3135L ) ; */
	X[7] = mult (X[7] , 30274L ) ;
	X[5] = mult (X[5] , 12540L ) ;
	X[3] = mult (X[3] , 30274L ) ;
	X[1] = mult (X[1] , 12540L ) ;

	/*   Stage 5 : result in F   */

	F[7] = subdiv2 (X[5] , X[7]) ;
	F[6] = adddiv2 (X[5] , X[7]) ;
	F[5] = subdiv2 (X[4] , X[6]) ;
	F[4] = adddiv2 (X[4] , X[6]) ;
	F[3] = subdiv2 (X[1] , X[3]) ;
	F[2] = adddiv2 (X[1] , X[3]) ;
	F[1] = subdiv2 (X[0] , X[2]) ;
	F[0] = adddiv2 (X[0] , X[2]) ;

	/*   Stage 6 : result in F   */

	/*           F[7] = mult (F[7] , 2896L ) ;
	F[4] = mult (F[4] , 5793L ) ;
	F[3] = mult (F[3] , 2896L ) ; */
	F[7] = mult (F[7] , 11585L ) ;
	F[4] = mult (F[4] , 23170L ) ;
	F[3] = mult (F[3] , 11585L ) ;

	/*   Stage 7 : result in F   */

	F[6] = F[7] + F[6] ;
	F[2] = F[3] + F[2] ;

	/*   Stage 8 : result in X   */

	X[5] = F[7] + F[5] ;
	X[3] = F[6] + F[5] ;
	X[1] = F[6] + F[4] ;
	X[7] = F[7] ;
	X[6] = F[3] ;
	X[4] = F[1] ;
	X[2] = F[2] ;
	X[0] = F[0] ;
}

/*   Two Dimensional DCT   */

/*   dct (X,F)

  t_sint16 X[64],F[64] ;
*/
void jpegenc_dct (t_sint16 * X, t_sint16 * F)
{
	t_sint32 A[8] ;
	t_sint32 interx[64];
	t_uint32 i,j ;

	/*   LINE DCT   */

	for (i=0;i<8;i++)
	{
		for (j=0;j<8;j++) A[j]=X[i*8+j]<<10 ;
		jpegenc_DCT8(A) ;
		for (j=0;j<8;j++) interx[j*8+i] = (A[j]+8)>>4 ;
	}

	/*   COLUMN DCT   */

	for (i=0;i<8;i++)
	{
		for (j=0;j<8;j++) A[j]=interx[i*8+j]<<4 ;
		jpegenc_DCT8(A) ;
		for (j=0;j<8;j++) F[j*8+i] = (A[j]+64)>>7 ;
	}

}
