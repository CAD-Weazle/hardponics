// file    : ltc4162.h
// author  : rb
// purpose : header file ltc4162.c
// date    : 221103
// last    : 221114

#ifndef __LTC4162_H__
#define __LTC4162_H__

// -- defines
#define LTC4162_RADR                   0b11010001  // LTC4162 read address
#define LTC4162_WADR                   0b11010000  // LTC4162 write address

#define LTC4162_EN_LIMIT_ALERTS_REG          0x0d  // enable limit monitoring and alert notification via SMBALERT
#define LTC4162_EN_CHARGER_STATE_ALERTS_REG  0x0e  // enable charger state notification via SMBALERT
#define LTC4162_EN_CHARGE_STATUS_ALERTS_REG  0x0f  // enable charge status notification via SMBALERT
#define LTC4162_THERMAL_START_REG            0x10  // <> should be 17897 on reading

#define LTC4162_CONFIG_BITS_REG              0x14  // system configuration settings
 #define LTC4162_CONFIG_BITS_SUSPEND      (1 << 5)  // - suspend charging (caution!)
 #define LTC4162_CONFIG_BITS_RUN_BSR      (1 << 4)  // - calculate series resistance battery
 #define LTC4162_CONFIG_BITS_TELEM_SPEED  (1 << 3)  // - 0: low speed / 1: high speed
 #define LTC4162_CONFIG_BITS_TELEM_FORCE  (1 << 2)  // - telemetry on also with battery power only <> test this
 #define LTC4162_CONFIG_BITS_MPPT_EN      (1 << 1)  // - enable MPPT

#define LTC4162_IIN_LIMIT_TARGET_REG         0x15  // max. input current limit
#define LTC4162_INPUT_UNDERVOLTAGE_REG       0x16  // min. input voltage limit
#define LTC4162_ARM_SHIP_MODE                0x19  // shipmode: 21325 (0x534d)
#define LTC4162_CHARGE_CURRENT_SETTING_REG   0x1a  // target charge current regulation
#define LTC4162_VCHARGE_SETTING_REG          0x1b  // final charge voltage regulation level
#define LTC4162_Cx_THRESHOLD_REG             0x1c  // for C/x detection and termination.
#define LTC4162_CHARGER_CONFIG_BITS_REG      0x29  // battery charger configuration settings
#define LTC4162_CHARGER_STATE_REG            0x34  // charger state
#define LTC4162_CHARGER_STATUS_REG           0x35  // charger status
#define LTC4162_LIMIT_ALERTS_REG             0x36  // 
#define LTC4162_CHARGE_STATE_ALERTS_REG      0x37  // 
#define LTC4162_CHARGE_STATUS_ALERTS_REG     0x38  // 
#define LTC4162_SYSTEM_STATUS_REG            0x39  // actual system status bits
#define LTC4162_VBAT_REG                     0x3a  // battery voltage
#define LTC4162_VIN_REG                      0x3b  // input voltage
#define LTC4162_VOUT_REG                     0x3c  // output voltage
#define LTC4162_IBAT_REG                     0x3d  //
#define LTC4162_IIN_REG                      0x3e  //
#define LTC4162_DIE_TEMP_REG                 0x3f  //
#define LTC4162_THERMISTOR_VOLTAGE_REG       0x40  // NTC pin ADC data (open = 21684)
#define LTC4162_JEITA_REGION_REG             0x42  // JEITA battery temperature region
#define LTC4162_CHEM_CELLS_REG               0x43  // battery chemistry: [11:8] / cell count: [3:0]
#define LTC4162_ICHARGE_DAC_REG              0x44  // actual charge current
#define LTC4162_VCHARGE_DAC_REG              0x45  // actual charge voltage
#define LTC4162_IIN_LIMIT_DAC_REG            0x46  // actual input current limit

#define CELL_COUNT                            (3)  // number of battery cells in series
#define uV                             (0.000001)  // 1e-6
#define mV                                (0.001)  // 1e-3
#define RSNSI                              (0.01)  // input current sense resistor [R]
#define RSNSB                              (0.01)  // charge current sense resistor [R]

#define ICHARGE_LSB                ((1*mV)/RSNSB)  // 1 LSB charge current setting

#define VBAT_LSB            (192.4*uV*CELL_COUNT)  // 1 LSB battery voltage
#define VIN_LSB                        (1.649*mV)  // 1 LSB input voltage
#define VOUT_LSB                       (1.653*mV)  // 1 LSB output voltage
#define IBAT_LSB          ((1000*1.466*uV)/RSNSB)  // 1 LSB battery current measurement [mA]
#define IIN_LSB           ((1000*1.466*uV)/RSNSI)  // 1 LSB input current measurement [mA]
#define IINTARGET_LSB            ((500*uV)/RSNSI)  // 1 LSB input current measurement [mA]

#define TDIE_FACTOR                      (0.0215)  // temperature facture
#define TDIE_OFFSET                      (-264.4)  // temperature offset


// -- prototypes
void init_ltc4162 (void);

uint16_t ltc4162_register_read  (uint8_t reg);
void     ltc4162_register_write (uint8_t reg, uint16_t dat);

void ltc4162_status_update (void);
void ltc4162_status_dump   (void);
void ltc4162_charger_state_dump (void);

void ltc4162_dump_registers (void);
void ltc4162_dump_registers2 (void);
void ltc4162_charge_status_dump (void);

#endif


