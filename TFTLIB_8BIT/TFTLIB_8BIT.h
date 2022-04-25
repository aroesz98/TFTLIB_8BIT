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
#include "fonts.h"

/**
 *Color of pen
 *If you want to use another color, you can choose one in RGB565 format.
 */
#define WHITE			0xFFFF
#define BLACK			0x0000
#define GRAY			0X8430
#define LIGHTGREY		0xD69A
#define DARKGREY		0x7BEF
#define RED				0xF800
#define GREEN			0x07E0
#define DARKGREEN   	0x03E0
#define BLUE			0x001F
#define LIGHTBLUE		0X7D7C
#define DARKBLUE		0X01CF
#define SKYBLUE			0x867D

#define YELLOW			0xFFE0
#define PURPLE			0x780F
#define MAGENTA			0xF81F
#define VIOLET			0x915C
#define CYAN			0x7FFF
#define DARKCYAN    	0x03EF
#define ORANGE			0xFB20

#define PINK        	0xFE19
#define BROWN       	0x9A60
#define GOLD        	0xFEA0
#define SILVER      	0xC618

/*MIXED*/
#define BRED			0XF81F
#define GRED			0XFFE0
#define GBLUE			0X07FF
#define BRRED			0XFC07
#define GRAYBLUE		0X5458
#define LGRAY			0XC618
#define LGRAYBLUE		0XA651
#define LBBLUE			0X2B12
#define NAVY        	0x000F
#define MAROON      	0x7800
#define OLIVE       	0x7BE0
#define GREENYELLOW 	0xB7E0

/* Control Registers and constant codes */
#define NOP				0x00
#define SWRESET			0x01
#define RDDID			0x04
#define RDDST			0x09

#define SLPIN			0x10
#define SLPOUT			0x11
#define PTLON			0x12
#define NORON			0x13

#define INVOFF			0x20
#define INVON			0x21
#define GAMMASET		0x26
#define DISPOFF			0x28
#define DISPON			0x29
#define CASET			0x2A
#define RASET			0x2B
#define RAMWR			0x2C
#define RAMRD			0x2E

#define COLMOD			0x3A
#define MADCTL			0x36

#define MADCTL_MH		0x04
#define MADCTL_ML		0x10
#define MADCTL_MV		0x20
#define MADCTL_MX		0x40
#define MADCTL_MY		0x80

#define MADCTL_RGB		0x00
#define MADCTL_BGR		0x08

#define FRMCTR1			0xB1
#define FRMCTR2			0xB2
#define DFUNCTR			0xB6
#define VSCRDEF			0x33
#define VSCRSADD		0x37

#define PWCTR1			0xC0
#define PWCTR2			0xC1
#define PWCTR3			0xC2
#define PWCTR4			0xC3
#define PWCTR5			0xC4
#define VMCTR1			0xC5
#define VMCTR2			0xC7

#define GMCTRP1			0xE0
#define GMCTRN1			0xE1

#define COLOR_MODE_16bit 0x55
#define COLOR_MODE_18bit 0x66

/* Basic operations */
template <typename T> static inline void
swap_coord(T& a, T& b) { T t = a; a = b; b = t;}

#define pgm_read_byte(addr) (*(const unsigned char *)(addr))

extern "C" char* sbrk(int incr);

constexpr float PixelAlphaGain   = 255.0;
constexpr float LoAlphaTheshold  = 64.0/255.0;
constexpr float HiAlphaTheshold  = 1.0 - LoAlphaTheshold;

enum class TFT_DRIVER : uint8_t
{
	ILI9327_PARALLEL	= 0x02,
	ILI9341_PARALLEL	= 0x04,
};

class TFTLIB_8BIT {
	private:
		uint8_t __rotation;
		int32_t _posx=0, _posy=0;
		int32_t __tx0 = 0, __tx1 = 0, __ty0 = 0, __ty1 = 0;
		int32_t __addr_col = 0, __addr_row = 0;
		GPIO_TypeDef* CS_PORT;
		GPIO_TypeDef* DC_PORT;
		GPIO_TypeDef* WR_PORT;
		GPIO_TypeDef* RD_PORT;
		GPIO_TypeDef* __PARALLEL_PORT;
		uint16_t CS_PIN;
		uint16_t DC_PIN;
		uint16_t WR_PIN;
		uint16_t RD_PIN;
		uint8_t _type;
		uint16_t __buffer_size = 1024;
		uint16_t *__buffer = new uint16_t[__buffer_size];
		FontDef *__font = &Font_11x18;
		uint16_t __text_fg = RED, __text_bg = BLACK;
		uint16_t SWAP_UINT16(uint16_t x) {x = (x >> 8) | (x << 8); return x;}

		int32_t _display_width  = 0;
		int32_t _display_height = 0;
		int32_t _width  = 0;
		int32_t _height = 0;

		void CS_L(void) { CS_PORT->BSRR = CS_PIN << 16U; }
		void CS_H(void) {  CS_PORT->BSRR = CS_PIN; }

		void DC_L(void) {  DC_PORT->BSRR = DC_PIN << 16U; }
		void DC_H(void) {  DC_PORT->BSRR = DC_PIN; }

		void WR_L(void) {  WR_PORT->BSRR = WR_PIN << 16U; }
		void WR_H(void) {  WR_PORT->BSRR = WR_PIN; }

		void RD_L(void) {  RD_PORT->BSRR = RD_PIN << 16U; }
		void RD_H(void) {  RD_PORT->BSRR = RD_PIN; }

		void WR_STROBE(void) { WR_L(); WR_L(); WR_L(); WR_H(); }
		void RD_STROBE(void) { RD_L(); RD_L(); RD_L(); RD_L(); RD_L(); RD_L(); }
		void RD_IDLE(void)	 { RD_H(); RD_H(); RD_H(); RD_H(); RD_H(); RD_H(); RD_H(); RD_H(); RD_H(); RD_H(); RD_H(); RD_H(); }

	public:
		TFTLIB_8BIT(GPIO_TypeDef *parallel_bus, TFT_DRIVER drv, GPIO_TypeDef *GPIO_RD_PORT, uint16_t GPIO_RD_PIN, GPIO_TypeDef *GPIO_WR_PORT, uint16_t GPIO_WR_PIN, GPIO_TypeDef *GPIO_DC_PORT, uint16_t GPIO_DC_PIN, GPIO_TypeDef *GPIO_CS_PORT, uint16_t GPIO_CS_PIN);
		~TFTLIB_8BIT();

		uint8_t readByte(void);

		inline void write8(uint8_t data) {	__PARALLEL_PORT->BSRR = (0x00FF0000 | (uint8_t)(data)); WR_STROBE(); }
		inline void write16(uint16_t data) { uint8_t h = (data)>>8, l = data; write8(h); write8(l); }

		void writeCommand8 (uint8_t);

		inline void writeData8(uint8_t *data, uint32_t len);
		inline void writeData16(uint16_t *data, uint32_t len);

		inline void writeSmallData8 (uint8_t);
		inline void writeSmallData16 (uint16_t);
		inline void writeSmallData32 (uint32_t);

		void setWindow8(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
		void readWindow8(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);

		void pushPixels16(const void* data_in, uint32_t len);
		void pushBlock16(uint16_t color, uint32_t len);

		void setRotation(uint8_t m);
		uint16_t color565(uint8_t r, uint8_t g, uint8_t b);
		uint16_t color16to8(uint16_t c);
		uint16_t color8to16(uint8_t color);
		uint16_t alphaBlend(uint8_t alpha, uint16_t fgc, uint16_t bgc);

		uint16_t readID(void);
		void init(void);

		void ARTtoggle();
		uint16_t width(void);
		uint16_t height(void);
		void fillScreen(uint16_t color);

		uint16_t readPixel(int32_t x0, int32_t y0);
		void drawPixel(int32_t x, int32_t y, uint16_t color);
		void drawPixelAlpha(int16_t x, int16_t y, uint16_t color, float alpha);

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

		void drawWideLine(float ax, float ay, float bx, float by, float wd, uint16_t fg_color);
		void drawWideLine(float ax, float ay, float bx, float by, float wd, uint16_t fg_color, uint16_t bg_color);
		void drawWedgeLine(float ax, float ay, float bx, float by, float aw, float bw, uint16_t fg_color);
		void drawWedgeLine(float ax, float ay, float bx, float by, float aw, float bw, uint16_t fg_color, uint16_t bg_color);

		void drawTriangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, uint16_t color);
		void drawTriangleAA(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, int32_t thickness, uint16_t color);

		void drawRect(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t color);
		void drawRectAA(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t color);

		void drawRoundRect(int32_t x, int32_t y, int32_t w, int32_t h, int32_t r, uint16_t color);

		void drawCircle(int32_t x0, int32_t y0, int32_t r, uint16_t color);

		void drawEllipse(int16_t x0, int16_t y0, int32_t rx, int32_t ry, uint16_t color);

		/* Text functions. */
		void setCursor(int32_t x, int32_t y);
		void setTextColor(int32_t fg, int32_t bg);
		void setFont(FontDef &Font);
		void writeChar(int32_t x, int32_t y, char ch);
		void writeString(int32_t x, int32_t y, char *str);
		void print(char *ch);
		void println(char *ch);
		void println(uint8_t *ch);

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
		void drawBitmap(int16_t x, int16_t y, int16_t w, int16_t h, const uint8_t *bitmap, uint16_t color);

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

#pragma GCC pop_options

#endif
