// -----------------------------------------------------------------
//
// Author: Albert Zedlitz
// File  : sfuxat.cpp
// Date  : 11.08.2017
// Abstract:
//    Ascii Table
//    UNIX specific parts
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

#ifdef SAPonUNIX
#include <iostream>
using namespace std;

//------------------------------------------------------------------------------
// SfCTable::setColor
//------------------------------------------------------------------------------
SAP_UINT SfCTable::setColor( const EColor        aColor ) {
    SAP_UINT                lAttr;
    EColor                lNewColor = aColor;

    switch (aColor)
    {
    case COLOR_TOGGLE:
            mAttr.mColor = COLOR_TOGGLE_DARK;
            lNewColor    = COLOR_BRIGHT;
            break;

    case COLOR_TOGGLE_DARK:
            mAttr.mColor = COLOR_TOGGLE;
            lNewColor    = COLOR_DARK;
            break;

    default:
            break;
    }

    switch (lNewColor)
    {
    case COLOR_INVERSE:     lAttr = 8 << 4 | 7;     break;  // cyan   - white 
    case COLOR_CHAR:        lAttr = 0 << 4 | 7;     break;  // black  - white
    case COLOR_BRIGHT:      lAttr = 7 << 4 | 0;     break;  // gray   - black
    case COLOR_DARK:        lAttr = 8 << 4 | 0;     break;  // cyan   - black
    case COLOR_RED:         lAttr = 4 << 4 | 7;     break;  // red    - white
    case COLOR_GREEN:       lAttr = 2 << 4 | 7;     break;  // green  - white
    case COLOR_NONE:
        return sCurAttr;        // keep current color
    default:                lAttr = 7 << 4 | 0;     break;  // gray   - black
    }

    sCurAttr = lAttr;
    return setAttr(lAttr);
}

//------------------------------------------------------------------------------
// SfCTable::setAttr
//------------------------------------------------------------------------------
SAP_UINT SfCTable::setAttr( const SAP_UINT aAttr ) {
    SAP_UINT  lOldAttr    = sCurAttr;
    SAP_UINT  lForeground = 0xF & (aAttr);
    SAP_UINT  lBackground = 0xF & (aAttr >> 4);

#ifdef SAPwithPASE400
    if (!isatty(0)) {
        return 0;
    }
#endif

    if (aAttr == 0) {
        cout << "\033[0m";
        return 0;
    }

    switch (lBackground) {
    case 0: cout << "\033[40m";  break; // black
    case 1: cout << "\033[44m";  break; // blue
    case 2: cout << "\033[42m";  break; // green
    case 4: cout << "\033[41m";  break; // red
    case 7: cout << "\033[47m";  break; // gray
    case 8: cout << "\033[43m";  break; // yellow
    default:
            cout << "\033[40m";  break; // black
    }

    switch (lForeground) 
    {
    case 0: cout << "\033[30m";  break; // black
    case 1: cout << "\033[34m";  break; // blue
    case 2: cout << "\033[32m";  break; // green
    case 4: cout << "\033[31m";  break; // red
    case 7: cout << "\033[37m";  break; // white
    default:
            cout << "\033[37m";  break; // white
    }

    return lOldAttr;
}

//------------------------------------------------------------------------------
// SfCTable::endline
//------------------------------------------------------------------------------
void SfCTable::endline( const EColor aColor ) {
    TString             lOutput = mOutput.str();
    TString::iterator   lIt;
    SAP_UINT            lTxtAttr;

    bool                lHandleColour = true;

    if (spOut != NULL) {
        fprintf(spOut, "\n%s", mOutput.str().c_str());
    }

#ifdef SAPwithPASE400
        lHandleColour = isatty(0) ? true : false;
#endif

    if ((aColor == COLOR_CHAR) && lHandleColour ) {
        setAttr(0);
        cout << endl;
        
        for (lIt  = lOutput.begin(); lIt != lOutput.end(); lIt ++) {
            if (*lIt == '*') {
                cout << "\033[34m";
            }
            else if (*lIt == '#') {
                cout << "\033[37m";
            }
            /*SAPUNICODEOK_CHARTYPE*/
            cout << char(*lIt);
        }
    }
    else {
        setAttr(0);
        cout << endl;
        setColor(aColor);
        cout << mOutput.str();
    }
    cout.flush();
    mOutput.str(cU(""));
}

#endif

