/**
 * @file gfx_colors.h
 * @author Jason Berger
 * @brief predefined colors for gfx module
 * @date 2022-12-21
 */





#define GFX_COLOR_RGB( red , grn , blu )     ((gfx_color_t) { .mData.mRGBdata = {.r = red, .g =grn, .b = blu}, .mMode = GFX_COLOR_MODE_888})
 
#define GFX_COLOR_ARGB_NONE     (gfx_color_t) { .mData.raw = 0x00000000, .mMode = GFX_COLOR_MODE_A888}
#define GFX_COLOR_WHITE         GFX_COLOR_RGB(0xFF,0xFF,0xFF) 
#define GFX_COLOR_BLACK         GFX_COLOR_RGB(0,0,0)
#define GFX_COLOR_RED           GFX_COLOR_RGB(0xFF,0,0) 
#define GFX_COLOR_GREEN         GFX_COLOR_RGB(0,0xFF,0) 
#define GFX_COLOR_BLUE          GFX_COLOR_RGB(0,0,0xFF) 
#define GFX_COLOR_YELLOW        GFX_COLOR_RGB(0xFF,0xFF,0xEB) 
#define GFX_COLOR_CYAN          GFX_COLOR_RGB(0,0xFF,0xFF)
#define GFX_COLOR_FUSIA         GFX_COLOR_RGB(0xFD,0x3F,0x92)

