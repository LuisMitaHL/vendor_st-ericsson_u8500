

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "cmdline_parser.h"




#ifdef __GNUC__
__attribute__ ((noreturn, format(printf, 2, 3)))
#endif
static void exitf(int retval, const char *fmt, ...) // TODO: duplicated code
{
    va_list a;
    va_start(a, fmt);
    vfprintf(stderr, fmt, a);
    exit(retval);
}


static int ExtractArg(int argc, char **argv, int argidx,
                      CmdLineArgs_t * CmdLine)
{
    if (CmdLine->Type == CMD_LINE_ARG_TYPE_BOOL && argv[argidx][0] == '-') {
        CmdLine->Type = CMD_LINE_ARG_TYPE_DONE;
        *((int *) CmdLine->Value) = 1;
        return 1;
    }

    int argoffs = 0;
    if (argv[argidx][0] == '-') {
        argoffs = 1;
    }

    if (argidx + argoffs < argc) {
        if (CmdLine->Type == CMD_LINE_ARG_TYPE_STRING) {
            CmdLine->Type = CMD_LINE_ARG_TYPE_DONE;
            *((const char **) CmdLine->Value) = argv[argidx + argoffs];
            return 1 + argoffs;
        }

        if (CmdLine->Type == CMD_LINE_ARG_TYPE_INT) {
            CmdLine->Type = CMD_LINE_ARG_TYPE_DONE;
            sscanf(argv[argidx + argoffs], "%d", (int*) CmdLine->Value);
            return 1 + argoffs;
        }
    }

    exitf(EXIT_FAILURE,
          "ExtractArg: Error parsing command line (missing parameter or repeated flag?)\n");
}

static void PrintHelp(const CmdLineArgs_t * Args, int NumArgs)
{
    unsigned int MaxFlagLength = 0;
    int i;
    for (i = 0; i < NumArgs; i++) {
        if (strlen(Args[i].FlagName_p) > MaxFlagLength) {
            MaxFlagLength = strlen(Args[i].FlagName_p);
        }
    }

    for (i = 0; i < NumArgs; i++) {
        const char *FlagName_p = Args[i].FlagName_p;
        const char *Desc_p = Args[i].Desc_p;
        while (*Desc_p) {
            int Length;
            if (strchr(Desc_p, '\n')) {
                Length = 1 + strchr(Desc_p, '\n') - Desc_p;
            } else {
                Length = (int)strlen(Desc_p);
            }

            printf("   %-*s  %.*s", MaxFlagLength, FlagName_p, Length,
                   Desc_p);
            FlagName_p = "";
            Desc_p += Length;
        }
    }
}

int ParseConfigHandler(int argc, char **argv,
                        CmdLineArgs_t * CmdLineArgs_p, int NumArgs,
                        const char *HelpPreText_p,
                        const char *HelpPostText_p,
                        int first_arg)
{
    if (first_arg == 1) {
        // first_arg == 1 --> argv[1] is first argument (argv[0] is file name)
        if (argc <= 1 || strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "?") == 0) {
            if (HelpPreText_p) printf("%s", HelpPreText_p);
            PrintHelp(CmdLineArgs_p, NumArgs);
            if (HelpPostText_p) printf("%s", HelpPostText_p);
        exit(EXIT_SUCCESS);
        }
    } else if (first_arg == 0) {
        // first_arg == 0 --> argv[0] is first argument, not file name
        if (argc == 0 || strcmp(argv[0], "--help") == 0 || strcmp(argv[0], "?") == 0) {
            if (HelpPreText_p) printf("%s", HelpPreText_p);
            PrintHelp(CmdLineArgs_p, NumArgs);
            if (HelpPostText_p) printf("%s", HelpPostText_p);
            exit(EXIT_SUCCESS);
        }
    } else { exitf(EXIT_FAILURE, "ParseConfigHandler: first_arg invalid\n"); }


    int codecSeparatorFound = 0;
    int i = first_arg;

    while (i < argc && !codecSeparatorFound) {
        int j;
        {
            for (j = 0; j < NumArgs; j++) {
                if (argv[i][0] == '-') {
                    if (strcmp(argv[i], CmdLineArgs_p[j].FlagName_p) == 0) {
                        i += ExtractArg(argc, argv, i, &CmdLineArgs_p[j]);
                        break;
                    }
                }
                else if (CmdLineArgs_p[j].FlagName_p[0] != '-'
                        && CmdLineArgs_p[j].Type != CMD_LINE_ARG_TYPE_DONE
                        && CmdLineArgs_p[j].Type != CMD_LINE_ARG_TYPE_BOOL) {
                        i += ExtractArg(argc, argv, i, &CmdLineArgs_p[j]);
                        break;

                }
            }
        }

        if (j == NumArgs) {
            fprintf(stderr, "Unhandled argument: '%s'\n", argv[i]);
            return 0;
        }
    }
    return 1;
}


