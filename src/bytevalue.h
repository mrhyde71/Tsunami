#ifndef CBYTEVALUE_H
#define CBYTEVALUE_H

#include <string>      // needed for std::string
#include <type_traits> // needed for std::enable_if

// #include <sstream>     // needed for std::ostringstream


template<size_t base, size_t exponent>
struct CompilePow
{
    enum : uint64_t { value = base*CompilePow<base, exponent - 1>::value };
};

template<> struct CompilePow<2, 1>
{
    enum { value = 2 };
};

template<>
struct CompilePow<2, 0>
{
    enum { value = 1 };
};



template<> struct CompilePow<10, 1>
{
    enum { value = 10 };
};

template<>
struct CompilePow<10, 0>
{
    enum { value = 1 };
};


static constexpr uint64_t A_TERABYTE{ CompilePow<10, 12>::value };
static constexpr uint64_t A_GIGABYTE{ CompilePow<10,  9>::value };
static constexpr uint64_t A_MEGABYTE{ CompilePow<10,  6>::value };
static constexpr uint64_t A_KILOBYTE{ CompilePow<10,  3>::value };
static constexpr uint64_t A_BYTE{ 1 };

constexpr uint64_t operator"" _p2(uint64_t v)
{
    return (static_cast<uint64_t>(2) << (v-1));
}


static constexpr uint64_t A_TEBIBYTE{ 40_p2 }; // 0x10000000000 = 2^40 = 1099511627776
static constexpr uint64_t A_GIBIBYTE{ 30_p2 }; // 0x40000000    = 2^30 = 1073741824
static constexpr uint64_t A_MEBIBYTE{ 20_p2 }; // 0x100000      = 2^20 = 1048576
static constexpr uint64_t A_KIBIBYTE{ 10_p2 }; // 0x400         = 2^10 = 1024


constexpr uint64_t operator"" _B(uint64_t v) { return v; }


constexpr uint64_t operator"" _KiB(uint64_t v) { return (A_KIBIBYTE * v); }
constexpr uint64_t operator"" _MiB(uint64_t v) { return (A_MEBIBYTE * v); }
constexpr uint64_t operator"" _GiB(uint64_t v) { return (A_GIBIBYTE * v); }
constexpr uint64_t operator"" _TiB(uint64_t v) { return (A_TEBIBYTE * v); }


constexpr uint64_t operator"" _kB(uint64_t v) { return (A_KILOBYTE * v); }
constexpr uint64_t operator"" _MB(uint64_t v) { return (A_MEGABYTE * v); }
constexpr uint64_t operator"" _GB(uint64_t v) { return (A_GIGABYTE * v); }
constexpr uint64_t operator"" _TB(uint64_t v) { return (A_TERABYTE * v); }


static_assert(1_KiB == A_KIBIBYTE, "KiB failed");
static_assert(1_MiB == A_MEBIBYTE, "MiB failed");
static_assert(1_GiB == A_GIBIBYTE, "GiB failed");
static_assert(1_TiB == A_TEBIBYTE, "TiB failed");


static_assert(1_kB == A_KILOBYTE, "kB failed");
static_assert(1_MB == A_MEGABYTE, "MB failed");
static_assert(1_GB == A_GIGABYTE, "GB failed");
static_assert(1_TB == A_TERABYTE, "TB failed");


template <class Tstring = std::string>
class CByteValue
{
public:
    using numeric_t = uint64_t;
    using string_t = Tstring;

private:
    numeric_t m_value;

public:
    enum class decimalUnit_t : uint64_t
    {
        TB = 1_TB,
        GB = 1_GB,
        MB = 1_MB,
        kB = 1_kB,
        B = 1_B
    };


    enum class binaryUnit_t : uint64_t
    {
        TiB = 1_TiB,
        GiB = 1_GiB,
        MiB = 1_MiB,
        KiB = 1_KiB,
        B = 1_B
    };

public:

    explicit CByteValue(const numeric_t& v = 0) : m_value(v)
    {
    }

    virtual ~CByteValue();

    static decimalUnit_t highestDecimalUnit(const numeric_t& value_in_bytes)
    {
        if (value_in_bytes >= 1_TB)
        {
            return decimalUnit_t::TB;
        }
        if (value_in_bytes >= 1_GB)
        {
            return decimalUnit_t::GB;
        }
        if (value_in_bytes >= 1_MB)
        {
            return decimalUnit_t::MB;
        }
        if (value_in_bytes >= 1_kB)
        {
            return decimalUnit_t::kB;
        }
        return decimalUnit_t::B;
    }

    static binaryUnit_t highestBinaryUnit(const numeric_t& value_in_bytes)
    {
        if (value_in_bytes >= 1_TiB)
        {
            return binaryUnit_t::TiB;
        }
        if (value_in_bytes >= 1_GiB)
        {
            return binaryUnit_t::GiB;
        }
        if (value_in_bytes >= 1_MiB)
        {
            return binaryUnit_t::MiB;
        }
        if (value_in_bytes >= 1_KiB)
        {
            return binaryUnit_t::KiB;
        }
        return binaryUnit_t::B;
    }

    static const string_t& decimalUnitLabel(decimalUnit_t dut);
    /*
    {
        static const std::map<decimalUnit_t, string_t> labels{
            {decimalUnit_t::B, string_t("B")},
            {decimalUnit_t::kB, string_t("kB")},
            {decimalUnit_t::MB, string_t("MB")},
            {decimalUnit_t::GB, string_t("GB")},
            {decimalUnit_t::TB, string_t("TB")},
        };

        return labels.at(dut);
    }
    */

    static const string_t& binaryUnitLabel(binaryUnit_t dut);
    /*
    {
        static const std::map<binaryUnit_t, string_t> labels{
            {binaryUnit_t::B, string_t("B")},
            {binaryUnit_t::KiB, string_t("KiB")},
            {binaryUnit_t::MiB, string_t("MiB")},
            {binaryUnit_t::GiB, string_t("GiB")},
            {binaryUnit_t::TiB, string_t("TiB")},
        };

        return labels.at(dut);
    }
    */

    static double to_decimalValue(decimalUnit_t du, const numeric_t& value)
    {
        if (du == decimalUnit_t::B)
        {
            return (double)value;
        }
        return (static_cast<double>(value) / static_cast<double>(du));
    }

    static double to_binaryValue(binaryUnit_t bu, const numeric_t& value)
    {
        if (bu == binaryUnit_t::B)
        {
            return (double)value;
        }
        return (static_cast<double>(value) / static_cast<double>(bu));
    }

    const numeric_t& to_bytes() const
    {
        return m_value;
    }


    double to_decimalValue(decimalUnit_t du) const
    {
        return CByteValue::to_decimalValue(du, m_value);
    }

    double to_binaryValue(binaryUnit_t bu) const
    {
        return CByteValue::to_binaryValue(bu, m_value);
    }


    double to_kilobyte() const
    {
        return to_decimalValue(decimalUnit_t::kB);
    }

    double to_megabyte() const
    {
        return to_decimalValue(decimalUnit_t::MB);
    }

    double to_gigabyte() const
    {
        return to_decimalValue(decimalUnit_t::GB);
    }

    double to_terabyte() const
    {
        return to_decimalValue(decimalUnit_t::TB);
    }

    double to_kibibyte() const
    {
        return to_binaryValue(binaryUnit_t::KiB);
    }

    double to_mebibyte() const
    {
        return to_binaryValue(binaryUnit_t::MiB);
    }

    double to_gibibyte() const
    {
        return to_binaryValue(binaryUnit_t::GiB);
    }

    double to_tebibyte() const
    {
        return to_binaryValue(binaryUnit_t::TiB);
    }

/*
    static string_t toDecimalMetricString(const numeric_t& v)
    {
        // this is used to obtain a string representing the nearest multiple in SI (International System of Units)
        std::ostringstream o;

        decimalUnit_t ut = CByteValue::highestDecimalUnit(v);

        o << static_cast<uint64_t>(CByteValue::to_decimalValue(ut, v)) << " " << CByteValue::decimalUnitLabel(ut);
        return o.str();
    }


    static string_t toBinaryMetricString(const numeric_t& v)
    {
        std::ostringstream o;

        binaryUnit_t ut = CByteValue::highestBinaryUnit(v);

        o << static_cast<uint64_t>(CByteValue::to_binaryValue(ut, v)) << " " << CByteValue::binaryUnitLabel(ut);
        return o.str();
    }
*/

    operator const numeric_t& () const
    {
        return to_bytes();
    }


};

#endif // CBYTEVALUE_H
