#pragma once
#include <fstream>
#include <LLUtils/PlatformUtility.h>
#include <LLUtils/FileHelper.h>
#include <LLUtils/Exception.h>
namespace FreeType
{
#pragma pack(push,1)
    struct BitmapFileHeader
    {
        uint16_t   bfType;
        uint32_t   bfSize;
        uint16_t   bfReserved1;
        uint16_t   bfReserved2;
        uint32_t   bfOffBits;
    };

    struct BitmapInfoHeader
    {
        uint32_t      biSize;
        int32_t       biWidth;
        int32_t       biHeight;
        uint16_t      biPlanes;
        uint16_t      biBitCount;
        uint32_t      biCompression;
        uint32_t      biSizeImage;
        int32_t       biXPelsPerMeter;
        int32_t       biYPelsPerMeter;
        uint32_t      biClrUsed;
        uint32_t      biClrImportant;
    };

#pragma pack(pop)
    struct BitmapBuffer
    {
        const std::byte* buffer;
        uint8_t bitsPerPixel;
        uint32_t width;
        uint32_t height;
        uint32_t rowPitch;
    };

    class Bitmap;
    using BitmapSharedPtr = std::shared_ptr<Bitmap>;

    class Bitmap
    {
  
    public:

        Bitmap(const BitmapBuffer& bitmapBuffer)
        {
            fBitmapBuffer = bitmapBuffer;
        }

        void SaveToFile(const std::wstring& fileName)
        {
            BitmapFileHeader fileHeader{};
            fileHeader.bfType = 0x4D42; // "BM"
            fileHeader.bfOffBits = sizeof(BitmapFileHeader) + sizeof(BitmapInfoHeader);

            BitmapInfoHeader infoHeader{};
            
            uint32_t rowPitch = fBitmapBuffer.bitsPerPixel * fBitmapBuffer.width / CHAR_BIT;

            infoHeader.biBitCount = static_cast<uint16_t>(fBitmapBuffer.bitsPerPixel);
            infoHeader.biHeight = static_cast<int32_t>(fBitmapBuffer.height);
            infoHeader.biWidth = static_cast<int32_t>(fBitmapBuffer.width);
            infoHeader.biPlanes = 1;
            infoHeader.biSize = 40;
            infoHeader.biSizeImage = rowPitch * fBitmapBuffer.height;

            LLUtils::Buffer flipped(fBitmapBuffer.height * fBitmapBuffer.rowPitch);


            for (size_t y = 0; y < fBitmapBuffer.height; y++)
            {
                auto destRow = (reinterpret_cast<uint8_t*>(flipped.data())) + (fBitmapBuffer.height - y - 1) * fBitmapBuffer.rowPitch;
                auto sourceRow = (reinterpret_cast<const uint8_t*>(fBitmapBuffer.buffer)) + y * fBitmapBuffer.rowPitch;

                for (uint32_t x = 0; x < fBitmapBuffer.width; x++)
                {
                    LLUtils::Color color = reinterpret_cast<const LLUtils::Color*>(sourceRow)[x];
					
					// Color class channel order layout is RGBA, Windows bitmap color channel order is BGRA.
					// swap R and B before writing to disk.
                    std::swap(color.R(), color.B());
                    
                    reinterpret_cast<LLUtils::Color*>(destRow)[x] = color;
                }
            }


            LLUtils::File::WriteAllBytes(fileName, sizeof(BitmapFileHeader), reinterpret_cast<std::byte*>(&fileHeader));
            LLUtils::File::WriteAllBytes(fileName, sizeof(BitmapInfoHeader), reinterpret_cast<const std::byte*>(&infoHeader), true);
            LLUtils::File::WriteAllBytes(fileName, infoHeader.biSizeImage, reinterpret_cast<const std::byte*>(flipped.data()), true);

        }

    private:
        BitmapBuffer fBitmapBuffer;
    };
}