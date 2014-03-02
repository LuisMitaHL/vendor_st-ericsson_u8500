#ifndef __CGPSHISTCELL_HIP__
#define __CGPSHISTCELL_HIP__
/**
* \file cgpsHistCell.hic
* \date 27/08/2010
* \version 1.0
*
* <B>Compiler:</B> ARM ADS\n
*
* <B>Description:</B> This file contains HistoricalCell.c function prototypes CGPS\n
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

void CGPS10_02UpdateHistoricalCellIdTable(s_GN_SUPL_PVTData* p_Nav_Data);

uint8_t CGPS10_03ReadingHistoricalCellIdTable(void);

uint8_t CGPS10_04WriteHistoricalCellIdTable(void);

void CGPS10_06PrintHistoricalCellIdTableData(void);

t_CGPSHistoricalCellInfo * CGPS10_05SearchIdInList(void);

bool CGPS10_11InjectHistPosData(void);

void CGPS10_07CheckCurrentCellStatus(void);

s_GN_SUPL_PVTData * CGPS10_08GetHistPosition(void);

void CGPS10_09UpdatePositionInHistDb(void);

void CGPS10_12ClearHistoricalData(void);

#endif /* AGPS_HISTORICAL_DATA_FTR */

#endif  /*__CGPSHISTCELL_HIP__*/

