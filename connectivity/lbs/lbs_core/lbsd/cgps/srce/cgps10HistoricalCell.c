/**
* \file cgps10HistoricalCell.c
* \date 29/08/2010
* \version 1.0
*
* <B>Compiler:</B> ARM ADS\n
*
* <B>Description:</B>  This file contains all functions relative to the Historical cell Id Table\n
*
* <TABLE>
*     <TR>
*             <TD> Date</TD><TD> Author</TD><TD> Description</TD>
*     </TR>
*     <TR>
*             <TD> 29.08.10 </TD><TD> Puneet Misra </TD><TD> Creation </TD>
*     </TR>
* </TABLE>
*/

#define CGPS10HISTCELL_C
#ifdef AGPS_HISTORICAL_DATA_FTR

#include "cgpsHistoricalCell.hi"
#include "cgpsutils.h"


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 1
/*********************************************************************************/
/*CGPS10_01AddHistoricalCellIdInfo :: Will add nav data in Historical Cell Id table*/
/*********************************************************************************/
void CGPS10_01AddHistoricalCellIdInfo()
{
    t_CGPSHistoricalCellInfo *pl_temp1 = NULL;
    t_CGPSHistoricalCellInfo *pl_temp = NULL;

    uint8_t vl_counter=0;

    MC_CGPS_TRACE_DBG(("HIST:In CGPS10_01AddHistoricalCellIdInfo"));

    if(vg_HistCell == NULL)
    {
        MC_CGPS_TRACE_DBG(("HIST:First Historicalcell Data"));


        pl_temp =(t_CGPSHistoricalCellInfo*)MC_RTK_GET_MEMORY(sizeof(t_CGPSHistoricalCellInfo));

        pl_temp->historicalcell_LocationID =  vg_CGPS_Supl_Location_Id;
        pl_temp->next = NULL;
        vg_HistCell = pl_temp;

        vg_HistCellInfoDb.first = pl_temp;
        vg_HistCellInfoDb.v_CurrCell= pl_temp;
        (vg_HistCellInfoDb.v_NumCell)++;
    }
    else
    {
        pl_temp = vg_HistCell;

        if((vg_HistCellInfoDb.v_NumCell) == MC_CGPS_HISTORICAL_CELL_ID_TABLE_SIZE)
        {
           MC_CGPS_TRACE_DBG(("HIST:Entries Full in HistoricalcellId Table. Writing from First"));

           vl_counter = vg_HistCellInfoDb.v_NewDataIndex;

           while(vl_counter>0)
           {
               pl_temp = pl_temp->next;
               vl_counter--;
           }

           pl_temp->historicalcell_LocationID =  vg_CGPS_Supl_Location_Id;
           vg_HistCellInfoDb.v_CurrCell = pl_temp;
           vg_HistCellInfoDb.v_NewDataIndex++;

           if(vg_HistCellInfoDb.v_NewDataIndex == MC_CGPS_HISTORICAL_CELL_ID_TABLE_SIZE)
           {
              vg_HistCellInfoDb.v_NewDataIndex = 0;
           }
        }
        else
        {


            pl_temp = vg_HistCell;
            while(pl_temp->next != NULL)
            {
                pl_temp = pl_temp->next;
            }


            pl_temp1 =(t_CGPSHistoricalCellInfo*)MC_RTK_GET_MEMORY(sizeof(t_CGPSHistoricalCellInfo));

            pl_temp1->historicalcell_LocationID =  vg_CGPS_Supl_Location_Id;
            vg_HistCellInfoDb.v_CurrCell = pl_temp1;
            pl_temp1->next = NULL;
            pl_temp->next = pl_temp1;

            (vg_HistCellInfoDb.v_NumCell)++;
          }
    }
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 2
/*******************************************************************************************************/
/* CGPS10_02UpdateHistoricalCellIdTable : will add cell id and position in table if cell id is new */
/*otherwise will update the newly available position in the available cell id in table. */
/******************************************************************************************************/
void CGPS10_02UpdateHistoricalCellIdTable(s_GN_SUPL_PVTData* vp_PvtData)
{
    if(vg_HistCellInfoDb.v_CurrCell == NULL)
    {
      if(vg_CGPS_Supl_Location_Id.Status == CIS_NONE)
      {
         MC_CGPS_TRACE_DBG(("HIST:No Location Id Available"));
         return;
      }
      CGPS10_01AddHistoricalCellIdInfo();
      if(vg_HistCellInfoDb.v_CurrCell != NULL)
        vg_HistCellInfoDb.v_CurrCell->historicalcell_nav_data = *vp_PvtData;

    }
    else
        vg_HistCellInfoDb.v_CurrCell->historicalcell_nav_data = *vp_PvtData;

    return;
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 3
/**********************************************************************************/
/* CGPS10_03ReadingHistoricalCellIdTable: Will read HistoricalCellDb.bin  file and populate
   the historical cell id table*/
/***********************************************************************************/
uint8_t CGPS10_03ReadingHistoricalCellIdTable(void)
{
    t_gfl_FILE * pl_File = NULL;
    t_CGPSHistoricalCellInfo *vl_temp,*vl_temp1;
    uint32_t vl_NumRead = 0;
    uint16_t vl_index = 0;
    t_CGPSRwHistoricalCellInfo vl_data = {0};

    vl_temp = vg_HistCell;

    MC_CGPS_TRACE_DBG(("HIST:IN CGPS10_03ReadingHistoricalCellIdTable() function %s ",CGPS_HIST_CELLDB_FILE_NAME));

    pl_File = (t_gfl_FILE *)MC_GFL_FOPEN(CGPS_HIST_CELLDB_FILE_NAME ,(const uint8_t*)"rb");

    if(pl_File)
    {

       MC_CGPS_TRACE_DBG(("HIST:HistoricalCellDb.bin File Opened for reading"));

       for ( vl_index = 0; vl_index < MC_CGPS_HISTORICAL_CELL_ID_TABLE_SIZE; vl_index++ )
       {
          vl_NumRead = MC_GFL_FREAD( &vl_data, 1, sizeof(t_CGPSRwHistoricalCellInfo), pl_File);

           if(vl_NumRead != sizeof(t_CGPSRwHistoricalCellInfo))
           {
               MC_CGPS_TRACE_DBG(("HIST:NO Data in HistoricalCellDb.bin File" ));
               vg_HistCellInfoDb.v_CurrCell = CGPS10_05SearchIdInList();
               MC_GFL_FCLOSE(pl_File);
               return TRUE;
           }
           else
           {
               if(vl_temp == NULL)
               {

                   MC_CGPS_TRACE_DBG(("HIST:First data population from HistoricalCellDb.bin"));

                   vl_temp =(t_CGPSHistoricalCellInfo*)MC_RTK_GET_MEMORY(sizeof(t_CGPSHistoricalCellInfo));

                   vl_temp->historicalcell_nav_data = vl_data. rw_historicalcell_nav_data;

                   vl_temp->historicalcell_LocationID = vl_data.rw_historicalcell_LocationID;

                   vl_temp->next = NULL;

                   vg_HistCell = vl_temp;

                   vg_HistCellInfoDb.first = vl_temp;

                   (vg_HistCellInfoDb.v_NumCell)++;

              }
              else
              {


                   while(vl_temp->next != NULL)
                   {
                       vl_temp = vl_temp->next;
                   }

                   vl_temp1 =(t_CGPSHistoricalCellInfo*)MC_RTK_GET_MEMORY(sizeof(t_CGPSHistoricalCellInfo));

                   vl_temp1->historicalcell_nav_data = vl_data. rw_historicalcell_nav_data;

                   vl_temp1->historicalcell_LocationID = vl_data.rw_historicalcell_LocationID;

                   vl_temp1->next = NULL;

                   vl_temp->next = vl_temp1;

                   (vg_HistCellInfoDb.v_NumCell)++;

              }
          }
       }

       MC_GFL_FCLOSE(pl_File);

    }
    else
    {

       MC_CGPS_TRACE_DBG(("HIST:Error in opening HistoricalCellDb.bin file for read"));
       return FALSE;
    }

    vg_HistCellInfoDb.v_CurrCell = CGPS10_05SearchIdInList();

    return TRUE;
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 4
/**********************************************************************************/
/* CGPS10_04WriteHistoricalCellIdTable:Will write all historical cell id table data in
   HistoricalCellDb.bin  file*/
/***********************************************************************************/
uint8_t CGPS10_04WriteHistoricalCellIdTable(void)
{

   t_gfl_FILE * pl_File = NULL;

   uint16_t vl_NumWrite = 0;

   t_CGPSHistoricalCellInfo  *pl_temp=NULL,*lp_for_freeing_mem=NULL;

   uint8_t vl_error_occured = FALSE;

   t_CGPSRwHistoricalCellInfo vl_data;    // local instance to have only nav data and location id struct not link

   pl_temp = vg_HistCell;

   CGPS10_06PrintHistoricalCellIdTableData();

   MC_CGPS_TRACE_DBG(("HIST:In CGPS10_04WriteHistoricalCellIdTable function %s ",CGPS_HIST_CELLDB_FILE_NAME));

   pl_File = (t_gfl_FILE *)MC_GFL_FOPEN(CGPS_HIST_CELLDB_FILE_NAME ,(const uint8_t*)"wb");

   if(pl_File)
   {
      while((pl_temp != NULL))
      {
          MC_CGPS_TRACE_DBG(("HIST:HistoricalCellDb.bin file opened for write"));

          vl_data.rw_historicalcell_nav_data = pl_temp->historicalcell_nav_data;

          vl_data.rw_historicalcell_LocationID = pl_temp->historicalcell_LocationID;

          vl_NumWrite =MC_GFL_FWRITE(&vl_data ,1,sizeof(t_CGPSRwHistoricalCellInfo), pl_File);

          if (vl_NumWrite != sizeof(t_CGPSRwHistoricalCellInfo))
          {
              vl_error_occured = TRUE;
              break;
          }

          lp_for_freeing_mem = pl_temp;
          pl_temp = pl_temp->next;

          MC_RTK_FREE_MEMORY(lp_for_freeing_mem);  // freeing memory which has been written in file


       }

       MC_GFL_FFLUSH(pl_File);
       MC_GFL_FCLOSE(pl_File);

      if (vl_error_occured == TRUE)
      {

         MC_CGPS_TRACE_DBG(("HIST:Error in writing in HistoricalCellDb.bin file"));

         return FALSE;
      }

     MC_CGPS_TRACE_DBG(("HIST:Success in writing the HistoricalCellDb.bin file"));
   }
   else
   {

      MC_CGPS_TRACE_DBG(("HIST:Error in opening HistoricalCellDb.bin file for write "));
      return FALSE;
   }

   vg_HistCell = NULL;
   return TRUE;
}

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 5
/**********************************************************************************/
/*HistoricalCell_Info * CGPS0_05searchIDinList: Will search incoming cell id with all
  cell ids available in HistoricalCellDb.bin file.If cell id found ,will be sent to supl.*/
/***********************************************************************************/
t_CGPSHistoricalCellInfo * CGPS10_05SearchIdInList(void)
{
   t_CGPSHistoricalCellInfo  *pl_temp = NULL;
   uint16_t                        vl_CellIndex = 0;
   uint8_t vl_Check_Match_Flag = FALSE;

   pl_temp = vg_HistCell;



   MC_CGPS_TRACE_DBG(("HIST:Searching Cell ID"));

   while(pl_temp != NULL)
   {
       vl_CellIndex++;
       switch(vg_CGPS_Supl_Location_Id.Type)
       {
           case CIT_gsmCell:
           {
               s_gsmCellInfo *pl_gsmCellInfo = &(pl_temp->historicalcell_LocationID.of_type.gsmCellInfo);

           s_gsmCellInfo *pl_StoredGsmCellInfo = &(vg_CGPS_Supl_Location_Id.of_type.gsmCellInfo);

               if((pl_gsmCellInfo->refMCC == pl_StoredGsmCellInfo->refMCC) &&
                  (pl_gsmCellInfo->refMNC == pl_StoredGsmCellInfo->refMNC) &&
                  (pl_gsmCellInfo->refLAC == pl_StoredGsmCellInfo->refLAC) &&
                  (pl_gsmCellInfo->refCI  == pl_StoredGsmCellInfo->refCI))
               {
                  MC_CGPS_TRACE_DBG(("HIST:Current GSM CellId =%d Present",
                      vg_CGPS_Supl_Location_Id.of_type.gsmCellInfo.refCI));

                  vl_Check_Match_Flag = TRUE;
               }
               break;
           }
           case CIT_wcdmaCell:
           {
               s_wcdmaCellInfo *pl_wcdmaCellInfo = &(pl_temp->historicalcell_LocationID.of_type.wcdmaCellInfo);

           s_wcdmaCellInfo *pl_StoredWcdmaCellInfo = &(vg_CGPS_Supl_Location_Id.of_type.wcdmaCellInfo);

               if((pl_wcdmaCellInfo->refMCC == pl_StoredWcdmaCellInfo->refMCC) &&
                  (pl_wcdmaCellInfo->refMNC == pl_StoredWcdmaCellInfo->refMNC) &&
                  (pl_wcdmaCellInfo->refUC  == pl_StoredWcdmaCellInfo->refUC))
               {
                  MC_CGPS_TRACE_DBG(("HIST:Current WCDMA CellId = %d Present",
                      vg_CGPS_Supl_Location_Id.of_type.wcdmaCellInfo.refUC));

                  vl_Check_Match_Flag = TRUE;
               }
               break;
           }
           case CIT_cdmaCell:break;
           default : break;
       }
       if(vl_Check_Match_Flag == FALSE)
       {
          if(vl_CellIndex != MC_CGPS_HISTORICAL_CELL_ID_TABLE_SIZE)
          {
             pl_temp = pl_temp->next;
          }
          else
          {
             pl_temp = NULL;
          }

       }
       else
       {
           break;
       }
   }

   return pl_temp ;
}

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 6
/**********************************************************************************/
/* CGPS10_06PrintHistoricalCellIdTableData: will print all data in historical cell id(link list)
for debugging purpose*/
/***********************************************************************************/
void CGPS10_06PrintHistoricalCellIdTableData()
{
#ifdef AGPS_HISTORICAL_DATA_DEBUG_FTR
    t_CGPSHistoricalCellInfo  *pl_temp = vg_HistCell;
    s_GN_AGPS_GAD_Data  *pl_Gad =  &pl_temp->historicalcell_nav_data.gad;
    s_GN_GPS_UTC_Data *pl_Utc = &pl_temp->historicalcell_nav_data.utc;

    uint16_t vl_index = 0;


    while(pl_temp != NULL)
    {
        MC_CGPS_TRACE_DBG(("HIST:LCID UTC DATA NO-- %d ---[%d %d %d] [%d:%d:%d.%d] %d %d ",
            vl_index,pl_Utc->Year,
            pl_Utc->Month,
            pl_Utc->Day,
            pl_Utc->Hours,
            pl_Utc->Minutes,
            pl_Utc->Seconds,
            pl_Utc->Milliseconds,
            pl_Utc->Acc_Est,
            pl_Utc->OS_Time_ms));


        MC_CGPS_TRACE_DBG(("HIST:LCID GAD DATA NO-- %d---[%d %d %d %d %d] [%d %d %d] [%d %d %c %d] [%d %c %d %d] ",
            vl_index,
            pl_Gad->latitudeSign,pl_Gad->latitude,pl_Gad->longitude,pl_Gad->altitudeDirection,pl_Gad->altitude,
            pl_Gad->uncertaintySemiMajor,pl_Gad->uncertaintySemiMinor,pl_Gad->orientationMajorAxis,
            pl_Gad->uncertaintyAltitude,pl_Gad->confidence,pl_Gad->verdirect,pl_Gad->bearing,
            pl_Gad->horspeed,pl_Gad->verspeed,pl_Gad->horuncertspeed,pl_Gad->veruncertspeed));

        if(pl_temp->historicalcell_LocationID.Type == CIT_gsmCell )
        {
            MC_CGPS_TRACE_DBG(("HIST:GSMCELLID DATA NO-- %d---%d %d [%d %d] Cell[%d %d] ",
            vl_index++,
            pl_temp->historicalcell_LocationID.Status,
            pl_temp->historicalcell_LocationID.Type,
            pl_temp->historicalcell_LocationID.of_type.gsmCellInfo.refMCC,
            pl_temp->historicalcell_LocationID.of_type.gsmCellInfo.refMNC,
            pl_temp->historicalcell_LocationID.of_type.gsmCellInfo.refLAC,
            pl_temp->historicalcell_LocationID.of_type.gsmCellInfo.refCI));
        }

        if(pl_temp->historicalcell_LocationID.Type == CIT_wcdmaCell )
        {
            MC_CGPS_TRACE_DBG(("HIST:WCDMACELLID DATA NO--- %d---%d %d [%d %d] UC[%d] ",
            vl_index++,
            pl_temp->historicalcell_LocationID.Status,
            pl_temp->historicalcell_LocationID.Type,
            pl_temp->historicalcell_LocationID.of_type.wcdmaCellInfo.refMCC,
            pl_temp->historicalcell_LocationID.of_type.wcdmaCellInfo.refMNC,
            pl_temp->historicalcell_LocationID.of_type.wcdmaCellInfo.refUC));
        }

        pl_temp = pl_temp->next;
        if(pl_temp != NULL)
        {
           pl_Gad = &pl_temp->historicalcell_nav_data.gad;
           pl_Utc = &pl_temp->historicalcell_nav_data.utc;
        }
        else
        {
           pl_Gad = NULL;
           pl_Utc = NULL;
        }
    }
#endif /* AGPS_HISTORICAL_DATA_DEBUG_FTR */
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 7
/**********************************************************************************/
/* Check the status of the current cell*/
/***********************************************************************************/

void CGPS10_07CheckCurrentCellStatus()
{

 MC_CGPS_TRACE_DBG(("HIST: MOBINFO WCDMA %d %d [%d %d] UC[%d]",
         vg_CGPS_Supl_Location_Id.Status,
         vg_CGPS_Supl_Location_Id.Type,
         vg_CGPS_Supl_Location_Id.of_type.wcdmaCellInfo.refMCC,
         vg_CGPS_Supl_Location_Id.of_type.wcdmaCellInfo.refMNC,
         vg_CGPS_Supl_Location_Id.of_type.wcdmaCellInfo.refUC));

 MC_CGPS_TRACE_DBG(("HIST: MOBINFO GSM %d %d [%d %d] UC[%d %d]",
        vg_CGPS_Supl_Location_Id.Status,
        vg_CGPS_Supl_Location_Id.Type,
        vg_CGPS_Supl_Location_Id.of_type.gsmCellInfo.refMCC,
        vg_CGPS_Supl_Location_Id.of_type.gsmCellInfo.refMNC,
        vg_CGPS_Supl_Location_Id.of_type.gsmCellInfo.refLAC,
        vg_CGPS_Supl_Location_Id.of_type.gsmCellInfo.refCI));

 MC_CGPS_TRACE_DBG(("HIST: Old Current Cell %x",vg_HistCellInfoDb.v_CurrCell));

 vg_HistCellInfoDb.v_CurrCell = CGPS10_05SearchIdInList();


 if( vg_HistCellInfoDb.v_CurrCell != NULL )
 {
    MC_CGPS_TRACE_DBG(("HIST: AF vg_HistCellInfoDb.v_CurrCell %x",vg_HistCellInfoDb.v_CurrCell));
 }

}

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 8
/**********************************************************************************/
/* CGPS10_08GetHistPosition Get the Historical Position*/
/***********************************************************************************/

s_GN_SUPL_PVTData * CGPS10_08GetHistPosition()
{
   s_GN_SUPL_PVTData *pl_cellID_Data = NULL;

   if(vg_HistCellInfoDb.v_CurrCell == NULL) return NULL;

    pl_cellID_Data = &((vg_HistCellInfoDb.v_CurrCell)->historicalcell_nav_data);

   /*As per the specification
           if the last position is within 5 second, SUPL must send this position with the confidence as 100
           if the last position is greater than 5 second, SUPL must send this position with the confidence as 50
       */
    if (GN_GPS_Get_OS_Time_ms() - pl_cellID_Data->utc.OS_Time_ms > CGPS_HIST_MAX_AGE_OF_GOOD_CONFIDENCE)
        pl_cellID_Data->gad.confidence = CGPS_HIST_CONFIDENCE_MAXIMUM;
    else
        pl_cellID_Data->gad.confidence = CGPS_HIST_CONFIDENCE_AVERAGE;


    MC_CGPS_TRACE_DBG(("HIST:UTC PVT SUPL [%d %d %d] [%d:%d:%d.%d] [%d %d] ",
        pl_cellID_Data->utc.Year,pl_cellID_Data->utc.Month,pl_cellID_Data->utc.Day,
        pl_cellID_Data->utc.Hours,pl_cellID_Data->utc.Minutes,pl_cellID_Data->utc.Seconds,
        pl_cellID_Data->utc.Milliseconds,pl_cellID_Data->utc.Acc_Est,pl_cellID_Data->utc.OS_Time_ms));

    MC_CGPS_TRACE_DBG(("HIST:GAD PVT SUPL [%d %d %d %d %d] [%d %d %d] [%d %d %d %d] [%d %d %d %d]",
        pl_cellID_Data->gad.latitudeSign,pl_cellID_Data->gad.latitude,pl_cellID_Data->gad.longitude,
        pl_cellID_Data->gad.altitudeDirection,pl_cellID_Data->gad.altitude,
        pl_cellID_Data->gad.uncertaintySemiMajor,pl_cellID_Data->gad.uncertaintySemiMinor,pl_cellID_Data->gad.orientationMajorAxis,
        pl_cellID_Data->gad.uncertaintyAltitude,pl_cellID_Data->gad.confidence,pl_cellID_Data->gad.verdirect,pl_cellID_Data->gad.bearing,
        pl_cellID_Data->gad.horspeed,pl_cellID_Data->gad.verspeed,
        pl_cellID_Data->gad.horuncertspeed,pl_cellID_Data->gad.veruncertspeed));

    return pl_cellID_Data;

}

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 10
/**********************************************************************************/
/*  CGPS10_10UpdataUtcfromNavn copy UTC data from NAV data if valid*/
/***********************************************************************************/
bool CGPS10_10UpdatUtcFromNav(s_GN_GPS_UTC_Data *p_UTC)
{
  MC_CGPS_TRACE_DBG(("HIST:vg_Nav_Data.Valid_SingleFix  %d",vg_Nav_Data.Valid_SingleFix));
  if(!vg_Nav_Data.Valid_SingleFix) return FALSE;

  p_UTC->Year = vg_Nav_Data.Year;
  p_UTC->Month = vg_Nav_Data.Month;
  p_UTC->Day = vg_Nav_Data.Day;
  p_UTC->Hours = vg_Nav_Data.Hours;
  p_UTC->Minutes = vg_Nav_Data.Minutes;
  p_UTC->Seconds = vg_Nav_Data.Seconds;
  p_UTC->Milliseconds = vg_Nav_Data.Milliseconds;
  p_UTC->OS_Time_ms = vg_Nav_Data.OS_Time_ms;
  p_UTC->Acc_Est = 0;//This field is not used and is not required to be filled
  return TRUE;

}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 9
/**********************************************************************************/
/* CGPS10_08GetHistPosition Ger the Historical Position*/
/***********************************************************************************/

void CGPS10_09UpdatePositionInHistDb(void)
{
    s_GN_AGPS_GAD_Data  p_GAD;
    s_GN_GPS_UTC_Data p_UTC;
    bool vl_GadReady,vl_UtcReady;
    s_GN_SUPL_PVTData vl_SuplNavData;
    uint32_t vl_GAD_Ref_TOW;
    int32_t vl_EFSP_dT_us;
    int16_t vl_GAD_Ref_TOW_Subms;
    s_GN_AGPS_QoP vl_QoP;


    uint32_t vl_Previous_Os_Time = 0;

    if(vg_CGPS_Supl_Location_Id.Status == CIS_stale || vg_CGPS_Supl_Location_Id.Status == CIS_unknown)
    {
       MC_CGPS_TRACE_DBG(("HIST:Cell ID Unknown or Stale->NO UPDATE in Db"));
       return;
    }

    vl_QoP.Deadline_OS_Time_ms = CGPS_HIST_DEFAULT_DELAY_MS;
    vl_QoP.Vert_Accuracy = CGPS_HIST_DEFAULT_VER_ACC;
    vl_QoP.Horiz_Accuracy =    CGPS_HIST_DEFAULT_HOR_ACC;

    if (GN_AGPS_Qual_Pos((U4*)&(vl_Previous_Os_Time), &vl_QoP)== FALSE) return;
    vl_GadReady = GN_AGPS_Get_GAD_Data(NULL,(U4*)&vl_GAD_Ref_TOW, (I2*)&vl_GAD_Ref_TOW_Subms,/* (U1*) &vl_GAD_Ref_Tow_Unc,*/ (I4*) &vl_EFSP_dT_us, &p_GAD);
    MC_CGPS_TRACE_DBG(("HIST:vl_GadReady %d",vl_GadReady));


   if(vl_GadReady)
   {
      vl_SuplNavData.gad = p_GAD;
      vl_UtcReady = CGPS10_10UpdatUtcFromNav(&p_UTC);
      MC_CGPS_TRACE_DBG(("HIST:vl_UtcReady %d",vl_UtcReady));

      if(vl_SuplNavData.gad.uncertaintySemiMajor < 60)
      {
         MC_CGPS_TRACE_DBG(("HIST:uncertaintySemiMajor less than 60"));
         vl_SuplNavData.gad.uncertaintySemiMajor = 60;
      }
      if(vl_SuplNavData.gad.uncertaintySemiMinor < 60)
      {
         MC_CGPS_TRACE_DBG(("HIST:uncertaintySemiMinor less than 60"));
         vl_SuplNavData.gad.uncertaintySemiMinor = 60;
      }

      vl_SuplNavData.utc = p_UTC;

      CGPS10_02UpdateHistoricalCellIdTable(&vl_SuplNavData);

      CGPS10_06PrintHistoricalCellIdTableData();
  }

}




#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 11
/***************************************************************************************************************/
/*  CGPS10_11InjectHistDataInGpsLib : Will inject historical cell pos data in GPSLIB if no  assisted gps
means only autonomous mode is permitted */
/*****************************************************************************************************************/
bool CGPS10_11InjectHistPosData(void)
{
   s_GN_AGPS_GAD_Data *p_RPos= NULL;
   bool vl_ret = 0;

  if(vg_HistCellInfoDb.v_CurrCell != NULL)
   {
       p_RPos = &((vg_HistCellInfoDb.v_CurrCell)->historicalcell_nav_data.gad);

       /*
       * 1km is scaled to 48
       * 20km is scaled to 80
       * 100m is scaled to 25
       */
       // Scaled value of uncertaintySemiMajor and uncertaintySemiMajor corresponding to 20 Km.
       //p_RPos->uncertaintySemiMajor = 80;
       //p_RPos->uncertaintySemiMinor = 80;
       p_RPos->uncertaintySemiMajor = 77; /*15.4 Kms*/
       p_RPos->uncertaintySemiMinor = 77; /*15.4 Kms*/


       MC_CGPS_TRACE_DBG(("HIST:LatSign %d, Lat %d, Long %d, UncSemiMajor %d, UncSemiMinor %d, OrienMajorAxis %d, UncAlt %d, Confi %d, AltDir %d, Alt %d\n", \
                     p_RPos->latitudeSign, p_RPos->latitude, p_RPos->longitude, p_RPos->uncertaintySemiMajor, p_RPos->uncertaintySemiMinor, p_RPos->orientationMajorAxis, \
                     p_RPos->uncertaintyAltitude, p_RPos->confidence, p_RPos->altitudeDirection, p_RPos->altitude));


       MC_CGPS_TRACE_DBG(("HIST:Verdict %d, Bearing %d, HorSpeed %d, VerSpeed %d, HorUncSpeed %d, VerUncSpeed %d\n", \
                     p_RPos->verdirect, p_RPos->bearing, p_RPos->horspeed, p_RPos->verspeed, p_RPos->horuncertspeed, p_RPos->veruncertspeed));

          /* Only for testing , randomize the position */
       //RandomizeLocation( &p_RPos , 20000 );

       MC_CGPS_TRACE_DBG(("HIST:LatSign %d, Lat %d, Long %d, UncSemiMajor %d, UncSemiMinor %d, OrienMajorAxis %d, UncAlt %d, Confi %d, AltDir %d, Alt %d\n", \
                     p_RPos->latitudeSign, p_RPos->latitude, p_RPos->longitude, p_RPos->uncertaintySemiMajor, p_RPos->uncertaintySemiMinor, p_RPos->orientationMajorAxis, \
                     p_RPos->uncertaintyAltitude, p_RPos->confidence, p_RPos->altitudeDirection, p_RPos->altitude));


       vl_ret=GN_AGPS_Set_GAD_Ref_Pos(p_RPos);

       if (vl_ret == FALSE)
          MC_CGPS_TRACE_DBG(("HIST:library reject the reference position"));
       else
             MC_CGPS_TRACE_DBG(("HIST:library accept the reference position"));

   }
   else
   {
      MC_CGPS_TRACE_DBG(("HIST: Either no current cell available OR SUPL or CP sessoin is registered"));
   }
   return vl_ret;

}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 12
/***************************************************************************************************************/
/*  CGPS10_12ClearHistoricalData : Will clear historical data from the memory */
/*****************************************************************************************************************/
void CGPS10_12ClearHistoricalData()
{
    t_CGPSHistoricalCellInfo *vl_temp;
    while(vg_HistCell != NULL)
    {
        vl_temp = vg_HistCell;
        vg_HistCell = vg_HistCell->next;
        MC_RTK_FREE_MEMORY(vl_temp);
    }
    vg_HistCellInfoDb.v_NumCell = 0;
    vg_HistCellInfoDb.first = NULL;
    vg_HistCellInfoDb.v_CurrCell = NULL;
    vg_HistCellInfoDb.v_NewDataIndex = 0;
}

#endif /*AGPS_HISTORICAL_DATA_FTR */


#undef CGPS10HISTCELL_C

