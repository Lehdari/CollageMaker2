//
// Project: CollageMaker2
// File: CsvLog.cpp
//
// Copyright (c) 2019 Miika 'Lehdari' Lehtimäki
// You may use, distribute and modify this code under the terms
// of the licence specified in file LICENCE which is distributed
// with this source code package.
//

#include "CsvLog.hpp"

#include <cstdio>


CsvLog::CsvLog(const std::string& fileName, char delim, int64_t nCols) :
    _fileName   (fileName),
    _delim      (delim),
    _nCols      (nCols)
{
}

CsvLog::CsvLog(CsvLog&& other) :
    _fileName   (other._fileName),
    _delim      (other._delim),
    _nCols      (other._nCols),
    _rows       (other._rows)
{
    other._fileName = std::string("");
    other._nCols = -1;
    other._rows.clear();
}

CsvLog& CsvLog::operator=(CsvLog&& other)
{
    _fileName = other._fileName;
    _delim = other._delim;
    _nCols = other._nCols;
    _rows = other._rows;

    other._fileName = std::string("");
    other._nCols = -1;
    other._rows.clear();

    return *this;

}

CsvLog::~CsvLog()
{
    if (_nCols > 0) {
        FILE* f = fopen(_fileName.c_str(), "w");

        if (f == nullptr) {
            fprintf(stderr, "Unable to open file %s for writing\n", _fileName.c_str());
            return;
        }

        for (auto& row : _rows) {
            for (auto& e : row) {
                fprintf(f, "%s%c", e.c_str(), _delim);
            }
            fprintf(f, "\n");
        }

        fclose(f);
    }
}
