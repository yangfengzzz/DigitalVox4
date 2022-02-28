//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "astc.h"
#include <mutex>
#include <astc_codec_internals.h>

#define MAGIC_FILE_CONSTANT 0x5CA1AB13

namespace vox {
BlockDim toBlockdim(const MTL::PixelFormat format) {
    switch (format) {
        case MTL::PixelFormatASTC_4x4_HDR:
        case MTL::PixelFormatASTC_4x4_LDR:
        case MTL::PixelFormatASTC_4x4_sRGB:
            return {4, 4, 1};
        case MTL::PixelFormatASTC_5x4_HDR:
        case MTL::PixelFormatASTC_5x4_LDR:
        case MTL::PixelFormatASTC_5x4_sRGB:
            return {5, 4, 1};
        case MTL::PixelFormatASTC_5x5_HDR:
        case MTL::PixelFormatASTC_5x5_LDR:
        case MTL::PixelFormatASTC_5x5_sRGB:
            return {5, 5, 1};
        case MTL::PixelFormatASTC_6x5_HDR:
        case MTL::PixelFormatASTC_6x5_LDR:
        case MTL::PixelFormatASTC_6x5_sRGB:
            return {6, 5, 1};
        case MTL::PixelFormatASTC_6x6_HDR:
        case MTL::PixelFormatASTC_6x6_LDR:
        case MTL::PixelFormatASTC_6x6_sRGB:
            return {6, 6, 1};
        case MTL::PixelFormatASTC_8x5_HDR:
        case MTL::PixelFormatASTC_8x5_LDR:
        case MTL::PixelFormatASTC_8x5_sRGB:
            return {8, 5, 1};
        case MTL::PixelFormatASTC_8x6_HDR:
        case MTL::PixelFormatASTC_8x6_LDR:
        case MTL::PixelFormatASTC_8x6_sRGB:
            return {8, 6, 1};
        case MTL::PixelFormatASTC_8x8_HDR:
        case MTL::PixelFormatASTC_8x8_LDR:
        case MTL::PixelFormatASTC_8x8_sRGB:
            return {8, 8, 1};
        case MTL::PixelFormatASTC_10x5_HDR:
        case MTL::PixelFormatASTC_10x5_LDR:
        case MTL::PixelFormatASTC_10x5_sRGB:
            return {10, 5, 1};
        case MTL::PixelFormatASTC_10x6_HDR:
        case MTL::PixelFormatASTC_10x6_LDR:
        case MTL::PixelFormatASTC_10x6_sRGB:
            return {10, 6, 1};
        case MTL::PixelFormatASTC_10x8_HDR:
        case MTL::PixelFormatASTC_10x8_LDR:
        case MTL::PixelFormatASTC_10x8_sRGB:
            return {10, 8, 1};
        case MTL::PixelFormatASTC_10x10_HDR:
        case MTL::PixelFormatASTC_10x10_LDR:
        case MTL::PixelFormatASTC_10x10_sRGB:
            return {10, 10, 1};
        case MTL::PixelFormatASTC_12x10_HDR:
        case MTL::PixelFormatASTC_12x10_LDR:
        case MTL::PixelFormatASTC_12x10_sRGB:
            return {12, 10, 1};
        case MTL::PixelFormatASTC_12x12_HDR:
        case MTL::PixelFormatASTC_12x12_LDR:
        case MTL::PixelFormatASTC_12x12_sRGB:
            return {12, 12, 1};
        default:
            throw std::runtime_error{"Invalid astc format"};
    }
}

struct AstcHeader {
    uint8_t magic[4];
    uint8_t blockdim_x;
    uint8_t blockdim_y;
    uint8_t blockdim_z;
    uint8_t xsize[3];        // x-size = xsize[0] + xsize[1] + xsize[2]
    uint8_t ysize[3];        // x-size, y-size and z-size are given in texels;
    uint8_t zsize[3];        // block count is inferred
};

void Astc::init() {
    // Initializes ASTC library
    static bool initialized{false};
    static std::mutex initialization;
    std::unique_lock<std::mutex> lock{initialization};
    if (!initialized) {
        // Init stuff
        prepare_angular_tables();
        build_quantization_mode_table();
        initialized = true;
    }
}

void Astc::decode(BlockDim blockdim, MTL::Size extent, const uint8_t *data_) {
    // Actual decoding
    astc_decode_mode decode_mode = DECODE_LDR_SRGB;
    uint32_t bitness = 8;
    swizzlepattern swz_decode = {0, 1, 2, 3};
    
    int xdim = blockdim.x;
    int ydim = blockdim.y;
    int zdim = blockdim.z;
    
    if ((xdim < 3 || xdim > 6 || ydim < 3 || ydim > 6 || zdim < 3 || zdim > 6) &&
        (xdim < 4 || xdim == 7 || xdim == 9 || xdim == 11 || xdim > 12 ||
         ydim < 4 || ydim == 7 || ydim == 9 || ydim == 11 || ydim > 12 || zdim != 1)) {
        throw std::runtime_error{"Error reading astc: invalid block"};
    }
    
    int xsize = static_cast<int>(extent.width);
    int ysize = static_cast<int>(extent.height);
    int zsize = static_cast<int>(extent.depth);
    
    if (xsize == 0 || ysize == 0 || zsize == 0) {
        throw std::runtime_error{"Error reading astc: invalid size"};
    }
    
    int xblocks = (xsize + xdim - 1) / xdim;
    int yblocks = (ysize + ydim - 1) / ydim;
    int zblocks = (zsize + zdim - 1) / zdim;
    
    auto astc_image = allocate_image(bitness, xsize, ysize, zsize, 0);
    initialize_image(astc_image);
    
    imageblock pb;
    for (int z = 0; z < zblocks; z++) {
        for (int y = 0; y < yblocks; y++) {
            for (int x = 0; x < xblocks; x++) {
                int offset = (((z * yblocks + y) * xblocks) + x) * 16;
                const uint8_t *bp = data_ + offset;
                
                physical_compressed_block pcb = *reinterpret_cast<const physical_compressed_block *>(bp);
                symbolic_compressed_block scb;
                
                physical_to_symbolic(xdim, ydim, zdim, pcb, &scb);
                decompress_symbolic_block(decode_mode, xdim, ydim, zdim, x * xdim, y * ydim, z * zdim, &scb, &pb);
                write_imageblock(astc_image, &pb, xdim, ydim, zdim, x * xdim, y * ydim, z * zdim, swz_decode);
            }
        }
    }
    
    setData(astc_image->imagedata8[0][0], astc_image->xsize * astc_image->ysize * astc_image->zsize * 4);
    setFormat(MTL::PixelFormatRGBA8Unorm_sRGB);
    setWidth(static_cast<uint32_t>(astc_image->xsize));
    setHeight(static_cast<uint32_t>(astc_image->ysize));
    setDepth(static_cast<uint32_t>(astc_image->zsize));
    
    destroy_image(astc_image);
}

Astc::Astc(const Image &image, bool flipY) :
Image{} {
    init();
    decode(toBlockdim(image.format()), image.extent(), image.data().data());
}

Astc::Astc(const std::vector<uint8_t> &data, bool flipY) :
Image{} {
    init();
    
    // Read header
    if (data.size() < sizeof(AstcHeader)) {
        throw std::runtime_error{"Error reading astc: invalid memory"};
    }
    AstcHeader header{};
    std::memcpy(&header, data.data(), sizeof(AstcHeader));
    uint32_t magicval = header.magic[0] + 256 * static_cast<uint32_t>(header.magic[1])
    + 65536 * static_cast<uint32_t>(header.magic[2])
    + 16777216 * static_cast<uint32_t>(header.magic[3]);
    if (magicval != MAGIC_FILE_CONSTANT) {
        throw std::runtime_error{"Error reading astc: invalid magic"};
    }
    
    BlockDim blockdim = {
        /* xdim = */ header.blockdim_x,
        /* ydim = */ header.blockdim_y,
        /* zdim = */ header.blockdim_z};
    
    MTL::Size extent = {
        /* width  = */ static_cast<uint32_t>(header.xsize[0] + 256 * header.xsize[1] + 65536 * header.xsize[2]),
        /* height = */ static_cast<uint32_t>(header.ysize[0] + 256 * header.ysize[1] + 65536 * header.ysize[2]),
        /* depth  = */ static_cast<uint32_t>(header.zsize[0] + 256 * header.zsize[1] + 65536 * header.zsize[2])};
    
    decode(blockdim, extent, data.data() + sizeof(AstcHeader));
}

}        // namespace vox
