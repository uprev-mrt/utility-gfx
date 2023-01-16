/**
  *@file gfx.c
  *@brief module for  graphics
  *@author Jason Berger
  *@date 010/15/2022
  */


/* Includes ------------------------------------------------------------------*/
#include "gfx.h"
#include "string.h"
#include <stdlib.h>
#include "gfx_colors.h"




/* Private Macros ------------------------------------------------------------*/

#ifndef _swap_int
#define _swap_int(a, b) { int t = a; a = b; b = t; }
#endif

/* Private Variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/



mrt_status_t gfx_convert_color( gfx_color_t* color, gfx_color_mode_e target)
{
    static uint8_t r;
    static uint8_t g; 
    static uint8_t b; 
    static uint8_t a; 
    if(target == color->mMode)
    {
        return MRT_STATUS_OK;
    }

    a = 255;

    switch(color->mMode)
    {
        case GFX_COLOR_MODE_MONO:
            if(color->mData.mMonoData.on)
            {
                r= 255;
                g= 255;
                b = 255;
            }
            else 
            {
                r= 0;
                g= 0;
                b = 0;
            }
            break; 
        case GFX_COLOR_MODE_565:
            r = color->mData.m565data.r * 8;
            g = color->mData.m565data.g * 8;
            b = color->mData.m565data.b * 8;
            break; 
        case GFX_COLOR_MODE_888:
            r = color->mData.mRGBdata.r;
            g = color->mData.mRGBdata.g;
            b = color->mData.mRGBdata.b;
            break; 
        case GFX_COLOR_MODE_A888:
            r = color->mData.mARGBdata.r;
            g = color->mData.mARGBdata.g;
            b = color->mData.mARGBdata.b;
            a = color->mData.mARGBdata.alpha;
            break;
        case GFX_COLOR_MODE_888A:
            r = color->mData.mRGBAdata.r;
            g = color->mData.mRGBAdata.g;
            b = color->mData.mRGBAdata.b;
            a = color->mData.mRGBAdata.alpha;
            break;
    }

    color->mData.raw = 0; 
    color->mMode = target;

    switch(target)
    {
        case GFX_COLOR_MODE_MONO:
            if( r | g| b)
            {
                color->mData.mMonoData.on = true; 
            }
            break; 
        case GFX_COLOR_MODE_565:
            color->mData.m565data.r = r >> 3;
            color->mData.m565data.g = g >> 2;
            color->mData.m565data.b = b >> 3;
            break; 
        case GFX_COLOR_MODE_888:
            color->mData.mRGBdata.r = r;
            color->mData.mRGBdata.g = g;
            color->mData.mRGBdata.b = b;
            break; 
        case GFX_COLOR_MODE_A888:
            color->mData.mARGBdata.r = r;
            color->mData.mARGBdata.g = g;
            color->mData.mARGBdata.b = b;
            color->mData.mARGBdata.alpha = a;
            break;
        case GFX_COLOR_MODE_888A:
            color->mData.mRGBAdata.r = r;
            color->mData.mRGBAdata.g = g;
            color->mData.mRGBAdata.b = b;
            color->mData.mRGBAdata.alpha = a;
            break;
    }

    return MRT_STATUS_OK;

}


/* Exported functions ------------------------------------------------------- */

mrt_status_t gfx_init_buffered(gfx_t* gfx, int width, int height, gfx_color_mode_e mode)
{

    switch (mode)
    {
        case GFX_COLOR_MODE_MONO:           //Monochromatic color mode
            gfx->mPixelSize = 1;
            break;
        case GFX_COLOR_MODE_565:            //16bit color mode using 565 format
            gfx->mPixelSize = 16;
            break;
        case GFX_COLOR_MODE_888:          //24 bit color mode 
            gfx->mPixelSize = 24;
            break;
        case GFX_COLOR_MODE_A888:          //24 bit color modes 
        case GFX_COLOR_MODE_888A:          
            gfx->mPixelSize = 32;
            break;
    }
    
    gfx->mMode = mode;
    gfx->mBufferSize = ((width * height) * (gfx->mPixelSize)) / 8;
    gfx->mBuffer = (uint8_t*) malloc(gfx->mBufferSize);
    memset(gfx->mBuffer,0,gfx->mBufferSize);
    gfx->mWidth = width;
    gfx->mHeight = height;
    gfx->mFont  = NULL;
    gfx->fWritePixel = &gfx_write_pixel;
    gfx->fWriteBuffer = &gfx_write_buffer;
    gfx->mDevice  = NULL;
    gfx->mBuffered = true;
    gfx_set_pen(gfx,1,GFX_COLOR_WHITE);

    return MRT_STATUS_OK;
}

mrt_status_t gfx_init_unbuffered(gfx_t* gfx, int width, int height, gfx_color_mode_e mode, f_gfx_write_pixel write_cb, void* dev )
{

    switch (mode)
    {
        case GFX_COLOR_MODE_MONO:           //Monochromatic color mode
            gfx->mPixelSize = 1;
            break;
        case GFX_COLOR_MODE_565:            //16bit color mode using 565 format
            gfx->mPixelSize = 16;
            break;
        case GFX_COLOR_MODE_888:          //24 bit color mode 
            gfx->mPixelSize = 24;
            break;
        case GFX_COLOR_MODE_888A:          //24 bit color modes 
        case GFX_COLOR_MODE_A888:          
            gfx->mPixelSize = 32;
            break;
    }

    gfx->mMode = mode;
    gfx->mBufferSize = ((width * height) * (gfx->mPixelSize)) / 8;
    gfx->mBuffer = NULL;
    gfx->mWidth = width;
    gfx->mHeight = height;
    gfx->mFont  = NULL;
    if(write_cb == NULL)
    {
        gfx->fWritePixel = &gfx_write_pixel; 
    }
    else 
    {
        gfx->fWritePixel = write_cb;
    }
    gfx->fWriteBuffer = &gfx_write_buffer;
    gfx->mDevice  = dev;
    gfx->mBuffered = true;
    gfx_set_pen(gfx,1,GFX_COLOR_WHITE);

    return MRT_STATUS_OK;
}


mrt_status_t gfx_deinit(gfx_t* gfx)
{
    gfx->mBufferSize = 0;
    gfx->mWidth =0;
    gfx->mHeight = 0;

  //if the gfx object manages its own buffer, free it from memory
  if(gfx->mBuffered)
  {
    free(gfx->mBuffer);
  }

    return MRT_STATUS_OK;
}

mrt_status_t gfx_set_pen(gfx_t* gfx, uint32_t stroke, gfx_color_t color)
{
    gfx->mPen.mColor = color; 
    gfx->mPen.mStroke = stroke;
    gfx_convert_color(&gfx->mPen.mColor, gfx->mMode); //Convert color to match canvas mode

    return MRT_STATUS_OK;
}

mrt_status_t gfx_write_pixel(gfx_t* gfx, int x, int y, gfx_color_t* val)
{
    //If we are out of bounds, ignore
    if(( x < 0) || (x >= gfx->mWidth) || (y < 0) || (y>= gfx->mHeight))
    {
        return MRT_STATUS_OK;
    }

    if(gfx->mFlags & GFX_FLAG_HFLIP)
    {
       x = gfx->mWidth - x; 
    }

    if(gfx->mFlags & GFX_FLAG_VFLIP)
    {
       y = gfx->mHeight - x; 
    }

    uint32_t cursor = ((y * gfx->mWidth) + x) * (gfx->mPixelSize / 8);
    uint32_t byteOffset = (cursor  / 8);
    uint8_t mask = 0x80;

    
    if(gfx->mMode == GFX_COLOR_MODE_MONO)
    {        
        //get number of bits off of alignment in case we are not writing on a byte boundary
        uint8_t bitOffset = cursor % 8;
        mask = mask >> bitOffset;

        if( val->mData.mMonoData.on == 0)
        {
            gfx->mBuffer[byteOffset] &= (~mask);
        }
        else
        {
            gfx->mBuffer[byteOffset] |= mask;
        }

    }
    else 
    {
        memcpy(&gfx->mBuffer[cursor], &val->mData, (gfx->mPixelSize / 8));
    }

    return MRT_STATUS_OK;
}

mrt_status_t gfx_write_buffer(gfx_t* gfx, int x, int y, uint8_t* data, int len, uint32_t opt)
{
    return MRT_STATUS_NOT_IMPLEMENTED;
}

mrt_status_t gfx_refresh(gfx_t* gfx)
{
    return gfx->fWriteBuffer(gfx, 0,0,gfx->mBuffer, gfx->mBufferSize, true);
}

mrt_status_t gfx_draw_bmp(gfx_t* gfx, int x, int y,const GFXBmp* bmp)
{
    uint32_t bmpIdx = 0;
    uint8_t mask =0x80;
    int bit =0;
    int i,a;

    if( bmp->mMode == GFX_COLOR_MODE_MONO)
    {

        for(i=0; i < bmp->mHeight; i ++)
        {
            for(a=0; a < bmp->mWidth; a++)
            {
            if(((bmp->mData[bmpIdx/8] << bit) & mask))
                gfx->fWritePixel(gfx, x+a, y+i, &gfx->mPen.mColor);
            bmpIdx ++;
            bit++;
            if(bit == 8)
                bit =0;
            }

        }
    }

    //TODO implement bitmap drawing for other color modes

    return MRT_STATUS_OK;
}

gfx_rect_t gfx_get_print_size(gfx_t* gfx, const char* text)
{

    gfx_rect_t ret; 
    ret.mX=0;
    ret.mY=0;
    ret.mWidth =0; 
    ret.mHeight =0;

    if(gfx->mFont == NULL)
    {
        return ret;
    }

    char c = *text++;   //grab first character from string
    int lineCount =1; 
    GFXglyph* glyph;    //pointer to glyph for current character

    int xx;
    int maxX =0;
    
    while(c != 0)
    {
        if(c == '\n')
        {
            lineCount++;
            xx = 0;
        }
        else if((c >= gfx->mFont->mFirst) && (c <= gfx->mFont->mLast))// make sure the font contains this character
        {

            xx += glyph->mXOffset + glyph->mXAdvance;

            if(xx > maxX)
            {
                maxX = xx;
            }

        }

        //get next character
        c = *text++;
    }

    if(maxX > 0)
    {
        ret.mWidth = maxX; 
        ret.mHeight = lineCount * gfx->mFont->mYAdvance;
    }

    return ret; 
}


mrt_status_t gfx_print(gfx_t* gfx, int x, int y, const char * text)
{
    //if a font has not been set, return error
  if(gfx->mFont == NULL)
    return MRT_STATUS_ERROR;


  int xx =x;     //current position for writing
  int yy = y;
  GFXglyph* glyph;    //pointer to glyph for current character
  GFXBmp bmp;         //bitmap struct used to draw glyph
  char c = *text++;   //grab first character from string

  //run until we hit a null character (end of string)
  while(c != 0)
  {
    if(c == '\n')
    {
      //if character is newline, we advance the y, and reset x
      yy+= gfx->mFont->mYAdvance;
      xx = x;
    }
    else if((c >= gfx->mFont->mFirst) && (c <= gfx->mFont->mLast))// make sure the font contains this character
    {
      //grab the glyph for current character from our font
      glyph = &gfx->mFont->mGlyph[c - gfx->mFont->mFirst]; //index in glyph array is offset by first printable char in font

      //map glyph to a bitmap that we can draw
      bmp.mData = &gfx->mFont->mBitmap[glyph->mOffset];
      bmp.mWidth = glyph->mWidth ;
      bmp.mHeight = glyph->mHeight ;
      bmp.mMode = GFX_COLOR_MODE_MONO; //Font glyphs are all stored as monochromatic bitmaps

      //draw the character
      gfx_draw_bmp(gfx, xx+glyph->mXOffset , yy+ glyph->mYOffset , &bmp );
      xx += glyph->mXOffset + glyph->mXAdvance;
    }


    //get next character
    c = *text++;
  }
    return MRT_STATUS_OK;
}

mrt_status_t gfx_draw_line(gfx_t* gfx, int x0, int y0, int x1, int y1)
{
    int16_t steep = abs(y1 - y0) > abs(x1 - x0);
    if (steep) {
        _swap_int(x0, y0);
        _swap_int(x1, y1);
    }

    if (x0 > x1) {
        _swap_int(x0, x1);
        _swap_int(y0, y1);
    }

    int16_t dx, dy;
    dx = x1 - x0;
    dy = abs(y1 - y0);

    int16_t err = dx / 2;
    int16_t ystep;

    if (y0 < y1) {
        ystep = 1;
    } else {
        ystep = -1;
    }

    for (; x0<=x1; x0++) {
        if (steep) {
            gfx->fWritePixel(gfx,y0, x0, &gfx->mPen.mColor);
        } else {
            gfx->fWritePixel(gfx, x0, y0, &gfx->mPen.mColor);
        }
        err -= dy;
        if (err < 0) {
            y0 += ystep;
            err += dx;
        }
    }


    return MRT_STATUS_OK;
}

mrt_status_t gfx_draw_rect(gfx_t* gfx, int x, int y, int w, int h, uint32_t opt)
{

    if(opt & GFX_OPT_FILL)
    {
        for(int i=0; i < h; i++)
        {
            for(int a=0; a < w; a++)
            {
                gfx->fWritePixel(gfx,x+a, y+i, &gfx->mPen.mColor);
            }
        }
    }
    else 
    {
        //TODO
    }
    return MRT_STATUS_OK;
}

mrt_status_t gfx_draw_circle(gfx_t* gfx, int x, int y, int r, uint32_t opt)
{

    //TODO
    return MRT_STATUS_OK;
}

mrt_status_t gfx_fill(gfx_t* gfx, gfx_color_t val)
{
    gfx_convert_color(&val, gfx->mMode);
    for(int y = 0; y < gfx->mHeight; y++)
    {
        for(int x = 0; x < gfx->mWidth; x++)
        {
            gfx_write_pixel(gfx,x,y,&val);
        }
    }
    
    return MRT_STATUS_OK;
}

mrt_status_t gfx_test_pattern(gfx_t* gfx)
{
    uint32_t grid_size = (gfx->mHeight / 8) ;
    uint32_t bar_width = (gfx->mWidth / 14) + 1;
    uint32_t bar_len = gfx->mHeight * 0.3;  
    uint32_t x_cursor = grid_size; 
    uint32_t y_cursor = grid_size;

    gfx_color_t colors[] = {GFX_COLOR_WHITE, GFX_COLOR_YELLOW, GFX_COLOR_CYAN, GFX_COLOR_GREEN, GFX_COLOR_FUSIA, GFX_COLOR_RED, GFX_COLOR_BLUE};

    //Grid 
    gfx_fill(gfx, GFX_COLOR_BLACK);
    gfx_set_pen(gfx, 1, GFX_COLOR_WHITE);
    gfx_draw_line(gfx,0,0,0,gfx->mHeight -1);
    gfx_draw_line(gfx,0,0,gfx->mWidth-1,0);
    gfx_draw_line(gfx,0,gfx->mHeight -1, gfx->mWidth -1,gfx->mHeight -1);
    gfx_draw_line(gfx,gfx->mWidth-1, 0, gfx->mWidth - 1,gfx->mHeight -1);
    while(y_cursor < gfx->mHeight)
    {
        gfx_draw_line(gfx,0,y_cursor,gfx->mWidth,y_cursor);
        y_cursor+= grid_size;
    }

    while(x_cursor < gfx->mWidth)
    {
        gfx_draw_line(gfx,x_cursor,0,x_cursor,gfx->mHeight);
        x_cursor+= grid_size;
    }

    //Color Panels

    x_cursor = (gfx->mWidth - (bar_width* 7)) /2; 
    y_cursor = gfx->mHeight * 0.3; 
    for(int i=0; i < 7; i++)
    {
        gfx_set_pen(gfx,1,colors[i]);
        gfx_draw_rect(gfx,x_cursor,y_cursor,bar_width,bar_len,true);
        x_cursor+= bar_width;
    }

    y_cursor += bar_len;
    x_cursor = (gfx->mWidth - (bar_width* 7)) /2; 
    bar_len = gfx->mHeight *0.1; 

    for(int i=0; i < 7; i++)
    {
        gfx_set_pen(gfx,1,colors[6-i]);

        gfx_draw_rect(gfx,x_cursor,y_cursor,bar_width,bar_len,true);
        
        x_cursor+= bar_width;
    }

    //Write 
    if(gfx->mFont != NULL)
    {
        gfx_rect_t bound = gfx_get_print_size(gfx, "Test");
        gfx_set_pen(gfx, 1, GFX_COLOR_BLACK); 
        bound.mX = (gfx->mWidth - bound.mWidth) / 2;
        bound.mY = (gfx->mHeight - bound.mHeight) / 2; 

        //gfx_draw_rect(gfx,bound.mX, bound.mY, bound.mWidth, bound.mHeight, true);
        gfx_set_pen(gfx,1,GFX_COLOR_WHITE); 
        gfx_print(gfx, bound.mX, bound.mY, "Test");
    }


    return MRT_STATUS_OK;
}

