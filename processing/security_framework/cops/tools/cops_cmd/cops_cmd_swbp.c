/*************************************************************************
 * Copyright ST-Ericsson 2012
 *
 *  This file is only used in OSE environment
 ************************************************************************/

#include <cops.h>
#include <cops_cmd_swbp.h>
#include <cops_common.h>  /* COPS_ALOG, ... */
#include <cops_data.h>
#include <r_cops.h>
#include <r_simlock.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>

#define HEX_BASE 16

/* Test data used in some of the functions */
static uint8 TestData_1[] = "1234567890";
static uint8 TestData_2[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 0};
static uint8 TestData_3[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 0,
                             1, 2, 3, 4, 5, 6, 7, 8, 9, 0,
                             1, 2, 3, 4, 5, 6, 7, 8, 9, 0,
                             1, 2, 3, 4, 5, 6, 7, 8, 9, 0,
                             1, 2, 3, 4, 5, 6, 7, 8, 9, 0,
                             1, 2, 3, 4, 5, 6, 7, 8, 9, 0,
                             1, 2, 3, 4, 5, 6, 7, 8, 9, 0,
                             1, 2, 3, 4, 5, 6, 7, 8, 9, 0 };

static uint8 TestSignedData_ape_normal[] = { /* Type: ape_normal */
0x52, 0x44, 0x48, 0x53,  0xD0, 0x00, 0x80, 0x00,
0x02, 0x00, 0x00, 0x00,  0x01, 0x00, 0x00, 0x00,
0x02, 0x00, 0x00, 0x00,  0x05, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,  0x0A, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
0xC7, 0x75, 0xE7, 0xB7,  0x57, 0xED, 0xE6, 0x30,
0xCD, 0x0A, 0xA1, 0x11,  0x3B, 0xD1, 0x02, 0x66,
0x1A, 0xB3, 0x88, 0x29,  0xCA, 0x52, 0xA6, 0x42,
0x2A, 0xB7, 0x82, 0x86,  0x2F, 0x26, 0x86, 0x46,
0x11, 0x7C, 0x9A, 0xDA,  0x35, 0x25, 0xDF, 0x33,
0x59, 0xF6, 0x12, 0x0B,  0xC1, 0x09, 0x76, 0x8A,
0x60, 0x03, 0x57, 0x10,  0xC9, 0x8B, 0xE7, 0xA4,
0x2B, 0xBE, 0xD4, 0xC7,  0x96, 0x21, 0x1C, 0x42,
0xE1, 0x55, 0x62, 0x69,  0x51, 0xB5, 0xD5, 0x39,
0x0D, 0x9E, 0xBD, 0xD7,  0xA9, 0xD1, 0x7A, 0xE8,
0xC2, 0x71, 0x14, 0xE0,  0x86, 0x47, 0x19, 0x26,
0xA9, 0x88, 0x20, 0xCE,  0x1A, 0x52, 0x30, 0xAC,
0xA9, 0x3C, 0x2C, 0xC6,  0x22, 0xA8, 0x82, 0x59,
0x78, 0x17, 0xD7, 0x5E,  0xFF, 0x71, 0x1B, 0x66,
0x42, 0x79, 0xFD, 0xC4,  0x2C, 0x32, 0x2D, 0x2F,
0x40, 0x09, 0x9F, 0xCE,  0x7D, 0xC3, 0x0E, 0xA1,
0x2F, 0xB8, 0xAE, 0xA8,  0x72, 0xCB, 0x5E, 0x25,
0xAC, 0xAF, 0xB8, 0xF6,  0x38, 0xEC, 0x76, 0xF9,
0x16, 0x14, 0x2D, 0x37,  0x53, 0xE5, 0xC4, 0xAE,
0xB8, 0xE1, 0x56, 0x64,  0x64, 0xF7, 0xE1, 0xC6,
0x31, 0x32, 0x33, 0x34,  0x35, 0x36, 0x37, 0x38,
0x39, 0x30 };

static uint8 TestSignedData_itp[] = { /* Type: ITP */
0x52, 0x44, 0x48, 0x53, /* Magic number (0x53484452) */
0xD0, 0x00,             /* Size of signed header, 0xD0 = 208 */
0x80, 0x00,             /* Size of signature,     0x80 = 128 */
0x02, 0x00, 0x00, 0x00, /* Hashtype of signature, 0x2 = SHA256 */
0x01, 0x00, 0x00, 0x00, /* Signature type, 0x1 = RSASSA_PKCS_V1_5 */
0x02, 0x00, 0x00, 0x00, /* Hashtype of payload, 0x2 = SHA256 */
0x0D, 0x00, 0x00, 0x00, /* Type of payload, 0xD = ITP */
0x00, 0x00, 0x00, 0x00, /* Flags (reserved) */
0x0A, 0x00, 0x00, 0x00, /* Size of payload */
0x00, 0x00, 0x00, 0x00, /* Software version number */
0x00, 0x00, 0x00, 0x00, /* Load address */
0x00, 0x00, 0x00, 0x00, /* Startup address */
0x00, 0x00, 0x00, 0x00, /* Spare (reserved) */
0xC7, 0x75, 0xE7, 0xB7,  0x57, 0xED, 0xE6, 0x30, /* SHA256 of Payload */
0xCD, 0x0A, 0xA1, 0x11,  0x3B, 0xD1, 0x02, 0x66,
0x1A, 0xB3, 0x88, 0x29,  0xCA, 0x52, 0xA6, 0x42,
0x2A, 0xB7, 0x82, 0x86,  0x2F, 0x26, 0x86, 0x46,
0x18, 0xF0, 0xDC, 0xB9,  0xA3, 0xA8, 0xD7, 0x03, /* Signature */
0x95, 0xE8, 0x2A, 0xF3,  0x7D, 0x99, 0x93, 0x5D,
0xD8, 0x02, 0x19, 0x4A,  0x95, 0x65, 0x34, 0x30,
0xD0, 0xC5, 0x01, 0x38,  0x21, 0x93, 0x7E, 0x4B,
0xEC, 0xE8, 0x0C, 0x89,  0x53, 0xFE, 0xBA, 0x45,
0x68, 0xDB, 0xDB, 0xA4,  0x30, 0x02, 0x5D, 0xEB,
0x8C, 0x84, 0x24, 0xC4,  0x4D, 0xA2, 0x30, 0xD1,
0x77, 0x1E, 0x09, 0x71,  0x51, 0x24, 0x0E, 0x45,
0x85, 0xA1, 0xD6, 0xE1,  0x9D, 0xDE, 0xED, 0xE8,
0xFB, 0x05, 0xE0, 0x9E,  0x29, 0xF6, 0xC0, 0x4A,
0x2D, 0xCD, 0x8F, 0x11,  0x6D, 0x3B, 0x0A, 0x48,
0x84, 0x7C, 0x7B, 0xD1,  0xEA, 0xAF, 0x66, 0x66,
0x91, 0xD2, 0x5E, 0x55,  0x6C, 0x19, 0x89, 0xC7,
0x81, 0x1E, 0xDE, 0x74,  0x3D, 0xFD, 0x67, 0xF8,
0x13, 0xF0, 0x21, 0xC3,  0x59, 0x33, 0x36, 0x1E,
0xA2, 0x62, 0x81, 0x43,  0x6F, 0x31, 0xE4, 0x7A,
0x31, 0x32, 0x33, 0x34,  0x35, 0x36, 0x37, 0x38, /* Payload = "1234567890" */
0x39, 0x30 };

/* Calibration data Type: ITP (app_calib_init.q)*/
static uint8 TestSignedData_itp_calibdata[] = {
    0x52, 0x44, 0x48, 0x53,  0xD0, 0x00, 0x80, 0x00, /* Header */
    0x02, 0x00, 0x00, 0x00,  0x01, 0x00, 0x00, 0x00,
    0x02, 0x00, 0x00, 0x00,  0x0D, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,  0x04, 0x02, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
    0x36, 0xD9, 0x4B, 0x53,  0x99, 0x62, 0xD8, 0x52, /* Hash of payload */
    0xE7, 0x26, 0xDB, 0xBE,  0xA0, 0xA1, 0x0D, 0x4F, /* (SHA256)        */
    0x04, 0x6B, 0x7D, 0xBF,  0xCC, 0x5A, 0x80, 0xC2,
    0xEC, 0xA1, 0x1F, 0x4C,  0x41, 0x11, 0x35, 0x8D,
    0x9D, 0xA7, 0x49, 0x99,  0x9C, 0xB3, 0xC4, 0xE8, /* Signature */
    0x84, 0x42, 0x65, 0x3B,  0xFD, 0x96, 0xCE, 0x9A,
    0x6B, 0x9C, 0x0A, 0xC6,  0xAA, 0xDB, 0x35, 0x36,
    0x73, 0x24, 0xF8, 0xB5,  0xC3, 0x67, 0x4F, 0xA0,
    0xDE, 0x2F, 0x74, 0x91,  0xB3, 0xA2, 0x29, 0x2C,
    0xAA, 0xC6, 0x71, 0xD4,  0x73, 0x79, 0x51, 0x3A,
    0x7F, 0xC3, 0x72, 0x6D,  0xDA, 0xEC, 0xBF, 0xE8,
    0xFA, 0xAA, 0xED, 0x53,  0x55, 0xC9, 0xAE, 0x41,
    0x0F, 0xB2, 0x8E, 0x0E,  0x5D, 0x5D, 0x2B, 0x7F,
    0x9E, 0x76, 0x8B, 0x40,  0xD8, 0x13, 0x01, 0x1E,
    0x05, 0x73, 0xDE, 0x62,  0x54, 0x11, 0x4C, 0xDC,
    0x32, 0x26, 0x8A, 0xDE,  0xCF, 0xD4, 0x3C, 0x96,
    0xDB, 0xED, 0x9E, 0x0B,  0xE5, 0x8F, 0x36, 0x38,
    0xC8, 0x9D, 0x5F, 0x23,  0xF2, 0x20, 0xC3, 0x14,
    0x38, 0xFA, 0xBD, 0x38,  0xC9, 0xCA, 0xAD, 0x9C,
    0xF0, 0xCF, 0xB6, 0x35,  0x56, 0x51, 0x17, 0x8F,
    0x03, 0x00, 0x00, 0x00,  0x03, 0x00, 0x00, 0x00, /* Payload */
    0x12, 0x00, 0x00, 0x00,  0x1D, 0x05, 0x0E, 0x03,
    0x28, 0x00, 0x00, 0x00,  0xDC, 0x01, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
    0x0C, 0x00, 0x00, 0x00,  0x01, 0x00, 0x00, 0x00,
    0x0D, 0xE4, 0x01, 0x40,  0x0D, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00,  0x01, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,  0x01, 0x00, 0x00, 0x00,
    0x0E, 0xE4, 0x01, 0x40,  0x0E, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00,  0x01, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,  0x02, 0x00, 0x00, 0x00,
    0x0F, 0xE4, 0x01, 0x40,  0x10, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00,  0x01, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,  0x00, 0x01, 0x04, 0x00,
    0x10, 0xE4, 0x01, 0x40,  0x14, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00,  0x01, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,  0x00, 0x01, 0x02, 0x03,
    0x04, 0x05, 0x08, 0x00,  0x11, 0xE4, 0x01, 0x40,
    0x14, 0x00, 0x00, 0x00,  0x01, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
    0x00, 0x01, 0x02, 0x03,  0x04, 0x05, 0x08, 0x00,
    0x13, 0xE4, 0x01, 0x40,  0x14, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00,  0x02, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,  0x00, 0x01, 0x02, 0x03,
    0x04, 0x05, 0x08, 0x00,  0x1E, 0xE4, 0x01, 0x40,
    0x14, 0x00, 0x00, 0x00,  0x01, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
    0x00, 0x01, 0x02, 0x03,  0x04, 0x05, 0x08, 0x00,
    0x1F, 0xE4, 0x01, 0x40,  0x14, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x01, 0x00,  0x01, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,  0x00, 0x01, 0x02, 0x03,
    0x04, 0x05, 0x08, 0x00,  0x20, 0xE4, 0x01, 0x40,
    0x14, 0x00, 0x00, 0x00,  0x01, 0x00, 0x00, 0x00,
    0x02, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
    0x00, 0x01, 0x02, 0x03,  0x04, 0x05, 0x08, 0x00,
    0x21, 0xE4, 0x01, 0x40,  0x14, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x01, 0x00,  0x02, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,  0x00, 0x01, 0x02, 0x03,
    0x04, 0x05, 0x08, 0x00,  0x28, 0xE4, 0x01, 0x40,
    0x0D, 0x00, 0x00, 0x00,  0x01, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00,  0x29, 0xE4, 0x01, 0x40,
    0x0F, 0x00, 0x00, 0x00,  0x01, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
    0x00, 0x03, 0x00, 0x00,  0x2A, 0xE4, 0x01, 0x40,
    0x11, 0x00, 0x00, 0x00,  0x01, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
    0x00, 0x01, 0x02, 0x05,  0x00, 0x00, 0x00, 0x00,
    0x2B, 0xE4, 0x01, 0x40,  0x13, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00,  0x01, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,  0x00, 0x01, 0x02, 0x03,
    0x04, 0x07, 0x00, 0x00,  0x2C, 0xE4, 0x01, 0x40,
    0x0D, 0x00, 0x00, 0x00,  0x01, 0x00, 0x00, 0x00,
    0x02, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00,  0x2D, 0xE4, 0x01, 0x40,
    0x0F, 0x00, 0x00, 0x00,  0x01, 0x00, 0x00, 0x00,
    0x02, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
    0x00, 0x03, 0x00, 0x00,  0x2E, 0xE4, 0x01, 0x40,
    0x11, 0x00, 0x00, 0x00,  0x01, 0x00, 0x00, 0x00,
    0x02, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
    0x00, 0x01, 0x02, 0x05,  0x00, 0x00, 0x00, 0x00,
    0x2F, 0xE4, 0x01, 0x40,  0x13, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00,  0x02, 0x00, 0x00, 0x00, /* Payload */
    0x00, 0x00, 0x00, 0x00,  0x00, 0x01, 0x02, 0x03,
    0x04, 0x07, 0x00, 0x00 };


/* Pre-defined ControlKey sets used for testing */
static SIMLock_ControlKeys_t Test_CK_1 = {
        {"11111111\0\0\0\0\0\0\0\0"},
        {"11111111\0\0\0\0\0\0\0\0"},
        {"11111111\0\0\0\0\0\0\0\0"},
        {"11111111\0\0\0\0\0\0\0\0"},
        {"11111111\0\0\0\0\0\0\0\0"} };
static SIMLock_ControlKeys_t Test_CK_2 = {
        {"2222222222222222"},
        {"2222222222222222"},
        {"2222222222222222"},
        {"2222222222222222"},
        {"2222222222222222"} };
static SIMLock_ControlKeys_t Test_CK_3 = {
        {"11111111\0\0\0\0\0\0\0\0"},
        {"22222222\0\0\0\0\0\0\0\0"},
        {"33333333\0\0\0\0\0\0\0\0"},
        {"44444444\0\0\0\0\0\0\0\0"},
        {"55555555\0\0\0\0\0\0\0\0"} };

static uint8 TestCopsData_NL[] = { /* NL unlocked */
 0x02, 0x00, 0x00, 0x00,  0x10, 0x00, 0x05, 0x00,  0x00 };
static uint8 TestCopsData_NL_L[] = { /* NL locked */
 0x02, 0x00, 0x00, 0x00,        /* Data header (Version) */
 0x10, 0x00, 0x05, 0x00,  0x01,  /* Lock setting (no IMSI ranges) */
 0x20, 0x00, 0x18, 0x00,  0x00, 0x00, 0x00, 0x00, /* Lock Control data */
 0x00, 0x05, 0x03, 0x00,  0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00 };
static uint8 TestCopsData_NL_L1[] = { /* NL locked */
 0x02, 0x00, 0x00, 0x00,        /* Data header (Version) */
 0x10, 0x00, 0x3C, 0x00,  0x01, 0x00, 0x00, 0x00, /* Lock Settings */
 0x01, 0x00, 0x00, 0x00,  0x02, 0x00, 0x02, 0x00,
 0x02, 0x00, 0x02, 0x00,  0x02, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00 };


static void cops_swbp_ReadBuildInfo(char *cmd, int *arg_index, int args);
static void cops_swbp_ReadImei(char *cmd, int *arg_index, int args);
static void cops_swbp_GetNbrOfOTP_Rows(char *cmd, int *arg_index, int args);
static void cops_swbp_WriteImei(char *cmd, int *arg_index, int args);
static void cops_swbp_GetChallenge(char *cmd, int *arg_index, int args);
static void cops_swbp_Authenticate(char *cmd, int *arg_index, int args);
static void cops_swbp_Deauthenticate(char *cmd, int *arg_index, int args);
static void cops_swbp_GetDeviceState(char *cmd, int *arg_index, int args);
static void cops_swbp_BindProperties(char *cmd, int *arg_index, int args);
static void cops_swbp_ReadData(char *cmd, int *arg_index, int args);
static void cops_swbp_ReadParameters(char *cmd, int *arg_index, int args);
static void cops_swbp_FindParam(char *cmd, int *arg_index, int args);
static void cops_swbp_BindData(char *cmd, int *arg_index, int args);
static void cops_swbp_VerifyDataBinding(char *cmd, int *arg_index, int args);
static void cops_swbp_VerifySignedHeader(char *cmd, int *arg_index, int args);
static void cops_swbp_CalcDigest(char *cmd, int *arg_index, int args);
static void cops_swbp_ReadOTP(char *cmd, int *arg_index, int args);

static void cops_swbp_Lock(char *cmd, int *arg_index, int args);
static void cops_swbp_Unlock(char *cmd, int *arg_index, int args);
static void cops_swbp_GetSimLockStatus(char *cmd, int *arg_index, int args);
static void cops_swbp_WriteCopsData(char *cmd, int *arg_index, int args);
static void cops_swbp_VerifyControlKeys(char *cmd, int *arg_index, int args);
static void cops_swbp_SIM_ChangeLockKey(char *cmd, int *arg_index, int args);
static void cops_swbp_VerifyImsi(char *cmd, int *arg_index, int args);
static void cops_swbp_GetProdDbgSettings(char *cmd, int *arg_index, int args);
static void cops_swbp_OTA_Unlock(char *cmd, int *arg_index, int args);

IDBG_TBL_START(cops_swbp_commands)
    IDBG_TBL_CMD(cops_swbp_ReadBuildInfo, "ReadBuildInfo")
    IDBG_TBL_CMD(cops_swbp_ReadImei, "ReadIMEI")
    IDBG_TBL_CMD(cops_swbp_GetNbrOfOTP_Rows, "GetNbrOfOTP_Rows")
    IDBG_TBL_CMD(cops_swbp_WriteImei, "WriteIMEI")
    IDBG_TBL_CMD(cops_swbp_GetChallenge, "GetChallenge")
    IDBG_TBL_CMD(cops_swbp_ReadData, "ReadData")
    IDBG_TBL_CMD(cops_swbp_ReadParameters, "ReadParameters")
    IDBG_TBL_CMD(cops_swbp_FindParam, "FindParam")
    IDBG_TBL_CMD(cops_swbp_Authenticate, "Authenticate")
    IDBG_TBL_CMD(cops_swbp_Deauthenticate, "Deauthenticate")
    IDBG_TBL_CMD(cops_swbp_GetDeviceState, "GetDeviceState")
    IDBG_TBL_CMD(cops_swbp_BindProperties, "BindProperties")
    IDBG_TBL_CMD(cops_swbp_BindData, "BindData")
    IDBG_TBL_CMD(cops_swbp_VerifyDataBinding, "VerifyDataBinding")
    IDBG_TBL_CMD(cops_swbp_VerifySignedHeader, "VerifySignedHeader")
    IDBG_TBL_CMD(cops_swbp_CalcDigest, "CalcDigest")
    IDBG_TBL_CMD(cops_swbp_ReadOTP, "ReadOTP")
    IDBG_TBL_CMD(cops_swbp_Lock, "Lock")
    IDBG_TBL_CMD(cops_swbp_Unlock, "Unlock")
    IDBG_TBL_CMD(cops_swbp_OTA_Unlock, "OTA_Unlock")
    IDBG_TBL_CMD(cops_swbp_GetSimLockStatus, "GetSimLockStatus")
    IDBG_TBL_CMD(cops_swbp_WriteCopsData, "WriteCopsData")
    IDBG_TBL_CMD(cops_swbp_VerifyControlKeys, "VerifyControlKeys")
    IDBG_TBL_CMD(cops_swbp_SIM_ChangeLockKey, "SIM_ChangeLockKey")
    IDBG_TBL_CMD(cops_swbp_VerifyImsi, "VerifyImsi")
    IDBG_TBL_CMD(cops_swbp_GetProdDbgSettings, "GetProductDebugSettings")
IDBG_TBL_END


static const char HexChars[] = {
  '0', '1', '2', '3', '4', '5', '6', '7',
  '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
};

/* Print data buffer in HEX format
 * (function copied from i_SecUtil_HexPrintBuffer) */
void PrintHexBuffer(const char   *BufferName_p,
                    const void   *Buffer_p,
                    size_t        BufferLength)
{
        char    LineBuf[68];
        size_t  LinePos = 0;
  const uint8  *Data_p = Buffer_p;
        size_t  n;
        char  Separator;
        char  PrintableChar;

    if (BufferName_p != NULL) {
        COPS_IDBG_PRINT("%s: Size = %d", BufferName_p, BufferLength);
    }

    if (Data_p != NULL) {
        /* Initialize the line buffer for a new line */
        memset(LineBuf, ' ', sizeof(LineBuf));
        LineBuf[sizeof(LineBuf) - 1] = '\0';
        LinePos = 0;
        for (n = 0; n < BufferLength; n++) {
            if (((n + 1) % 4) == 0) {
                Separator = ' ';
            } else {
                Separator = ':';
            }

            /*lint -save -e718 -e746 -e409 -e48 error in lint header file*/
            if (isprint(Data_p[n])) {
                PrintableChar = (char)Data_p[n];
            } else {
                PrintableChar = '.';
            }
            /*lint -restore */

            LineBuf[LinePos * 3 + 0]  = HexChars[Data_p[n] >> 4];
            LineBuf[LinePos * 3 + 1]  = HexChars[Data_p[n] & 0x0F];
            LineBuf[LinePos * 3 + 2]  = Separator;
            LineBuf[50 + LinePos]     = PrintableChar;

            LinePos++;
            if (LinePos == 16) {
                COPS_IDBG_PRINT("  %s", LineBuf);

                /* Initialize the line buffer for a new line */
                memset(LineBuf, ' ', sizeof(LineBuf));
                LineBuf[sizeof(LineBuf) - 1] = '\0';
                LinePos = 0;
            }
        }
        if (LinePos != 0) {
            COPS_IDBG_PRINT("  %s", LineBuf);
        }
    }
}

static char *cops_swbp_ReturnCodeDesc(COPS_ReturnCode_t rc)
{
    static char DefaultDescr[32];

    switch (rc) {
    case COPS_RETURN_CODE_OK:
        return "OK";
    case COPS_RETURN_CODE_ASYNC_CALL:
        return "ASYNC_CALL";
    case COPS_RETURN_CODE_IPC_ERROR:
        return "IPC_ERROR";
    case COPS_RETURN_CODE_INTERNAL_IPC_ERROR:
        return "INTERNAL_IPC_ERROR";
    case COPS_RETURN_CODE_ASYNC_IPC_ERROR:
        return "ASYNC_IPC_ERROR";
    case COPS_RETURN_CODE_ARGUMENT_ERROR:
        return "ARGUMENT_ERROR";
    case COPS_RETURN_CODE_STORAGE_ERROR:
        return "STORAGE_ERROR";
    case COPS_RETURN_CODE_MEMORY_ALLOCATION_ERROR:
        return "MEMORY_ALLOCATION_ERROR";
    case COPS_RETURN_CODE_UNSPECIFIC_ERROR:
        return "UNSPECIFIC_ERROR";
    case COPS_RETURN_CODE_SERVICE_NOT_AVAILABLE_ERROR:
        return "SERVICE_NOT_AVAILABLE_ERROR";
    case COPS_RETURN_CODE_SERVICE_ERROR:
        return "SERVICE_ERROR";
    case COPS_RETURN_CODE_NOT_AUTHENTICATED_ERROR:
        return "NOT_AUTHENTICATED_ERROR";
    case COPS_RETURN_CODE_CHALLENGE_MISSING_ERROR:
        return "CHALLENGE_MISSING_ERROR";
    case COPS_RETURN_CODE_SIGNATURE_VERIFICATION_ERROR:
        return "SIGNATURE_VERIFICATION_ERROR";
    case COPS_RETURN_CODE_DATA_TAMPERED_ERROR:
        return "DATA_TAMPERED_ERROR";
    case COPS_RETURN_CODE_DATA_CONFIGURATION_ERROR:
        return "DATA_CONFIGURATION_ERROR";
    case COPS_RETURN_CODE_INCORRECT_SIM:
        return "INCORRECT_SIM";
    case COPS_RETURN_CODE_TIMER_RUNNING:
        return "TIMER_RUNNING";
    case COPS_RETURN_CODE_NO_UNLOCK_ATTEMPTS_LEFT:
        return "NO_UNLOCK_ATTEMPTS_LEFT";
    case COPS_RETURN_CODE_INVALID_SIMLOCK_KEY:
        return "INVALID_SIMLOCK_KEY";
    case COPS_RETURN_CODE_INTERNAL_ERROR:
        return "INTERNAL_ERROR";
    case COPS_RETURN_CODE_LOCKING_ERROR:
        return "LOCKING_ERROR";
    case COPS_RETURN_CODE_UNLOCK_ATTEMPTS_STILL_LEFT:
        return "UNLOCK_ATTEMPTS_STILL_LEFT";
    case COPS_RETURN_CODE_AUTOLOCK_NOT_ALLOWED:
        return "AUTOLOCK_NOT_ALLOWED";
    case COPS_RETURN_CODE_OTP_LOCKED_ERROR:
        return "OTP_LOCKED_ERROR";
    case COPS_RETURN_CODE_BUFFER_TOO_SMALL:
        return "BUFFER_TOO_SMALL";
    default:
        sprintf(DefaultDescr, "Unknown return code %d (0x%x)", rc, rc);
        return DefaultDescr;
    }
}

static char *cops_swbp_SimLockReturnCodeDesc(SIMLock_ReturnCode_t rc)
{
    static char DefaultDescr[32];

    switch (rc) {
    case SIMLOCK_RETURN_CODE_OK:
        return "OK";
    case SIMLOCK_RETURN_CODE_ASYNC_CALL:
        return "ASYNC_CALL";
    case SIMLOCK_RETURN_CODE_IPC_ERROR:
        return "IPC_ERROR";
    case SIMLOCK_RETURN_CODE_INTERNAL_IPC_ERROR:
        return "INTERNAL_IPC_ERROR";
    case SIMLOCK_RETURN_CODE_ASYNC_IPC_ERROR:
        return "ASYNC_IPC_ERROR";
    case SIMLOCK_RETURN_CODE_ARGUMENT_ERROR:
        return "ARGUMENT_ERROR";
    case SIMLOCK_RETURN_CODE_STORAGE_ERROR:
        return "STORAGE_ERROR";
    case SIMLOCK_RETURN_CODE_MEMORY_ALLOCATION_ERROR:
        return "MEMORY_ALLOCATION_ERROR";
    case SIMLOCK_RETURN_CODE_UNSPECIFIC_ERROR:
        return "UNSPECIFIC_ERROR";
    case SIMLOCK_RETURN_CODE_SERVICE_NOT_AVAILABLE_ERROR:
        return "SERVICE_NOT_AVAILABLE_ERROR";
    case SIMLOCK_RETURN_CODE_SERVICE_ERROR:
        return "SERVICE_ERROR";
    case SIMLOCK_RETURN_CODE_NOT_AUTHENTICATED_ERROR:
        return "NOT_AUTHENTICATED_ERROR";
    case SIMLOCK_RETURN_CODE_CHALLENGE_MISSING_ERROR:
        return "CHALLENGE_MISSING_ERROR";
    case SIMLOCK_RETURN_CODE_SIGNATURE_VERIFICATION_ERROR:
        return "SIGNATURE_VERIFICATION_ERROR";
    case SIMLOCK_RETURN_CODE_DATA_TAMPERED_ERROR:
        return "DATA_TAMPERED_ERROR";
    case SIMLOCK_RETURN_CODE_DATA_CONFIGURATION_ERROR:
        return "DATA_CONFIGURATION_ERROR";
    case SIMLOCK_RETURN_CODE_INCORRECT_SIM:
        return "INCORRECT_SIM";
    case SIMLOCK_RETURN_CODE_TIMER_RUNNING:
        return "TIMER_RUNNING";
    case SIMLOCK_RETURN_CODE_NO_UNLOCK_ATTEMPTS_LEFT:
        return "NO_UNLOCK_ATTEMPTS_LEFT";
    case SIMLOCK_RETURN_CODE_TIMED_OUT:
        return "TIMED_OUT";
    case SIMLOCK_RETURN_CODE_INCORRECT_DATA_BINDING:
        return "INCORRECT_DATA_BINDING";
    case SIMLOCK_RETURN_CODE_LOCK_PERMANENTLY_DISABLED:
        return "LOCK_PERMANENTLY_DISABLED";
    case SIMLOCK_RETURN_CODE_INCORRECT_CONTROLKEY:
        return "INCORRECT_CONTROLKEY";
    case SIMLOCK_RETURN_CODE_INTERNAL_ERROR:
        return "INTERNAL_ERROR";
    default:
        sprintf(DefaultDescr, "Unknown return code %d (0x%x)", rc, rc);
        return DefaultDescr;
    }
}

/* Convert one ASCIIHex byte in string format to one byte (uint8)
 *  Handles 0x10 and 10 equally
 *  Assumes that length of buf is enough */
static uint8 Str2Hex(char *buf)
{
    char *EndPtr = NULL;
    char  HexData[3];

    HexData[2] = '\0';

    if (buf[0] == '0' && (buf[1] == 'x' || buf[1] == 'X')) {
        memcpy(HexData, buf+2, 2);
    } else {
        memcpy(HexData, buf, 2);
    }

    return (uint8)strtoul(HexData, &EndPtr, HEX_BASE);
}

/* Returns build info about this file and COPS is general */
static void cops_swbp_ReadBuildInfo(char *cmd, int *arg_index,
                                    int args)
{
    COPS_IDBG_PRINT("DATE = %s", __DATE__);
    COPS_IDBG_PRINT("TIME = %s", __TIME__);

#ifdef COPS_IM_STUB_LEVEL_API_PROXY
    COPS_IDBG_PRINT(" - COPS_IM_STUB_LEVEL_API_PROXY defined");
#endif
#ifdef COPS_TAPP_EMUL
    COPS_IDBG_PRINT(" - COPS_TAPP_EMUL defined");
#endif
#ifdef COPS_ENABLE_FEATURE_STUBBED_SECURE_WORLD
    COPS_IDBG_PRINT(" - COPS_ENABLE_FEATURE_STUBBED_SECURE_WORLD defined");
#endif
#ifdef COPS_IPC_HANDSHAKE_NEEDED
    COPS_IDBG_PRINT(" - COPS_IPC_HANDSHAKE_NEEDED defined");
#endif
#ifdef COPS_TAPP_LOCAL
    COPS_IDBG_PRINT(" - COPS_TAPP_LOCAL defined");
#endif
#ifdef COPS_USE_AUTO_GEN_INCLUDE
    COPS_IDBG_PRINT(" - COPS_USE_AUTO_GEN_INCLUDE defined");
#endif
#ifdef COPS_IN_LOADERS
    COPS_IDBG_PRINT(" - COPS_IN_LOADERS defined");
#endif

#ifdef COPS_STORAGE_DIR
    COPS_IDBG_PRINT(" - COPS_STORAGE_DIR = %s", STR(COPS_STORAGE_DIR));
#endif
#ifdef COPS_LOG_FILE
    COPS_IDBG_PRINT(" - COPS_LOG_FILE = %s", STR(COPS_LOG_FILE));
#endif
#ifdef COPS_SOCKET_PATH
    COPS_IDBG_PRINT(" - COPS_SOCKET_PATH = %s", STR(COPS_SOCKET_PATH));
#endif
#ifdef SIPC_SERVER
    COPS_IDBG_PRINT(" - SIPC_SERVER = %s", STR(SIPC_SERVER));
#endif
#ifdef SIPC_CLIENT
    COPS_IDBG_PRINT(" - SIPC_CLIENT = %s", STR(SIPC_CLIENT));
#endif
#ifdef COPS_SYNC_FILE
    COPS_IDBG_PRINT(" - COPS_SYNC_FILE = %s", STR(COPS_SYNC_FILE));
#endif

    /* TEST!!!! */
    COPS_LOG(LOG_ERROR, "error-log");
    COPS_LOG(LOG_WARNING, "warning-log");
    COPS_LOG(LOG_INFO, "info-log");
}

/* Usage: ReadImei
 *   Returns IMEI of the device
 */
static void cops_swbp_ReadImei(char *cmd, int *arg_index, int args)
{
    COPS_ReturnCode_t rc;
    COPS_IMEI_t  imei;

    rc = Do_COPS_ReadIMEI(&imei);
    if (rc != COPS_RETURN_CODE_OK) {
        COPS_IDBG_PRINT("ReadIMEI failed, rc = 0x%x (%s)", rc,
                cops_swbp_ReturnCodeDesc(rc));
    } else {
        char sz[64];
        sprintf(sz, "IMEI = %d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                imei.Digits[0],  imei.Digits[1],  imei.Digits[2],
                imei.Digits[3],  imei.Digits[4],  imei.Digits[5],
                imei.Digits[6],  imei.Digits[7],  imei.Digits[8],
                imei.Digits[9],  imei.Digits[10], imei.Digits[11],
                imei.Digits[12], imei.Digits[13], imei.Digits[14]);
        COPS_IDBG_PRINT(sz);
    }
}

static void cops_swbp_GetNbrOfOTP_Rows(char *cmd, int *arg_index, int args)
{
    COPS_ReturnCode_t rc;
    uint32 nbr;

    rc = Do_COPS_GetNbrOf_OTP_Rows(&nbr);
    if (rc != COPS_RETURN_CODE_OK) {
        COPS_IDBG_PRINT("GetNbrOfOTP_Rows failed, rc = 0x%x (%s)", rc,
                        cops_swbp_ReturnCodeDesc(rc));
    } else {
        COPS_IDBG_PRINT("GetNbrOfOTP_Rows: %ld\n", nbr);
    }
}

/* Syntax: WriteImei <imei>
 *  imei: 14-15 digits
 *    if 15 digits are used, the 15th digit (Luhn check digit) must be correct
 */
static void cops_swbp_WriteImei(char *cmd, int *arg_index, int args)
{
    COPS_BindPropertiesArg_t  BPArg;
    COPS_IMEI_t               imei;
    COPS_ReturnCode_t         rc;
    size_t                    n;
    const char               *imei_data = NULL;

    COPS_LOG(LOG_INFO, "[COPS]SWBP BindProperties [START]");

    memset(&BPArg, 0, sizeof(BPArg));
    memset(&imei, 0, sizeof(imei));

    if (args < 1) {
        COPS_IDBG_PRINT("Usage: WriteImei <imei>");
        return;
    }

    imei_data = cmd + arg_index[0];
    COPS_LOG(LOG_INFO, "[COPS] new imei = %s\n", imei_data);

    if (strlen(imei_data) < 14 || strlen(imei_data) > 15) {
        COPS_IDBG_PRINT("Enter 14-15 digits as IMEI");
        return;
    }

    imei.Digits[COPS_UNPACKED_IMEI_LENGTH-1] = 0xFF;

    for (n = 0; n < strlen(imei_data); n++) {
        if (imei_data[n] >= '0' && imei_data[n] <= '9')
            imei.Digits[n] = imei_data[n] - '0';
        else {
            COPS_IDBG_PRINT("Enter 14-15 digits as IMEI");
            return;
        }
    }
    BPArg.IMEI_p = &imei;

    rc = Do_COPS_BindProperties(&BPArg);

    if (rc != COPS_RETURN_CODE_OK) {
        COPS_IDBG_PRINT("WriteImei failed, rc = 0x%x (%s)", rc,
                cops_swbp_ReturnCodeDesc(rc));
    } else {
        COPS_IDBG_PRINT("WriteImei succeeded");
    }
}

/* Usage: GetChallenge
 *   Returns the RSA challenge
 */
static void cops_swbp_GetChallenge(char *cmd, int *arg_index, int args)
{
    COPS_ReturnCode_t rc;
    uint8             Buffer[COPS_CHALLENGE_LENGTH];
    size_t            BufferLength = 0;

    /* Get size of challenge */
    rc = Do_COPS_GetChallenge(COPS_AUTH_RSA_CHALLENGE, NULL, &BufferLength);

    if (rc != COPS_RETURN_CODE_OK) {
        COPS_IDBG_PRINT("GetChallenge failed, rc = 0x%x (%s)", rc,
                cops_swbp_ReturnCodeDesc(rc));
    } else {
        if (BufferLength > sizeof(Buffer)) {
            COPS_IDBG_PRINT("Challenge too big! %d > %d", BufferLength,
                                                          sizeof(Buffer));
        } else {
            /* Get actual challenge data */
            rc = Do_COPS_GetChallenge(COPS_AUTH_RSA_CHALLENGE,
                                      Buffer, &BufferLength);
            if (rc != COPS_RETURN_CODE_OK) {
                COPS_IDBG_PRINT("GetChallenge failed, size=%d, rc = 0x%x (%s)",
                        BufferLength, rc, cops_swbp_ReturnCodeDesc(rc));
            } else {
                COPS_IDBG_HEX("Challenge data", Buffer, BufferLength);
            }
        }
    }
}

/* Usage Authenticate [AuthType] [AuthData...]
 *  AuthType: 1=SimLock Keys, 2=RSA Challenge, 3=Permanent
 *  AuthData: depending on AuthType
 *    (AuthType=1]: NL_key NSL_key SP_key CL_key ESL_key
 *    (AuthType=2]: Signed challenge data in HexFormat (not impl yet)
 *    (AuthType=3]: No auth data needed
 */
static void cops_swbp_Authenticate(char *cmd, int *arg_index, int args)
{
    boolean                 Permanently = FALSE; /* Always! */
    COPS_AuthData_t         AuthData;
    COPS_ReturnCode_t       rc;
    int                     val;
    SIMLock_ControlKeys_t   SimLock_CKs;

    COPS_LOG(LOG_INFO, "SWBP Authenticate[START]\n");

    if (args < 1)
        goto UsageError;
    val = atoi(cmd + arg_index[0]);

    COPS_LOG(LOG_INFO, "[COPS]SWBP Authenticate AuthType=%d\n", val);

    /* SL auth */
    if (val == 1) {
        char *value;

        COPS_LOG(LOG_INFO, "[COPS]SL auth\n");

        /* All five ControlKeys must be supplied */
        if (args != 6)
            goto UsageError;

        memset(&SimLock_CKs, 0, sizeof(SimLock_CKs));

        value = cmd + arg_index[1];
        if (strlen(value) <= SIMLOCK_CONTROLKEY_MAX_LENGTH) {
            strcpy(SimLock_CKs.NL_Key.Value, value);
        }

        value = cmd + arg_index[2];
        if (strlen(value) <=  SIMLOCK_CONTROLKEY_MAX_LENGTH) {
            strcpy(SimLock_CKs.NSL_Key.Value, value);
        }

        value = cmd + arg_index[3];
        if (strlen(value) <= SIMLOCK_CONTROLKEY_MAX_LENGTH) {
            strcpy(SimLock_CKs.SPL_key.Value, value);
        }

        value = cmd + arg_index[4];
        if (strlen(value) <= SIMLOCK_CONTROLKEY_MAX_LENGTH) {
            strcpy(SimLock_CKs.CL_key.Value, value);
        }

        value = cmd + arg_index[5];
        if (strlen(value) <= SIMLOCK_CONTROLKEY_MAX_LENGTH) {
            strcpy(SimLock_CKs.FESLL_Key.Value, value);
        }

        AuthData.AuthType = COPS_AUTH_SIMLOCK_KEYS;
        AuthData.Length = sizeof(SimLock_CKs);
        AuthData.Data_p = (uint8_t *)&SimLock_CKs;
    }

    /* RSA auth, not supported yet! */
    else if (val == 2) {
        COPS_LOG(LOG_INFO, "[COPS]RSA auth\n");
        AuthData.AuthType = COPS_AUTH_RSA_CHALLENGE;
        AuthData.Length = 0;
        AuthData.Data_p = NULL;
    } else if (val == 3) {  /* Permanent auth */
        COPS_LOG(LOG_INFO, "[COPS]Perm auth\n");
        AuthData.AuthType = COPS_AUTH_PERMANENT_AUTHENTICATION;
        AuthData.Length = 0;
        AuthData.Data_p = NULL;
    }

    else
        goto UsageError;

    rc = Do_COPS_Authenticate(Permanently, &AuthData);

    if (rc != COPS_RETURN_CODE_OK) {
        COPS_IDBG_PRINT("Authenticate failed, rc = 0x%x (%s)", rc,
                cops_swbp_ReturnCodeDesc(rc));
    } else {
        COPS_IDBG_PRINT("Authenticate succeeded");
    }

    return;
UsageError:
    COPS_IDBG_PRINT("Usage: Authenticate [AuthType] [AuthData...]");
    COPS_IDBG_PRINT(" AuthType: 1=ControlKeys 2=RSA 3=Permanent");
}

/* Usage Deauthenticate [Permanently]
 *  Permanently: 0/1 (0 if parameter omitted)
 */
static void cops_swbp_Deauthenticate(char *cmd, int *arg_index, int args)
{
    boolean             Permanently = FALSE;
    COPS_ReturnCode_t   rc;

    if (args > 0) {
        char *param = cmd + arg_index[0];
        if (*param == '1')
            Permanently = TRUE;
    }

    COPS_LOG(LOG_INFO, "[COPS]SWBP Deauthenticate. Perm=%s [START]\n",
             Permanently ? "TRUE" : "FALSE");
    Permanently = FALSE; /* Always set this to false for now! */

    rc = Do_COPS_Deauthenticate(Permanently);

    if (rc != COPS_RETURN_CODE_OK) {
        COPS_IDBG_PRINT("Deauthenticate failed, rc = 0x%x (%s)", rc,
                cops_swbp_ReturnCodeDesc(rc));
    } else {
        COPS_IDBG_PRINT("Deauthenticate succeeded");
    }
}

static void cops_swbp_GetDeviceState(char *cmd, int *arg_index, int args)
{
    COPS_DeviceState_t  DeviceState;
    COPS_ReturnCode_t   rc;

    COPS_LOG(LOG_INFO, "[COPS]SWBP GetDeviceState [START]\n");

    rc = Do_COPS_GetDeviceState(&DeviceState);

    if (rc != COPS_RETURN_CODE_OK) {
        COPS_IDBG_PRINT("GetDeviceState failed, rc = 0x%x (%s)", rc,
                cops_swbp_ReturnCodeDesc(rc));
    } else {
        COPS_IDBG_PRINT("State = %s (%d)",
                DeviceState == COPS_DEVICESTATE_FULL ? "FULL" : "RELAXED",
                DeviceState);
    }
}

/* Syntax: BindProperties <imei> <ControlKeys> <COPS_DATA>
 *  imei: 0=No IMEI, 1=350050500009692, 2=123456789012347, 3=999999999999994
 * ControlKeys: 0: No Control Keys, 1:All "11111111"
 *              2: "2222222222222222"
 *              3: "11111111", "22222222", ... , "55555555"
 */
static void cops_swbp_BindProperties(char *cmd, int *arg_index, int args)
{
    COPS_BindPropertiesArg_t  BPArg;
    COPS_IMEI_t               imei;
    COPS_AuthData_t           AuthData;
    COPS_ReturnCode_t         rc;
    size_t                    n;
    char                      *param;
    const char                *imei_data = NULL;
    SIMLock_ControlKeys_t     SimLock_CKs;

    const char *const Test_IMEI_1 = "350050500009692";
    const char *const Test_IMEI_2 = "123456789012347";
    const char *const Test_IMEI_3 = "999999999999994";

    COPS_LOG(LOG_INFO, "[COPS]SWBP BindProperties [START]\n");

    memset(&BPArg, 0, sizeof(BPArg));
    memset(&imei, 0, sizeof(imei));

    if (args < 3) {
        goto UsageError;
    }

    /* IMEI */
    param = cmd + arg_index[0];
    COPS_LOG(LOG_INFO, "Param = %s\n", param);
    if (*param == '1')
        imei_data = Test_IMEI_1;
    else if (*param == '2')
        imei_data = Test_IMEI_2;
    else if (*param == '3')
        imei_data = Test_IMEI_3;
    if (imei_data != NULL) {
        for (n = 0; n < COPS_UNPACKED_IMEI_LENGTH; n++) {
            imei.Digits[n] = imei_data[n] - '0';
        }
        BPArg.IMEI_p = &imei;
        COPS_LOG(LOG_INFO, "BindProperties. Imei=%s\n", imei_data);
    }

    /* Authentication data (ControlKeys) */
    memset(&SimLock_CKs, 0, sizeof(SimLock_CKs));
    param = cmd + arg_index[1];
    if (*param >= '1' && *param <= '3') {
        if (*param == '1') {
            memcpy(&SimLock_CKs, &Test_CK_1, sizeof(SimLock_CKs));
        } else if (*param == '2') {
            memcpy(&SimLock_CKs, &Test_CK_2, sizeof(SimLock_CKs));
        } else {
            memcpy(&SimLock_CKs, &Test_CK_3, sizeof(SimLock_CKs));
        }
        AuthData.AuthType = COPS_AUTH_SIMLOCK_KEYS;
        AuthData.Length = sizeof(SimLock_CKs);
        AuthData.Data_p = (uint8_t *)&SimLock_CKs;
        BPArg.NumNewAuthData = 1;
        BPArg.AuthData_p = &AuthData;
    }

    /* COPS Data */
    param = cmd + arg_index[2];
    if (*param == '1') {
        BPArg.COPS_DataLength = sizeof(TestCopsData_NL);
        BPArg.COPS_Data_p = TestCopsData_NL;
        BPArg.Merge_COPS_Data = TRUE;
    } else if (*param == '2') {
        BPArg.COPS_DataLength = sizeof(TestCopsData_NL_L);
        BPArg.COPS_Data_p = TestCopsData_NL_L;
        BPArg.Merge_COPS_Data = TRUE;
    } else if (*param == '3') {
        BPArg.COPS_DataLength = sizeof(TestCopsData_NL_L1);
        BPArg.COPS_Data_p = TestCopsData_NL_L1;
        BPArg.Merge_COPS_Data = TRUE;
    }

    rc = Do_COPS_BindProperties(&BPArg);

    if (rc != COPS_RETURN_CODE_OK) {
        COPS_IDBG_PRINT("BindProperties failed, rc = 0x%x (%s)", rc,
                cops_swbp_ReturnCodeDesc(rc));
    } else {
        COPS_IDBG_PRINT("BindProperties succeeded");
    }
    return;
UsageError:
  COPS_IDBG_PRINT("Usage: BindProperties [IMEI] [ControlKeys] [COPS_DATA]");
  COPS_IDBG_PRINT(" IMEI: 0=No change, 1=%s, 2=%s, 3=%s, 15_digit_IMEI",
          Test_IMEI_1, Test_IMEI_2, Test_IMEI_3);
  COPS_IDBG_PRINT(" ControlKeys: 0=No change, 1:All '11111111', "
          "2:All '2222222222222222', 3:'11111111','22222222',...'55555555'");
  COPS_IDBG_PRINT(" COPS_DATA: 0=No change, 1:NL unlocked, 2:NL locked");
}

/* Read COPS data */
static void cops_swbp_ReadData(char *cmd, int *arg_index, int args)
{
    uint8               Buffer[1024];
    uint32              BufferLength;
    COPS_ReturnCode_t   rc;

    /* Get size of data */
    memset(Buffer, 0, sizeof(Buffer));
    BufferLength = 0;
    rc = Do_COPS_ReadData(NULL, &BufferLength);
    if (rc != COPS_RETURN_CODE_OK) {
        COPS_IDBG_PRINT("ReadData(size) failed, rc = 0x%x (%s)", rc,
                cops_swbp_ReturnCodeDesc(rc));
    }
    /* Get actual data */
    rc = Do_COPS_ReadData(Buffer, &BufferLength);

    if (rc != COPS_RETURN_CODE_OK) {
        COPS_IDBG_PRINT("ReadData failed, rc = 0x%x (%s)", rc,
                         cops_swbp_ReturnCodeDesc(rc));
    }

    /* Print data even if ReadData returned an error */
    COPS_IDBG_HEX("COPS Data", Buffer, BufferLength);
}

static char *GetCOPSParameterDesc(uint16 ParamId)
{
    switch (ParamId) {
    case COPS_PARAMETER_ID_IMEI:
        return "IMEI";
    case COPS_PARAMETER_ID_SIMLOCK_DEFAULT_KEY:
        return "SimLock-Default-CK";
    case COPS_PARAMETER_ID_LOCK_SETTING_NL:
        return "LockSetting-NL";
    case COPS_PARAMETER_ID_LOCK_SETTING_NSL:
        return "LockSetting-NSL";
    case COPS_PARAMETER_ID_LOCK_SETTING_SPL:
        return "LockSetting-SPL";
    case COPS_PARAMETER_ID_LOCK_SETTING_CL:
        return "LockSetting-CL";
    case COPS_PARAMETER_ID_LOCK_SETTING_SIML:
        return "LockSetting-SIML";
    case COPS_PARAMETER_ID_LOCK_SETTING_ESLL:
        return "LockSetting-ESLL";
    case COPS_PARAMETER_ID_LOCK_SETTING_TESTSIM:
        return "LockSetting-TESTSIM";
    case COPS_PARAMETER_ID_LOCK_CONTROL_NL:
        return "LockControl-NL";
    case COPS_PARAMETER_ID_LOCK_CONTROL_NSL:
        return "LockControl-NSL";
    case COPS_PARAMETER_ID_LOCK_CONTROL_SPL:
        return "LockControl-SPL";
    case COPS_PARAMETER_ID_LOCK_CONTROL_CL:
        return "LockControl-CL";
    case COPS_PARAMETER_ID_LOCK_CONTROL_SIML:
        return "LockControl-SIML";
    case COPS_PARAMETER_ID_LOCK_CONTROL_ESLL:
        return "LockControl-ESL";
    case COPS_PARAMETER_ID_LOCK_DEFINITION_NL:
        return "LockDef-NL";
    case COPS_PARAMETER_ID_LOCK_DEFINITION_NSL:
        return "LockDef-NSL";
    case COPS_PARAMETER_ID_LOCK_DEFINITION_SPL:
        return "LockDef-SPL";
    case COPS_PARAMETER_ID_LOCK_DEFINITION_CL:
        return "LockDef-CL";
    case COPS_PARAMETER_ID_LOCK_DEFINITION_SIML:
        return "LockDef-SIML";
    case COPS_STORAGE_PARAMETER_ID_SUBDOMAIN:
        return "SubDomain";
    case COPS_STORAGE_PARAMETER_ID_TPID:
        return "TPID";
    case COPS_STORAGE_PARAMETER_ID_MCK_ATTEMPTS:
        return "MCK-ATTEMPTS";
    case COPS_PARAMETER_ID_MODEM_DATA:
        return "Modem-Data";
    case COPS_PARAMETER_ID_SIMLOCK_KEY_NL:
        return "CK-NL";
    case COPS_PARAMETER_ID_SIMLOCK_KEY_NSL:
        return "CK-NSL";
    case COPS_PARAMETER_ID_SIMLOCK_KEY_SPL:
        return "CK-SPL";
    case COPS_PARAMETER_ID_SIMLOCK_KEY_CL:
        return "CK-CL";
    case COPS_PARAMETER_ID_SIMLOCK_KEY_SIML:
        return "CK-SIML";
    case COPS_PARAMETER_ID_SIMLOCK_KEY_ESLL:
        return "CK-ESL";
    case COPS_PARAMETER_ID_SIMLOCK_MASTER_CONTROL_KEY:
        return "Master CK";
    case COPS_PARAMETER_ID_PERMANENT_AUTHENTICATION:
        return "Perm authentication";
    case COPS_PARAMETER_ID_COUNTER:
        return "Counter";
    case COPS_PARAMETER_ID_MAC:
        return "MAC";
    default:
        return "Unknown";
    }
}

/* Help function. Will print parameter data from COPS_DATA_0.CSD */
static void PrintCOPSParameters(uint8 *Buffer_p, uint32 BufferLength)
{
  uint32 Pos = 4; /* Skip header */
  uint16 ParamID;
  uint16 ParamSize;

  if (Buffer_p == NULL || BufferLength < 4)
    return;

  /* Header (Version) */
  COPS_IDBG_PRINT("Version = 0x%02x %02x, 0x%02x %02x",
      Buffer_p[0], Buffer_p[1], Buffer_p[2], Buffer_p[3]);

  /* Parse COPS data buffer */
  while (Pos + 4 < BufferLength) {
    memcpy(&ParamID, &Buffer_p[Pos], 2);
    memcpy(&ParamSize, &Buffer_p[Pos+2], 2);

    COPS_IDBG_PRINT("ParamID = 0x%x (%s), Size=%d", ParamID,
            GetCOPSParameterDesc(ParamID), ParamSize);
    if (Pos + ParamSize <= BufferLength) {
        /* Print max 320 bytes of parameter data */
        COPS_IDBG_HEX("ParamData", &Buffer_p[Pos+4], MIN(ParamSize-4, 320));
    }
    Pos += ParamSize;
  }
}

/* Read COPS data, parameter by parameter */
static void cops_swbp_ReadParameters(char *cmd, int *arg_index, int args)
{
    uint8               Buffer[1024];
    uint32              BufferLength;
    COPS_ReturnCode_t   rc;

    /* Get size of data */
    BufferLength = 0;
    rc = Do_COPS_ReadData(NULL, &BufferLength);
    if (rc != COPS_RETURN_CODE_OK) {
        COPS_IDBG_PRINT("ReadData(size) failed, rc = 0x%x (%s)", rc,
                cops_swbp_ReturnCodeDesc(rc));
    } else {
        /* Get actual data */
        rc = Do_COPS_ReadData(Buffer, &BufferLength);

        if (rc != COPS_RETURN_CODE_OK) {
            COPS_IDBG_PRINT("ReadData failed, rc = 0x%x (%s)", rc,
                    cops_swbp_ReturnCodeDesc(rc));
        } else {
            PrintCOPSParameters(Buffer, BufferLength);
        }
    }
}

/* Create and return string of IMSI digits in an IMSI range (EDN format) */
static char *cops_swbp_GetImsiRangeString(uint16 Mask)
{
    static char range[16];

    memset(range, 0, sizeof(range));

    if (((Mask&0x3FF) == 0x3FF) || Mask == 0) {
        strcpy(range, "*");
    } else {
        int i;
        int offset = 0;

        for (i = 0; i <= 9; i++) {
            if ((Mask&0x01) == 1) {
                range[offset] = '0' + i;
                offset++;
            }
            Mask = Mask >> 1;
        }
    }

    return range;
}

/*
 * 5 bytes are used for 4 IMSI digits (10 bits/digit)
 * The following macros are used for parsing IMSI digits in EDN format
 */
#define DIGIT_0(data, offset) \
    ((((uint16)data[offset]&0x00FF)>>0) + (((uint16)data[offset+1]&0x0003)<<8))
#define DIGIT_1(data, offset) \
    ((((uint16)data[offset]&0x00FC)>>2) + (((uint16)data[offset+1]&0x000F)<<6))
#define DIGIT_2(data, offset) \
    ((((uint16)data[offset]&0x00F0)>>4) + (((uint16)data[offset+1]&0x003F)<<4))
#define DIGIT_3(data, offset) \
    ((((uint16)data[offset]&0x00C0)>>6) + (((uint16)data[offset+1]&0x00FF)<<2))

/*
 * Parse lock settings data
 * Extract IMSI ranges and GID values (EDN format)
 */
static void cops_swbp_PrintLockSettingsData(uint8 *ParamData_p,
                                            uint32 ParamDataLength)
{
    uint8  LockSetting;

    if (ParamData_p == NULL || ParamDataLength < 1) {
        COPS_IDBG_PRINT("Parameter data not correct (%d)", ParamDataLength);
        return;
    }

    /* Mandatory LockSettings data */
    LockSetting = ParamData_p[0];

    COPS_IDBG_PRINT("LockSetting = %d (%s)", LockSetting,
        LockSetting == SIMLOCK_LOCK_SETTING_UNLOCKED ? "Unlocked" :
        LockSetting == SIMLOCK_LOCK_SETTING_LOCKED ? "Locked" :
        LockSetting == SIMLOCK_LOCK_SETTING_DISABLED ? "Disabled" :
        LockSetting == SIMLOCK_LOCK_SETTING_AUTOLOCK_ENABLED ? "Autolock" :
        LockSetting == SIMLOCK_LOCK_SETTING_AUTOLOCK_TO_CNL_ENABLED ?
                "AutoL-CNL" : "???");

    /* Parse IMSI ranges, if there are any */
    if (ParamDataLength > 1) {
        uint16 offset = 1;
        while (offset < ParamDataLength - 1) {
            uint8   NbrOfImsiDigits;
            boolean GID_present[2];
            uint16  Mask;
            uint8   Digit;
            char    buf[16];
            char    textout[256];
            boolean ImsiBitsLeft = FALSE;
            uint8   i;

            /* Parse EDN header */
            NbrOfImsiDigits = ParamData_p[offset] & 0x0F;
            GID_present[0] = (ParamData_p[offset] & 0x10) != 0;
            GID_present[1] = (ParamData_p[offset] & 0x20) != 0;
            offset++;

            memset(textout, 0, sizeof(textout));
            for (Digit = 0; Digit < NbrOfImsiDigits; Digit++) {
                if (Digit%4 == 0) {
                    Mask = DIGIT_0(ParamData_p, offset);
                    offset++;
                    ImsiBitsLeft = TRUE; /* 6 bits left for next digit */
                } else if (Digit%4 == 1) {
                    Mask = DIGIT_1(ParamData_p, offset);
                    offset++;
                    ImsiBitsLeft = TRUE; /* 4 bits left for next digit */
                } else if (Digit%4 == 2) {
                    Mask = DIGIT_2(ParamData_p, offset);
                    offset++;
                    ImsiBitsLeft = TRUE; /* 2 bits left for next digit */
                } else {
                    Mask = DIGIT_3(ParamData_p, offset);
                    offset += 2;
                    ImsiBitsLeft = FALSE;
                }
                sprintf(buf, "[%s]", cops_swbp_GetImsiRangeString(Mask));
                if (Digit > 0)
                    strcat(textout, ",");
                strcat(textout, buf);
            }
            COPS_IDBG_PRINT("IMSI range: %s", textout);

            /* Are there bits left for IMSI byte range? */
            if (ImsiBitsLeft)
                offset++;

            /* GID1 and GID2 */
            for (i = 0; i < 2; i++) {
                if (GID_present[i]) {
                    uint8 NbrOfGids = ParamData_p[offset];
                    offset++;

                    memset(textout, 0, sizeof(textout));

                    while (NbrOfGids > 0) {
                        sprintf(buf, "%d ", ParamData_p[offset]);
                        strcat(textout, buf);
                        NbrOfGids--;
                        offset++;
                    }
                    COPS_IDBG_PRINT("  GID%d: %s", i+1, textout);
                }
            }
        }
    }
}

/* Find COPS parameter
 * Usage FindParameter <ParamId>
 */
static void cops_swbp_FindParam(char *cmd, int *arg_index, int args)
{
    uint8               COPS_Data[1024];
    uint32              COPS_DataLength = sizeof(COPS_Data);
    COPS_ReturnCode_t   rc;
    int                 ParamId;
    char               *EndPtr = NULL;

    /* Get Param ID */
    if (args < 1) {
        COPS_IDBG_PRINT("Usage: FindParam [ParamId in Hex]");
        return;
    }
    ParamId = (int)strtoul(cmd + arg_index[0], &EndPtr, HEX_BASE);

    /* Get COPS-Data */
    rc = Do_COPS_ReadData(COPS_Data, &COPS_DataLength);

    if (rc != COPS_RETURN_CODE_OK) {
        COPS_IDBG_PRINT("ReadData failed, rc = 0x%x (%s)", rc,
                cops_swbp_ReturnCodeDesc(rc));
    } else {
        uint8  *ParamData_p = NULL;
        size_t  ParamDataLength = 0;

        rc = Do_COPS_FindParameter(COPS_Data,
                                   COPS_DataLength,
                                   ParamId,
                                   &ParamData_p,
                                   &ParamDataLength);
        if (rc != COPS_RETURN_CODE_OK) {
            COPS_IDBG_PRINT("FindParameter failed, rc = 0x%x (%s)", rc,
                    cops_swbp_ReturnCodeDesc(rc));
        } else {
            COPS_IDBG_HEX("Param data", ParamData_p, ParamDataLength);

            /* Write detailed info about some parameters */
            if (ParamId >= 0x10 && ParamId < 0x20) {
                cops_swbp_PrintLockSettingsData(ParamData_p, ParamDataLength);
            }
        }
    }
}

/* Bind data
 * Usage BindData <TestDataIndex> (<SizeOfData>)
 *   TestDataIndex: 1,2,3,4
 */
static void cops_swbp_BindData(char *cmd, int *arg_index, int args)
{
    COPS_ReturnCode_t   rc;
    int                 Id;
    uint8              *Data_p;
    uint32              DataLength;
    boolean             malloc_used = FALSE;
    COPS_MAC_t          MAC;

    /* Get TestDataID */
    if (args < 1) {
        goto UsageError;
    }
    Id = atoi(cmd + arg_index[0]);

    if (Id == 1) {
        Data_p = TestData_1;
        DataLength = sizeof(TestData_1);
    } else if (Id == 2) {
        Data_p = TestData_2;
        DataLength = sizeof(TestData_2);
    } else if (Id == 3) {
        Data_p = TestData_3;
        DataLength = sizeof(TestData_3);
    } else if (Id == 4) {
        /* Varying data size */
        uint32 i;

        if (args < 2) {
            goto UsageError;
        }
        DataLength = atoi(cmd + arg_index[1]);

        Data_p = (uint8 *)CONSECUTIVE_HEAP_UNSAFE_UNTYPED_ALLOC(DataLength);
        if (Data_p == NULL) {
            COPS_IDBG_PRINT("Can't alloc consecutive data");
            Data_p = (uint8 *)malloc(DataLength);
            malloc_used = TRUE;
            if (Data_p == NULL) {
                COPS_IDBG_PRINT("Can't alloc data at all");
                return;
            }
        }

        for (i = 0; i < DataLength; i++)
            Data_p[i] = 0x57 + i%127;
    } else {
        goto UsageError;
    }

    if (DataLength < 128)
        COPS_IDBG_HEX("Data", Data_p, DataLength);
    else
        COPS_IDBG_HEX("Data (first part)", Data_p, 128);

    rc = Do_COPS_BindData(Data_p, DataLength, &MAC);

    if (rc != COPS_RETURN_CODE_OK) {
        COPS_IDBG_PRINT("BindData failed, rc = 0x%x (%s)", rc,
                cops_swbp_ReturnCodeDesc(rc));
    } else {
        COPS_IDBG_HEX("MAC", &MAC, sizeof(MAC));
    }

    if (Id == 4) {
        if (Data_p != NULL && malloc_used) {
            /*lint -e673 */
            free(Data_p);
            /*lint +e673 */
        } else if (Data_p != NULL) {
            CONSECUTIVE_HEAP_UNSAFE_FREE(&Data_p);
        }
    }

    return;

UsageError:
    COPS_IDBG_PRINT("Usage: BindData [TestDataId] [SizeOfData]");
    COPS_IDBG_PRINT(" TestDataId: Small data size=1,2,3 Random data=4");
    COPS_IDBG_PRINT(" SizeOfData: Only used when TestDataId = 4");
}

/* Verify data binding
 * Usage VerifyDataBinding <TestDataId> <MAC(in AsciiHex)>
 */
static void cops_swbp_VerifyDataBinding(char *cmd, int *arg_index, int args)
{
    COPS_ReturnCode_t   rc;
    int                 Id;
    uint8              *Data_p;
    uint32              DataLength;
    COPS_MAC_t          MAC;
    char               *Param;
    char                HexValue[2];
    int                 i;

    /* Get TestDataID */
    if (args < 2) {
        goto UsageError;
    }
    Id = atoi(cmd + arg_index[0]);

    if (Id == 1) {
        Data_p = TestData_1;
        DataLength = sizeof(TestData_1);
    } else if (Id == 2) {
        Data_p = TestData_2;
        DataLength = sizeof(TestData_2);
    } else if (Id == 3) {
        Data_p = TestData_3;
        DataLength = sizeof(TestData_3);
    } else {
        goto UsageError;
    }

    /* MAC to check */
    Param = cmd + arg_index[1];
    if (strlen(Param) != COPS_MAC_SIZE*2) {
        COPS_IDBG_PRINT("  Current entered MAC size=%d, should be %d",
                strlen(Param), COPS_MAC_SIZE*2);
        goto UsageError;
    }
    for (i = 0; i < COPS_MAC_SIZE*2; i += 2) {
        HexValue[0] = Param[i+0];
        HexValue[1] = Param[i+1];
        MAC.Value[i/2] = Str2Hex(HexValue);
    }
    COPS_IDBG_HEX("Data", Data_p, DataLength); /* TEMP!!!! */
    COPS_IDBG_HEX("MAC", &MAC, sizeof(MAC)); /* TEMP!!!! */

    rc = Do_COPS_VerifyDataBinding(Data_p, DataLength, &MAC);

    if (rc != COPS_RETURN_CODE_OK) {
        COPS_IDBG_PRINT("VerifyDataBinding failed, rc = 0x%x (%s)", rc,
                cops_swbp_ReturnCodeDesc(rc));
    } else {
        COPS_IDBG_PRINT("VerifyDataBinding succeeded");
    }

    return;

UsageError:
    COPS_IDBG_PRINT("Usage: VerifyDataBinding [TestDataId] [MAC]");
    COPS_IDBG_PRINT(" TestDataId: 1-3 Predefined sets of data");
    COPS_IDBG_PRINT(" MAC: 32 bytes AsciiHex (e.g. BA34BE345F45...A9)");
}

/* Verify signed header
 * Usage VerifySignedHeader <TestDataId>
 */
static void cops_swbp_VerifySignedHeader(char *cmd, int *arg_index, int args)
{
    COPS_ReturnCode_t   rc;
    int                 Id;
    uint8              *Data_p = NULL;
    uint32              DataLength;
    COPS_PayloadType_t          PayloadType = COPS_PAYLOADTYPE_MODEM_CODE;
    COPS_VerifySignedHeader_t   HeaderInfo;

    /* Get TestDataID */
    if (args < 1) {
        goto UsageError;
    }
    Id = atoi(cmd + arg_index[0]);

    if (Id == 1) {
        /* Random data. Cannot be verified */
        Data_p = TestData_3;
        DataLength = sizeof(TestData_3);
    } else if (Id == 2) {
        /* payload type ape_normal */
        Data_p = TestSignedData_ape_normal;
        DataLength = sizeof(TestSignedData_ape_normal);
        PayloadType = (COPS_PayloadType_t)5;
    } else if (Id == 3) {
        /* ITP data */
        Data_p = TestSignedData_itp;
        DataLength = sizeof(TestSignedData_itp);
        PayloadType = COPS_PAYLOADTYPE_ITP;
    } else if (Id == 4) {
        /* Calibration data (of type ITP) */
        Data_p = TestSignedData_itp_calibdata;
        DataLength = sizeof(TestSignedData_itp_calibdata);
        PayloadType = COPS_PAYLOADTYPE_ITP;
    } else {
        goto UsageError;
    }

    /* COPS_IDBG_HEX("Data", Data_p, DataLength); */

    rc = Do_COPS_VerifySignedHeader(Data_p, PayloadType, &HeaderInfo);

    if (rc != COPS_RETURN_CODE_OK) {
        COPS_IDBG_PRINT("Do_COPS_VerifySignedHeader failed, rc = 0x%x (%s)", rc,
                cops_swbp_ReturnCodeDesc(rc));
    } else {
        COPS_Hash_t PayloadHash;
        uint8 *PayloadData_p = Data_p + HeaderInfo.HeaderSize;
        uint32 PayloadSize = HeaderInfo.PayloadSize;

        COPS_IDBG_PRINT("Do_COPS_VerifySignedHeader succeeded");
        COPS_IDBG_PRINT("Size of header data=%d", DataLength);
        COPS_IDBG_PRINT("HeaderSize=%d, PayloadSize=%d",
                HeaderInfo.HeaderSize, HeaderInfo.PayloadSize);
        COPS_IDBG_PRINT("HashType=0x%x, PayloadType=0x%x",
                HeaderInfo.HashType, HeaderInfo.PayloadType);

      /* Calc hash, and compare with expected */
        rc = Do_COPS_CalculateDigest(HeaderInfo.HashType,
                                     PayloadData_p,
                                     PayloadSize,
                                     &PayloadHash);
        if (rc != COPS_RETURN_CODE_OK) {
            COPS_IDBG_PRINT("CalculateDigest failed, rc = 0x%x (%s)", rc,
                    cops_swbp_ReturnCodeDesc(rc));
        } else {
            if (0 == memcmp(&HeaderInfo.ExpectedHash, &PayloadHash,
                            sizeof(HeaderInfo.ExpectedHash))) {
                COPS_IDBG_PRINT("Payload hash is correct");
            } else {
                COPS_IDBG_PRINT("Payload hash is NOT correct!");
                COPS_IDBG_HEX("PayloadHash", &PayloadHash, COPS_HASH_LENGTH);
                COPS_IDBG_HEX("Expected PayloadHash",
                        &HeaderInfo.ExpectedHash, COPS_HASH_LENGTH);
            }
        }
    }

    return;
UsageError:
    COPS_IDBG_PRINT("Usage: VerifySignedHeader [TestDataId]");
    COPS_IDBG_PRINT(" TestDataId: 1,2,3,4");
}

/* Calculate message digest
 * Usage CalcDigest <HashType> <HashData>
 *  HashType: 1=SHA1, 2=SHA256, 16=SHA1_HMAC, 32=SHA256_HMAC
 *  HashData: 1=TestData_1, 2=TestData_2, 3=TestData_3, 4=Random data, HexData
 */
static void cops_swbp_CalcDigest(char *cmd, int *arg_index, int args)
{
    COPS_ReturnCode_t   rc;
    COPS_HashType_t     HashType;
    int                 DataType;
    uint8              *Data_p;
    uint32              DataLength;
    boolean             malloc_used = FALSE;
    COPS_Hash_t         Hash;
    size_t              HashSize;
    uint8               HexData[64];

    memset(&Hash, 0, sizeof(Hash));

    if (args < 2) {
        goto UsageError;
    }

    HashType = (COPS_HashType_t)atoi(cmd + arg_index[0]);
    if (HashType == COPS_HASHTYPE_SHA1_HASH ||
        HashType == COPS_HASHTYPE_SHA1_HMAC) {
        HashSize = COPS_HASH_LENGTH_SHA1;
    } else if (HashType == COPS_HASHTYPE_SHA256_HASH ||
               HashType == COPS_HASHTYPE_SHA256_HMAC) {
        HashSize = COPS_HASH_LENGTH_SHA256;
    } else {
        goto UsageError;
    }

    DataType = atoi(cmd + arg_index[1]);
    if (DataType == 1) {
        Data_p = TestData_1;
        DataLength = sizeof(TestData_1) - 1; /* Not including \0 */
    } else if (DataType == 2) {
        Data_p = TestData_2;
        DataLength = sizeof(TestData_2);
    } else if (DataType == 3) {
        Data_p = TestData_3;
        DataLength = sizeof(TestData_3);
    } else if (DataType == 4) {
        uint32 i;

        if (args < 3) {
            goto UsageError;
        }
        DataLength = atoi(cmd + arg_index[2]);

        Data_p = (uint8 *)CONSECUTIVE_HEAP_UNSAFE_UNTYPED_ALLOC(DataLength);
        if (Data_p == NULL) {
            COPS_IDBG_PRINT("Can't alloc consecutive data");
            Data_p = (uint8 *)malloc(DataLength);
            malloc_used = TRUE;
            if (Data_p == NULL) {
                COPS_IDBG_PRINT("Can't alloc data at all");
                return;
            }
        }

        for (i = 0; i < DataLength; i++)
            Data_p[i] = 0x57 + i%127;
    } else {
        size_t i;
        uint32 Offset = 0;
        char *ASCIIHexData_p = cmd + arg_index[1];

        if (strlen(ASCIIHexData_p)%2 != 0)
            goto UsageError;
        else if (strlen(ASCIIHexData_p)/2 > sizeof(HexData))
            goto UsageError;

        /* Convert ASCIIHex to byte data */
        for (i = 0; i < strlen(ASCIIHexData_p); i += 2) {
            HexData[Offset] = Str2Hex(ASCIIHexData_p+i);
            Offset++;
        }
        Data_p = HexData;
        DataLength = Offset;
    }

    COPS_IDBG_HEX("Data", Data_p, MIN(DataLength, 256));

    rc = Do_COPS_CalculateDigest(HashType, Data_p, DataLength, &Hash);

    if (rc != COPS_RETURN_CODE_OK) {
        COPS_IDBG_PRINT("CalculateDigest failed, rc = 0x%x (%s)", rc,
                cops_swbp_ReturnCodeDesc(rc));
    } else {
        COPS_IDBG_HEX("Hash", &Hash, HashSize);
    }

    if (DataType == 4) {
        if (Data_p != NULL && malloc_used) {
            /*lint -e673 */
            free(Data_p);
            /*lint +e673 */
        } else if (Data_p != NULL) {
            CONSECUTIVE_HEAP_UNSAFE_FREE(&Data_p);
        }
    }
    DELAY_MS(300);

    return;
UsageError:
    COPS_IDBG_PRINT("Usage: CalcDigest <HashType> <HashData> <SizeOfData>");
    COPS_IDBG_PRINT(" HashType: 1=SHA1 2=SHA256 16=SHA1_HMAC 32=SHA256_HMAC");
    COPS_IDBG_PRINT(" HashData: 1-3 (pre-defined data sets)");
    COPS_IDBG_PRINT(" HashData: 4 (random data alloocated on heap)");
    COPS_IDBG_PRINT(" HashData: Any ASCIIHex data (max 64 bytes)");
    COPS_IDBG_PRINT(" SizeOfData: Only used when HashData = 4");
}

/* Read OTP data
 * Usage ReadOTP
 */
static void cops_swbp_ReadOTP(char *cmd, int *arg_index, int args)
{
    COPS_ReturnCode_t   rc;

    uint8 OTP_Buf8[184] = {
    0x04, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
    0x05, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
    0x06, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
    0x19, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
    0x1A, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
    0x1B, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
    0x1C, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
    0x1D, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
    0x1E, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
    0x1F, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
    0x20, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
    0x21, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
    0x22, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
    0x23, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
    0x24, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
    0x25, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
    0x26, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
    0x27, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
    0x28, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
    0x29, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
    0x2A, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
    0x2B, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
    0x2C, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00 };
    uint32 OTP_Buf[184/4];

    memcpy(OTP_Buf, OTP_Buf8, sizeof(OTP_Buf8));

    COPS_IDBG_HEX("OTP_Buf(in)", OTP_Buf, sizeof(OTP_Buf));

    rc = Do_COPS_Read_OTP(OTP_Buf, sizeof(OTP_Buf));

    if (rc != COPS_RETURN_CODE_OK) {
        COPS_IDBG_PRINT("ReadOTP failed, rc = 0x%x (%s)", rc,
                cops_swbp_ReturnCodeDesc(rc));
    } else {
        COPS_IDBG_HEX("OTP_Buf(out)", OTP_Buf, sizeof(OTP_Buf));
    }
}

/* Activate SIMLock
 * Usage Lock <LockType> <ControlKey> <IMSI>
 *  IMSI: Which IMSI digits to lock to (5-8 digits). Not mandatory
 */
static void cops_swbp_Lock(char *cmd, int *arg_index, int args)
{
    SIMLock_ReturnCode_t    rc;
    SIMLock_LockArg_t       LockArg;
    char                   *CK_p;
    SIMLock_ExplicitLockData_t ExpLockData;
    size_t                  i;

    memset(&LockArg, 0, sizeof(LockArg));
    memset(&ExpLockData, 0, sizeof(ExpLockData));

    LockArg.LockData.UpdateLockDefinition = FALSE;
    LockArg.LockData.SetLock = TRUE;
    LockArg.ExplicitLockData_p = NULL;

    if (args < 3) {
        goto UsageError;
    }

    LockArg.LockData.LockType = (SIMLock_Type_t)atoi(cmd + arg_index[0]);
    if (LockArg.LockData.LockType >= NUMBER_OF_SIMLOCKS) {
        COPS_IDBG_PRINT("Unknown LockType");
        goto UsageError;
    }

    LockArg.LockMode = (SIMLock_LockMode_t)atoi(cmd + arg_index[1]);
    if (LockArg.LockMode >= SIMLOCK_LOCK_MODE_RESERVED) {
        COPS_IDBG_PRINT("Unknown LockMode");
        goto UsageError;
    }

    /* Validate control key */
    CK_p = cmd + arg_index[2];
    if (strlen(CK_p) < COPS_SIMLOCK_SIM_CONTROLKEY_MIN_LENGTH ||
        strlen(CK_p) > COPS_SIMLOCK_CONTROLKEY_MAX_LENGTH) {
        COPS_IDBG_PRINT("ControlKey must be 6-16 digits");
        /* continue anyway */
    }
    for (i = 0; i < strlen(CK_p); i++) {
        if (CK_p[i] < '0' || CK_p[i] > '9') {
            COPS_IDBG_PRINT("ControlKey must consist of digits");
        }
        LockArg.LockData.ControlKey.Value[i] = CK_p[i];
    }

    if (args == 4) {
        char *IMSI_p = cmd + arg_index[3];
        memset(ExpLockData.IMSI.Data, 0xFF, SIM_IMSI_LEN);

        for (i = 0; i < strlen(IMSI_p); i++) {
            ExpLockData.IMSI.Data[i] = IMSI_p[i] - '0';
        }
        LockArg.ExplicitLockData_p = &ExpLockData;
    }

    COPS_IDBG_HEX("LockArg", &LockArg, sizeof(LockArg));

    rc = Do_SIMLock_Lock(&LockArg);

    if (rc != COPS_RETURN_CODE_OK) {
        COPS_IDBG_PRINT("Lock failed, rc = 0x%x (%s)", rc,
                cops_swbp_SimLockReturnCodeDesc(rc));
    } else {
        COPS_IDBG_PRINT("Lock set");
    }

    return;
UsageError:
    COPS_IDBG_PRINT("Usage: Lock <LockType> <LockMode> <ControlKey> <IMSI>");
    COPS_IDBG_PRINT("  LockType must be <%d", NUMBER_OF_SIMLOCKS);
    COPS_IDBG_PRINT("  LockMode must be <%d", SIMLOCK_LOCK_MODE_RESERVED);
    COPS_IDBG_PRINT("  ControlKey must be 6/8-16 digits");
    COPS_IDBG_PRINT("  IMSI is optional (used as explicit lock data)");
}

/* Deactivate SIMLock
 * Usage Unlock <LockType> <ControlKey>
 */
static void cops_swbp_Unlock(char *cmd, int *arg_index, int args)
{
    SIMLock_ReturnCode_t    rc;
    SIMLock_Type_t          LockType;
    char                   *CK_p;
    size_t                  i;
    SIMLock_ControlKey_t    ControlKey;

    if (args < 2) {
        COPS_IDBG_PRINT("Usage: Unlock <LockType> <ControlKey>");
        return;
    }
    LockType = (SIMLock_Type_t)atoi(cmd + arg_index[0]);
    if (LockType >= NUMBER_OF_SIMLOCKS) {
        COPS_IDBG_PRINT("Unknown LockType");
        return;
    }

    /* Validate control key */
    memset(&ControlKey, 0, sizeof(ControlKey));
    CK_p = cmd + arg_index[1];
    if (strlen(CK_p) < COPS_SIMLOCK_SIM_CONTROLKEY_MIN_LENGTH ||
        strlen(CK_p) > COPS_SIMLOCK_CONTROLKEY_MAX_LENGTH) {
        COPS_IDBG_PRINT("ControlKey must be 6-16 digits");
        /* continue anyway */
    }
    for (i = 0; i < strlen(CK_p); i++) {
        if (CK_p[i] < '0' || CK_p[i] > '9') {
            COPS_IDBG_PRINT("ControlKey must consist of digits");
            /* continue anyway */
        }
        ControlKey.Value[i] = CK_p[i];
    }

    COPS_IDBG_PRINT("LockType=%d, CK=%s", LockType, CK_p);
    DELAY_MS(300);

    rc = Do_SIMLock_Unlock(LockType, &ControlKey);

    if (rc != COPS_RETURN_CODE_OK) {
        COPS_IDBG_PRINT("Unlock failed, rc = 0x%x (%s)", rc,
                cops_swbp_SimLockReturnCodeDesc(rc));
    } else {
        COPS_IDBG_PRINT("Unlock succeeded");
    }
}

/* Print status of a specific SimLock */
static void cops_swbp_PrintSimLockStatus(SIMLock_LockStatus_t *LockStatus_p,
                                         const char *LockTypeName)
{
    COPS_IDBG_PRINT("[%s Lock (%d)]", LockTypeName, LockStatus_p->LockType);
    COPS_IDBG_PRINT("  LockDefinition = %02x", LockStatus_p->LockDefinition);

    COPS_IDBG_PRINT("  LockSettings = %s (%d)",
      LockStatus_p->LockSetting == SIMLOCK_LOCK_SETTING_UNLOCKED ? "Unlocked" :
      LockStatus_p->LockSetting == SIMLOCK_LOCK_SETTING_LOCKED ? "Locked" :
      LockStatus_p->LockSetting == SIMLOCK_LOCK_SETTING_DISABLED ? "Disabled" :
      LockStatus_p->LockSetting == SIMLOCK_LOCK_SETTING_AUTOLOCK_ENABLED ?
              "AutoLock" :
      LockStatus_p->LockSetting == SIMLOCK_LOCK_SETTING_AUTOLOCK_TO_CNL_ENABLED
              ? "AutoLock to CNL" : "?", LockStatus_p->LockSetting);

    COPS_IDBG_PRINT("  AttempsLeft=%d, TimerAttemptsLeft=%d, TimerRunning=%d, "
            "TimeLeftOnRunningTimer=%d",
            LockStatus_p->AttemptsLeft, LockStatus_p->TimerAttemptsLeft,
            LockStatus_p->TimerIsRunning, LockStatus_p->TimeLeftOnRunningTimer);
}

/* Get status of SIMLocks
 * Usage: GetSimLockStatus
 */
static void cops_swbp_GetSimLockStatus(char *cmd, int *arg_index, int args)
{
    SIMLock_ReturnCode_t    rc;
    SIMLock_Status_t        LockStatus;

    rc = Do_SIMLock_GetStatus(&LockStatus);

    if (rc != COPS_RETURN_CODE_OK) {
        COPS_IDBG_PRINT("GetSimLockStatus failed, rc = 0x%x (%s)", rc,
                cops_swbp_SimLockReturnCodeDesc(rc));
    } else {
        COPS_IDBG_PRINT("CardStatus = %s",
          LockStatus.SIM_CardStatus == SIMLOCK_SIM_CARD_STATUS_NOT_CHECKED ?
                  "Not checked" :
          LockStatus.SIM_CardStatus == SIMLOCK_SIM_CARD_STATUS_NOT_APPROVED ?
                          "Not approved" : "Approved");

        cops_swbp_PrintSimLockStatus(&LockStatus.NL_Status,  "Network");
        cops_swbp_PrintSimLockStatus(&LockStatus.NSL_Status, "Network Subset");
        cops_swbp_PrintSimLockStatus(&LockStatus.SPL_Status, "ServiceProvider");
        cops_swbp_PrintSimLockStatus(&LockStatus.CL_Status,  "Corporate");
        cops_swbp_PrintSimLockStatus(&LockStatus.SIML_Status, "SIM Personali.");
        cops_swbp_PrintSimLockStatus(&LockStatus.ESLL_Status, "ESL");

        COPS_IDBG_PRINT("FailedLock = %d", LockStatus.FailedLock);
    }
}

/* Write one parameter in COPS data.
 * Usage: WriteCopsData [ParamId] [Data1...]
 *   E.g. "WriteCopsData 10 0", to set Network Lock to Unlocked
 */
static void cops_swbp_WriteCopsData(char *cmd, int *arg_index, int args)
{
    COPS_BindPropertiesArg_t  BPArg;
    COPS_ReturnCode_t         rc;
    int                       ParamId;
    uint8                     NewCopsData[256];

    /* COPS data structs. Starts with 4 bytes of version data */
    static uint8 SimLockSettingData[] = {
     0x02, 0x00, 0x00, 0x00, 0x10, 0x00, 0x05, 0x00, 0x00 };
    static uint8 SimLockControlData[] = {
     0x02, 0x00, 0x00, 0x00,
     0x20, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x05, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    static uint8 SimLockDefinitionData[] = {
     0x02, 0x00, 0x00, 0x00,
     0x30, 0x00, 0x08, 0x00, 0x7F, 0x00, 0x00, 0x00 };

    memset(&BPArg, 0, sizeof(BPArg));
    BPArg.Merge_COPS_Data = TRUE;

    if (args < 2) {
        COPS_IDBG_PRINT("Args<2");
        goto UsageError;
    }

    /* Parameter Id*/
    ParamId = Str2Hex(cmd + arg_index[0]);

    /* Lock setting */
    if (ParamId >= 0x10 && ParamId <= 0x16) {
        uint8 LockMode = (uint8)atoi(cmd + arg_index[1]);

        SimLockSettingData[4+0] = ParamId;
        SimLockSettingData[4+4] = LockMode;
        BPArg.COPS_DataLength = sizeof(SimLockSettingData);
        BPArg.COPS_Data_p = SimLockSettingData;
    }

    /* Lock control data */
    else if (ParamId >= 0x20 && ParamId <= 0x26) {
        if (args < 3) {
            goto UsageError;
        } else {
            uint8 NbrOfAttempts      = (uint8)atoi(cmd + arg_index[1]);
            uint8 NbrOfTimerAttempts = (uint8)atoi(cmd + arg_index[2]);

            SimLockControlData[4+0] = ParamId;
            SimLockControlData[4+9]  = NbrOfAttempts;
            SimLockControlData[4+10] = NbrOfTimerAttempts;

            BPArg.COPS_DataLength = sizeof(SimLockControlData);
            BPArg.COPS_Data_p = SimLockControlData;
        }
    }

    /* Lock definition */
    else if (ParamId >= 0x30 && ParamId <= 0x36) {
        char *EndPtr = NULL;
        uint32 LockDef = strtoul(cmd + arg_index[1], &EndPtr, HEX_BASE);

        SimLockDefinitionData[4+0] = ParamId;
        memcpy(&SimLockDefinitionData[4+4], &LockDef, 4);

        BPArg.COPS_DataLength = sizeof(SimLockDefinitionData);
        BPArg.COPS_Data_p = SimLockDefinitionData;
    }

    /* Any data */
    else if (ParamId == 0) {
        char *CopsData_p = cmd + arg_index[1];
        char  HexData[2];
        uint32 i;
        uint32 Offset = 0;

        /* Copy version data from SimLockControlData */
        memcpy(NewCopsData, SimLockControlData, 4);
        Offset = 4;

        /* Check indata */
        if (strlen(CopsData_p) % 2 != 0) {
            COPS_IDBG_PRINT("Supplied data is not correct");
            goto UsageError;
        } else if (strlen(CopsData_p)/2 > sizeof(NewCopsData)-4) {
            COPS_IDBG_PRINT("Supplied data is too big");
            goto UsageError;
        }

        /* Convert ASCIIHex to byte data */
        for (i = 0; i < strlen(CopsData_p); i += 2) {
            memcpy(HexData, CopsData_p+i, 2);
            NewCopsData[Offset] = Str2Hex(HexData);
            Offset++;
        }

        BPArg.COPS_DataLength = Offset;
        BPArg.COPS_Data_p = NewCopsData;
    }

    /* Deafult ControlKey for SIM Personalization Lock */
    else if (ParamId == 4) {
        char *CK_p = cmd + arg_index[1];
        size_t CK_length = strlen(CK_p);
        size_t i;

        if (CK_length < COPS_SIMLOCK_SIM_CONTROLKEY_MIN_LENGTH ||
            CK_length > COPS_SIMLOCK_CONTROLKEY_MAX_LENGTH) {
            COPS_IDBG_PRINT("Supplied ControlKey is too big");
            goto UsageError;
        }

        /* Use SimLockControlData struct */
        SimLockControlData[4+0] = ParamId;
        SimLockControlData[6+0] = 4 + CK_length;

        /* Convert ASCIIHex to byte data */
        for (i = 0; i < CK_length; i++) {
            SimLockControlData[8+i] = CK_p[i];
        }

        BPArg.COPS_DataLength = 4 + 4 + CK_length;
        BPArg.COPS_Data_p = SimLockControlData;
    }

    else {
        goto UsageError;
    }

    rc = Do_COPS_BindProperties(&BPArg);

    if (rc != COPS_RETURN_CODE_OK) {
        COPS_IDBG_PRINT("BindProperties failed, rc = 0x%x (%s)", rc,
                cops_swbp_ReturnCodeDesc(rc));
    } else {
        COPS_IDBG_PRINT("COPS Data updated (BindProperties succeeded)");
    }

    return;
UsageError:
  COPS_IDBG_PRINT("Usage: WriteCopsData [ParamId in HexFormat] DATA...");
  COPS_IDBG_PRINT(" ParamId=4 DATA=<DefaultControlKey>");
  COPS_IDBG_PRINT(" ParamId=0x10-0x16 DATA=[LockMode]");
  COPS_IDBG_PRINT(" ParamId=0x20-0x26 DATA=[NbrOfAttempts] [NbrOfTimerAtmpts]");
  COPS_IDBG_PRINT(" ParamId=0x30-0x36 DATA=[LockDefinition in ASCII Hex]");
  COPS_IDBG_PRINT(" ParamId=0 DATA=<Any COPS data in ASCII hex format>");
}

/* Validate and copy control key value from argument */
static boolean cops_swbp_CopyControlKeyArg(
        SIMLock_ControlKey_t *CK_p,
        const char           *Arg_p,
        const size_t          MinLength,
        const size_t          MaxLength)
{
    size_t i;

    if (Arg_p == NULL || CK_p == NULL) {
        COPS_IDBG_PRINT("NULL pointer arguments");
        return FALSE;
    }

    if (strlen(Arg_p) < MinLength || strlen(Arg_p) > MaxLength) {
        COPS_IDBG_PRINT("Control key length is not correct");
        return FALSE;
    }

    for (i = 0; i < strlen(Arg_p); i++) {
        if (Arg_p[i] < '0' || Arg_p[i] > '9') {
            COPS_IDBG_PRINT("Only digits allowed in Control key");
            return FALSE;
        }
    }

    strcpy(CK_p->Value, Arg_p);
    return TRUE;
}

/* Verify all control keys
 * Usage: VerifyControlKeys [ControlKeys...]
 *   ControlKeys: 1=All '11111111', 2: All '2222222222222222',
 *                3:'11111111' '22222222' ... '55555555'
 *     or
 *   ControlKeys: NL_key, NSL_key, SP_key, Co_Key, ESL_key
 */
static void cops_swbp_VerifyControlKeys(char *cmd, int *arg_index, int args)
{
    SIMLock_ControlKeys_t   ControlKeys;
    SIMLock_ReturnCode_t    rc;

    memset(&ControlKeys, 0, sizeof(ControlKeys));

    if (args == 1) {
        int param = atoi(cmd + arg_index[0]);

        if (param == 1) {
            memcpy(&ControlKeys, &Test_CK_1, sizeof(SIMLock_ControlKeys_t));
        } else if (param == 2) {
            memcpy(&ControlKeys, &Test_CK_2, sizeof(SIMLock_ControlKeys_t));
        } else if (param == 3) {
            memcpy(&ControlKeys, &Test_CK_3, sizeof(SIMLock_ControlKeys_t));
        } else {
            goto UsageError;
        }
    } else if (args == 5) {
        if (!cops_swbp_CopyControlKeyArg(&ControlKeys.NL_Key,
                                    cmd + arg_index[0], 8, 16))
            goto UsageError;
        if (!cops_swbp_CopyControlKeyArg(&ControlKeys.NSL_Key,
                                    cmd + arg_index[1], 8, 16))
            goto UsageError;
        if (!cops_swbp_CopyControlKeyArg(&ControlKeys.SPL_key,
                                    cmd + arg_index[2], 8, 16))
            goto UsageError;
        if (!cops_swbp_CopyControlKeyArg(&ControlKeys.CL_key,
                                    cmd + arg_index[3], 8, 16))
            goto UsageError;
        if (!cops_swbp_CopyControlKeyArg(&ControlKeys.FESLL_Key,
                                    cmd + arg_index[4], 8, 16))
            goto UsageError;
    } else {
        goto UsageError;
    }
    PrintHexBuffer("CK", &ControlKeys, sizeof(ControlKeys)); /* TEMP!!! */

    rc = Do_SIMLock_VerifyControlKeys(&ControlKeys);

    if (rc != COPS_RETURN_CODE_OK) {
        COPS_IDBG_PRINT("VerifyControlKeys failed, rc = 0x%x (%s)", rc,
                cops_swbp_SimLockReturnCodeDesc(rc));
    } else {
        COPS_IDBG_PRINT("VerifyControlKeys succeeded");
    }

    return;
UsageError:
  COPS_IDBG_PRINT("Usage: VerifyControlKeys [ControlKeys...]");
  COPS_IDBG_PRINT(" ControlKeys: 1=All '11111111', 2:'2222222222222222', "
          "3:'11111111' '22222222' ... '55555555'");
  COPS_IDBG_PRINT("   or");
  COPS_IDBG_PRINT("   : [NL_key] [NSL_key] [SPL_key] [CL_Key] [ESL_key]");
}

/* Change ControlKey for SIM Lock (Personalization lock)
 * Usage: SIM_ChangeLockKey [Old ControlKey] [New ControlKey]
 */
static void cops_swbp_SIM_ChangeLockKey(char *cmd, int *arg_index, int args)
{
    SIMLock_ControlKey_t    OldCK;
    SIMLock_ControlKey_t    NewCK;
    SIMLock_ReturnCode_t    rc;

    if (args != 2) {
        goto UsageError;
    }

    if (!cops_swbp_CopyControlKeyArg(&OldCK, cmd + arg_index[0], 4, 8))
        goto UsageError;
    if (!cops_swbp_CopyControlKeyArg(&NewCK, cmd + arg_index[1], 4, 8))
        goto UsageError;

    PrintHexBuffer("Old CK", &OldCK, sizeof(OldCK)); /* TEMP */
    PrintHexBuffer("New CK", &NewCK, sizeof(NewCK)); /* !!!! */

    rc = Do_SIMLock_SIM_ChangeLockControlKey(&OldCK, &NewCK);

    if (rc != COPS_RETURN_CODE_OK) {
        COPS_IDBG_PRINT("SIM_ChangeLockKey failed, rc = 0x%x (%s)", rc,
                cops_swbp_SimLockReturnCodeDesc(rc));
    } else {
        COPS_IDBG_PRINT("SIM_ChangeLockKey succeeded");
    }

    return;
UsageError:
    COPS_IDBG_PRINT("Usage: SIM_ChangeLockKey [Old CtrlKey] [New CtrlKey]");

}

/* Verify IMSI (15 digits)
 * Usage: VerifyImsi [IMSI]
 *   If no IMSI specified, use 240-993-00000546-4
 */
static void cops_swbp_VerifyImsi(char *cmd, int *arg_index, int args)
{
    SIMLock_IMSI_t          imsi;
    SIMLock_ReturnCode_t    rc;

    /* Need service handle to use Request_SIM_IMSI_Read() */
    /* So I can't read out current used IMSI here */

    if (args == 0) {
        SIMLock_IMSI_t TestSim = {
                0x08, 0x20, 0x04, 0x99, 0x03, 0x00, 0x00, 0x45, 0x46 };
        memcpy(&imsi, &TestSim, sizeof(TestSim));
    } else if (args == 1) {
        char *p = cmd + arg_index[0];
        if (strlen(p) != 15) {
            goto UsageError;
        }
        imsi.Data[0] = 8; /* Length of IMSI in bytes */
        imsi.Data[1] = ((p[0] - '0')<<4);
        imsi.Data[2] = p[1] - '0' + ((p[2] - '0')<<4);
        imsi.Data[3] = p[3] - '0' + ((p[4] - '0')<<4);
        imsi.Data[4] = p[5] - '0' + ((p[6] - '0')<<4);
        imsi.Data[5] = p[7] - '0' + ((p[8] - '0')<<4);
        imsi.Data[6] = p[9] - '0' + ((p[10] - '0')<<4);
        imsi.Data[7] = p[11] - '0' + ((p[12] - '0')<<4);
        imsi.Data[8] = p[13] - '0' + ((p[14] - '0')<<4);
    } else {
        goto UsageError;
    }

    PrintHexBuffer("imsi", &imsi, sizeof(imsi)); /* TEMP!!! */

    rc = Do_SIMLock_Verify_IMSI(&imsi);

    if (rc != COPS_RETURN_CODE_OK) {
        COPS_IDBG_PRINT("Verify_IMSI failed, rc = 0x%x (%s)", rc,
                cops_swbp_SimLockReturnCodeDesc(rc));
    } else {
        COPS_IDBG_PRINT("Verify_IMSI succeeded");
    }

    return;

UsageError:
  COPS_IDBG_PRINT("Usage: Verify_IMSI [IMSI of 15 digits]");
}

/* Usage: Get product JTAG debug settings.
 *   Returns Modem and APE JTAG settings of the device
 */
static void cops_swbp_GetProdDbgSettings(char *cmd, int *arg_index, int args)
{
    COPS_ReturnCode_t rc;
    uint32            DbgSettings = 0;

    rc = Do_COPS_GetProductDebugSettings(&DbgSettings);
    if (rc != COPS_RETURN_CODE_OK) {
        COPS_IDBG_PRINT("GetProductDebugSettings failed, rc = 0x%x (%s)", rc,
                        cops_swbp_ReturnCodeDesc(rc));
    } else {
        COPS_IDBG_PRINT("JTAG debug Modem = %d, JTAG debug APE = %d "
                       "(enabled = 1, disabled = 0)\n",
                       ((DbgSettings & COPS_DEBUG_FLAG_JTAG_ENABLED_MODEM) > 0),
                       ((DbgSettings & COPS_DEBUG_FLAG_JTAG_ENABLED_APE) > 0));
    }
}

/* OTA_Unlock of Network lock
 * Usage: OTA_Unlock [NS_UnlockKey]
 */
static void cops_swbp_OTA_Unlock(char *cmd, int *arg_index, int args)
{
    COPS_IMEI_t                 imei;
    uint8                       Data[120];
    uint8                       DataLength = sizeof(Data);
    SIMLock_OTA_ReplyMessage_t  ReplyMessage;
    SIMLock_ReturnCode_t        rc;

    if (args != 1) {
        goto UsageError;
    }

    if (COPS_RETURN_CODE_OK != Do_COPS_ReadIMEI(&imei)) {
        COPS_IDBG_PRINT("Can't retrieve IMEI");
        return;
    }

    memset(Data, 0, DataLength);
    memcpy(&Data[40], cmd + arg_index[0], 8);
    memset(&Data[48], 'F', 24); /* 3*8 with all F's (=unused) */
    memcpy(&Data[72], imei.Digits, 15);
    memset(&Data[88], 'F', 32); /* 4*8 with all F's (=unused) */

    COPS_IDBG_HEX("OTA-data", Data, DataLength); /* TEMP!!! */

    rc = Do_SIMLock_OTA_Unlock(Data, DataLength, &ReplyMessage);

    if (rc != COPS_RETURN_CODE_OK) {
        COPS_IDBG_PRINT("OTA_Unlock failed, rc = 0x%x (%s)", rc,
                cops_swbp_SimLockReturnCodeDesc(rc));
    } else {
        COPS_IDBG_PRINT("OTA_Unlock succeeded");
        COPS_IDBG_HEX("OTA reply", &ReplyMessage, sizeof(ReplyMessage));
    }

    return;
UsageError:
  COPS_IDBG_PRINT("Usage: OTA_Unlock [NS_UnlockKey]");
}
