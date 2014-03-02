/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * \brief  Routing Control
 */

#ifndef AT_TOK_H
#define AT_TOK_H 1

#ifdef __cplusplus
extern "C" {
#endif

    /**
     * Returns 1 if line starts with prefix, 0 if it does not.
     */
    int strStartsWith(const char *line, const char *prefix);

    int at_tok_start(char **p_cur);
    int at_tok_nextint(char **p_cur, int *p_out);
    int at_tok_nexthexint(char **p_cur, int *p_out);

    int at_tok_nextbool(char **p_cur, char *p_out);
    int at_tok_nextstr(char **p_cur, char **out);

    int at_tok_hasmore(char **p_cur);

    int at_tok_charcounter(char *p_in, char needle, int *p_out);

#ifdef __cplusplus
}
#endif
#endif
