#include <img_lib.h>
#include <jpeg_image.h>
#include <ppm_image.h>
#include <bmp_image.h>

#include <filesystem>
#include <string_view>
#include <iostream>

using namespace std;

enum class Format { JPEG, PPM, BMP, UNKNOWN};

class ImageFormatInterface {
public:
    virtual bool SaveImage(const img_lib::Path& file, const img_lib::Image& image) const = 0;
    virtual img_lib::Image LoadImage(const img_lib::Path& file) const = 0;
    virtual ~ImageFormatInterface() = default;
};

namespace image_type {
    class Jpeg : public ImageFormatInterface {
    public:
        bool SaveImage(const img_lib::Path& file, const img_lib::Image& image) const override {
            return img_lib::SaveJPEG(file, image);
        }
        img_lib::Image LoadImage(const img_lib::Path& file) const override {
            return img_lib::LoadJPEG(file);
        }
    };

    class Ppm : public ImageFormatInterface {
    public:
        bool SaveImage(const img_lib::Path& file, const img_lib::Image& image) const override {
            return img_lib::SavePPM(file, image);
        }
        img_lib::Image LoadImage(const img_lib::Path& file) const override {
            return img_lib::LoadPPM(file);
        }
    };

    class Bmp : public ImageFormatInterface {
    public:
        bool SaveImage(const img_lib::Path& file, const img_lib::Image& image) const override {
            return img_lib::SaveBMP(file, image);
        }
        img_lib::Image LoadImage(const img_lib::Path& file) const override {
            return img_lib::LoadBMP(file);
        }

    };
}

Format GetFormatByExtension(const img_lib::Path& input_file) {
    const string ext = input_file.extension().string();
    if (ext == ".jpg"sv || ext == ".jpeg"sv) {
        return Format::JPEG;
    }

    if (ext == ".ppm"sv) {
        return Format::PPM;
    }

    if (ext == ".bmp"sv) {
        return Format::BMP;
    }

    return Format::UNKNOWN;
}

ImageFormatInterface* GetFormatInterface(const img_lib::Path& path) {
    if (GetFormatByExtension(path) == Format::JPEG) {
        return new image_type::Jpeg();
    }
    else if (GetFormatByExtension(path) == Format::PPM) {
        return new image_type::Ppm();
    }
    else if (GetFormatByExtension(path) == Format::BMP) {
        return new image_type::Bmp();
    }
    else {
        return nullptr;
    }
}
int main(int argc, const char** argv) {
    if (argc != 3) {
        cerr << "Usage: "sv << argv[0] << " <in_file> <out_file>"sv << endl;
        return 1;
    }

    img_lib::Path in_path = argv[1];
    img_lib::Path out_path = argv[2];
    ImageFormatInterface* input_file = GetFormatInterface(in_path);
    if (!input_file) {
        cerr << "Unknown format of the input file"sv << endl;
        return 2;
    }
    ImageFormatInterface* output_file = GetFormatInterface(out_path);
    if (!output_file) {
        cerr << "Unknown format of the output file"sv << endl;
        return 3;
    }
    img_lib::Image image = input_file->LoadImage(in_path);
    if (!image) {
        cerr << "Loading failed"sv << endl;
        return 4;
    }

    if (!output_file->SaveImage(out_path, image)) {
        cerr << "Saving failed"sv << endl;
        return 5;
    }
    delete input_file; 
    delete output_file;

    cout << "Successfully converted"sv << endl;
}