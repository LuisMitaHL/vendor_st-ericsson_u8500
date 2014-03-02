#ifndef __CGPSHISTCELL_HIC__
#define __CGPSHISTCELL_HIC__
/**
* \file cgpsHistCell.hic
* \date 27/08/2010
* \version 1.0
*
* <B>Compiler:</B> ARM ADS\n
*
* <B>Description:</B> This file contains Historical Data constants CGPS\n
* 
* <TABLE>
*     <TR>
*             <TD> Date</TD><TD> Author</TD><TD> Description</TD>
*     </TR>
*     <TR>
*             <TD> 27.08.10</TD><TD> Puneet Misra </TD><TD> Creation </TD>
*     </TR>
* </TABLE>
*/

#ifdef AGPS_HISTORICAL_DATA_FTR

#define MC_CGPS_HISTORICAL_CELL_ID_TABLE_SIZE 1000

#define CGPS_HIST_CELLDB_FILE_NAME "/data/HistoricalCellDb.bin"   

#define CGPS_HIST_DEFAULT_HOR_ACC       25 /* This corresponds to horizontal acc of 100m  k = log( (hor_acc_in_m/10) + 1 ) / log ( 1.1 ) */
#define CGPS_HIST_DEFAULT_VER_ACC       47 /* This corresponds to vertical acc of 100m k = log( (ver_acc_in_m/45) + 1 ) / log ( 1.025 ) */
#define CGPS_HIST_DEFAULT_DELAY         7  /* This corresponds to 128s delay 2^7 */
#define CGPS_HIST_DEFAULT_DELAY_MS         128000  /* This corresponds to 128s */
#define CGPS_HIST_DEFAULT_MAX_LOC_AGE   10 /* Dont want fix older than 10s */
#define CGPS_HIST_MAX_AGE_OF_GOOD_CONFIDENCE  5000 /* Confidence must be set to 100 if position is within 5 sec*/
#define CGPS_HIST_CONFIDENCE_MAXIMUM  100 /* Confidence as 100 percent*/
#define CGPS_HIST_CONFIDENCE_AVERAGE  50   /* Confidence as 50 percent*/

#endif  /* AGPS_HISTORICAL_DATA_FTR */

#endif /*__CGPSHISTCELL_HIC__*/

