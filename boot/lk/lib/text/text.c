/*
 * Copyright (c) 2008-2010 Travis Geiselbrecht
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/**
 * @addtogroup graphics
 * @{
 */

/**
 * @file
 * @brief  Console text display
 *
 * This module displays text on the console.  The text is retained so that
 * it can be redisplayed if the display is cleared.
 *
 * Output is to the default graphics display
 */

#include <debug.h>
#include <list.h>
#include <stdlib.h>
#include <string.h>
#include <dev/display.h>
#include <lib/gfx.h>
#include <lib/font.h>
#include <lib/text.h>

#include "target_config.h"
#include "mcde.h"
#include "mcde_display.h"

#define TEXT_COLOR 0xffffffff
#define BLACK      0x0


extern struct mcde_chnl_state *chnl;

#define RGB565_BYTES_PER_PIX   2
#define CHAR_PER_LINE         46
#define MAX_TEXT_Y           410


static struct list_node text_list = LIST_INITIAL_VALUE(text_list);
void display_get_info(struct display_info *info);

struct text_line {
	struct list_node node;
	const char *str;
	int x, y;
};

/**
 * @brief  Add a string to the console text
 */
void text_draw(int x, int y, const char *string)
{
	static int text_y=0;
	struct text_line *line = malloc(sizeof(struct text_line));

	line->str = strdup(string);
	line->x = x;
	/* Ignore y position, new line below old */
	if (y < text_y)
		y = text_y;
	line->y = y;

	list_add_head(&text_list, &line->node);

	text_update();

	/* Wrap around if reaching end of screen */
	text_y += FONT_Y;
	if(text_y>MAX_TEXT_Y) {
		/* Empty strings list*/
		while (list_remove_head(&text_list))
			;
		/* Wrap y position */
		text_y=0;
	}
}

/**
 * @brief  Refresh the display
 */
void text_update(void)
{
	gfx_surface *surface;
	struct mcde_rectangle	rect;

	if (IS_ERR(chnl)) {
		dprintf (CRITICAL, "mcde_chnl_get failed %d\n", (int)chnl);
		return;
	}

	/* get a surface covering the whole display */
	surface = gfx_create_surface((void *)chnl->ovly0->paddr, chnl->vmode.xres,
		chnl->vmode.yres, chnl->ovly0->stride/RGB565_BYTES_PER_PIX, GFX_FORMAT_RGB_565);

	/* Update the overlay property to cover the whole display */
	mcde_ovly_set_source_area(chnl->ovly0, 0, 0, chnl->vmode.xres, chnl->vmode.yres);
	mcde_ovly_set_dest_pos(chnl->ovly0, 1, 1, 1);
	mcde_ovly_apply(chnl->ovly0);

	dprintf (SPEW, "X %d, Y%d, S %d. ovly %d * %d\n", chnl->vmode.xres, chnl->vmode.yres, chnl->ovly0->stride, chnl->ovly0->h, chnl->ovly0->w);

	/* Always clear the surface */
	memset( (void *)chnl->ovly0->paddr, 0xFF, \
		chnl->vmode.xres*chnl->vmode.yres*RGB565_BYTES_PER_PIX);

	/* print the text in the surface */
	struct text_line *line;
	list_for_every_entry(&text_list, line, struct text_line, node) {
		const char *c;
		int x = line->x;
		/* Truncate x position at end of screen */
		for (c = line->str; *c && (x < (int)chnl->vmode.xres); c++) {
			font_draw_char(surface, *c, x, line->y, BLACK);
			x += FONT_X;
		}
	}

	rect.x = 0;
	rect.y = 0;
	rect.w = chnl->vmode.xres;
	rect.h = chnl->vmode.yres;

	mcde_chnl_update(chnl, &rect, false);
	mdelay(20);
	mcde_chnl_update(chnl, &rect, false);

	gfx_flush(surface);

	gfx_surface_destroy(surface);
}

#if WITH_LIB_CONSOLE

#include <lib/console.h>

static int cmd_print(int argc, const cmd_args *argv);

STATIC_COMMAND_START
	{ "print", "print text on the display", &cmd_print }
STATIC_COMMAND_END(cmd_print);


/**
 * @brief  Display some text on the display
 */
static int cmd_print(int argc, const cmd_args *argv){
	char text[CHAR_PER_LINE];
	char *text_ptr=text;
	int text_length=0;
	int tmp_length=0;

	//struct mcde_chnl_state *chnl;

	if (argc < 2) {
		printf("not enough arguments\n");
		printf("%s <text>\n", argv[0].str);
		return -1;
	}

	/* put all words given as parameter in same text buffer */
	for( int i=1; i<argc; i++ ){
		tmp_length=strlen(argv[i].str);
		if( (text_length+tmp_length) < CHAR_PER_LINE ){
			strcpy(text_ptr, argv[i].str);
			text_length+=tmp_length;
			text_ptr+=tmp_length;
			*text_ptr=' ';
		}
		text_length++;
		text_ptr++;
	}
	*text_ptr=0;

	text_draw (0, 0, text);
	return 0;
	//return text_print_lcd (text);
}

#endif //#if WITH_LIB_CONSOLE

