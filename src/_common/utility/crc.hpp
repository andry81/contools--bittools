#pragma once

#include "utility/platform.hpp"

#include <cstdint>

#include <utility>


namespace utility
{
    // crc-16
    //
    //polynomial example: x**0 + x**5 + x**12 + x**16
    //                    (1)0001000000100001       = 0x1021
    //                       1000010000001000(1)    = 0x8408
    uint16_t crc16(uint16_t polynomial, uint16_t crc, const void * buf, size_t size, uint16_t crc_init = uint16_t(~0U),
        uint16_t xor_in = 0U, uint16_t xor_out = uint16_t(~0U), bool input_reflected = false, bool result_reflected = false);

    // crc-32 (BZIP2, MPEG2, POSIX, JAMCRC, C, D, Q)
    //
    //polynomial example: x**0 + x**1 + x**2 + x**4 + x**5* +x**7 + x**8 + x**10 + x**11 + x**12 + x**16* +x**22 + x**23 + x**26 + x**32
    //                    (1)00000100110000010001110110110111       = 0x04C11DB7
    //                       11101101101110001000001100100000(1)    = 0xEDB88320
    uint32_t crc32(uint32_t polynomial, uint32_t crc, const void * buf, size_t size, uint32_t crc_init = uint32_t(~0U),
        uint32_t xor_in = 0U, uint32_t xor_out = uint32_t(~0U), bool input_reflected = false, bool result_reflected = false);
}