// -----------------------------------------------------------------
//
// Author: Albert Zedlitz
// File  : AsciiTable.hpp
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

#include <sstream>
#include <string>
#include <deque>

using namespace std;
#define SAPonNT
#define MAX(a,b) (a) > (b) ? (a) : (b)
#define MIN(a,b) (a) > (b) ? (b) : (a)


typedef	ostringstream		    TStringStream;
typedef string			        TString;
typedef deque<TString>          TStringList;

typedef TString::iterator		TStringIt;
typedef TString::const_iterator	TStringCoIt;
typedef	TString::size_type		TStringSize;
typedef	TString::value_type		TStringCh;
typedef char                    SAP_UC;
typedef long long               SAP_LLONG;
typedef unsigned long long      SAP_ULLONG;
typedef double                  SAP_DOUBLE;
typedef unsigned int            SAP_UINT;
typedef int                     SAP_INT;
typedef const SAP_UINT          SAP_CUINT;

#include "sfntat.hpp"
#include "sfuxat.hpp"
