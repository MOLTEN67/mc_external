#include "..//headers/blur.h"

// -------------------------------------------------------
// Blur is DX11-only and NOT supported on Minecraft Java
// (which uses OpenGL). These are stub implementations.
// -------------------------------------------------------

void draw_background_blur(ImDrawList* draw_list, void* device, void* ctx, float rounding)
{
    // No-op: blur not supported on OpenGL / Minecraft Java Edition
    (void)draw_list;
    (void)device;
    (void)ctx;
    (void)rounding;
}

void release_blur_resources()
{
    // No-op
}
