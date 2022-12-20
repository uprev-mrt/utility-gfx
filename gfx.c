/**
  *@file gfx.c
  *@brief module for  graphics
  *@author Jason Berger
  *@date 010/15/2022
  */

#include "gfx.h"
#include "string.h"
#include <stdlib.h>

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
        case GFX_COLOR_MODE_888A:          //24 bit color mode 
            gfx->mPixelSize = 32;
            break;
    }

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
        case GFX_COLOR_MODE_888A:          //24 bit color mode 
            gfx->mPixelSize = 32;
            break;
    }

    gfx->mBufferSize = ((width * height) * (gfx->mPixelSize)) / 8;
    gfx->mBuffer = NULL;
    gfx->mWidth = width;
    gfx->mHeight = height;
    gfx->mFont  = NULL;
    gfx->fWritePixel = write_cb;
    gfx->fWriteBuffer = &gfx_write_buffer;
    gfx->mDevice  = dev;
    gfx->mBuffered = true;

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

mrt_status_t gfx_write_pixel(gfx_t* gfx, int x, int y, gfx_color_t val)
{
    //If we are out of bounds, ignore
    if(( x < 0) || (x >= gfx->mWidth) || (y < 0) || (y>= gfx->mHeight))
    return MRT_STATUS_OK;

    uint32_t cursor = (y * gfx->mWidth) + x;
    uint32_t byteOffset = (cursor  / 8);
    uint8_t mask = 0x80;

    
    if(gfx->mMode == GFX_COLOR_MODE_MONO)
    {        
        //get number of bits off of alignment in case we are not writing on a byte boundary
        uint8_t bitOffset = cursor % 8;
        mask = mask >> bitOffset;

        if( val.mData.mMonoData.on == 0)
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
        memcpy(&gfx->mBuffer[byteOffset],&val.mData, (gfx->mPixelSize / 8));
    }

    return MRT_STATUS_OK;
}

mrt_status_t gfx_write_buffer(gfx_t* gfx, int x, int y, uint8_t* data, int len, bool wrap)
{
    return MRT_STATUS_NOT_IMPLEMENTED;
}

mrt_status_t gfx_refresh(gfx_t* gfx)
{
    return gfx->fWriteBuffer(gfx, 0,0,gfx->mBuffer, gfx->mBufferSize, true);
}

mrt_status_t gfx_draw_bmp(gfx_t* gfx, int x, int y,const GFXBmp* bmp, gfx_color_t val)
{
    uint32_t bmpIdx = 0;
    uint8_t mask =0x80;
    int bit =0;
    int i,a;

    if( gfx->mMode == GFX_COLOR_MODE_MONO)
    {
        for(i=0; i < bmp->mHeight; i ++)
        {
            for(a=0; a < bmp->mWidth; a++)
            {
            if(((bmp->mData[bmpIdx/8] << bit) & mask))
                gfx->fWritePixel(gfx, x+a, y+i, val);
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

mrt_status_t gfx_print(gfx_t* gfx, int x, int y, const char * text, gfx_color_t val)
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
      gfx_draw_bmp(gfx, xx+glyph->mXOffset , yy+ glyph->mYOffset , &bmp,val );
      xx += glyph->mXOffset + glyph->mXAdvance;
    }


    //get next character
    c = *text++;
  }
    return MRT_STATUS_OK;
}

mrt_status_t gfx_draw_line(gfx_t* gfx, int x0, int y0, int x1, int y1, gfx_color_t val)
{
    int16_t steep = abs(y1 - y0) > abs(x1 - x0);
    int swap;
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
            gfx->fWritePixel(gfx,y0, x0, val);
        } else {
            gfx->fWritePixel(gfx, x0, y0, val);
        }
        err -= dy;
        if (err < 0) {
            y0 += ystep;
            err += dx;
        }
    }
    return MRT_STATUS_OK;
}

mrt_status_t gfx_draw_rect(gfx_t* gfx, int x, int y, int w, int h, gfx_color_t val)
{
    for(int i=0; i < h; i++)
    {
        for(int a=0; a < w; a++)
        {
            gfx->fWritePixel(gfx,x+a, y+i, val);
        }
    }
    return MRT_STATUS_OK;
}

mrt_status_t gfx_fill(gfx_t* gfx, gfx_color_t val)
{
    if(gfx->mBuffered)
    {
        //memset(gfx->mBuffer, val, gfx->mBufferSize);
    }
    else
    {
        //TODO
    }
    return MRT_STATUS_OK;
}
