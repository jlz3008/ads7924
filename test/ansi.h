/*
 Copyright © 2014 José Luis Zabalza  License LGPLv3+: GNU
 LGPL version 3 or later <http://www.gnu.org/copyleft/lgpl.html>.
 This is free software: you are free to change and redistribute it.
 There is NO WARRANTY, to the extent permitted by law.
*/

// Only implement necessary ANSI functions to do simple menu work
// It is not the purpouse the implementation of the entire ANSI repertoire

#ifndef ANSI_H
#define ANSI_H


typedef enum {from_cursor,to_cursor,all_section} ClearMode;
void ClearScreen(ClearMode mode = all_section);
void ClearLine(ClearMode mode = all_section);

void CursorUp(int num=1);
void CursorDown(int num=1);
void CursorLeft(int num=1);
void CursorRight(int num=1);

typedef enum  {Reset = 0,
               Black = '0',
               Red =   '1',
               Green   = '2',
               Yellow  = '3',
               Blue    = '4',
               Magenta = '5',
               Cyan    = '6',
               White   = '7'
              } Color;

void SetColor(Color c,bool Background=false, bool Bright=false);
void SaveCursor();
void RestoreCursor();
void GotoXY(int x, int y);

#endif // ANSI_H
