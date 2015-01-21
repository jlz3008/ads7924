/*
 Copyright © 2014 José Luis Zabalza  License LGPLv3+: GNU
 LGPL version 3 or later <http://www.gnu.org/copyleft/lgpl.html>.
 This is free software: you are free to change and redistribute it.
 There is NO WARRANTY, to the extent permitted by law.
*/

#include "ansi.h"

#include <Arduino.h>

//-----------------------------------------------------------
void ClearScreen(ClearMode mode)
{
    uint8_t v[]={27,'['};
    Serial.write(v,2);
    if(mode == to_cursor)
        Serial.print('1');
    else if(mode == all_section)
        Serial.print('2');
    Serial.print('J');
}
//-----------------------------------------------------------
void ClearLine(ClearMode mode)
{
    uint8_t v[]={27,'['};
    Serial.write(v,2);
    if(mode == to_cursor)
        Serial.print('1');
    else if(mode == all_section)
        Serial.print('2');
    Serial.print('K');
}
//-----------------------------------------------------------
void CursorUp(int pos)
{
    uint8_t v[]={27,'['};
    Serial.write(v,2);
    Serial.print(pos);
    Serial.print('A');
}
//-----------------------------------------------------------
void CursorDown(int pos)
{
    uint8_t v[]={27,'['};
    Serial.write(v,2);
    Serial.print(pos);
    Serial.print('B');
}
//-----------------------------------------------------------
void CursorLeft(int pos)
{
    uint8_t v[]={27,'['};
    Serial.write(v,2);
    Serial.print(pos);
    Serial.print('D');
}
//-----------------------------------------------------------
void CursorRight(int pos)
{
    uint8_t v[]={27,'['};
    Serial.write(v,2);
    Serial.print(pos);
    Serial.print('C');
}
//-----------------------------------------------------------
void SaveCursor()
{
    uint8_t v[]={27,'[','s'};
    Serial.write(v,3);
}
//-----------------------------------------------------------
void RestoreCursor()
{
    uint8_t v[]={27,'[','u'};
    Serial.write(v,3);
}
//-----------------------------------------------------------
void GotoColumn(int x)
{
    uint8_t v[]={27,'['};
    Serial.write(v,2);
    Serial.print(x);
    Serial.print('I');
    Serial.print("cursor colum");
}
//-----------------------------------------------------------
void GotoXY(int x,int y)
{
    uint8_t v[]={27,'['};
    Serial.write(v,2);
    Serial.print(y);
    Serial.print(";");
    Serial.print(x);
    Serial.print("H");
}
//-----------------------------------------------------------
void SetColor(Color c,bool Background, bool Bright)
{
    uint8_t resetAll[]={27,'[','0','m'};
    uint8_t v[]={27,'[','4','0',';','0','m'};
    if(c == Reset)
    {
        Serial.write(resetAll,4);
    }
    else
    {
        if(Bright)
            v[5]='1';
        else
            v[5]='0';
        if(Background)
            v[2]='4';
        else
            v[2]='3';
        v[3]=c;
        Serial.write(v,7);
    }
}
