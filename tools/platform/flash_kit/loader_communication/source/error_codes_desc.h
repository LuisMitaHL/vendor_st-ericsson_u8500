/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2012
 * License terms: 3-clause BSD license
 ******************************************************************************/

typedef struct {
    std::string short_desc;
    std::string long_desc;
} desc;

void GetLoaderErrorDescription_Call(uint32 ErrorNr, uint8 *ShortDescription, uint8 *LongDescription, uint32 ShorDescrBufLen, uint32 LongDescrBufLen);
