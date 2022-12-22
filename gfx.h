/**
  *@file color_gfx.h
  *@brief module for handling graphics canvas
  *@author Jason Berger
  *@date 03/15/2019
  */
#pragma once

/* Includes ------------------------------------------------------------------*/
#include "Platforms/Common/mrt_platform.h"
#include "gfx_colors.h"

/* Exported macro ------------------------------------------------------------*/
#define GFX_PIXEL_OFF 0
#define GFX_PIXEL_ON 1
#define GFX_PIXEL_INVERT 2

/* Exported types ------------------------------------------------------------*/

struct gfx_struct;

typedef enum{
  GFX_COLOR_MODE_MONO,          //Monochromatic color mode
  GFX_COLOR_MODE_565,           //16bit color mode using 565 format
  GFX_COLOR_MODE_888,           //24 bit color mode 
  GFX_COLOR_MODE_888A,          //24 bit color mode with alpha channel
  GFX_COLOR_MODE_A888          //24 bit color mode with alpha channel
}gfx_color_mode_e;

/**
 * @brief struct for pixel gfx_color a.k.a pixel value
 * @note use of unions allows it to be cast as uint32
 */
typedef struct{
  union{
    //structure for 24bit color data with optional alpha channel
    struct{       
      uint8_t r;
      uint8_t g;
      uint8_t b;
      uint8_t alpha;
    } mRGBAdata;

    struct{       
      uint8_t r;
      uint8_t g;
      uint8_t b;
      uint8_t alpha;
    } mARGBdata;

    //structure for 24bit
    struct{       
      uint8_t r;
      uint8_t g;
      uint8_t b;
    } mRGBdata;

    //structure for 16bit 565 data
    struct{
      uint8_t r : 5;
      uint8_t g : 6;
      uint8_t b : 5;
    } m565data;

    struct {
      uint8_t on: 1;
    } mMonoData;

    uint32_t raw; //raw 32bit value
  } mData;
  gfx_color_mode_e mMode;
}gfx_color_t;



typedef mrt_status_t (*f_gfx_write_pixel)(struct gfx_struct* gfx, int x, int y, gfx_color_t color);           
typedef mrt_status_t (*f_gfx_write)(struct gfx_struct* gfx, int x, int y, uint8_t* data, int len, bool wrap); //pointer to write function
typedef mrt_status_t (*f_gfx_read)(struct gfx_struct* gfx, int x, int y, uint8_t* data, int len, bool wrap); //pointer to read function

/**
 * @brief Color bitmap struct used to store and display images
 */
typedef struct{
	const uint8_t* mData;         //Data for bitmap
	int mWidth;                   //Width (in pixels)
	int mHeight;                  //Hieght (in pixels)
  gfx_color_mode_e mMode;       //Color mode of data
}GFXBmp;

/**
 * @brief glyphs are monochromatic bitmaps representing a single character in a font 
 */
typedef struct {                // Data stored PER GLYPH
	int mOffset;             // Pointer into GFXfont->bitmap
	uint8_t  mWidth, mHeight;     // Bitmap dimensions in pixels
	uint8_t  mXAdvance;           // Distance to advance cursor (x axis)
	int8_t   mXOffset, mYOffset;  // Dist from cursor pos to UL corner
} GFXglyph;

/**
 * @brief Font data
 */
typedef struct { // Data stored for FONT AS A WHOLE:
	uint8_t  *mBitmap;        // Glyph bitmaps, concatenated
	GFXglyph *mGlyph;         // Glyph array
	uint8_t   mFirst, mLast;  // ASCII extents
	uint8_t   mYAdvance;      // Newline distance (y axis)
} GFXfont;

typedef struct gfx_struct{
  uint8_t* mBuffer;						      //buffer to store pixel data
  int mWidth;						            // width of buffer in pixels
  int mHeight;							        //height of buffer in pixels
  uint32_t mBufferSize;					    //size of buffer (in bytes)
  uint8_t mPixelSize;               //pixel size in bits
  const GFXfont* mFont;       	    //font to use for printing
  f_gfx_write_pixel fWritePixel;    //pointer to write function
  f_gfx_write fWriteBuffer;         //pointer to write buffer function
  gfx_color_mode_e mMode;           //Color mode of canvas
  void* mDevice;					          //void pointer to device for unbuffered implementation. Use null if not needed
  bool mBuffered;                   //Indicates if memory is buffered
} gfx_t;

#ifdef __cplusplus
extern "C"
{
#endif

/* Exported constants --------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

/**
  *@brief initializes a gfx_t that manages its own buffer
  *@param gfx ptr to gfx_t to be initialized
  *@param width width (in pixels) of display buffer
  *@param height height (in pixels) of display buffer
  *@param mode color mode of canvas
  *@return status
  */
mrt_status_t gfx_init_buffered(gfx_t* gfx, int width, int height, gfx_color_mode_e mode);

/**
  *@brief initializes a gfx_t that does not manage its own buffer. This is used for large displays where storing the buffer locally doesnt make sense
  *@param gfx ptr to gfx_t to be initialized
	*@param width width (in pixels) of display buffer
  *@param height height (in pixels) of display buffer
  *@param mode color mode of canvas
	*@param write_cb call back to function to write pixels
	*@param dev void ptr to device (set NULL if not needed in write handler )
  *@return status
  */
mrt_status_t gfx_init_unbuffered(gfx_t* gfx, int width, int height, gfx_color_mode_e mode, f_gfx_write_pixel write_cb, void* dev );

/**
  *@brief frees deinitializes gfx object and frees buffer
  *@param gfx ptr to graphics object
  */
mrt_status_t gfx_deinit(gfx_t* gfx);

/**
  *@brief writes a single pixel on the canvas
  *@param gfx ptr to gfx object
  *@param x x coord to draw
  *@param y y coord to draw
  *@param val pixel value
  *@return status
  */
mrt_status_t gfx_write_pixel(gfx_t* gfx, int x, int y, gfx_color_t val);

/**
  *@brief writes an array of bytes to the buffer
  *@param gfx ptr to gfx_t descriptor
  *@param data ptr to black data being written
  *@param len number of bytes being written
  *@param wrap whether or not to wrap when we reach the end of current row
  *@return status of operation
  */
mrt_status_t gfx_write_buffer(gfx_t* gfx, int x, int y, uint8_t* data, int len, bool wrap);

/**
  *@brief writes buffer to device using fWriteBuffer
  *@param gfx ptr to gfx_t descriptor
  *@return status of operation
  */
mrt_status_t gfx_refresh(gfx_t* gfx);

/**
  *@brief Draws a bitmap to the buffer
  *@param gfx ptr to gfx_t descriptor
  *@param x x coord to begin drawing at
  *@param y y coord to begin drawing at
  *@param bmp bitmap to draw
  *@param val pixel value on
  *@return status of operation
  */
mrt_status_t gfx_draw_bmp(gfx_t* gfx, int x, int y,const GFXBmp* bmp, gfx_color_t val);

/**
  *@brief Draws a glyph
  *@param gfx ptr to gfx_t descriptor
  *@param x x coord to begin drawing at
  *@param y y coord to begin drawing at
  *@param bmp bitmap to draw
  *@param val pixel value on
  *@return status of operation
  */
mrt_status_t gfx_draw_bmp(gfx_t* gfx, int x, int y,const GFXBmp* bmp, gfx_color_t val);

/**
  *@brief Draws rendered text to the buffer
  *@param gfx ptr to gfx_t descriptor
  *@param x x coord to begin drawing at
  *@param y y coord to begin drawing at
  *@param text text to be written
  *@param val pixel value
  *@return status of operation
  */
mrt_status_t gfx_print(gfx_t* gfx, int x, int y, const char * text, gfx_color_t val);

/**
  *@brief draws a rectangle
  *@param gfx ptr to gfx canvas
	*@param x0 x coord of p1
  *@param y0 y coord of p1
	*@param x1 x coord of p2
  *@param y1 y coord of p2
  *@param val pixel value
  *@return "Return of the function"
  */
mrt_status_t gfx_draw_line(gfx_t* gfx, int x0, int y0, int x1, int y1, gfx_color_t val);

/**
  *@brief draws a rectangle
  *@param gfx ptr to gfx canvas
	*@param x x coord to begin drawing at
  *@param y y coord to begin drawing at
	*@param w width
  *@param h height
  *@param stroke pen thickness
  *@param fill fill in rectangle
  *@param val pixel value
  *@return "Return of the function"
  */
mrt_status_t gfx_draw_rect(gfx_t* gfx, int x, int y, int w, int h, int stroke, bool fill, gfx_color_t val);

/**
  *@brief fill buffer with value
  *@param gfx ptr to gfxice
  *@param val value to write
  *@return status of operation
  */
mrt_status_t gfx_fill(gfx_t* gfx, gfx_color_t val);


/**
  *@brief Draw color test pattern
  *@param gfx ptr to gfx 
  *@return status of operation
  */
mrt_status_t gfx_test_pattern(gfx_t* gfx);

#ifdef __cplusplus
}
#endif
