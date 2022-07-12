/*
 * TFTLIB_8BIT.h
 *
 *  Created on: Jan 10, 2022
 *  Updated on: Feb 20, 2022
 *      Author: asz
 */

#ifndef INC_TFTLIB_8BIT_H_
#define INC_TFTLIB_8BIT_H_

#pragma GCC push_options
#pragma GCC optimize ("Ofast")

#include "stm32f4xx_hal.h"
#include "algorithm"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <Print.h>
#include "GFXFF/gfxfont.h"

enum COLORS{
	WHITE		= 0xFFFF,
	BLACK		= 0x0000,
	GRAY		= 0X8430,
	LIGHTGREY	= 0xD69A,
	DARKGREY	= 0x7BEF,
	RED			= 0xF800,
	GREEN		= 0x07E0,
	DARKGREEN	= 0x03E0,
	BLUE		= 0x001F,
	LIGHTBLUE	= 0X7D7C,
	DARKBLUE	= 0X01CF,
	SKYBLUE		= 0x867D,
	YELLOW		= 0xFFE0,
	PURPLE		= 0x780F,
	MAGENTA		= 0xF81F,
	VIOLET		= 0x915C,
	CYAN		= 0x7FFF,
	DARKCYAN	= 0x03EF,
	ORANGE		= 0xFB20,
	PINK		= 0xFE19,
	BROWN		= 0x9A60,
	GOLD		= 0xFEA0,
	SILVER		= 0xC618,
	BRED		= 0XF81F,
	GRED		= 0XFFE0,
	GBLUE		= 0X07FF,
	BRRED		= 0XFC07,
	GRAYBLUE	= 0X5458,
	LGRAY		= 0XC618,
	LGRAYBLUE	= 0XA651,
	LBBLUE		= 0X2B12,
	NAVY		= 0x000F,
	MAROON		= 0x7800,
	OLIVE		= 0x7BE0,
	GREENYELLOW	= 0xB7E0,

	DKBLUE		= 0x000D,
	DKTEAL		= 0x020C,
	DKGREEN		= 0x03E0,
	DKCYAN		= 0x03EF,
	DKRED		= 0x6000,
	DKMAGENTA	= 0x8008,
	DKYELLOW	= 0x8400,
	DKORANGE 	= 0x8200,
	DKPINK		= 0x9009,
	DKPURPLE	= 0x4010,
	DKGREY		= 0x4A49,
};

enum DATUM {
	TL_DATUM = 0, // Top left (default)
	TC_DATUM = 1, // Top centre
	TR_DATUM = 2, // Top right
	ML_DATUM = 3, // Middle left
	CL_DATUM = 3, // Centre left, same as above
	MC_DATUM = 4, // Middle centre
	CC_DATUM = 4, // Centre centre, same as above
	MR_DATUM = 5, // Middle right
	CR_DATUM = 5, // Centre right, same as above
	BL_DATUM = 6, // Bottom left
	BC_DATUM = 7, // Bottom centre
	BR_DATUM = 8, // Bottom right
	L_BASELINE = 9, // Left character baseline (Line the 'A' character would sit on)
	C_BASELINE = 10, // Centre character baseline
	R_BASELINE = 11, // Right character baseline
};

/* Control Registers and constant codes */
#define SWRESET			0x01
#define CASET			0x2A
#define RASET			0x2B
#define RAMWR			0x2C
#define RAMRD			0x2E

#define SLPIN			0x10
#define SLPOUT			0x11
#define DISPOFF			0x28
#define DISPON			0x29
#define FRMCTR1			0xB1
#define FRMCTR2			0xB2
#define PWCTR1			0xC0
#define PWCTR2			0xC1
#define PWCTR3			0xC2
#define PWCTR4			0xC3
#define PWCTR5			0xC4
#define VMCTR1			0xC5
#define VMCTR2			0xC7

#define COLMOD			0x3A
#define COLOR_MODE_16bit 0x55
#define COLOR_MODE_18bit 0x66

#define MADCTL			0x36
#define MADCTL_MH		0x04
#define MADCTL_ML		0x10
#define MADCTL_MV		0x20
#define MADCTL_MX		0x40
#define MADCTL_MY		0x80
#define MADCTL_RGB		0x00
#define MADCTL_BGR		0x08

#define Byte8H(ByteH) ((uint8_t)(((uint16_t)(ByteH)&0xFF00)>>8))
#define Byte8L(ByteL) ((uint8_t)( (uint16_t)(ByteL)&0x00FF))

/* Basic operations */
template <typename T> static inline void
swap_coord(T& a, T& b) { T t = a; a = b; b = t;}

extern "C" char* sbrk(int incr);

constexpr float PixelAlphaGain   = 255.0;
constexpr float LoAlphaTheshold  = 64.0/255.0;
constexpr float HiAlphaTheshold  = 1.0 - LoAlphaTheshold;

inline GFXglyph *pgm_read_glyph_ptr(const GFXfont *gfxFont, uint8_t c) {
	return gfxFont->glyph + c;
}

inline uint8_t *pgm_read_bitmap_ptr(const GFXfont *gfxFont) {
	return gfxFont->bitmap;
}

enum LCD_DRIVER {
	ILI9341_PARALLEL	= 0x01,
	ILI9327_PARALLEL	= 0x02,
	NT35510_PARALLEL	= 0x04,
};

class TFTLIB_8BIT : public Print { friend class TFT_eSprite;
	private:
		uint8_t _rotation;
		int32_t _tx0, _tx1, _ty0, _ty1;
		GPIO_TypeDef* CS_PORT;
		GPIO_TypeDef* DC_PORT;
		GPIO_TypeDef* WR_PORT;
		GPIO_TypeDef* RD_PORT;
		GPIO_TypeDef* _PARALLEL_PORT;
		uint16_t CS_PIN;
		uint16_t DC_PIN;
		uint16_t WR_PIN;
		uint16_t RD_PIN;
		uint8_t _type;
		uint16_t _buffer_size = 1024;
		uint16_t *_buffer = new uint16_t[_buffer_size];
		GFXfont  *gfxFont;
		uint16_t _text_fg = RED, _text_bg = BLACK;
		uint16_t SWAP_UINT16(uint16_t x) {x = (x >> 8) | (x << 8); return x;}

		uint8_t  decoderState = 0;   // UTF8 decoder state        - not for user access
		uint16_t decoderBuffer;      // Unicode code-point buffer - not for user access

		uint8_t glyph_ab,   // Smooth font glyph delta Y (height) above baseline
				glyph_bb;   // Smooth font glyph delta Y (height) below baseline

		bool     _utf8;
		bool     isDigits;   // adjust bounding box for numbers to reduce visual jiggling
		bool     textwrapX, textwrapY;  // If set, 'wrap' text at right and optionally bottom edge of display

		int32_t  cursor_x = 0, cursor_y = 0, padX = 0;       // Text cursor x,y and padding setting
		uint32_t textcolor, textbgcolor;         // Text foreground and background colours

		uint8_t	textfont  = 1,  // Current selected font number
				textsize  = 1,  // Current font size multiplier
				textdatum = TL_DATUM; // Text reference datum

		int32_t _display_width  = 480;
		int32_t _display_height = 800;
		int32_t _width  = 480;
		int32_t _height = 800;

		int16_t textWidth(const char *string);
		int16_t textWidth(const char *string, uint8_t font);
		int16_t fontHeight(void);
		int16_t fontHeight(int16_t font);
		uint16_t decodeUTF8(uint8_t c);
		uint16_t decodeUTF8(uint8_t *buf, uint16_t *index, uint16_t remaining);

		void CS_L(void) { CS_PORT->BSRR = CS_PIN << 16U; }
		void CS_H(void) {  CS_PORT->BSRR = CS_PIN; }

		void DC_L(void) {  DC_PORT->BSRR = DC_PIN << 16U; }
		void DC_H(void) {  DC_PORT->BSRR = DC_PIN; }

		void WR_L(void) {  WR_PORT->BSRR = WR_PIN << 16U; }
		void WR_H(void) {  WR_PORT->BSRR = WR_PIN; }

		void RD_L(void) {  RD_PORT->BSRR = RD_PIN << 16U; }
		void RD_H(void) {  RD_PORT->BSRR = RD_PIN; }

		void WR_STROBE(void) { WR_L(); WR_H(); }
		void RD_STROBE(void) { RD_L(); asm("NOP"); asm("NOP"); asm("NOP"); }
		void RD_IDLE(void)	 { RD_H(); asm("NOP"); }

	public:
		TFTLIB_8BIT(GPIO_TypeDef *parallel_bus, uint8_t drv, GPIO_TypeDef *GPIO_RD_PORT, uint16_t GPIO_RD_PIN, GPIO_TypeDef *GPIO_WR_PORT, uint16_t GPIO_WR_PIN, GPIO_TypeDef *GPIO_DC_PORT, uint16_t GPIO_DC_PIN, GPIO_TypeDef *GPIO_CS_PORT, uint16_t GPIO_CS_PIN);
		~TFTLIB_8BIT();

		uint8_t readByte(void);

		inline void write8(uint8_t data) {	_PARALLEL_PORT->BSRR = (0x00FF0000 | (uint8_t)(data)); WR_STROBE(); }
		inline void write16(uint16_t data) { write8(data >> 8); write8(data); }

		void writeCommand8 (uint8_t);
		void writeCommand16 (uint16_t);
		void writeData8(uint8_t *data, uint32_t len);
		void writeData16(uint16_t *data, uint32_t len);
		inline void writeSmallData8 (uint8_t);
		inline void writeSmallData16 (uint16_t);
		inline void writeSmallData32 (uint32_t);

		void setWindow8(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
		void readWindow8(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
		void pushPixels16(const void* data_in, uint32_t len);
		void pushBlock16(uint16_t color, uint32_t len);

		void init(void);
		void setRotation(uint8_t m);
		uint16_t readID(void);
		uint16_t width(void);
		uint16_t height(void);

		void fillScreen(uint16_t color);

		uint16_t	readPixel(int32_t x0, int32_t y0);
		void		drawPixel(int32_t x, int32_t y, uint16_t color);
		uint16_t	drawPixel(int32_t x, int32_t y, uint32_t color, uint8_t alpha, uint32_t bg_color);
		void		drawPixelAlpha(int16_t x, int16_t y, uint16_t color, float alpha);
		void		drawSpot(float ax, float ay, float r, uint32_t color);
		void		fillSmoothCircle(int32_t x, int32_t y, int32_t r, uint32_t color, uint32_t bg_color = 0x00FFFFFF);
		void		fillSmoothRoundRect(int32_t x, int32_t y, int32_t w, int32_t h, int32_t radius, uint32_t color, uint32_t bg_color = 0x00FFFFFF);

		inline float wedgeLineDistance(float xpax, float ypay, float bax, float bay, float dr);
		inline void drawCircleHelper( int32_t x0, int32_t y0, int32_t rr, uint8_t cornername, uint16_t color);
		inline void fillCircleHelper(int32_t x0, int32_t y0, int32_t r, uint8_t cornername, int32_t delta, uint16_t color);
		inline void fillCircleHelperAA(int32_t x0, int32_t y0, int32_t r, uint8_t cornername, int32_t delta, uint16_t color);
		inline void fillAlphaCircleHelper(int32_t x0, int32_t y0, int32_t r, uint8_t cornername, int32_t delta, uint16_t color, uint8_t alpha);

		void drawFastHLine(int32_t x, int32_t y, int32_t w, uint16_t color);
		void drawHLineAlpha(int32_t x, int32_t y, int32_t w, uint16_t color, uint8_t alpha);
		void drawFastVLine(int32_t x, int32_t y, int32_t w, uint16_t color);
		void drawVLineAlpha(int32_t x, int32_t y, int32_t h, uint16_t color, uint8_t alpha);
		void drawLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint16_t color);

		void drawWideLine(float ax, float ay, float bx, float by, float wd, uint16_t fg_color, uint16_t bg_color = 0xFFFF);
		void drawWedgeLine(float ax, float ay, float bx, float by, float ar, float br, uint32_t fg_color, uint32_t bg_color = 0x00FFFFFF);

		void drawTriangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, uint16_t color);
		void drawTriangleAA(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, int32_t thickness, uint16_t color);

		void drawRect(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t color);
		void drawRectAA(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t color);

		void drawRoundRect(int32_t x, int32_t y, int32_t w, int32_t h, int32_t r, uint16_t color);

		void drawCircle(int32_t x0, int32_t y0, int32_t r, uint16_t color);

		void drawEllipse(int16_t x0, int16_t y0, int32_t rx, int32_t ry, uint16_t color);

		/* Text functions. */
		uint8_t getTextDatum(void);
		void getCursor(int16_t* x, int16_t* y);
		uint16_t getTextPadding(void);

		void setCursor(int16_t x, int16_t y);
		void setTextColor(uint16_t c);
		void setTextColor(uint16_t c, uint16_t b);
		void setTextWrap(bool wrapX, bool wrapY = false);
		void setTextSize(uint8_t s);
		void setFreeFont(const GFXfont *f);
		void setTextDatum(uint8_t d);
		void setTextPadding(uint16_t x_width);

		size_t write(uint8_t utf8);
		int16_t drawChar(uint16_t uniCode, int32_t x, int32_t y);
		void drawChar(int32_t x, int32_t y, uint16_t c, uint32_t color, uint32_t bg, uint8_t size);

		int16_t drawString(const char *string, int32_t poX, int32_t poY);
		int16_t drawCentreString(const char *string, int32_t dX, int32_t poY);
		int16_t drawRightString(const char *string, int32_t dX, int32_t poY);

		int16_t drawNumber(long long_num, int32_t poX, int32_t poY);
		int16_t drawFloat(float floatNumber, uint8_t dp, int32_t poX, int32_t poY);

		/* Extented Graphical functions. */
		void fillTriangle(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint16_t color);
		void fillTriangleAA( int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint16_t color);
		void fillTriangleAlpha(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint16_t color, uint8_t alpha);

		void fillRect(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t color);
		void fillRectAA(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t color);
		void fillRectAlpha(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t color, uint8_t alpha);

		void fillRoundRect(int32_t x, int32_t y, int32_t w, int32_t h, int32_t r, uint16_t color);
		void fillRoundRectAA(int32_t x, int32_t y, int32_t w, int32_t h, int32_t r, uint16_t color);
		void fillAlphaRoundRect(int32_t x, int32_t y, int32_t w, int32_t h, int32_t r, uint16_t color, uint8_t alpha);

		void fillCircle(int32_t x0, int32_t y0, int32_t r, uint16_t color);
		void fillCircleAA(float x, float y, float r, uint16_t color);
		void fillCircleAlpha(int32_t x0, int32_t y0, int32_t r, uint16_t color, uint8_t alpha);

		void fillEllipse(int16_t x0, int16_t y0, int32_t rx, int32_t ry, uint16_t color);

		void drawImage(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t *data);
		void drawBitmap(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t *bitmap, uint16_t color);
		void drawBitmap(int16_t x, int16_t y, int16_t w, int16_t h, const uint8_t *bitmap, uint16_t color);
		void drawBitmap(int16_t x, int16_t y, int16_t w, int16_t h, const uint8_t *bitmap, uint16_t fgcolor, uint16_t bgcolor);
        void drawXBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t fgcolor);
        void drawXBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t fgcolor, uint16_t bgcolor);

		uint16_t color565(uint8_t r, uint8_t g, uint8_t b);
		uint16_t color16to8(uint16_t c);
		uint16_t color8to16(uint8_t color);
		uint16_t alphaBlend(uint8_t alpha, uint16_t fgc, uint16_t bgc);

		uint32_t testFillScreen();
		uint32_t testText();
		uint32_t testLines(uint16_t color);
		uint32_t testFastLines(uint16_t color1, uint16_t color2);
		uint32_t testRects(uint16_t color);
		uint32_t testFilledRects(uint16_t color1, uint16_t color2);
		uint32_t testFilledCircles(uint8_t radius, uint16_t color);
		uint32_t testCircles(uint8_t radius, uint16_t color);
		uint32_t testTriangles();
		uint32_t testFilledTriangles();
		uint32_t testRoundRects();
		uint32_t testFilledRoundRects();
		void benchmark(void);
		void cpuConfig(void);
		int FreeRAM();
};

#endif
#pragma GCC pop_options

//#endif
