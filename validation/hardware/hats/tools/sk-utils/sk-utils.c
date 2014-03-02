/*
*	Copyright (C) ST-Ericsson 2011
*	This file provides utilities used by 'sk' shell script
*	Author:	gerald.baeza@stericsson.com
*/

#include <stdio.h>
#include <stdlib.h>

#define ADD 0
#define SUB 1
#define AND 2
#define OR  3
#define SET 4
#define CLR 5

int is_hex(char * txt_line)
{
  if( (txt_line[0] =='0') && ((txt_line[1] =='x') || (txt_line[1] =='X')))
    return 1;
  else 
    return 0;
}

int is_bin(char * txt_line)
{
  if( (txt_line[0] == 'b') || (txt_line[0] == 'B'))
    return 1;
  else
    return 0;
}

int main( int argc, char *argv[] )
{
  int fct;
  long long op1, op2;
  long result = 0;

  if (argc == 1) { 
    printf("Usage: %s <operation> <value1> <value2>\n\n", argv[0]);
    printf(" <operations> can be:\n");
    printf("    '0' = ADD = <value1> + <value2>\n");
    printf("    '1' = SUB = <value1> - <value2>\n");
    printf("    '2' = AND = <value1> & <value2>\n");
    printf("    '3' = OR  = <value1> + <value2>\n");
    printf("    '4' = SET = set bit numbered <value1> in <value2>\n");
    printf("    '5' = CLR = clear bit numbered <value1> in <value2>\n");
  }
  else {
    fct = atoi( argv[1] );

    if(is_hex(argv[2])){
      op1 = strtoll( argv[2], (char**)NULL, 16 );
    }
    else if(is_bin(argv[2])){
      (argv[2])++; /* Skip the initial 'b' */
      op1 = strtoll( argv[2], (char**)NULL, 2 );
    }
    else{
      op1 = strtoll( argv[2], (char**)NULL, 10 );
    }

    if(is_hex(argv[3])){
      op2 = strtoll( argv[3], (char**)NULL, 16 );
    }
    else if(is_bin(argv[3])){
      (argv[3])++; /* Skip the initial 'b' */
      op2 = strtoll( argv[3], (char**)NULL, 2 );
    }
    else{
      op2 = strtoll( argv[3], (char**)NULL, 10 );
    }

    switch ( fct ){
      case ADD :
        result = (long) op1 + op2;
      break;
      case SUB :
        result = (long) op1 - op2;
      break;
      case AND :
        result = (long) op1 & op2;
      break;
      case OR :
        result = (long) op1 | op2;
      break;
      case SET :
        result = (long) op2 | (1 << op1);
      break;
      case CLR :
        result = (long) op2 ^ (1 << op1);
      break;
    }
  printf("0x%08x\n", (int)result);
  }
 return 0;
}


