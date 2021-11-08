//
// Copyright © 2021, Alexander Borisov, https://github.com/SashaBorisov/uuid
//

#include <iostream>
#include <iomanip>
#include <sstream>

#include <gtest/gtest.h>
#include <rfc4122/uuid.h>



template<typename L, typename R>
bool bytes_equal(const L& left, const R& right)
{
    if constexpr (sizeof(left) != sizeof(right))
    {
        return false;
    }
    return 0 == std::memcmp(&left, &right, sizeof(left));
}

template<typename O>
O& print_bytes(O& output, const std::byte* bytes, const size_t count)
{
    output << std::hex << std::setfill('0') << std::setw(2);
    for(const std::byte* byte = bytes; byte < bytes + count; ++byte)
    {
        output << (static_cast<uint16_t>(*byte) & 0xFF);
    }
    return output;
}

template<typename O, typename V>
O& print_bytes(O& output, const V& value)
{
    const auto bytes = reinterpret_cast<const std::byte*>(&value);
    return print_bytes(output, bytes, sizeof(V));
}

template<typename V>
std::string bytes_string(const V& value)
{
    std::stringstream printer;
    print_bytes(printer, value);
    return printer.str();
}

template<typename V>
std::string hex_string(const int width, const V& value)
{
    std::stringstream printer;
    printer << std::hex << std::setfill('0') << std::setw(width)
            << value;
    return printer.str();
}

TEST(Internals, byte_order)
{
    using namespace rfc4122::__internal;

    static constexpr byte_order::endian host_other = byte_order::other(byte_order::host);

    {
        const auto abcd = byte_order::bytes_to_host_value(0xAB, 0xCD);
        EXPECT_EQ(0xABCD, abcd);
    }
    {
        const auto cdab = byte_order::bytes_to_value<host_other>(0xAB, 0xCD);
        EXPECT_EQ(0xCDAB, cdab);
    }
    {
        const auto abcdef12 = byte_order::bytes_to_host_value(0xAB, 0xCD, 0xEF, 0x12);
        EXPECT_EQ(0xABCDEF12, abcdef12);
    }
    {
        const auto _12efcdab = byte_order::bytes_from_host_to_value(0xAB, 0xCD, 0xEF, 0x12);
        EXPECT_EQ(0x12EFCDAB, _12efcdab);
    }
    {
        const auto abcdef1234567890 = byte_order::bytes_to_host_value(0xAB, 0xCD, 0xEF, 0x12, 0x34, 0x56, 0x78, 0x90);
        EXPECT_EQ(0xABCDEF1234567890, abcdef1234567890);
    }
    {
        const auto _9078563412efcdab = byte_order::bytes_to_value<host_other>(0xAB, 0xCD, 0xEF, 0x12, 0x34, 0x56, 0x78, 0x90);
        EXPECT_EQ(0x9078563412EFCDAB, _9078563412efcdab);
    }
}

TEST(Parse, literal_1)
{
    constexpr uint8_t expected[] = 
    {
          0xab, 0xcd, 0xef, 0x12
        , 0x34, 0x56
        , 0x78, 0x9a
        , 0xbc, 0xde
        , 0xf1, 0x23, 0x45, 0x67, 0x89, 0xab  
    };

    {
        const char char_literal[] = "abcdef12-3456-789a-bcde-f123456789ab";
        const auto uuid_char = rfc4122::from_literal(char_literal);
        EXPECT_TRUE(bytes_equal(expected, uuid_char));
    }
    {
        const char8_t utf8_literal[] = u8"abcdef12-3456-789a-bcde-f123456789ab";
        const auto uuid_utf8 = rfc4122::from_literal(utf8_literal);
        EXPECT_TRUE(bytes_equal(expected, uuid_utf8));
    }
    {
        const wchar_t wide_literal[] = L"abcdef12-3456-789a-bcde-f123456789ab";
        const auto uuid_wide = rfc4122::from_literal(wide_literal);
        EXPECT_TRUE(bytes_equal(expected, uuid_wide));
    }
    {
        const char16_t utf16_literal[] = u"abcdef12-3456-789a-bcde-f123456789ab";
        const auto uuid_utf16 = rfc4122::from_literal(utf16_literal);
        EXPECT_TRUE(bytes_equal(expected, uuid_utf16));
    }
    {
        const char32_t utf32_literal[] = U"abcdef12-3456-789a-bcde-f123456789ab";
        const auto uuid_utf32 = rfc4122::from_literal(utf32_literal);
        EXPECT_TRUE(bytes_equal(expected, uuid_utf32));
    }
}

TEST(Parse, literal_2)
{
    constexpr uint8_t expected[] = 
    {
          0xab, 0xcd, 0xef, 0x12
        , 0x34, 0x56
        , 0x78, 0x9a
        , 0xbc, 0xde
        , 0xf1, 0x23, 0x45, 0x67, 0x89, 0xab  
    };

    {
        constexpr auto uuid_char = "abcdef12-3456-789a-bcde-f123456789ab"_uuid;
        EXPECT_TRUE(bytes_equal(expected, uuid_char));
    }
    {
        constexpr auto uuid_utf8 = u8"abcdef12-3456-789a-bcde-f123456789ab"_uuid;
        EXPECT_TRUE(bytes_equal(expected, uuid_utf8));
    }
    {
        constexpr auto uuid_wchar = L"abcdef12-3456-789a-bcde-f123456789ab"_uuid;
        EXPECT_TRUE(bytes_equal(expected, uuid_wchar));
    }
    {
        constexpr auto uuid_utf16 = u"abcdef12-3456-789a-bcde-f123456789ab"_uuid;
        EXPECT_TRUE(bytes_equal(expected, uuid_utf16));
    }
    {
        constexpr auto uuid_utf32 = U"abcdef12-3456-789a-bcde-f123456789ab"_uuid;
        EXPECT_TRUE(bytes_equal(expected, uuid_utf32));
    }
}

TEST(Parse, from_string)
{
    constexpr uint8_t expected[] = 
    {
          0xab, 0xcd, 0xef, 0x12
        , 0x34, 0x56
        , 0x78, 0x9a
        , 0xbc, 0xde
        , 0xf1, 0x23, 0x45, 0x67, 0x89, 0xab  
    };

    {
        const char* const char_string = "abcdef12-3456-789a-bcde-f123456789ab";
        const auto uuid_char = rfc4122::from_string(char_string);
        EXPECT_TRUE(bytes_equal(expected, uuid_char));
    }
    {
        const char8_t* const utf8_string = u8"abcdef12-3456-789a-bcde-f123456789ab";
        const auto uuid_utf8 = rfc4122::from_string(utf8_string);
        EXPECT_TRUE(bytes_equal(expected, uuid_utf8));
    }
    {
        const wchar_t* const wide_string = L"abcdef12-3456-789a-bcde-f123456789ab";
        const auto uuid_wide = rfc4122::from_string(wide_string);
        EXPECT_TRUE(bytes_equal(expected, uuid_wide));
    }
    {
        const char16_t* const utf16_string = u"abcdef12-3456-789a-bcde-f123456789ab";
        const auto uuid_utf16 = rfc4122::from_string(utf16_string);
        EXPECT_TRUE(bytes_equal(expected, uuid_utf16));
    }
    {
        const char32_t* const utf32_string = U"abcdef12-3456-789a-bcde-f123456789ab";
        const auto uuid_utf32 = rfc4122::from_string(utf32_string);
        EXPECT_TRUE(bytes_equal(expected, uuid_utf32));
    }
}

TEST(Parse, stream)
{
    constexpr uint8_t expected[] = 
    {
          0xab, 0xcd, 0xef, 0x12
        , 0x34, 0x56
        , 0x78, 0x9a
        , 0xbc, 0xde
        , 0xf1, 0x23, 0x45, 0x67, 0x89, 0xab  
    };

    {
        std::stringstream char_stream;
        char_stream << "abcdef12-3456-789a-bcde-f123456789ab";
        rfc4122::uuid uuid_char_stream{};
        rfc4122::parse(char_stream, uuid_char_stream);
        EXPECT_TRUE(bytes_equal(expected, uuid_char_stream));
    }
    {
        std::basic_stringstream<char8_t> utf8_stream;
        utf8_stream << u8"abcdef12-3456-789a-bcde-f123456789ab";
        rfc4122::uuid uuid_utf8_stream{};
        rfc4122::parse(utf8_stream, uuid_utf8_stream);
        EXPECT_TRUE(bytes_equal(expected, uuid_utf8_stream));
    }
    {
        std::wstringstream wchar_stream;
        wchar_stream << L"abcdef12-3456-789a-bcde-f123456789ab";
        rfc4122::uuid uuid_wchar_stream{};
        rfc4122::parse(wchar_stream, uuid_wchar_stream);
        EXPECT_TRUE(bytes_equal(expected, uuid_wchar_stream));
    }
    {
        std::basic_stringstream<char16_t> utf16_stream;
        utf16_stream << u"abcdef12-3456-789a-bcde-f123456789ab";
        rfc4122::uuid uuid_utf16_stream{};
        rfc4122::parse(utf16_stream, uuid_utf16_stream);
        EXPECT_TRUE(bytes_equal(expected, uuid_utf16_stream));
    }
    {
        std::basic_stringstream<char32_t> utf32_stream;
        utf32_stream << U"abcdef12-3456-789a-bcde-f123456789ab";
        rfc4122::uuid uuid_utf32_stream{};
        rfc4122::parse(utf32_stream, uuid_utf32_stream);
        EXPECT_TRUE(bytes_equal(expected, uuid_utf32_stream));
    }
}

TEST(Format, to_string)
{
    constexpr uint8_t expected[] = 
    {
          0xab, 0xcd, 0xef, 0x12
        , 0x34, 0x56
        , 0x78, 0x9a
        , 0xbc, 0xde
        , 0xf1, 0x23, 0x45, 0x67, 0x89, 0xab  
    };
    rfc4122::uuid actual{};
    std::memcpy(&actual, &expected, sizeof(actual));

    EXPECT_EQ(  "abcdef12-3456-789a-bcde-f123456789ab", rfc4122::to_string(   actual));
    EXPECT_EQ(u8"abcdef12-3456-789a-bcde-f123456789ab", rfc4122::to_u8string( actual));
    EXPECT_EQ( L"abcdef12-3456-789a-bcde-f123456789ab", rfc4122::to_wstring(  actual));
    EXPECT_EQ( u"abcdef12-3456-789a-bcde-f123456789ab", rfc4122::to_u16string(actual));
    EXPECT_EQ( U"abcdef12-3456-789a-bcde-f123456789ab", rfc4122::to_u32string(actual));
}

TEST(Format, parts)
{
    constexpr uint8_t expected[] = 
    {
          0xab, 0xcd, 0xef, 0x12
        , 0x34, 0x56
        , 0x78, 0x9a
        , 0xbc, 0xde
        , 0xf1, 0x23, 0x45, 0x67, 0x89, 0xab  
    };
    rfc4122::uuid actual{};
    std::memcpy(&actual, &expected, sizeof(actual));
    
    EXPECT_EQ("abcdef12"    , hex_string(4, actual.part1()));
    EXPECT_EQ("3456"        , hex_string(2, actual.part2()));
    EXPECT_EQ("789a"        , hex_string(2, actual.part3()));
    EXPECT_EQ("bcde"        , hex_string(2, actual.part4()));
    EXPECT_EQ("f123456789ab", hex_string(6, actual.part5()));
}

