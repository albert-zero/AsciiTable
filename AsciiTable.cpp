// -----------------------------------------------------------------
//
// Author: Albert Zedlitz
// File  : AsciiTable.cpp
// Date  : 11.08.2017
// Abstract:
//    Ascii Table
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
#include "AsciiTable.hpp"
#include <iostream>

// -----------------------------------------------------------------
// Example for a logo with two colors
// The color is defined in COLOR_CHAR, which prints different chars
// in different colors. In this implementation [*:blue, #:white]
// -----------------------------------------------------------------
void showLogo() {
    SfCTable::TAttr	lAttr(0, SfCTable::COLOR_CHAR, 78);
    SfCTable	lTableSap("", lAttr + SfCTable::R_MESSAGE);

    lTableSap << row;
    lTableSap << row << " ******************************************";
    lTableSap << row << " *****************************************";
    lTableSap << row << " ***#####******####*****#######*********";
    lTableSap << row << " **##***##****##**##****##****##*******";
    lTableSap << row << " ***##********##**##****##****##*****";
    lTableSap << row << " ****###*****########***#######*****";
    lTableSap << row << " ******##****##****##***##********";
    lTableSap << row << " **##***##**##******##**##*******";
    lTableSap << row << " ***#####***##******##**##*****";
    lTableSap << row << " *****************************";
    lTableSap << row << " ****************************";
    //lTableSap << row << "";
    lTableSap << row << " valid commands: [example, complex, subtable, exit] ";
    lTableSap << row;
    lTableSap.print();

}

// -----------------------------------------------------------------
// Example for a table: [width: 78, color: toggle for each row]
// -----------------------------------------------------------------
void showSimpleExample() {
    SfCTable::TAttr	aStdAttr;
    aStdAttr.mColor = SfCTable::COLOR_TOGGLE;
    aStdAttr.mWidth = 78;

    SfCTable::TAttr	lAttr(aStdAttr);
    SfCTable        lTable("Table Simple Example", lAttr);

    // Define the header line with three columns
    lTable << hdr << "Company" << "Contact" << "Contry";

    // Add rows to the table
    lTable << row << "Alfreds Futterkiste"          << "Maria Anders"    << "Germany";
    lTable << row << "Centro comercial Moctezuma"   << "Francisco Chang" << "Mexico";
    lTable << row << "Laughing Bacchus Winecellars" << "Yoshi Tannamuri" << "Canada";

    // Print table
    lTable.print();
}

// -----------------------------------------------------------------
// -----------------------------------------------------------------
void showComplexExample() {
    SfCTable::TAttr	aStdAttr;
    aStdAttr.mColor = SfCTable::COLOR_TOGGLE;
    aStdAttr.mWidth = 78;

    SfCTable::TAttr	lAttr(aStdAttr);
    SfCTable        lTable("Table Complex Example", lAttr);

    // Define the header line with three columns
    lTable << hdr << "Description"                  // normal string cell
        << SfCTable::BOOL  << "Bool (true/false)"   // cell converts bool values [true, false]
        << SfCTable::YESNO << "Bool (yes/no)"       // cell converts bool values [yes, no]
        << SfCTable::FIXED << "Number";             // cell width is calculated by header entry

    // Add rows to the table
    lTable << row  << "Row[false]"    << 0 << 0 <<   0;                         // Integer are right bound
    lTable << row  << "Row[true]"     << 1 << 1 << SfCTable::RIGHT << "100";    // String could also be right bound
    lTable << row  << "Row[true/hex]" << 1 << 1 << SfCTable::HEX   <<  100;     // output hex
    lTable << row  << "Row[merge]"    << 0 << 0 << SfCTable::MERGE;             // merge cell to the right
    lTable << row  << SfCTable::R_LINE << "Row[line]";                          // merge all cells to one row


    SfCTable* lpRefTable = lTable.subTable(lAttr);                                 // create a sub table
    *lpRefTable << SfCTable::R_ACKNOWLEDGE << row << "Ask to continue: [yes]/no";  // ask for acknowlegde

    // Print table
    lTable.print();

    TString lAnswer;                // print table and sub table
    lpRefTable->getInput(lAnswer);  // read answer into lAnswer    
}

// -----------------------------------------------------------------
// -----------------------------------------------------------------
void showSubTableExample() {
    SfCTable::TAttr	aStdAttr;
    aStdAttr.mColor = SfCTable::COLOR_TOGGLE;
    aStdAttr.mWidth = 78;

    SfCTable::TAttr	lAttr(aStdAttr);
    SfCTable::TAttr	lStreetAttr;
    lStreetAttr.mWidth = 20;
    lStreetAttr.set(SfCTable::FIXED);

    SfCTable::TAttr	lAttrBlue(1 << SfCTable::FIXED, SfCTable::COLOR_BLUE, 15);

    SfCTable        lTable("Table Simple Example", lAttr);

    // Define the header line with three columns
    lTable << hdr << "Company" << "Contact" << "Contry";
    
    SfCTable* lpRefHeader;

    // Add rows to the table
    // A subtable is inserted at the current row, so that for each row there could be a subtable.
    lTable << row << "Alfreds Futterkiste" << "Maria Anders" << "Germany";
    lpRefHeader = lTable.subTable(lAttr);
    *lpRefHeader << hdr << lStreetAttr << "Street" <<  "Number";
    *lpRefHeader << row << "Neue Strasse" << lAttrBlue << 22;

    lTable << row << "Centro comercial Moctezuma"   << "Francisco Chang" << "Mexico";
    lpRefHeader = lTable.subTable(lAttr);
    *lpRefHeader << hdr << lStreetAttr << "Street" << SfCTable::FIXED << "Number";
    *lpRefHeader << row << "Via Dolores" << 4477;

    lTable << row << "Laughing Bacchus Winecellars" << "Yoshi Tannamuri" << "Canada";
    lpRefHeader = lTable.subTable(lAttr);
    *lpRefHeader << hdr << lStreetAttr << "Street" << SfCTable::FIXED << "Number";
    *lpRefHeader << row << "Main Road" << 2222;

    // Print table
    lTable.print();
}

// -----------------------------------------------------------------
// Example for a command line loop with prompt
// -----------------------------------------------------------------
void mainLoop() {
    TString aCmd = "> ";
    
    for (;;) {
        SfCTable lPrompt;
        lPrompt << SfCTable::R_PROMPT << row << "> ";
        lPrompt.print();

        lPrompt.getInput(aCmd);

        if (aCmd == "exit") {
            break;
        }
        else if (aCmd == "example") {
            showSimpleExample();
        }
        else if (aCmd == "complex") {
            showComplexExample();
        }
        else if (aCmd == "subtable") {
            showSubTableExample();
        }
    }
}


// -----------------------------------------------------------------
// -----------------------------------------------------------------
int main(int argc, char **argv) {

    showLogo();
    mainLoop();
    return 0;
}
