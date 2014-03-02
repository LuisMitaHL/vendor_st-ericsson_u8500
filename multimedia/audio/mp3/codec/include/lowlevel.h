


#ifndef _LOW_LEVEL_
#define _LOW_LEVEL_

#ifdef __flexcc2__
#define mkpragma(a) _Pragma(# a)
#define at_reg(regname) mkpragma(at_reg regname)



asm long mklong( int msp, int lsp )
{
	mv @{msp}, @{}.1
	mv @{lsp}, @{}.0
}

asm long mkplong( int msp, int lsp )
{
	dmv @{msp}, ph
	dmv @{lsp}, pl
	dmvl p, @{}
}

asm void set_addressing_mode( int val )
{
	xmv	@{val}, adctl
}

asm void set_modulo_range1( int YMEM * min, int YMEM * max )
{
	xmv @{min}, min1
	xmv @{max}, max1
}

asm void set_modulo_range2( int YMEM * min, int YMEM * max )
{
	xmv @{min}, min2
	xmv @{max}, max2
}

asm void set_modulo_range3( int YMEM * min, int YMEM * max )
{
	xmv @{min}, min3
	xmv @{max}, max3
}

asm void set_bitrev_base1( int * base )
{
	xmv @{base}, min1
}

asm void set_bitrev_base2( int * base )
{
	xmv @{base}, min2
}

asm void set_bitrev_base3( int * base )
{
	xmv @{base}, min3
}

asm void set_bitrev_basey1( int YMEM * base )
{
	xmv @{base}, min1
}

asm void set_bitrev_basey2( int YMEM * base )
{
	xmv @{base}, min2
}

asm void set_bitrev_basey3( int YMEM* base )
{
	xmv @{base}, min3
}

asm void wsetGmode( void )
{
	xset Gmode
}

asm void wclrGmode( void )
{
	xclear Gmode
}


asm int YMEM * mod_add1( int YMEM * addr, int idx )
{
	circ_mea_inc_idx @{addr}, min1, max1, @{idx}
	xmv @{addr}, @{}
}

asm int YMEM * mod_add2( int YMEM * addr, int idx )
{
	circ_mea_inc_idx @{addr}, min2, max2, @{idx}
	xmv @{addr}, @{}
}





#endif // __flexcc2__

	
#define BITREV_AX1			0x0001
#define MOD_AX1				0x0002
#define BITREV_REL_AX1		0x0003
#define BITREV_AX2			0x0004
#define MOD_AX2				0x0008
#define BITREV_REL_AX2		0x000C
#define BITREV_AX3			0x0010
#define MOD_AX3				0x0020
#define BITREV_REL_AX3		0x0030
#define BITREV_AXX1			0x0040
#define MOD_AXX1			0x0080
#define BITREV_REL_AXX1		0x00C0
#define BITREV_AXX2			0x0100
#define MOD_AXX2			0x0200
#define BITREV_REL_AXX2		0x0300
#define BITREV_AXX3			0x0400
#define MOD_AXX3			0x0800
#define BITREV_REL_AXX3		0x0C00
#define LINEAR_MODE	0x0






#endif // _LOW_LEVEL_
