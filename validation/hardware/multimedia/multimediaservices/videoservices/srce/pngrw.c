/*****************************************************************************/
/**
*  Copyright (C) ST-Ericsson 2009
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   Propose png read and write functions
* \author  ST-Ericsson
*/
/*****************************************************************************/

#include <stdint.h>			/* uint32_t ... */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/* 
 * TODO (read) add transparency management
 *
 * */


/* PNG debug level
 * 3 for max level (many information)
 * 0 for no debug information */
#define PNG_DEBUG 3
#include <png.h>



/* 
 * write_png - write a RGB 24bpp png image to a file
 *
 * @output_filename
 * @input: image raw data
 * @input_width: raw image width
 * @input_height: raw image height
 * @input_bpp: only 16, 24 and 32bpp are supported
 * @return: 0 if ok
 *
 * */
int32_t write_png(char *output_filename, 
		uint8_t *input, 
		uint32_t input_width, uint32_t input_height, 
		uint32_t input_bpp)
{
	FILE *fp;
	png_structp png_ptr;
	png_infop info_ptr;
	png_bytep *row_pointers;
	volatile png_bytep row=0;
	uint32_t row_ofs, ofs, x, y;
	uint8_t r, g, b;
	uint8_t  *p8;
	uint16_t *p16;
	uint32_t   *p32;

	/* image parameters */
	int32_t bit_depth = 8; /* 8 bit per color channel */
	int32_t color_type = PNG_COLOR_TYPE_RGB; /* 24bpp */

	/* create a new file */
	if ((fp = fopen(output_filename, "wb")) == NULL) {
		printf("Could not open the file %s for writing the png image!\n", 
				output_filename);
		return (-1);
	}

	/* initialize png struct */
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, 
			NULL, NULL, NULL);
	if (png_ptr == NULL) {
		printf("Error during png structure init\n");
		goto out_close;
	}

	/* Allocate/initialize the image information data. */
	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL) {
		printf("Error during png image information data creation\n");
		goto out_struct;
	}

	/* Set error handling */
	if (setjmp(png_jmpbuf(png_ptr))) {
		printf("Error during setting the error handling function\n");
		goto out_struct;
	}

	/* Set up the output control if you are using standard C streams */
	png_init_io(png_ptr, fp);

	/* Write IHDR chunk*/
	png_set_IHDR(png_ptr, info_ptr, input_width, input_height,
		     bit_depth, 
			 color_type, 
			 PNG_INTERLACE_NONE,
		     PNG_COMPRESSION_TYPE_BASE, 
			 PNG_FILTER_TYPE_BASE);
	png_write_info(png_ptr, info_ptr);
	if (setjmp(png_jmpbuf(png_ptr))) {
		printf("Error during png info write\n");
		goto out_struct;
	}

	/* Allocate row mem */
	row_pointers = (png_bytep *)malloc(sizeof(png_bytep) * input_height);
	if (row_pointers == NULL) {
		printf("Error during row memory allocation\n");
		goto out_free;
	}
	for(y = 0; y < input_height; y++) {
		row_pointers[y] = (png_bytep)malloc(png_get_rowbytes(png_ptr, 
					info_ptr));
		if (row_pointers[y] == NULL) {
			printf("Error during row memory allocation\n");
			goto out_free_table;
		}
	}

	/* Write data row by row after color conversion */
	p8  = (uint8_t *)(input);
	p16 = (uint16_t *)(input);
	p32 = (uint32_t *)(input);
	ofs = 0;
	for (y = 0; y < input_height; y++) {
		row_ofs = 0;
		row = row_pointers[y];
		for (x = 0; x < input_width; x++) {
			if (input_bpp == 16) {
				r = (uint8_t)((uint16_t)((p16[ofs] >> 11) << 3));
				g = (uint8_t)((uint16_t)((p16[ofs] >> 5) << 2));
				b = (uint8_t)((uint16_t)((p16[ofs] & 0x1F) << 3));
			} else if (input_bpp == 24) {
				r = p8[(ofs * 3)];
				g = p8[(ofs * 3) + 1];
				b = p8[(ofs * 3) + 2];
			} else if (input_bpp == 32) {
				r = (uint8_t)((uint32_t)((p32[ofs] >> 16) & 0xFF));
				g = (uint8_t)((uint32_t)((p32[ofs] >> 8) & 0xFF));
				b = (uint8_t)((uint32_t)(p32[ofs] & 0xFF));
			} else {
				printf("Bad input color format\n");
				goto out_free_table;
			}
			ofs++;
			row[row_ofs++] = r;
			row[row_ofs++] = g;
			row[row_ofs++] = b;
		}
	}

	png_write_image(png_ptr, row_pointers);

	/* Write png file end */
	png_write_end(png_ptr, info_ptr);
	if (setjmp(png_jmpbuf(png_ptr))) {
		printf("Error during png end write\n");
		goto out_free_table;
	}

	goto everything_is_ok;


	/* Error managements */
out_free_table:
	/* Free row mem table */
	for(y = 0; y < input_height; y++) 
		free(row_pointers[y]);
out_free:
	free(row_pointers);
out_struct:
	/* clean up */
	png_destroy_write_struct(&png_ptr, &info_ptr);
out_close:
	/* close the file */ 
	fclose(fp);
	/* go out with an error value */
	return (-1);


everything_is_ok:
	/* clean up and close the file */
	free(row);
	png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(fp);
	/* Everything is ok */
	return 0;
}



#define RGB888TO565(r, g, b) ((uint16_t)(((r>>3)<<11) + ((g>>2)<<5) + (b>>3))) 
#define RGB888TO0888(r, g, b) ((uint32_t)((r<<16) + (g<<8) + (b))) 



/*
 * read_png - read a 24bpp png file to an output buffer. PNG image is cropped
 *            if the size is bigger than the output buffer.
 * 
 * @input_filename:
 * @output: may contain a background image (in case of alpha or transparency)
 * @output_width:
 * @output_height:
 * @output_bpp: only 16, 24 and 32bpp are supported
 * @return: 0 if ok.
 *
 * */
int32_t read_png(char *input_filename,  
		uint8_t *output, 
		uint32_t output_width, uint32_t output_height, 
		uint32_t output_bpp)
{
	FILE *fp;
	int8_t header[8];	/* 8 is the maximum size that can be checked */
	png_structp png_ptr;
	png_infop info_ptr;
	png_bytep *row_pointers;
	png_bytep row;
	int32_t number_passes;
	uint32_t x, y, row_ofs, ofs; 
	volatile uint32_t width, height;
	uint8_t  *p8;
	uint16_t *p16;
	uint32_t   *p32;

	/* open file and test for it being a png */
	if ((fp = fopen(input_filename, "rb")) == NULL) {
		printf("Could not open the file %s for reading the png image\n", 
				input_filename);
		return (-1);
	}
		
	/* Check if the file is a PNG */
	fread(header, 1, 8, fp);
	if (png_sig_cmp((png_bytep)header, 0, 8)) {
		printf("The file %s is not recognized as a PNG file\n", 
				input_filename);
        fclose(fp);
		return (-1);
	}

	/* Initialize png struct */
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 
			NULL, NULL, NULL);
	if (png_ptr == NULL) {
		printf("Error during png structure init\n");
		goto out_close;
	}

	/* Allocate/initialize the image information data. */
	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL) {
		printf("Error during png image information data creation\n");
		goto out_struct;
	}

	/* Set error handling */
	if (setjmp(png_jmpbuf(png_ptr))) {
		printf("Error during setting the error handling function\n");
		goto out_struct;
	}

	/* Init standard read and read the ONG signature allready checked */
	png_init_io(png_ptr, fp);
	png_set_sig_bytes(png_ptr, 8);

	/* Read the info chunks */
	png_read_info(png_ptr, info_ptr);
	
	/* Check color type*/
	if ((info_ptr->color_type != PNG_COLOR_TYPE_RGB) && 
		(info_ptr->color_type != PNG_COLOR_TYPE_RGB_ALPHA) &&
		(info_ptr->color_type != PNG_COLOR_TYPE_PALETTE)) {
		printf("Color format not supported\n");
		goto out_struct;
	}

	/* Check xy resolution */
	if (info_ptr->width > output_width) {
		printf("Warning: PNG width (%d) is bigger than output width (%d) "
				"so the PNG will be cropped.\n", 
				(int)(info_ptr->width), output_width);
		width = output_width;
	} else if (info_ptr->width < output_width) {
		printf("Warning: PNG width (%d) is smaller than output width (%d) "
				"so the output will not be totally filled.\n", 
				(int)(info_ptr->width), output_width);
		width = info_ptr->width;
	} else
		width = output_width;

	if (info_ptr->height > output_height) {
		printf("Warning: PNG height (%d) is bigger than output height (%d) "
				"so the PNG will be cropped.\n", 
				(int)(info_ptr->height), output_height);
		height = output_height;
	} else if (info_ptr->height < output_height) {
		printf("Warning: PNG height (%d) is smaller than output height (%d)"
				"so the output will not be totally filled.\n", 
				(int)(info_ptr->height), output_height);
		height = info_ptr->height;
	} else
		height = output_height;

	/* Extract multiple pixels with bit depths of 1, 2, and 4 from a single
	 * byte into separate bytes (useful for paletted and grayscale images). */
	png_set_packing(png_ptr);

	/* Expand paletted colors into true RGB triplets */
	if (info_ptr->color_type == PNG_COLOR_TYPE_PALETTE) {
		png_set_palette_to_rgb(png_ptr);
	}

	/* Expand paletted or RGB images with transparency to full alpha channels
     * so the data will be available as RGBA quartets. */
	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) {
		png_set_tRNS_to_alpha(png_ptr);
	} else {
		/* Add filler (or alpha) byte (before/after each RGB triplet) */
		png_set_filler(png_ptr, 0xff, PNG_FILLER_AFTER);
	}

	/* Turn on interlace handling.  REQUIRED if you are not using
	 * png_read_image().  To see how to handle interlacing passes,
	 * see the png_read_row() method below: */
	number_passes = png_set_interlace_handling(png_ptr);

	/* Optional call to gamma correct and add the background to the palette
	 * and update info structure.  REQUIRED if you are expecting libpng to
	 * update the palette for you (ie you selected such a transform above). */
	png_read_update_info(png_ptr, info_ptr);
	if (setjmp(png_jmpbuf(png_ptr))) {
		printf("Error during PNG update info read\n"); 
		goto out_struct;
	}

	/* Allocate row mem */
	row_pointers = (png_bytep *)malloc(sizeof(png_bytep) * info_ptr->height);
	if (row_pointers == NULL) {
		printf("Error during row memory allocation\n");
		goto out_free;
	}

	for(y = 0; y < info_ptr->height; y++) {
		row_pointers[y] = (png_bytep)malloc(png_get_rowbytes(png_ptr, 
					info_ptr));
		if (row_pointers[y] == NULL) {
			printf("Error during row memory allocation\n");
			goto out_free_table;
		}
	}

	/* read the png file */
	png_read_image(png_ptr, row_pointers);
	if (setjmp(png_jmpbuf(png_ptr))) {
		printf("Error during PNG image read\n");
		goto out_free_table;
	}

	/* fill the output */
	p8 = (uint8_t *)(output);
	p16 = (uint16_t *)(output);
	p32 = (uint32_t *)(output);
	for (y = 0; y < height; y++) {	
		row = row_pointers[y];
		ofs = y * output_width;
		row_ofs = 0;
		for (x = 0; x < width; x++) {
			if (output_bpp == 16) {
				p16[ofs++] = RGB888TO565(row[row_ofs], 
						row[row_ofs + 1], row[row_ofs + 2]);
			} else if (output_bpp == 24) {
				p8[ofs * 3] = row[row_ofs];
				p8[ofs * 3 + 1] = row[row_ofs + 1];
				p8[ofs * 3 + 2] = row[row_ofs + 2];
				ofs++;
			} else if (output_bpp == 32) {
				p32[ofs++] = RGB888TO0888(row[row_ofs], 
						row[row_ofs + 1], row[row_ofs + 2]);
			} else {
				printf("Bad output format\n");
				goto out_free_table;
			}
			row_ofs += 4;
		}
	}
	
	/* PNG end */
	png_read_end(png_ptr, info_ptr);
	if (setjmp(png_jmpbuf(png_ptr))) {
		printf("Error during png end write\n");
		goto out_free_table;
	}

	goto everything_is_ok;


	/* Error managements */
out_free_table:
	/* Free row mem table */
	for(y = 0; y < info_ptr->height; y++) 
		free(row_pointers[y]);
out_free:
	free(row_pointers);
out_struct:
	/* clean up */
	png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);
out_close:
	/* close the file */ 
	fclose(fp);
	/* go out with an error value */
	return (-1);


everything_is_ok:
	/* clean up all and close the file */
	for(y = 0; y < info_ptr->height; y++) 
		free(row_pointers[y]);
	free(row_pointers);
	png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);
    fclose(fp);
	/* Everything is ok */
	return 0;
}




