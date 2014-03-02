//*************************************************************************
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 
//
// Filename GAD_helper.c
//
// $Header: X:/MKS Projects/prototype/prototype/GAD_helper/rcs/GAD_helper.c 1.19 2009/01/13 14:09:35Z grahama Rel $
// $Locker: $
//*************************************************************************
//*************************************************************************
///
/// \ingroup rrlp_helper
///
/// \file
/// \brief
///      GAD helper routines implementation.
///
///      Specification:
///      3GPP TS 23.032 V5.0.0 (2003-03)
///      3rd Generation Partnership Project;
///      Technical Specification Group Core Network;
///      Universal Geographical Area Description (GAD)
///      (Release 5)
///      Access routines specifically for converting to and from GAD format.
//*************************************************************************

#include <stdlib.h>

#include "GAD_helper.h"
#include "GN_AGPS_api.h"
#include "GAD_Conversions.h"

/// Local type for determining the layout of the elements within a GAD message.
typedef struct GAD_Element_Position {
   U1  BufferSize;                  ///< Size of the GAD message.
   U1  Latitude;                    ///< Offset of Latitude within the GAD message, starting with 1 (0 if field not present).
   U1  Longitude;                   ///< Offset of Longitude within the GAD message, starting with 1 (0 if field not present).
   U1  Altitude;                    ///< Offset of Altitude within the GAD message, starting with 1 (0 if field not present).
   U1  Uncertainty_Semi_Major;      ///< Offset of Uncertainty_Semi_Major within the GAD message, starting with 1 (0 if field not present).
   U1  Uncertainty_Semi_Minor;      ///< Offset of Uncertainty_Semi_Minor within the GAD message, starting with 1 (0 if field not present).
   U1  Orientation_Of_Major_Axis;   ///< Offset of Orientation_Of_Major_Axis within the GAD message, starting with 1 (0 if field not present).
   U1  Uncertainty_Altitude;        ///< Offset of Uncertainty_Altitude within the GAD message, starting with 1 (0 if field not present).
   U1  Confidence;                  ///< Offset of Confidence within the GAD message, starting with 1 (0 if field not present).
} s_GAD_Element_Position;

e_GAD_Shape_Type GAD_Shape_Type_From_Message(s_GAD_Message *s_GAD_Message);
BL GN_GAD_Element_Position_From_Shape_Type(e_GAD_Shape_Type ShapeType, s_GAD_Element_Position *p_GAD_Element_Position);

BL GN_GAD_Latitude_from_Message_Field(R4 *p_Latitude, U1 p_Message_Field[3]);
BL GN_GAD_Longitude_from_Message_Field(R4 *p_Longitude, U1 p_Message_Field[3]);
BL GN_GAD_Uncertainty_from_Message_Field(R4 *p_Uncertainty, U1 p_Message_Field[1]);
BL GN_GAD_Orientation_from_Message_Field(I2 *p_RMS_SMajBrg, U1 p_Message_Field[1]);
BL GN_GAD_Altitude_from_Message_Field(R4 *p_Altitude, U1 p_Message_Field[2]);
BL GN_GAD_Uncertainty_Altitude_from_Message_Field(R4 *p_RMS_Altitude, U1 p_Message_Field[1]);
BL GN_GAD_Confidence_from_Message_Field(U1 *Confidence, U1 p_Message_Field[1]);

BL GN_GAD_Message_Field_from_Latitude(U1 p_Message_Field[3], R4 Latitude);
BL GN_GAD_Message_Field_from_GAD_Latitude(U1 p_Message_Field[3], U4 Latitude, U1 Latitude_Sign);

BL GN_GAD_Message_Field_from_Longitude(U1 p_Message_Field[3], R4 Longitude);
BL GN_GAD_Message_Field_from_GAD_Longitude(U1 p_Message_Field[3], U4 Longitude);

BL GN_GAD_Message_Field_from_Altitude(U1 p_Message_Field[2], R4 Altitude);
BL GN_GAD_Message_Field_from_GAD_Altitude(U1 p_Message_Field[2], U2 Altitude, I1 AltitudeDirection);

BL GN_GAD_Message_Field_from_Uncertainty(U1 p_Message_Field[1], R4 RMS_Uncertainty);
BL GN_GAD_Message_Field_from_GAD_Uncertainty(U1 p_Message_Field[1], U1 RMS_Uncertainty);

BL GN_GAD_Message_Field_from_Orientation(U1 p_Message_Field[1], I2 RMS_SMajBrg);
BL GN_GAD_Message_Field_from_GAD_Orientation(U1 p_Message_Field[1], U1 OrientationMajorAxis);

BL GN_GAD_Message_Field_from_Uncertainty_Altitude(U1 p_Message_Field[1], R4 RMS_Altitude);
BL GN_GAD_Message_Field_from_GAD_Uncertainty_Altitude(U1 p_Message_Field[1], U1 RMS_Altitude);
BL GN_GAD_Message_Field_from_Confidence(U1 p_Message_Field[1], U1 Confidence);

//*****************************************************************************
/// \brief
///      Internal function for calculating an enum to indicate the shape type.
///
/// \returns
///      #e_GAD_Shape_Type describing the Shape Type in the message.
//*****************************************************************************
e_GAD_Shape_Type GAD_Shape_Type_From_Message
(
   s_GAD_Message *p_GAD_Message  ///< [in] Pointer to GAD message
                                 ///     containing a shape.
)
{
   e_GAD_Shape_Type ShapeType;

   // remove 'spare' nibble from first octet and shift shape type to lower nibble.
   ShapeType = (e_GAD_Shape_Type) ((p_GAD_Message->message[0]) >> 4);
   /// The switch statement central to this function relies on the enum values
   /// in #e_GAD_Shape_Type matching the values in the GAD document.
   switch (ShapeType)
   {
   case GN_GAD_Ellipsoid_Point:
   case GN_GAD_Ellipsoid_point_with_uncertainty_Circle:
   case GN_GAD_Ellipsoid_point_with_uncertainty_Ellipse:
   case GN_GAD_Polygon:
   case GN_GAD_Ellipsoid_point_with_altitude:
   case GN_GAD_Ellipsoid_point_with_altitude_and_uncertainty_Ellipsoid:
   case GN_GAD_Ellipsoid_Arc:
      return ShapeType;
      break;
   case GN_GAD_Unknown:
   default:
      return GN_GAD_Unknown;
      break;
   }
}

//*****************************************************************************
/// \brief
///      Internal function for obtaining positions of elements based on a
///      Shape Type.
///
/// \returns
///      Boolean indicating whether the element positions are valid
//*****************************************************************************
BL GN_GAD_Element_Position_From_Shape_Type
(
   e_GAD_Shape_Type        ShapeType,              ///< [in] Shape type.
   s_GAD_Element_Position *p_GAD_Element_Position  ///< Pointer to structure containing
                                                   ///  positions of elements.
)
{
   // First mark everything as invalid. i.e. at octet 0.
   p_GAD_Element_Position->BufferSize  = 0;
   p_GAD_Element_Position->Latitude    = 0;
   p_GAD_Element_Position->Longitude   = 0;
   p_GAD_Element_Position->Altitude    = 0;
   p_GAD_Element_Position->Uncertainty_Semi_Major  = 0;
   p_GAD_Element_Position->Uncertainty_Semi_Minor  = 0;
   p_GAD_Element_Position->Orientation_Of_Major_Axis = 0;
   p_GAD_Element_Position->Uncertainty_Altitude    = 0;
   p_GAD_Element_Position->Confidence  = 0;

   switch (ShapeType)
   {
   case GN_GAD_Ellipsoid_Point:
      p_GAD_Element_Position->BufferSize  = 7;
      p_GAD_Element_Position->Latitude    = 2;   // Octet 2.
      p_GAD_Element_Position->Longitude   = 5;   // Octet 5.
      break;
   case GN_GAD_Ellipsoid_point_with_uncertainty_Circle:
      p_GAD_Element_Position->BufferSize  = 8;
      p_GAD_Element_Position->Latitude    = 2;   // Octet 2.
      p_GAD_Element_Position->Longitude   = 5;   // Octet 5.
      // For a circle the semi major equals the semi minor.
      p_GAD_Element_Position->Uncertainty_Semi_Major = 8;   // Octet 8.
      p_GAD_Element_Position->Uncertainty_Semi_Minor = 8;   // Octet 8.
      break;
   case GN_GAD_Ellipsoid_point_with_uncertainty_Ellipse:
      p_GAD_Element_Position->BufferSize  = 11;
      p_GAD_Element_Position->Latitude    = 2;   // Octet 2.
      p_GAD_Element_Position->Longitude   = 5;   // Octet 5.
      p_GAD_Element_Position->Uncertainty_Semi_Major = 8;   // Octet 8.
      p_GAD_Element_Position->Uncertainty_Semi_Minor = 9;   // Octet 9.
      p_GAD_Element_Position->Orientation_Of_Major_Axis = 10;   // Octet 10.
      p_GAD_Element_Position->Confidence  = 11;   // Octet 11.
      break;
   case GN_GAD_Polygon:        // There are no plans to implement this type.
      return FALSE;
      break;
   case GN_GAD_Ellipsoid_point_with_altitude:
      p_GAD_Element_Position->BufferSize  = 9;
      p_GAD_Element_Position->Latitude    = 2;   // Octet 2.
      p_GAD_Element_Position->Longitude   = 5;   // Octet 5.
      p_GAD_Element_Position->Altitude    = 8;   // Octet 8.
      break;
   case GN_GAD_Ellipsoid_point_with_altitude_and_uncertainty_Ellipsoid:
      p_GAD_Element_Position->BufferSize  = 14;
      p_GAD_Element_Position->Latitude    = 2;   // Octet 2.
      p_GAD_Element_Position->Longitude   = 5;   // Octet 5.
      p_GAD_Element_Position->Altitude    = 8;   // Octet 8.
      p_GAD_Element_Position->Uncertainty_Semi_Major  = 10;   // Octet 10.
      p_GAD_Element_Position->Uncertainty_Semi_Minor  = 11;   // Octet 11.
      p_GAD_Element_Position->Orientation_Of_Major_Axis = 12;   // Octet 12.
      p_GAD_Element_Position->Uncertainty_Altitude    = 13;   // Octet 13.
      p_GAD_Element_Position->Confidence  = 14;   // Octet 14.
      break;
   case GN_GAD_Ellipsoid_Arc:  // There are no plans to implement this type.
      return FALSE;
      break;
   case GN_GAD_Unknown:
   default:
      return FALSE;
      break;
   }
   return TRUE;
}

//*****************************************************************************
/// \brief
///      Externally accessible function for extracting GN_AGPS_Pos from a GAD
///      message.
///
/// \returns
///      Boolean indicating whether the conversion was successful.
//*****************************************************************************
BL GN_AGPS_Pos_From_GAD_Message
(
   s_GAD_Message *p_GAD_Message, ///< [in] Pointer to a structure containing a
                                 ///     GAD message.
   s_GN_AGPS_Ref_Pos *p_GN_AGPS_Ref_Pos  ///< [out] Pointer to a structure containing
                                 ///     an AGPS position.
)
{
   e_GAD_Shape_Type ShapeType = GAD_Shape_Type_From_Message(p_GAD_Message);

   // Zero means not present.
   s_GAD_Element_Position Position_Of = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };

   BL MessageOK = GN_GAD_Element_Position_From_Shape_Type(ShapeType, &Position_Of);

   U1 Confidence = 0; /// \todo Do we need to modify the RMS values dependent on confidence?

   // Check to see if buffer in GAD message is big enough for the interpreted shape.
   if (p_GAD_Message->buflen < Position_Of.BufferSize)
   {
      return FALSE;
   }

   p_GN_AGPS_Ref_Pos->Height = 0;
   p_GN_AGPS_Ref_Pos->Height_OK = FALSE;
   p_GN_AGPS_Ref_Pos->Latitude = 0.0;
   p_GN_AGPS_Ref_Pos->Longitude = 0.0;
   p_GN_AGPS_Ref_Pos->RMS_Height = -1;
   p_GN_AGPS_Ref_Pos->RMS_SMaj = -1;
   p_GN_AGPS_Ref_Pos->RMS_SMin = -1;
   p_GN_AGPS_Ref_Pos->RMS_SMajBrg = 0;

   if (Position_Of.Latitude > 0)
   {
      MessageOK &= GN_GAD_Latitude_from_Message_Field(
                     &p_GN_AGPS_Ref_Pos->Latitude,
                     &p_GAD_Message->message[Position_Of.Latitude - 1]);
   }
   if (Position_Of.Longitude > 0)
   {
      MessageOK &= GN_GAD_Longitude_from_Message_Field(
                     &p_GN_AGPS_Ref_Pos->Longitude,
                     &p_GAD_Message->message[Position_Of.Longitude - 1]);
   }
   if (Position_Of.Altitude > 0)
   {
      MessageOK &= GN_GAD_Altitude_from_Message_Field(
                     &p_GN_AGPS_Ref_Pos->Height,
                     &p_GAD_Message->message[Position_Of.Altitude - 1]);
      p_GN_AGPS_Ref_Pos->Height_OK = TRUE;
   }
   if (Position_Of.Uncertainty_Semi_Major > 0)
   {
      MessageOK &= GN_GAD_Uncertainty_from_Message_Field(
                     &p_GN_AGPS_Ref_Pos->RMS_SMaj,
                     &p_GAD_Message->message[Position_Of.Uncertainty_Semi_Major - 1]);
   }
   if (Position_Of.Uncertainty_Semi_Minor > 0)
   {
      MessageOK &= GN_GAD_Uncertainty_from_Message_Field(
                     &p_GN_AGPS_Ref_Pos->RMS_SMin,
                     &p_GAD_Message->message[Position_Of.Uncertainty_Semi_Minor- 1]);
   }
   if (Position_Of.Orientation_Of_Major_Axis > 0)
   {
      MessageOK &= GN_GAD_Orientation_from_Message_Field(
                     &p_GN_AGPS_Ref_Pos->RMS_SMajBrg,
                     &p_GAD_Message->message[Position_Of.Orientation_Of_Major_Axis - 1]);
   }
   if (Position_Of.Uncertainty_Altitude > 0)
   {
      MessageOK &= GN_GAD_Uncertainty_Altitude_from_Message_Field(
                     &p_GN_AGPS_Ref_Pos->RMS_Height,
                     &p_GAD_Message->message[Position_Of.Uncertainty_Altitude - 1]);
   }
   if (Position_Of.Confidence > 0)
   {
      MessageOK &= GN_GAD_Confidence_from_Message_Field(
                     &Confidence,
                     &p_GAD_Message->message[Position_Of.Confidence - 1]);
   }
   return MessageOK;
}

//*****************************************************************************
/// \brief
///      Externally accessible function for extracting s_GN_AGPS_GAD_Data from a GAD
///      message.
///
/// \returns
///      Boolean indicating whether the conversion was successful.
//*****************************************************************************
BL GN_AGPS_GAD_Data_From_GAD_Message
(
   s_GAD_Message *p_GAD_Message,          ///< [in] Pointer to a structure containing a
                                          ///     GAD message.
   s_GN_AGPS_GAD_Data *p_GN_AGPS_GAD_Data ///< [out] Pointer to a structure containing
                                          ///     an AGPS in GAD Data format position.
)
{
   e_GAD_Shape_Type ShapeType = GAD_Shape_Type_From_Message(p_GAD_Message);

   // Zero means not present.
   s_GAD_Element_Position Position_Of = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };

   BL MessageOK = GN_GAD_Element_Position_From_Shape_Type(ShapeType, &Position_Of);

   // Check to see if buffer in GAD message is big enough for the interpreted shape.
   if (p_GAD_Message->buflen < Position_Of.BufferSize)
   {
      return FALSE;
   }
   //typedef struct //GN_AGPS_GAD_Data
   //{
   //   U1 latitudeSign;           // Latitude sign [0=North, 1=South]
   //   U4 latitude;               // Latitude      [range 0..8388607 for 0..90 degrees]
   //   I4 longitude;              // Longitude     [range -8388608..8388607 for -180..+180 degrees]
   //   U2 altitude;               // Altitude            [range 0..32767 metres]

   p_GN_AGPS_GAD_Data->altitudeDirection = -1;     //   I1 altitudeDirection;      // Altitude Direction  [0=Height, 1=Depth,     -1=Not present]
   p_GN_AGPS_GAD_Data->uncertaintySemiMajor = 255; //   U1 uncertaintySemiMajor;   // Horizontal Uncertainty Semi-Major Axis [range 0..127, 255=Unknown]
   p_GN_AGPS_GAD_Data->uncertaintySemiMinor = 255; //   U1 uncertaintySemiMinor;   // Horizontal Uncertainty Semi-Minor Axis [range 0..127, 255=Unknown]
   p_GN_AGPS_GAD_Data->orientationMajorAxis = 255; //   U1 orientationMajorAxis;   // Orientation of the Semi-Major Axis [range 0..89 x 2 degrees, 255=Unknown]
   p_GN_AGPS_GAD_Data->uncertaintyAltitude = 255;  //   U1 uncertaintyAltitude;    // Altitude Uncertainty  [range 0..127,  255=Unknown]
   p_GN_AGPS_GAD_Data->confidence = 255;           //   U1 confidence;             // Position Confidence   [range 1..100%, 255=Unknown]

   p_GN_AGPS_GAD_Data->verdirect = -1;             //   I1 verdirect;              // Vertical direction [0=Upwards, 1=Downwards, -1=Not present]
   p_GN_AGPS_GAD_Data->bearing = -1;               //   I2 bearing;                // Bearing            [range 0..359 degrees,   -1=Not present]
   p_GN_AGPS_GAD_Data->horspeed = -1;              //   I2 horspeed;               // Horizontal Speed   [range 0..32767 km/hr,   -1=Not present]
   p_GN_AGPS_GAD_Data->verspeed = -1;              //   I1 verspeed;               // Vertical Speed     [range 0..127 km/hr,     -1=Not present]
   p_GN_AGPS_GAD_Data->horuncertspeed = 255;       //   U1 horuncertspeed;         // Horizontal Speed Uncertainty [range 0..254 km/hr, 255=Unknown]
   p_GN_AGPS_GAD_Data->veruncertspeed = 255;       //   U1 veruncertspeed;         // Vertical Speed Uncertainty   [range 0..254 km/hr, 255=Unknown]

   //}  s_GN_AGPS_GAD_Data;           // GN A-GPS GAD (Geographical Area Description)


   if (Position_Of.Latitude > 0)
   {
      //   U1 latitudeSign;           // Latitude sign [0=North, 1=South]
      //   U4 latitude;               // Latitude      [range 0..8388607 for 0..90 degrees]
      U1 *p_Latitude_Index = &p_GAD_Message->message[Position_Of.Latitude - 1];
      U4 Temp_Latitude;

      Temp_Latitude = (( p_Latitude_Index[0] ) << 16 |
                       ( p_Latitude_Index[1] ) <<  8 |
                       ( p_Latitude_Index[2] )) ;
      p_GN_AGPS_GAD_Data->latitudeSign = (Temp_Latitude & GAD_LATITUDE_SIGN_MASK) != 0 ? 1 : 0;
      p_GN_AGPS_GAD_Data->latitude     =  Temp_Latitude & GAD_LATITUDE_RANGE_MASK;
   }
   if (Position_Of.Longitude > 0)
   {
      //   I4 longitude;              // Longitude     [range -8388608..8388607 for -180..+180 degrees]
      U1 *p_Longitude_Index = &p_GAD_Message->message[Position_Of.Longitude - 1];

      p_GN_AGPS_GAD_Data->longitude = (  ((I1) p_Longitude_Index[0] )<<16 |
                                             ( p_Longitude_Index[1] )<<8  |
                                             ( p_Longitude_Index[2] ));
   }
   if (Position_Of.Altitude > 0)
   {
      //   U2 altitude;               // Altitude            [range 0..32767 metres]
      //   I1 altitudeDirection;      // Altitude Direction  [0=Height, 1=Depth,     -1=Not present]
      U1 *p_Altitude_Index = &p_GAD_Message->message[Position_Of.Altitude - 1];
      U2 Temp_Altitude;

      Temp_Altitude = ( (p_Altitude_Index[0]<<8) | (p_Altitude_Index[1]) ) ;
      p_GN_AGPS_GAD_Data->altitudeDirection = (Temp_Altitude & GAD_ALTITUDE_DIRECTION_MASK) != 0 ? 1 : 0;
      p_GN_AGPS_GAD_Data->altitude = (Temp_Altitude & GAD_ALTITUDE_RANGE_MASK);

   }
   if (Position_Of.Uncertainty_Semi_Major > 0)
   {
      //   U1 uncertaintySemiMajor;   // Horizontal Uncertainty Semi-Major Axis [range 0..127, 255=Unknown]
      p_GN_AGPS_GAD_Data->uncertaintySemiMajor = p_GAD_Message->message[Position_Of.Uncertainty_Semi_Major - 1];

   }
   if (Position_Of.Uncertainty_Semi_Minor > 0)
   {
      //   U1 uncertaintySemiMinor;   // Horizontal Uncertainty Semi-Minor Axis [range 0..127, 255=Unknown]
      p_GN_AGPS_GAD_Data->uncertaintySemiMinor = p_GAD_Message->message[Position_Of.Uncertainty_Semi_Minor - 1];
   }
   if (Position_Of.Orientation_Of_Major_Axis > 0)
   {
      //   U1 orientationMajorAxis;   // Orientation of the Semi-Major Axis [range 0..89 x 2 degrees, 255=Unknown]
      p_GN_AGPS_GAD_Data->orientationMajorAxis = p_GAD_Message->message[Position_Of.Orientation_Of_Major_Axis - 1];
   }
   if (Position_Of.Uncertainty_Altitude > 0)
   {
      //   U1 uncertaintyAltitude;    // Altitude Uncertainty  [range 0..127,  255=Unknown]
      p_GN_AGPS_GAD_Data->uncertaintyAltitude = p_GAD_Message->message[Position_Of.Uncertainty_Altitude - 1];
   }
   if (Position_Of.Confidence > 0)
   {
      //   U1 confidence;             // Position Confidence   [range 1..100%, 255=Unknown]
      p_GN_AGPS_GAD_Data->confidence = p_GAD_Message->message[Position_Of.Confidence - 1];
   }
   return MessageOK;
}

//*****************************************************************************
/// \brief
///      Internal function for extracting a Latitude from a GAD
///      message.
///
/// \returns
///      Boolean indicating whether the Latitude was valid.
//*****************************************************************************
BL GN_GAD_Latitude_from_Message_Field
(
   R4 *p_Latitude,         ///< [out] Latitude extracted from the GAD message.
   U1 p_Message_Field[3]   ///< [in] Pointer to the Latitude position within
                           ///     the GAD message
)
{
   BL Latitude_South_Of_Equator;
   U1 *p_Latitude_Index = p_Message_Field;
   U4 Temp_Latitude;
   R4 Temp_R_Latitude;

   Temp_Latitude = ((p_Latitude_Index[0])<<16 |
                     (p_Latitude_Index[1])<<8 |
                      p_Latitude_Index[2]) ;
   Latitude_South_Of_Equator = (Temp_Latitude & GAD_LATITUDE_SIGN_MASK) != 0;
   Temp_R_Latitude = (R4) (Temp_Latitude & GAD_LATITUDE_RANGE_MASK) /
                           GAD_LATITUDE_SCALING_FACTOR;
   Temp_R_Latitude *= (R4) GAD_LATITUDE_DEGREES_IN_RANGE *
                           (Latitude_South_Of_Equator ? -1 : 1);
   *p_Latitude = Temp_R_Latitude;
   return TRUE;
}

//*****************************************************************************
/// \brief
///      Internal function for extracting a Longitude from a GAD
///      message.
///
/// \returns
///      Boolean indicating whether the Longitude was valid.
//*****************************************************************************
BL GN_GAD_Longitude_from_Message_Field
(
   R4 *p_Longitude,        ///< [out] Longitude extracted from the GAD message.
   U1 p_Message_Field[3]   ///< [in] Pointer to the Longitude position within
                           ///     the GAD message
)
{
   U1 *p_Longitude_Index = p_Message_Field;
   I4 Temp_Longitude;
   R4 Temp_R_Longitude;

   Temp_Longitude = (((I1)  p_Longitude_Index[0])<<16 |
                           (p_Longitude_Index[1])<<8  |
                            p_Longitude_Index[2]);
   Temp_R_Longitude = (R4) Temp_Longitude / (R4) GAD_LONGITUDE_SCALING_FACTOR;
   Temp_R_Longitude *= (R4) GAD_LONGITUDE_DEGREES_IN_RANGE;
   *p_Longitude = Temp_R_Longitude;
   return TRUE;
}

//*****************************************************************************
/// \brief
///      Internal function for extracting an Uncertainty from a GAD
///      message.
///
/// \returns
///      Boolean indicating whether the Uncertainty was valid.
//*****************************************************************************
BL GN_GAD_Uncertainty_from_Message_Field
(
   R4 *p_Uncertainty,      ///< [out] Uncertainty extracted from the GAD message.
   U1 p_Message_Field[1]   ///< [in] Pointer to the Uncertainty within
                           ///     the GAD message
)
{
    U1 *p_Uncertainty_Index = p_Message_Field;

    *p_Uncertainty = (R4) GAD_BINOMIAL_UNCERTAINTY_FROM_K(
                              GAD_UNCERTAINTY_C,
                              GAD_UNCERTAINTY_x,
                              (*p_Uncertainty_Index & GAD_UNCERTAINTY_MASK));
    return TRUE;
}

//*****************************************************************************
/// \brief
///      Internal function for extracting an Orientation from a GAD
///      message.
///
/// \returns
///      Boolean indicating whether the Orientation was valid.
//*****************************************************************************
BL GN_GAD_Orientation_from_Message_Field
(
   I2 *p_RMS_SMajBrg,      ///< [out] Orientation extracted from the GAD message.
   U1 p_Message_Field[1]   ///< [in] Pointer to the Orientation within
                           ///     the GAD message
)
{
   U1 *p_RMS_SMajBrg_Index = p_Message_Field;

   if (*p_RMS_SMajBrg_Index > GAD_ORIENTATION_ANGLE_INCREMENT_MAX)
   {
      // Bearing must be less than 179 before conversion.
      return FALSE;
   }
   else
   {
      *p_RMS_SMajBrg = (I2) *p_RMS_SMajBrg_Index * GAD_DEGREES_INCREMENT;
   }
   return TRUE;
}

//*****************************************************************************
/// \brief
///      Internal function for extracting an Altitude from a GAD
///      message.
///
/// \returns
///      Boolean indicating whether the Altitude was valid.
//*****************************************************************************
BL GN_GAD_Altitude_from_Message_Field
(
   R4 *p_Altitude,         ///< [out] Altitude extracted from the GAD message.
   U1 p_Message_Field[2]   ///< [in] Pointer to the Altitude within
                           ///     the GAD message
)
{
   BL Below_WGS84_Ellipsoid;
   U1 *p_Altitude_Index = p_Message_Field;
   U2 Temp_Altitude;
   R4 Temp_R_Altitude;

   Temp_Altitude = ( (p_Altitude_Index[0]<<8) | (p_Altitude_Index[1]) ) ;
   Below_WGS84_Ellipsoid = (Temp_Altitude & GAD_ALTITUDE_DIRECTION_MASK) != 0;
   Temp_R_Altitude = (R4) (Temp_Altitude & GAD_ALTITUDE_RANGE_MASK) *
                              (Below_WGS84_Ellipsoid ? -1 : 1) ;
   *p_Altitude = Temp_R_Altitude;
   return TRUE;
}

//*****************************************************************************
/// \brief
///      Internal function for extracting an Uncertainty Altitude from a GAD
///      message.
///
/// \returns
///      Boolean indicating whether the Uncertainty Altitude was valid.
//*****************************************************************************
BL GN_GAD_Uncertainty_Altitude_from_Message_Field
(
   R4 *p_RMS_Altitude,     ///< [out] Uncertainty Altitude extracted from the
                           ///   GAD message.
   U1 p_Message_Field[1]   ///< [in] Pointer to the Uncertainty Altitude within
                           ///   the GAD message
)
{
   U1 *p_Uncertainty_Altitude_Index = p_Message_Field;

   *p_RMS_Altitude = (R4) GAD_BINOMIAL_UNCERTAINTY_FROM_K(
                              GAD_UNCERTAINTY_ALTITUDE_C,
                              GAD_UNCERTAINTY_ALTITUDE_x,
                              (*p_Uncertainty_Altitude_Index &
                               GAD_UNCERTAINTY_ALTITUDE_MASK));
   return TRUE;
}

//*****************************************************************************
/// \brief
///      Internal function for extracting a Confidence from a GAD
///      message.
///
/// \returns
///      Boolean indicating whether the Confidence was valid.
//*****************************************************************************
BL GN_GAD_Confidence_from_Message_Field
(
   U1 *p_Confidence,       ///< [out] Confidence extracted from the GAD message.
   U1 p_Message_Field[1]   ///< [in] Pointer to the Confidence within
                           ///   the GAD message
)
{
   U1 Temp_Confidence = p_Message_Field[0];
   if (Temp_Confidence > 100)
   {
      // For values where confidence is greater than 100 or confidence is 0
      // the interpretation in the GAD message is 'unknown'.
      Temp_Confidence = 0;
   }
   *p_Confidence = Temp_Confidence;
   return TRUE;
}

//*****************************************************************************
/// \brief
///      Externally accessible function for encoding a GAD message from a
///      GN_AGPS_Pos.
///
/// \returns
///      Boolean indicating whether the conversion was successful.
//*****************************************************************************
BL GN_GAD_Message_From_AGPS_Pos
(
   s_GN_AGPS_Ref_Pos *p_GN_AGPS_Ref_Pos, ///< [in] Pointer to a structure containing
                                 ///   an AGPS position.
   s_GAD_Message *p_GAD_Message  ///< [out] Pointer to a structure containing a
                                 ///   GAD message.
)
{
   // Zero means not present.
   s_GAD_Element_Position Position_Of = { 0, 0, 0, 0, 0, 0, 0, 0, 0};

   /// We always convert a GPS position estimate into an
   /// Ellipsoid point with altitude and uncertainty ellipsoid message.
   e_GAD_Shape_Type ShapeType =
      GN_GAD_Ellipsoid_point_with_altitude_and_uncertainty_Ellipsoid;

   BL Status = GN_GAD_Element_Position_From_Shape_Type(ShapeType, &Position_Of);

   if (p_GAD_Message->buflen < Position_Of.BufferSize)
   {
      return FALSE;
   }
   p_GAD_Message->message[0] = (U1) ShapeType << 4;
   Status &= GN_GAD_Message_Field_from_Latitude(
               &p_GAD_Message->message[1],
               p_GN_AGPS_Ref_Pos->Latitude);
   Status &= GN_GAD_Message_Field_from_Longitude(
               &p_GAD_Message->message[4],
               p_GN_AGPS_Ref_Pos->Longitude);
   Status &= GN_GAD_Message_Field_from_Altitude(
               &p_GAD_Message->message[7],
               p_GN_AGPS_Ref_Pos->Height);
   Status &= GN_GAD_Message_Field_from_Uncertainty(
               &p_GAD_Message->message[9],
               p_GN_AGPS_Ref_Pos->RMS_SMaj);
   Status &= GN_GAD_Message_Field_from_Uncertainty(
               &p_GAD_Message->message[10],
               p_GN_AGPS_Ref_Pos->RMS_SMin);
   Status &= GN_GAD_Message_Field_from_Orientation(
               &p_GAD_Message->message[11],
               p_GN_AGPS_Ref_Pos->RMS_SMajBrg);
   Status &= GN_GAD_Message_Field_from_Uncertainty_Altitude(
               &p_GAD_Message->message[12],
               p_GN_AGPS_Ref_Pos->RMS_Height);
   Status &= GN_GAD_Message_Field_from_Confidence(
               &p_GAD_Message->message[13],
               GAD_SIGMA_ONE_CONFIDENCE);
   return Status;
}

//*****************************************************************************
/// \brief
///      Externally accessible function for encoding a GAD message from a
///      GN_AGPS_Pos.
///
/// \returns
///      Boolean indicating whether the conversion was successful.
//*****************************************************************************
BL GN_GAD_Message_From_AGPS_GAD_Data
(
   s_GN_AGPS_GAD_Data *p_GN_AGPS_GAD_Data, ///< [in] Pointer to a structure containing
                                 ///   an AGPS position.
   s_GAD_Message *p_GAD_Message  ///< [out] Pointer to a structure containing a
                                 ///   GAD message.
)
{
   // Zero means not present.
   s_GAD_Element_Position Position_Of = { 0, 0, 0, 0, 0, 0, 0, 0, 0};

   /// We always convert a GPS position estimate into an
   /// Ellipsoid point with altitude and uncertainty ellipsoid message.
   e_GAD_Shape_Type ShapeType =
      GN_GAD_Ellipsoid_point_with_altitude_and_uncertainty_Ellipsoid;

   BL Status = GN_GAD_Element_Position_From_Shape_Type(ShapeType, &Position_Of);

   if (p_GAD_Message->buflen < Position_Of.BufferSize)
   {
      return FALSE;
   }
   p_GAD_Message->message[0] = (U1) ShapeType << 4;
   Status &= GN_GAD_Message_Field_from_GAD_Latitude(
               &p_GAD_Message->message[1],
               p_GN_AGPS_GAD_Data->latitude,
               p_GN_AGPS_GAD_Data->latitudeSign);
   Status &= GN_GAD_Message_Field_from_GAD_Longitude(
               &p_GAD_Message->message[4],
               p_GN_AGPS_GAD_Data->longitude);
   Status &= GN_GAD_Message_Field_from_GAD_Altitude(
               &p_GAD_Message->message[7],
               p_GN_AGPS_GAD_Data->altitude,
               p_GN_AGPS_GAD_Data->altitudeDirection);
   Status &= GN_GAD_Message_Field_from_GAD_Uncertainty(
               &p_GAD_Message->message[9],
               p_GN_AGPS_GAD_Data->uncertaintySemiMajor);
   Status &= GN_GAD_Message_Field_from_GAD_Uncertainty(
               &p_GAD_Message->message[10],
               p_GN_AGPS_GAD_Data->uncertaintySemiMinor);
   Status &= GN_GAD_Message_Field_from_GAD_Orientation(
               &p_GAD_Message->message[11],
               (U1) p_GN_AGPS_GAD_Data->orientationMajorAxis);
   Status &= GN_GAD_Message_Field_from_GAD_Uncertainty_Altitude(
               &p_GAD_Message->message[12],
               (U1) p_GN_AGPS_GAD_Data->uncertaintyAltitude);
   Status &= GN_GAD_Message_Field_from_Confidence(
               &p_GAD_Message->message[13],
               GAD_SIGMA_ONE_CONFIDENCE);
   return Status;
}

//*****************************************************************************
/// \brief
///      Internal function for Encoding a Latitude into a GAD
///      message.
///
/// \returns
///      Boolean indicating whether the Encode was successful.
//*****************************************************************************
BL GN_GAD_Message_Field_from_Latitude
(
   U1 p_Message_Field[3],  ///< [in] Pointer to the Latitude field within
                           ///  the GAD message
   R4 Latitude             ///< [in] Latitude to be encoded into the GAD message.
)
{
   R4 Temp_R_Latitude = Latitude;
   U4 Temp_Latitude;
   U4 Latitude_Sign = 0x000000; // Bit 23 set to 1 = south, bit 23 set to 0 = north.

   if (Temp_R_Latitude < 0.0)
   {
      Temp_R_Latitude *= -1;  // Code the range as a positive value.
      Latitude_Sign = GAD_LATITUDE_SIGN_MASK; // Bit 23 set to 1 = south.
   }
   Temp_Latitude = (U4) ((Temp_R_Latitude /
                           GAD_LATITUDE_DEGREES_IN_RANGE) *
                          GAD_LATITUDE_SCALING_FACTOR);
   Temp_Latitude |= Latitude_Sign;
   p_Message_Field[0] = (U1) ((Temp_Latitude >> 16) & 0xff);
   p_Message_Field[1] = (U1) ((Temp_Latitude >>  8) & 0xff);
   p_Message_Field[2] = (U1) ((Temp_Latitude      ) & 0xff);

   return TRUE;
}

//*****************************************************************************
/// \brief
///      Internal function for Encoding a Latitude into a GAD
///      message.
///
/// \returns
///      Boolean indicating whether the Encode was successful.
//*****************************************************************************
BL GN_GAD_Message_Field_from_GAD_Latitude
(
   U1 p_Message_Field[3],  ///< [in/out] Pointer to the Latitude field within
                           ///  the GAD message
   U4 Latitude,            ///< [in] Latitude to be encoded into the GAD message.
   U1 Latitude_Sign        ///< [in] Sign of Latitude to be encoded into the GAD message.
)
{
   U4 Temp_Latitude = Latitude;

   if (Latitude_Sign != 0)
   {
      Temp_Latitude |= GAD_LATITUDE_SIGN_MASK; // Bit 23 set to 1 = south.
   }
   p_Message_Field[0] = (U1) ((Temp_Latitude >> 16) & 0xff);
   p_Message_Field[1] = (U1) ((Temp_Latitude >>  8) & 0xff);
   p_Message_Field[2] = (U1) ((Temp_Latitude      ) & 0xff);

   return TRUE;
}

//*****************************************************************************
/// \brief
///      Internal function for Encoding a Longitude into a GAD
///      message.
///
/// \returns
///      Boolean indicating whether the Encode was successful.
//*****************************************************************************
BL GN_GAD_Message_Field_from_Longitude
(
   U1 p_Message_Field[3],  ///< [in] Pointer to the Longitude field within
                           ///  the GAD message
   R4 Longitude            ///< [in] Longitude to be encoded into the GAD message.
)
{
   I4 Temp_Longitude;

   Temp_Longitude = (I4) ((Longitude / GAD_LONGITUDE_DEGREES_IN_RANGE) * GAD_LONGITUDE_SCALING_FACTOR);
   p_Message_Field[0] = (U1) ((Temp_Longitude >> 16) & 0xff);
   p_Message_Field[1] = (U1) ((Temp_Longitude >>  8) & 0xff);
   p_Message_Field[2] = (U1) ((Temp_Longitude      ) & 0xff);

   return TRUE;
}

//*****************************************************************************
/// \brief
///      Internal function for Encoding a Longitude into a GAD
///      message.
///
/// \returns
///      Boolean indicating whether the Encode was successful.
//*****************************************************************************
BL GN_GAD_Message_Field_from_GAD_Longitude
(
   U1 p_Message_Field[3],  ///< [in] Pointer to the Longitude field within
                           ///  the GAD message
   U4 Longitude            ///< [in] Longitude to be encoded into the GAD message.
)
{
   p_Message_Field[0] = (U1) ((Longitude >> 16) & 0xff);
   p_Message_Field[1] = (U1) ((Longitude >>  8) & 0xff);
   p_Message_Field[2] = (U1) ((Longitude      ) & 0xff);

   return TRUE;
}

//*****************************************************************************
/// \brief
///      Internal function for Encoding an Uncertainty into a GAD
///      message.
///
/// \returns
///      Boolean indicating whether the Encode was successful.
//*****************************************************************************
BL GN_GAD_Message_Field_from_Uncertainty
(
   U1 p_Message_Field[1],  ///< [in] Pointer to the Uncertainty field within
                           ///  the GAD message.
   R4 RMS_Uncertainty      ///< [in] Uncertainty to be encoded into the GAD message.
)
{
   U1 K;
   R4 Temp_R_K;

   Temp_R_K = (R4) GAD_BINOMIAL_K_FROM_UNCERTAINTY(
                     RMS_Uncertainty,
                     GAD_UNCERTAINTY_C,
                     GAD_UNCERTAINTY_x);
   K = (U1) Temp_R_K;
   if (K > GAD_UNCERTAINTY_MASK)
   {
      return FALSE;
   }
   p_Message_Field[0] = K & GAD_UNCERTAINTY_MASK;

   return TRUE;
}

//*****************************************************************************
/// \brief
///      Internal function for Encoding an Uncertainty into a GAD
///      message.
///
/// \returns
///      Boolean indicating whether the Encode was successful.
//*****************************************************************************
BL GN_GAD_Message_Field_from_GAD_Uncertainty
(
   U1 p_Message_Field[1],  ///< [in] Pointer to the Uncertainty field within
                           ///  the GAD message.
   U1 RMS_Uncertainty      ///< [in] Uncertainty to be encoded into the GAD message.
)
{
   p_Message_Field[0] = RMS_Uncertainty;

   return TRUE;
}

//*****************************************************************************
/// \brief
///      Internal function for Encoding an Orientation of the Semi Major
///      Uncertainty into a GAD message.
///
/// \returns
///      Boolean indicating whether the Encode was successful.
//*****************************************************************************
BL GN_GAD_Message_Field_from_Orientation
(
   U1 p_Message_Field[1],  ///< [in] Pointer to the Orientation field within
                           ///  the GAD message.
   I2 RMS_SMajBrg          ///< [in] Orientation to be encoded into the GAD message.
)
{
   U1 Bearing;

   if (RMS_SMajBrg >= GAD_ORIENTATION_ANGLE_DEGREES_MAX)
   {
      return FALSE;
   }
   else if (RMS_SMajBrg <= -GAD_ORIENTATION_ANGLE_DEGREES_MAX)
   {
      return FALSE;
   }
   else if (RMS_SMajBrg < 0)
   {
      Bearing = (U1) (((I2) GAD_ORIENTATION_ANGLE_DEGREES_MAX - RMS_SMajBrg) /
                              GAD_DEGREES_INCREMENT);
   }
   else
   {
      Bearing = (U1) ((RMS_SMajBrg) / GAD_DEGREES_INCREMENT);
   }
   p_Message_Field[0] = (U1) Bearing;

   return TRUE;
}

//*****************************************************************************
/// \brief
///      Internal function for Encoding an Orientation of the Semi Major
///      Uncertainty into a GAD message.
///
/// \returns
///      Boolean indicating whether the Encode was successful.
//*****************************************************************************
BL GN_GAD_Message_Field_from_GAD_Orientation
(
   U1 p_Message_Field[1],  ///< [in] Pointer to the Orientation field within
                           ///  the GAD message.
   U1 OrientationMajorAxis ///< [in] Orientation to be encoded into the GAD message.
)
{
   p_Message_Field[0] = OrientationMajorAxis;

   return TRUE;
}

//*****************************************************************************
/// \brief
///      Internal function for Encoding an Altitude into a GAD message.
///
/// \returns
///      Boolean indicating whether the Encode was successful.
//*****************************************************************************
BL GN_GAD_Message_Field_from_Altitude
(
   U1 p_Message_Field[2],  ///< [in] Pointer to the Altitude field within
                           ///      the GAD message.
   R4 Altitude             ///< [in] Altitude to be encoded into the GAD message.
)
{
   BL Below_WGS84_Ellipsoid;
   U2 Temp_Altitude;
   R4 Temp_R_Altitude;

   if (Altitude < 0)
   {
      Below_WGS84_Ellipsoid = TRUE;
      Temp_R_Altitude = Altitude * -1;
   }
   else
   {
      Below_WGS84_Ellipsoid = FALSE;
      Temp_R_Altitude = Altitude;
   }

   // Will the value fit?
   if (Altitude > GAD_ALTITUDE_RANGE_MASK)
   {
      return FALSE;
   }

   Temp_Altitude = ((U2) Temp_R_Altitude) & GAD_ALTITUDE_RANGE_MASK;
   if (Below_WGS84_Ellipsoid)
   {
      // Set the direction bit in the message.
      Temp_Altitude |= GAD_ALTITUDE_DIRECTION_MASK;   // Below WGS84 ellipsoid msb = 1,
                                                      // above WGS84 ellipsoid msb = 0.
   }
   p_Message_Field[0] = (U1) (Temp_Altitude >> 8);
   p_Message_Field[1] = (U1) (Temp_Altitude & 0xff);

   return TRUE;
}

//*****************************************************************************
/// \brief
///      Internal function for Encoding an Altitude into a GAD message.
///
/// \returns
///      Boolean indicating whether the Encode was successful.
//*****************************************************************************
BL GN_GAD_Message_Field_from_GAD_Altitude
(
   U1 p_Message_Field[2],  ///< [in/out] Pointer to the Altitude field within
                           ///      the GAD message.
   U2 Altitude,            ///< [in] Altitude to be encoded into the GAD message.
   I1 AltitudeDirection    ///< [in] Direction of Altitude to be encoded into the GAD message.
)
{
   U2 Temp_Altitude = Altitude;

   if (AltitudeDirection < 0)
   {
      // Set the direction bit in the message.
      Temp_Altitude |= GAD_ALTITUDE_DIRECTION_MASK;   // Below WGS84 ellipsoid msb = 1,
                                                      // above WGS84 ellipsoid msb = 0.
   }
   p_Message_Field[0] = (U1) (Temp_Altitude >> 8);
   p_Message_Field[1] = (U1) (Temp_Altitude & 0xff);

   return TRUE;
}

//*****************************************************************************
/// \brief
///      Internal function for Encoding an Uncertainty Altitude into a GAD
///      message.
///
/// \returns
///      Boolean indicating whether the Encode was successful.
//*****************************************************************************
BL GN_GAD_Message_Field_from_Uncertainty_Altitude
(
   U1 p_Message_Field[1],  ///< [in] Pointer to the Uncertainty Altitude
                           ///      field within the GAD message.
   R4 RMS_Altitude         ///< [in] Uncertainty Altitude to be encoded into
                           ///      the GAD message.
)
{
   U1 K;
   R4 Temp_R_K;

   Temp_R_K = (R4) GAD_BINOMIAL_K_FROM_UNCERTAINTY(RMS_Altitude,
                                                 GAD_UNCERTAINTY_ALTITUDE_C,
                                                 GAD_UNCERTAINTY_ALTITUDE_x);
   K = (U1) Temp_R_K;
   if (K > GAD_UNCERTAINTY_ALTITUDE_MASK)
   {
      return FALSE;
   }
   p_Message_Field[0] = K & GAD_UNCERTAINTY_ALTITUDE_MASK;

   return TRUE;
}

//*****************************************************************************
/// \brief
///      Internal function for Encoding an Uncertainty Altitude into a GAD
///      message.
///
/// \returns
///      Boolean indicating whether the Encode was successful.
//*****************************************************************************
BL GN_GAD_Message_Field_from_GAD_Uncertainty_Altitude
(
   U1 p_Message_Field[1],  ///< [in] Pointer to the Uncertainty Altitude
                           ///      field within the GAD message.
   U1 RMS_Altitude         ///< [in] Uncertainty Altitude to be encoded into
                           ///      the GAD message.
)
{
   p_Message_Field[0] = RMS_Altitude;

   return TRUE;
}

//*****************************************************************************
/// \brief
///      Internal function for Encoding an Confidence into a GAD
///      message.
///
/// \returns
///      Boolean indicating whether the Encode was successful.
//*****************************************************************************
BL GN_GAD_Message_Field_from_Confidence
(
   U1 p_Message_Field[1],  ///< [in] Pointer to the Confidence field within
                           ///      the GAD message.
   U1 Confidence           ///< [in] Confidence to be encoded into the GAD
                           ///      message.
)
{
   if (Confidence > 100)
   {
      // For values where confidence is greater than 100 or confidence is 0
      // the interpretation in the GAD message is 'unknown'.
      Confidence = 0;
   }
   p_Message_Field[0] = Confidence;
   return TRUE;
}

//*****************************************************************************
/// \brief
///      Internal function for Converting from Nav Data to GAD Data formats.
///
/// \returns
///      Boolean indicating whether the conversion was successful.
//*****************************************************************************
BL GN_AGPS_GAD_Data_From_Nav_Data
(
   s_GN_AGPS_GAD_Data   *p_GN_AGPS_GAD_Data,
   s_GN_GPS_Nav_Data    *p_GN_GPS_Nav_Data
)
{
   if (p_GN_GPS_Nav_Data == NULL)
   {
      return FALSE;
   }
   if (p_GN_GPS_Nav_Data->FixType == GN_GPS_FIX_NONE ||
       p_GN_GPS_Nav_Data->FixType == GN_GPS_FIX_ESTIMATED)
   {
      return FALSE;
   }
   //U1 latitudeSign;           // Latitude sign [0=North, 1=South]
   p_GN_AGPS_GAD_Data->latitudeSign = p_GN_GPS_Nav_Data->Latitude >= 0 ? 0 : 1;
   //U4 latitude;               // Latitude      [range 0..8388607 for 0..90 degrees]
   p_GN_AGPS_GAD_Data->latitude = (U4) (abs((long) p_GN_GPS_Nav_Data->Latitude) / GAD_LATITUDE_DEGREES_IN_RANGE * GAD_LATITUDE_SCALING_FACTOR);
   //I4 longitude;              // Longitude     [range -8388608..8388607 for -180..+180 degrees]
   p_GN_AGPS_GAD_Data->longitude = (I4) ((p_GN_GPS_Nav_Data->Longitude / GAD_LONGITUDE_DEGREES_IN_RANGE) * GAD_LONGITUDE_SCALING_FACTOR);
   //I1 altitudeDirection;      // Altitude Direction  [0=Height, 1=Depth,     -1=Not present]
   p_GN_AGPS_GAD_Data->altitudeDirection = p_GN_GPS_Nav_Data->Altitude_Ell >= 0 ? 0 : 1;
   //U2 altitude;               // Altitude            [range 0..32767 metres]
   p_GN_AGPS_GAD_Data->altitude = (U2) abs((long)p_GN_GPS_Nav_Data->Altitude_Ell);
   //U1 uncertaintySemiMajor;   // Horizontal Uncertainty Semi-Major Axis [range 0..127, 255=Unknown]
   {
      U1 K;
      R4 Temp_R_K;

      Temp_R_K = (R4) GAD_BINOMIAL_K_FROM_UNCERTAINTY(
                        p_GN_GPS_Nav_Data->H_AccMaj,
                        GAD_UNCERTAINTY_C,
                        GAD_UNCERTAINTY_x);
      K = (U1) Temp_R_K;
      if (K > GAD_UNCERTAINTY_MASK)
      {
         p_GN_AGPS_GAD_Data->uncertaintySemiMajor = 255;
      }
      else
      {
         p_GN_AGPS_GAD_Data->uncertaintySemiMajor = K;
      }
   }
   //U1 uncertaintySemiMinor;   // Horizontal Uncertainty Semi-Minor Axis [range 0..127, 255=Unknown]
   {
      U1 K;
      R4 Temp_R_K;

      Temp_R_K = (R4) GAD_BINOMIAL_K_FROM_UNCERTAINTY(
                        p_GN_GPS_Nav_Data->H_AccMin,
                        GAD_UNCERTAINTY_C,
                        GAD_UNCERTAINTY_x);
      K = (U1) Temp_R_K;
      if (K > GAD_UNCERTAINTY_MASK)
      {
         p_GN_AGPS_GAD_Data->uncertaintySemiMinor = 255;
      }
      else
      {
         p_GN_AGPS_GAD_Data->uncertaintySemiMinor = K;
      }
   }
   //U1 orientationMajorAxis;   // Orientation of the Semi-Major Axis [range 0..89 x 2 degrees, 255=Unknown]
   p_GN_AGPS_GAD_Data->orientationMajorAxis = (U1) p_GN_GPS_Nav_Data->H_AccMajBrg/2 ;
   //U1 uncertaintyAltitude;    // Altitude Uncertainty  [range 0..127,  255=Unknown]
   {
      U1 K;
      R4 Temp_R_K;

      Temp_R_K = (R4) GAD_BINOMIAL_K_FROM_UNCERTAINTY(p_GN_GPS_Nav_Data->V_AccEst,
                                                    GAD_UNCERTAINTY_ALTITUDE_C,
                                                    GAD_UNCERTAINTY_ALTITUDE_x);
      K = (U1) Temp_R_K;
      if (K > GAD_UNCERTAINTY_ALTITUDE_MASK)
      {
         p_GN_AGPS_GAD_Data->uncertaintyAltitude = 255;
      }
      else
      {
         p_GN_AGPS_GAD_Data->uncertaintyAltitude= K;
      }

   }
   //U1 confidence;             // Position Confidence   [range 0..100%, 255=Unknown]
   p_GN_AGPS_GAD_Data->confidence = 67; // Sigma 1 == 67%

   //I1 verdirect;              // Vertical direction [0=Upwards, 1=Downwards, -1=Not present]
   p_GN_AGPS_GAD_Data->verdirect = p_GN_GPS_Nav_Data->VerticalVelocity >= 0 ? 0 : 1;
   //I2 bearing;                // Bearing            [range 0..359 degrees,   -1=Not present]
   p_GN_AGPS_GAD_Data->bearing = (I2)p_GN_GPS_Nav_Data->CourseOverGround;
   //I2 horspeed;               // Horizontal Speed   [range 0..32767 km/hr,   -1=Not present]
   p_GN_AGPS_GAD_Data->horspeed = (I2) p_GN_GPS_Nav_Data->SpeedOverGround;
   //I1 verspeed;               // Vertical Speed     [range 0..127 km/hr,     -1=Not present]
   p_GN_AGPS_GAD_Data->verspeed = (I1) p_GN_GPS_Nav_Data->VerticalVelocity;
   //U1 horuncertspeed;         // Horizontal Speed Uncertainty [range 0..254 km/hr, 255=Unknown]
   p_GN_AGPS_GAD_Data->horuncertspeed = 255;
   //U1 veruncertspeed;         // Vertical Speed Uncertainty   [range 0..254 km/hr, 255=Unknown]
   p_GN_AGPS_GAD_Data->veruncertspeed = 255;

   return TRUE;
}

