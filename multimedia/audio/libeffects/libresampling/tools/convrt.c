/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   convrt.c
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include <stdio.h>
#include <math.h> 
#include <stdlib.h>     
#include <ctype.h> 
#include <string.h>     
#include "interpol.h" 
#define HEADSIZ 44
#define MAXHEADSIZ 80
#define MAXCHAR 120
#define MAXCH 8
#define MINCH 1
#define DISPCNTMAX 1000 
#define fmin 10.0
#define fmax 96000.0
#define ogainmin 0.0
#define ogainmax 99999.999999
#define nbmin 8
#define nbmax 32
#define mantmin 0
#define mantmax 16
#define USMAX 0x7FFFFFFE
#define USLONGMAX (long)0x7FFFFFFE
#define USE_DEF_FILE
enum mixtype
{
        NOMIX,
        DOWNMIX,
        UPMIX
};
                
typedef struct inputparameters 
{
        char    *optionname; 
        char    *optiontype; 
        int             *ivar; 
        int             ivarmin;
        int             ivarmax;
        long    *lvar;
        long    lvarmin;
        long    lvarmax; 
        float   *fvar;
        float   fvarmin;
        float   fvarmax; 
        char    *cvar;
        char    *cvarlist; 
}       inpar; 
enum argptrs 
{
        pitype,
        potype,
        pifloatfmt,
        pofloatfmt,
        pifs,
        pofs,
        pinb,
        ponb,
        pimant,
        pomant,
        pinch,
        ponch,
        piend,
        poend,
        pixfn,
        poxfn,
        piskip,
        poskip,
        powrite,
		pogain,
        pdisp,
        pupdate,
		preset,
        pSTOP
};

#define ARGSIZ pSTOP+1
/*------------------------------- Function prototypes --------------------------------------*/ 

int     readhead(FILE *fin,int *nbch,int *nbits,int     *intfs,int *nbytestot,int *datavalid,char *disp);
int     readheadau(FILE *fin,int *nbch,int *nbits,int     *intfs,int *nbytestot,int *datavalid,char *disp);
int     skiphead(FILE *fin,int nnskip,int *datavalid); 
int     insample(FILE *fin,char *filetype,int *datavalid, 
                double *xinputdata,int nbch,int imant,int nbits,int littlend,int transparent, char *floatfmt);  
int     inithead(int nbch,int nbits,int intfs,FILE *fout);
int     initheadau(int nbch,int nbits,int intfs,FILE *fout);
int     closeheader(int num,FILE *fout);
int     closeheaderau(int num,FILE *fout);
int     outsample(FILE *fout,char *filetype,
                double *xoutputdata,int omant,int nbch,int nbits,int littlend,  
                int *bytecnt,long *nnskip,long *nnsamptofile, int transparent, char *floatfmt, int mix, float ogain); 
int     parsecmdline(inpar *prgarg,     char *iargv[],int *argptr,int iargc,
                int     *error,char     *ixfname[],char *oxfname[]); 
void int_to_hexstr(unsigned int input, int ndigit, char *out);
/*------------------------------------------------------------------------------------------*/ 

int     main(int argc, char     *argv[]) 
{ 
        /*-------------------- General variables -----------------------------------------*/ 

        FILE *fin[MAXCH],*fout[MAXCH],*fdef;
        char *NO_NAME,*NO_NAME_KYB,ifloatfmt[18],ofloatfmt[18];
        char *in_name[MAXCH],IN_NAME[MAXCH][MAXCHAR],*out_name[MAXCH],OUT_NAME[MAXCH][MAXCHAR];
        char *finspec,*foutspec,ctemp[2],itype[2],otype[2],iend[2],oend[2],disp[2],update[2],reset[2];
        int     argptr,nbconsumed,ilittlend,olittlend,error,ok,i,nbsrc,inb,onb,imant,omant; 
        int     nbgenerated,tick,datavalid=1,bytecnt[MAXCH],nbinfile,nboutfile,dispcntr,inch,onch,ONCH,inskip; 
        int     nbytestot,ifs,itemp,fileopened,mix; 
        long nskip,nsamptofile,itempl,sav1,sav2;
        double in_float[MAXCH],out_float[MAXCH];
        float fs,fso,ogain,xtemp;
        double time;
        inpar progarg[ARGSIZ]; 
        /*--------------------- specific src variables ------------------------------------*/ 

        srcvar SRC[MAXCH]; 
        double srcbulk[MAXCH][SRCBULKSIZ];
        /*--------------------------------------------------------------------------------*/
        NO_NAME="xx";
        NO_NAME_KYB="x";
        /*-------- All of the program cmdline parameters with default values -------------*/ 

        progarg[pitype].optionname="itype";     
        progarg[pitype].optiontype="c"; 
        progarg[pitype].cvar=itype;     
        progarg[pitype].cvarlist="w,b,f,h,a";
        strcpy (itype,"x");     

        progarg[potype].optionname="otype";     
        progarg[potype].optiontype="c"; 
        progarg[potype].cvar=otype;
        progarg[potype].cvarlist="w,b,f,h,a";
        strcpy (otype,"x");

        progarg[pifloatfmt].optionname="ifloatfmt"; 
        progarg[pifloatfmt].optiontype="c"; 
        progarg[pifloatfmt].cvar=ifloatfmt;
        progarg[pifloatfmt].cvarlist="x";
        strcpy (ifloatfmt,"x"); 

        progarg[pofloatfmt].optionname="ofloatfmt"; 
        progarg[pofloatfmt].optiontype="c"; 
        progarg[pofloatfmt].cvar=ofloatfmt;
        progarg[pofloatfmt].cvarlist="x";
        strcpy (ofloatfmt,"x"); 

        progarg[pifs].optionname="ifs"; 
        progarg[pifs].optiontype="f"; 
        progarg[pifs].fvar=&fs; 
        progarg[pifs].fvarmin=fmin;     
        progarg[pifs].fvarmax=fmax;     
        fs=fmin-1.0; 

        progarg[pofs].optionname="ofs"; 
        progarg[pofs].optiontype="f"; 
        progarg[pofs].fvar=&fso; 
        progarg[pofs].fvarmin=fmin;     
        progarg[pofs].fvarmax=fmax;     
        fso=fmin-1.0;   

        progarg[pinb].optionname="inb"; 
        progarg[pinb].optiontype="i"; 
        progarg[pinb].ivar=&inb; 
        progarg[pinb].ivarmin=nbmin; 
        progarg[pinb].ivarmax=nbmax; 
        inb=nbmin-1;    

        progarg[ponb].optionname="onb"; 
        progarg[ponb].optiontype="i"; 
        progarg[ponb].ivar=&onb; 
        progarg[ponb].ivarmin=nbmin; 
        progarg[ponb].ivarmax=nbmax; 
        onb=nbmin-1;

        progarg[pimant].optionname="imant";     
        progarg[pimant].optiontype="i"; 
        progarg[pimant].ivar=&imant; 
        progarg[pimant].ivarmin=mantmin; 
        progarg[pimant].ivarmax=mantmax; 
        imant=mantmin;  

        progarg[pomant].optionname="omant";     
        progarg[pomant].optiontype="i"; 
        progarg[pomant].ivar=&omant; 
        progarg[pomant].ivarmin=mantmin; 
        progarg[pomant].ivarmax=mantmax; 
        omant=mantmin;  

        progarg[pinch].optionname="inch"; 
        progarg[pinch].optiontype="i"; 
        progarg[pinch].ivar=&inch; 
        progarg[pinch].ivarmin=MINCH; 
        progarg[pinch].ivarmax=MAXCH; 
        inch=MINCH-1;   

        progarg[ponch].optionname="onch"; 
        progarg[ponch].optiontype="i"; 
        progarg[ponch].ivar=&onch; 
        progarg[ponch].ivarmin=MINCH; 
        progarg[ponch].ivarmax=MAXCH; 
        onch=MINCH-1;   

        progarg[piend].optionname="iend"; 
        progarg[piend].optiontype="c"; 
        progarg[piend].cvar=iend; 
        progarg[piend].cvarlist="l,b";
        strcpy (iend,"x"); 

        progarg[poend].optionname="oend"; 
        progarg[poend].optiontype="c"; 
        progarg[poend].cvar=oend; 
        progarg[poend].cvarlist="l,b";
        strcpy (oend,"x"); 

        progarg[pixfn].optionname="ixfn"; 
        progarg[pixfn].optiontype="cifile";     
        for     (i=1;i<MAXCH;i++)
        {
                strcpy(IN_NAME[i],NO_NAME);
                in_name[i]=IN_NAME[i];
        }
        progarg[poxfn].optionname="oxfn"; 
        progarg[poxfn].optiontype="cofile";     
        for     (i=1;i<MAXCH;i++)
        {
                strcpy(OUT_NAME[i],NO_NAME);
                out_name[i]=OUT_NAME[i];
        }
        progarg[piskip].optionname="iskip";     
        progarg[piskip].optiontype="i"; 
        progarg[piskip].ivar=&inskip; 
        progarg[piskip].ivarmin=0; 
        progarg[piskip].ivarmax=USMAX;  
        inskip=USMAX+1; 

        progarg[poskip].optionname="oskip";     
        progarg[poskip].optiontype="l"; 
        progarg[poskip].lvar=&nskip; 
        progarg[poskip].lvarmin=0; 
        progarg[poskip].lvarmax=USLONGMAX; 
        nskip=USLONGMAX+1; 

        progarg[powrite].optionname="owrite"; 
        progarg[powrite].optiontype="l"; 
        progarg[powrite].lvar=&nsamptofile;     
        progarg[powrite].lvarmin=0;     
        progarg[powrite].lvarmax=USLONGMAX; 
        nsamptofile=USLONGMAX+1;

        progarg[pogain].optionname="outputgain"; 
        progarg[pogain].optiontype="f"; 
        progarg[pogain].fvar=&ogain; 
        progarg[pogain].fvarmin=ogainmin;     
        progarg[pogain].fvarmax=ogainmax;     
        ogain=ogainmin-1.0;  		

        progarg[pdisp].optionname="disp"; 
        progarg[pdisp].optiontype="cnoarg";     
        progarg[pdisp].cvar=disp; 
        strcpy (disp,"n"); 

        progarg[pupdate].optionname="update"; 
        progarg[pupdate].optiontype="cnoarg";   
        progarg[pupdate].cvar=update; 
        strcpy (update,"n"); 

        progarg[preset].optionname="reset"; 
        progarg[preset].optiontype="cnoarg";   
        progarg[preset].cvar=reset; 
        strcpy (reset,"n"); 

        progarg[pSTOP].optionname="STOP"; 

        /*--------------------- cmdline parsing ----------------------------------------*/ 
        argptr=1;
        if (argc>=3) 
        {
                strcpy(IN_NAME[0],argv[argptr]);
                in_name[0]=IN_NAME[0];
                if (in_name[0][0]=='-')
                {
                        printf("\n\n********** BAD INPUT FILE NAME **********\n\n"); 
                        exit(1);
                }
                argptr++;
                strcpy(OUT_NAME[0],argv[argptr]);
                out_name[0]=OUT_NAME[0]; 
                if (out_name[0][0]=='-')
                {
                        printf("\n\n********** BAD OUTPUT FILE NAME **********\n\n"); 
                        exit(1);
                }
                argptr++;
        }
        parsecmdline(progarg, argv,&argptr,argc,&error,&in_name[1],&out_name[1]);
        if (error!=0) 
        { 
                printf("\n\n******* Input Parameter Error*******\n\n"); 
                exit(1); 
        }
        /*--------------------- default values  --------------------------*/
        fileopened=0;
#ifdef USE_DEF_FILE
		if (reset[0]!='y')
		{
        	if ((fdef=fopen("convrt.def","r t"))!=0)
			{
				fileopened=1;
			}
		}
#endif
        if (fileopened!=0) fscanf(fdef,"%f",&xtemp);
        else xtemp=44100.0;
        if ((fs<fmin)||(fs>fmax)||(reset[0]=='y')) fs=xtemp;
		
        if (fileopened!=0) fscanf(fdef,"%f",&xtemp);
        else xtemp=44100.0;
        if ((fso<fmin)||(fso>fmax)||(reset[0]=='y')) fso=xtemp;

		if (fileopened!=0) fscanf(fdef,"%d",&itemp);
        else itemp=16;
        if ((inb<nbmin)||(inb>nbmax)||(reset[0]=='y')) inb=itemp;

		if (fileopened!=0) fscanf(fdef,"%d",&itemp);
        else itemp=16;
        if ((onb<nbmin)||(onb>nbmax)||(reset[0]=='y')) onb=itemp;

		if (fileopened!=0) fscanf(fdef,"%d",&itemp);
        else itemp=0;
        if ((imant<mantmin)||(imant>mantmax)||(reset[0]=='y')) imant=itemp;

		if (fileopened!=0) fscanf(fdef,"%d",&itemp);
        else itemp=0;
        if ((omant<mantmin)||(omant>mantmax)||(reset[0]=='y')) omant=itemp;

		if (fileopened!=0) fscanf(fdef,"%d",&itemp);
        else itemp=1;
        if ((inch<MINCH)||(inch>MAXCH)||(reset[0]=='y')) inch=itemp;

		if (fileopened!=0) fscanf(fdef,"%d",&itemp);
        else itemp=1;
        if ((onch<MINCH)||(onch>MAXCH)||(reset[0]=='y')) onch=itemp;

		if (fileopened!=0) fscanf(fdef,"%d",&itemp);
        else itemp=0;
        if ((inskip>USMAX)||(reset[0]=='y')) inskip=itemp;

		if (fileopened!=0) fscanf(fdef,"%ld",&itempl);
        else itempl=0;
        if ((nskip>USLONGMAX)||(reset[0]=='y')) nskip=itempl;

		if (fileopened!=0) fscanf(fdef,"%ld",&itempl);
        else itempl=USLONGMAX;
        if ((nsamptofile>USLONGMAX)||(reset[0]=='y')) nsamptofile=itempl;

		if (fileopened!=0) fscanf(fdef,"%f",&xtemp);
        else xtemp=1.0;
        if ((ogain<ogainmin)||(ogain>ogainmax)||(reset[0]=='y')) ogain=xtemp;

		if (fileopened!=0) fscanf(fdef,"%s",ctemp);
        else strcpy(ctemp,"w");
        if ((itype[0]=='x')||(reset[0]=='y')) strcpy(itype,ctemp);

		if (fileopened!=0) fscanf(fdef,"%s",ctemp);
        else strcpy(ctemp,"w");
        if ((otype[0]=='x')||(reset[0]=='y')) strcpy(otype,ctemp);

		if (fileopened!=0) fscanf(fdef,"%s",ctemp);
        else strcpy(ctemp,"e");
        if ((ifloatfmt[0]=='x')||(reset[0]=='y')) strcpy(ifloatfmt,ctemp);

		if (fileopened!=0) fscanf(fdef,"%s",ctemp);
        else strcpy(ctemp,"e");
        if ((ofloatfmt[0]=='x')||(reset[0]=='y')) strcpy(ofloatfmt,ctemp);

		if (fileopened!=0) fscanf(fdef,"%s",ctemp);
        else strcpy(ctemp,"l");
        if ((iend[0]=='x')||(reset[0]=='y')) strcpy(iend,ctemp);

		if (fileopened!=0) fscanf(fdef,"%s",ctemp);
        else strcpy(ctemp,"l");
        if ((oend[0]=='x')||(reset[0]=='y')) strcpy(oend,ctemp);

		for (i=1;i<MAXCH;i++)
        {
                if (fileopened!=0) fscanf(fdef,"%s\n",IN_NAME[i]);
                else strcpy(IN_NAME[i],NO_NAME);
                if ((strcmp(in_name[i],NO_NAME_KYB)==0)||(reset[0]=='y')) 
                        in_name[i]=NO_NAME;
                else
                {
                        if (strcmp(in_name[i],NO_NAME)==0) in_name[i]=IN_NAME[i];
                }
        }
        for (i=1;i<MAXCH;i++)
        {
                if (fileopened!=0) fscanf(fdef,"%s\n",OUT_NAME[i]);
                else strcpy(OUT_NAME[i],NO_NAME);
                if ((strcmp(out_name[i],NO_NAME_KYB)==0)||(reset[0]=='y')) 
                        out_name[i]=NO_NAME;
                else
                {
                        if (strcmp(out_name[i],NO_NAME)==0) out_name[i]=OUT_NAME[i];
                }
        }
        if (fileopened!=0) fclose(fdef);
        /*------------- write default values to file ---------*/
        if ((update[0]=='y')||(reset[0]=='y'))
        {
                fdef=fopen("convrt.def","w t");
                {
                        fprintf(fdef,"%f %f %d %d %d %d %d %d %d %ld %ld %f\n",
                                        fs,fso,inb,onb,imant,omant,inch,onch,inskip,nskip,nsamptofile,ogain);
                        fprintf(fdef,"%s %s %s %s %s %s \n",itype,otype,ifloatfmt,ofloatfmt,iend,oend);
                        for (i=1;i<MAXCH;i++)
                                fprintf(fdef,"%s\n",in_name[i]);
                        for (i=1;i<MAXCH;i++)
                                fprintf(fdef,"%s\n",out_name[i]);
                }
                fclose(fdef);
        }
		if (reset[0]=='y') exit(0);
        /*--------------------- Display the     usage ------------------------------------------*/
        if (argc<3)     
        {
            printf("\nuse: convrt  INFILE OUTFILE [options]\n");
            printf("INFILE         = filename containing input samples \n"); 
            printf("OUTFILE        = filename containing output samples \n\n");
            printf("---------------------- options and <default values> stored in convrt.def ----------------------\n");
            printf("-itype <%s>,-otype <%s>        = file types (w)av (b)inary (f)loat (h)ex (a)au \n",itype,otype); 
            printf("-ifloatfmt <%s>,-ofloatfmt <%s>= float file type format eg. 10.0f,f,e\n",ifloatfmt,ofloatfmt); 
            printf("-ifs <%5d>,-ofs <%5d>    = sampling freq in Hz\n",(int)fs,(int)fso); 
            printf("-inb <%2d>,-onb <%2d>          = no. of bits/sample 8,16,24,32\n",inb,onb);
            printf("-imant <%2d>,-omant <%2d>      = no. of bits before decimal(-imant 1 for 2x, -omant 1 for x/2)\n",imant,omant);
            printf("-inch* <%d>,-onch <%d>         = no. of channels\n",inch,onch);
            printf("-iend <%s>,-oend <%s>          = (b)ig or (l)ittle endian\n",iend,oend);        
            printf("-iskip  <%12d>       = no. input bytes to skip before conversion\n",inskip); 
            printf("-oskip  <%12ld>       = no. outsamples/channel to skip after conversion\n",nskip);      
            printf("-owrite <%12ld>       = no. outsamples/channel to write after conversion\n",nsamptofile);
			printf("-outputgain <%12.6f>   = outputgain (0.0 to 99999.999999)\n",ogain);
            for (i=1;i<MAXCH;i++)
            {
              		if (strcmp(in_name[i],NO_NAME)==0)
                     		printf("-ixfn%d*  <%12s>      = opt. in filenames for interleaving (x=none) \n",i-1,NO_NAME_KYB);
                  	else
                         	printf("-ixfn%d*  <%12s>      = opt. in filenames for interleaving (x=none)  \n",i-1,in_name[i]);
          	}
            for (i=1;i<MAXCH;i++)
            {
              		if (strcmp(out_name[i],NO_NAME)==0)
                      		printf("-oxfn%d*  <%12s>      = opt. out filenames for deinterleaving (x=none)  \n",i-1,NO_NAME_KYB);
                  	else
                        	printf("-oxfn%d*  <%12s>      = opt. out filenames for deinterleaving (x=none)  \n",i-1,out_name[i]);
            }
            printf("-disp                        = print more information for help menu or during execution \n");
			if (disp[0]=='y')
			{
#ifdef USE_DEF_FILE
                printf("-update                      = update def values in file convrt.def\n");
			 	printf("-reset                       = reset def values in file convrt.def\n");
#endif
				printf(" \n\n\n*COMBINATIONS USING xnch AND 1 OR 2 FILES\n\n");
				printf(" inch onch nbinfiles nboutfiles     result\n");
				printf(" -----------------------------------------------------------------------------------\n");
				printf("   1    1     1          1          mono signal in 1 output file\n");
				printf("   1    1     1          2          not allowed\n");
				printf("   1    1     2          1          mono mixed in 1 output file (signals are added)\n");
				printf("   1    1     2          2          not allowed\n");
				printf("   1    2     1          1          dual mono (L=R) in 1 output file\n");
				printf("   1    2     1          2          not allowed\n");
				printf("   1    2     2          1          stereo signal in 1 output file INFILE to L -ixfn0 to R\n");
				printf("   1    2     2          2          not allowed\n");
				printf("   2    1     1          1          mono mixed in 1 output file (signals are added)\n");
				printf("   2    1     1          2          2 mono files with L to OUTFILE R to -oxfn0 resp\n");
				printf("   2    1     2          1          not allowed\n");
				printf("   2    1     2          2          not allowed\n");
				printf("   2    2     1          1          stereo signal in 1 input file , 1 output file\n");
				printf("   2    2     1          2          not allowed\n");
				printf("   2    2     2          1          not allowed\n");
				printf("   2    2     2          2          not allowed\n");
			}
            exit(1); 
        } 
        /*------------------ Initialization     ---------------------------------*/     
        nbinfile=1;     
        for     (i=1;i<MAXCH;i++)
        {
                if (strcmp(in_name[i],NO_NAME)!=0)
                        nbinfile++;
                else
                        break;
        }
        nboutfile=1;
        for     (i=1;i<MAXCH;i++)
        {
                if (strcmp(out_name[i],NO_NAME)!=0)
                        nboutfile++;
                else
                        break;
        }
        if (itype[0]=='f')      
                finspec="rt"; 
        else 
                finspec="rb"; 
        if (otype[0]=='f')
                foutspec="wt";  
        else
                foutspec="wb"; 
        if (iend[0]=='l')  
                ilittlend=1; 
        else  
                ilittlend=0; 
        if (oend[0]=='l')  
                olittlend=1; 
        else  
                olittlend=0;
        if ((fin[0]=fopen(in_name[0],finspec))==0) 
        { 
                printf("\n\n********** BAD INPUT FILE NAME **********\n\n"); 
                exit(1); 
        } 
        if (itype[0]=='w') 
        {  
                if (readhead(fin[0],&inch,&inb,&ifs,&nbytestot,&datavalid,disp)==0) return 0; 
                fs=(double)ifs; 
        }
        if (itype[0]=='a') 
        {
			if (readheadau(fin[0],&inch,&inb,&ifs,&nbytestot,&datavalid,disp)==0) return 0; 
            fs=(double)ifs; 
        }
        mix=NOMIX;
        ONCH=onch;
        if (inch!=onch) 
        { 
                if (inch>1)     
                {
                        if ((onch==1)&&(nbinfile==1)&&(nboutfile==1))
                        {
                                mix=DOWNMIX;
                                ONCH=inch;
                        }
                        else
                        {
                                if (onch>1)     
                                { 
                                        printf("\n\n******** onch SPECIFICATION ERROR ********\n\n"); 
                                        exit(1); 
                                } 
                                if (nbinfile>1) 
                                {
                                        printf("\n\n******** ixfn SPECIFICATION ERROR ********\n\n"); 
                                        exit(1); 
                                } 
                                if (nboutfile!=inch) 
                                { 
                                        printf("\n\n******** oxfn SPECIFICATION ERROR ********\n\n"); 
                                        exit(1); 
                                }
                        }
                } 
                else 
                {
                        if ((nbinfile==1)&&(nboutfile==1))
                        {
                                mix=UPMIX;
                        }
                        else
                        {       
                                if (nboutfile>1) 
                                { 
                                        printf("\n\n******** oxfn SPECIFICATION ERROR ********\n\n"); 
                                        exit(1); 
                                } 
                                if (nbinfile!=onch)     
                                { 
                                        printf("\n\n******** ixfn SPECIFICATION ERROR ********\n\n"); 
                                        exit(1); 
                                }
                        }       
                } 
        } 
        else 
        {
                if ((inch==1)&&(nbinfile>1)&&(nboutfile==1))
                {
                        mix=DOWNMIX;
                        ONCH=nbinfile;
                }
                else
                {
                        if ((nbinfile>1)||(nboutfile>1)) 
                        { 
                                printf("\n\n******** ixfn or oxfn SPECIFICATION ERROR ********\n\n"); 
                                exit(1); 
                        }
                }
        } 
        for     (i=0;i<nboutfile;i++) 
        { 
                fout[i]=fopen(out_name[i],foutspec); 
        } 
        for     (i=1;i<nbinfile;i++) 
        { 
                if ((fin[i]=fopen(in_name[i],finspec))==0) 
                { 
                        printf("\n\n********** BAD INPUT FILE NAME  **********\n\n"); 
                        exit(1); 
                } 
        } 
        if (itype[0]=='b') 
        { 
                for     (i=0;i<nbinfile;i++)
                        skiphead(fin[i],inskip,&datavalid);
        }
        inb/=8;inb*=8;onb/=8;onb*=8;
        if (otype[0]=='w') 
        {
                for (i=0;i<nboutfile;i++) 
                        inithead(onch,onb,fso,fout[i]); 
        }
        if (otype[0]=='a') 
        {
                for (i=0;i<nboutfile;i++) 
                        initheadau(onch,onb,fso,fout[i]); 
        } 
        if (disp[0]=='y') 
        {
                printf("\n-----------------------------------------------------------------------------------\n"); 
                printf("Input file characteristics:\n"); 
                if (itype[0]=='f')
                {
                        printf("type: %c ifloatfmt: %s fs: %f bits before decimal: %d nbchan: %d iskip: %d \n",
                                        itype[0],ifloatfmt,fs,imant,inch,inskip);
                }
                else
                {
                        printf("type: %c fs: %f bits/sample: %d bits before decimal: %d nbchan: %d endian: %c iskip: %d \n",
                                        itype[0],fs,inb,imant,inch,iend[0],inskip);
                }
                printf("%d input file(s): ",nbinfile);
                for (i=0;i<nbinfile;i++)
                {
                        printf(" %s",in_name[i]);
                }
                printf("\n");
                printf("-----------------------------------------------------------------------------------\n"); 
                printf("Output file characteristics:\n"); 
                if (otype[0]=='f')
                {
                        printf("type: %c ofloatfmt: %s fs: %f bits before decimal: %d nbchan: %d oskip: %ld owrite: %ld outputgain: %f",
                                        otype[0],ofloatfmt,fso,omant,onch,nskip,nsamptofile,ogain);
                }
                else
                {
                        printf("type: %c fs: %f bits/sample: %d bits before decimal: %d nbchan: %d endian: %c oskip: %ld owrite: %ld outputgain: %f\n",
                                        otype[0],fso,onb,omant,onch,oend[0],nskip,nsamptofile,ogain);
                }
                printf("%d output file(s): ",nboutfile);
                for (i=0;i<nboutfile;i++)
                {
                        printf(" %s",out_name[i]);
                }
                printf("\n");
                printf("------------------------------------------------------------------------------------\n"); 
        }
        if (inch>ONCH)
                nbsrc=inch;
        else
                nbsrc=ONCH;
        for     (i=0;i<nbsrc;i++)
        { 
                srcinit((double)MINDB,(double)passratiopoly,fs,fso,(int)NTAPS,(int)MAXPHASE,
                                (int)NTAPSsinx,(int)degree,&srcbulk[i][0],&SRC[i]);
        } 
        time=0.0; 
        tick=0;
        for (i=0;i<MAXCH;i++)
        {
                bytecnt[i]=0;
        }
        nbconsumed=1; 
        dispcntr=0;

        /*-------------------------- Execution of program --------------------------------*/ 

        if (fs==fso) 
        { 
                printf("Direct Conversion of input samples\n");
                while(1) 
                {
                        for     (i=0;i<nbinfile;i++)
                        {
                                if ((((itype[0]=='b')||(itype[0]=='w')||(itype[0]=='h')||(itype[0]=='a'))&&
											((otype[0]=='b')||(otype[0]=='w')||(otype[0]=='h')||(otype[0]=='a')))&&(imant==omant))
                                {
                                        insample(fin[i],itype,&datavalid,&in_float[i],inch,inb,imant,ilittlend,1,ifloatfmt);
                                }
                                else
                                {
                                        insample(fin[i],itype,&datavalid,&in_float[i],inch,inb,imant,ilittlend,0,ifloatfmt);
                                }
                        }
                        if (datavalid!=1) break;
                        sav1=nskip;sav2=nsamptofile;
                        if (nskip==0)   
                        { 
                                if (nsamptofile>0) 
                                {
                                        time+=1.0/fso;
                                }
                        }
                        for     (i=0;i<nboutfile;i++)
                        { 
                                nskip=sav1;nsamptofile=sav2;
                                if ((((itype[0]=='b')||(itype[0]=='w')||(itype[0]=='h')||(itype[0]=='a'))&&
											((otype[0]=='b')||(otype[0]=='w')||(otype[0]=='h')||(otype[0]=='a')))&&(imant==omant))
                                {
                                        outsample(fout[i],otype,&in_float[i],omant,ONCH,onb,olittlend,&bytecnt[i], 
                                                        &nskip,&nsamptofile,1,ofloatfmt,mix,ogain);
                                }
                                else
                                {
                                        outsample(fout[i],otype,&in_float[i],omant,ONCH,onb,olittlend,&bytecnt[i], 
                                                        &nskip,&nsamptofile,0,ofloatfmt,mix,ogain);
                                }
                        } 
                        tick=(int)time; 
                        dispcntr++;     
                        if (dispcntr>DISPCNTMAX) 
                        { 
                                dispcntr=0;     
                                if (disp[0]=='y') printf("Elapsed equivalent sample time in seconds: %d\r",tick); 
                        }
						if (nsamptofile==0) break;
                }
                if (disp[0]=='y') printf("Elapsed equivalent sample time in seconds: %f\r",time); 
        } 
        else 
        { 
                printf("Interpolation/decimation of input samples\n"); 
                while(1) 
                { 
                        if (nbconsumed==1) 
                        {
                                for     (i=0;i<nbinfile;i++) 
                                        ok=insample(fin[i],itype,&datavalid,&in_float[i],inch,inb,imant,ilittlend,0,ifloatfmt); 
                        } 
                        for     (i=0;i<nbsrc;i++)
                        {
                                nbconsumed=src(&in_float[i],ok,&out_float[i],1,&nbgenerated,&SRC[i]); 
                        } 
                        if (nbgenerated==1)      
                        {
                                if (nskip==0)   
                                { 
                                        if (nsamptofile>0) 
                                        {
                                                time+=1.0/fso;
                                        }
                                }
                                sav1=nskip;sav2=nsamptofile; 
                                for     (i=0;i<nboutfile;i++)
                                {
                                        nskip=sav1;nsamptofile=sav2;
                                        outsample(fout[i],otype,&out_float[i],omant,ONCH,onb,olittlend,&bytecnt[i], 
                                                        &nskip,&nsamptofile,0,ofloatfmt,mix,ogain);
                                }       
                        } 
                        tick=(int)time; 
                        dispcntr++;     
                        if (dispcntr>DISPCNTMAX) 
                        { 
                                dispcntr=0;     
                                if (disp[0]=='y') printf("Elapsed equivalent sample time in seconds: %d\r",tick); 
                        } 
                        if ((nbconsumed==0)&&(nbgenerated==0)) break;
						if (nsamptofile==0) break;
                }
                if (disp[0]=='y') printf("Elapsed equivalent sample time in seconds: %f\r",time); 
        } 
        /*-------------------------------- Shut down -------------------------------------*/ 

        if (disp[0]=='y') printf("\n"); 
        if (otype[0]=='w') 
        { 
                for     (i=0;i<nboutfile;i++) 
                        closeheader(bytecnt[i],fout[i]); 
        }
        if (otype[0]=='a') 
        { 
                for     (i=0;i<nboutfile;i++) 
                        closeheaderau(bytecnt[i],fout[i]); 
        }
        for     (i=0;i<nbinfile;i++)
                fclose(fin[i]); 
        for     (i=0;i<nboutfile;i++)
                fclose(fout[i]); 
        return(0); 
} 
/*---------------------------- Functions -------------------------------------------*/ 

int     readhead(FILE *fin,int *nbch,int *nbits,int     *intfs,int *nbytestot,int *datavalid,char *disp) 
{ 
        int     i,nhead; 
        unsigned char sample; 
        unsigned int tempdata,indata,subchunk2id=0;
		if (disp[0]=='y') printf("---------- reading wav file header ---------\n");
        nhead=0;
        //--------- reading ChunkID "RIFF=0x52494646"
        *datavalid=fread(&sample,1,1,fin);
		if (sample!=0x52)
		{
			printf("not a wav file\n");
			return 0;
		}
		*datavalid=fread(&sample,1,1,fin);
		if (sample!=0x49)
		{
			printf("not a wav file\n");
			return 0;
		}
		*datavalid=fread(&sample,1,1,fin);
		if (sample!=0x46)
		{
			printf("not a wav file\n");
			return 0;
		}
		*datavalid=fread(&sample,1,1,fin);
		if (sample!=0x46)
		{
			printf("not a wav file\n");
			return 0;
		}
		nhead+=4;
        tempdata=0;     
        for     (i=0;i<4;i++) 
        { 
                *datavalid=fread(&sample,1,1,fin); 
                indata=(unsigned int)sample; 
                indata<<=(8*i); 
                tempdata|=indata; 
                nhead++; 
        } //--------- reading ChunkSize 
        *nbytestot=(int)tempdata+8;
        for     (i=0;i<14;i++) 
        { 
                *datavalid=fread(&sample,1,1,fin); 
                nhead++; 
        } 
        *datavalid=fread(&sample,1,1,fin); 
        nhead++; 
        *nbch=sample;
        *datavalid=fread(&sample,1,1,fin); 
        nhead++; 
        tempdata=0;     
        for     (i=0;i<4;i++) 
        { 
                *datavalid=fread(&sample,1,1,fin); 
                nhead++; 
                indata=(unsigned int)sample; 
                indata<<=(8*i); 
                tempdata|=indata; 
        } //---------- read fs ------------------- 
        *intfs=(int)tempdata;
        for     (i=0;i<6;i++) 
        { 
                *datavalid=fread(&sample,1,1,fin); 
                nhead++; 
        } 
        *datavalid=fread(&sample,1,1,fin); 
        nhead++; 
        *nbits=sample;

        //      while (nhead<HEADSIZ)
        while ((subchunk2id != 0x64617461)&&(nhead<MAXHEADSIZ))         
        { 
                *datavalid=fread(&sample,1,1,fin);
                subchunk2id=(subchunk2id<<8)|sample;
                nhead++; 
        }
        i=0;
        while ((i<4)&&(nhead<MAXHEADSIZ))
        {
                *datavalid=fread(&sample,1,1,fin);
                i++;
                nhead++;
        }
        //      printf("nhead: %d\n",nhead);
		*nbytestot-=nhead;
		if (disp[0]=='y') printf("nbytes/per channel announced: %d",*nbytestot/(*nbch));
        return 1 ; 
}
int     readheadau(FILE *fin,int *nbch,int *nbits,int     *intfs,int *nbytestot,int *datavalid ,char *disp) 
{
	unsigned char ibuf;
	unsigned int offset,type;
	*datavalid=1;
	*nbits=16;
	if (disp[0]=='y') printf("---------- reading au file header ---------\n");
	ibuf=fgetc(fin);	//ck for magic number
	if (ibuf!=0x2e)
	{
		printf("not an au file\n");
		return 0;
	}
	ibuf=fgetc(fin);	
	if (ibuf!=0x73)
	{
		printf("not an au file\n");
		return 0;
	}
	ibuf=fgetc(fin);	
	if (ibuf!=0x6e)
	{
		printf("not an au file\n");
		return 0;
	}
	ibuf=fgetc(fin);	
	if (ibuf!=0x64)
	{
		printf("not an au file\n");
		return 0;
	}

	ibuf=fgetc(fin);//get offset
	offset=ibuf;
	offset<<=8;
	ibuf=fgetc(fin);
	offset|=ibuf;
	offset<<=8;
	ibuf=fgetc(fin);
	offset|=ibuf;
	offset<<=8;
	ibuf=fgetc(fin);
	offset|=ibuf;
	if (disp[0]=='y') printf("offset announced: %d\n",offset);

	ibuf=fgetc(fin);//get data size
	*nbytestot=ibuf;
	*nbytestot<<=8;
	ibuf=fgetc(fin);
	*nbytestot|=ibuf;
	*nbytestot<<=8;
	ibuf=fgetc(fin);
	*nbytestot|=ibuf;
	*nbytestot<<=8;
	ibuf=fgetc(fin);
	*nbytestot|=ibuf;

	ibuf=fgetc(fin);//get encoded type
	type=ibuf;
	type<<=8;
	ibuf=fgetc(fin);
	type|=ibuf;
	type<<=8;
	ibuf=fgetc(fin);
	type|=ibuf;
	type<<=8;
	ibuf=fgetc(fin);
	type|=ibuf;
	if (type!=3)
	{
		printf("only 16-bit linear files are recognized\n");
		return 0;
	}
	if (disp[0]=='y') printf("encoded type announced: %d\n",type);

	ibuf=fgetc(fin);//get sample rate
	*intfs=ibuf;
	*intfs<<=8;
	ibuf=fgetc(fin);
	*intfs|=ibuf;
	*intfs<<=8;
	ibuf=fgetc(fin);
	*intfs|=ibuf;
	*intfs<<=8;
	ibuf=fgetc(fin);
	*intfs|=ibuf;

	ibuf=fgetc(fin);//get nbch
	*nbch=ibuf;
	*nbch<<=8;
	ibuf=fgetc(fin);
	*nbch|=ibuf;
	*nbch<<=8;
	ibuf=fgetc(fin);
	*nbch|=ibuf;
	*nbch<<=8;
	ibuf=fgetc(fin);
	*nbch|=ibuf;
	if (disp[0]=='y') printf("number of bytes announced per channel: %d",*nbytestot/(*nbch));

	if(feof(fin)) *datavalid=0;
	return 1;
}
int     skiphead(FILE *fin,int nnskip,int *datavalid) 
{ 
        int     i; 
        unsigned char sample; 
        i=nnskip; 
        for     (i=0;i<nnskip;i++) 
        { 
                *datavalid=fread(&sample,1,1,fin); 
        } 
        return(0); 
} 
int     insample(FILE *fin,char *filetype,int *datavalid, 
                double *xinputdata,int nbch,int nbits,int imant,int littlend, int transparent, char *floatfmt) 
{ 
        int     iter,filedata,signeddata,i,nbytes; 
        unsigned char sample; 
        unsigned int tempdata,indata;
        double xxmult;
		float ftemp;
        char fmt[20];
        strcpy(fmt,"%");
        strcat(fmt,floatfmt);
        strcat(fmt,"\n");
        xxmult=xmult/pow(2,imant);
        nbytes=nbits/8; 
        for     (iter=0;iter<nbch;iter++) 
        { 
                if (filetype[0]=='f') 
                { 
                        *datavalid=fscanf(fin,fmt,&ftemp);
						xinputdata[iter]=(double)ftemp;
//						 *datavalid=fscanf(fin,fmt,&xinputdata[iter]);
                }
                else
                {
                        if ((filetype[0]=='b')||(filetype[0]=='w')||(filetype[0]=='a'))
                        { 
                                tempdata=0;     
                                for     (i=0;i<nbytes;i++) 
                                {
                                        {
                                                *datavalid=fread(&sample,1,1,fin); 
                                                indata=(unsigned int)sample;
                                        }
                                        if (littlend==0) 
                                        { 
                                                indata<<=(8*(nbytes-i-1)); 
                                        } 
                                        else 
                                        { 
                                                indata<<=(8*i); 
                                        } 
                                        tempdata|=indata; 
                                } 
                                if (*datavalid==1) 
                                { 
                                        /*---------     wav     file only particularity -----------*/ 
                                        if ((nbytes==1)&&(filetype[0]=='w')) 
                                        { 
                                                signeddata=((int)tempdata-128)<<24;     
                                        } 
                                        /*-------------------------------------------------*/ 
                                        else 
                                        { 
                                                signeddata=(int)(tempdata<<((4-nbytes)*8));     
                                        } 
                                } 
                        } // if ((filetype[0]=='b')||(filetype[0]=='w'))
                        else
                        {       
                                *datavalid=fscanf(fin,"%x\n",&tempdata);
                                signeddata=(int)(tempdata<<((4-nbytes)*8));
                        }  // if ((filetype[0]=='b')||(filetype[0]=='w'))
                        filedata=signeddata;
                        if (transparent==0)
                                xinputdata[iter]=(double)filedata/xxmult;
                        else
                                xinputdata[iter]=(double)filedata;
                }  //if (filetype[0]=='f')
        }
        if (*datavalid==1) 
                return(1);
        else
                return(0); 
} 
int     inithead(int nbch,int nbits,int intfs,FILE *fout) 
{ 
        int     ptr,i,tempdata,nbytes,headercnt; 
        unsigned int HEAD[HEADSIZ];     
        unsigned char sample; 
        nbytes=nbits/8; 
        HEAD[0] =0x52; 
        HEAD[1] =0x49; 
        HEAD[2] =0x46; 
        HEAD[3] =0x46; 
        HEAD[4] =0;     
        HEAD[5] =0;     
        HEAD[6] =0;     
        HEAD[7] =0;     
        HEAD[8] =0x57; 
        HEAD[9] =0x41; 
        HEAD[10]=0x56; 
        HEAD[11]=0x45; 
        HEAD[12]=0x66; 
        HEAD[13]=0x6d; 
        HEAD[14]=0x74; 
        HEAD[15]=0x20; 
        HEAD[16]=16; 
        HEAD[17]=0;     
        HEAD[18]=0;     
        HEAD[19]=0;     
        HEAD[20]=1;     
        HEAD[21]=0;     
        HEAD[22]=nbch; 
        HEAD[23]=0;     
        tempdata=intfs; 
        ptr=24; 
        for     (i=0;i<4;i++) 
        { 
                sample=(unsigned char)(tempdata&0x00ff); 
                HEAD[ptr+i]=(int)sample; 
                tempdata>>=8; 
        } 
        tempdata=nbch*nbytes*intfs;     
        ptr=28; 
        for     (i=0;i<4;i++) 
        { 
                sample=(unsigned char)(tempdata&0x00ff); 
                HEAD[ptr+i]=(int)sample; 
                tempdata>>=8; 
        } 
        HEAD[32]=nbch*nbytes; 
        HEAD[33]=0;     
        HEAD[34]=8*nbytes; 
        HEAD[35]=0;     
        HEAD[36]=0x64; 
        HEAD[37]=0x61; 
        HEAD[38]=0x74; 
        HEAD[39]=0x61; 
        HEAD[40]=0;     
        HEAD[41]=0;     
        HEAD[42]=0;     
        HEAD[43]=0;     
        headercnt=HEADSIZ; 
        while (headercnt>0)     
        { 
                sample=(unsigned char)HEAD[HEADSIZ-headercnt]; 
                fwrite(&sample,1,1,fout); 
                headercnt--; 
        } 
        return(0); 
}
int     initheadau(int nbch,int nbits,int intfs,FILE *fout) 
{
	unsigned int nbytes=0xffffffff;
	char ibuf;
	fputc(0x2e,fout);//magic number
	fputc(0x73,fout);
	fputc(0x6e,fout);
	fputc(0x64,fout);

	fputc(0x0,fout);//offset
	fputc(0x0,fout);
	fputc(0x0,fout);
	fputc(0x18,fout);

	ibuf=nbytes>>24;//data size
	fputc(ibuf,fout); 
	ibuf=nbytes>>16;
	fputc(ibuf,fout); 
	ibuf=nbytes>>8;
	fputc(ibuf,fout); 
	ibuf=nbytes;
	fputc(ibuf,fout);

	fputc(0x0,fout);//encoding type 16-bit linear
	fputc(0x0,fout); 
	fputc(0x0,fout); 
	fputc(0x3,fout);

	ibuf=intfs>>24;//sample rate
	fputc(ibuf,fout); 
	ibuf=intfs>>16;
	fputc(ibuf,fout); 
	ibuf=intfs>>8;
	fputc(ibuf,fout); 
	ibuf=intfs;
	fputc(ibuf,fout);

	fputc(0x0,fout);//,ch
	fputc(0x0,fout);
	fputc(0x0,fout);
	fputc(nbch,fout);
	return(0); 
} 
int     closeheader(int num,FILE *fout) 
{ 
        int     i,tempdata,total,total1;
        unsigned char sample;   
        total=HEADSIZ+num-8; 
        tempdata=total; 
        fseek(fout,4L,0); 
        for     (i=0;i<4;i++) 
        { 
                sample=(unsigned char)(tempdata&0x00ff); 
                fwrite(&sample,1,1,fout); 
                tempdata>>=8; 
        } 
        total1=(total-36); 
        tempdata=total1; 
        fseek(fout,40L,0); 
        for     (i=0;i<4;i++) 
        { 
                sample=(unsigned char)(tempdata&0x00ff); 
                fwrite(&sample,1,1,fout); 
                tempdata>>=8; 
        } 
        return(0); 
}
int     closeheaderau(int num,FILE *fout) 
{
	unsigned int nbytes;
	unsigned char ibuf;

	nbytes=num;
	fseek(fout,((long)8),SEEK_SET);
	ibuf=nbytes>>24;//data size
	fputc(ibuf,fout); 
	ibuf=nbytes>>16;
	fputc(ibuf,fout); 
	ibuf=nbytes>>8;
	fputc(ibuf,fout); 
	ibuf=nbytes;
	fputc(ibuf,fout);
	return(0); 
} 
int     outsample(FILE *fout,char *filetype, 
                double *xoutputdata,int omant,int nbch,int nbits,int littlend,int *bytecnt, 
                long *nnskip,long       *nnsamptofile, int transparent, char *floatfmt, int mix, float ogain) 
{ 
        int     i,iter,nbytes,filedata,datain,roundval;
        unsigned int hexdata;
        unsigned char sample;
        double xxmult,xmixdata=0;
        char fmt[20];
        char hexval[10];
        strcpy(fmt,"%");
        strcat(fmt,floatfmt);
        strcat(fmt,"\n");
        xxmult=xmult/pow(2,omant);
        nbytes=nbits/8;
        roundval=0;
        if (nbits<32)
                roundval=1<<(31-nbits);
        if (*nnskip==0) 
        { 
                if (*nnsamptofile>0) 
                { 
                        for (iter=0;iter<nbch;iter++) 
                        {
							    if (ogain!=1.00)
								{
									xoutputdata[iter]*=ogain;
								}
                                if (mix==DOWNMIX)
                                        xmixdata+=xoutputdata[iter];
                                else
                                {
                                        if (mix==UPMIX)
                                                xmixdata=xoutputdata[0];
                                        else
                                                xmixdata=xoutputdata[iter];
                                }
                                if (filetype[0]=='f') 
                                {
                                        if (mix==DOWNMIX)
                                        {
                                                if (iter==(nbch-1))
                                                        fprintf(fout,fmt,xmixdata);
                                        }
                                        else
                                                fprintf(fout,fmt,xmixdata);
                                } 
                                else 
                                {
                                        if (transparent==0)
                                                datain=(int)(xmixdata*xxmult);
                                        else
                                                datain=(int)xmixdata;
                                        datain+=roundval;
                                        if ((filetype[0]=='w')||(filetype[0]=='b')||(filetype[0]=='a'))
                                        {
                                                if (littlend==0) 
                                                { 
                                                        filedata=datain; 
                                                } 
                                                else 
                                                { 
                                                        filedata=(datain>>(8*(4-nbytes))); 
                                                }
                                                for (i=0;i<nbytes;i++) 
                                                { 
                                                        if (littlend==0) 
                                                        { 
                                                                sample=(unsigned char)((filedata&0xff000000)>>24); 
                                                        } 
                                                        else 
                                                        { 
                                                                sample=(unsigned char)(filedata&0x00ff); 
                                                        }
                                                        /*----- wav     file only particularity -------*/  
                                                        if ((nbytes==1)&&(filetype[0]=='w')) 
                                                        { 
                                                                if (sample<128) sample+=128; 
                                                                else sample-=128; 
                                                        } 
                                                        /*-----------------------------------------*/
                                                        if (mix==DOWNMIX)
                                                        {
                                                                if (iter==(nbch-1))
                                                                {
                                                                        fwrite(&sample,1,1,fout);
                                                                        (*bytecnt)++; 
                                                                }
                                                        }
                                                        else
                                                        {
                                                                fwrite(&sample,1,1,fout);
                                                                (*bytecnt)++; 
                                                        }
                                                        if (littlend==0) 
                                                        { 
                                                                filedata<<=8; 
                                                        } 
                                                        else 
                                                        { 
                                                                filedata>>=8; 
                                                        } 
                                                } //for (i=0;i<nbytes;i++)
                                        } //if ((filetype[0]=='w')||(filetype[0]=='b'))
                                        else // so it's hex format
                                        { 
                                                hexdata=(unsigned int)datain>>((4-nbytes)*8);
                                                int_to_hexstr(hexdata, nbytes*2, hexval);
                                                if (mix==DOWNMIX)
                                                {
                                                        if (iter==(nbch-1))
                                                                fprintf(fout,"%s\n",hexval);
                                                }
                                                else
                                                        fprintf(fout,"%s\n",hexval);
                                        }
                                }  //if (filetype[0]=='f') 
                        }  //for        (iter=0;iter<nbch;iter++)
                        if (*nnsamptofile>0) (*nnsamptofile)--; 
                } 
        } 
        else 
        { 
                (*nnskip)--; 
        } 
        return(0); 
} 
int     parsecmdline(inpar *prgarg ,char *iargv[],int *argptr,int iargc,
                int     *error,char     *ixfname[],char *oxfname[])     
{ 
        int     i,j,len,len1,found,fileno,iter,foundcompare; 
        char *option,temp[20];
        *error=0;
        while (*argptr<iargc) 
        {
                if (iargv[*argptr][0]=='-')     
                {
                        found=0; 
                        i=0; 
                        while ((found==0)&&(prgarg[i].optionname!="STOP")) 
                        {
                                option=iargv[*argptr];
                                len1=strlen(prgarg[i].optionname); 
                                len=strlen(iargv[*argptr])-1; 
                                if (len>len1) len=len1; 
                                found=1; 
                                for     (j=0;j<len;j++) 
                                { 
                                        if (iargv[*argptr][j+1]!=prgarg[i].optionname[j]) 
                                                found=0; 
                                } 
                                if (found==0) 
                                { 
                                        i++; 
                                } 
                        } 
                        if (found!=0) 
                        { 
                                if (prgarg[i].optiontype=="cnoarg")     
                                { 
                                        strcpy(prgarg[i].cvar,"y");     
                                } 
                                else 
                                { 
                                        (*argptr)++;
                                        if (*argptr>=iargc)
                                        {
                                                *error=1;
                                                return(0);
                                        }
                                        if (iargv[*argptr][0]=='-')
                                        {
                                                *error=1;
                                                return(0);
                                        }
                                        if (prgarg[i].optiontype=="c") 
                                        { 
                                                len1=strlen(iargv[*argptr]); 
                                                if (len1==0) 
                                                {
                                                        *error=1;
                                                        return(0);
                                                }
                                                else
                                                {
                                                        len=strlen(prgarg[i].cvarlist);
                                                        iter=0;
                                                        j=0;
                                                        foundcompare=1;
                                                        while ((len>0)&&(prgarg[i].cvarlist[iter]!='x'))
                                                        {
                                                                if (prgarg[i].cvarlist[iter]==',') 
                                                                {
                                                                        if (foundcompare==1) break;
                                                                        foundcompare=1;
                                                                        j=0;
                                                                        iter++;
                                                                        len--;
                                                                }
                                                                if (((iargv[*argptr][j])!=prgarg[i].cvarlist[iter])&&
                                                                                (j<len1))
                                                                { 
                                                                        foundcompare=0;
                                                                }
                                                                j++;
                                                                iter++;
                                                                len--;
                                                        }
                                                        if (foundcompare==0) 
                                                        {
                                                                *error=1;
                                                                return(0);
                                                        }
                                                        else
                                                        {
                                                                strcpy(prgarg[i].cvar,iargv[*argptr]);
                                                        }
                                                } 
                                        } 
                                        else if (prgarg[i].optiontype=="cifile") 
                                        {
                                                if (strlen(iargv[*argptr])==0)
                                                {
                                                        *error=1;
                                                        return(0);
                                                }
                                                strcpy(temp,&option[len1+1]);
                                                fileno=atoi(temp);
                                                if (fileno<MAXCH-1)
                                                        ixfname[fileno]=iargv[*argptr];
                                                else
                                                {
                                                        *error=1;
                                                        return(0);
                                                }
                                        } 
                                        else if (prgarg[i].optiontype=="cofile") 
                                        {
                                                if (strlen(iargv[*argptr])==0) 
                                                {       
                                                        *error=1;
                                                        return(0);
                                                }
                                                strcpy(temp,&option[len1+1]);
                                                fileno=atoi(temp);
                                                if (fileno<MAXCH-1)
                                                        oxfname[fileno]=iargv[*argptr];
                                                else
                                                {
                                                        *error=1;
                                                        return(0);
                                                }
                                        } 
                                        else if (prgarg[i].optiontype=="i")     
                                        {
                                                if (strlen(iargv[*argptr])==0) 
                                                {
                                                        *error=1;
                                                        return(0);
                                                }
                                                *(prgarg[i].ivar)=atoi(iargv[*argptr]); 
                                                if (*(prgarg[i].ivar)<prgarg[i].ivarmin) 
                                                {
                                                        *error=1;
                                                        return(0);
                                                }
                                                if (*(prgarg[i].ivar)>prgarg[i].ivarmax) 
                                                {
                                                        *error=1;
                                                        return(0);
                                                }
                                        } 
                                        else if (prgarg[i].optiontype=="l")     
                                        {
                                                if (strlen(iargv[*argptr])==0) 
                                                {
                                                        *error=1;
                                                        return(0);
                                                }
                                                *(prgarg[i].lvar)=atol(iargv[*argptr]); 
                                                if (*(prgarg[i].lvar)<prgarg[i].lvarmin) 
                                                {
                                                        *error=1;
                                                        return(0);
                                                }
                                                if (*(prgarg[i].lvar)>prgarg[i].lvarmax) 
                                                {
                                                        *error=1;
                                                        return(0);
                                                }
                                        } 
                                        else if (prgarg[i].optiontype=="f")     
                                        {
                                                if (strlen(iargv[*argptr])==0) 
                                                {
                                                        *error=1;
                                                        return(0);
                                                }
                                                *(prgarg[i].fvar)=(atof(iargv[*argptr]));
                                                if (*(prgarg[i].fvar)<prgarg[i].fvarmin) 
                                                {
                                                        *error=1;
                                                        return(0);
                                                }
                                                if (*(prgarg[i].fvar)>prgarg[i].fvarmax) 
                                                {
                                                        *error=1;
                                                        return(0);
                                                }

                                        } 
                                } 
                                (*argptr)++;                     
                        } 
                        else 
                        { 
                                *error=1;
                                return(0);
                        } 
                } 
                else 
                { 
                        *error=1;
                        return(0);
                } 
        } 
        return(0); 
} 
void int_to_hexstr(unsigned int input, int ndigit, char *out)
{
        char chartable[16]={'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};
        int i;
        unsigned int temp;
        temp=input;
        out[ndigit]=0;
        for (i=0;i<ndigit;i++)
        {
                out[ndigit-i-1]=chartable[temp%16];
                temp/=16;
        }
}

