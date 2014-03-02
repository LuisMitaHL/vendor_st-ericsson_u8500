
//****************************************************************************
// GPS IP Centre, ST-Ericsson (UK) Ltd.
// Copyright (c) 2009 ST-Ericsson (UK) Ltd.
// 15-16 Cottesbrooke Park, Heartlands Business Park, Daventry, NN11 8YL, UK.
// All rights reserved.
//
// Filename gps_ptypes.h
//
// $Header: S:/GN_49xxPE_MKS/49xxPE_API/rcs/gps_ptypes.h 1.2 2011/08/15 11:51:18Z geraintf Rel $
// $Locker: $
//****************************************************************************

#ifndef GPS_PTYPES_H
#define GPS_PTYPES_H

//****************************************************************************
/// \file
/// \brief
///     GNSS PE Core Library primitive data type definitions (platform specific).
//
/// \defgroup  gps_ptypes  GNSS PE Core Library Primitive Data Type Definitions.
//
/// \brief
///     GNSS PE Core Library primitive data type definitions (platform specific).
//
/// \details
///     GNSS PE Core Library primitive data type definitions typedefs.
///     These typedefs were also used to generate the Core PE Libraries and
///     therefore should not be changed without requesting a new library.
//
//*****************************************************************************

/// \addtogroup gps_ptypes
/// \{

//*****************************************************************************

// Unsigned integer types
typedef unsigned char   U1;         ///< 1 byte unsigned numerical value.
typedef unsigned short  U2;         ///< 2 byte unsigned numerical value.
typedef unsigned int    U4;         ///< 4 byte unsigned numerical value.

// Signed integer types
typedef signed   char   I1;         ///< 1 byte signed numerical value.
typedef signed   short  I2;         ///< 2 byte signed numerical value.
typedef signed   int    I4;         ///< 4 byte signed numerical value.

// Real floating point number, single and double precision
#ifdef LBS_LINUX_UBUNTU_FTR
#if defined(RENAME_RX_TYPES)
#undef R4
#define R4 R4_t
#undef R8
#define R8 R8_t
#endif
#endif
typedef float           R4;         ///< 4 byte floating point value.
typedef double          R8;         ///< 8 byte floating point value.

// Define ASCII character type
typedef char            CH;         ///< ASCII character.

// Boolean / Logical type
typedef unsigned char   BL;         ///< Boolean logical (#TRUE or #FALSE only).
typedef unsigned char   L1;
typedef unsigned short  L2;
typedef unsigned int    L4;

//*****************************************************************************

// Ensure TRUE is defined
#ifndef TRUE
   #define TRUE  1                  ///< TRUE is defined as 1.
#endif

// Ensure FALSE is defined
#ifndef FALSE
   #define FALSE 0                  ///< FALSE is defined as 0.
#endif

// Ensure NULL is defined
#ifndef NULL
   #define NULL (void*)0            ///< NULL is defined as (void*)0.
#endif

//*****************************************************************************

/// \}

//****************************************************************************

#endif   // GPS_PTYPES_H

// end of file
