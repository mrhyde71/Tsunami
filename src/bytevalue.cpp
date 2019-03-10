#include "cbytevalue.h"

#include <map> // needed for std::map

template<>
CByteValue<>::~CByteValue()
{
}

// static
template<>
const CByteValue<>::string_t& CByteValue<>::decimalUnitLabel(CByteValue<>::decimalUnit_t dut)
{
    static const std::map<CByteValue<>::decimalUnit_t, CByteValue<>::string_t> labels{
        {decimalUnit_t::B,  CByteValue<>::string_t("B")},
        {decimalUnit_t::kB, CByteValue<>::string_t("kB")},
        {decimalUnit_t::MB, CByteValue<>::string_t("MB")},
        {decimalUnit_t::GB, CByteValue<>::string_t("GB")},
        {decimalUnit_t::TB, CByteValue<>::string_t("TB")},
    };

    return labels.at(dut);
}

// static
template<>
const CByteValue<>::string_t& CByteValue<>::binaryUnitLabel(CByteValue<>::binaryUnit_t dut)
{
    static const std::map<CByteValue<>::binaryUnit_t, CByteValue<>::string_t> labels{
        {binaryUnit_t::B,   CByteValue<>::string_t("B")},
        {binaryUnit_t::KiB, CByteValue<>::string_t("KiB")},
        {binaryUnit_t::MiB, CByteValue<>::string_t("MiB")},
        {binaryUnit_t::GiB, CByteValue<>::string_t("GiB")},
        {binaryUnit_t::TiB, CByteValue<>::string_t("TiB")},
    };

    return labels.at(dut);
}
