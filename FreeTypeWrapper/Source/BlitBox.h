#pragma once
#include <cstdint>
#include <LLUtils/Warnings.h>
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

        template <typename color_type>
        static void BlitPremultiplied (BlitBox& dst, const BlitBox& src)
        {
LLUTILS_DISABLE_WARNING_PUSH
LLUTILS_DISABLE_WARNING_UNSAFE_BUFFER_USAGE

            const std::byte* srcPos = src.buffer + src.GetStartOffset();
            std::byte* dstPos = dst.buffer + dst.GetStartOffset();

            //Perform range check on target.
            if (dst.left + src.width > dst.width || dst.top + src.height > dst.height)
                LL_EXCEPTION(LLUtils::Exception::ErrorCode::LogicError, "Buffer out of bounds");
LLUTILS_DISABLE_WARNING_POP            
            const uint32_t bytesPerLine = src.pixelSizeInbytes * src.width;

            for (uint32_t y = src.top; y < src.height; y++)
            {
                for (uint32_t x = 0; x < bytesPerLine; x += sizeof(color_type))
                {
                    using namespace LLUtils;

                    const color_type& srcColor = *reinterpret_cast<const color_type*>(srcPos + x);
                    color_type& dstColor = *reinterpret_cast<color_type*>(dstPos + x);
                    dstColor = dstColor.BlendPreMultiplied(srcColor);
                }
                dstPos += dst.rowPitch;
                srcPos += src.rowPitch;
            }
        }


        static void Blit(BlitBox& dst, const BlitBox& src)
        {
LLUTILS_DISABLE_WARNING_PUSH
LLUTILS_DISABLE_WARNING_UNSAFE_BUFFER_USAGE
            const std::byte* srcPos = src.buffer + src.GetStartOffset();
            std::byte* dstPos = dst.buffer + dst.GetStartOffset();

            //Perform range check on target.
            if (dst.left + src.width > dst.width || dst.top + src.height > dst.height)
                LL_EXCEPTION(LLUtils::Exception::ErrorCode::LogicError, "Buffer out of bounds");
LLUTILS_DISABLE_WARNING_POP

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