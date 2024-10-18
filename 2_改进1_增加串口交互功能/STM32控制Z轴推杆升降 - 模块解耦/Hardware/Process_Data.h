#ifndef __PROCESS_DATA_H_
#define __PROCESS_DATA_H_

// MOVEÃüÁîµÄASCIIÂë
static uint8_t moveCode[5] = {0x4D, 0x4F, 0x56, 0x45, 0x20};
static uint8_t scanCode[5] = {0x53, 0x43, 0x41, 0x4E, 0x20};

uint8_t checkArrayForMove(const uint8_t *array1);
uint8_t checkArrayForScan(const uint8_t *array);
uint8_t ConvertCommandToNumber(const uint8_t *array, uint32_t arrayLength, uint32_t *Processed_Data);

#endif
