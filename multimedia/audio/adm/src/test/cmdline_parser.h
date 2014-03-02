#ifndef _INCLUSION_GUARD_CMD_LINE_PARSER_H_
#define _INCLUSION_GUARD_CMD_LINE_PARSER_H_


typedef enum {
    CMD_LINE_ARG_TYPE_STRING,
    CMD_LINE_ARG_TYPE_INT,
    CMD_LINE_ARG_TYPE_BOOL,
    CMD_LINE_ARG_TYPE_DONE
} CmdLineArgType_t;

typedef struct {
    CmdLineArgType_t Type;
    void *Value;
    const char *FlagName_p;
    const char *Desc_p;
} CmdLineArgs_t;

// first_arg == 0 --> argv[0] is first argument, not file name
// first_arg == 1 --> argv[0] is filename, argv[1] is first argument
int ParseConfigHandler(int argc, char **argv,
                        CmdLineArgs_t * CmdLineArgs_p, int NumArgs,
                        const char *HelpPreText_p,
                        const char *HelpPostText_p,
                        int first_arg);


#endif // _INCLUSION_GUARD_COMMAND_LINE_PARSER_H_
