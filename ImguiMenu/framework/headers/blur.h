#pragma once
#include "includes.h"

// Stub: blur is not supported on OpenGL (Minecraft Java Edition)
void draw_background_blur(ImDrawList* draw_list, void* device, void* ctx, float rounding);
void release_blur_resources();
