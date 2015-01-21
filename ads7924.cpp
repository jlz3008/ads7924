/*
 Copyright © 2014 José Luis Zabalza  License LGPLv3+: GNU
 LGPL version 3 or later <http://www.gnu.org/copyleft/lgpl.html>.
 This is free software: you are free to change and redistribute it.
 There is NO WARRANTY, to the extent permitted by law.
*/

#include <ads7924.h>
#include <Arduino.h>


//---------------------------------------------------------------------------
ADCI2C_ADS7924::ADCI2C_ADS7924(uint8_t i2c_addr,unsigned vref)
{
    Wire.begin();

    m_u8I2CAddr = i2c_addr;
    m_uVRef_mv = vref;
    m_8AlarmEnabled = 0;
    m_u8SelectedChannel = 0;
    m_eWorkingMode = idle; // It's no posible execute getWorkingMode() here
}
//--------------------------------------------------------------------------------
unsigned ADCI2C_ADS7924::getMillivolts(uint8_t channel)
{
    unsigned long val;
    unsigned Result;

    val = getChannel(channel);
    Result = ((unsigned long)val * (unsigned long)m_uVRef_mv)/4096UL;

    return(Result);
}
//--------------------------------------------------------------------------------
unsigned ADCI2C_ADS7924::getChannel(uint8_t channel)
{
    return(getReg(ADS7924Register(DataChannel0+(channel * 2))) >> 4 );
}
//--------------------------------------------------------------------------------
uint8_t ADCI2C_ADS7924::getSelectedChannel()
{
    return(getReg(ModeCntrl) & 0x03);
}
//--------------------------------------------------------------------------------
void ADCI2C_ADS7924::selectChannel(uint8_t channel)
{
    m_u8SelectedChannel = channel;
    setReg(ModeCntrl,m_eWorkingMode | channel);
}
//--------------------------------------------------------------------------------
unsigned ADCI2C_ADS7924::getUpperMillivoltsThreshold(uint8_t channel)
{
    unsigned long val = getUpperThreshold(channel);
    return(((unsigned long)m_uVRef_mv * (unsigned long)val)  /  256UL)  ;
}
//--------------------------------------------------------------------------------
unsigned ADCI2C_ADS7924::getLowerMillivoltsThreshold(uint8_t channel)
{
    unsigned long val = getLowerThreshold(channel);
    return(((unsigned long)m_uVRef_mv * (unsigned long)val)  /  256UL)  ;
}
//--------------------------------------------------------------------------------
void ADCI2C_ADS7924::setUpperMillivoltsThreshold(uint8_t channel, unsigned millivolts)
{
    unsigned long val = (unsigned long)millivolts * 256UL / (unsigned long)m_uVRef_mv  ;
    setUpperThreshold(channel,val);
}
//--------------------------------------------------------------------------------
void ADCI2C_ADS7924::setLowerMillivoltsThreshold(uint8_t channel, unsigned millivolts)
{
    unsigned long val = (unsigned long)millivolts * 256UL / (unsigned long)m_uVRef_mv  ;
    setLowerThreshold(channel,val);
}
//--------------------------------------------------------------------------------
bool ADCI2C_ADS7924::getAlarmStatus(uint8_t channel)
{
    return((getReg(IntCntrl) & (1 << (channel + 4))) != 0);
}
//--------------------------------------------------------------------------------
uint8_t ADCI2C_ADS7924::getAlarmStatus()
{
    return(getReg(IntCntrl) >> 4);
}
//--------------------------------------------------------------------------------
uint8_t ADCI2C_ADS7924::getAlarmActivated()
{
    return(getReg(IntCntrl) & 0x0F);
}
//--------------------------------------------------------------------------------
bool ADCI2C_ADS7924::getAlarmActivated(uint8_t channel)
{
    return((getReg(IntCntrl) & (1 << channel)) != 0);
}
//--------------------------------------------------------------------------------
uint8_t ADCI2C_ADS7924::getAlarmEnabled()
{
    return(m_8AlarmEnabled & 0x0F);
}
//--------------------------------------------------------------------------------
bool ADCI2C_ADS7924::getAlarmEnabled(uint8_t channel)
{
    return((m_8AlarmEnabled & (1 << channel)) != 0);
}
//--------------------------------------------------------------------------------
void ADCI2C_ADS7924::setAlarmEnabled(uint8_t channel,bool st)
{
    if(st)
        m_8AlarmEnabled |= (1 << channel);
    else
        m_8AlarmEnabled &= ~(1 << channel);

    setReg(IntCntrl,m_8AlarmEnabled);
}
//--------------------------------------------------------------------------------
uint8_t ADCI2C_ADS7924::getAlarmCount(void)
{
    IntConfigReg val;

    val.all = getReg(IntConfig);
    return(val.almcnt);
}
//--------------------------------------------------------------------------------
void ADCI2C_ADS7924::setAlarmCount(uint8_t count)
{
    IntConfigReg val;

    val.all = getReg(IntConfig);
    val.almcnt = count;
    setReg(IntConfig,val.all);
}
//--------------------------------------------------------------------------------
ADCI2C_ADS7924::IntMode ADCI2C_ADS7924::getIntMode(void)
{
    IntConfigReg val;

    val.all = getReg(IntConfig);
    return(IntMode(val.intcnfg));
}
//--------------------------------------------------------------------------------
void ADCI2C_ADS7924::setIntMode(IntMode mode)
{
    IntConfigReg val;

    val.all = getReg(IntConfig);
    val.intcnfg = mode;
    setReg(IntConfig,val.all);
}
//--------------------------------------------------------------------------------
ADCI2C_ADS7924::PinPolarity ADCI2C_ADS7924::getIntPinPolarity(void)
{
    IntConfigReg val;

    val.all = getReg(IntConfig);
    return(PinPolarity(val.intpol));
}
//--------------------------------------------------------------------------------
void ADCI2C_ADS7924::setIntPinPolarity(PinPolarity pol)
{
    IntConfigReg val;

    val.all = getReg(IntConfig);
    val.intpol = pol;
    setReg(IntConfig,val.all);
}
//--------------------------------------------------------------------------------
ADCI2C_ADS7924::IntPinSignaling ADCI2C_ADS7924::getIntPinSignaling(void)
{
    IntConfigReg val;

    val.all = getReg(IntConfig);
    return(IntPinSignaling(val.inttrig));
}
//--------------------------------------------------------------------------------
void ADCI2C_ADS7924::setIntPinSignaling(IntPinSignaling sig)
{
    IntConfigReg val;

    val.all = getReg(IntConfig);
    val.inttrig = sig;
    setReg(IntConfig,val.all);
}
//--------------------------------------------------------------------------------
ADCI2C_ADS7924::ConversionControl ADCI2C_ADS7924::getConversionControl(void)
{
    SlpConfigReg val;

    val.all = getReg(SlpConfig);
    return(ConversionControl(val.convctrl));
}
//--------------------------------------------------------------------------------
void ADCI2C_ADS7924::setConversionControl(ConversionControl convctrl)
{
    SlpConfigReg val;

    val.all = getReg(SlpConfig);
    val.convctrl = convctrl;
    setReg(SlpConfig,val.all);
}
//--------------------------------------------------------------------------------
ADCI2C_ADS7924::SleepTimeDivider ADCI2C_ADS7924::getSleepTimeDivider(void)
{
    SlpConfigReg val;

    val.all = getReg(SlpConfig);
    return(SleepTimeDivider(val.slpdiv4));
}
//--------------------------------------------------------------------------------
void ADCI2C_ADS7924::setSleepTimeDivider(SleepTimeDivider slpdiv4)
{
    SlpConfigReg val;

    val.all = getReg(SlpConfig);
    val.slpdiv4 = slpdiv4;
    setReg(SlpConfig,val.all);
}
//------------------------------------------------------------------------------
ADCI2C_ADS7924::SleepTimeMultiplier ADCI2C_ADS7924::getSleepTimeMultiplier(void)
{
    SlpConfigReg val;

    val.all = getReg(SlpConfig);
    return(SleepTimeMultiplier(val.slpmult8));
}
//--------------------------------------------------------------------------------
void ADCI2C_ADS7924::setSleepTimeMultiplier(SleepTimeMultiplier slpmult8)
{
    SlpConfigReg val;

    val.all = getReg(SlpConfig);
    val.slpmult8 = slpmult8;
    setReg(SlpConfig,val.all);
}
//------------------------------------------------------------------------------
ADCI2C_ADS7924::SleepTime ADCI2C_ADS7924::getSleepTime(void)
{
    SlpConfigReg val;

    val.all = getReg(SlpConfig);
    return(SleepTime(val.slptime));
}
//--------------------------------------------------------------------------------
void ADCI2C_ADS7924::setSleepTime(SleepTime slptime)
{
    SlpConfigReg val;

    val.all = getReg(SlpConfig);
    val.slptime = slptime;
    setReg(SlpConfig,val.all);
}
//------------------------------------------------------------------------------
// in microsecs (min 6, max 68)
unsigned ADCI2C_ADS7924::getAcquireTime(void)
{
    return(getReg(AcqConfig) * 2 + 6);
}
//--------------------------------------------------------------------------------
void ADCI2C_ADS7924::setAcquireTime(unsigned time)
{
    if(time >= 6 && time <= 70)
        setReg(AcqConfig,(time - 6) / 2);
}
//------------------------------------------------------------------------------
// in register limits (min 0, max 31)
unsigned ADCI2C_ADS7924::getAcquireTimeReg(void)
{
    return(getReg(AcqConfig));
}
//--------------------------------------------------------------------------------
void ADCI2C_ADS7924::setAcquireTimeReg(unsigned time)
{
   setReg(AcqConfig,time  & 0x1F);
}
//------------------------------------------------------------------------------
ADCI2C_ADS7924::Calibration ADCI2C_ADS7924::getCalibration(void)
{
    PwrConfigReg val;

    val.all = getReg(PwrConfig);
    return(Calibration(val.calcntl));
}
//--------------------------------------------------------------------------------
void ADCI2C_ADS7924::setCalibration(Calibration calcntl)
{
    PwrConfigReg val;

    val.all = getReg(PwrConfig);
    val.calcntl = calcntl;
    setReg(PwrConfig,val.all);
}
//--------------------------------------------------------------------------------
bool ADCI2C_ADS7924::getPwrPinEnable(void)
{
    PwrConfigReg val;

    val.all = getReg(PwrConfig);
    return(val.pwrconen);
}
//--------------------------------------------------------------------------------
void ADCI2C_ADS7924::setPwrPinEnable(bool pwrconen)
{
    PwrConfigReg val;

    val.all = getReg(PwrConfig);
    val.pwrconen = pwrconen;
    setReg(PwrConfig,val.all);
}
//--------------------------------------------------------------------------------
ADCI2C_ADS7924::PinPolarity ADCI2C_ADS7924::getPwrPinPolarity(void)
{
    PwrConfigReg val;

    val.all = getReg(PwrConfig);
    return(PinPolarity(val.pwrconpol));
}
//--------------------------------------------------------------------------------
void ADCI2C_ADS7924::setPwrPinPolarity(PinPolarity pol)
{
    PwrConfigReg val;

    val.all = getReg(PwrConfig);
    val.pwrconpol = pol;
    setReg(PwrConfig,val.all);
}
//------------------------------------------------------------------------------
// in microsecs (min 0, max 32)
unsigned ADCI2C_ADS7924::getPowerUpTime(void)
{
    PwrConfigReg val;

    val.all = getReg(PwrConfig);
    return(val.pwruptime * 2);
}
//--------------------------------------------------------------------------------
void ADCI2C_ADS7924::setPowerUpTime(unsigned time)
{
    if(time >= 0 && time <= 32)
    {
        PwrConfigReg val;

        val.all = getReg(PwrConfig);
        val.pwruptime = time / 2;
        setReg(PwrConfig,val.all);
    }
}
//--------------------------------------------------------------------------------
void ADCI2C_ADS7924::softReset()
{
    setReg(Reset,0xAA);
}
//--------------------------------------------------------------------------------
void ADCI2C_ADS7924::manualCapture()
{
    if( m_eWorkingMode == manual_single || m_eWorkingMode == manual_scan ) // when actual mode is manual
    {                                                                      // we must force start capture
        setReg(ModeCntrl,m_eWorkingMode | m_u8SelectedChannel);
    }
}

//--------------------------------------------------------------------------------
void ADCI2C_ADS7924::changeWorkingMode(WorkingMode mode)
{
/*
    When switching between modes, be
    sure to first select the Awake mode and then switch
    to the desired mode. This procedure ensures the
    internal control logic is properly synchronized.

    Also recommends changing to awake mode before modify any
    register. In this library this is application responsability
*/
    if((mode != idle || mode != awake)
            &&
       (mode != manual_single || m_eWorkingMode != manual_single)) // when actual mode
    {
        setReg(ModeCntrl,awake | m_u8SelectedChannel);
        delay(10);
    }

    m_eWorkingMode = mode;
    setReg(ModeCntrl,mode | m_u8SelectedChannel);
}
//--------------------------------------------------------------------------------
ADCI2C_ADS7924::WorkingMode ADCI2C_ADS7924::getWorkingMode()
{
    return(WorkingMode(getReg(ModeCntrl) & 0xFC));
}
//--------------------------------------------------------------------------------
unsigned ADCI2C_ADS7924::getUpperThreshold(uint8_t channel)
{
    return(getReg(ADS7924Register(UpperThresholdChannel0 + (channel * 2))));
}
//--------------------------------------------------------------------------------
unsigned ADCI2C_ADS7924::getLowerThreshold(uint8_t channel)
{
    return(getReg(ADS7924Register(LowerThresholdChannel0 + (channel * 2))));
}
//--------------------------------------------------------------------------------
void ADCI2C_ADS7924::setUpperThreshold(uint8_t channel,uint8_t val)
{
    setReg(ADS7924Register(UpperThresholdChannel0 + (channel * 2)),val);
}
//--------------------------------------------------------------------------------
void ADCI2C_ADS7924::setLowerThreshold(uint8_t channel,uint8_t val)
{
    setReg(ADS7924Register(LowerThresholdChannel0 + (channel * 2)),val);
}
//--------------------------------------------------------------------------------
void ADCI2C_ADS7924::clearInterrupt()
{
    getReg(IntConfig);
/*
 * Datasheet say
 * To clear an interrupt from an alarm condition, read
 * the INTCONFIG register (12h). To clear an interrupt
 * from data ready, read the data registers. The interrupt
 * clears when the lower four bits are retrieved.
 *
 * I do not know how to interpret this sentence, but this code don't work
 *
    for(int i=0;i<8;i+=2)
        getReg(ADS7924Register(DC0L+i)) ;
 */
    for(int i=0;i<4;i++)
        getChannel(i);
}
//--------------------------------------------------------------------------------
// Private API
//--------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
unsigned ADCI2C_ADS7924::getReg(ADS7924Register reg)
{
    unsigned Result=0xFFFF;
    bool word;
    switch(reg)
    {

    case DataChannel0      :
    case DataChannel1      :
    case DataChannel2      :
    case DataChannel3      :
        word = true;
        break;
    default:
        word = false;
        break;
    }

    Wire.beginTransmission(m_u8I2CAddr);
    Wire.write(reg); // pointer reg
    Wire.endTransmission();

    uint8_t c;

// I don't know why, it doesn't work Reading Multiple Byte (Fig. 34 of the datasheet)
//  Wire.requestFrom(m_u8I2CAddr, uint8_t(((word) ? 2:1)));
    Wire.requestFrom(m_u8I2CAddr, uint8_t(1));
    if(Wire.available())
    {
        c = Wire.read();
#ifdef DEBUG
                Serial.print("LSB="); Serial.print(c,HEX);
#endif
                Result = c;
        if(word)
        {
// I don't know why, it doesn't work Reading Multiple Byte (Fig. 34 of the datasheet)
            Wire.beginTransmission(m_u8I2CAddr);
            Wire.write(reg+1); // pointer reg
            Wire.endTransmission();
            Wire.requestFrom(m_u8I2CAddr, uint8_t(1));
            if(Wire.available())
            {
                c = Wire.read();
#ifdef DEBUG
                Serial.print(" MSB="); Serial.println(c,HEX);
#endif
                Result = (Result << 8) + (unsigned(c));
            }
            else
            {
#ifdef DEBUG
                Serial.println("Error ");
#endif
                Result = 0xFFFF;
            }
        }
    }
#ifdef DEBUG
    else
        Serial.println("Error");
#endif


#ifdef DEBUG
    Serial.print("getReg["); Serial.print(uint8_t(reg),HEX);Serial.print("]->");Serial.println(Result,HEX);
#endif
    return(Result);
}

//-------------------------------------------------------------------------------
void ADCI2C_ADS7924::setReg(ADS7924Register reg, unsigned newValue)
{
    bool word;
#ifdef DEBUG
    Serial.print("setReg["); Serial.print(uint8_t(reg),HEX);Serial.print("]=");Serial.println(newValue,HEX);
#endif
    switch(reg)
    {
    case DataChannel0      :
    case DataChannel1      :
    case DataChannel2      :
    case DataChannel3      :
        word = true;
        break;
    default:
        word = false;
        break;
    }

    Wire.beginTransmission(m_u8I2CAddr);
    Wire.write(reg); // pointer reg
    if(word)
    {
        Wire.write((newValue >> 8) & 0xFF);
    }
    Wire.write(newValue & 0xFF);

    Wire.endTransmission();
}

