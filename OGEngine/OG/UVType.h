#pragma once
// calculate texture coordinates
namespace OG
{
    enum UVType {
        PLANAR_UV = 0,
        CYLINDRICAL_UV,
        SPHERICAL_UV,
        CUBE_MAPPED_UV
    };

    enum UVCalc
    {
        CPU_SIDE = 0, // Calculate UV on CPU
        GPU_PLANAR = 1, // Calculate UV on GPU as PLANAR (CUBEMAP)
        GPU_CYLINDRICAL = 2,
        GPU_SPHERICAL = 3,
    };
}



