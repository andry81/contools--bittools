#include "main.hpp"

#include "utility/utility.hpp"
#include "utility/assert.hpp"

#include "tackle/file_reader.hpp"

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#include <string>
#include <iostream>
#include <vector>
#include <algorithm>

#include <stdio.h>
#include <stdlib.h>

namespace po = boost::program_options;
using namespace utility;

namespace boost {
    namespace fs = filesystem;
}

namespace
{
    struct UserData
    {
        size_t byte_width;
        FileHandle file_out_handle;
    };

    void mirror_buffer(uint8_t * buf, uint32_t byte_width)
    {
        ASSERT_TRUE(buf && byte_width);
        // reverse bits in bytes
        for (size_t i = 0; i < byte_width; i++) {
            buf[i] = utility::reverse(buf[i]);
        }
        // reverse bytes
        std::reverse(buf, buf + byte_width);
    }

    void _read_file_chunk(uint8_t * buf, uint64_t size, void * user_data)
    {
        if (sizeof(size_t) < sizeof(uint64_t)) {
            const uint64_t max_value = uint64_t((std::numeric_limits<size_t>::max)());
            if (size > max_value) {
                throw std::runtime_error(
                    (boost::format(
                        BOOST_PP_CAT(__FUNCTION__, ": size is out of buffer: size=%llu")) %
                            size).str());
            }
        }

        const UserData & data = *static_cast<UserData *>(user_data);

        const size_t read_size = uint32_t(size);

        if (read_size < data.byte_width) {
            memmove(&buf[data.byte_width - read_size], buf, read_size);
            memset(buf, 0, data.byte_width - read_size);
        }

        mirror_buffer(buf, data.byte_width);

        const size_t write_size = fwrite(buf, 1, data.byte_width, data.file_out_handle.get());
        const int file_write_err = ferror(data.file_out_handle.get());
        if (write_size < data.byte_width) {
            utility::debug_break();
            throw std::system_error{ file_write_err, std::system_category(), data.file_out_handle.path() };
        }
    }
}

int main(int argc, char* argv[])
{
    try {
        std::string in_file;
        std::string out_file;
        std::string byte_width_str;

        po::options_description desc("Allowed options");
        desc.add_options()
            ("help,h", "print usage message")
            ("input,i",
                po::value(&in_file), "input file and output file prefix if output file is not set explicitly")
            ("output,o",
                po::value(&out_file), "output file")
            ("byte_width,b",
                po::value(&byte_width_str), "byte width of the file stream to mirror")
        ;

        po::positional_options_description p;
        p.add("input", 1);
        p.add("byte_width", 1);

        po::variables_map vm;
        po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
        po::notify(vm); // important, otherwise related option variables won't be initialized

        if (vm.count("help")) {
            std::cout << desc << "\n";
            return 0;
        }

        if (!boost::fs::exists(in_file)) {
            fprintf(stderr, "error: input file is not found: \"%s\"\n", in_file.c_str());
            return 1;
        }

        if (in_file == out_file) {
            fprintf(stderr, "error: output file should not be input\n");
            return 2;
        }

        FileHandle file_in_handle = open_file(in_file, "rb", _SH_DENYWR);

        boost::fs::path in_file_path = boost::fs::path(in_file);

        if (out_file.empty()) {
            const std::string & out_parent_path = in_file_path.parent_path().string();
            out_file = out_parent_path + (!out_parent_path.empty() ? "/" : "") + in_file_path.stem().string() + "_mirror" + in_file_path.extension().string();
        }

        FileHandle file_out_handle = open_file(out_file, "wb", _SH_DENYWR);

        typedef std::shared_ptr<uint8_t> ReadBufSharedPtr;

        std::vector<uint8_t> row = { 0xff, 0xff, 0xff, 0xff };
        uint32_t byte_width = 32; // 256 bits
        if (!byte_width_str.empty()) {
            byte_width = std::stoul(byte_width_str, 0, 0);
        }
        // maximum
        if (byte_width > 1024 * 1024) {
            byte_width = 1024 * 1024;
        }

        UserData user_data;
        user_data.byte_width = byte_width;
        user_data.file_out_handle = file_out_handle;
        tackle::FileReader(file_in_handle, _read_file_chunk).do_read(&user_data, {}, byte_width, byte_width);
    }
    catch (std::exception & e) {
        std::cerr << e.what() << "\n";
        return -1;
    }

    return 0;
}
