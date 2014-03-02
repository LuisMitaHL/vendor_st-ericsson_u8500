#include <math.h>
#include "ZXImgFilterWavelet.h"
#include "ZXImgCommon.h"

#define __ENABLE_EDGE_AVOID__

void do_synthesis(int8_t* p_src, int8_t* p_dst, int width, int height);

int32_t sndaWaveletFilterInit(int32_t* pHandle, uint32_t width, uint32_t height, float_t threshold, float_t low, uint32_t threads_cnt)
{
	WaveletFilter	*pFilter = 0;
	int32_t	 ret = 0; //OK 
	
    //#1 allocate memory
    pFilter = (WaveletFilter*)malloc_aligned(sizeof(WaveletFilter), 16);
	if(pFilter==NULL)
	{
		LOGE("WaveletFilterInit error: pFilter malloc!\n");
		ret = -1;
		goto	_error;
	}
	memset(pFilter,0,sizeof(WaveletFilter));
    pFilter->buf = (uint8_t*)malloc_aligned(height*width*(3+threads_cnt)+width*20,32); //!! reduce memory after optimization
	if(pFilter->buf==NULL)
	{
		LOGE("WaveletFilterInit error: pFilter->buf malloc!\n");
		ret = -1;
		goto	_error;
	}
	
	//#2 reserve the width&height and return
	pFilter->width = width;
	pFilter->height = height;
	pFilter->threads_cnt = threads_cnt;

	//#3 set some parameters
	pFilter->threshold = threshold>4.0f?4.0f:threshold;
	pFilter->low       = low;
	pFilter->one_low_thr = (1-low) * threshold;
	pFilter->tholds[4] = 1; 
	pFilter->tholds[3] = 2; 
	pFilter->tholds[2] = 3; 
	pFilter->tholds[1] = 6; 
	pFilter->tholds[0] = 16;

	//#4 set func pointers
#ifdef __ENABLE_EDGE_AVOID__
	pFilter->func_hat_transform  = hat_transform_ea;
#else
	pFilter->func_hat_transform  = hat_transform;
#endif
	pFilter->func_calc_stdev     = calc_stdev;
	pFilter->func_calc_threshold = calc_threshold;

	*pHandle = (uint32_t)pFilter;
	return ret;
	
_error:
	if(pFilter)
	{
		if(pFilter->buf)
		{
			free_aligned(pFilter->buf);
			pFilter->buf = NULL;
		}
		
		free_aligned(pFilter);
	}
	
    return ret;
}

void  sndaWaveletFilterRelease(int Handle)
{
	WaveletFilter* pFilter = (WaveletFilter*)Handle;
	if(pFilter)
	{
		if(pFilter->buf)
		{
			free_aligned(pFilter->buf);
			pFilter->buf = NULL;
		}
		
		free_aligned(pFilter);
	}
    return;
}


int32_t sndaWaveletFilterProcess(int32_t Handle, uint8_t* p_src, uint8_t* p_dst)
{
	WaveletFilter* pFilter;
	int size;

	pFilter = (WaveletFilter*)Handle;
	size   = pFilter->width * pFilter->height;

	//Process Y channel
	FilterWaveletC1(Handle, p_src, p_dst, 0, 1, 1);
	
	//Process U channels
//	FilterWaveletC1(Handle, p_src+size, p_dst+size, 0, 2, 2);

	//Process V channel
//	FilterWaveletC1(Handle, p_src+size, p_dst+size, 1, 2, 2);

	return WAVELET_FILTER_OK;

}


// input: hpass is char type and lpass is uchar type.
// output: hpass is char type and lpass is uchar type. 
void calc_threshold(uint8_t* fimg[3], int32_t width, int32_t height, 
				    uint32_t lpass, uint32_t hpass, uint8_t th, float one_low_stdev[5])
{
	int32_t i, j, wstride;
	float thold;
	int8_t *p_h, *p_0;
	uint8_t* p_l;

	wstride = ZXCORE_ALIGN_4(width);

	p_h = (int8_t*)fimg[hpass];
	p_0 = (int8_t*)fimg[0];
	p_l = (uint8_t*)fimg[lpass];

	for (j = 0; j < height; j++, p_h+=wstride, p_0+=wstride, p_l+=wstride)
	{
		for(i=0; i<width; i++)
		{
			if(p_h[i]<th && p_h[i]>-th)
				p_h[i] = (p_h[i]+4)>>4;  //p_h[i] *= low; //low = 0.125 (for 3) and low = 0.0625 (for 4)
			else
			{
				if (p_l[i] > 204) {
					thold = one_low_stdev[4];
				} else if (p_l[i] > 153) {
					thold = one_low_stdev[3];
				} else if (p_l[i] > 102) {
					thold = one_low_stdev[2];
				} else if (p_l[i] > 51) {
					thold = one_low_stdev[1];
				} else {
					thold = one_low_stdev[0];
				}
				
				if(p_h[i]>0)
					p_h[i] -= thold;
				else
					p_h[i] += thold;
				
			}
			
			if (hpass)	p_0[i] += p_h[i];
		}
		
	}
	
}

// p_src: p_h; p_dst: p_0
void do_synthesis(int8_t* p_src, int8_t* p_dst, int width, int height)
{
	int i,j,wstride;
	int8_t *src, *dst;
	src = p_src; dst = p_dst;
	wstride = ((width+3)>>2)<<2;
	for (i=0; i<height; i++, src+=wstride, dst+=wstride)
	{
		for(j=0; j<width; j++)
			dst[j] += src[j];
	}
}


void hat_transform_row(uint8_t *p_src, uint16_t *p_dst, int32_t width, int sc)
{
	int curr,left,right; 
	curr = 0; left = sc; right = sc; 
	for (; curr < sc; curr++,left--,right++)
		p_dst[curr] = (p_src[curr]*2) + p_src[left] + p_src[right];
	for (; curr + sc < width; curr++,left++,right++)
		p_dst[curr] = (p_src[curr]*2) + p_src[left] + p_src[right];
	right -= 2;
	for (; curr < width; curr++,left++,right--)
		p_dst[curr] = (p_src[curr]*2) + p_src[left] + p_src[right];
}

void hat_transform_col(uint16_t *current, uint16_t *top, uint16_t* bottom, 
					   uint8_t* output, int32_t width)
{
	int i, sum;
	for (i=0; i<width; i++)
	{
		sum = ((current[i]<<1) + top[i] +  bottom[i]);
		output[i] = (sum+8)>>4;
	}
}


unsigned int  EdgeStopFuncTab[256] = {
	1024,1024,512,341,256,205,171,146,128,114,102,93,85,79,73,68,
	64,60,57,54,51,49,47,45,43,41,39,38,37,35,34,33,
	32,31,30,29,28,28,27,26,26,25,24,24,23,23,22,22,
	21,21,20,20,20,19,19,19,18,18,18,17,17,17,17,16,
	16,16,16,15,15,15,15,14,14,14,14,14,13,13,13,13,
	13,13,12,12,12,12,12,12,12,12,11,11,11,11,11,11,
	11,11,10,10,10,10,10,10,10,10,10,10,9,9,9,9,
	9,9,9,9,9,9,9,9,9,8,8,8,8,8,8,8,
	8,8,8,8,8,8,8,8,8,7,7,7,7,7,7,7,
	7,7,7,7,7,7,7,7,7,7,7,7,7,7,6,6,
	6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
	6,6,6,6,6,6,6,6,6,6,6,5,5,5,5,5,
	5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
	5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
	5,5,5,5,4,4,4,4,4,4,4,4,4,4,4,4,
	4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4
};

//////////////////////////////////////////////////////////////////////////
// edge-avoiding hat transform
void hat_transform_row_ea(uint8_t *p_src, uint16_t *p_dst, int32_t width, int sc)
{
    int curr,left,right, leftDiff, rightDiff, sum, sumWeight, thrDiff; 
	int leftWeight, rightWeight, centerWeight;
	thrDiff = 16; centerWeight = 1024;
    curr = 0; left = sc; right = sc; 
    for (; curr < sc; curr++,left--,right++)
	{
		leftDiff  = p_src[curr]-p_src[left];
		rightDiff = p_src[right]-p_src[curr];
		leftWeight  = EdgeStopFuncTab[abs(leftDiff)];
		rightWeight = EdgeStopFuncTab[abs(rightDiff)];
		sumWeight = centerWeight + leftWeight + rightWeight; sum = (p_src[curr]*centerWeight);
		{sum += p_src[left] * leftWeight; }
		{sum += p_src[right]* rightWeight;}
		p_dst[curr] = (uint16_t)(sum / (float)sumWeight + 0.5);
	}
    for (; curr + sc < width; curr++,left++,right++)
	{
        leftDiff  = p_src[curr]-p_src[left];
		rightDiff = p_src[right]-p_src[curr];
		leftWeight  = EdgeStopFuncTab[abs(leftDiff)];
		rightWeight = EdgeStopFuncTab[abs(rightDiff)];
		sumWeight = centerWeight + leftWeight + rightWeight; sum = (p_src[curr]*centerWeight);
		{sum += p_src[left] * leftWeight; }
		{sum += p_src[right]* rightWeight;}
		p_dst[curr] = (uint16_t)(sum / (float)sumWeight + 0.5);
	}
    right -= 2;
    for (; curr < width; curr++,left++,right--)
	{
        leftDiff  = p_src[curr]-p_src[left];
		rightDiff = p_src[right]-p_src[curr];
		leftWeight  = EdgeStopFuncTab[abs(leftDiff)];
		rightWeight = EdgeStopFuncTab[abs(rightDiff)];
		sumWeight = centerWeight + leftWeight + rightWeight; sum = (p_src[curr]*centerWeight);
		{sum += p_src[left] * leftWeight; }
		{sum += p_src[right]* rightWeight;}
		p_dst[curr] = (uint16_t)(sum / (float)sumWeight + 0.5);
	}
}

void hat_transform_col_ea(uint8_t *p_src_cur, uint8_t *p_src_top, uint8_t *p_src_bottom,
						  uint16_t *current, uint16_t *top, uint16_t* bottom, 
						  uint8_t* output, int32_t width)
{
    int i, sum, topWeight, bottomWeight, sumWeight, centerWeight;
	centerWeight = 512;
    for (i=0; i<width; i++)
    {
		topWeight    = EdgeStopFuncTab[abs(p_src_cur[i]-p_src_top[i])];
		bottomWeight = EdgeStopFuncTab[abs(p_src_cur[i]-p_src_bottom[i])];
        sum = ((current[i]*centerWeight) + top[i] * topWeight +  bottom[i] * bottomWeight);
		sumWeight = centerWeight + topWeight + bottomWeight;
        output[i] = (uint8_t)(sum / (float)sumWeight + 0.5);
    }
}

//p_buf: temp memory with (2*sc+2)-width uint16 type.(sc=1,2,4,8,16,...) 
void hat_transform_ea(uint8_t* p_src, uint8_t* p_dst, uint16_t* p_buf, int width, int height, int sc)
{
	int i,j,wstride, current, top, bottom, rowH;
	unsigned short *rowbuf, *rows[33];
	unsigned char  *output, *input, *src_cur, *src_top, *src_bottom;
	
	wstride = (width+3)>>2<<2; 
	rowH    = sc * 2 + 1;
	//outbuf  = p_buf;         //store col sum of current row
	rowbuf  = p_buf; //store top row 
	// set pointers for fast accessing to row buffers.
	rows[0]= p_buf + width;
	for (i=1; i<=2*sc; i++) 
		rows[i] = rows[i-1] + width;
	
	input   = p_src;
	output  = p_dst;
	
	i = 0;
	
	//#0: do row sum from row #0 to row #(rowH-1)
	for (; i<=2*sc; i++, input+=wstride)
		hat_transform_row_ea(input, rows[i], width, sc);
	
	//#1: do col sum in 0 ~ (sc-1) row
	current = 0; top = sc; bottom = sc; 
	src_cur = p_src;	src_top = p_src + top * wstride;  src_bottom = p_src + bottom * wstride;
	for (j=0; j<sc; j++, current++, top--, bottom++, output+=wstride, 
		src_cur+=wstride, src_top-=wstride, src_bottom+=wstride)
	{
		hat_transform_col_ea(src_cur, src_top, src_bottom, rows[current], rows[top], rows[bottom], output, width);
	}
	
	
	//#2: do row sum from row #(2*sc) to row #(height-1), 
	//and do col sum from row #sc to row (height-1-sc) 
	for (; i<height; i++, input+=wstride, output+=wstride, 
		current++, top++, bottom++,
		src_cur+=wstride, src_top+=wstride, src_bottom+=wstride)
	{
		//calculate the bottom row sum into @rows[bottom],
		//and calculate the col sum @output 
		if(bottom>=rowH)  bottom -= rowH;
		if(current>=rowH) current -= rowH;
		if(top>=rowH)     top -= rowH;
		hat_transform_row_ea(input, rows[bottom], width, sc);  
		hat_transform_col_ea(src_cur, src_top, src_bottom, rows[current], rows[top], rows[bottom], output, width);
	}
	
	bottom = (bottom-2); if(bottom<0) bottom += rowH;
	src_bottom -= (2*wstride); 
	//#3: do col sum from row #(height-sc) to row #(height-1) 
	for (j=0; j<sc; j++, output+=wstride, current++, top++, bottom--,
		src_cur+=wstride, src_top+=wstride, src_bottom-=wstride)
	{
		if(bottom>=rowH)  bottom -= rowH;
		if(current>=rowH) current -= rowH;
		if(top>=rowH)     top -= rowH;
		hat_transform_col_ea(src_cur, src_top, src_bottom, rows[current], rows[top], rows[bottom], output, width);
	}
}


void hat_transform_col_ea1(uint16_t *current, uint16_t *top, uint16_t* bottom, 
                          uint8_t* output, int32_t width)
{
    int i, sum;
    for (i=0; i<width; i++)
    {
        sum = ((current[i]<<1) + top[i] +  bottom[i]);
        output[i] = (sum+4)>>2;
    }
}



//p_buf: temp memory with (2*sc+2)-width uint16 type.(sc=1,2,4,8,16,...) 
void hat_transform(uint8_t* p_src, uint8_t* p_dst, uint16_t* p_buf, int width, int height, int sc)
{
	int i,j,wstride, current, top, bottom, rowH;
	unsigned short *rowbuf, *rows[33];
	unsigned char  *output, *input;
	
	wstride = (width+3)>>2<<2; 
	rowH    = sc * 2 + 1;
	//outbuf  = p_buf;         //store col sum of current row
	rowbuf  = p_buf; //store top row 
	// set pointers for fast accessing to row buffers.
	rows[0]= p_buf + width;
	for (i=1; i<=2*sc; i++) 
		rows[i] = rows[i-1] + width;
	
	input   = p_src;
	output  = p_dst;
	
	i = 0;
	
	//#0: do row sum from row #0 to row #(rowH-1)
	for (; i<=2*sc; i++, input+=wstride)
		hat_transform_row(input, rows[i], width, sc);

	//#1: do col sum in 0 ~ (sc-1) row
	current = 0; top = sc; bottom = sc;
	for (j=0; j<sc; j++, current++, top--, bottom++, output+=wstride)
		hat_transform_col(rows[current], rows[top], rows[bottom], output, width);


	//#2: do row sum from row #(2*sc) to row #(height-1), 
	//and do col sum from row #sc to row (height-1-sc) 
	for (; i<height; i++, input+=wstride, output+=wstride, 
		current++, top++, bottom++)
	{
		//calculate the bottom row sum into @rows[bottom],
		//and calculate the col sum @output 
		if(bottom>=rowH)  bottom -= rowH;
		if(current>=rowH) current -= rowH;
		if(top>=rowH)     top -= rowH;
		hat_transform_row(input, rows[bottom], width, sc);  
		hat_transform_col(rows[current], rows[top], rows[bottom], output, width);
	}

	bottom = (bottom-2); if(bottom<0) bottom += rowH;
	//#3: do col sum from row #(height-sc) to row #(height-1) 
	for (j=0; j<sc; j++, output+=wstride, current++, top++, bottom--)
	{
		if(bottom>=rowH)  bottom -= rowH;
		if(current>=rowH) current -= rowH;
		if(top>=rowH)     top -= rowH;
		hat_transform_col(rows[current], rows[top], rows[bottom], output, width);
	}
}



#define  _USE_HIST_STDEV_
// calculate standard deviation  
void calc_stdev(int8_t* p_h, uint8_t* p_l, int width, int height, uint8_t thold, float stdev[5])
{
	int i,j,colstep,rowstep, tmp, wstride;
	
#ifdef _USE_HIST_STDEV_
	uint16_t hist[5][16], sq_h[16]; 
	float w, wsum;
#else
	uint16_t samples[5];
#endif

	wstride = ((width+3)>>2)<<2;

	colstep = 8; rowstep = colstep * wstride; //sampling for speed

#ifdef _USE_HIST_STDEV_
	memset(hist, 0, 160);
	for(i=0; i<16; i++)
		sq_h[i] = i * i;

	for (i = 0; i < height; i+=colstep, p_h+=rowstep, p_l+=rowstep)
	{
		for(j=0; j<width; j+=colstep)
		{
			tmp = ZXCORE_ABS(p_h[j]);

			if(tmp >= thold) 
				continue;
			
			if (p_l[j] > 204) {
				hist[4][tmp]++;
			} else if (p_l[j] > 153) {
				hist[3][tmp]++;
			}	else if (p_l[j] > 102) {
				hist[2][tmp]++;
			}	else if (p_l[j] > 51) {
				hist[1][tmp]++;
			} else {
				hist[0][tmp]++;
			}
		}
	}

	for (i=0; i<5; i++)
	{
//		write_array_to_file_u16("hist.dat", hist[i], 1, 16);
		w = 1; wsum = 0;
		for(j=0; j<16; j++)
		{
			w += hist[i][j];
			wsum += (sq_h[j]*hist[i][j]);
		}
		stdev[i] = (float)sqrt(wsum / w);
	}

//	write_array_to_file_s8("h0.dat", p_h, width, height);
//	write_array_to_file_u8("l0.dat", p_l, width, height);

#else
	stdev[0]   = stdev[1] = stdev[2] = stdev[3] = stdev[4] = 0.0;
    samples[0] = samples[1] = samples[2] = samples[3] = samples[4] = 0;

	for (i = 0; i < height; i+=colstep, p_h+=rowstep, p_l+=rowstep)
	{
		for(j=0; j<width; j+=colstep)
		{
			tmp = p_h[j] * p_h[j];
			if (p_h[j] < thold && p_h[j] > -thold)
			{
				if (p_l[j] > 204) {
					stdev[4] += tmp;
					samples[4]++;
				} else if (p_l[j] > 153) {
					stdev[3] += tmp;
					samples[3]++;
				}	else if (p_l[j] > 102) {
					stdev[2] += tmp;
					samples[2]++;
				}	else if (p_l[j] > 51) {
					stdev[1] += tmp;
					samples[1]++;
				} else {
					stdev[0] += tmp;
					samples[0]++;
				}
			}
		}
	}
	stdev[0] = sqrt (stdev[0] / (samples[0] + 1));
	stdev[1] = sqrt (stdev[1] / (samples[1] + 1));
	stdev[2] = sqrt (stdev[2] / (samples[2] + 1));
	stdev[3] = sqrt (stdev[3] / (samples[3] + 1));
    stdev[4] = sqrt (stdev[4] / (samples[4] + 1));
#endif
}


// actual denoising & smoothing algorithm. 
void FilterWaveletC1(int Handle, uint8_t* p_src, uint8_t* p_dst, int offset, int cn, int whScale)
{
	
	WaveletFilter *pFilter;
	unsigned int  i, lev, lpass, hpass;
	int8_t  *p_h, *p_0;
	uint8_t *fimg[3], *p_l, *src, *dst;
	uint16_t *p_buf;
	uint32_t width, height, size;

	pFilter = (WaveletFilter*)Handle; 
	if(!pFilter) return;

	width  = pFilter->width  / whScale;
	height = pFilter->height / whScale;
	size   = width * height;
	
	fimg[0] = (uint8_t*)pFilter->buf;
	fimg[1] = (uint8_t*)(pFilter->buf+size);
	fimg[2] = (uint8_t*)(pFilter->buf+2*size);
	p_buf   = (uint16_t*)(pFilter->buf+3*size);

	// get channel pixels
	src = p_src + offset;
	dst = p_dst + offset;
	
	
	// get source data and copy to memory.
	for (i=0; i<size; i++)
		fimg[0][i] = src[i*cn];
	
	hpass = 0;
	for (lev = 0; lev < 3; lev++)
	{
		
		lpass = ((lev & 1) + 1);
		
		// LOGE("lpass:%d,hpass:%d\n",lpass, hpass);
		
		// do hat transform
		// input: hpass--uchar  lpass--uchar
		// output:hpass--uchar  lpass--uchar
		pFilter->func_hat_transform(fimg[hpass], fimg[lpass], p_buf, width, height, 1<<lev);
		
		// calculate stdevs for all intensities 
		// input:  hpass--uchar lpass--uchar
		// output: hpass--char  lpass--uchar
		p_h = (int8_t*)fimg[hpass];
		p_l = (uint8_t*)fimg[lpass];
		for (i=0; i<size; i++) 
			p_h[i] = fimg[hpass][i] - fimg[lpass][i];
		
		pFilter->func_calc_stdev(p_h, p_l, width, height, pFilter->tholds[lev], pFilter->stdev);
		
		for(i=0; i<5; i++) 
			pFilter->stdev[i] *= pFilter->one_low_thr;
		
		/* do thresholding */
		// input:  hpass--char  lpass--uchar
		// output: hpass--char  lpass--uchar
		pFilter->func_calc_threshold(fimg, width, height, lpass, hpass, pFilter->tholds[lev], pFilter->stdev);
		
		hpass = lpass;
		
	}

	// set the results
	for (i=0; i<size; i++)
		dst[i*cn] = fimg[0][i] + fimg[lpass][i];
	
}

