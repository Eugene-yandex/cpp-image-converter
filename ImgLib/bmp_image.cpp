#include "bmp_image.h"
#include "pack_defines.h"

#include <array>
#include <fstream>
#include <string_view>

using namespace std;

namespace img_lib {
    const int COUNT_ELEMENTS_PIXEL = 3;
    const int MULTIPLICITY_OF_ALIGNMENT = 4;

    PACKED_STRUCT_BEGIN BitmapFileHeader{
        array<char,2> signature{'B','M'};
        uint32_t size_all = 0; 
        uint32_t reserve = 0;
        uint32_t indentation_from_the_beginning = 0; 
    }
    PACKED_STRUCT_END

    PACKED_STRUCT_BEGIN BitmapInfoHeader{
        uint32_t size_info_header = 0; 
        int width = 0; 
        int height = 0; 
        unsigned short int count_of_planes = 1;
        unsigned short int count_bits_on_pix = 24;
        uint32_t compression_type = 0;
        uint32_t count_of_bytes_in_data = 0; 
        int horizontal_resolution = 11811;
        int vertical_resolution = 11811;
        int count_of_colors_used = 0;
        int count_of_colors_significant = 0x1000000;
    }
    PACKED_STRUCT_END

static int GetBMPStride(int w) {
        return MULTIPLICITY_OF_ALIGNMENT * ((w * COUNT_ELEMENTS_PIXEL + COUNT_ELEMENTS_PIXEL) / MULTIPLICITY_OF_ALIGNMENT);
    }

    // напишите эту функцию
    bool SaveBMP(const Path& file, const Image& image) {
        ofstream out(file, ios::binary);

        BitmapFileHeader file_header;
        const int indent = GetBMPStride(image.GetWidth());
        file_header.indentation_from_the_beginning = sizeof(BitmapFileHeader) + sizeof(BitmapInfoHeader);
        file_header.size_all = file_header.indentation_from_the_beginning + (indent * image.GetHeight());
        out.write(reinterpret_cast<char*>(&file_header), sizeof(BitmapFileHeader));

        BitmapInfoHeader info_header;
        info_header.size_info_header = sizeof(BitmapInfoHeader);
        info_header.width = image.GetWidth();
        info_header.height = image.GetHeight();
        info_header.count_of_bytes_in_data = indent * info_header.height;
        out.write(reinterpret_cast<char*>(&info_header), sizeof(BitmapInfoHeader));

        std::vector<char> buff(indent);

        for (int y = info_header.height -1; y >= 0; --y) {
            const Color* line = image.GetLine(y);
            for (int x = 0; x < info_header.width; ++x) {
                buff[x * COUNT_ELEMENTS_PIXEL + 2] = static_cast<char>(line[x].r);
                buff[x * COUNT_ELEMENTS_PIXEL + 1] = static_cast<char>(line[x].g);
                buff[x * COUNT_ELEMENTS_PIXEL] = static_cast<char>(line[x].b);
            }
            out.write(buff.data(), indent);
        }

        return out.good();

    }

    // напишите эту функцию
    Image LoadBMP(const Path& file) {
        ifstream ifs(file, ios::binary);

        BitmapFileHeader file_header;
        BitmapInfoHeader info_header;

        ifs.read(reinterpret_cast<char*>(&file_header), sizeof(BitmapFileHeader));
        if (!ifs.good()) {
            return {};
        }

        ifs.read(reinterpret_cast<char*>(&info_header), sizeof(BitmapInfoHeader));
        if (!ifs.good()) {
            return {};
        }

        if (file_header.signature[0] != 'B' || file_header.signature[1] != 'M') {
            return {};
        }

        int indent = (file_header.size_all - sizeof(BitmapFileHeader) - sizeof(BitmapInfoHeader)) / info_header.height;
        Image result(info_header.width, info_header.height, Color::Black());

        std::vector<char> buff(indent);

        for (int y = info_header.height - 1; y >= 0; --y) {
            Color* line = result.GetLine(y);
            ifs.read(buff.data(), indent);
            if (!ifs.good()) {
                return {};
            }

            for (int x = 0; x < info_header.width; ++x) {
                line[x].r = static_cast<byte>(buff[x * COUNT_ELEMENTS_PIXEL + 2]);
                line[x].g = static_cast<byte>(buff[x * COUNT_ELEMENTS_PIXEL + 1]);
                line[x].b = static_cast<byte>(buff[x * COUNT_ELEMENTS_PIXEL]);
            }
        }

        return result;

    }

}  // namespace img_lib