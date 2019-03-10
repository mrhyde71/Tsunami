#include "bytevalue.h"

#include <map>      // needed for std::map
#include <cmath>    // needed for std::modf
#include <sstream>  // needed for std::ostringstream
#include <iomanip>  // needed for stream manip

CByteValue::~CByteValue()
{
}

// static
CByteValue::decimalUnit_t CByteValue::rank_decimalUnit(const bytevalue_t& value_in_bytes)
{
    if (value_in_bytes >= 1_TB)
    {
        return CByteValue::decimalUnit_t::TB;
    }
    if (value_in_bytes >= 1_GB)
    {
        return CByteValue::decimalUnit_t::GB;
    }
    if (value_in_bytes >= 1_MB)
    {
        return CByteValue::decimalUnit_t::MB;
    }
    if (value_in_bytes >= 1_kB)
    {
        return CByteValue::decimalUnit_t::kB;
    }
    return CByteValue::decimalUnit_t::B;
}

// static
CByteValue::binaryUnit_t CByteValue::rank_binaryUnit(const bytevalue_t& value_in_bytes)
{
    if (value_in_bytes >= 1_TiB)
    {
        return CByteValue::binaryUnit_t::TiB;
    }
    if (value_in_bytes >= 1_GiB)
    {
        return CByteValue::binaryUnit_t::GiB;
    }
    if (value_in_bytes >= 1_MiB)
    {
        return CByteValue::binaryUnit_t::MiB;
    }
    if (value_in_bytes >= 1_KiB)
    {
        return CByteValue::binaryUnit_t::KiB;
    }
    return CByteValue::binaryUnit_t::B;
}


void convertToRankValueAndGetStrings_decimal(const CByteValue::bytevalue_t v, QString& got_rank_value, QString& got_rank_label)
{
    double numeric_rank_value {};
    CByteValue::getValueAndRank_decimal<QString>(v, numeric_rank_value, got_rank_label);

    double fract_part = std::modf(numeric_rank_value, &fract_part);
    if (fract_part != 0.0)
    {
        got_rank_value = QString().setNum(numeric_rank_value,'f', 1);
    }
    else
    {
        got_rank_value = QString("%0").arg(static_cast<int>(numeric_rank_value));
    }
}

void convertToRankValueAndGetStrings_binary(const CByteValue::bytevalue_t v, QString& got_rank_value, QString& got_rank_label)
{
    double numeric_rank_value {};
    CByteValue::getValueAndRank_binary<QString>(v, numeric_rank_value, got_rank_label);

    double fract_part = std::modf(numeric_rank_value, &fract_part);
    if (fract_part != 0.0)
    {
        got_rank_value = QString().setNum(numeric_rank_value,'f', 1);
    }
    else
    {
        got_rank_value = QString("%0").arg(static_cast<int>(numeric_rank_value));
    }
}


void convertToRankValueAndGetStrings_decimal(const CByteValue::bytevalue_t v, std::string& got_rank_value, std::string& got_rank_label)
{
    double numeric_rank_value {};
    CByteValue::getValueAndRank_decimal<std::string>(v, numeric_rank_value, got_rank_label);

    std::ostringstream o;
    o << std::setw(4) << std::setprecision(3) << numeric_rank_value;
    got_rank_value = o.str();
}


void convertToRankValueAndGetStrings_binary(const CByteValue::bytevalue_t v, std::string& got_rank_value, std::string& got_rank_label)
{
    double numeric_rank_value {};
    CByteValue::getValueAndRank_binary<std::string>(v, numeric_rank_value, got_rank_label);

    std::ostringstream o;
    o << std::setw(4) << std::setprecision(3) << numeric_rank_value;
    got_rank_value = o.str();
}


void convertToRankValueAndGetStrings_binary(const CByteValue::bytevalue_t v, std::wstring& got_rank_value, std::wstring& got_rank_label)
{
    double numeric_rank_value {};
    CByteValue::getValueAndRank_binary<std::wstring>(v, numeric_rank_value, got_rank_label);

    std::wostringstream o;
    o << std::setw(4) << std::setprecision(3) << numeric_rank_value;
    got_rank_value = o.str();
}
