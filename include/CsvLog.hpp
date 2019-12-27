//
// Project: CollageMaker2
// File: CsvLog.hpp
//
// Copyright (c) 2019 Miika 'Lehdari' Lehtimäki
// You may use, distribute and modify this code under the terms
// of the licence specified in file LICENCE which is distributed
// with this source code package.
//

#ifndef COLLAGEMAKER2_CSVLOG_HPP
#define COLLAGEMAKER2_CSVLOG_HPP


#include <string>
#include <vector>
#include <cstdint>
#include <sstream>
#include <cassert>


/*
 * Class CsvLog
 *
 * CsvLog is a helper class for creating csv files.
 * Log file is saved once the CsvLog object goes goes out of scope.
 */
class CsvLog {
public:
    // Construct a CsvLog object
    CsvLog(const std::string& fileName,
        char delim=';',
        int64_t nCols = 1);

    CsvLog(const CsvLog&) = delete;
    CsvLog(CsvLog&&);
    CsvLog& operator=(const CsvLog&) = delete;
    CsvLog& operator=(CsvLog&&);

    // Log is saved on destruction
    ~CsvLog();

    template <typename... T_Cols>
    void addRow(const T_Cols&... cols);

private:
    using Row = std::vector<std::string>;

    std::string         _fileName;
    char                _delim;
    int64_t             _nCols;

    std::vector<Row>    _rows;

    template <typename T>
    inline std::string stringify(const T& e);
};


template <typename... T_Cols>
void CsvLog::addRow(const T_Cols&... cols)
{
    assert(sizeof...(cols) == _nCols);
    Row newRow({stringify(cols)...});
    _rows.push_back(newRow);
}

template<typename T>
std::string CsvLog::stringify(const T& e)
{
    std::stringstream ss;
    ss << e;
    return ss.str();
}


#endif //COLLAGEMAKER2_CSVLOG_HPP
