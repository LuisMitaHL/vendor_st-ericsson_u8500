/*
 * Copyright (c) 2008 Travis Geiselbrecht
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
#include <debug.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <lib/console.h>
#include "target_config.h"

static cmd_block *command_list = NULL;

/* a linear array of statically defined command blocks,
   defined in the linker script.
 */
extern cmd_block __commands_start;
extern cmd_block __commands_end;

static int cmd_help(int argc, const cmd_args *argv);
static int cmd_test(int argc, const cmd_args *argv);
static int cmd_hist(int argc, const cmd_args *argv);
static int last_cmd(int argc, const cmd_args *argv);

int console_run_string_of_cmds(const char *string);
/* for the linked list of command history */
typedef struct cmdHist{
    char* cmd;
    struct cmdHist* next;
    struct cmdHist* prev;
} cmdNode;

static cmdNode* firstCmd = NULL;
static cmdNode* lastCmd = NULL;
static cmdNode* currCmd = NULL;

STATIC_COMMAND_START
    { "help", "this list with or without option\nhelp < [pattern] ln\n[pattern] = pattern searched\nln = Line Number\n", &cmd_help },
    { "test", "test the command processor", &cmd_test },
    { "hist", "Show the command history", &cmd_hist },
    { "?", "alias for help", &cmd_help },
    { "!", "alias for hist", &cmd_hist },
    { "!!", "last command", &last_cmd },
STATIC_COMMAND_END(help);

#if defined(LOADER_WO_HBTS)
int HBTS_platform_init(void)
{
  return 0;
}
#else
extern int HBTS_platform_init(void);
#endif

int console_init(void)
{

#if defined(ENABLE_FEATURE_BUILD_HBTS)
    HBTS_platform_init();
#endif

    printf("console_init: entry\n");

    /* add all the statically defined commands to the list */
    cmd_block *block;
    for (block = &__commands_start; block != &__commands_end; block++) {
        console_register_commands(block);
    }

    return 0;
}

static const cmd *match_command(const char *command)
{
    cmd_block *block;
    size_t i;

    for (block = command_list; block != NULL; block = block->next) {
        const cmd *curr_cmd = block->list;
        for (i = 0; i < block->count; i++) {
            if (strcmp(command, curr_cmd[i].cmd_str) == 0) {
                return &curr_cmd[i];
            }
        }
    }

    return NULL;
}

static int read_line(char *buffer, unsigned int len)
{
    unsigned int pos = 0;
    unsigned int i = 0;
    unsigned int cnt = 0;
    int escape_level = 0;
    cmd_block *block;
    size_t j;
    char *tmpBuf;

    for (i = 0; i < len; i++) {
        buffer[i] = 0;
    }
    for (;;) {
        char c;

        /* loop until we get a char */
        if (getc(&c) < 0)
            continue;

        //printf("c = 0x%hhx\n", c);

        if (escape_level == 0) {
            switch (c) {
                case '\r':
                case '\n':
                    putc(c);
                    goto done;

                case 0x7f: // backspace or delete
                case 0x8:
                    if ((pos > 0) && pos == strlen(buffer)) {
                        pos--;
                        buffer[pos] = 0;
                        puts("\x1b[1D"); // move to the left one
                        putc(' ');
                        puts("\x1b[1D"); // move to the left one
                    }
                    else if ((pos > 0) && pos < strlen(buffer)) {
                        pos--;
                        puts("\x1b[1D"); // move to the left one
                        for (i = pos; i < strlen(buffer); i++) {
                            buffer[i] = buffer[i + 1];
                            putc(buffer[i]);
                        }
                        putc(' ');
                        puts("\x1b[1D"); // move to the left one
                        for (i = strlen(buffer); i > pos; i--) {
                        puts("\x1b[1D"); // move to the left one
                        }
                    }
                    break;

                case 0x1b: // escape
                    escape_level++;
                    break;

                case '\t': //auto completion using TAB
                    /* do not autocomplete when no char entered
                       or cursor not at end of line */
                    if ((pos == 0) || pos < strlen(buffer))
                        break;

                    for (block = command_list; block != NULL;
                                         block = block->next) {
                        const cmd *curr_cmd = block->list;
                        for (j = 0; j < block->count; j++) {
                            if (strncmp(buffer, curr_cmd[j].cmd_str,
                                               strlen(buffer)) == 0) {
                                cnt++;
                                if (cnt == 1)
                                    tmpBuf = (char*)curr_cmd[j].cmd_str;
                                else if (cnt > 1) {
                                    printf("\n%s", tmpBuf);
                                    tmpBuf = (char*)curr_cmd[j].cmd_str;
                                }
                            }
                        }
                    }
                    if (cnt == 1) {
                        pos = 0;
                        printf("\r                                                                 ");
                        printf("\r%s%s", CONFIG_SYS_PROMPT, tmpBuf);
                        strcpy(buffer, tmpBuf);
                        pos = strlen(buffer);
                    } else if (cnt > 1) {
                        printf("\n%s", tmpBuf);
                        printf("\n%s%s", CONFIG_SYS_PROMPT, buffer);
                    }
                    cnt = 0;
                    break;
                default:
                    if (pos == strlen(buffer)) {
                        buffer[pos++] = c;
                        putc(c);
                    }
                    else if (pos < strlen(buffer)) {
                        for (i = strlen(buffer); i > pos; i--) {
                            buffer[i] = buffer[i - 1];
                        }
                        putc(c);
                        buffer[pos++] = c;
                        for (i = pos; i < strlen(buffer); i++) {
                            putc(buffer[i]);
                        }
                        for (i = strlen(buffer); i > pos; i--) {
                            puts("\x1b[1D"); // move to the left one
                        }
                    }
            }
        } else if (escape_level == 1) {
            // inside an escape, look for '['
            if (c == '[') {
                escape_level++;
            } else {
                // we didn't get it, abort
                escape_level = 0;
            }
        } else { // escape_level > 1
            switch (c) {
                case 67: // right arrow
                    if (pos < strlen(buffer)) {
                        pos++;
                        puts("\x1b[1C"); // move to the right one
                    };
                    break;
                case 68: // left arrow
                    if (pos > 0) {
                        pos--;
                        puts("\x1b[1D"); // move to the left one
                    }
                    break;
                case 65: // up arrow
                    if (firstCmd != NULL && currCmd != firstCmd)
                    {
                        if (currCmd == NULL)
                            currCmd = lastCmd;
                        else
                            currCmd = currCmd->prev;
                        if (currCmd != NULL)
                        {
                            pos=0;
                            printf("\r                                                                 ");
                            printf("\r%s%s",CONFIG_SYS_PROMPT,currCmd->cmd);
                            strcpy(buffer,currCmd->cmd);
                            pos = strlen(buffer);
                        }
                    }
                    break;
                case 66: // down arrow
                    if (firstCmd != NULL && currCmd != NULL)
                    {
                        currCmd = currCmd->next;
                        if (currCmd != NULL)
                        {
                            pos=0;
                            printf("\r                                                                 ");
                            printf("\r%s%s",CONFIG_SYS_PROMPT,currCmd->cmd);
                            strcpy(buffer,currCmd->cmd);
                            pos = strlen(buffer);
                        }
                        else
                        {
                            pos=0;
                            printf("\r                                                                 ");
                            printf("\r%s",CONFIG_SYS_PROMPT);
                            for (i = 0; i < len; i++) {
                                buffer[i] = 0;
                            }
                        }
                    }
                    break;
                default:
                    break;
            }
            escape_level = 0;
        }

        /* end of line. */
        if (pos == (len - 1) || strlen(buffer) == (len - 1)) {
            puts("\nerror: line too long\n");
            goto done;
        }
    }

done:
//  printf("returning pos %d\n", pos);

    if (pos == (len - 1) || strlen(buffer) == (len - 1))
        pos = 0;
    else if (pos < strlen(buffer))
        pos = strlen(buffer);
    buffer[pos] = 0;
    return pos;
}

static int tokenize_command(char *buffer, cmd_args *args, int arg_count)
{
    int pos;
    int arg;
    bool finished;
    bool token_found;
    enum {
        INITIAL = 0,
        IN_SPACE,
        IN_TOKEN
    } state;

    pos = 0;
    arg = 0;
    state = INITIAL;
    finished = false;
    token_found = false;

    for (;;) {
        char c = buffer[pos];

        if (c == '\0')
            finished = true;

        //printf("c 0x%hhx state %d arg %d pos %d\n", c, state, arg, pos);

        switch (state) {
            case INITIAL:
                if (isspace(c)) {
                    state = IN_SPACE;
                } else {
                    state = IN_TOKEN;
                    args[arg].str = &buffer[pos];
                }
                break;
            case IN_TOKEN:
                token_found = true;
                if (finished) {
                    arg++;
                    goto done;
                }
                if (isspace(c)) {
                    arg++;
                    buffer[pos] = 0;
                    /* are we out of tokens? */
                    if (arg == arg_count)
                        goto done;
                    state = IN_SPACE;
                }
                pos++;
                break;
            case IN_SPACE:
                if (finished)
                    goto done;
                if (!isspace(c)) {
                    state = IN_TOKEN;
                    args[arg].str = &buffer[pos];
                }
                pos++;
                break;
        }
    }

done:
    if (!token_found)
        arg = -1;
    return arg;
}

static void convert_args(int argc, cmd_args *argv)
{
    int i;

    for (i = 0; i < argc; i++) {
        argv[i].u = atoui(argv[i].str);
        argv[i].i = atoi(argv[i].str);
    }
}

static void console_loop(void)
{
    cmdNode *tmpCmd = NULL;
    char buffer[256] = "\0";

    printf("entering main console loop\n");

    for (;;) {
        printf("%s", CONFIG_SYS_PROMPT);

        int len = read_line(buffer, sizeof(buffer));
        if (len == 0) {
            putc ('\n');
            continue;
        }

        tmpCmd = (cmdNode*)malloc(sizeof(cmdNode));
        if (NULL == tmpCmd) {
            printf ("%s (%d): calloc failed \n", __func__, __LINE__);
            continue;
        }

        tmpCmd->cmd = calloc(strlen(buffer)+1,1);
        if (NULL == tmpCmd->cmd) {
            printf ("%s (%d): calloc failed \n", __func__, __LINE__);
            free (tmpCmd);
            continue;
        }

        strcpy(tmpCmd->cmd,buffer);
        tmpCmd->next = NULL;
        tmpCmd->prev = NULL;

        if (firstCmd == NULL) {
            firstCmd = tmpCmd;
        } else if (lastCmd != NULL) {
            lastCmd->next = tmpCmd;
            tmpCmd->prev = lastCmd;
        }
        currCmd = tmpCmd;
        lastCmd = currCmd;
        tmpCmd = NULL;

        console_run_string_of_cmds(buffer);
        currCmd = NULL;

    }
}


void console_start(void)
{

    console_loop();
}

int console_run_string_of_cmds(const char *string)
{
    char *strOfCmd;
    char *endOfCmd;

    strOfCmd = (char*)string;
    endOfCmd = strpbrk(strOfCmd,";'");
    while (endOfCmd != NULL)
    {
        if (*endOfCmd == ';')
        {
            *endOfCmd = '\0';
            console_run_command(strOfCmd);
            strOfCmd = endOfCmd+1;
            endOfCmd = strpbrk(strOfCmd,";'");
        }
        else
        {
            endOfCmd = strchr(endOfCmd+1,'\'');
            if (endOfCmd != NULL)
                endOfCmd = strpbrk(endOfCmd+1,";'");
            else
            {
                printf("Strange format of command\n");
                return -1;
            }
        }
    }
    return console_run_command(strOfCmd);
}

int console_run_command(const char *string)
{
    cmd_args args[16];

    printf("%s%s\n",CONFIG_SYS_PROMPT, string);
    int argc = tokenize_command((char*)string, args, 16);
    if (argc < 0)
    {
        printf("syntax error\n");
        return -1;
    }

    convert_args(argc, args);

    /* try to match the command */
    const cmd *command = match_command(args[0].str);
    if (!command)
    {
        printf("command: \"%s\" not found\n",args[0].str);
        return -1;
    }

    int result = command->cmd_callback(argc, args);

    return result;
}

void console_register_commands(cmd_block *block)
{
    ASSERT(block);
    ASSERT(block->next == NULL);

    block->next = command_list;
    command_list = block;
}

static void help_display(const cmd** current_array,int index ,int advanced_display,int counter)
{
    char *p;

    switch(advanced_display)
    {
        default:
        case 0:
        case 1:
            printf("    %-50s: ", current_array[index]->cmd_str);
            break;
        case 2:
            printf("%02d  %-50s: ", counter,current_array[index]->cmd_str);
            break;
    }

    p = (char*)current_array[index]->help_str;
    if (p) {
        while (*p) {
            putc (*p);
            if (*p == '\n' && *(p + 1)) {
                printf ("                                                      ");
            }
            p++;
        }
    }
    putc ('\n');
}

static int cmd_help(int argc, const cmd_args *argv)
{
    printf("command list:\n");

    cmd_block *block;
    size_t i;
    int j=0;
    int k=0;
    int swaps=0;
    int cmd_number=0;
    const cmd** curr_array=NULL;
    int count=1;

    for (block = command_list; block != NULL; block = block->next) {
        for (i = 0; i < block->count; i++) {
            cmd_number++;
        }
    }
    curr_array=(const cmd**)malloc(cmd_number*sizeof(cmd*));
    j=0;
    for (block = command_list; block != NULL; block = block->next) {
        const cmd *curr_cmd = block->list;
        for (i = 0; i < block->count; i++) {
            curr_array[j]=&(curr_cmd[i]);
            j++;
        }
    }

    /* Sort command list */
    for (k = cmd_number - 1; k > 0; --k) {
        swaps = 0;
        for (j = 0; j < k; ++j) {
            if (strcmp (curr_array[j]->cmd_str,curr_array[j + 1]->cmd_str) > 0) {
                const cmd *tmp;
                tmp = curr_array[j];
                curr_array[j] = curr_array[j + 1];
                curr_array[j + 1] = tmp;
                ++swaps;
            }
        }
        if (!swaps)
            break;
    }

    for (j = 0; j < cmd_number; j++) {
        if ((argc >= 3) && (argc < 5))
        {
            if((strstr (curr_array[j]->cmd_str,argv[2].str)!=NULL) && (strncmp(argv[1].str,"<",1)==0))
            {
                if(strncmp(argv[3].str,"ln",2)==0)
                {
                    help_display(curr_array,j,2,count++);
                }
                else
                {
                    help_display(curr_array,j,1,0);
                }
            }
        }
        else
        {
            help_display(curr_array,j,0,0);
        }
    }
    free(curr_array);

    return 0;
}

static int cmd_test(int argc, const cmd_args *argv)
{
    int i;

    printf("argc %d, argv %p\n", argc, argv);
    for (i = 0; i < argc; i++)
        printf("\t%d: str '%s', i %d, u %#x\n", i, argv[i].str, argv[i].i, argv[i].u);

    return 0;
}

static int cmd_hist(int argc, const cmd_args *argv)
{
    cmdNode *tmpNode = firstCmd;
    int i=1;
    int cmd_number=0;
    char* cmd_tmp=NULL;

    switch(argc)
    {
        case 1:
            printf("\n\nThis is the command history:\n");
            while (tmpNode != NULL)
            {
                printf("  %i.  %s\n",i++,tmpNode->cmd);
                tmpNode = tmpNode->next;
            }
            break;
        case 2:
            cmd_number=atoul(argv[1].str);
            while (tmpNode != NULL)
            {
                if(i==cmd_number)
                {
                    cmd_tmp = (char*)malloc(strlen(tmpNode->cmd)+1);
                    strncpy(cmd_tmp,tmpNode->cmd,strlen(tmpNode->cmd)+1);
                    console_run_string_of_cmds(cmd_tmp);
                    free(cmd_tmp);
                    return 0;
                }
                i++;
                tmpNode = tmpNode->next;
            }
            printf("\nCommand number not available\n");
            return 0;

        default:
            return -1;
    }
    return 0;
}

static int last_cmd(int argc, const cmd_args *argv)
{
    cmdNode *tmpNode = firstCmd;

    switch(argc)
    {
        case 1:
            if (firstCmd==NULL)
                printf("None commands\n");
            else
            {
                while (tmpNode->next != NULL)
                {
                    tmpNode = tmpNode->next;
                }
                if((tmpNode->prev==NULL) || (strncmp(tmpNode->prev->cmd,"!!",2)==0) )
                    printf("None commands\n");
                else
                    console_run_string_of_cmds(tmpNode->prev->cmd);
            }
            break;
        default:
            return -1;
    }
    return 0;
}
