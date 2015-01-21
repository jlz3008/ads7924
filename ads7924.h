/*
 Copyright © 2014 José Luis Zabalza  License LGPLv3+: GNU
 LGPL version 3 or later <http://www.gnu.org/copyleft/lgpl.html>.
 This is free software: you are free to change and redistribute it.
 There is NO WARRANTY, to the extent permitted by law.
*/
#ifndef ADS7924_H
#define ADS7924_H

#include <Wire.h>
#include <inttypes.h>


class ADCI2C_ADS7924
{

public:

/* ADS7924 modes */

    typedef enum { //  datasheet Page 21 Register
        idle=0x00,              // 000000 = Idle mode (default)
        awake=0x80,             // 100000 = Awake mode
        manual_single=0xC0,     // 110000 = Manual-Single mode
        manual_scan=0xC8,       // 110010 = Manual-Scan mode
        auto_single=0xC4,       // 110001 = Auto-Single mode
        auto_scan=0xCC,         // 110011 = Auto-Scan mode
        auto_single_sleep=0xE8, // 111001 = Auto-Single with Sleep mode
        auto_scan_sleep=0xEC,   // 111011 = Auto-Scan with Sleep mode
        auto_burst_sleep=0xFC   // 111111 = Auto-Burst Scan with Sleep mode
    } WorkingMode;

// Int and Conversion modes
    typedef enum  {
        Alarm              = 0,
        BusyAlarm          = 1,
        OneConversion      = 2,
        BusyOneConversion  = 3,
        FourConversions    = 6,
        BusyFourConversion = 7
    } IntMode;

    typedef enum {active_low = 0,active_high} PinPolarity;
    typedef enum {level_signal = 0, pulse_signal} IntPinSignaling;
    typedef enum {conversion_continue = 0, conversion_stoped} ConversionControl;
    typedef enum {div_by_1 = 0, div_by_4} SleepTimeDivider;
    typedef enum {mul_by_1 = 0, mul_by_8} SleepTimeMultiplier;
    typedef enum  {
        sleep_time_2_5ms    = 0,
        sleep_time_5ms,
        sleep_time_10ms,
        sleep_time_20ms,
        sleep_time_40ms,
        sleep_time_80ms,
        sleep_time_160ms,
        sleep_time_320ms
    } SleepTime;

    typedef enum {calibration_off = 0, calibration_on} Calibration;

// vref = reference voltage in mV default=3.3V
    ADCI2C_ADS7924(uint8_t i2c_addr, unsigned vref=3300);

// ---- Working mode --------------------

    void changeWorkingMode(WorkingMode mode);
    WorkingMode getWorkingMode();

// ---- Select the channel---------------------

    void selectChannel(uint8_t channel);
    uint8_t getSelectedChannel();

// ---- Get acquired data -------------------

    void manualCapture();

    unsigned getChannel(uint8_t channel);
    unsigned getMillivolts(uint8_t channel);

// ---- Thresholds ------------------------------

    unsigned getUpperThreshold(uint8_t channel);
    unsigned getLowerThreshold(uint8_t channel);
    void setUpperThreshold(uint8_t channel,uint8_t val);
    void setLowerThreshold(uint8_t channel,uint8_t val);

    unsigned getUpperMillivoltsThreshold(uint8_t channel);
    unsigned getLowerMillivoltsThreshold(uint8_t channel);
    void setUpperMillivoltsThreshold(uint8_t channel, unsigned millivolts);
    void setLowerMillivoltsThreshold(uint8_t channel, unsigned millivolts);

// ---- Alarms ----------------------------------

    bool getAlarmActivated(uint8_t channel);
    uint8_t getAlarmActivated();

    bool getAlarmStatus(uint8_t channel);
    uint8_t getAlarmStatus();

    bool getAlarmEnabled(uint8_t channel);
    uint8_t getAlarmEnabled();

    void setAlarmEnabled(uint8_t channel,bool st);

    uint8_t getAlarmCount(void);
    void setAlarmCount(uint8_t count);

// ---- Interrupt ----------------------------------

    IntMode getIntMode(void);
    void setIntMode(IntMode mode);

    PinPolarity getIntPinPolarity(void);
    void setIntPinPolarity(PinPolarity pol);

    IntPinSignaling getIntPinSignaling(void);
    void setIntPinSignaling(IntPinSignaling sig);

    void clearInterrupt();

// ---- Timers --------------------------------------

    ConversionControl getConversionControl(void);
    void setConversionControl(ConversionControl convctrl);

    SleepTimeDivider getSleepTimeDivider(void);
    void setSleepTimeDivider(SleepTimeDivider slpdiv4);

    SleepTimeMultiplier getSleepTimeMultiplier(void);
    void setSleepTimeMultiplier(SleepTimeMultiplier slpmul8);

    SleepTime getSleepTime(void) ;
    void setSleepTime(SleepTime slptime);

    unsigned getAcquireTimeReg(void);
    void setAcquireTimeReg(unsigned time);

    unsigned getAcquireTime(void);
    void setAcquireTime(unsigned time);

// ---- Reset, PinPower and calibration -----------------------

    Calibration getCalibration(void);
    void setCalibration(Calibration calcntl);

    bool getPwrPinEnable(void);
    void setPwrPinEnable(bool pwrconen);

    PinPolarity getPwrPinPolarity(void);
    void setPwrPinPolarity(PinPolarity pol);

    unsigned getPowerUpTime(void);
    void setPowerUpTime(unsigned time);

    void softReset();

private:
//---------------------------------------------------------------
// Private types
//---------------------------------------------------------------
        typedef enum  {
            ModeCntrl         = 0x00,                     // ADC Mode Control Register
            IntCntrl          = 0x01,                     // Interrupt Control Register
            DataChannel0      = 0x02,                     // Conversion Data for Channel ch
            DataChannel1      = 0x04,
            DataChannel2      = 0x06,
            DataChannel3      = 0x08,
            UpperThresholdChannel0 = 0x0A,                 // Threshold for Channel ch comprator
            LowerThresholdChannel0 = 0x0B,
            UpperThresholdChannel1 = 0x0C,
            LowerThresholdChannel1 = 0x0D,
            UpperThresholdChannel2 = 0x0E,
            LowerThresholdChannel2 = 0x0F,
            UpperThresholdChannel3 = 0x10,
            LowerThresholdChannel3 = 0x11,
            IntConfig         = 0x12,                     // Interrupt Configuration Register
            SlpConfig         = 0x13,                     // Sleep Configuration Register
            AcqConfig         = 0x14,                     // Acquire Configuration Register
            PwrConfig         = 0x15,                     // Power-Up Configuration Register
            Reset             = 0x16                      // Reset
        } ADS7924Register;
//---------------------------------------------------------------
    typedef union _IntConfigReg
    {
        uint8_t all;
        struct {
            uint8_t inttrig:1;// INTTRIG: INT output pin signaling
                              //   0 = Static signal for use with level triggering (default)
                              //   1 = Pulse signal for use with edge triggering
             uint8_t intpol:1;// INTPOL: INT pin polarity
                              //   0 = Active low (default)
                              //   1 = Active high
                         // These bits determine which signal is output on INT.
                         // They also select the conversion control event; see the CONVCTRL bit
                         // in the SLPCONFIG register. The configuration of these bits is shown in Table .
                         //
     // BIT SETTING     INT PIN CONFIGURATION                           CONVERSION CONTROL EVENT
     //   000               Alarm                                                 Alarm
     //   001               Busy                                                  Alarm
     //   010         Data ready: one conversion completed         Data ready: one conversion complete
     //   011               Busy                                   Data ready: one conversion complete
     //   100             Do not use                                               ----
     //   101             Do not use                                               ----
     //   110         Data ready: all four conversions complete    Data ready: four conversions complete
     //   111               Busy                                   Data ready: four conversions complete

            uint8_t intcnfg:3;// INTCNFG[2:0]: INT output pin configuration
            uint8_t almcnt:3; // ALMCNT[2:0]: Alarm count
                         // These bits set the number of times the comparator threshold limit
                         // (either upper or lower) must be exceeded to generate an alarm.
                         //   000 = Every conversion generates an alarm
                         //   010 = Exceeding the threshold limit 1 time generates an alarm condition
                         //   100 = Exceeding the threshold limit 2 times generates an alarm condition
                         //   110 = Exceeding the threshold limit 3 times generates an alarm condition
                         //   111 = Exceeding the threshold limit 4 times generates an alarm condition
                         //   101 = Exceeding the threshold limit 5 times generates an alarm condition
                         //   110 = Exceeding the threshold limit 6 times generates an alarm condition
                         //   111 = Exceeding the threshold limit 7 times generates an alarm condition
        };
    } IntConfigReg;
//---------------------------------------------------------------
    typedef union _SlpConfigReg
    {
        uint8_t all;

        struct {
            uint8_t slptime:3;   // slptime[2:0]: Sleep time setting
                            //   000 = 2.5ms (default)
                            //   001 = 5ms
                            //   010 = 10ms
                            //   011 = 20ms
                            //   100 = 40ms
                            //   101 = 80ms
                            //   110 = 160ms
                            //   111 = 320ms
            uint8_t slpconfig_b3:1;
            uint8_t slpmult8:1;  // SLPMULT8: Sleep time 8x multiplier
                            //   0 = Sleep time multiplier is '1' (default)
                            //   1 = Sleep time multiplier is '8'
            uint8_t slpdiv4:1;   // SLPDIV4: Sleep time 4x divider
                            //     This bit sets the SleepTime of the sleep clock.
                            //   0 = Sleep time divider is '1' (default)
                            //   1 = Sleep time divider is '4'
            uint8_t convctrl:1;  // CONVCTRL: Conversion control
                            //     This bit determines the conversion status after a conversion control event;
                            //     see the INTCNFG bits in the INTCONFIG register.
                            //   0 = Conversions continue, independent of the control event status (default)
                            //   1 = Conversions are stopped as soon as a control event occurs;
                            //       the event must be cleared to resume conversions
        };
    } SlpConfigReg;
//---------------------------------------------------------------
    typedef union _PwrConfigReg
    {
        uint8_t all;
        struct {
            uint8_t pwruptime:5;// PWRUPTIME[4:0]: Power-up time setting
                           // These bits set the power-up time (default = 0).
                           // tPWR = PWRUPTIME[4:0] × 2ms.
            uint8_t pwrconen:1; // PWRCONEN: PWRCON enable
                           // 0 = The PWRCON pin is disabled (default)
                           // 1 = The PWRCON pin is always enabled
            uint8_t pwrconpol:1;// PWRCONPOL: PWRCON pin polarity
                           // 0 = Active low (default)
                           // 1 = Active high
            uint8_t calcntl:1;  // CALCNTL: Calibration control
                           // 0 = Setting CH3 in the Mode Control register selects the CH3 input
                           //     to be routed to the MUXOUT pin. (default)
                           // 1 = Setting CH3 in the Mode Control register connects the MUXOUT pin to AGND.
        };
    } PwrConfigReg;

//---------------------------------------------------------------
// Private members
//---------------------------------------------------------------
    uint8_t m_u8I2CAddr;
    unsigned m_uVRef_mv;       // Reference voltage in mV

    WorkingMode m_eWorkingMode; // The ModeCntrl register show the actual chip mode. This field set the desired
                                 // mode when the acquisition is runing. Default = manual_single

    uint8_t m_u8SelectedChannel; // User selected channel

    unsigned  m_8AlarmEnabled; // The INTCNTRL register don't return enabled alarm status.
                               // It return if a masked alarm is active.

    unsigned getReg(ADS7924Register reg);
    void setReg(ADS7924Register reg,unsigned newValue);

};



#endif /* ADS7924_H */

