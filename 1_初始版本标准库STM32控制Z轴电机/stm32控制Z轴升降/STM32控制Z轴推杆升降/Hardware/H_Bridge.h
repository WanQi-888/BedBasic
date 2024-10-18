#ifndef __H_BRIDGE_H_
#define __H_BRIDGE_H_

void H_Bridge_Init(void);
void H_BridgeEnable(void);
void H_BridgeDisable(void);
void H_Bridge_Set_PWM1(uint16_t compare);
void H_Bridge_Set_PWM2(uint16_t compare);

#endif
