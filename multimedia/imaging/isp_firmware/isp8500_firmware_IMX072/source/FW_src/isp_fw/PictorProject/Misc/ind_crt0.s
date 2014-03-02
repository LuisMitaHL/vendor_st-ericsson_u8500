//******************************************************************************
//*                                                                            *
//*        Copyright 2004, STMicroelectronics, Incorporated.                   *
//*        All rights reserved                                                 *
//*                                                                            *
//*          STMICROELECTRONICS, INCORPORATED PROPRIETARY INFORMATION          *
//* This software is supplied under the terms of a license agreement or        *
//* nondisclosure agreement with STMicroelectronics and may not be copied or   *
//* disclosed except in accordance with the terms of that agreement.           *
//*                                                                            *
//******************************************************************************
//* Comments:                                                                  *
//*                                                                            *
//*    This file is a manual transcription of crt0.c, which is supposed to be  *
//* the real source for libc.a.                                                *
//*                                                                            *
//******************************************************************************

// SET 	__EMBEDDED_END__ to 1
// when you want an endless loop after main with no exit function
// for example in a final embedded application
.define __EMBEDDED_END__ 1
    
    .weak __stm_begin_ctors
    .weak __stm_end_ctors
    .weak __stm_begin_dtors
    .weak __stm_end_dtors

    .text
    .entry
    .globl __stm_ind_crt0
    
// void __stm_ind_crt0 ( int argc, char *argv[], char *envp[] ) {
__stm_ind_crt0:

// Homing parameters argc=r0, argv=r1 and envp=r2
    pushrl     r0-r2,r6-r7,lk
    
// Cl = &__stm_binfo_clear; // r3 used as Cl
    make       r3, %abs16to31(__stm_binfo_clear)
    more       r3, %abs0to15(__stm_binfo_clear)

// r4 used to store (__stm_einfo_clear)
    make       r4, %abs16to31(__stm_einfo_clear)
    more       r4, %abs0to15(__stm_einfo_clear)

// while ( Cl < &__stm_einfo_clear ) {
.while1_test:
    cmpge      g0, r3, r4
g0? jr         .while1_end

// memset(Cl->To,Cl->Val,Cl->Len);
    lw         r0, @(r3!+4)
    lw         r1, @(r3!+4)
    lw         r2, @(r3!+4)
    cmplt      g0, r2, 1
g0? jrgtudec   g0, r2, .memset_end
.memset_start:
    sb         @(r0!+1), r1
g0? jrgtudec   g0, r2, .memset_start
.memset_end:

// Cl++;
// already done in lw above //    add        r3, r3, 12

// }
    jr         .while1_test
.while1_end:

// if (__syscall(SYS_embedded,0,0,0)) {
    make       r0, 13   // SYS_embedded immediate value
    make       r1, 0
    make       r2, 0
    make       r3, 0
    callr      __syscall
    cmpeq      g0, r0, 0
g0? jr         .if1_end

// Cp = &__stm_binfo_copy; // r3 used as Cp
    make       r3, %abs16to31(__stm_binfo_copy)
    more       r3, %abs0to15(__stm_binfo_copy)

// r4 used to store (__stm_einfo_copy)
    make       r4, %abs16to31(__stm_einfo_copy)
    more       r4, %abs0to15(__stm_einfo_copy)

// while ( Cp < &__stm_einfo_copy ) {
.while2_test:
    cmpge      g0, r3, r4
g0? jr         .while2_end

// bcopy(Cm->To,Cm->From,Cm->Len);
    lw         r0, @(r3!+4)
    lw         r1, @(r3!+4)
    lw         r2, @(r3!+4)
    cmplt      g0, r2, 1
g0? jrgtudec   g0, r2, .bcopy_end
.bcopy_start:
    lb         r5, @(r1!+1)
    sb         @(r0!+1), r5
g0? jrgtudec   g0, r2, .bcopy_start
.bcopy_end:

// Cm++;
// already done in lw above //    addu       r3, r3, 12

// }
    jr         .while2_test
.while2_end:

// } /* if */
.if1_end:


// environ = envp;
    make       r3, %abs16to31(environ)
    more       r3, %abs0to15(environ)
    addu       r4, sp, 12               // env is located at sp+12
    sw         @(r3+0), r4

// call _finit
//NOT WITH OPEN64    callr      _finit

/*---------------------------------------------------*/
/* Call C static constructors  if any (used by gcov)*/

// r6 (callee saved) used to store &__stm_begin_ctors; 
    make       r6, %abs16to31(__stm_begin_ctors)
    more       r6, %abs0to15(__stm_begin_ctors)

// r7 (callee saved) used to store (__stm_end_ctors)
    make       r7, %abs16to31(__stm_end_ctors)
    more       r7, %abs0to15(__stm_end_ctors)

// while ( &__stm_begin_ctors < &__stm_end_ctors ) {
.while_ctor_test:
    cmpge      g0, r6, r7
g0? jr         .while_ctor_end
	
	lw 			r1, @( r6 !+ 4 )          
	cmpequ 		g3, r1, 0             
	// if not null Call C constructors
	g3? jr 		.while_ctor_end                
	calla 		r1
    jr         	.while_ctor_test
// end_while }
.while_ctor_end:
/*------------------------------------------------*/


// exit( main(argc,argv,envp) );
    lw         r0, @(sp+20)
    lw         r1, @(sp+16)
    lw         r2, @(sp+12)
    callr      main

// ******************* test __EMBEDDED_END__  *******************************
.if __EMBEDDED_END__ == 1
 // no exit : endless loop
.endlessloop:
	jr         .endlessloop

.else

/*---------------------------------------------------*/
/* Call C static destructors  if any */

// r6 (callee saved) used to store &__stm_begin_dtors; 
    make       r6, %abs16to31(__stm_begin_dtors)
    more       r6, %abs0to15(__stm_begin_dtors)

// r7 (callee saved) used to store (__stm_end_dtors)
    make       r7, %abs16to31(__stm_end_dtors)
    more       r7, %abs0to15(__stm_end_dtors)

// while ( &__stm_begin_dtors < &__stm_end_dtors ) {
.while_dtor_test:
    cmpge      	g0, r6, r7
g0? jr         	.while_dtor_end
	
	lw 			r1, @( r6 !+ 4 )          
	cmpequ 		g3, r1, 0             
	// if not null Call C destructors
	g3? jr 		.while_dtor_end                
	calla 		r1
    jr         	.while_dtor_test
// end_while }
.while_dtor_end:

/*------------------------------------------------*/


//  exit  -------------------------------------------------
    callr      exit
// }
    poprl      r0-r2,r6-r7,lk
    rts

.endif
// ******************* end  test __EMBEDDED_END__  *******************************
                                                                                
	.type	__stm_ind_crt0,@function
	.size	__stm_ind_crt0,.-__stm_ind_crt0


	.data

// int * reent_errno_pt;
	.comm	reent_errno_pt,4,4
	.type	reent_errno_pt,@object
	.size	reent_errno_pt,4

// char **environ;
	.comm	environ,4,4
	.type	environ,@object
	.size	environ,4
