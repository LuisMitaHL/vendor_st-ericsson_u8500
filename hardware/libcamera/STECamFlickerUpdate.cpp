/*
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
*
* Code adapted for usage of OMX components. All functions
* using OMX functionality are under copyright of ST-Ericsson
*
*  This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*/

#define DBGT_LAYER 1
#define DBGT_PREFIX "Flickr"

//Internal includes
#define CAM_LOG_TAG "STECamera"
#include "STECamTrace.h"
#include "STECamUtils.h"
#include "STECamDynSetting.h"

//Multimedia includes
#include <OMX_Symbian_IVCommonExt_Ste.h>

namespace android
{

struct CountryMap {
    uint32_t mCountryCode; /**< Country code */
    OMX_SYMBIAN_FLICKERREMOVALTYPE mType; /**< Flicker type */
};

static const struct CountryMap CountryNumeric[] = {
        { 202, OMX_SYMBIAN_FlickerRemoval50 }, // Greece
        { 204, OMX_SYMBIAN_FlickerRemoval50 }, // Netherlands
        { 206, OMX_SYMBIAN_FlickerRemoval50 }, // Belgium
        { 208, OMX_SYMBIAN_FlickerRemoval50 }, // France
        { 212, OMX_SYMBIAN_FlickerRemoval50 }, // Monaco
        { 213, OMX_SYMBIAN_FlickerRemoval50 }, // Andorra
        { 214, OMX_SYMBIAN_FlickerRemoval50 }, // Spain
        { 216, OMX_SYMBIAN_FlickerRemoval50 }, // Hungary
        { 219, OMX_SYMBIAN_FlickerRemoval50 }, // Croatia
        { 220, OMX_SYMBIAN_FlickerRemoval50 }, // Serbia
        { 222, OMX_SYMBIAN_FlickerRemoval50 }, // Italy
        { 226, OMX_SYMBIAN_FlickerRemoval50 }, // Romania  OMX_SYMBIAN_FlickerRemoval50
        { 228, OMX_SYMBIAN_FlickerRemoval50 }, // Switzerland
        { 230, OMX_SYMBIAN_FlickerRemoval50 }, // Czech Republic
        { 231, OMX_SYMBIAN_FlickerRemoval50 }, // Slovakia
        { 232, OMX_SYMBIAN_FlickerRemoval50 }, // Austria
        { 234, OMX_SYMBIAN_FlickerRemoval50 }, // United Kingdom
        { 235, OMX_SYMBIAN_FlickerRemoval50 }, // United Kingdom
        { 238, OMX_SYMBIAN_FlickerRemoval50 }, // Denmark
        { 240, OMX_SYMBIAN_FlickerRemoval50 }, // Sweden
        { 242, OMX_SYMBIAN_FlickerRemoval50 }, // Norway
        { 244, OMX_SYMBIAN_FlickerRemoval50 }, // Finland
        { 246, OMX_SYMBIAN_FlickerRemoval50 }, // Lithuania
        { 247, OMX_SYMBIAN_FlickerRemoval50 }, // Latvia
        { 248, OMX_SYMBIAN_FlickerRemoval50 }, // Estonia
        { 250, OMX_SYMBIAN_FlickerRemoval50 }, // Russian Federation
        { 255, OMX_SYMBIAN_FlickerRemoval50 }, // Ukraine
        { 257, OMX_SYMBIAN_FlickerRemoval50 }, // Belarus
        { 259, OMX_SYMBIAN_FlickerRemoval50 }, // Moldova
        { 260, OMX_SYMBIAN_FlickerRemoval50 }, // Poland
        { 262, OMX_SYMBIAN_FlickerRemoval50 }, // Germany
        { 266, OMX_SYMBIAN_FlickerRemoval50 }, // Gibraltar
        { 268, OMX_SYMBIAN_FlickerRemoval50 }, // Portugal
        { 270, OMX_SYMBIAN_FlickerRemoval50 }, // Luxembourg
        { 272, OMX_SYMBIAN_FlickerRemoval50 }, // Ireland
        { 274, OMX_SYMBIAN_FlickerRemoval50 }, // Iceland
        { 276, OMX_SYMBIAN_FlickerRemoval50 }, // Albania
        { 278, OMX_SYMBIAN_FlickerRemoval50 }, // Malta
        { 280, OMX_SYMBIAN_FlickerRemoval50 }, // Cyprus
        { 282, OMX_SYMBIAN_FlickerRemoval50 }, // Georgia
        { 283, OMX_SYMBIAN_FlickerRemoval50 }, // Armenia
        { 284, OMX_SYMBIAN_FlickerRemoval50 }, // Bulgaria
        { 286, OMX_SYMBIAN_FlickerRemoval50 }, // Turkey
        { 288, OMX_SYMBIAN_FlickerRemoval50 }, // Faroe Islands
        { 290, OMX_SYMBIAN_FlickerRemoval50 }, // Greenland
        { 293, OMX_SYMBIAN_FlickerRemoval50 }, // Slovenia
        { 294, OMX_SYMBIAN_FlickerRemoval50 }, // Macedonia
        { 295, OMX_SYMBIAN_FlickerRemoval50 }, // Liechtenstein
        { 297, OMX_SYMBIAN_FlickerRemoval50 }, // Montenegro
        { 302, OMX_SYMBIAN_FlickerRemoval60 }, // Canada
        { 310, OMX_SYMBIAN_FlickerRemoval60 }, // United States of America
        { 311, OMX_SYMBIAN_FlickerRemoval60 }, // United States of America
        { 312, OMX_SYMBIAN_FlickerRemoval60 }, // United States of America
        { 313, OMX_SYMBIAN_FlickerRemoval60 }, // United States of America
        { 314, OMX_SYMBIAN_FlickerRemoval60 }, // United States of America
        { 315, OMX_SYMBIAN_FlickerRemoval60 }, // United States of America
        { 316, OMX_SYMBIAN_FlickerRemoval60 }, // United States of America
        { 330, OMX_SYMBIAN_FlickerRemoval60 }, // Puerto Rico
        { 334, OMX_SYMBIAN_FlickerRemoval60 }, // Mexico
        { 338, OMX_SYMBIAN_FlickerRemoval50 }, // Jamaica
        { 340, OMX_SYMBIAN_FlickerRemoval50 }, // Martinique
        { 342, OMX_SYMBIAN_FlickerRemoval50 }, // Barbados
        { 346, OMX_SYMBIAN_FlickerRemoval60 }, // Cayman Islands
        { 350, OMX_SYMBIAN_FlickerRemoval60 }, // Bermuda
        { 352, OMX_SYMBIAN_FlickerRemoval50 }, // Grenada
        { 354, OMX_SYMBIAN_FlickerRemoval60 }, // Montserrat
        { 362, OMX_SYMBIAN_FlickerRemoval50 }, // Netherlands Antilles
        { 363, OMX_SYMBIAN_FlickerRemoval60 }, // Aruba
        { 364, OMX_SYMBIAN_FlickerRemoval60 }, // Bahamas
        { 365, OMX_SYMBIAN_FlickerRemoval60 }, // Anguilla
        { 366, OMX_SYMBIAN_FlickerRemoval50 }, // Dominica
        { 368, OMX_SYMBIAN_FlickerRemoval60 }, // Cuba
        { 370, OMX_SYMBIAN_FlickerRemoval60 }, // Dominican Republic
        { 372, OMX_SYMBIAN_FlickerRemoval60 }, // Haiti
        { 401, OMX_SYMBIAN_FlickerRemoval50 }, // Kazakhstan
        { 402, OMX_SYMBIAN_FlickerRemoval50 }, // Bhutan
        { 404, OMX_SYMBIAN_FlickerRemoval50 }, // India
        { 405, OMX_SYMBIAN_FlickerRemoval50 }, // India
        { 410, OMX_SYMBIAN_FlickerRemoval50 }, // Pakistan
        { 413, OMX_SYMBIAN_FlickerRemoval50 }, // Sri Lanka
        { 414, OMX_SYMBIAN_FlickerRemoval50 }, // Myanmar
        { 415, OMX_SYMBIAN_FlickerRemoval50 }, // Lebanon
        { 416, OMX_SYMBIAN_FlickerRemoval50 }, // Jordan
        { 417, OMX_SYMBIAN_FlickerRemoval50 }, // Syria
        { 418, OMX_SYMBIAN_FlickerRemoval50 }, // Iraq
        { 419, OMX_SYMBIAN_FlickerRemoval50 }, // Kuwait
        { 420, OMX_SYMBIAN_FlickerRemoval60 }, // Saudi Arabia
        { 421, OMX_SYMBIAN_FlickerRemoval50 }, // Yemen
        { 422, OMX_SYMBIAN_FlickerRemoval50 }, // Oman
        { 424, OMX_SYMBIAN_FlickerRemoval50 }, // United Arab Emirates
        { 425, OMX_SYMBIAN_FlickerRemoval50 }, // Israel
        { 426, OMX_SYMBIAN_FlickerRemoval50 }, // Bahrain
        { 427, OMX_SYMBIAN_FlickerRemoval50 }, // Qatar
        { 428, OMX_SYMBIAN_FlickerRemoval50 }, // Mongolia
        { 429, OMX_SYMBIAN_FlickerRemoval50 }, // Nepal
        { 430, OMX_SYMBIAN_FlickerRemoval50 }, // United Arab Emirates
        { 431, OMX_SYMBIAN_FlickerRemoval50 }, // United Arab Emirates
        { 432, OMX_SYMBIAN_FlickerRemoval50 }, // Iran
        { 434, OMX_SYMBIAN_FlickerRemoval50 }, // Uzbekistan
        { 436, OMX_SYMBIAN_FlickerRemoval50 }, // Tajikistan
        { 437, OMX_SYMBIAN_FlickerRemoval50 }, // Kyrgyz Rep
        { 438, OMX_SYMBIAN_FlickerRemoval50 }, // Turkmenistan
        { 440, OMX_SYMBIAN_FlickerRemoval60 }, // Japan
        { 441, OMX_SYMBIAN_FlickerRemoval60 }, // Japan
        { 452, OMX_SYMBIAN_FlickerRemoval50 }, // Vietnam
        { 454, OMX_SYMBIAN_FlickerRemoval50 }, // Hong Kong
        { 455, OMX_SYMBIAN_FlickerRemoval50 }, // Macao
        { 456, OMX_SYMBIAN_FlickerRemoval50 }, // Cambodia
        { 457, OMX_SYMBIAN_FlickerRemoval50 }, // Laos
        { 460, OMX_SYMBIAN_FlickerRemoval50 }, // China
        { 466, OMX_SYMBIAN_FlickerRemoval60 }, // Taiwan
        { 470, OMX_SYMBIAN_FlickerRemoval50 }, // Bangladesh
        { 472, OMX_SYMBIAN_FlickerRemoval50 }, // Maldives
        { 502, OMX_SYMBIAN_FlickerRemoval50 }, // Malaysia
        { 505, OMX_SYMBIAN_FlickerRemoval50 }, // Australia
        { 510, OMX_SYMBIAN_FlickerRemoval50 }, // Indonesia
        { 514, OMX_SYMBIAN_FlickerRemoval50 }, // East Timor
        { 515, OMX_SYMBIAN_FlickerRemoval60 }, // Philippines
        { 520, OMX_SYMBIAN_FlickerRemoval50 }, // Thailand
        { 525, OMX_SYMBIAN_FlickerRemoval50 }, // Singapore
        { 530, OMX_SYMBIAN_FlickerRemoval50 }, // New Zealand
        { 535, OMX_SYMBIAN_FlickerRemoval60 }, // Guam
        { 536, OMX_SYMBIAN_FlickerRemoval50 }, // Nauru
        { 537, OMX_SYMBIAN_FlickerRemoval50 }, // Papua New Guinea
        { 539, OMX_SYMBIAN_FlickerRemoval50 }, // Tonga
        { 541, OMX_SYMBIAN_FlickerRemoval50 }, // Vanuatu
        { 542, OMX_SYMBIAN_FlickerRemoval50 }, // Fiji
        { 544, OMX_SYMBIAN_FlickerRemoval60 }, // American Samoa
        { 545, OMX_SYMBIAN_FlickerRemoval50 }, // Kiribati
        { 546, OMX_SYMBIAN_FlickerRemoval50 }, // New Caledonia
        { 548, OMX_SYMBIAN_FlickerRemoval50 }, // Cook Islands
        { 602, OMX_SYMBIAN_FlickerRemoval50 }, // Egypt
        { 603, OMX_SYMBIAN_FlickerRemoval50 }, // Algeria
        { 604, OMX_SYMBIAN_FlickerRemoval50 }, // Morocco
        { 605, OMX_SYMBIAN_FlickerRemoval50 }, // Tunisia
        { 606, OMX_SYMBIAN_FlickerRemoval50 }, // Libya
        { 607, OMX_SYMBIAN_FlickerRemoval50 }, // Gambia
        { 608, OMX_SYMBIAN_FlickerRemoval50 }, // Senegal
        { 609, OMX_SYMBIAN_FlickerRemoval50 }, // Mauritania
        { 610, OMX_SYMBIAN_FlickerRemoval50 }, // Mali
        { 611, OMX_SYMBIAN_FlickerRemoval50 }, // Guinea
        { 613, OMX_SYMBIAN_FlickerRemoval50 }, // Burkina Faso
        { 614, OMX_SYMBIAN_FlickerRemoval50 }, // Niger
        { 616, OMX_SYMBIAN_FlickerRemoval50 }, // Benin
        { 617, OMX_SYMBIAN_FlickerRemoval50 }, // Mauritius
        { 618, OMX_SYMBIAN_FlickerRemoval50 }, // Liberia
        { 619, OMX_SYMBIAN_FlickerRemoval50 }, // Sierra Leone
        { 620, OMX_SYMBIAN_FlickerRemoval50 }, // Ghana
        { 621, OMX_SYMBIAN_FlickerRemoval50 }, // Nigeria
        { 622, OMX_SYMBIAN_FlickerRemoval50 }, // Chad
        { 623, OMX_SYMBIAN_FlickerRemoval50 }, // Central African Republic
        { 624, OMX_SYMBIAN_FlickerRemoval50 }, // Cameroon
        { 625, OMX_SYMBIAN_FlickerRemoval50 }, // Cape Verde
        { 627, OMX_SYMBIAN_FlickerRemoval50 }, // Equatorial Guinea
        { 631, OMX_SYMBIAN_FlickerRemoval50 }, // Angola
        { 633, OMX_SYMBIAN_FlickerRemoval50 }, // Seychelles
        { 634, OMX_SYMBIAN_FlickerRemoval50 }, // Sudan
        { 636, OMX_SYMBIAN_FlickerRemoval50 }, // Ethiopia
        { 637, OMX_SYMBIAN_FlickerRemoval50 }, // Somalia
        { 638, OMX_SYMBIAN_FlickerRemoval50 }, // Djibouti
        { 639, OMX_SYMBIAN_FlickerRemoval50 }, // Kenya
        { 640, OMX_SYMBIAN_FlickerRemoval50 }, // Tanzania
        { 641, OMX_SYMBIAN_FlickerRemoval50 }, // Uganda
        { 642, OMX_SYMBIAN_FlickerRemoval50 }, // Burundi
        { 643, OMX_SYMBIAN_FlickerRemoval50 }, // Mozambique
        { 645, OMX_SYMBIAN_FlickerRemoval50 }, // Zambia
        { 646, OMX_SYMBIAN_FlickerRemoval50 }, // Madagascar
        { 647, OMX_SYMBIAN_FlickerRemoval50 }, // France
        { 648, OMX_SYMBIAN_FlickerRemoval50 }, // Zimbabwe
        { 649, OMX_SYMBIAN_FlickerRemoval50 }, // Namibia
        { 650, OMX_SYMBIAN_FlickerRemoval50 }, // Malawi
        { 651, OMX_SYMBIAN_FlickerRemoval50 }, // Lesotho
        { 652, OMX_SYMBIAN_FlickerRemoval50 }, // Botswana
        { 653, OMX_SYMBIAN_FlickerRemoval50 }, // Swaziland
        { 654, OMX_SYMBIAN_FlickerRemoval50 }, // Comoros
        { 655, OMX_SYMBIAN_FlickerRemoval50 }, // South Africa
        { 657, OMX_SYMBIAN_FlickerRemoval50 }, // Eritrea
        { 702, OMX_SYMBIAN_FlickerRemoval60 }, // Belize
        { 704, OMX_SYMBIAN_FlickerRemoval60 }, // Guatemala
        { 706, OMX_SYMBIAN_FlickerRemoval60 }, // El Salvador
        { 708, OMX_SYMBIAN_FlickerRemoval60 }, // Honduras
        { 710, OMX_SYMBIAN_FlickerRemoval60 }, // Nicaragua
        { 712, OMX_SYMBIAN_FlickerRemoval60 }, // Costa Rica
        { 714, OMX_SYMBIAN_FlickerRemoval60 }, // Panama
        { 722, OMX_SYMBIAN_FlickerRemoval50 }, // Argentina
        { 724, OMX_SYMBIAN_FlickerRemoval60 }, // Brazil
        { 730, OMX_SYMBIAN_FlickerRemoval50 }, // Chile
        { 732, OMX_SYMBIAN_FlickerRemoval60 }, // Colombia
        { 734, OMX_SYMBIAN_FlickerRemoval60 }, // Venezuela
        { 736, OMX_SYMBIAN_FlickerRemoval50 }, // Bolivia
        { 738, OMX_SYMBIAN_FlickerRemoval60 }, // Guyana
        { 740, OMX_SYMBIAN_FlickerRemoval60 }, // Ecuador
        { 742, OMX_SYMBIAN_FlickerRemoval50 }, // French Guiana
        { 744, OMX_SYMBIAN_FlickerRemoval50 }, // Paraguay
        { 746, OMX_SYMBIAN_FlickerRemoval60 }, // Suriname
        { 748, OMX_SYMBIAN_FlickerRemoval50 }, // Uruguay
        { 750, OMX_SYMBIAN_FlickerRemoval50 }, // Falkland Islands
    };

/*  Default value */
static const OMX_SYMBIAN_FLICKERREMOVALTYPE kDefaultFlickerType = OMX_SYMBIAN_FlickerRemoval60;

OMX_SYMBIAN_FLICKERREMOVALTYPE cameraGetLocation()
{
    DBGT_PROLOG("");

    /* Mobile Network Code (MNC) is used in combination with a Mobile Country Code (MCC)
       to uniquely identify a mobile phone operator/carrier.
       gsm.operator.numeric contains MNC + MCC.
       MCC is the first-three digit of the gsm.operator.numeric which is followed
       by MNC which can be a 2/3digit number.
       MCC is used for determing the flicker value */
    char operatorCode[7];
    uint32_t countryCode;

    DynSetting::get(DynSetting::ECountryCode, operatorCode);
    DBGT_PTRACE("Operator code =%s",operatorCode);

    if(NULL != operatorCode)
    {
        DBGT_PTRACE("gsm.operator.numeric is not set so setting the default value - %d", kDefaultFlickerType);
        DBGT_EPILOG("");
        return kDefaultFlickerType;
    }

    operatorCode[3] = 0;
    countryCode = atoi(operatorCode);

    DBGT_PTRACE("Country code =%d", countryCode);

    int left = 0;
    int right = ARRAYCOUNT(CountryNumeric) - 1;

    while (left <= right) {

        int index = (left + right) >> 1;

        if (CountryNumeric[index].mCountryCode == countryCode) {
            DBGT_EPILOG("");
            return CountryNumeric[index].mType;
        }
        else if (CountryNumeric[index].mCountryCode > countryCode)
            right = index - 1;
        else
            left = index + 1;
    }

    DBGT_PTRACE("Country code not mapped for flicker value so setting the default value - %d", kDefaultFlickerType);
    DBGT_EPILOG("");
    return kDefaultFlickerType;
}

}; // namespace android
