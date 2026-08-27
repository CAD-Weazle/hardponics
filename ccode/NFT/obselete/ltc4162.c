// file    : ltc4162.c
// author  : rb
// purpose : LTC4162 MPPT Solar Multiple Cell Battery Charger
// date    : 221103
// last    : 221117
//

#include "includes.h"

// -- globals
float Vbat = 0.0;  // battery voltage
float Vin  = 0.0;  // input volrage, from solar panel
float Vout = 0.0;  // system output voltage (not used)
float Ibat = 0.0;  // battery current (not used)
float Iin  = 0.0;  // input current, from solar panel
float Iset = 0.0;  // targer charge current
float Tdie = 0.0;  // die temperature

float Iin_target = 0.0;

uint16_t charger_state = 0;
uint16_t charge_status = 0;

// init 
void init_ltc4162 (void)
{
  int res;

  // enable telemetry even when no solar panel present
  res = ltc4162_register_read (LTC4162_CONFIG_BITS_REG);

  res |= LTC4162_CONFIG_BITS_TELEM_FORCE    // enable telemetry
      |  LTC4162_CONFIG_BITS_TELEM_SPEED    // high speed measurements
      |  LTC4162_CONFIG_BITS_MPPT_EN        // enable MPPT
      ;

  ltc4162_register_write (LTC4162_CONFIG_BITS_REG, res);

  // set input current limit (0x15) - (iin_limit_target+1) * 500uV/RSNSI = (iin_limit_target+1) * 50mA
//ltc4162_register_write (LTC4162_IIN_LIMIT_TARGET_REG, 19);         // Iin limit = 1A
  ltc4162_register_write (LTC4162_IIN_LIMIT_TARGET_REG, 39);         // Iin limit = 2A

  // set target charge current (0x1a) - (current_setting+1) * 1mV/RSNSB = (current_setting+1) * 100mA
  ltc4162_register_write (LTC4162_CHARGE_CURRENT_SETTING_REG, 9);    // Icharge = 1A 
//ltc4162_register_write (LTC4162_CHARGE_CURRENT_SETTING_REG, 19);   // Icharge = 2A 

  // set charger config bits (0x29)
  ltc4162_register_write (LTC4162_CHARGER_CONFIG_BITS_REG, 0b000);   // disable JEITA / disable 'C/x' charging
//ltc4162_register_write (LTC4162_CHARGER_CONFIG_BITS_REG, 0b100);   // disable JEITA / enable 'C/x' charging
//ltc4162_register_write (LTC4162_CHARGER_CONFIG_BITS_REG, 0b001);   // enable JEITA  / disable 'C/x' charging

  // dump settings
  ltc4162_dump_registers2 ();
}

// read LTC4162 register 
uint16_t ltc4162_register_read (uint8_t reg)
{
  return (i2c_read_word (reg));
}

// write LTC4162 register 
void ltc4162_register_write (uint8_t reg, uint16_t dat)
{
  i2c_write_word (reg, dat);
}

// update LTC4162 internal status 
void ltc4162_status_update (void)
{
  // get actual U/I/T
  Vbat =  (int16_t)ltc4162_register_read (LTC4162_VBAT_REG) * VBAT_LSB;
  Vin  =  (int16_t)ltc4162_register_read (LTC4162_VIN_REG)  * VIN_LSB;
  Vout =  (int16_t)ltc4162_register_read (LTC4162_VOUT_REG) * VOUT_LSB;
  Ibat =  (int16_t)ltc4162_register_read (LTC4162_IBAT_REG) * IBAT_LSB;
  Iin  =  (int16_t)ltc4162_register_read (LTC4162_IIN_REG)  * IIN_LSB;
  Tdie = ((int16_t)ltc4162_register_read (LTC4162_DIE_TEMP_REG) * TDIE_FACTOR) + TDIE_OFFSET;
  Iset = ((int16_t)ltc4162_register_read (LTC4162_CHARGE_CURRENT_SETTING_REG)+1) * ICHARGE_LSB;
  Iin_target = ((int16_t)ltc4162_register_read (LTC4162_IIN_LIMIT_TARGET_REG)+1) * IINTARGET_LSB;


  // get charger state
  charger_state = ltc4162_register_read (LTC4162_CHARGER_STATE_REG);
  charge_status = ltc4162_register_read (LTC4162_CHARGER_STATUS_REG);
}

// dump LTC4162 internal status 
void ltc4162_status_dump (void)
{
  printf2 ("Vb %2.2f ", Vbat);
  printf2 ("Vi %2.2f ", Vin);
  printf2 ("Vo %2.2f ", Vout);
  printf2 ("Ib %2.2f ", Ibat);
  printf2 ("Ii %2.2f ", Iin);
  printf2 ("Ichg %2.2f ", Iset);
  printf2 ("Tdie %2.2f ", Tdie);
  printf2 ("Iin_target %2.2f ", Iin_target);

  ltc4162_charger_state_dump ();
  ltc4162_charge_status_dump ();

  printf2 ("LIM 0x%04x ",        ltc4162_register_read (LTC4162_LIMIT_ALERTS_REG));
  printf2 ("CSTATE 0x%04x ",     ltc4162_register_read (LTC4162_CHARGE_STATE_ALERTS_REG));
  printf2 ("CSTATUS 0x%04x ",    ltc4162_register_read (LTC4162_CHARGE_STATUS_ALERTS_REG));
  printf2 ("SYSSTATUS 0x%04x ",  ltc4162_register_read (LTC4162_SYSTEM_STATUS_REG));  // 0x39
  printf2 ("CONFIGBITS 0x%04x ", ltc4162_register_read (LTC4162_CONFIG_BITS_REG));    // 0x14

  printf2 ("\n");
}

// dump LTC4162 charger state
void ltc4162_charger_state_dump (void)
{
  switch (charger_state)
  {
    case 4096: // 0x1000
      printf2 ("bat_detect_failed_fault ");
      break;
    case 2048: // 0x0800
      printf2 ("battery_detection ");
      break;
    case  256: // 0x0100
      printf2 ("charger_suspended ");
      break;
    case  128: // 0x0080
      printf2 ("precharge ");
      break;
    case   64: // 0x0040
      printf2 ("cc_cv_charge ");
      break;
    case   32: // 0x0020
      printf2 ("ntc_pause ");
      break;
    case   16: // 0x0010
      printf2 ("timer_term ");
      break;
    case    8: // 0x0008
      printf2 ("c_over_x_term ");
      break;
    case    4: // 0x0004
      printf2 ("max_charge_time_fault ");
      break;
    case    2: // 0x0002
      printf2 ("bat_missing_fault ");
      break;
    case    1: // 0x0001
      printf2 ("bat_short_fault ");
      break;
    default:
      break;
  }
}

// dump LTC4162 internal status 
void ltc4162_charge_status_dump (void)
{

  switch (charge_status)
  {
    case 32:  // 0x00020
      printf2 ("ilim_reg_active ");
      break;
    case 16:  // 0x0010
      printf2 ("thermal_reg_active ");
      break;
    case  8:  // 0x0008
      printf2 ("vin_uvcl_active ");
      break;
    case  4:  // 0x0004
      printf2 ("iin_limit_active ");
      break;
    case  2:  // 0x0002
      printf2 ("constant_current ");
      break;
    case  1:  // 0x0001
      printf2 ("constant_voltage ");
      break;
    case  0:  // 0x0000
      printf2 ("charger_off ");
      break;
    default:
      break;
  }
}

// dump LTC4162 registers <> todo: add missing of register
void ltc4162_dump_registers (void)
{
  printf2 ("LTC4162_CONFIG_BITS_REG   : 0x%04x\n", ltc4162_register_read (LTC4162_CONFIG_BITS_REG));
  printf2 ("LTC4162_CHARGER_STATE_REG : 0x%04x\n", ltc4162_register_read (LTC4162_CHARGER_STATE_REG));
  printf2 ("LTC4162_CHARGER_STATUS_REG: 0x%04x\n", ltc4162_register_read (LTC4162_CHARGER_STATUS_REG));
  printf2 ("LTC4162_THERMAL_START_REG : %d\n",     ltc4162_register_read (LTC4162_THERMAL_START_REG));
  printf2 ("LTC4162_VBAT_REG          : %d\n",     ltc4162_register_read (LTC4162_VBAT_REG));
  printf2 ("LTC4162_VIN_REG           : %d\n",     ltc4162_register_read (LTC4162_VIN_REG));
  printf2 ("LTC4162_VOUT_REG          : %d\n",     ltc4162_register_read (LTC4162_VOUT_REG));
  printf2 ("LTC4162_IBAT_REG          : %d\n",     (int16_t)ltc4162_register_read (LTC4162_IBAT_REG));
  printf2 ("LTC4162_IIN_REG           : %d\n",     (int16_t)ltc4162_register_read (LTC4162_IIN_REG));
  printf2 ("LTC4162_DIE_TEMP_REG      : %d\n",     ltc4162_register_read (LTC4162_DIE_TEMP_REG));
  printf2 ("LTC4162_CHEM_CELLS_REG    : 0x%08x\n", ltc4162_register_read (LTC4162_CHEM_CELLS_REG));
}

void ltc4162_dump_registers2 (void)
{
  int res;

  printf2 ("#-- settings -------------------------------------\n");

  // get charger config bits 
  res = ltc4162_register_read (LTC4162_CHARGER_CONFIG_BITS_REG);
  printf2 ("#CHARGER CONFIG BITS REG    : 0x%04x\n", res);

  res = ltc4162_register_read (LTC4162_IIN_LIMIT_TARGET_REG);
  printf2 ("#IIN_LIMIT                  : %2.2f [A]\n", (res+1) * 0.050);  // 500uV/RSNSI = 500uV*100 = 50mV

  // input voltage limit
  res = ltc4162_register_read (LTC4162_INPUT_UNDERVOLTAGE_REG);
  printf2 ("#VIN_UNDER_LIMIT            : %2.2f [V]\n", (res+1) * 0.140625);

  // test ship mode off
  res = ltc4162_register_read (LTC4162_ARM_SHIP_MODE);
  printf2 ("#SHIP_MODE                  : %d\n", res);

  // get target charge current
  res = ltc4162_register_read (LTC4162_CHARGE_CURRENT_SETTING_REG);
  printf2 ("#CHARGE_CURRENT             : %2.2f [A]\n", (res+1) * 0.100);  // 1mV/RSNSB = 1mV*100 = 100mV

  // check final charge voltage (should be 3x4V2 = 12V6)
  res = ltc4162_register_read (LTC4162_VCHARGE_SETTING_REG);
  printf2 ("#V_FINAL                    : %2.2f [V]\n", CELL_COUNT * ((res * 0.0125) + 3.8125));

  // check C/x threshold 
  res = ltc4162_register_read (LTC4162_Cx_THRESHOLD_REG);
  printf2 ("#C/x threshold              : %d\n", res);

  // check #cells/chemistry
  res = ltc4162_register_read (LTC4162_CHEM_CELLS_REG);
  printf2 ("#CHEM_CELLS                 : 0x%04x\n", res);  // 0x31c3 -> b[3:0]: 3 cells, b[11:8]: 1 (type 'L42')

  // check NTC voltage
  res = ltc4162_register_read (LTC4162_THERMISTOR_VOLTAGE_REG);
  if (res) // needed for i2c error <> WTF????????
    printf2 ("#NTC                        : %d [R]\n", (10000 * res) / (21829 - res)); 

  // check JEITA region
  res = ltc4162_register_read (LTC4162_JEITA_REGION_REG);
  printf2 ("#JEITA region               : %d\n", res); 

  // check I charge DAC
  res = ltc4162_register_read (LTC4162_ICHARGE_DAC_REG);
  printf2 ("#I charge DAC               : %2.2f [A]\n", (res+1) * 0.1); 

  // check V charge DAC
  res = ltc4162_register_read (LTC4162_VCHARGE_DAC_REG);
  printf2 ("#V charge DAC               : %2.2f [V]\n", CELL_COUNT * ((res*0.0125) + 3.8125)); 

  // check I input limit DAC
  res = ltc4162_register_read (LTC4162_IIN_LIMIT_DAC_REG);
  printf2 ("#I input limit DAC          : %2.2f [A]\n", (res+1) * 0.050); 

  // check limit alert enable bits
  res = ltc4162_register_read (LTC4162_EN_LIMIT_ALERTS_REG);
  printf2 ("#EN_LIMIT_ALERTS_REG        : 0x%04x\n", res); 

  // check charger state notification enable bits
  res = ltc4162_register_read (LTC4162_EN_CHARGER_STATE_ALERTS_REG);
  printf2 ("#EN_CHARGER_STATE_ALERTS_REG: 0x%04x\n", res); 

  // check charge status notification enable bits
  res = ltc4162_register_read (LTC4162_EN_CHARGE_STATUS_ALERTS_REG);
  printf2 ("#EN_CHARGE_STATUS_ALERTS_REG: 0x%04x\n", res); 

  printf2 ("#-------------------------------------------------\n");
}

