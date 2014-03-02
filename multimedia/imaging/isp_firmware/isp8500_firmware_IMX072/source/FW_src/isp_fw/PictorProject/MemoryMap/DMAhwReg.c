#define BaseAddress 0xF0002400

#include "DMAhwReg.h"

volatile DMA_IP_ts *p_DMA_IP = (DMA_IP_ts *)(BaseAddress + 0x000);    //DMA

