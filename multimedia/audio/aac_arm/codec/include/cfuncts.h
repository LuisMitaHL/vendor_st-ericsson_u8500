#define	WORKSIZ	5020
#define	NUM1 101
#define	NCOFS 7
#define	pi 3.14159265358979
#define	Epsilon	1.e-10

int	fftcalc(float *in,float	*out,int ord);

int	chebwin(int	npts,double	mindB,double *coefs);
/*--------------------------------------------------------------
inputs:
	npts	=	number of points in	the	window
	mindB	=	outband	attenuation	of window
outputs:
	coefs	=	calculated window coefs
----------------------------------------------------------------*/

int	polyflat(int	npts,double	*x,	double *y, int order,double	*p);
/*--------------------------------------------------------------
inputs:
	npts	=	number of points in	the	data to	flat
	x		=	x values of	data in	an array
	y		=	y values of	data in	an array
	order	=	desired	order
outputs:
	returns	outputorder	(normally equal	to order) 
	p		=	outputorder+1 polynomial coefficients
----------------------------------------------------------------*/

int	flat1(double	*xx,double *yy,int num,double contrl,int idegre,double *coefs,double *sumsqs,double	*aa,double *bb);

int	flat2(double	*coefs,int odegre,double *aa,double	*bb);

double polyval(double *p, double x,	int	order);

int	pflat(double	ratio,int ntaps,int	order ,double mindB, double	*pval);

int	evalwin(double *wintab,int coefindx,double *alphatab,double	*interpolatedwinvalues,int ntaps,int nalpha);
/*---------------------------------------------------------------------------------------------------
inputs:
	wintab					=	the	set	of window coefficients
	coefindx				=	the	index of the coefficient to	be evaluated
	alphatab				=	the	offset table range -.5 to -.5 samples
	interpolatedwinvalues	=	the	interpolated values	for	wintab[coefindx-1] from	-.5	to .5 samples
	ntaps					=	the	number of total	taps in	wintab
	nalpha					=	the	number of elements in alphatab
----------------------------------------------------------------------------------------------------*/

int	sinxcalc(double	*pval, double alpha, int order,	int	ntaps, double *coefs);

int	coefcalc(double	ratio,int ntaps,double mindB, double *coefs);

typedef	struct srcvariables
{
	int	decimratio;
	int	interpratio;
	int	modulo_incntr;
	int	modulo_outcntr;
	int	NTAPSperphase;
	int	iNTAPSsinx;
	int	smplrdy;
	int	idegree;
	int polyptr;
	int outptr;
	double tin;
	double tout;
	double filtergain;
	double fs;
	double fso;
	double polyphaseout;
	double *KF;
	double *polydlyline;
	double *outdlyline;	
	double *p;
}	srcvar;

int	srcinit(float xMINDB,float xpassratiopoly,float	fs,float fso,int INTAPS,
	int	IMAXPHASE,int iNTAPSsinx,int idegree,double	*srcbulk,srcvar	*SRC);
/*------------------------------------------------------------------------------------------
inputs:
	xMINDB			= the rejection	lvl	in +dB for the polyphase and sinx filters
	xpassratiopoly	= ratio	of fc/fs for the polyphase filters (generally between .4 and .5)
	fs				= input	sampling frequency
	fso				= output sampling frequency
	INTAPS			= number of	taps for each polyphase	filter
	IMAXPHASE		= the number of	max	phases for the polyphase filters
	iNTAPSsinx		= the number of	taps for the sinx filter 
	idegree			= the degree of	the	polynomial for the sinx	filter coefs
	srcbulk			= the double memory	start address
	srcvar			= the address for the variable structure

	the	calling	program	must reserve:
	(1)	double memory srcbulk with SRCBULKSIZ=
		(2*INTAPS*(IMAXPHASE+1))+iNTAPSsinx+(((iNTAPSsinx/2)+1)*(idegree+1))
	(2)	the	scrvar structure SRC
--------------------------------------------------------------------------------------------*/

int	src(float *insmpl,int inblksiz,float *outsmpl,int outblksiz,int	*nbgenerated,srcvar	*SRC);

