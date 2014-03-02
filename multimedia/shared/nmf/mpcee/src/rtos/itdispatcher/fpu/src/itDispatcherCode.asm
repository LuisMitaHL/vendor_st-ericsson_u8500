
;	Memory Space __X (memtype 1)

;	Section = .cst.data

;	Constant = true

	.section .cst.data, "aw" ,@progbits
L1:
	.data	-1048576
L2:
	.data	_itDummy

;	Section = .bss

;	Constant = false

;	Initialized to zero = true

	.section .bss, "aw" ,@nobits
	.extern	_rtos_itdispatcher_fpu_it

;	Memory Space __MMIO (memtype 4)

;	Section = .bss4

	.section .bss4, "aw" ,@nobits
	.extern	_AHB_IF_flush
	.global	_itDummy
	.global	_itDispatcher

	.section .text, "ax" ,@progbits
_itDummy:
; Annotation 'directive' :
;      Symbol '_itDummy_local_size' = 0
;      Symbol '_itDummy_savedreg_size' = 0
;      Symbol '_itDummy_spill_size' = 0
;      No Segment defined
.equ _itDummy_local_size , 0
.equ _itDummy_savedreg_size , 0
.equ _itDummy_spill_size , 0
L3:
.begin_bundle
	stmmio_f	flag,_AHB_IF_flush
.end_bundle
.begin_bundle
	rtiAlign
.end_bundle

	.section .text, "ax" ,@progbits
_itDispatcher:
; Annotation 'directive' :
;      Symbol '_itDispatcher_local_size' = 1
;      Symbol '_itDispatcher_savedreg_size' = 41
;      Symbol '_itDispatcher_spill_size' = 0
;      No Segment defined
.equ _itDispatcher_local_size , 1
.equ _itDispatcher_savedreg_size , 41
.equ _itDispatcher_spill_size , 0
L5:
.begin_bundle
	push_d	r2
.end_bundle
.begin_bundle
	push_d	r3
.end_bundle
.begin_bundle
	push_d	r4
.end_bundle
.begin_bundle
	push_d	r5
	xsave	ext2,rh5
.end_bundle
.begin_bundle
	push_d	fp0
.end_bundle
.begin_bundle
	push_d	fp2
.end_bundle
.begin_bundle
	push_d	fp3
.end_bundle
.begin_bundle
	push_d	fp1
	fdivstate_get	fp1
.end_bundle
.begin_bundle
	push	flag
	mode24
.end_bundle
.begin_bundle
	push	fpl1
.end_bundle
.begin_bundle
	push	fph1
	mode24
.end_bundle
.begin_bundle
	push	ax1
.end_bundle
.begin_bundle
	push	ax2
.end_bundle
.begin_bundle
	push	ax3
.end_bundle
.begin_bundle
	push	rh5
	xsave	ext3,rh5
.end_bundle
.begin_bundle
	push	rh5
	xsave	ext4,rh5
.end_bundle
.begin_bundle
	push	rh5
	xsave	ext5,rh5
.end_bundle
.begin_bundle
	push	rh5
.end_bundle
.begin_bundle
	pushlb	
.end_bundle
.begin_bundle
	pushls	
.end_bundle
.begin_bundle
	pushlelc	
.end_bundle
.begin_bundle
	pushlb	
.end_bundle
.begin_bundle
	pushls	
.end_bundle
.begin_bundle
	pushlelc	
.end_bundle
.begin_bundle
	pushlb	
.end_bundle
.begin_bundle
	pushls	
.end_bundle
.begin_bundle
	pushlelc	
.end_bundle
.begin_bundle
	push	max1
.end_bundle
.begin_bundle
	push	min1
.end_bundle
.begin_bundle
	push	sp1
.end_bundle
.begin_bundle
	push	adctl
.end_bundle
.begin_bundle
	push	axx1
.end_bundle
.begin_bundle
	push	rl0
.end_bundle
.begin_bundle
	meai_dec_sp_impl1	sp0,acuspdecone
	xmvi	_rtos_itdispatcher_fpu_it,ax2
.end_bundle
.begin_bundle
	xmv	sp0,ax1
.end_bundle
.begin_bundle
	xmv	ax1,rl2
.end_bundle
.begin_bundle
	addi	rl2,42,rl2
.end_bundle
.begin_bundle
	xmv	rl2,ax1
.end_bundle
.begin_bundle
	ldxi_f	ax1,rl0
.end_bundle
.begin_bundle
	subi_impl	rl0,constone,rl2
	ldx_f	1,axx1
.end_bundle
.begin_bundle
	andi	rl2,31,rl2
.end_bundle
.begin_bundle
	xmvi	8192,adctl
	asli_impl	rl2,constone,rl2
.end_bundle
.begin_bundle
	mea_inc	ax2,rl2
.end_bundle
.begin_bundle
	ldxi_f	ax2,ax1
	meai_inc_impl	ax2,acuincone
.end_bundle
.begin_bundle
	stx_f	ax1,1
.end_bundle
.begin_bundle
	ldxi_f	ax2,ax1
.end_bundle
.begin_bundle
	jalAlign	ax1
.end_bundle
CG0L5:
.begin_bundle
	stx_f	axx1,1
.end_bundle
.begin_bundle
	ldx_f	L1,rl2
.end_bundle
.begin_bundle
	and	rl0,rl2,rl3
	ldx_f	L2,ax1
.end_bundle
.begin_bundle
	xmv	ax1,rl2
.end_bundle
.begin_bundle
	add	rl2,rl3,rl2
	xmv	sp0,ax1
.end_bundle
.begin_bundle
	addi_impl	rl2,constone,rl3
	xmv	ax1,rl2
.end_bundle
.begin_bundle
	addi	rl2,42,rl2
.end_bundle
.begin_bundle
	xmv	rl2,ax1
.end_bundle
.begin_bundle
	stxi_f	rl3,ax1
.end_bundle
.begin_bundle
	stmmio_f	flag,_AHB_IF_flush
.end_bundle
.begin_bundle
	meai_inc_sp_impl1	sp0,acuspincone
.end_bundle
.begin_bundle
	pop	rl0
.end_bundle
.begin_bundle
	pop	axx1
.end_bundle
.begin_bundle
	pop	adctl
.end_bundle
.begin_bundle
	pop	rh5
.end_bundle
.begin_bundle
	pop	min1
.end_bundle
.begin_bundle
	pop	max1
.end_bundle
.begin_bundle
	poplelc	
.end_bundle
.begin_bundle
	popls	
.end_bundle
.begin_bundle
	poplb	
.end_bundle
.begin_bundle
	poplelc	
.end_bundle
.begin_bundle
	popls	
.end_bundle
.begin_bundle
	poplb	
.end_bundle
.begin_bundle
	poplelc	
.end_bundle
.begin_bundle
	popls	
.end_bundle
.begin_bundle
	poplb	
.end_bundle
.begin_bundle
	xmv	rh5,sp1
.end_bundle
.begin_bundle
	pop	rh5
.end_bundle
.begin_bundle
	xload	rh5,ext5
	pop	rh5
.end_bundle
.begin_bundle
	xload	rh5,ext4
	pop	rh5
.end_bundle
.begin_bundle
	xload	rh5,ext3
	pop	rh5
.end_bundle
.begin_bundle
	xload	rh5,ext2
	pop	ax3
.end_bundle
.begin_bundle
	pop	ax2
.end_bundle
.begin_bundle
	pop	ax1
.end_bundle
.begin_bundle
	pop	fph1
.end_bundle
.begin_bundle
	pop	fpl1
.end_bundle
.begin_bundle
	fdivstate_set	fp1
	pop	flag
.end_bundle
.begin_bundle
	pop_d	fp1
.end_bundle
.begin_bundle
	pop_d	fp3
.end_bundle
.begin_bundle
	pop_d	fp2
.end_bundle
.begin_bundle
	pop_d	fp0
.end_bundle
.begin_bundle
	pop_d	r5
.end_bundle
.begin_bundle
	pop_d	r4
.end_bundle
.begin_bundle
	pop_d	r3
.end_bundle
.begin_bundle
	pop_d	r2
.end_bundle
.begin_bundle
	rtsAlign
.end_bundle
