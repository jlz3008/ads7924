/*
 Copyright © 2014 José Luis Zabalza  License LGPLv3+: GNU
 LGPL version 3 or later <http://www.gnu.org/copyleft/lgpl.html>.
 This is free software: you are free to change and redistribute it.
 There is NO WARRANTY, to the extent permitted by law.
*/


#define VERSION "1.1"

#include <inttypes.h>
#include <assert.h>

#include <ads7924.h>
#include "ansi.h"

#define SUBMENU_ROW 21

static const unsigned suVRef = 3300; // ADS7924 Voltage Reference (board specific)

static uint8_t sChannelSelected = 0; // application channel selected

//--------- ADC control object --------------------------------------------------

ADCI2C_ADS7924 adc = ADCI2C_ADS7924(0x48,suVRef);

//---------Volatile variables for interrupt routine -----------------------------

static volatile int status = LOW;
static volatile int sICount = 0;
static volatile bool sNeedClearInterrupt = false;
static volatile bool sNeedRefreshData = true;

//------Local prototipes---------------------------------------------------------

void Menu();

void SubMenuWorkingMode();
void SubMenuIntCfg();
void SubMenuSleepTime();
void ClearSubmenu();

const char * IntMode2str(ADCI2C_ADS7924::IntMode mode);
const char * WorkingMode2str(ADCI2C_ADS7924::WorkingMode mode);
const char * SleepTime2str(ADCI2C_ADS7924::SleepTime sltp);

int inputNumber(int & input, int maxnumchar, int min, int max);

void IdleProc();

//--------------------------------------------------------------------------------
void interfunc(void)
{
    status = !status;
    digitalWrite(13,status);
    sICount++;
    sNeedClearInterrupt = true;
}
//--------------------------------------------------------------------------------
void setup()
{
    Serial.begin(115200);
    Serial.println("Start");

    pinMode(13, OUTPUT);

    attachInterrupt(0, interfunc, RISING) ;

    ClearScreen();
    Menu();
}
//--------------------------------------------------------------------------------
void loop()
{
    char c;

    if(Serial.available())
    {
        c = Serial.read();
        switch(c)
        {
        case '0':
        case '1':
        case '2':
        case '3':
            sChannelSelected = c - '0';
            adc.selectChannel(sChannelSelected);
            ClearSubmenu();
            SetColor(Green,true,true);
            Serial.print("Selected channel is ");Serial.println(sChannelSelected);
            SetColor(Reset);
            sNeedRefreshData=true;
            break;
        case 'B':
        case 'b':
            adc.setCalibration((c=='B') ? ADCI2C_ADS7924::calibration_on : ADCI2C_ADS7924::calibration_off);
            ClearSubmenu();
            SetColor(Green,true,true);
            Serial.print("Calibration is ");
            Serial.println((adc.getCalibration()) ? "On":"Off");
            SetColor(Reset);
            break;
        case 'K':
        case 'k':
            adc.setAlarmEnabled(sChannelSelected,(c=='K'));
            ClearSubmenu();
            SetColor(Green,true,true);
            Serial.print("Alarm for channel ");Serial.print(sChannelSelected);
            Serial.print(" is ");SetColor(Green,false,true); Serial.println((adc.getAlarmEnabled(sChannelSelected)) ? "Active":"Inactive");SetColor(Reset);
            SetColor(Reset);
            sNeedRefreshData=true;
            break;
        case 'N':
        case 'n':
        {
            int count;
            ClearSubmenu();
            Serial.println("-------------- Alarm count  ---------------");
            Serial.println(" Input range 0 <= value <= 7");
            Serial.println(" 0 = every conversion");
            Serial.println("----------------------------------------------");

            switch(inputNumber(count,1,0,7))
            {
            case  0:
            {
                adc.setAlarmCount(count);
                ClearSubmenu();
                SetColor(Green,true,true);
                Serial.print("Alarm Count ");Serial.println(adc.getAlarmCount());
                SetColor(Reset);
            }
                break;
            case -1:
            {
                ClearSubmenu();
                Serial.println("--------------[ERROR]-------------------------");
                Serial.println(" Input range 0 <= value <= 7");
                Serial.println("----------------------------------------------");
            }
                break;
            default:
                ClearSubmenu();
                break;
            }
        }
            break;
        case 'U':
        {
            int Threshold;
            ClearSubmenu();
            Serial.println("-------------- Upper threshold ---------------");
            Serial.print(" Input range 0 <= value <= ");Serial.print(suVRef);
            Serial.println("----------------------------------------------");

            switch(inputNumber(Threshold,4,0,suVRef))
            {
            case  0:
            {
                adc.setUpperMillivoltsThreshold(sChannelSelected,Threshold);
                ClearSubmenu();
                SetColor(Green,true,true);
                Serial.print("Upper Threshold for channel ");Serial.print(sChannelSelected);
                Serial.print(" is ");Serial.print(adc.getUpperThreshold(sChannelSelected));
                Serial.print(" = ");Serial.print(adc.getUpperMillivoltsThreshold(sChannelSelected));Serial.println("mV");
                SetColor(Reset);
            }
                break;
            case -1:
            {
                ClearSubmenu();
                Serial.println("--------------[ERROR]-------------------------");
                Serial.print(" Input range 0 <= value <= ");Serial.print(suVRef);
                Serial.println("----------------------------------------------");
            }
                break;
            default:
                ClearSubmenu();
                break;
            }
        }
            break;
        case 'u':
        {
            int Threshold;
            ClearSubmenu();
            Serial.println("-------------- Upper threshold ---------------");
            Serial.println(" Input range 0 <= value <= 255");
            Serial.println("----------------------------------------------");
            switch(inputNumber(Threshold,3,0,255))
            {
            case  0:
            {
                adc.setUpperThreshold(sChannelSelected,Threshold);
                ClearSubmenu();
                SetColor(Green,true,true);
                Serial.print("Upper Threshold for channel ");Serial.print(sChannelSelected);
                Serial.print(" is ");Serial.print(adc.getUpperThreshold(sChannelSelected));
                Serial.print(" = ");Serial.print(adc.getUpperMillivoltsThreshold(sChannelSelected));Serial.println("mV");
                SetColor(Reset);
            }
                break;
            case -1:
            {
                ClearSubmenu();
                Serial.println("--------------[ERROR]-------------------------");
                Serial.println(" Input range 0 <= value <= 255");
                Serial.println("----------------------------------------------");
            }
                break;
            default:
                ClearSubmenu();
                break;
            }
        }
            break;
        case 'L':
        {
            int Threshold;
            ClearSubmenu();
            Serial.println("-------------- Lower threshold ---------------");
            Serial.print(" Input range 0 <= value <= ");Serial.print(suVRef);
            Serial.println("----------------------------------------------");
            switch(inputNumber(Threshold,4,0,suVRef))
            {
            case  0:
            {
                adc.setLowerMillivoltsThreshold(sChannelSelected,Threshold);
                ClearSubmenu();
                SetColor(Green,true,true);
                Serial.print("Lower Threshold for channel ");Serial.print(sChannelSelected);
                Serial.print(" is ");Serial.print(adc.getLowerThreshold(sChannelSelected));
                Serial.print(" = ");Serial.print(adc.getLowerMillivoltsThreshold(sChannelSelected));Serial.println("mV");
                SetColor(Reset);
            }
                break;
            case -1:
            {
                ClearSubmenu();
                Serial.println("--------------[ERROR]-------------------------");
                Serial.print(" Input range 0 <= value <= ");Serial.print(suVRef);
                Serial.println("----------------------------------------------");
            }
                break;
            default:
                ClearSubmenu();
                break;
            }
        }
            break;
        case 'l':
        {
            int Threshold;
            ClearSubmenu();
            Serial.println("-------------- Lower threshold ---------------");
            Serial.println(" Input range 0 <= value <= 255");
            Serial.println("----------------------------------------------");
            switch(inputNumber(Threshold,3,0,255))
            {
            case  0:
            {
                adc.setLowerThreshold(sChannelSelected,Threshold);
                ClearSubmenu();
                SetColor(Green,true,true);
                Serial.print("Lower Threshold for channel ");Serial.print(sChannelSelected);
                Serial.print(" is ");Serial.print(adc.getLowerThreshold(sChannelSelected));
                Serial.print(" = ");Serial.print(adc.getLowerMillivoltsThreshold(sChannelSelected));Serial.println("mV");
                SetColor(Reset);
            }
                break;
            case -1:
            {
                ClearSubmenu();
                Serial.println("--------------[ERROR]-------------------------");
                Serial.println(" Input range 0 <= value <= 255");
                Serial.println("----------------------------------------------");
            }
                break;
            default:
                ClearSubmenu();
                break;
            }
        }
            break;
        case 'A':
        {
            int ATime;
            ClearSubmenu();
            Serial.println("-------------- Acquire time in uS-------------");
            Serial.println(" Input range 6 <= value <= 68");
            Serial.println("----------------------------------------------");
            switch(inputNumber(ATime,2,6,68))
            {
            case  0:
            {
                adc.setAcquireTime(ATime);
                ClearSubmenu();
                SetColor(Green,true,true);
                Serial.print("Acquire Time Reg =");Serial.print(adc.getAcquireTimeReg());
                Serial.print(" = ");Serial.print(adc.getAcquireTime());Serial.println(" uS");
                SetColor(Reset);
            }
                break;
            case -1:
            {
                ClearSubmenu();
                Serial.println("--------------[ERROR]-------------------------");
                Serial.println(" Input range 6 <= value <= 68");
                Serial.println("----------------------------------------------");
            }
                break;
            default:
                ClearSubmenu();
                break;
            }
        }
            break;
        case 'a':
        {
            int ATime;
            ClearSubmenu();
            Serial.println("-------------- Acquire Reg ------------------");
            Serial.println(" Input range 0 <= value <= 31");
            Serial.println("----------------------------------------------");
            switch(inputNumber(ATime,2,0,31))
            {
            case  0:
            {
                adc.setAcquireTimeReg(ATime);
                ClearSubmenu();
                SetColor(Green,true,true);
                Serial.print("Acquire Time Reg =");Serial.print(adc.getAcquireTimeReg());
                Serial.print(" = ");Serial.print(adc.getAcquireTime());Serial.println(" uS");
                SetColor(Reset);
            }
                break;
            case -1:
            {
                ClearSubmenu();
                Serial.println("--------------[ERROR]-------------------------");
                Serial.println(" Input range 0 <= value <= 31");
                Serial.println("----------------------------------------------");
            }
                break;
            default:
                ClearSubmenu();
                break;
            }
        }
            break;
        case 'G':
        case 'g':
            SubMenuIntCfg();
            ClearSubmenu();
            SetColor(Green,true,true);
            Serial.print("INT mode = ");     Serial.println(IntMode2str(adc.getIntMode()));
            SetColor(Reset);
            break;
        case 'E':
        case 'e':
            SubMenuSleepTime();
            ClearSubmenu();
            SetColor(Green,true,true);
            Serial.print("Sleep time ="); Serial.println(SleepTime2str(adc.getSleepTime()));
            SetColor(Reset);
            break;
        case 'P':
            adc.setIntPinPolarity(ADCI2C_ADS7924::active_high);
            ClearSubmenu();
            SetColor(Green,true,true);
            Serial.print("Int polarity is active ");Serial.println((adc.getIntPinPolarity()) ? "High" : "low");
            SetColor(Reset);
            break;
        case 'p':
            adc.setIntPinPolarity(ADCI2C_ADS7924::active_low);
            ClearSubmenu();
            SetColor(Green,true,true);
            Serial.print("Int polarity is active ");Serial.println((adc.getIntPinPolarity()) ? "High" : "low");
            SetColor(Reset);
            break;
        case 'S':
            adc.setIntPinSignaling(ADCI2C_ADS7924::pulse_signal);
            ClearSubmenu();
            SetColor(Green,true,true);
            Serial.print("Int signal by ");Serial.println((adc.getIntPinSignaling()) ? "Pulse" : "Level");
            SetColor(Reset);
            break;
        case 's':
            adc.setIntPinSignaling(ADCI2C_ADS7924::level_signal);
            ClearSubmenu();
            SetColor(Green,true,true);
            Serial.print("Int signal by ");Serial.println((adc.getIntPinSignaling()) ? "Pulse" : "Level");
            SetColor(Reset);
            break;
        case 'V':
        case 'v':
            adc.setConversionControl((c == 'V') ?ADCI2C_ADS7924::conversion_stoped:ADCI2C_ADS7924::conversion_continue);
            ClearSubmenu();
            SetColor(Green,true,true);
            Serial.print("Conversion control after conversion event ");Serial.println((adc.getConversionControl()) ? "Stop" : "Continue");
            SetColor(Reset);
            break;
        case 'J':
        case 'j':
            adc.setSleepTimeDivider((c == 'J') ? ADCI2C_ADS7924::div_by_4:ADCI2C_ADS7924::div_by_1);
            ClearSubmenu();
            SetColor(Green,true,true);
            Serial.print("Sleep time Divider ");Serial.println((adc.getSleepTimeDivider()) ? "/4" : "/1");
            SetColor(Reset);
            break;
        case 'T':
        case 't':
            adc.setSleepTimeMultiplier((c == 'T') ? ADCI2C_ADS7924::mul_by_8 : ADCI2C_ADS7924::mul_by_1);
            ClearSubmenu();
            SetColor(Green,true,true);
            Serial.print("Sleep time Multiplier");Serial.println((adc.getSleepTimeMultiplier()) ? "x8" : "x1");
            SetColor(Reset);
            break;
        case 'I':
        case 'i':
            ClearSubmenu();
            Serial.println("----------------------------------------------");
            Serial.print("Working mode = ");
                     SetColor(Green,false,true);Serial.println(WorkingMode2str(adc.getWorkingMode()));SetColor(Reset);
            Serial.print("Calibration is ");
                     SetColor(Green,false,true);Serial.println((adc.getCalibration()) ? "On":"Off");SetColor(Reset);
            Serial.print("Int polarity is active ");
                     SetColor(Green,false,true);Serial.println((adc.getIntPinPolarity()) ? "High" : "low");SetColor(Reset);
            Serial.print("Int signal by ");
                     SetColor(Green,false,true);Serial.println((adc.getIntPinSignaling()) ? "Pulse" : "Level");SetColor(Reset);
            Serial.print("INT mode = ");
                     SetColor(Green,false,true); Serial.println(IntMode2str(adc.getIntMode()));SetColor(Reset);
            Serial.print("Conversion control after conversion event ");
                     SetColor(Green,false,true);Serial.println((adc.getConversionControl()) ? "Stop" : "Continue");SetColor(Reset);
            Serial.print("Acquire Time Reg = ");
                      SetColor(Green,false,true);Serial.print(adc.getAcquireTimeReg());SetColor(Reset);
                      Serial.print(" = ");SetColor(Green,false,true);Serial.print(adc.getAcquireTime());Serial.println(" uS");SetColor(Reset);
            Serial.print("Alarm Count   = ");
                      SetColor(Green,false,true); Serial.print(adc.getAlarmCount());SetColor(Reset);Serial.println("");
            for(int i=0;i<4;i++)
            {
                Serial.print("Alarm for channel ");Serial.print(i);
                Serial.print(" is ");SetColor(Green,false,true); Serial.println((adc.getAlarmEnabled(i)) ? "Active":"Inactive");SetColor(Reset);

                Serial.print("Lower Threshold for channel ");Serial.print(i);
                Serial.print(" is ");SetColor(Green,false,true);Serial.print(adc.getLowerThreshold(i));SetColor(Reset);
                Serial.print(" = ");SetColor(Green,false,true);Serial.print(adc.getLowerMillivoltsThreshold(i));Serial.println(" mV");SetColor(Reset);

                Serial.print("Upper Threshold for channel ");Serial.print(i);
                Serial.print(" is ");SetColor(Green,false,true);Serial.print(adc.getUpperThreshold(i));SetColor(Reset);
                Serial.print(" = ");SetColor(Green,false,true);Serial.print(adc.getUpperMillivoltsThreshold(i));Serial.println(" mV");SetColor(Reset);
            }
            Serial.println("----------------------------------------------");
            sNeedRefreshData = true;
            break;
        case 'C':
        case 'c':
            adc.clearInterrupt();
            sICount = 0;
            ClearSubmenu();
            SetColor(Green,true,true);
            Serial.println("Interrupt is clear");
            SetColor(Reset);
            sNeedRefreshData = true;
            break;
        case 'M':
        case 'm':
            SubMenuWorkingMode();
            ClearSubmenu();
            SetColor(Green,true,true);
            Serial.print("Working mode=");Serial.println(WorkingMode2str(adc.getWorkingMode()));
            SetColor(Reset);
            sNeedRefreshData = true;
            break;
        case 'R':
        case 'r':
            adc.softReset();
            ClearSubmenu();
            SetColor(Green,true,true);
            Serial.println("ADC was reset");
            SetColor(Reset);
            sNeedRefreshData = true;
            break;
        case ' ':
            ClearSubmenu();

            adc.manualCapture();

            for(int i=0;i<4;i++)
            {
                Serial.print("Channel [");Serial.print(i);  Serial.print("] = ");
                SetColor(Green,true,true); Serial.print(adc.getChannel(i),HEX); SetColor(Reset);
                Serial.print(" = ");
                SetColor(Green,false,true); Serial.print(adc.getMillivolts(i)); SetColor(Reset);
                Serial.print(" mV");
                GotoXY(35,SUBMENU_ROW+i);
                Serial.print("Low = ");
                SetColor(Green,false,true); Serial.print(adc.getLowerThreshold(i),HEX);  SetColor(Reset);
                Serial.print(" = ");
                SetColor(Green,false,true); Serial.print(adc.getLowerMillivoltsThreshold(i)); SetColor(Reset);
                Serial.print(" mV");
                GotoXY(60,SUBMENU_ROW+i);
                Serial.print("Upp = ");
                SetColor(Green,false,true); Serial.print(adc.getUpperThreshold(0),HEX);  SetColor(Reset);
                Serial.print(" = ");
                SetColor(Green,false,true); Serial.print(adc.getUpperMillivoltsThreshold(i)); SetColor(Reset);
                Serial.println(" mV");
            }
            Serial.println("----------------------------------------------------------------------------");
            sNeedRefreshData = true;
            break;
        }
    }// if serial available
    else
        IdleProc();
}
//-----------------------------------------------------------
void IdleProc()
{
    static int siClr;
    if(sNeedClearInterrupt)
    {
        sNeedClearInterrupt = false;
        adc.clearInterrupt();

        SaveCursor();
        GotoXY(25,3);
        SetColor(Red,true,true);
        Serial.print("Clear Interrupt");
        SetColor(Reset);
        RestoreCursor();
        siClr = 50;
    }
    else
    {
        if(siClr > 0)
        {
            siClr--;
            SaveCursor();
            GotoXY(25,3);
            SetColor(Red,true,true);
            Serial.print("Clear Interrupt");
            SetColor(Reset);
            RestoreCursor();
        }
        else if(siClr == 0)
        {
            siClr--;
            sNeedRefreshData = true;
        }
    }
    if(sNeedRefreshData)
    {
        sNeedRefreshData = false;
        GotoXY(1,3);
        ClearLine(); Serial.print(" Interrupt counter = ");
        SetColor(Yellow,true,true); Serial.print(sICount);SetColor(Reset);Serial.println("");
        ClearLine();Serial.print(" Channel selected  = ");
        SetColor(Yellow,true,true); Serial.print(sChannelSelected);SetColor(Reset);Serial.println("");
        ClearLine();Serial.print(" Mode              = ");
        SetColor(Yellow,true,true); Serial.println(WorkingMode2str(adc.getWorkingMode()));SetColor(Reset);
        GotoXY(40,3); Serial.print("Alarm Enabled = ");SetColor(Yellow,true,true); Serial.print(adc.getAlarmEnabled(),HEX);SetColor(Reset);Serial.println("");
        GotoXY(40,4); Serial.print("Alarm Status  = ");SetColor(Yellow,true,true); Serial.print(adc.getAlarmStatus(),HEX);SetColor(Reset);Serial.println("");
        GotoXY(40,5); Serial.print("Alarm Active  = ");SetColor(Yellow,true,true); Serial.print(adc.getAlarmActivated(),HEX);SetColor(Reset);Serial.println("");
        GotoXY(1,21);
    }
}

//-----------------------------------------------------------
int inputNumber(int & input, int maxnumchar, int min, int max)
{
    char val[10];
    int index = 0;
    int Result = 1 ; // continue
    char c;

    assert(maxnumchar < 10);

    while(Result == 1)
    {
        while(!Serial.available())
            ;

        c = Serial.read();
        if(c >= '0' && c <= '9')
        {
            if(index >= maxnumchar)
            {
                Result = -1; // error
            }
            else
            {
                val[index++] = c;
                Serial.print(c);
            }
        }
        else if(c == 27) // esc
        {
            Result = -2; // cancel the input
        }
        else if(c == 8) // BS
        {
            if(index > 0)
            {
                CursorLeft();
                ClearLine(from_cursor);
                index--;
            }
        }
        else if(c == '\r')
        {
            // End numerical value capture
            val[index] = '\0';
            input = atol(val);
            if( input > max || input < min)
                Result = -1; // error
            else
                Result = 0; // OK
        }
    }
    return(Result);
}

//-----------------------------------------------------------
void Menu()
{
    Serial.println("ADS7924 Test Aplication ");
    Serial.println("=======================");
    Serial.println("");
    Serial.println("");
    Serial.println("");

    Serial.println("----------------------------------------------------------------------------");
    Serial.println(" 0.- Select channel 0                     1.- Select channel 1");
    Serial.println(" 2.- Select channel 2                     3.- Select channel 3");
    Serial.println(" B/b Set calibration (On/off)                                  ");
    Serial.println(" K/k Alarm selected channel (Enab/Disab)  N.- Set Alarm Count" );
    Serial.println(" U/u Set threshold High (mV/capture)      L/l Set threshold Low  (mV/capture)");
    Serial.println(" T/t Sleep time multiplier (x8/x1)        J/j Sleep time divider (div4/div1)");
    Serial.println(" V/v Set conversion control (Stop/Cont)   A/a Acquire Time (uS/reg)");
    Serial.println(" P/p Set Int Polarity (high/low)          S/s Set Int Signaling (pulse/level)");
    Serial.println(" M.- Change Working Mode                  G.- Change Int config mode");
    Serial.println(" E.- Change Sleep time                    R.- Soft Reset          ");
    Serial.println(" C.- clear Interrupt                      I.- Info");
    Serial.println("----------------------------------------------------------------------------");
    Serial.println("                        SPACE make a capture");
    Serial.println("----------------------------------------------------------------------------");
}
//-----------------------------------------------------------
void ClearSubmenu()
{
    GotoXY(1,SUBMENU_ROW);
    ClearScreen(from_cursor);
}
//-----------------------------------------------------------
void SubMenuWorkingMode()
{
    bool selected = false;

    ADCI2C_ADS7924::WorkingMode WorkingMode = ADCI2C_ADS7924::manual_single;

    ClearSubmenu();
    Serial.println(" ADS7924 Working Mode");
    Serial.println("======================");
    Serial.println(" 0.- Idle");
    Serial.println(" 1.- AWAKE");
    Serial.println(" 2.- manual single");
    Serial.println(" 3.- manual scan");
    Serial.println(" 4.- auto single");
    Serial.println(" 5.- auto scan");
    Serial.println(" 6.- auto single sleep");
    Serial.println(" 7.- auto scan sleep");
    Serial.println(" 8.- auto burst sleep");
    Serial.println("   ESC = Cancel");
    while(true)
    {
        while(!Serial.available())
            IdleProc();

        switch(Serial.read())
        {
        case '0':WorkingMode = ADCI2C_ADS7924::idle; selected = true;break;
        case '1':WorkingMode = ADCI2C_ADS7924::awake; selected = true;break;
        case '2':WorkingMode = ADCI2C_ADS7924::manual_single; selected = true;break;
        case '3':WorkingMode = ADCI2C_ADS7924::manual_scan;selected = true;break;
        case '4':WorkingMode = ADCI2C_ADS7924::auto_single;selected = true;break;
        case '5':WorkingMode = ADCI2C_ADS7924::auto_scan;selected = true;break;
        case '6':WorkingMode = ADCI2C_ADS7924::auto_single_sleep;selected = true;break;
        case '7':WorkingMode = ADCI2C_ADS7924::auto_scan_sleep;selected = true;break;
        case '8':WorkingMode = ADCI2C_ADS7924::auto_burst_sleep;selected = true;break;
        case 27: goto fin;  break;
        }
        if(selected)
            break;
    }

    adc.changeWorkingMode(WorkingMode);

fin: ;
}

//--------------------------------------------------------------------------------
void SubMenuIntCfg()
{
    bool selected = false;
    ADCI2C_ADS7924::IntMode mode;

    ClearSubmenu();
    Serial.println(" ADS7924 Int signal Mode");
    Serial.println("=========================");
    Serial.println(" 0.- Alarm");
    Serial.println(" 1.- BusyAlarm          ");
    Serial.println(" 2.- OneConversion      ");
    Serial.println(" 3.- BusyOneConversion  ");
    Serial.println(" 4.- FourConversions    ");
    Serial.println(" 5.- BusyFourConversion ");
    Serial.println("    ESC = Cancel");
    while(true)
    {
        while(!Serial.available())
            IdleProc();
        switch(Serial.read())
        {
        case '0':mode = ADCI2C_ADS7924::Alarm; selected = true;break;
        case '1':mode = ADCI2C_ADS7924::BusyAlarm; selected = true;break;
        case '2':mode = ADCI2C_ADS7924::OneConversion;selected = true;break;
        case '3':mode = ADCI2C_ADS7924::BusyOneConversion;selected = true;break;
        case '4':mode = ADCI2C_ADS7924::FourConversions;selected = true;break;
        case '5':mode = ADCI2C_ADS7924::BusyFourConversion;selected = true;break;
        case 27: goto fin;  break;
        }
        if(selected)
            break;
    }

    adc.setIntMode(mode);

fin: ;
}

//--------------------------------------------------------------------------------
void SubMenuSleepTime()
{
    bool selected = false;
    ADCI2C_ADS7924::SleepTime sltp;


    ClearSubmenu();
    Serial.println(" ADS7924 Sleep Time");
    Serial.println("====================");
    Serial.println(" 1.- 2.5ms");
    Serial.println(" 2.- 5ms");
    Serial.println(" 3.- 10ms");
    Serial.println(" 4.- 20ms");
    Serial.println(" 5.- 40ms");
    Serial.println(" 6.- 80ms");
    Serial.println(" 7.- 160ms");
    Serial.println(" 8.- 320ms");
    Serial.println("    ESC = Cancel");
    while(true)
    {
        while(!Serial.available())
            IdleProc();
        switch(Serial.read())
        {
        case '1':sltp = ADCI2C_ADS7924::sleep_time_2_5ms; selected = true;break;
        case '2':sltp = ADCI2C_ADS7924::sleep_time_5ms;selected = true;break;
        case '3':sltp = ADCI2C_ADS7924::sleep_time_10ms;selected = true;break;
        case '4':sltp = ADCI2C_ADS7924::sleep_time_20ms;selected = true;break;
        case '5':sltp = ADCI2C_ADS7924::sleep_time_40ms;selected = true;break;
        case '6':sltp = ADCI2C_ADS7924::sleep_time_80ms;selected = true;break;
        case '7':sltp = ADCI2C_ADS7924::sleep_time_160ms;selected = true;break;
        case '8':sltp = ADCI2C_ADS7924::sleep_time_320ms;selected = true;break;
        case 27: goto fin;  break;
        }
        if(selected)
            break;
    }

    adc.setSleepTime(sltp);

fin: ;
}
//--------------------------------------------------------------------------------
const char * WorkingMode2str(ADCI2C_ADS7924::WorkingMode mode)
{
    switch(mode)
    {
        case ADCI2C_ADS7924::idle :              return("idle"); break;
        case ADCI2C_ADS7924::awake :             return("awake"); break;
        case ADCI2C_ADS7924::manual_single :     return("manual single"); break;
        case ADCI2C_ADS7924::manual_scan :       return("manual scan"); break;
        case ADCI2C_ADS7924::auto_single :       return("auto single"); break;
        case ADCI2C_ADS7924::auto_scan :         return("auto scan"); break;
        case ADCI2C_ADS7924::auto_single_sleep : return("auto single sleep"); break;
        case ADCI2C_ADS7924::auto_scan_sleep :   return("auto scan sleep"); break;
        case ADCI2C_ADS7924::auto_burst_sleep :  return("auto burst sleep"); break;
        default:                                 return("not defined mode") ; break;
    }
}
//--------------------------------------------------------------------------------
const char * IntMode2str(ADCI2C_ADS7924::IntMode mode)
{
    switch(mode)
    {
        case ADCI2C_ADS7924::Alarm :             return("Alarm"); break;
        case ADCI2C_ADS7924::BusyAlarm :         return("Busy - Alarm"); break;
        case ADCI2C_ADS7924::OneConversion :     return("One Conversion"); break;
        case ADCI2C_ADS7924::BusyOneConversion : return("Busy - OneConversion"); break;
        case ADCI2C_ADS7924::FourConversions :   return("Four Conversions"); break;
        case ADCI2C_ADS7924::BusyFourConversion: return("Busy - FourConversion"); break;
        default:                                 return("not defined mode") ; break;
    }
}
//--------------------------------------------------------------------------------
const char * SleepTime2str(ADCI2C_ADS7924::SleepTime sltp)
{
    switch(sltp)
    {
    case ADCI2C_ADS7924::sleep_time_2_5ms: return("2.5 ms"); break;
    case ADCI2C_ADS7924::sleep_time_5ms:   return("5 ms"); break;
    case ADCI2C_ADS7924::sleep_time_10ms:  return("10 ms"); break;
    case ADCI2C_ADS7924::sleep_time_20ms:  return("20 ms"); break;
    case ADCI2C_ADS7924::sleep_time_40ms:  return("40 ms"); break;
    case ADCI2C_ADS7924::sleep_time_80ms:  return("80 ms"); break;
    case ADCI2C_ADS7924::sleep_time_160ms: return("160 ms"); break;
    case ADCI2C_ADS7924::sleep_time_320ms: return("320 ms"); break;
    default:           return("not defined time") ; break;
    }
}
