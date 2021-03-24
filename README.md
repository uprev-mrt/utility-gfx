# ColorGfx

Module for  graphics buffering. The module is intended to handle graphics using multiple color modes (mono,565,24bit). Once functional this will be able to replace the existing MonoGFX. One benefit to this ithe ability to convert assets between color modes for displaying on any display. example: a color image could be displayed on a mono chromatic display, or a tri-color which buffers as 3 separate monochramitic canvases.

the gfx_t struct can be initialized 'buffered' or 'unbuffered'. When it is buffered, it allocates its buffer in memory and works with the local copy. When it is unbuffered, all drawing functions are sent to the callback function for writing pixels. This allows the use of displays with areas too large to store in ram.



buffered example:
```
color_gfx_t gfx;

//initialize a 128x32 canvase
mono_gfx_init_buffered(&gfx,128,32, GFX_COLOR_MODE_565);

//draw a 30x20 rectangle at x,y = 5,5
mono_gfx_draw_rect(&gfx, 5,5,30,20, COLOR_RED);
```
