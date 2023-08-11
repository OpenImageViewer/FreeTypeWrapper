#pragma once
#include <cstdint>
namespace FreeType
{
    struct BlitBox
    {
        std::byte* buffer;
        uint32_t rowPitch;
        uint32_t width;
        uint32_t height;
        uint32_t left;
        uint32_t top;
        uint32_t pixelSizeInbytes;

        uint32_t GetStartOffset() const
        {
            return (top * rowPitch) + (left * pixelSizeInbytes);
        }

        static void Blit(BlitBox& dst, const BlitBox& src)
        {
            const std::byte* srcPos = src.buffer + src.GetStartOffset();
            std::byte* dstPos = dst.buffer + dst.GetStartOffset();

            //Perform range check on target.
            if (dst.left + src.width > dst.width)
                LL_EXCEPTION(LLUtils::Exception::ErrorCode::LogicError, "Width overflow");

            if (dst.top + src.height > dst.height)
                LL_EXCEPTION(LLUtils::Exception::ErrorCode::LogicError, "Height overflow ");


            const uint32_t bytesPerLine = src.pixelSizeInbytes * src.width;

            for (uint32_t y = src.top; y < src.height; y++)
            {
                for (uint32_t x = 0; x < bytesPerLine; x += 4)
                {
                    using namespace LLUtils;

                    const Color& srcColor = *reinterpret_cast<const Color*>(srcPos + x);
                    Color& dstColor = *reinterpret_cast<Color*>(dstPos + x);
                    dstColor = dstColor.Blend(srcColor);
                }
                dstPos += dst.rowPitch;
                srcPos += src.rowPitch;
            }
        }
    };
}