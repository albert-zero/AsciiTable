// -----------------------------------------------------------------
//
// Author: Albert Zedlitz
// File  : sfxxat.hpp
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
#pragma once

#ifdef SAPonNT
#include <Windows.h>
#endif

#include <iostream>
#include <deque>
#include <iomanip>

//==============================================================================
/// @class SfCTable
/// This class provides a table definition. It allows the user to add columns,
/// rows and output attributes. The table class provides a formatted output.
//==============================================================================
class SfCTable
{
    friend SfCTable& hdr(SfCTable&    arTable);

private:
    static FILE*        spOut;           ///< File descriptor for redirection
    static SAP_CUINT    sMaxRowWidth;    ///< Maximum row with
    static SAP_CUINT    sMaxColWidth;    ///< Maximum column width
    static SAP_CUINT    sMinColWidth;    ///< Minimum column width
    static SAP_CUINT    sMaxColCount;    ///< Maximum column count
    static SAP_UINT     sPageLength;     ///< Maximum table length
    static SAP_INT      sTableLength;    ///< Maximum rows number in print
    static bool         sAlarmed;        ///< SIGALRM caught?
public:

    static TString      sSepStr;         ///< Dynamic column separator char

#   define MIN_COL_WIDTH    3
#   define MAX_COL_WIDTH    2048
#   define MAX_COL_COUNT    32
#   define MAX_ROW_WIDTH    4096
#   define MAX_ROW_COUNT    10000

    //--------------------------------------------------------------------------
    /// @enum EColor
    /// Color definition for well defined text-background configurations.
    //--------------------------------------------------------------------------
    typedef enum EColor {
        COLOR_NONE,
        COLOR_INVERSE,         ///< PD: bright fg, dark bg
        COLOR_TOGGLE,          ///< PD: toggle bright and dark
        COLOR_TOGGLE_DARK,     ///< PD: toggle bright and dark
        COLOR_SAME,            ///< PD: no toggle this time
        COLOR_BRIGHT,          ///< PD: bright bg
        COLOR_DARK,            ///< PD: dark bg
        COLOR_CHAR,            ///< PD: different color for '#' and '*'
        COLOR_GREEN,           ///< green background
        COLOR_RED,             ///< red background
        COLOR_BLUE,            ///< blue background
        COLOR_WHITE            ///< white background
    } EColor;

    //--------------------------------------------------------------------------
    /// @enum EPrintOption 
    /// Options to print only specific parts of the table.
    //--------------------------------------------------------------------------
    typedef enum EPrintOption {
        PRINT_ALL,             ///< print entire table
        PRINT_HEADER,          ///< print header only
        PRINT_BODY             ///< print body without header
    } EPrintOption;

    //--------------------------------------------------------------------------
    /// @enum EFlags
    /// Flags for row and column layout.
    //--------------------------------------------------------------------------
    typedef enum EFlags {
        NUMBER,                ///< INTEGER data (right aligned)
        UNSIGNED,              ///< UNSIGNED data (right aligned)
        HEX,                   ///< Display numeric data hexadecimal
        BOOL,                  ///< Boolean data (true / false)
        YESNO,                 ///< Boolean data (yes / no)
        FIXED,                 ///< Cell width is fixed
        MERGE,                 ///< Cell merges to the right
        RIGHT,                 ///< Cell alignment right
        LEFT,                  ///< Cell alignment left
        R_LINE,                ///< Draw a line
        R_HEADER,              ///< Cell with header layout
        R_INVISIBLE,           ///< Row invisible
        R_SUBTABLE,            ///< Row starts a subtable
        R_PROMPT,              ///< Row prompt and read input
        R_ACKNOWLEDGE,         ///< Row prompt and read acknowledge
        R_MESSAGE              ///< Row with result, warning or error
    } EFlags;

    //==========================================================================
    /// @class TAttr
    /// The attribute class manages attributes for cells, rows, and tables.
    //==========================================================================
    class TAttr {
        friend class    SfCTable;

    public:
        SAP_UINT      mFlags;        ///< cell and row layout flags
        EColor        mColor;        ///< color
        TStringSize   mWidth;        ///< cell or column width

        //----------------------------------------------------------------------
        /// Default constructor.
        //----------------------------------------------------------------------
        TAttr() :
             mFlags(0),              ///< default flags
             mColor(COLOR_NONE),     ///< default color
             mWidth(MIN_COL_WIDTH) { ///< default width
        }

        //----------------------------------------------------------------------
        /// Constructor with arguments to reset default vaules
        //----------------------------------------------------------------------
        TAttr(
            SAP_UINT    aFlags,                     ///< layout flag bits
            EColor      aColor = COLOR_NONE,        ///< column entry color
            TStringSize aWidth = MIN_COL_WIDTH ) :  ///< column entry width
              mFlags(aFlags),
              mColor(aColor),
              mWidth(aWidth) {
        }

        //----------------------------------------------------------------------
        /// Copy constructor.
        //----------------------------------------------------------------------
        TAttr( const TAttr& arAttr ):               ///< the attributes to copy.
              mFlags(arAttr.mFlags),
              mColor(arAttr.mColor),
              mWidth(arAttr.mWidth) {
        }

        //----------------------------------------------------------------------
        /// Assignment operator.
        //----------------------------------------------------------------------
        TAttr& operator = ( const TAttr& arAttr ) { ///< the attributes to copy.
            mFlags = arAttr.mFlags;
            mColor = arAttr.mColor;
            mWidth = arAttr.mWidth;
            return *this;
        }

        //----------------------------------------------------------------------
        /// Sets a flag.
        //----------------------------------------------------------------------
        void set( const EFlags aFlag ) {            ///< the flag to set.
            mFlags |= (1 << aFlag);
        }

        //----------------------------------------------------------------------
        /// Clears a flag.
        //----------------------------------------------------------------------
        void reset( const EFlags& aFlag ) {         ///< the flag to reset.
            mFlags &= ~(1 << aFlag);
        }

        //----------------------------------------------------------------------
        /// Tests whether a flag is set.
        //----------------------------------------------------------------------
        bool test( const EFlags aFlag ) const {             ///< the flag to test.
            return (mFlags & (1 << aFlag)) != 0;
        }

        //----------------------------------------------------------------------
        /// ADD (merge) operator for layout flags.
        //----------------------------------------------------------------------
        TAttr operator + ( const EFlags aFlag ) const {     ///< the new flag.
            TAttr lAttr = *this;

            lAttr.mFlags |= (1 << aFlag);
            return lAttr;
        }

        //----------------------------------------------------------------------
        /// ADD (merge) operator for colors.
        //----------------------------------------------------------------------
        TAttr operator + ( const EColor aColor ) const {    ///< the new color value.
            TAttr    lAttr = *this;

            lAttr.mColor = aColor;
            return lAttr;
        }

        //----------------------------------------------------------------------
        /// ADD (merge) operator for cell width.
        //----------------------------------------------------------------------
        TAttr operator + ( const TStringSize aWidth ) const {   ///< the new cell width.
            TAttr    lAttr = *this;

            lAttr.mWidth = aWidth;
            return lAttr;
        }

        //----------------------------------------------------------------------
        /// SUB (clear) operator for layout flags.
        //----------------------------------------------------------------------
        TAttr operator - ( const EFlags aFlag ) const {       ///< the new layout flag value.
            TAttr    lAttr = *this;

            lAttr.mFlags &= ~(1 << aFlag);
            return lAttr;
        }

        //----------------------------------------------------------------------
        /// assignment-ADD (merge) operator for layout flags.
        //----------------------------------------------------------------------
        TAttr& operator += ( const EFlags aFlag ) {         ///< the new layout flag value.
            this->mFlags |= (1 << aFlag);
            return *this;
        }

        //----------------------------------------------------------------------
        /// assignment-SUB (clear) operator for layout flags.
        //----------------------------------------------------------------------
        TAttr& operator -= ( const EFlags aFlag ) {       ///< the new layout flag value.
            this->mFlags &= ~(1 << aFlag);
            return *this;
        }

        //----------------------------------------------------------------------
        /// assignment-OR (merge) operator for layout flags.
        //----------------------------------------------------------------------
        TAttr& operator |= ( const SAP_UINT aFlags ) {    ///< the layout flags to merge.
            this->mFlags |= aFlags;
            return *this;
        }

        //----------------------------------------------------------------------
        /// assignment-ADD (merge) operator for attributes.
        //----------------------------------------------------------------------
        TAttr& operator += ( const TAttr&  arAttr) {    ///< the new attributes.
            this->mFlags |= arAttr.mFlags;

            if (arAttr.mColor != COLOR_NONE) {
                this->mColor = arAttr.mColor;
            }
            
            if (arAttr.mWidth != MIN_COL_WIDTH) {
                this->mWidth = arAttr.mWidth;
            }
            return *this;
        }
    };

private:
    //==========================================================================
    /// @class TCell
    /// The cell class manages the cells of a table. It provides the cell
    /// width and type, which allows to compute the maximum width for formatted
    /// output.
    //==========================================================================
    class TCell {
        friend class    SfCTable;

    private:
        TString        mValue;      ///< cell value
        TAttr          mCellAttr;   ///< cell attributes
        TString        mRemain;     ///< remainder for line break

    public:
        //----------------------------------------------------------------------
        /// Default constructor.
        //----------------------------------------------------------------------
        TCell(): 
            mValue(""),
            mCellAttr(TAttr()) {
        }

        //----------------------------------------------------------------------
        /// Copy constructor.
        //----------------------------------------------------------------------
        TCell( const TCell& arCell ):        ///< the cell to copy.
            mValue(arCell.mValue),
            mCellAttr(arCell.mCellAttr) {
        }

        //----------------------------------------------------------------------
        //----------------------------------------------------------------------
        TCell(
            const TString&  arValue,
            const TAttr&    arAttr ): 
            mValue(arValue),
            mCellAttr(arAttr) {
        }

        //----------------------------------------------------------------------
        //----------------------------------------------------------------------
        TCell(
            const SAP_UC*   apValue,
            const TAttr&    arAttr ):
            mValue(apValue),
            mCellAttr(arAttr) {
        }

        //----------------------------------------------------------------------
        /// Assignment operator.
        //----------------------------------------------------------------------
        TCell& operator=( const TCell& arCell ) {        ///< the cell to copy.
            mValue = arCell.mValue;
            mCellAttr = arCell.mCellAttr;
            return *this;
        }

        //----------------------------------------------------------------------
        /// Get the cell's value as text.
        /// The result can be
        ///  -    the entire value (if the value fits in the space provided in the
        ///    cell's attributes), or
        ///  -    a part of the value (if the value contains newline characters or
        ///    if it doesn't fit into the available space).
        //----------------------------------------------------------------------
        TString getValue(
                const bool  aContinue,      ///< Continuation call
                bool&        arRemain) {    ///< tells whether some data remains.

            TString        lValue;
            TStringSize    lOff = 0;
            TStringSize    lPos;

            if (!aContinue) {
                mRemain = mValue;
            }

            lPos = mRemain.find_first_of("\n");
            if (lPos == TString::npos) {
                lPos = mRemain.size();
            }
            else if ((lPos > 0) && (mRemain.at(lPos - 1) == ('\r'))) {
                lPos--;
                lOff = 2;
            }
            else {
                lOff = 1;
            }

            if (mCellAttr.test(FIXED) && (mCellAttr.mWidth < lPos)) {
                lPos = mCellAttr.mWidth;
            }

            lValue = mRemain.substr(0, lPos);
            mRemain = mRemain.substr(lPos + lOff);

            arRemain = (mRemain.size() > 0);
            return lValue;
        }
    };

    //==========================================================================
    /// @typedef TCells
    /// Container for the cells of a row.
    //==========================================================================
    typedef deque<TCell> TCells;

    //==========================================================================
    /// @class TRow
    /// The row class stores the cells of each row.
    /// A row can contain header cells or data cells, and it may contain input
    /// from the terminal.
    //==========================================================================
    class TRow {
        friend class    SfCTable;

    private:
        SfCTable*    mpSubTable;    ///< Subtable
        TRow*        mpHeader;      ///< Header row
        TCells       mCells;        ///< Cells within the row
        TAttr        mRowAttr;      ///< Row layout flags
        bool         mContinue;     ///< Linebreak in a column

    public:
        //----------------------------------------------------------------------
        /// Default constructor.
        //----------------------------------------------------------------------
        TRow() :
            mpSubTable(NULL),
            mpHeader(NULL),
            mContinue(false) {
        }

        //----------------------------------------------------------------------
        /// Destructor.
        //----------------------------------------------------------------------
        ~TRow() {
            if (mpSubTable) {
                delete mpSubTable;
            }
        }
    };

    //==========================================================================
    /// Container for the rows of a terminal table.
    /// The first row in mRows is the header row, other rows contain data or
    /// act as placeholders for sub-tables.
    //==========================================================================
    typedef deque<TRow>    TRows;

    TRows           mRows;          ///< Data rows
    TRow*           mpPrompt;       ///< Prompt row for interactive rows
    TRow*           mpHeader;       ///< Header row

    EPrintOption    mPrintOption;   ///< Defines which part of the table to print
    TStringStream   mOutput;        ///< Formatted output line
    SAP_INT         mIndent;        ///< Indentation

    TString         mHeadLine;      ///< Table header
    TString         mInput;         ///< Input string for interactive rows
    SAP_UINT        mOldAttr;       ///< Old console color and font
    static SAP_UINT sCurAttr;       ///< Actual console color and font
    TAttr           mAttr;          ///< Table attributes
    TAttr           mOpAttr;        ///< Output operator attributes

public:
    //--------------------------------------------------------------------------
    /// Constructor.
    //--------------------------------------------------------------------------
    SfCTable(
        const TString&  arHeadLine = "",      ///< Header line
        const TAttr&    arAttr = TAttr() ):   ///< Attributes        
        mRows(1),
        mpPrompt(NULL),
        mpHeader(NULL),
        mPrintOption(PRINT_ALL),
        mIndent(1),
        mHeadLine(arHeadLine),
        mOldAttr(),
        mAttr(arAttr) {

        TRow& lrHeader = mRows.back();

        lrHeader.mRowAttr.set(R_HEADER);
        lrHeader.mRowAttr.set(R_INVISIBLE);

        mpHeader = &lrHeader;
    }

    //--------------------------------------------------------------------------
    /// Destructor.
    //--------------------------------------------------------------------------
    ~SfCTable() {
    }

private:
    void addCell(TCell&  arCell) {        ///< the new cell
        TRow&   lrRow = mRows.back();

        //----------------------------------------------------------------------
        // Check the row length.
        //----------------------------------------------------------------------
        if (lrRow.mCells.size() >= sMaxColCount) {
            cerr << "SfCTable::operator<<(%s): too many cells.\n", arCell.mValue.c_str();
            return;
        }

        if (mpHeader == &lrRow) {
            //------------------------------------------------------------------
            // Output goes into the header row.
            //------------------------------------------------------------------
            if (arCell.mCellAttr.test(FIXED) != 0) {
                arCell.mCellAttr.mWidth = MAX(arCell.mCellAttr.mWidth, MAX(sMinColWidth, MIN(arCell.mValue.size(), sMaxColWidth)));
            }
        }
        else {
            //------------------------------------------------------------------
            // Output is table data.
            //------------------------------------------------------------------
            size_t    iCell = lrRow.mCells.size();

            if (mpHeader->mCells.size() <= iCell) {
                mpHeader->mCells.resize(iCell + 1);
            }

            TCell&    lrHdr = mpHeader->mCells[iCell];

            //------------------------------------------------------------------
            // Merge cell flags and header flags.
            //------------------------------------------------------------------
            arCell.mCellAttr.mFlags |= lrHdr.mCellAttr.mFlags;

            //------------------------------------------------------------------
            // Convert boolean cell values.
            //------------------------------------------------------------------
            if (arCell.mCellAttr.test(BOOL)) {
                arCell.mValue = (arCell.mValue != "0") ? "true" : "false";
            }
            else if (arCell.mCellAttr.test(YESNO)) {
                arCell.mValue = (arCell.mValue != "0") ? "yes" : "no";
            }

            //------------------------------------------------------------------
            // Set the cell width.
            //------------------------------------------------------------------
            if (lrHdr.mCellAttr.test(FIXED)) {
                arCell.mCellAttr.mWidth = lrHdr.mCellAttr.mWidth;
            }
            else {
                //--------------------------------------------------------------
                // The cell width is the maximum of:
                // - the cell attribute width,
                // - the header attribute width, and
                // - the width of the data in the cell (taking line breaks into 
                //   account).
                //--------------------------------------------------------------
                TStringSize lSize = 0;
                TStringSize lPos  = 0;
                TStringSize lOff  = 0;

                while ((lPos = arCell.mValue.find('\n', lOff)) != TString::npos) {
                    if (lSize < (lPos - lOff)) {
                        lSize = (lPos - lOff);
                    }
                    lOff = lPos + 1;
                }

                if (lSize == 0) {
                    lSize = arCell.mValue.length();
                }

                TStringSize lWidth = MIN(sMaxColWidth, MAX(MAX(lSize, arCell.mCellAttr.mWidth), lrHdr.mCellAttr.mWidth));

                arCell.mCellAttr.mWidth = lWidth;

                if ((lrHdr.mCellAttr.mWidth < lWidth) && !arCell.mCellAttr.test(MERGE)) {
                    lrHdr.mCellAttr.mWidth = lWidth;
                }
            }

        }

        lrRow.mCells.push_back(arCell);
    }

public:
    //--------------------------------------------------------------------------
    /// Sets or clears the SIGALRM flag.
    /// @retval    true    if some data was read.
    ///    @retval    false    the timeout has expired.
    //--------------------------------------------------------------------------
    static void setAlarm( bool aValue ) {        ///< new \c sAlarmed flag value.
        sAlarmed = aValue;
    }

    //--------------------------------------------------------------------------
    /// Reads input from the terminal
    /// @retval    true    if some data was read.
    ///    @retval    false    the timeout has expired.
    //--------------------------------------------------------------------------
    bool getInput(
            TString&         arInput,           ///< the data read.
            const SAP_INT    aTimeout = -1) {   ///< input timeout (seconds)
        
        SAP_UC        lA7Buffer[256];
        SAP_INT       lErrno = 0;
        SAP_INT       lRc = 0;

        memset(lA7Buffer, 0, sizeof(lA7Buffer));
        errno = 0;

        cin >> arInput;
        return 0;
    }

    //--------------------------------------------------------------------------
    /// SfCTable::setHeader
    /// Set the header row as reference.
    //--------------------------------------------------------------------------
    void setHeader( SfCTable*    apRef ) {        ///< Reference table
        mpHeader = &apRef->mRows.front();
    }

    //--------------------------------------------------------------------------
    /// SfCTable::redirect
    /// Redirect output of the table into a given file
    //--------------------------------------------------------------------------
    static void redirect( FILE* apOut ) {        ///< Output file descriptor
        if (spOut) {
            fclose(spOut);
        }
        spOut = apOut;
    }

    //--------------------------------------------------------------------------
    /// SfCTable::setAttr
    /// Sets the colors and attributes for output
    //--------------------------------------------------------------------------
    static SAP_UINT setAttr(const SAP_UINT aAttr);    ///< New colors and attributes

    //--------------------------------------------------------------------------
    /// SfCTable::setTableLength
    /// Set max number of table rows for output
    //--------------------------------------------------------------------------
    inline static void setTableLength( const SAP_INT aTableLength ) {
        sTableLength = aTableLength;
    }

    //--------------------------------------------------------------------------
    /// SfCTable::getTableLength
    /// get max number of table rows for output
    //--------------------------------------------------------------------------
    inline static SAP_INT getTableLength() {
        return sTableLength;
    }

    //--------------------------------------------------------------------------
    /// SfCTable::setPageLength
    /// Set max number of table rows
    //--------------------------------------------------------------------------
    inline static void setPageLength( const SAP_UINT    aPageLength ) {
        sPageLength = aPageLength;
    }

    //--------------------------------------------------------------------------
    /// SfCTable::getPageLength
    /// Get max table length
    //--------------------------------------------------------------------------
    inline static SAP_INT getPageLength() {
        return sPageLength;
    }

    //--------------------------------------------------------------------------
    /// SfCTable::setColor
    /// Sets the color for output
    /// @return The old color attribute
    //--------------------------------------------------------------------------
    SAP_UINT setColor(const EColor aColor);       ///< New color

    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    inline TAttr getOpAttr(void) {
        TAttr        lAttr(mOpAttr);

        mOpAttr = TAttr();
        return lAttr;
    }


private:
    //--------------------------------------------------------------------------
    /// SfCTable::endline
    /// Trigger the output
    //--------------------------------------------------------------------------
    void endline(const SfCTable::EColor aColor);

    //--------------------------------------------------------------------------
    /// SfCTable::printSpace
    /// Print spaces
    //--------------------------------------------------------------------------
    void printSpace(
        const size_t    aWidth,             ///< number of fill characters
        const SAP_UC    aChar = ' ') {      ///< the character to fill with
        mOutput << TString(MIN(aWidth, sMaxRowWidth), aChar);
    }

    //--------------------------------------------------------------------------
    /// SfCTable::printLine
    /// Prints a separator line.
    //--------------------------------------------------------------------------
    void printLine(
        TRow&           arRow,                      ///< Row to print
        const EColor    aColor = COLOR_BRIGHT ) {   ///< Row color
        TCells::iterator lIt;

        for (lIt = arRow.mCells.begin(); lIt != arRow.mCells.end(); lIt++) {
            mOutput << "+" << TString(lIt->mCellAttr.mWidth, '-');
        }

        if (mAttr.mWidth > mOutput.str().size()) {
            mOutput << TString(mAttr.mWidth - mOutput.str().size(), '-');
        }

        mOutput << "+";
        endline(aColor);

        if (mAttr.mColor == COLOR_TOGGLE) {
            mAttr.mColor = COLOR_TOGGLE_DARK;
        }
    }


    //--------------------------------------------------------------------------
    /// SfCTable::printHeader
    /// Prints the header of a table.
    //--------------------------------------------------------------------------
    void printHeader(void) {
        if (mpHeader->mRowAttr.test(R_INVISIBLE)) {
            return;
        }

        printSpace(mIndent);
        printLine(*mpHeader, COLOR_BRIGHT);

        printRow(*mpHeader, COLOR_BRIGHT);

        printSpace(mIndent);
        printLine(*mpHeader, COLOR_BRIGHT);
    }


    //--------------------------------------------------------------------------
    /// SfCTable::printRow
    /// Prints a row of a table
    //--------------------------------------------------------------------------
    void printRow(
        TRow&           arRow,                  ///< Row to print
        const EColor    aColor = COLOR_NONE ) { ///< Row color

        if (arRow.mRowAttr.test(R_LINE)) {
            printSpace(mIndent);
            printLine(*mpHeader);
            return;
        }

        if (arRow.mRowAttr.test(R_INVISIBLE)) {
            return;
        }
        printSpace(mIndent);

        if (arRow.mRowAttr.test(R_PROMPT) ||
            arRow.mRowAttr.test(R_ACKNOWLEDGE)) {
            mpPrompt = &arRow;
            return;
        }

        if (arRow.mRowAttr.test(R_SUBTABLE)) {
            arRow.mpSubTable->print(mPrintOption);
            return;
        }

        TCells::iterator lHdrIt = mpHeader->mCells.begin();
        TCells::iterator lCellIt = arRow.mCells.begin();
        TString          lValue;
        size_t           lFill = 0;
        TAttr            lAttr;
        bool             lRemain = false;
        bool             lHasRes = false;

        mOutput << (arRow.mRowAttr.test(R_MESSAGE) ? " " : sSepStr);

        //----------------------------------------------------------------------
        // Process the cells of the row.
        //----------------------------------------------------------------------
        while (lHdrIt != mpHeader->mCells.end() && lCellIt != arRow.mCells.end()) {
            TCell&    lrCurCell = (*lCellIt);
            TCell&    lrHdrCell = (*lHdrIt);
            size_t    lWidth = lrHdrCell.mCellAttr.mWidth;
            size_t    lSize = 0;

            lAttr = lrCurCell.mCellAttr;

            //------------------------------------------------------------------
            // Fetch the cell contents.
            //------------------------------------------------------------------
            if (lCellIt != arRow.mCells.end()) {
                lValue = lrCurCell.getValue(arRow.mContinue, lHasRes);
                lRemain = lRemain || lHasRes;
                lSize = lValue.size();
            }
            else {
                lValue = "";
            }

            if ((lWidth < lSize) && lrCurCell.mCellAttr.test(MERGE)) {
                //--------------------------------------------------------------
                // Check how many colums the MERGE cell spans.
                //--------------------------------------------------------------
                lHdrIt++;
                while (lHdrIt != mpHeader->mCells.end()) {
                    lWidth += lHdrIt->mCellAttr.mWidth + 1;
                    if (lWidth >= lSize) {
                        break;
                    }
                    lHdrIt++;
                }

                if ((lWidth < lSize) && (lHdrIt == mpHeader->mCells.end())) {
                    //----------------------------------------------------------
                    // Use everything that is lelft...
                    //----------------------------------------------------------
                    lWidth = mAttr.mWidth - mOutput.str().size();
                }
            }

            //------------------------------------------------------------------
            // Compute the fill space and the printable value.
            //------------------------------------------------------------------
            if (lWidth >= lValue.size()) {
                lFill = lWidth - lValue.size();
            }
            else {
                lFill = 0;
                lValue = lValue.substr(0, lWidth - 1) + "*";
            }

            if (lAttr.test(RIGHT) ||
               (lAttr.test(NUMBER) && !arRow.mRowAttr.test(R_HEADER) && !lAttr.test(LEFT))) {
                // right align for numbers
                printSpace(lFill);
                mOutput << lValue;
            }
            else {
                mOutput << lValue;
                printSpace(lFill);
            }

            //------------------------------------------------------------------
            // Increment the iterators.
            //------------------------------------------------------------------
            if (lHdrIt != mpHeader->mCells.end()) {
                lHdrIt++;
            }
            
            if (lCellIt != arRow.mCells.end()) {
                lCellIt++;
            }
            //------------------------------------------------------------------
            // Emit a seprator if there is somethiing left.
            //------------------------------------------------------------------
            if ((lHdrIt != mpHeader->mCells.end()) && (lCellIt != arRow.mCells.end())) {
                mOutput << sSepStr;
            }
        }

        if (mAttr.mWidth > mOutput.str().size()) {
            printSpace(mAttr.mWidth - mOutput.str().size());
        }
        mOutput << (arRow.mRowAttr.test(R_MESSAGE) ? " " : sSepStr);

        endline(aColor);

        if (lRemain) {
            arRow.mContinue = lRemain;
            printRow(arRow, COLOR_SAME);
        }
    }

    //--------------------------------------------------------------------------
    /// Evaluates the width of the table.
    //--------------------------------------------------------------------------
    void evalWidth(void) {
        size_t        lCells = mpHeader->mCells.size();
        TStringSize   lWidth = 1;

        for (size_t lCell = 0; lCell < lCells; lCell++) {
            lWidth += mpHeader->mCells[lCell].mCellAttr.mWidth + 1;
        }

        if (mpHeader->mRowAttr.mWidth < lWidth) {
            mpHeader->mRowAttr.mWidth = lWidth;
        }
    }

public:

    //--------------------------------------------------------------------------
    /// SfCTable::print
    /// Prints table or part of it
    //--------------------------------------------------------------------------
    void print( EPrintOption aOption = PRINT_ALL ) { ///< Print option
        mPrintOption = aOption;

        //----------------------------------------------------------------------
        // Print the table head line.
        //----------------------------------------------------------------------
        if (aOption != PRINT_BODY && mHeadLine != "") {
            printSpace(mIndent);
            mOutput << mHeadLine;
            endline(mAttr.mColor);
        }

        //----------------------------------------------------------------------
        // Print the table header.
        //----------------------------------------------------------------------
        evalWidth();

        TRows::iterator    lRowIt = mRows.begin();
        SAP_UINT    lSkipRows = 0;
        SAP_UINT    lShowRows = mRows.size();
        SAP_UINT    lNumRow = 0;

        if (sTableLength > 0) {
            lShowRows = sTableLength;
        }
        else if ((sTableLength < 0) && (lShowRows > (SAP_UINT) (-sTableLength))) {
            lSkipRows = lShowRows + sTableLength;
        }

        for (; lRowIt != mRows.end() && lShowRows > 0; ++lRowIt) {
            if (lSkipRows > 0) {
                lSkipRows--;
                continue;
            }

            TRow& lrRow = (*lRowIt);

            mOutput.str("");

            if (lrRow.mRowAttr.test(R_HEADER)) {
                if (aOption != PRINT_BODY) {
                    printHeader();
                }
            }
            else {
                if (aOption == PRINT_HEADER) {
                    if (lrRow.mRowAttr.test(R_SUBTABLE)) {
                        lrRow.mpSubTable->print(aOption);
                        continue;
                    }
                    else {
                        break;
                    }
                }
                printRow( lrRow, lrRow.mRowAttr.mColor != COLOR_NONE ? lrRow.mRowAttr.mColor : mAttr.mColor );
                lShowRows--;
            }
        }

        //----------------------------------------------------------------------
        // print a prompt
        //----------------------------------------------------------------------
        if (mpPrompt != NULL && mpPrompt->mCells.size() > 0) {
            if (mpPrompt->mRowAttr.test(R_ACKNOWLEDGE)) {
                mOutput << " " << mpPrompt->mCells.front().mValue << ": ";
                endline(COLOR_GREEN);
            }
            else {
                mOutput << mpPrompt->mCells.front().mValue;
                endline(COLOR_BRIGHT);
            }
        }
    }

    //--------------------------------------------------------------------------
    /// Output manipulator hook operator.
    /// This operator ensures that an expression of the form <code> tbl << row
    /// </code> calls \c row(tbl), and then returns \c *this. 
    //--------------------------------------------------------------------------
    SfCTable& operator << (SfCTable&  (*apFn)(SfCTable&) ) {
        return apFn(*this);
    }

    //--------------------------------------------------------------------------
    /// Output operator for attributes.
    /// This operator changes the output attributed for the next output.
    //--------------------------------------------------------------------------
    SfCTable& operator << ( const SfCTable::TAttr&  arAttr ) {    ///< Cell attributes
        mOpAttr = arAttr;
        return *this;
    }

    //--------------------------------------------------------------------------
    /// Output operator for strings.
    //--------------------------------------------------------------------------
    SfCTable& operator << ( const TString& arValue ) {        ///< Cell value
        TCell   lCell(arValue, getOpAttr());
        addCell(lCell);
        return *this;
    }

    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    SfCTable& operator << ( const SAP_UC* apValue ) {        ///< Cell value
        TCell   lCell(apValue, getOpAttr());
        addCell(lCell);
        return *this;
    }

    //--------------------------------------------------------------------------
    /// Output operators for numeric data.
    //--------------------------------------------------------------------------
    SfCTable& operator << ( const SAP_ULLONG aValue ) {     ///< Cell value
        TAttr           lAttr(getOpAttr());
        TStringStream   lOs;

        TRow&           lrRow = mRows.back();
        size_t          iCell = lrRow.mCells.size();

        if (mpHeader->mCells.size() > iCell) {
            lAttr += mpHeader->mCells[iCell].mCellAttr;
        }

        if (lAttr.test(HEX)) {
            lOs << "0x" << setbase(ios_base::hex) << (SAP_UINT)(aValue & 0xFFFFFFFF);
        }
        else if (lAttr.test(UNSIGNED)) {
            lOs << (SAP_ULLONG)aValue;
        }
        else {
            lOs << (SAP_LLONG)aValue;
        }
        lAttr.set(NUMBER);

        TCell  lCell(lOs.str(), lAttr);
        addCell(lCell);
        return *this;
    }

    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    inline SfCTable& operator << ( const SAP_LLONG aValue ) {        ///< Cell value
        return SfCTable::operator << ((SAP_ULLONG)aValue);
    }

    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    inline SfCTable& operator << ( const SAP_UINT aValue ) {        ///< Cell value
        return SfCTable::operator << ((SAP_ULLONG)aValue);
    }

    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    inline SfCTable& operator << ( const SAP_INT aValue ) {        ///< Cell value
        return SfCTable::operator << ((SAP_ULLONG)aValue);
    }

    //--------------------------------------------------------------------------
    /// Output operator for numeric data.
    //--------------------------------------------------------------------------
    SfCTable& operator << ( const SAP_DOUBLE aValue ) {             ///< Cell value
        //TRow&        lrRow = mRows.back();
        size_t  iCell = mRows.back().mCells.size();
        TAttr   lAttr(getOpAttr());

        if (mpHeader->mCells.size() > iCell) {
            lAttr += mpHeader->mCells[iCell].mCellAttr;
        }
        TStringStream lOs;

        lOs << aValue;
        lAttr.set(NUMBER);

        TCell   lCell(lOs.str(), lAttr);

        addCell(lCell);
        return *this;
    }

    //--------------------------------------------------------------------------
    /// Modification operator for field attributes.
    //--------------------------------------------------------------------------
    SfCTable& operator << ( const EFlags& arFlag ) {        ///< the new layout flag value.
        mOpAttr += arFlag;
        return *this;
    }

    //--------------------------------------------------------------------------
    /// Marks the header as visible.
    //--------------------------------------------------------------------------
    SfCTable& hdr( const TAttr&    arAttr = TAttr() ) {    ///< Row attributes
        mpHeader->mRowAttr.reset(SfCTable::R_INVISIBLE);
        mpHeader->mRowAttr += arAttr;
        return *this;
    }

    //--------------------------------------------------------------------------
    /// Appends a data row to the table.
    //--------------------------------------------------------------------------
    SfCTable& row( const TAttr&    arAttr = TAttr() ) {    ///< Row attributes
        //----------------------------------------------------------------------
        // Check for table overflow.
        //----------------------------------------------------------------------
        if (mRows.size() > sPageLength) {
            //------------------------------------------------------------------
            // Print the data that is already available.
            //------------------------------------------------------------------
            print(PRINT_HEADER);
            print(PRINT_BODY);

            //------------------------------------------------------------------
            // Erase the data (non-header, non-subtable) lines.
            //------------------------------------------------------------------
            TRows::iterator lIt;

            for (lIt = mRows.begin() + 1; lIt != mRows.end(); lIt++) {
                if (!lIt->mRowAttr.test(R_HEADER) && !lIt->mRowAttr.test(R_SUBTABLE)) {
                    break;
                }
            }
            mRows.erase(lIt, mRows.end());
        }

        //----------------------------------------------------------------------
        // Add a new row, and set that row's header and attributes.
        //----------------------------------------------------------------------
        TRow  lRow;

        lRow.mpHeader = mpHeader;
        lRow.mRowAttr = arAttr;
        lRow.mRowAttr |= mAttr.mFlags;
        mRows.push_back(lRow);
        return *this;
    }

    //--------------------------------------------------------------------------
    /// Creates a subtable.
    /// The current row acts as a placeholder for the subtable in the superior
    /// table.
    //--------------------------------------------------------------------------
    SfCTable* subTable( const TAttr&  arAttr = TAttr() ) {    ///< Subtable attributes
        row(arAttr);

        TRow&        lrRow = mRows.back();

        lrRow.mRowAttr.set(R_SUBTABLE);
        lrRow.mpSubTable = new SfCTable("", arAttr);
        lrRow.mpSubTable->mIndent = mIndent + 4;
        return lrRow.mpSubTable;
    }
};

//------------------------------------------------------------------------------
/// Static field initialization.
//------------------------------------------------------------------------------
TString      SfCTable::sSepStr = "|";
FILE*        SfCTable::spOut = NULL;
SAP_CUINT    SfCTable::sMinColWidth = MIN_COL_WIDTH;
SAP_CUINT    SfCTable::sMaxColWidth = MAX_COL_WIDTH;
SAP_CUINT    SfCTable::sMaxColCount = MAX_COL_COUNT;
SAP_CUINT    SfCTable::sMaxRowWidth = MAX_ROW_WIDTH;
SAP_UINT     SfCTable::sPageLength  = MAX_ROW_COUNT;
SAP_INT      SfCTable::sTableLength = MAX_ROW_COUNT;
SAP_UINT     SfCTable::sCurAttr = 0;
bool         SfCTable::sAlarmed = false;

//------------------------------------------------------------------------------
/// Output operators for tables.
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
SfCTable& hdr( SfCTable& arTable ) {       ///< Table to write to
    return arTable.hdr(arTable.getOpAttr());
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
SfCTable& row( SfCTable& arTable ) {        ///< Table to write to
    return arTable.row(arTable.getOpAttr());
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
SfCTable& line(SfCTable& arTable ) {       ///< Table to write to
    return arTable.row(arTable.getOpAttr() + SfCTable::R_LINE);
}


