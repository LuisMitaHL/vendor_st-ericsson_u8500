/****************************************************************************
*                                                                           *
*  File:        getarg.c                                                    *
*  Version:     1.0                                                         *
*  Author:      Göran Rundqwist                                             *
*  Description: Library functions for splitting a line into arguments.      *
*                                                                           *
****************************************************************************/
#define MSDEV

#include "string.h"
#ifdef MSDEV
  #define FALSE 0
  #define TRUE  1
#else
  #include "stddefs.h"
#endif

/****************************************************************************
*                                                                           *
*  Name:        GetArg                                                      *
*  Parameters:  String - string to be split up into arguments.              *
*               Max - max number of arguments the string will be split up   *
*                     into.                                                 *
*               Text - Flag if string is data or text.                      *
*               argc - Number of found arguments.                           *
*               argv - vector with pointers to found arguments. Dimension   *
*               must be at least (Max).                                     *
*  Description: Set pointers to the different parameters in String.         *
*               Note that string is "destroyed". If Text is TRUE            *
*               '+', '-', '*', '/', '(', ')', '<', '=', '>' and '.'         *
*               will be classified as separators.                           *
*                                                                           *
****************************************************************************/

void GetArg( char *String, short Max, short Text, short *argc, char *argv[])
{
  char Data, IsSep;
  short k, len;

  *argc = 0;
  Data = FALSE;
  len = (short)strlen( String );

  for ( k = 0 ; k < len ; k++ )
  {
    switch( String[k] )
    {
    case 0x1A:          /* EOF is a separator */
    case ' ':
    case '\t':
    case '\n':
    case ',':   IsSep = TRUE;
      break;

    case '"':
      if ( !Data )
      {
          /* New parameter detected */
          argv[*argc] = &String[k + 1];
          (*argc)++;
          k++;

          /* Scan string until next " is found. */
          for ( ; k < len && String[k] != '"' ; k++ );
          IsSep = TRUE;
          break;
      }

    case '#':
    case '+':
    case '-':
    case '*':
    case '/':
    case '(':
    case ')':
    case '<':
    case '=':
    case '>':
    case '.':
      IsSep = Text;
      break;

    default:
      IsSep = FALSE;
      break;
    }

    if ( IsSep )
    {
      /* Separator */
      Data = FALSE;
      String[k] = '\0';
    }
    else
    {
      if ( !Data )
      {
        if ( *argc < Max )
        {
          /* New parameter detected */
          argv[*argc] = &String[k];
          (*argc)++;
          Data = TRUE;
        }
        else
        {
          /* Cannot handle more parameters */
          break;
        }
      }
    }
  }
}

