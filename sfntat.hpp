// -----------------------------------------------------------------
//
// Author: Albert Zedlitz
// File  : sfntat.cpp
// Date  : 11.08.2017
// Abstract:
//    Ascii Table
//    Windows specific parts
//
// Copyright (C) 2017  Albert Zedlitz
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

// -----------------------------------------------------------------
#pragma once

#ifdef SAPonNT
#include "sfxxat.hpp"

//------------------------------------------------------------------------------
// SfCTable::setColor
//------------------------------------------------------------------------------
SAP_UINT SfCTable::setColor ( const EColor    aColor ) {
    SAP_UINT      lAttr;
    EColor        lNewColor = aColor;

    switch (aColor) {
    case COLOR_TOGGLE:
        mAttr.mColor = COLOR_TOGGLE_DARK;
        lNewColor    = COLOR_BRIGHT;
        break;

    case COLOR_TOGGLE_DARK:
        mAttr.mColor = COLOR_TOGGLE;
        lNewColor    = COLOR_DARK;
        break;

    case COLOR_SAME:
        if ((mAttr.mColor == COLOR_TOGGLE) || (mAttr.mColor == COLOR_TOGGLE_DARK)) {
            lNewColor = mAttr.mColor == COLOR_TOGGLE ? COLOR_DARK : COLOR_BRIGHT;
        }
        else {
            setAttr(sCurAttr);
            return sCurAttr;
        }
        break;

    default:
        break;
    }

    switch (lNewColor)
    {
    case COLOR_INVERSE: lAttr = 8 << 4 | 0xF; break;
    case COLOR_CHAR:
    case COLOR_BRIGHT:  lAttr = 7 << 4 | 0x0; break;
    case COLOR_DARK:    lAttr = 8 << 4 | 0x0; break;
    case COLOR_RED:     lAttr = 4 << 4 | 0xF; break;
    case COLOR_GREEN:   lAttr = 2 << 4 | 0xF; break;
    case COLOR_NONE:
        return sCurAttr;  // keep current color
    default:            lAttr = 7 << 4 | 0x0; break;
    }

    sCurAttr = lAttr;
    return setAttr(lAttr);
}

//------------------------------------------------------------------------------
// SfCTable::setAttr
//------------------------------------------------------------------------------
SAP_UINT SfCTable::setAttr( const SAP_UINT aAttr ){

    CONSOLE_SCREEN_BUFFER_INFO lConsoleScreenBufferInfo;
    SAP_UINT      lOldAttr;
    HANDLE        hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

    GetConsoleScreenBufferInfo(hStdout, &lConsoleScreenBufferInfo);
    lOldAttr = (SAP_UINT) lConsoleScreenBufferInfo.wAttributes;

    SetConsoleTextAttribute(hStdout, (WORD) aAttr);
    return lOldAttr;
}

//------------------------------------------------------------------------------
// SfCTable::endline
//------------------------------------------------------------------------------
void SfCTable::endline( const EColor aColor ) {
    const SAP_UC    *lA7Ptr;
    HANDLE          hStdout;
    SAP_UINT        lAttr;
    DWORD           lNrWritten;
    DWORD           lNrBytes;
    TString         lBuffer = mOutput.str();

    hStdout  = GetStdHandle(STD_OUTPUT_HANDLE);
    lNrBytes = (mOutput.str()).size();
    
    if (spOut != NULL) {
        fprintf(spOut, "\n%s", mOutput.str().c_str());
    } 
    else {
        if (aColor == COLOR_CHAR) {
            //----------------------------------------------------------------------
            // COLOR_CHAR sets color for different characters
            //----------------------------------------------------------------------
            lAttr = setAttr(0);
            WriteFile(hStdout, "\n", 1, &lNrWritten, NULL);
            lAttr = setAttr(sCurAttr);

            for (lA7Ptr = lBuffer.c_str(); *lA7Ptr != '\0';  lA7Ptr ++) {
                if (*lA7Ptr == '*') {
                    lAttr = setAttr(lAttr | FOREGROUND_BLUE);
                }
                else if (*lA7Ptr == '#') {
                    lAttr = setAttr(lAttr | 0xF );
                }
                else {
                    lAttr = setAttr(lAttr | 0xF );
                }
                WriteFile(hStdout, lA7Ptr, 1, &lNrWritten, NULL);
                setAttr(lAttr);
            }
            setAttr(lAttr);
        }
        else {
            setAttr(0);
            WriteFile(hStdout, "\n", 1, &lNrWritten, NULL);
            setColor(aColor);
            WriteFile(hStdout, lBuffer.c_str(), lNrBytes, &lNrWritten, NULL);
        }
    }
    mOutput.str("");
}

#endif

