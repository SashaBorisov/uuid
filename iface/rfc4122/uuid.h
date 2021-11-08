#pragma once
//
// Copyright © 2021, Alexander Borisov, https://github.com/SashaBorisov/uuid
//

#include <cstdint>
#include <cctype>
#include <cinttypes>
#include <optional>
#include <utility>
#include <string>
#include <string_view>
#include <ostream>
#include <istream>



namespace rfc4122
{
    namespace __internal
    {
        
        struct byte_order
        {
            enum endian: int32_t
            {
                  little_endian = 1
                , big_endian    = 2
            };

            static constexpr endian other(const endian from) noexcept
            {
                return from == little_endian ? big_endian : little_endian; 
            }

            static constexpr endian net = big_endian;
            static constexpr endian host = []() noexcept -> endian
            {
                return static_cast<endian>((0xFF & (little_endian | (big_endian << 24))));
            }();
            static_assert(     host == little_endian 
                            || host == big_endian, "wrong host byte_order");

            // template<typename ...B>
            // using closin_type = std::tuple_element_t<sizeof...(B) - 1, std::tuple<
            //       uint8_t
            //     , uint16_t
            //     , uint32_t, uint32_t
            //     , uint64_t, uint64_t, uint64_t, uint64_t>>;
            //
            // template<endian target, typename ...B>
            // static constexpr typename closin_type<B...> bytes_to_value(const B... byte) noexcept
            // {
            //     using type = closin_type<B...>;
            //     type value = 0;
            //     int shift = -8;
            //     if constexpr (host == target)
            //     {
            //         // const type bytes[] = {static_cast<type>(byte)...};
            //         // int i = sizeof...(B);
            //         value =
            //         (    (
            //                 shift += 8,
            //                 (   (static_cast<type>(byte) << shift) 
            //                   & (static_cast<type>(0xFF) << shift)  )
            //         )|...|0);
            //     }
            //     else
            //     {
            //         // int shift = 0;
            //         value =
            //         (0|...|(
            //                 shift += 8,
            //                 (   (static_cast<type>(byte) << shift) 
            //                   & (static_cast<type>(0xFF) << shift)  )
            //         )    );
            //     }
            //     return value;
            // }

            template<endian target>
            static constexpr uint16_t bytes_to_value(const uint8_t byte0, const uint8_t byte1) noexcept
            {
                if constexpr (host == target)
                {
                    return    (0xFF00 & (static_cast<uint16_t>(byte0) << 8)) 
                            | (0x00FF &  static_cast<uint16_t>(byte1));
                }
                return    (0xFF00 & (static_cast<uint16_t>(byte1) << 8)) 
                        | (0x00FF &  static_cast<uint16_t>(byte0));
            }

            template<endian target>
            static constexpr uint32_t bytes_to_value( const uint8_t byte0, const uint8_t byte1
                                                    , const uint8_t byte2, const uint8_t byte3)  noexcept
            {
                if constexpr (host == target)
                {
                    return    (0xFF000000u & (static_cast<uint32_t>(byte0) << 24)) 
                            | (0x00FF0000u & (static_cast<uint32_t>(byte1) << 16))
                            | (0x0000FF00u & (static_cast<uint32_t>(byte2) <<  8)) 
                            | (0x000000FFu & (static_cast<uint32_t>(byte3) <<  0));
                }
                return    (0xFF000000u & (static_cast<uint32_t>(byte3) << 24)) 
                        | (0x00FF0000u & (static_cast<uint32_t>(byte2) << 16))
                        | (0x0000FF00u & (static_cast<uint32_t>(byte1) <<  8)) 
                        | (0x000000FFu & (static_cast<uint32_t>(byte0) <<  0));
            }

            template<endian target>
            static constexpr uint64_t bytes_to_value( const uint8_t byte0, const uint8_t byte1
                                                    , const uint8_t byte2, const uint8_t byte3
                                                    , const uint8_t byte4, const uint8_t byte5
                                                    , const uint8_t byte6, const uint8_t byte7 )  noexcept
            {
                if constexpr (host == target)
                {
                    return    (0xFF00000000000000u & (static_cast<uint64_t>(byte0) << 56)) 
                            | (0x00FF000000000000u & (static_cast<uint64_t>(byte1) << 48))
                            | (0x0000FF0000000000u & (static_cast<uint64_t>(byte2) << 40))
                            | (0x000000FF00000000u & (static_cast<uint64_t>(byte3) << 32))
                            | (0x00000000FF000000u & (static_cast<uint64_t>(byte4) << 24)) 
                            | (0x0000000000FF0000u & (static_cast<uint64_t>(byte5) << 16))
                            | (0x000000000000FF00u & (static_cast<uint64_t>(byte6) <<  8)) 
                            | (0x00000000000000FFu & (static_cast<uint64_t>(byte7) <<  0));
                }
                return    (0xFF00000000000000u & (static_cast<uint64_t>(byte7) << 56)) 
                        | (0x00FF000000000000u & (static_cast<uint64_t>(byte6) << 48))
                        | (0x0000FF0000000000u & (static_cast<uint64_t>(byte5) << 40)) 
                        | (0x000000FF00000000u & (static_cast<uint64_t>(byte4) << 32))
                        | (0x00000000FF000000u & (static_cast<uint64_t>(byte3) << 24)) 
                        | (0x0000000000FF0000u & (static_cast<uint64_t>(byte2) << 16))
                        | (0x000000000000FF00u & (static_cast<uint64_t>(byte1) <<  8)) 
                        | (0x00000000000000FFu & (static_cast<uint64_t>(byte0) <<  0));
            }

            template<typename ...B>
            static constexpr auto bytes_from_little_to_value(const B... bytes) noexcept 
            {
                return bytes_to_value<big_endian>(bytes...);
            }

            template<typename ...B>
            static constexpr auto bytes_to_little_value(const B... bytes) noexcept 
            {
                return bytes_to_value<little_endian>(bytes...);
            }

            template<typename ...B>
            static constexpr auto bytes_from_big_to_value(const B... bytes) noexcept
            {
                return bytes_to_little_value(bytes...);
            }
            
            template<typename ...B>
            static constexpr auto bytes_to_big_value(const B... bytes) noexcept
            {
                return bytes_from_little_to_value(bytes...);
            }

            template<typename ...B>
            static constexpr auto bytes_from_net_to_value(const B... bytes) noexcept
            {
                return bytes_to_value<other(net)>(bytes...);
            }
            
            template<typename ...B>
            static constexpr auto bytes_to_net_value(const B... bytes) noexcept
            {
                return bytes_to_value<net>(bytes...);
            }

            template<typename ...B>
            static constexpr auto bytes_from_host_to_value(const B... bytes) noexcept
            {
                return bytes_to_value<other(host)>(bytes...);
            }
            
            template<typename ...B>
            static constexpr auto bytes_to_host_value(const B... bytes) noexcept
            {
                return bytes_to_value<host>(bytes...);
            }

            template<uint16_t from, uint16_t to, endian target, size_t bytes_size, typename V>
            static constexpr void value_to_bytes(uint8_t (&byte)[bytes_size], const V value) noexcept
            {
                static_assert(from < to);
                static_assert(to <= bytes_size);
                static_assert(to - from <= sizeof(V));

                if constexpr (host == target)
                {
                    int shift = static_cast<int>(to - from - 1);
                    for(auto i = from; i < to; ++i, shift -= 8)
                    {
                        byte[i] = static_cast<uint8_t>((value >> shift) & V{0xFF});
                    }
                }
                else
                {
                    int shift = 0;
                    for(auto i = from; i < to; ++i, shift += 8)
                    {
                        byte[i] = static_cast<uint8_t>((value >> shift) & V{0xFF});
                    }
                }
            }

            template<uint16_t from, uint16_t to, size_t bytes_size, typename V>
            static constexpr void value_to_net_bytes(uint8_t (&byte)[bytes_size], const V value) noexcept
            {
                value_to_bytes<from,to,net>(byte, value);
            }

            template<uint16_t from, uint16_t to, size_t bytes_size, typename V>
            static constexpr void value_to_little_bytes(uint8_t (&byte)[bytes_size], const V value) noexcept
            {
                value_to_bytes<from,to,little_endian>(byte, value);
            }
        };

        static_assert('0' == u8'0');
        static_assert('9' == u8'9');
        static_assert('A' == u8'A');
        static_assert('F' == u8'F');
        static_assert('a' == u8'a');
        static_assert('f' == u8'f');
        static_assert('-' == u8'-');

        static_assert('0' == L'0');
        static_assert('9' == L'9');
        static_assert('A' == L'A');
        static_assert('F' == L'F');
        static_assert('a' == L'a');
        static_assert('f' == L'f');
        static_assert('-' == L'-');

        static_assert('0' == u'0');
        static_assert('9' == u'9');
        static_assert('A' == u'A');
        static_assert('F' == u'F');
        static_assert('a' == u'a');
        static_assert('f' == u'f');
        static_assert('-' == u'-');

        static_assert('0' == U'0');
        static_assert('9' == U'9');
        static_assert('A' == U'A');
        static_assert('F' == U'F');
        static_assert('a' == U'a');
        static_assert('f' == U'f');
        static_assert('-' == U'-');

        static constexpr uint32_t PARTS_QUARTETS_COUNT[] = {8u, 4u, 4u, 4u, 12u};

        using quartet = std::byte;
        using octet   = uint8_t;

        constexpr std::optional<quartet> hex_to_quartet(const char hex) noexcept
        {
            using maybe_quartet = std::optional<quartet>;
            return    '0' <= hex && hex <= '9' ? maybe_quartet{static_cast<quartet>(hex - '0')}
                    : 'A' <= hex && hex <= 'F' ? maybe_quartet{static_cast<quartet>(hex - 'A' + 0x0A)}
                    : 'a' <= hex && hex <= 'f' ? maybe_quartet{static_cast<quartet>(hex - 'a' + 0x0A)}
                    : std::nullopt;
        }

        template<typename C>
        constexpr std::optional<octet> hexes_to_octet(const C high_hex, const C low_hex) noexcept
        {
            using maybe_octet = std::optional<octet>;
            const auto high = hex_to_quartet(static_cast<char>(high_hex));
            const auto low  = hex_to_quartet(static_cast<char>(low_hex ));
            return high && low  ? maybe_octet{    (static_cast<octet>(*high) << 4) 
                                                |  static_cast<octet>(*low)       }
                                : std::nullopt;
        }

        static constexpr const char HEX_LETTERS[] = 
        {
              '0', '1', '2', '3', '4', '5', '6', '7'
            , '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
        };

        constexpr char low_hex(const octet byte) noexcept
        {
            return HEX_LETTERS[0x0Fu & static_cast<uint32_t>(byte)];
        }

        constexpr char high_hex(const octet byte) noexcept
        {
            return HEX_LETTERS[0x0Fu & (static_cast<uint32_t>(byte) >> 4)];
        }

    } // __internal


    
    static constexpr char NIL_UUID_STRING[] = "00000000-0000-0000-0000-000000000000";
    static constexpr size_t UUID_STRING_LENGTH = std::size(NIL_UUID_STRING) - 1u;
    template<typename C>
    using literal = C[std::size(NIL_UUID_STRING)];

    enum class variant: uint8_t
    {
		  unknown           = 0b11000000
		, ncs_compatibility = 0b00000000
		, rfc4122           = 0b10000000
		, microsoft         = 0b10100000
		, future            = 0b11100000
	};

    enum class version: uint8_t
    {
          time_based   = 1
        , dce_security = 2
        , md5_name     = 3
        , random       = 4
        , sha1_name    = 5
    };

    struct uuid
    {
        constexpr uuid() noexcept = default;
        constexpr uuid(const uuid&) noexcept = default;
        constexpr uuid& operator = (const uuid&) = default;

        explicit uuid(const std::byte* const raw)
            : byte{   static_cast<uint8_t>(raw[ 0]), static_cast<uint8_t>(raw[ 1])
                    , static_cast<uint8_t>(raw[ 2]), static_cast<uint8_t>(raw[ 3])
                    , static_cast<uint8_t>(raw[ 4]), static_cast<uint8_t>(raw[ 5])
                    , static_cast<uint8_t>(raw[ 6]), static_cast<uint8_t>(raw[ 7])
                    , static_cast<uint8_t>(raw[ 8]), static_cast<uint8_t>(raw[ 9])
                    , static_cast<uint8_t>(raw[10]), static_cast<uint8_t>(raw[11])
                    , static_cast<uint8_t>(raw[12]), static_cast<uint8_t>(raw[13])
                    , static_cast<uint8_t>(raw[14]), static_cast<uint8_t>(raw[15]) }
        {}

        constexpr uuid(   const uint32_t part1
                        , const uint16_t part2
                        , const uint16_t part3
                        , const uint16_t part4
                        , const uint64_t part5 ) noexcept
        {
            __internal::byte_order::value_to_net_bytes< 0, 4>(byte, part1);
            __internal::byte_order::value_to_net_bytes< 4, 6>(byte, part2);
            __internal::byte_order::value_to_net_bytes< 6, 8>(byte, part3);
            __internal::byte_order::value_to_net_bytes< 8,10>(byte, part4);
            __internal::byte_order::value_to_net_bytes<10,16>(byte, part5);
        }

        constexpr uuid(   const uint64_t timestamp
                        , const rfc4122::variant variant
                        , const rfc4122::version version 
                        , const uint16_t clock_sequence
                        , const uint64_t node           ) noexcept
        {
            __internal::byte_order::value_to_little_bytes< 0, 7>(byte, timestamp);
            byte[7] = (0xF0 & static_cast<uint8_t>(variant))
                    | (0x0F & static_cast<uint8_t>(version));
            __internal::byte_order::value_to_net_bytes< 8,10>(byte, clock_sequence);
            __internal::byte_order::value_to_net_bytes<10,16>(byte, node);
        }

        constexpr std::strong_ordering operator <=> (const uuid& other) const noexcept
        {
            std::strong_ordering temp = std::strong_ordering::equal;
            const std::strong_ordering result =
                  0 != (temp = byte[ 0] <=> other.byte[ 0]) ? temp
                : 0 != (temp = byte[ 1] <=> other.byte[ 1]) ? temp
                : 0 != (temp = byte[ 2] <=> other.byte[ 2]) ? temp
                : 0 != (temp = byte[ 3] <=> other.byte[ 3]) ? temp
                : 0 != (temp = byte[ 4] <=> other.byte[ 4]) ? temp
                : 0 != (temp = byte[ 5] <=> other.byte[ 5]) ? temp
                : 0 != (temp = byte[ 6] <=> other.byte[ 6]) ? temp
                : 0 != (temp = byte[ 7] <=> other.byte[ 7]) ? temp
                : 0 != (temp = byte[ 8] <=> other.byte[ 8]) ? temp
                : 0 != (temp = byte[ 9] <=> other.byte[ 9]) ? temp
                : 0 != (temp = byte[10] <=> other.byte[10]) ? temp
                : 0 != (temp = byte[11] <=> other.byte[11]) ? temp
                : std::strong_ordering::equal;
            return result;
        }

        constexpr uint32_t part1() const noexcept {return __internal::byte_order::bytes_from_net_to_value(byte[0],byte[1],byte[2],byte[3]);}
        constexpr uint16_t part2() const noexcept {return __internal::byte_order::bytes_from_net_to_value(byte[4],byte[5]);}
        constexpr uint16_t part3() const noexcept {return __internal::byte_order::bytes_from_net_to_value(byte[6],byte[7]);}
        constexpr uint16_t part4() const noexcept {return __internal::byte_order::bytes_from_net_to_value(byte[8],byte[9]);}
        constexpr uint64_t part5() const noexcept {return __internal::byte_order::bytes_from_net_to_value(0, 0, byte[10],byte[11], byte[12],byte[13],byte[14],byte[15]);}

        constexpr uint64_t timestamp() const noexcept 
        {
            return __internal::byte_order::bytes_from_little_to_value
            (
                  byte[0], byte[1], byte[2], byte[3]
                , byte[4], byte[5], byte[6], byte[7] & 0xF0
            );
        }

        constexpr rfc4122::variant variant() const noexcept
        {
            using variants = rfc4122::variant;
            const uint8_t byte7 = byte[7];
            return    (byte7 & 0b10000000) == static_cast<uint8_t>(variants::ncs_compatibility) ? variants::ncs_compatibility
                    : (byte7 & 0b11000000) == static_cast<uint8_t>(variants::rfc4122          ) ? variants::rfc4122
                    : (byte7 & 0b11100000) == static_cast<uint8_t>(variants::microsoft        ) ? variants::microsoft
                    : (byte7 & 0b11100000) == static_cast<uint8_t>(variants::future           ) ? variants::future
                    : rfc4122::variant::unknown;
        }

        constexpr rfc4122::version version() const noexcept
        {
            return static_cast<rfc4122::version>(byte[7] & 0x0F);
        }

        constexpr uint16_t clock_sequence() const noexcept 
        {
            return part4() & 0x3F;
        }

        constexpr uint64_t node() const noexcept 
        {
            return part5();
        }

    private:
        uint8_t byte[16] = {};
        
        template<typename C>
        friend constexpr void to_literal(literal<C>& buffer, const uuid& id) noexcept;

        template<typename C>
        friend constexpr uuid from_string(const C* const text, const size_t length) noexcept;

        template<typename C, typename T>
        friend std::basic_istream<C,T>& parse(std::basic_istream<C,T>& input, rfc4122::uuid& id);    

    }; // uuid
    static_assert(16u == sizeof(uuid));
    static_assert(std::is_trivially_copyable_v<uuid>);

    static constexpr uuid NIL_UUID{};

    // uuid generate_uuid();

    template<typename C>
    constexpr void to_literal(literal<C>& buffer, const uuid& id) noexcept
    {
        using namespace rfc4122::__internal;

        auto symbol_index = 0u;
        auto  octet_index = 0u;
        for(auto quartets_count: PARTS_QUARTETS_COUNT)
        {
            for(auto i = 0u; i < quartets_count; i += 2u, ++octet_index)
            {
                buffer[symbol_index++] = high_hex(id.byte[octet_index]); 
                buffer[symbol_index++] =  low_hex(id.byte[octet_index]);
            }
            if(symbol_index >= UUID_STRING_LENGTH) break;
            buffer[symbol_index++] = '-';
        }
    }

    template<typename C, typename T>
    std::basic_ostream<C,T>& print(std::basic_ostream<C,T>& output, const uuid& id)
    {
        literal<C> buffer{};
        to_literal(buffer, id);
        return output << buffer;
    }

    template<typename S>
    S to_basic_string(const uuid& id)
    {
        using C = typename S::value_type;
        S string(UUID_STRING_LENGTH, '\0');
        auto& buffer = *static_cast<literal<C>*>(static_cast<void*>(std::data(string)));
        to_literal(buffer, id);
        return string;
    }

    static std::string    to_string   (const rfc4122::uuid& id) {return to_basic_string<std::string   >(id);}
    static std::u8string  to_u8string (const rfc4122::uuid& id) {return to_basic_string<std::u8string >(id);}
    static std::wstring   to_wstring  (const rfc4122::uuid& id) {return to_basic_string<std::wstring  >(id);}
    static std::u16string to_u16string(const rfc4122::uuid& id) {return to_basic_string<std::u16string>(id);}
    static std::u32string to_u32string(const rfc4122::uuid& id) {return to_basic_string<std::u32string>(id);}


    template<typename C>
    constexpr uuid from_string(const C* const text, const size_t length) noexcept
    {
        using namespace rfc4122::__internal;
        
        uuid id{};
        auto symbol_index = 0u;
        auto  octet_index = 0u;
        for(auto quartets_count: PARTS_QUARTETS_COUNT)
        {
            for(auto i = 0u; i < quartets_count; i += 2u, ++octet_index)
            {
                const C high = text[symbol_index++];
                const C  low = text[symbol_index++];
                const std::optional<octet> temp = hexes_to_octet(high, low);
                if(!temp) break;
                id.byte[octet_index] = *temp;
            }
            if(symbol_index >= UUID_STRING_LENGTH) break;
            if('-' != text[symbol_index++]) return uuid{};
        }
        return id;
    }

    template<typename C>
    constexpr uuid from_literal(const literal<C>& text) noexcept
    {
        return from_string(std::data(text), UUID_STRING_LENGTH);
    }

    template<typename C, typename T>
    constexpr uuid from_string(const std::basic_string_view<C,T>& text) noexcept
    {
        return from_string(std::data(text), std::size(text));
    }

    template<typename C>
    constexpr uuid from_string(const C* const text) noexcept
    {
        return from_string(std::basic_string_view<C,std::char_traits<C>>{text});
    }

    template<typename C, typename T>
    std::basic_istream<C,T>& parse(std::basic_istream<C,T>& input, rfc4122::uuid& id)
    {
        using namespace rfc4122::__internal;

        uint8_t* byte = std::data(id.byte);
        const uint8_t* const end_byte = byte + std::size(id.byte);
        for(auto quartets_count: PARTS_QUARTETS_COUNT)
        {
            for(auto i = 0u; i < quartets_count; i += 2u)
            {
                const auto high = hex_to_quartet(static_cast<char>(input.peek()));
                if(!high) break;
                input.get();

                const auto low = hex_to_quartet(static_cast<char>(input.peek()));
                if(!low) break;
                input.get();
                
                const octet temp = (static_cast<octet>(*high) << 4) 
                                 |  static_cast<octet>(*low);
                *byte++ = temp;
            }

            if(end_byte <= byte) break;

            const C dash = input.peek();
            if('-' != dash) break;
            input.get();
        }
        return input;
    }

} // namespace rfc4122

namespace std
{

using rfc4122::to_string   ;
using rfc4122::to_u8string ;
using rfc4122::to_wstring  ;
using rfc4122::to_u16string;
using rfc4122::to_u32string;

}


constexpr rfc4122::uuid operator"" _uuid(const char* const text, const size_t length) noexcept
{
    return rfc4122::from_string(text, length);
}

constexpr rfc4122::uuid operator"" _uuid(const char8_t* const text, const size_t length) noexcept
{
    return rfc4122::from_string(text, length);
}

constexpr rfc4122::uuid operator"" _uuid(const wchar_t* const text, const size_t length) noexcept
{
    return rfc4122::from_string(text, length);
}

constexpr rfc4122::uuid operator"" _uuid(const char16_t* const text, const size_t length) noexcept
{
    return rfc4122::from_string(text, length);
}

constexpr rfc4122::uuid operator"" _uuid(const char32_t* const text, const size_t length) noexcept
{
    return rfc4122::from_string(text, length);
}

template<typename C, typename T>
std::basic_ostream<C,T>& operator << (std::basic_ostream<C,T>& output, const rfc4122::uuid& id)
{
    return rfc4122::print(output, id);
}

template<typename C, typename T>
std::basic_istream<C,T>& operator >> (std::basic_istream<C,T>& input, rfc4122::uuid& id)
{
    return rfc4122::parse(input, id);
}
