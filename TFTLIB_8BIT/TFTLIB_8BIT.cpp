/*
 * TFTLIB_8BIT.cpp
 *
 *  Created on: Jan 10, 2022
 *  Updated on: Feb 20, 2022
 *      Author: asz
 */


#include <TFTLIB_8BIT.h>
#include "hw_drv.h"

using namespace std;

/***************************************************************************************
** Function name:           TFTLIB_8BIT
** Description:             TFTLIB_8BIT Constructor
***************************************************************************************/
TFTLIB_8BIT::TFTLIB_8BIT(GPIO_TypeDef *parallel_bus, TFT_DRIVER drv, GPIO_TypeDef *GPIO_RD_PORT, uint16_t GPIO_RD_PIN, GPIO_TypeDef *GPIO_WR_PORT, uint16_t GPIO_WR_PIN, GPIO_TypeDef *GPIO_DC_PORT, uint16_t GPIO_DC_PIN, GPIO_TypeDef *GPIO_CS_PORT, uint16_t GPIO_CS_PIN) {
	__PARALLEL_PORT = parallel_bus;

	CS_PORT	 = GPIO_CS_PORT;
	CS_PIN	 = GPIO_CS_PIN;

	DC_PORT	 = GPIO_DC_PORT;
	DC_PIN	 = GPIO_DC_PIN;

	WR_PORT = GPIO_WR_PORT;
	WR_PIN  = GPIO_WR_PIN;

	RD_PORT = GPIO_RD_PORT;
	RD_PIN  = GPIO_RD_PIN;

	_type = (uint8_t)drv;

	PIN_OUTPUT(__PARALLEL_PORT, 0x00FF);

	RD_H();
}

/***************************************************************************************
** Function name:           ~TFTLIB_8BIT
** Description:             TFTLIB_8BIT Destructor
***************************************************************************************/
TFTLIB_8BIT::~TFTLIB_8BIT() {
	delete[] __buffer;
}

uint16_t TFTLIB_8BIT::width(void){
	return _width;
}

uint16_t TFTLIB_8BIT::height(void){
	return _height;
}

uint16_t TFTLIB_8BIT::readID(void) {
	writeCommand8(0xD3);

	PIN_INPUT(GPIOB, 0x00FF);

	readByte();
	readByte();
	uint16_t id = readByte() << 8 | readByte();

	PIN_OUTPUT(GPIOB, 0x00FF);

	return id;
}

/***************************************************************************************
** Function name:           init
** Description:             Init display with selected driver
***************************************************************************************/
void TFTLIB_8BIT::init(void){
	if(_type == (uint8_t)TFT_DRIVER::ILI9327_PARALLEL) {
		_display_width  = 240;
		_display_height = 400;
		#include "ili9327_drv.h"
	}

	else if(_type == (uint8_t)TFT_DRIVER::ILI9341_PARALLEL) {
		_display_width  = 240;
		_display_height = 320;
		#include "ili9341_parallel_drv.h"
	}

	setRotation(3);
	fillScreen(GREEN);
}

/***************************************************************************************
** Function name:           readByte
** Description:             Read 1 byte of data from display;
***************************************************************************************/
uint8_t TFTLIB_8BIT::readByte(void)
{
	uint8_t b = 0;

	DC_H();
	CS_L();
	RD_STROBE();

	b = (__PARALLEL_PORT->IDR & 0x00FF);

	RD_IDLE();
	CS_H();

	return b;
}

/***************************************************************************************
** Function name:           writeCommand8
** Description:             Writing command to parallel display
***************************************************************************************/
void TFTLIB_8BIT::writeCommand8 (uint8_t cmmd) {
	CS_L();
	DC_L();

	write8(cmmd);

	DC_H();
	CS_H();
}

/***************************************************************************************
** Function name:           writeData8
** Description:             Write 8bit data to parallel display
***************************************************************************************/
inline void TFTLIB_8BIT::writeData8(uint8_t *data, uint32_t len) {
	CS_L();
	DC_H();

	while(len-->0) write8(*data++);

	CS_H();
}

/***************************************************************************************
** Function name:           writeData16
** Description:             Write 16bit data to parallel display
***************************************************************************************/
inline void TFTLIB_8BIT::writeData16(uint16_t *data, uint32_t len) {
	CS_L();
	DC_H();

	while(len > 31) {
		write16(*data++); write16(*data++); write16(*data++); write16(*data++);
		write16(*data++); write16(*data++); write16(*data++); write16(*data++);
		write16(*data++); write16(*data++); write16(*data++); write16(*data++);
		write16(*data++); write16(*data++); write16(*data++); write16(*data++);
		write16(*data++); write16(*data++); write16(*data++); write16(*data++);
		write16(*data++); write16(*data++); write16(*data++); write16(*data++);
		write16(*data++); write16(*data++); write16(*data++); write16(*data++);
		write16(*data++); write16(*data++); write16(*data++); write16(*data++);
		len-=32;
	}

	while(len > 7) {
		write16(*data++); write16(*data++); write16(*data++); write16(*data++);
		write16(*data++); write16(*data++); write16(*data++); write16(*data++);
		len-=8;
	}

	while(len-->0) write16(*data++);
	CS_H();
}

/***************************************************************************************
** Function name:           writeSmallData8
** Description:             Write 8bit data via GPIO Port
***************************************************************************************/
inline void TFTLIB_8BIT::writeSmallData8(uint8_t data) {
	CS_L();
	DC_H();

	write8(data);

	CS_H();
}

/***************************************************************************************
** Function name:           writeSmallData16
** Description:             Write 16bit data via GPIO Port
***************************************************************************************/
inline void TFTLIB_8BIT::writeSmallData16 (uint16_t data) {
	CS_L();
	DC_H();

	write8(data >> 8);
	write8(data);

	CS_H();
}

/***************************************************************************************
** Function name:           writeSmallData32
** Description:             Write 32bit data via GPIO Port
***************************************************************************************/
inline void TFTLIB_8BIT::writeSmallData32(uint32_t data) {
	CS_L();
	DC_H();

	write8(data>>24);
	write8(data>>16);
	write8(data>>8);
	write8(data);

	CS_H();
}

/***************************************************************************************
** Function name:           setRotation
** Description:             Set the rotation direction of the display
***************************************************************************************/
void TFTLIB_8BIT::setRotation(uint8_t m)
{
	m = m % 4;
	__rotation = m;

	if(_type == (uint8_t)TFT_DRIVER::ILI9327_PARALLEL) {
		writeCommand8(MADCTL);
		switch (m) {
		case 0:
			writeSmallData8(MADCTL_MX | MADCTL_BGR);
			_width  = _display_width;
			_height = _display_height;
			break;
		case 1:
			writeSmallData8(MADCTL_MV | MADCTL_BGR);
			_width  = _display_height;
			_height = _display_width;
			break;
		case 2:
			writeSmallData8(MADCTL_MY | MADCTL_BGR);
			_width  = _display_width;
			_height = _display_height;
			break;
		case 3:
			writeSmallData8(MADCTL_MX | MADCTL_MY | MADCTL_MV | MADCTL_BGR);
			_width  = _display_height;
			_height = _display_width;
			break;
		default:
			break;
		}
	}

	else if(_type == (uint8_t)TFT_DRIVER::ILI9341_PARALLEL) {
		writeCommand8(MADCTL);
		switch (m) {
		case 0:
			writeSmallData8(MADCTL_MX | MADCTL_BGR);
			_width  = _display_width;
			_height = _display_height;
			break;
		case 1:
			writeSmallData8(MADCTL_MV | MADCTL_BGR);
			_width  = _display_height;
			_height = _display_width;
			break;
		case 2:
			writeSmallData8(MADCTL_MY | MADCTL_BGR);
			_width  = _display_width;
			_height = _display_height;
			break;
		case 3:
			writeSmallData8(MADCTL_MX | MADCTL_MY | MADCTL_MV | MADCTL_BGR);
			_width  = _display_height;
			_height = _display_width;
			break;
		default:
			break;
		}
	}
}

/***************************************************************************************
** Function name:           color565
** Description:             Convert value RGB888 to RGB565
***************************************************************************************/
uint16_t TFTLIB_8BIT::color565(uint8_t r, uint8_t g, uint8_t b) {
	uint16_t color = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
	return color;
}

/***************************************************************************************
** Function name:           color16to8
** Description:             Convert 16bit palette to 8bit palette
***************************************************************************************/
uint16_t TFTLIB_8BIT::color16to8(uint16_t c) {
  return ((c & 0xE000)>>8) | ((c & 0x0700)>>6) | ((c & 0x0018)>>3);
}

/***************************************************************************************
** Function name:           color8to16
** Description:             Convert 8bit palette to 16bit palette
***************************************************************************************/
uint16_t TFTLIB_8BIT::color8to16(uint8_t color) {
  uint8_t  blue[] = {0, 11, 21, 31}; // blue 2 to 5 bit colour lookup table
  uint16_t color16 = 0;

  //        =====Green=====     ===============Red==============
  color16  = (color & 0x1C)<<6 | (color & 0xC0)<<5 | (color & 0xE0)<<8;
  //        =====Green=====    =======Blue======
  color16 |= (color & 0x1C)<<3 | blue[color & 0x03];

  return color16;
}

/***************************************************************************************
** Function name:           alphaBlend
** Description:             Mix fgc & bgc with selected alpha channel(255 = full bgc)
***************************************************************************************/
uint16_t TFTLIB_8BIT::alphaBlend(uint8_t alpha, uint16_t fgc, uint16_t bgc) {
	// For speed use fixed point maths and rounding to permit a power of 2 division
	uint16_t fgR = ((fgc >> 10) & 0x3E) + 1;
	uint16_t fgG = ((fgc >>  4) & 0x7E) + 1;
	uint16_t fgB = ((fgc <<  1) & 0x3E) + 1;

	uint16_t bgR = ((bgc >> 10) & 0x3E) + 1;
	uint16_t bgG = ((bgc >>  4) & 0x7E) + 1;
	uint16_t bgB = ((bgc <<  1) & 0x3E) + 1;

	// Shift right 1 to drop rounding bit and shift right 8 to divide by 256
	uint16_t r = (((fgR * alpha) + (bgR * (255 - alpha))) >> 9);
	uint16_t g = (((fgG * alpha) + (bgG * (255 - alpha))) >> 9);
	uint16_t b = (((fgB * alpha) + (bgB * (255 - alpha))) >> 9);

	return (r << 11) | (g << 5) | (b << 0);
}

/***************************************************************************************
** Function name:           setWindow8
** Description:             Set start/end address of drawed window (For parallel display)
***************************************************************************************/
void TFTLIB_8BIT::setWindow8(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
	if ((x0 > x1) || (x1 > _width) || (y0 > y1) || (y1 > _height)) return;

	if (_type == (uint8_t)TFT_DRIVER::ILI9327_PARALLEL) {
	    if (__rotation == 2) y0 += 32, y1 += 32;
	    if (__rotation == 3) x0 += 32, x1 += 32;
    }

	if (__addr_col != (x0<<16 | x1)) {
		writeCommand8(CASET);
		writeSmallData32(((uint32_t) x0 << 16) | x1);
		__addr_col = (x0<<16 | x1);
	}

	if (__addr_row != (y0<<16 | y1)) {
		writeCommand8(RASET);
		writeSmallData32(((uint32_t) y0 << 16) | y1);
		__addr_row = (y0<<16 | y1);
	}

	writeCommand8(RAMWR);
}

/***************************************************************************************
** Function name:           setWindow8
** Description:             Set start/end address of drawed window (For parallel display)
***************************************************************************************/
void TFTLIB_8BIT::readWindow8(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
	if ((x0 > x1) || (x1 > _width) || (y0 > y1) || (y1 > _height)) return;

	if (_type == (uint8_t)TFT_DRIVER::ILI9327_PARALLEL) {
	    if (__rotation == 2) y0 += 32, y1 += 32;
	    if (__rotation == 3) x0 += 32, x1 += 32;
    }

	writeCommand8(CASET);
	writeSmallData32(((uint32_t) x0 << 16) | x1);

	writeCommand8(RASET);
	writeSmallData32(((uint32_t) y0 << 16) | y1);

	writeCommand8(RAMRD);
}

/***************************************************************************************
** Function name:           pushPixels
** Description:             Write pixels from pointer (for JPEG Decoding)
***************************************************************************************/
void TFTLIB_8BIT::pushPixels16(const void* data_in, uint32_t len) {
	uint16_t *data = (uint16_t*)data_in;
	while (len-- > 0) writeData16(data++, len);
}

/***************************************************************************************
** Function name:           pushBlock16
** Description:             Push block of 16bit data (For parallel display)
***************************************************************************************/
inline void TFTLIB_8BIT::pushBlock16(uint16_t color, uint32_t len = 1){
	CS_L();
	DC_H();

	while(len>31) {
		write16(color); write16(color); write16(color); write16(color);
		write16(color); write16(color); write16(color); write16(color);
		write16(color); write16(color); write16(color); write16(color);
		write16(color); write16(color); write16(color); write16(color);
		write16(color); write16(color); write16(color); write16(color);
		write16(color); write16(color); write16(color); write16(color);
		write16(color); write16(color); write16(color); write16(color);
		write16(color); write16(color); write16(color); write16(color);
		len -= 32;
	}

	while(len>7) {
		write16(color); write16(color); write16(color); write16(color);
		write16(color); write16(color); write16(color); write16(color);
		len -= 8;
	}

	while (len--) write16(color);

	CS_H();
}

/***************************************************************************************
** Function name:           fillScreen
** Description:             Fast color fillig screen function
***************************************************************************************/
void TFTLIB_8BIT::fillScreen(uint16_t color)
{
	writeCommand8(DISPOFF);
	writeCommand8(SLPIN);
	setWindow8(0, 0, _width - 1, _height - 1);
	pushBlock16(color, _width * _height);
	writeCommand8(SLPOUT);
	writeCommand8(DISPON);
}

/***************************************************************************************
** Function name:           cpuConfig
** Description:             Print MCU clock configuration
***************************************************************************************/
void TFTLIB_8BIT::cpuConfig(void){
	fillScreen(BLACK);
	char prt[32];
	uint32_t hclk = HAL_RCC_GetHCLKFreq();
	uint32_t apb1 = HAL_RCC_GetPCLK1Freq();
	uint32_t apb2 = HAL_RCC_GetPCLK2Freq();
	uint32_t sysclk = HAL_RCC_GetSysClockFreq();

	setTextColor(RED, BLACK);
	setFont(Font_11x18);
	setCursor(0, 30);

	sprintf(prt, "HCLK: %lu", hclk);
	println(prt);
	sprintf(prt, "APB1: %lu", apb1);
	println(prt);
	sprintf(prt, "APB2: %lu", apb2);
	println(prt);
	sprintf(prt, "SYSCLK: %lu", sysclk);
	println(prt);
}

/***************************************************************************************
** Function name:           FreeRAM
** Description:             Return free RAM
***************************************************************************************/
int TFTLIB_8BIT::FreeRAM() {
	register uint32_t sp asm("sp");
	return reinterpret_cast<char*>(sp) - reinterpret_cast<char*>(sbrk(0));
}

/***************************************************************************************
** Function name:           Toggle ART Accelerator
** Description:             Turn on / Turn off ART Accelerator
***************************************************************************************/
void TFTLIB_8BIT::ARTtoggle() {
	setTextColor(RED, BLACK);
	setFont(Font_11x18);
	setCursor(0, 0);
	if((FLASH->ACR & FLASH_ACR_ICEN)!=FLASH_ACR_ICEN) { // art disabled
		/* enable the ART accelerator */
		/* enable prefetch buffer */
		FLASH->ACR |= FLASH_ACR_PRFTEN;
		/* Enable flash instruction cache */
		FLASH->ACR |= FLASH_ACR_ICEN;
		/* Enable flash data cache */
		FLASH->ACR |= FLASH_ACR_DCEN;
		asm("wfi"); //wait for a systick interrupt
		fillScreen(BLACK);
		setCursor(0, 0);
		println((char*)"ART enabled");
	}

	else {
		/* disable the ART accelerator */
		/* disable flash instruction cache */
		FLASH->ACR &= ~FLASH_ACR_ICEN;
		/* disable flash data cache */
		FLASH->ACR &= ~FLASH_ACR_DCEN;
		/* enable prefetch buffer */
		FLASH->ACR &= ~FLASH_ACR_PRFTEN;
		asm("wfi"); //wait for a systick interrupt
		fillScreen(BLACK);
		setCursor(0, 0);
		println((char*)"ART disabled");
	}
}

/***************************************************************************************
** Function name:           readPixel (PARALLEL DISPLAY ONLY!)
** Description:             Read single pixel at coords x&y
***************************************************************************************/
uint16_t TFTLIB_8BIT::readPixel(int32_t x0, int32_t y0)
{
	uint8_t data[4];

	readWindow8(x0, y0, x0, y0);

	// Set masked pins D0- D7 to input
	PIN_INPUT(__PARALLEL_PORT, 0x00FF);

	readByte();
	data[1] = readByte();
	data[2] = readByte();
	data[3] = readByte();

	// Set masked pins D0- D7 to output
	PIN_OUTPUT(__PARALLEL_PORT, 0x00FF);

	return (((data[1] & 0xF8) << 8) | ((data[2] & 0xFC) << 3) | (data[3]  >> 3));
}

/***************************************************************************************
** Function name:           drawPixel
** Description:             Draw single pixel at coords x&y
***************************************************************************************/
void TFTLIB_8BIT::drawPixel(int32_t x, int32_t y, uint16_t color)
{
	if ((x < 0) || (x >= _width) || (y < 0) || (y >= _height))	return;

	setWindow8(x, y, x, y);
	pushBlock16(color, 1);
}

/***************************************************************************************
** Function name:           drawPixelAlpha (PARALLEL DISPLAY ONLY!)
** Description:             Draw single pixel with alpha channel
***************************************************************************************/
void TFTLIB_8BIT::drawPixelAlpha(int16_t x, int16_t y, uint16_t color, float alpha) {
	if ((x < 0) || (x >= _width) || (y < 0) || (y >= _height))	return;

	if (alpha > 0.996) {
		drawPixel(x, y, color);
	}

	alpha = alpha * PixelAlphaGain;
	if (alpha > 255) alpha = 255;

	drawPixel(x, y, alphaBlend((uint8_t)alpha, color, readPixel(x, y)));
}

/***************************************************************************************
** Function name:           drawFastHLine
** Description:             Fast drawing Horizontal Line
***************************************************************************************/
inline void TFTLIB_8BIT::drawFastHLine(int32_t x, int32_t y, int32_t w, uint16_t color) {
	if(x < 0 || x > _width || y < 0 || y > _height || x + w > _width) return;

	setWindow8(x, y, x + w - 1, y);
	pushBlock16(color, w);
}

/***************************************************************************************
** Function name:           drawHLineAlpha
** Description:             Fast drawing Horizontal Line
***************************************************************************************/
void TFTLIB_8BIT::drawHLineAlpha(int32_t x, int32_t y, int32_t w, uint16_t color, uint8_t alpha) {
	if(x < 0 || x > _width || y < 0 || y > _height || x + w > _width) return;

	uint16_t i = 0;

	while(i < w) {
		__buffer[i] = alphaBlend(alpha, color, readPixel(x + i, y));
		i++;
	}

	setWindow8(x, y, x + w, y);
	writeData16(__buffer, w);
}

/***************************************************************************************
** Function name:           drawFastVLine
** Description:             Drawing Vertical Line
***************************************************************************************/
inline void TFTLIB_8BIT::drawFastVLine(int32_t x, int32_t y, int32_t h, uint16_t color) {
	if(x < 0 || x > _width || y < 0 || y > _height || y + h > _height) return;

	setWindow8(x, y, x, y + h - 1);
	pushBlock16(color, h);
}

/***************************************************************************************
** Function name:           drawVLineAlpha
** Description:             Drawing Vertical Line
***************************************************************************************/
void TFTLIB_8BIT::drawVLineAlpha(int32_t x, int32_t y, int32_t h, uint16_t color, uint8_t alpha) {
	if(x < 0 || x > _width || y < 0 || y > _height || y + h > _height) return;

	uint16_t i = 0;

	while(i < h) {
		__buffer[i] = alphaBlend(alpha, color, readPixel(x + i, y));
		i++;
	}

	setWindow8(x, y, x, y + h - 1);
	writeData16(__buffer, h);
}

/***************************************************************************************
** Function name:           drawCircleHelper
** Description:             Support function for drawRoundRect()
***************************************************************************************/
inline void TFTLIB_8BIT::drawCircleHelper( int32_t x0, int32_t y0, int32_t rr, uint8_t cornername, uint16_t color) {
	if (rr <= 0) return;
	int32_t f     = 1 - rr;
	int32_t ddF_x = 1;
	int32_t ddF_y = -2 * rr;
	int32_t xe    = 0;
	int32_t xs    = 0;
	int32_t len   = 0;

	while (xe < rr--) {
		while (f < 0) {
			++xe;
			f += (ddF_x += 2);
		}

		f += (ddF_y += 2);

		if (xe-xs==1) {
			if (cornername & 0x1) { // left top
				drawPixel(x0 - xe, y0 - rr, color);
				drawPixel(x0 - rr, y0 - xe, color);
			}

			if (cornername & 0x2) { // right top
				drawPixel(x0 + rr    , y0 - xe, color);
				drawPixel(x0 + xs + 1, y0 - rr, color);
			}

			if (cornername & 0x4) { // right bottom
				drawPixel(x0 + xs + 1, y0 + rr    , color);
				drawPixel(x0 + rr, y0 + xs + 1, color);
			}

			if (cornername & 0x8) { // left bottom
				drawPixel(x0 - rr, y0 + xs + 1, color);
				drawPixel(x0 - xe, y0 + rr    , color);
			}
		}

		else {
			len = xe - xs++;
			if (cornername & 0x1) { // left top
				drawFastHLine(x0 - xe, y0 - rr, len, color);
				drawFastVLine(x0 - rr, y0 - xe, len, color);
			}

			if (cornername & 0x2) { // right top
				drawFastVLine(x0 + rr, y0 - xe, len, color);
				drawFastHLine(x0 + xs, y0 - rr, len, color);
			}

			if (cornername & 0x4) { // right bottom
				drawFastHLine(x0 + xs, y0 + rr, len, color);
				drawFastVLine(x0 + rr, y0 + xs, len, color);
			}

			if (cornername & 0x8) { // left bottom
				drawFastVLine(x0 - rr, y0 + xs, len, color);
				drawFastHLine(x0 - xe, y0 + rr, len, color);
			}
		}
	xs = xe;
	}
}

/***************************************************************************************
** Function name:           wideWedgeDistance
** Description:             Support function for drawWedgeLine
***************************************************************************************/
inline float TFTLIB_8BIT::wedgeLineDistance(float xpax, float ypay, float bax, float bay, float dr) {
	float h = fmaxf(fminf((xpax * bax + ypay * bay) / (bax * bax + bay * bay), 1.0f), 0.0f);
	float dx = xpax - bax * h, dy = ypay - bay * h;
	return sqrtf(dx * dx + dy * dy) + h * dr;
}

/***************************************************************************************
** Function name:           fillCircleHelper
** Description:             Support function for fillRoundRect()
***************************************************************************************/
inline void TFTLIB_8BIT::fillCircleHelper(int32_t x0, int32_t y0, int32_t r, uint8_t cornername, int32_t delta, uint16_t color) {
	if(x0 < 0 || y0 < 0 || x0 > _width || y0 > _height || x0 + r > _width || y0 + r > _height) return;

	int32_t f     = 1 - r;
	int32_t ddF_x = 1;
	int32_t ddF_y = -r - r;
	int32_t y     = 0;

	delta++;

	while (y < r) {
		if (f >= 0) {
			if (cornername & 0x1) drawFastHLine(x0 - y, y0 + r, y + y + delta, color);
			ddF_y += 2;
			f     += ddF_y;
			if (cornername & 0x2) drawFastHLine(x0 - y, y0 - r, y + y + delta, color);
			r--;
		}

		y++;
		if (cornername & 0x1) drawFastHLine(x0 - r, y0 + y, r + r + delta, color);

		ddF_x += 2;
		f     += ddF_x;
		if (cornername & 0x2) drawFastHLine(x0 - r, y0 - y, r + r + delta, color);
	}
}

/***************************************************************************************
** Function name:           fillCircleHelperAA
** Description:             Support function for fillRoundRectAA()
***************************************************************************************/
inline void TFTLIB_8BIT::fillCircleHelperAA(int32_t x0, int32_t y0, int32_t r, uint8_t cornername, int32_t delta, uint16_t color) {
	int32_t f     = 1 - r;
	int32_t ddF_x = 1;
	int32_t ddF_y = -r - r;
	int32_t y     = 0;

	delta++;

	while (y < r) {
		if (f >= 0) {
			if (cornername & 0x1) drawWideLine(x0 - y, y0 + r, x0 + y + delta, y0 + r, 1, color);
			ddF_y += 2;
			f     += ddF_y;
			if (cornername & 0x2) drawWideLine(x0 - y, y0 - r, x0 + y + delta, y0 - r, 1, color);
			r--;
		}

		y++;
		if (cornername & 0x1) drawWideLine(x0 - r, y0 + y, x0 + r + delta, y0 + y, 1, color);

		ddF_x += 2;
		f     += ddF_x;
		if (cornername & 0x2) drawWideLine(x0 - r, y0 - y, x0 + r + delta, y0 - y, 1, color);
	}
}

/***************************************************************************************
** Function name:           fillAlphaCircleHelper
** Description:             Support function for fillAlphaRoundRect()
***************************************************************************************/
inline void TFTLIB_8BIT::fillAlphaCircleHelper(int32_t x0, int32_t y0, int32_t r, uint8_t cornername, int32_t delta, uint16_t color, uint8_t alpha) {
	int32_t f     = 1 - r;
	int32_t ddF_x = 1;
	int32_t ddF_y = -r - r;
	int32_t y     = 0;

	delta++;

	while (y < r) {
		if (f >= 0) {
			if (cornername & 0x1) drawHLineAlpha(x0 - y, y0 + r, y + y + delta, color, alpha);
			ddF_y += 2;
			f     += ddF_y;
			if (cornername & 0x2) drawHLineAlpha(x0 - y, y0 - r, y + y + delta, color, alpha);
			r--;
		}

		y++;
		if (cornername & 0x1) drawHLineAlpha(x0 - r, y0 + y, r + r + delta, color, alpha);

		ddF_x += 2;
		f     += ddF_x;
		if (cornername & 0x2) drawHLineAlpha(x0 - r, y0 - y, r + r + delta, color, alpha);
	}
}

/***************************************************************************************
** Function name:           drawLine
** Description:             Draw a line with single color
***************************************************************************************/
void TFTLIB_8BIT::drawLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint16_t color)
{
	if(x0 < 0 || x0 > _width || y0 < 0 || y0 > _height || x1 < 0 || x1 > _width || y1 < 0 || y1 > _height) return;
	bool steep = abs(y1 - y0) > abs(x1 - x0);
	if (steep) {
		swap_coord(x0, y0);
		swap_coord(x1, y1);
	}

	if (x0 > x1) {
		swap_coord(x0, x1);
		swap_coord(y0, y1);
	}

	int32_t dx = x1 - x0, dy = abs(y1 - y0);

	int32_t err = dx >> 1, ystep = -1, xs = x0, dlen = 0;

	if (y0 < y1) ystep = 1;

	// Split into steep and not steep for FastH/V separation
	if (steep) {
		for (; x0 <= x1; x0++) {
			dlen++;
			err -= dy;
			if (err < 0) {
				if (dlen == 1) drawPixel(y0, xs, color);
				else drawFastVLine(y0, xs, dlen, color);
				dlen = 0;
				y0 += ystep; xs = x0 + 1;
				err += dx;
			}
		}
		if (dlen) drawFastVLine(y0, xs, dlen, color);
	}
	else {
		for (; x0 <= x1; x0++) {
			dlen++;
			err -= dy;
			if (err < 0) {
				if (dlen == 1) drawPixel(xs, y0, color);
				else drawFastHLine(xs, y0, dlen, color);
				dlen = 0;
				y0 += ystep; xs = x0 + 1;
				err += dx;
			}
		}
		if (dlen) drawFastHLine(xs, y0, dlen, color);
	}
}

/***************************************************************************************
** Function name:           drawWideLine
** Description:             Draw anti-aliased line with single color
***************************************************************************************/
void TFTLIB_8BIT::drawWideLine(float ax, float ay, float bx, float by, float wd, uint16_t fg_color) {
	drawWedgeLine( ax, ay, bx, by, wd/2.0, wd/2.0, fg_color, 0xFFFF);
}

/***************************************************************************************
** Function name:           drawWideLine
** Description:             Draw anti-aliased line with single color
***************************************************************************************/
void TFTLIB_8BIT::drawWideLine(float ax, float ay, float bx, float by, float wd, uint16_t fg_color, uint16_t bg_color) {
	drawWedgeLine( ax, ay, bx, by, wd/2.0, wd/2.0, fg_color, bg_color);
}

/***************************************************************************************
** Function name:           drawWedgeLine
** Description:             Draw anti-aliased line with single color
***************************************************************************************/
void TFTLIB_8BIT::drawWedgeLine(float ax, float ay, float bx, float by, float ar, float br, uint16_t fg_color) {
	drawWedgeLine(ax, ay, bx, by, ar, br, fg_color, 0xFFFF);
}

/***************************************************************************************
** Function name:           drawWedgeLine
** Description:             Draw anti-aliased line with single color
***************************************************************************************/
void TFTLIB_8BIT::drawWedgeLine(float ax, float ay, float bx, float by, float ar, float br, uint16_t fg_color, uint16_t bg_color = 0xFFFF) {
	if ( (abs(ax - bx) < 0.01f) && (abs(ay - by) < 0.01f) ) bx += 0.01f;  // Avoid divide by zero

	// Find line bounding box
	int32_t x0 = (int32_t)floorf(fminf(ax-ar, bx-br));
	int32_t x1 = (int32_t) ceilf(fmaxf(ax+ar, bx+br));
	int32_t y0 = (int32_t)floorf(fminf(ay-ar, by-br));
	int32_t y1 = (int32_t) ceilf(fmaxf(ay+ar, by+br));

	// Establish x start and y start
	int32_t ys = ay;
	if ((ax-ar)>(bx-br)) ys = by;

	float rdt = ar - br; // Radius delta
	float alpha = 1.0f;
	ar += 0.5;

	float xpax, ypay, bax = bx - ax, bay = by - ay;

	int32_t xs = x0;
	// Scan bounding box from ys down, calculate pixel intensity from distance to line
	for (int32_t yp = ys; yp <= y1; yp++) {
		bool swin = true;  // Flag to start new window area
		bool endX = false; // Flag to skip pixels
		ypay = yp - ay;
		for (int32_t xp = xs; xp <= x1; xp++) {
			if (endX) if (alpha <= LoAlphaTheshold) break;  // Skip right side
			xpax = xp - ax;
			alpha = ar - wedgeLineDistance(xpax, ypay, bax, bay, rdt);
			if (alpha <= LoAlphaTheshold ) continue;
			// Track edge to minimise calculations
			if (!endX) {
				endX = true;
				xs = xp;
			}

			if (alpha > HiAlphaTheshold) {
				if (swin) {
					setWindow8(xp, yp, width()-1, yp);
					swin = false;
				}
				pushBlock16(fg_color);
				continue;
			}

			//Blend color with background and plot
			if(bg_color == 0xFFFF) {
				bg_color = readPixel(xp, yp);
				swin = true;
			}

			if (swin) {
				setWindow8(xp, yp, width()-1, yp);
				swin = false;
			}

			pushBlock16(alphaBlend((uint8_t)(alpha * PixelAlphaGain), fg_color, bg_color));
		}
	}

	// Reset x start to left side of box
	xs = x0;
	// Scan bounding box from ys-1 up, calculate pixel intensity from distance to line
	for (int32_t yp = ys-1; yp >= y0; yp--) {
		bool swin = true;  // Flag to start new window area
		bool endX = false; // Flag to skip pixels
		ypay = yp - ay;
		for (int32_t xp = xs; xp <= x1; xp++) {
			if (endX) if (alpha <= LoAlphaTheshold) break;  // Skip right side of drawn line
			xpax = xp - ax;
			alpha = ar - wedgeLineDistance(xpax, ypay, bax, bay, rdt);
			if (alpha <= LoAlphaTheshold ) continue;

			// Track line boundary
			if (!endX) {
				endX = true;
				xs = xp;
			}

			if (alpha > HiAlphaTheshold) {
				if (swin) {
					setWindow8(xp, yp, width()-1, yp);
					swin = false;
				}
				pushBlock16(fg_color);
				continue;
			}

			//Blend color with background and plot
			if(bg_color == 0xFFFF) {
				bg_color = readPixel(xp, yp);
				swin = true;
			}

			if (swin) {
				setWindow8(xp, yp, width()-1, yp);
				swin = false;
			}

			pushBlock16(alphaBlend((uint8_t)(alpha * PixelAlphaGain), fg_color, bg_color));
		}
	}
}

/***************************************************************************************
** Function name:           drawTriangle
** Description:             Draw a triangle with single color
***************************************************************************************/
void TFTLIB_8BIT::drawTriangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, uint16_t color)
{
	drawLine(x1, y1, x2, y2, color);
	drawLine(x2, y2, x3, y3, color);
	drawLine(x3, y3, x1, y1, color);
}

/***************************************************************************************
** Function name:           drawTriangleAA
** Description:             Draw anti-aliased triangle with single color and specified thickness
***************************************************************************************/
void TFTLIB_8BIT::drawTriangleAA(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, int32_t thickness, uint16_t color) {
	drawWideLine(x1, y1, x2, y2, thickness, color);
	drawWideLine(x2, y2, x3, y3, thickness, color);
	drawWideLine(x3, y3, x1, y1, thickness, color);
}

/***************************************************************************************
** Function name:           drawRect
** Description:             Draw a rectangle with single color
***************************************************************************************/
void TFTLIB_8BIT::drawRect(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t color)
{
	if(x < 0 || y < 0 || x + w > _width || y + h > _height) return;
	drawFastHLine(x, y, w, color);
	drawFastVLine(x, y, h, color);
	drawFastVLine(x+w, y, h, color);
	drawFastHLine(x, y+h, w, color);
}

/***************************************************************************************
** Function name:           drawRectAA
** Description:             Draw anti-aliased rectangle with single color
***************************************************************************************/
void TFTLIB_8BIT::drawRectAA(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t color)
{
	if(x < 0 || y < 0 || w < 0 || h < 0 || x > _width || y > _height || x + w > _width || y + h > _height) return;
	drawWideLine(x, y, x + w, y, 1, color);
	drawWideLine(x, y, x, y + h, 1, color);
	drawWideLine(x + w, y, x + w, y + h, 1, color);
	drawWideLine(x, y + h, x + w, y + h, 1, color);
}

/***************************************************************************************
** Function name:           drawRoundRect
** Description:             Draw a rectangle with rounded corners
***************************************************************************************/
void TFTLIB_8BIT::drawRoundRect(int32_t x, int32_t y, int32_t w, int32_t h, int32_t r, uint16_t color)
{
	if(x < 0 || y < 0 || w < 0 || h < 0 || x > _width || y > _height || x + w > _width || y + h > _height) return;
	drawFastHLine(x + r  , y    , w - r - r, color); // Top
	drawFastHLine(x + r  , y + h - 1, w - r - r, color); // Bottom
	drawFastVLine(x    , y + r  , h - r - r, color); // Left
	drawFastVLine(x + w - 1, y + r  , h - r - r, color); // Right

	// draw four corners
	drawCircleHelper(x + r    , y + r    , r, 1, color);
	drawCircleHelper(x + w - r - 1, y + r    , r, 2, color);
	drawCircleHelper(x + w - r - 1, y + h - r - 1, r, 4, color);
	drawCircleHelper(x + r    , y + h - r - 1, r, 8, color);
}

/***************************************************************************************
** Function name:           drawCircle
** Description:             Draw a circle with single color
***************************************************************************************/
void TFTLIB_8BIT::drawCircle(int32_t x0, int32_t y0, int32_t r, uint16_t color)
{
	if(x0 - r < 0 || x0 + r > _width || y0 - r < 0 || y0 + r > _height)	return;

	int32_t f     = 1 - r;
	int32_t ddF_y = -2 * r;
	int32_t ddF_x = 1;
	int32_t xs    = -1;
	int32_t xe    = 0;
	int32_t len   = 0;

	bool first = true;
	do {
		while (f < 0) {
			++xe;
			f += (ddF_x += 2);
		}
		f += (ddF_y += 2);

		if (xe-xs>1) {
			if (first) {
				len = 2*(xe - xs)-1;
				drawFastHLine(x0 - xe, y0 + r, len, color);
				drawFastHLine(x0 - xe, y0 - r, len, color);
				drawFastVLine(x0 + r, y0 - xe, len, color);
				drawFastVLine(x0 - r, y0 - xe, len, color);
				first = false;
			}
			else {
				len = xe - xs++;
				drawFastHLine(x0 - xe, y0 + r, len, color);
				drawFastHLine(x0 - xe, y0 - r, len, color);
				drawFastHLine(x0 + xs, y0 - r, len, color);
				drawFastHLine(x0 + xs, y0 + r, len, color);

				drawFastVLine(x0 + r, y0 + xs, len, color);
				drawFastVLine(x0 + r, y0 - xe, len, color);
				drawFastVLine(x0 - r, y0 - xe, len, color);
				drawFastVLine(x0 - r, y0 + xs, len, color);
			}
		}
		else {
			++xs;
			drawPixel(x0 - xe, y0 + r, color);
			drawPixel(x0 - xe, y0 - r, color);
			drawPixel(x0 + xs, y0 - r, color);
			drawPixel(x0 + xs, y0 + r, color);

			drawPixel(x0 + r, y0 + xs, color);
			drawPixel(x0 + r, y0 - xe, color);
			drawPixel(x0 - r, y0 - xe, color);
			drawPixel(x0 - r, y0 + xs, color);
		}
		xs = xe;
	} while (xe < --r);
}

/***************************************************************************************
** Function name:           drawEllipse
** Description:             Draw an ellipse with single color
***************************************************************************************/
void TFTLIB_8BIT::drawEllipse(int16_t x0, int16_t y0, int32_t rx, int32_t ry, uint16_t color)
{
	if(x0 - rx < 0 || x0 + rx > _width || y0 - ry < 0 || y0 + ry > _height || rx < 2 || ry < 2) return;
	int32_t x, y;
	int32_t rx2 = rx * rx;
	int32_t ry2 = ry * ry;
	int32_t fx2 = 4 * rx2;
	int32_t fy2 = 4 * ry2;
	int32_t s;

	for (x = 0, y = ry, s = 2*ry2+rx2*(1-2*ry); ry2*x <= rx2*y; x++) {
		drawPixel(x0 + x, y0 + y, color);
		drawPixel(x0 - x, y0 + y, color);
		drawPixel(x0 - x, y0 - y, color);
		drawPixel(x0 + x, y0 - y, color);
		if (s >= 0) {
			s += fx2 * (1 - y);
			y--;
		}
		s += ry2 * ((4 * x) + 6);
	}

	for (x = rx, y = 0, s = 2*rx2+ry2*(1-2*rx); rx2*y <= ry2*x; y++) {
		drawPixel(x0 + x, y0 + y, color);
		drawPixel(x0 - x, y0 + y, color);
		drawPixel(x0 - x, y0 - y, color);
		drawPixel(x0 + x, y0 - y, color);
		if (s >= 0) {
			s += fy2 * (1 - x);
			x--;
		}
		s += rx2 * ((4 * y) + 6);
	}
}

/***************************************************************************************
** Function name:           fillTriangle
** Description:             Draw filled triangle with fixed color
***************************************************************************************/
void TFTLIB_8BIT::fillTriangle ( int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint16_t color)
{
	int32_t a, b, y, last;

	if (y0 > y1) {
		swap_coord(y0, y1); swap_coord(x0, x1);
	}

	if (y1 > y2) {
		swap_coord(y2, y1); swap_coord(x2, x1);
	}

	if (y0 > y1) {
		swap_coord(y0, y1); swap_coord(x0, x1);
	}

	if (y0 == y2) {
		a = b = x0;
		if (x1 < a)      a = x1;
		else if (x1 > b) b = x1;
		if (x2 < a)      a = x2;
		else if (x2 > b) b = x2;
		drawFastHLine(a, y0, b - a + 1, color);
		return;
	}

	int32_t
	dx01 = x1 - x0,
	dy01 = y1 - y0,
	dx02 = x2 - x0,
	dy02 = y2 - y0,
	dx12 = x2 - x1,
	dy12 = y2 - y1,
	sa   = 0,
	sb   = 0;

	if (y1 == y2) last = y1;
	else last = y1 - 1;

	for (y = y0; y <= last; y++) {
		a   = x0 + sa / dy01;
		b   = x0 + sb / dy02;
		sa += dx01;
		sb += dx02;

		if (a > b) swap_coord(a, b);
		drawFastHLine(a, y, b - a + 1, color);
	}

	sa = dx12 * (y - y1);
	sb = dx02 * (y - y0);

	for (; y <= y2; y++) {
		a   = x1 + sa / dy12;
		b   = x0 + sb / dy02;
		sa += dx12;
		sb += dx02;

		if (a > b) swap_coord(a, b);
		drawFastHLine(a, y, b - a + 1, color);
	}
}

/***************************************************************************************
** Function name:           fillTriangleAA
** Description:             Draw anti-aliased filled triangle with fixed color
***************************************************************************************/
void TFTLIB_8BIT::fillTriangleAA( int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint16_t color)
{
	int32_t a, b, y, last;

	if (y0 > y1) {
		swap_coord(y0, y1); swap_coord(x0, x1);
	}

	if (y1 > y2) {
		swap_coord(y2, y1); swap_coord(x2, x1);
	}

	if (y0 > y1) {
		swap_coord(y0, y1); swap_coord(x0, x1);
	}

	if (y0 == y2) {
		a = b = x0;
		if (x1 < a)      a = x1;
		else if (x1 > b) b = x1;
		if (x2 < a)      a = x2;
		else if (x2 > b) b = x2;
		drawWideLine(a, y0, b + 1, y0, 1, color);
		return;
	}

	int32_t
	dx01 = x1 - x0,
	dy01 = y1 - y0,
	dx02 = x2 - x0,
	dy02 = y2 - y0,
	dx12 = x2 - x1,
	dy12 = y2 - y1,
	sa   = 0,
	sb   = 0;

	if (y1 == y2) last = y1;
	else last = y1 - 1;

	for (y = y0; y <= last; y++) {
		a   = x0 + sa / dy01;
		b   = x0 + sb / dy02;
		sa += dx01;
		sb += dx02;

		if (a > b) swap_coord(a, b);
		drawWideLine(a, y, b + 1, y, 1, color);
	}

	sa = dx12 * (y - y1);
	sb = dx02 * (y - y0);

	for (; y <= y2; y++) {
		a   = x1 + sa / dy12;
		b   = x0 + sb / dy02;
		sa += dx12;
		sb += dx02;

		if (a > b) swap_coord(a, b);
		drawWideLine(a, y, b + 1, y, 1, color);
	}
}

/***************************************************************************************
** Function name:           fillTriangleAlpha
** Description:             Draw filled triangle with alpha channel
***************************************************************************************/
void TFTLIB_8BIT::fillTriangleAlpha( int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint16_t color, uint8_t alpha) {
	int32_t a, b, y, last;

	if (y0 > y1) {
		swap_coord(y0, y1); swap_coord(x0, x1);
	}

	if (y1 > y2) {
		swap_coord(y2, y1); swap_coord(x2, x1);
	}

	if (y0 > y1) {
		swap_coord(y0, y1); swap_coord(x0, x1);
	}

	if (y0 == y2) {
		a = b = x0;
		if (x1 < a)      a = x1;
		else if (x1 > b) b = x1;
		if (x2 < a)      a = x2;
		else if (x2 > b) b = x2;
		drawHLineAlpha(a, y0, b - a + 1, color, alpha);
		return;
	}

	int32_t
	dx01 = x1 - x0,
	dy01 = y1 - y0,
	dx02 = x2 - x0,
	dy02 = y2 - y0,
	dx12 = x2 - x1,
	dy12 = y2 - y1,
	sa   = 0,
	sb   = 0;

	if (y1 == y2) last = y1;
	else last = y1 - 1;

	for (y = y0; y <= last; y++) {
		a   = x0 + sa / dy01;
		b   = x0 + sb / dy02;
		sa += dx01;
		sb += dx02;

		if (a > b) swap_coord(a, b);
		drawHLineAlpha(a, y, b - a + 1, color, alpha);
	}

	sa = dx12 * (y - y1);
	sb = dx02 * (y - y0);
	for (; y <= y2; y++) {
		a   = x1 + sa / dy12;
		b   = x0 + sb / dy02;
		sa += dx12;
		sb += dx02;

		if (a > b) swap_coord(a, b);
		drawHLineAlpha(a, y, b - a + 1, color, alpha);
	}
}

/***************************************************************************************
** Function name:           fillRect
** Description:             Draw a filled rectangle with fixed color
***************************************************************************************/
void TFTLIB_8BIT::fillRect(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t color)
{
	if(x < 0 || y < 0 || w < 0 || h < 0 || x > _width || y > _height || x + w > _width || y + h > _height) return;

	setWindow8(x, y, x + w - 1, y + h - 1);
	pushBlock16(color, w * h);
}

/***************************************************************************************
** Function name:           fillRectAA
** Description:             Draw anti-aliased filled rectangle with fixed color
***************************************************************************************/
void TFTLIB_8BIT::fillRectAA(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t color)
{
	if(x < 0 || y < 0 || w < 0 || h < 0 || x > _width || y > _height || x + w > _width || y + h > _height) return;

	setWindow8(x, y, x + w - 1, y + h - 1);
	pushBlock16(color, w * h);

	drawWideLine(x, y, x + w - 1, y, 1, color);
	drawWideLine(x, y+h-1, x+w-1, y+h-1, 1, color);

	drawWideLine(x, y, x, y + h - 1, 1, color);
	drawWideLine(x + w, y, x + w, y + h - 1, 1, color);
}

/***************************************************************************************
** Function name:           fillRectAlpha
** Description:             Draw a filled rectangle with alpha channel
***************************************************************************************/
void TFTLIB_8BIT::fillRectAlpha(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t color, uint8_t alpha)
{
	if(x < 0 || y < 0 || w < 0 || h < 0 || x > _width || y > _height || x + w > _width || y + h > _height) return;

	uint16_t i = 0;
	while(i++ < h) {
		uint16_t j = 0;

		while(j < w) {
			__buffer[j] = alphaBlend(alpha, color, readPixel(x + j, y + i));
			j++;
		}

		setWindow8(x, y + i, x + w, y + i);
		writeData16(__buffer, w);
	}
}

/***************************************************************************************
** Function name:           fillRoundRect
** Description:             Draw a filled rectangle with rounded corners & single color
***************************************************************************************/
void TFTLIB_8BIT::fillRoundRect(int32_t x, int32_t y, int32_t w, int32_t h, int32_t r, uint16_t color) {
  fillRect(x, y + r, w, h - r - r, color);

  fillCircleHelper(x + r, y + h - r - 1, r, 1, w - r - r - 1, color);
  fillCircleHelper(x + r    , y + r, r, 2, w - r - r - 1, color);
}

/***************************************************************************************
** Function name:           fillRoundRectAA
** Description:             Draw a filled rectangle with rounded corners & single color
***************************************************************************************/
void TFTLIB_8BIT::fillRoundRectAA(int32_t x, int32_t y, int32_t w, int32_t h, int32_t r, uint16_t color) {
  fillRectAA(x, y + r, w, h - r - r, color);

  fillCircleHelperAA(x + r, y + h - r - 1, r, 1, w - r - r - 1, color);
  fillCircleHelperAA(x + r    , y + r, r, 2, w - r - r - 1, color);
}

/***************************************************************************************
** Function name:           fillAlphaRoundRect
** Description:             Draw filled rectangle with rounded corners & alpha channel
***************************************************************************************/
void TFTLIB_8BIT::fillAlphaRoundRect(int32_t x, int32_t y, int32_t w, int32_t h, int32_t r, uint16_t color, uint8_t alpha) {
  fillRectAlpha(x, y + r, w, h - r - r, color, alpha);

  fillAlphaCircleHelper(x + r, y + h - r - 1, r, 1, w - r - r - 1, color, alpha);
  fillAlphaCircleHelper(x + r    , y + r, r, 2, w - r - r - 1, color, alpha);
}

/***************************************************************************************
** Function name:           fillCircle
** Description:             Draw a filled circle with single color
***************************************************************************************/
void TFTLIB_8BIT::fillCircle(int32_t x0, int32_t y0, int32_t r, uint16_t color) {
	if(x0 - r < 0 || x0 + r > _width || y0 - r < 0 || y0 + r > _height)	return;
	int32_t  x  = 0;
	int32_t  dx = 1;
	int32_t  dy = r+r;
	int32_t  p  = -(r>>1);

	drawFastHLine(x0 - r, y0, dy+1, color);

	while(x<r){
		if(p>=0) {
			drawFastHLine(x0 - x + 1, y0 + r, dx-1, color);
			drawFastHLine(x0 - x + 1, y0 - r, dx-1, color);
			dy-=2;
			p-=dy;
			r--;
		}

		dx+=2;
		p+=dx;
		x++;

		drawFastHLine(x0 - r, y0 + x, dy+1, color);
		drawFastHLine(x0 - r, y0 - x, dy+1, color);
	}
}

/***************************************************************************************
** Function name:           fillCircleAA
** Description:             Draw anti-aliased filled circle with fixed color
***************************************************************************************/
void TFTLIB_8BIT::fillCircleAA(float x, float y, float r, uint16_t color) {
	if(x - r < 0 || x + r > _width || y - r < 0 || y + r > _height)	return;
	drawWedgeLine(x, y, x, y, r, r, color, 0xFFFF);
}

/***************************************************************************************
** Function name:           fillCircleAlpha
** Description:             Draw filled circle with alpha channel
***************************************************************************************/
void TFTLIB_8BIT::fillCircleAlpha(int32_t x0, int32_t y0, int32_t r, uint16_t color, uint8_t alpha) {
	if(x0 - r < 0 || x0 + r > _width || y0 - r < 0 || y0 + r > _height)	return;
	int16_t  x  = 0;
	int16_t  dx = 1;
	int16_t  dy = r+r;
	int16_t  p  = -(r>>1);

	drawFastHLine(x0 - r, y0, dy+1, color);

	while(x<r){
		if(p>=0) {
			drawHLineAlpha(x0 - x, y0 + r, dx, color, alpha);
			dy-=2;
			p-=dy;
			drawHLineAlpha(x0 - x, y0 - r, dx, color, alpha);
			r--;
		}
		x++;
		drawHLineAlpha(x0 - r, y0 + x, dy+1, color, alpha);
		dx+=2;
		p+=dx;
		drawHLineAlpha(x0 - r, y0 - x, dy+1, color, alpha);
	}
}

/***************************************************************************************
** Function name:           fillEllipse
** Description:             Draw a filled ellipse with single color
***************************************************************************************/
void TFTLIB_8BIT::fillEllipse(int16_t x0, int16_t y0, int32_t rx, int32_t ry, uint16_t color)
{
  if(x0 - rx < 0 || x0 + rx > _width || y0 - ry < 0 || y0 + ry > _height || rx < 2 || ry < 2) return;
  int32_t x, y;
  int32_t rx2 = rx * rx;
  int32_t ry2 = ry * ry;
  int32_t fx2 = 4 * rx2;
  int32_t fy2 = 4 * ry2;
  int32_t s;

  for (x = 0, y = ry, s = 2*ry2+rx2*(1-2*ry); ry2*x <= rx2*y; x++) {
    drawFastHLine(x0 - x, y0 - y, x + x + 1, color);
    drawFastHLine(x0 - x, y0 + y, x + x + 1, color);

    if (s >= 0) {
      s += fx2 * (1 - y);
      y--;
    }
    s += ry2 * ((4 * x) + 6);
  }

  for (x = rx, y = 0, s = 2*rx2+ry2*(1-2*rx); rx2*y <= ry2*x; y++) {
    drawFastHLine(x0 - x, y0 - y, x + x + 1, color);
    drawFastHLine(x0 - x, y0 + y, x + x + 1, color);

    if (s >= 0) {
      s += fy2 * (1 - x);
      x--;
    }
    s += rx2 * ((4 * y) + 6);
  }
}

/***************************************************************************************
** Function name:           drawImage
** Description:             Draw image at coords x&y
***************************************************************************************/
void TFTLIB_8BIT::drawImage(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t *data) {
	if(x < 0 || y < 0 || w < 0 || h < 0 || x > _width || y > _height || x + w > _width || y + h > _height) return;

	setWindow8(x, y, x + w - 1, y + h - 1);
	writeData16(data, w * h);
}

/***************************************************************************************
** Function name:           drawBitmap
** Description:             Draw bitmap from array with fixed color
***************************************************************************************/
void TFTLIB_8BIT::drawBitmap(int16_t x, int16_t y, int16_t w, int16_t h, const uint8_t *bitmap, uint16_t color) {
	if(x < 0 || y < 0 || w < 0 || h < 0 || x > _width || y > _height || x + w > _width || y + h > _height) return;

	int32_t i, j, byteWidth = (w + 7) / 8;

	for (j = 0; j < h; j++) {
		for (i = 0; i < w; i++ ) {
			if (pgm_read_byte(bitmap + j * byteWidth + i / 8) & (128 >> (i & 7))) {
				drawPixel(x + i, y + j, color);
			}
		}
	}
}

/***************************************************************************************
** Function name:           setCursor
** Description:             Set text cursor at x&y. Used for print/println function
***************************************************************************************/
void TFTLIB_8BIT::setCursor(int32_t x, int32_t y){
	if(x > _width || y > _height) return;
	_posx = x;
	_posy = y;
}

/***************************************************************************************
** Function name:           setTextColor
** Description:             Set text cursor at x&y. Used for print/println function
***************************************************************************************/
void TFTLIB_8BIT::setTextColor(int32_t fg, int32_t bg){
	__text_fg = fg;
	__text_bg = bg;
}

/***************************************************************************************
** Function name:           setFont
** Description:             Set text cursor at x&y. Used for print/println function
***************************************************************************************/
void TFTLIB_8BIT::setFont(FontDef &Font){
	__font = &Font;
}

/***************************************************************************************
** Function name:           writeChar
** Description:             Print character at coords x&y with selected font
***************************************************************************************/
void TFTLIB_8BIT::writeChar(int32_t x, int32_t y, char ch) {
	int32_t i=0, b=0, j=0;

	if(x + __font->width > _width || y + __font->height > _height) return;
	setWindow8(x, y, x + __font->width - 1, y + __font->height - 1);

	for (i = 0; i < __font->height; i++) {
		b = __font->data[(ch - 32) * __font->height + i];
		for (j = 0; j < __font->width; j++) {
			if ((b << j) & 0x8000) {
				__buffer[(i*__font->width) + j] = __text_fg;
			}
			else {
				__buffer[(i*__font->width) + j] = __text_bg;
			}
		}
	}
	writeData16(__buffer, __font->width * __font->height);
}

/***************************************************************************************
** Function name:           writeString
** Description:             Print string at coords x&y with selected font
***************************************************************************************/
void TFTLIB_8BIT::writeString(int32_t x, int32_t y, char *ch) {
	while(*ch){
		if(strcmp(reinterpret_cast<const char*>(&ch), " ") == 0){
			x += __font->width;
			ch++;
			break;
		}

		else {
			if(x + __font->width > _width || y + __font->height > _height) return;
			int32_t i=0, b=0, j=0;
			setWindow8(x, y, x + __font->width - 1, y + __font->height - 1);

			for (i = 0; i < __font->height; i++) {
				b = __font->data[(*ch - 32) * __font->height + i];
				for (j = 0; j < __font->width; j++) {
					if ((b << j) & 0x8000) {
						__buffer[(i*__font->width) + j] = __text_fg;
					}
					else {
						__buffer[(i*__font->width) + j] = __text_bg;
					}
				}
			}
			writeData16(__buffer, __font->width * __font->height);
			x += __font->width;
			ch++;
		}
	}
}

/***************************************************************************************
** Function name:           print
** Description:             Print string with selected font
***************************************************************************************/
void TFTLIB_8BIT::print(char *ch) {
	int32_t i=0, b=0, j=0;

	while(*ch){

		if(*ch < 32 || *ch > 128 || *ch == 0) return;
		if(_posx + __font->width > _width) {
			_posx = 0;
			setCursor(_posx, _posy);
		}

		setWindow8(_posx, _posy, _posx + __font->width - 1, _posy + __font->height - 1);

		for (i = 0; i < __font->height; i++) {
			b = __font->data[(*ch - 32) * __font->height + i];
			for (j = 0; j < __font->width; j++) {
				if ((b << j) & 0x8000) {
					__buffer[(i*__font->width) + j] = __text_fg;
				}
				else {
					__buffer[(i*__font->width) + j] = __text_bg;
				}
			}
		}
		writeData16(__buffer, __font->width * __font->height);
		_posx += __font->width;
		ch++;
	}
}

/***************************************************************************************
** Function name:           println
** Description:             Print string with selected font
***************************************************************************************/
void TFTLIB_8BIT::println(uint8_t *ch)
{
	int32_t cur_x = _posx;
	int32_t i=0, b=0, j=0;

	if(*ch < 32 || *ch > 128 || *ch == 0) return;
	if(cur_x + __font->width > _width) {
		cur_x = 0;
		if(_posy + __font->height > _height) _posy = 0;
		else _posy += __font->height;
		setCursor(cur_x, _posy);
	}

	setWindow8(cur_x, _posy, cur_x + __font->width - 1, _posy + __font->height - 1);

	for (i = 0; i < __font->height; i++) {
		b = __font->data[(*ch - 32) * __font->height + i];
		for (j = 0; j < __font->width; j++) {
			if ((b << j) & 0x8000) {
				__buffer[(i*__font->width) + j] = __text_fg;
			}
			else {
				__buffer[(i*__font->width) + j] = __text_bg;
			}
		}
	}
	writeData16(__buffer, __font->width * __font->height);
	cur_x += __font->width;
	ch++;

	if(_posy + __font->height > _height)
		_posy = 0;
	else
		_posy += __font->height;
}

void TFTLIB_8BIT::println(char *ch)
{
	int32_t cur_x = _posx;
	int32_t i=0, b=0, j=0;

	while(*ch){
		if(*ch < 32 || *ch > 128 || *ch == 0) return;
		if(cur_x + __font->width > _width) {
			cur_x = 0;
			if(_posy + __font->height > _height) _posy = 0;
			else _posy += __font->height;
			setCursor(cur_x, _posy);
		}

		setWindow8(cur_x, _posy, cur_x + __font->width - 1, _posy + __font->height - 1);

		for (i = 0; i < __font->height; i++) {
			b = __font->data[(*ch - 32) * __font->height + i];
			for (j = 0; j < __font->width; j++) {
				if ((b << j) & 0x8000) {
					__buffer[(i*__font->width) + j] = __text_fg;
				}
				else {
					__buffer[(i*__font->width) + j] = __text_bg;
				}
			}
		}
		writeData16(__buffer, __font->width * __font->height);
		cur_x += __font->width;
		ch++;
	}

	if(_posy + __font->height > _height)
		_posy = 0;
	else
		_posy += __font->height;
}

/***************************************************************************************************************************
** 												Test functions for display benchmark
****************************************************************************************************************************/
uint32_t TFTLIB_8BIT::testFillScreen() {
  unsigned long start = HAL_GetTick();
  fillScreen(BLACK);
  fillScreen(RED);
  fillScreen(GREEN);
  fillScreen(BLUE);
  fillScreen(BLACK);
  return HAL_GetTick() - start;
}

uint32_t TFTLIB_8BIT::testText() {
  fillScreen(BLACK);

  unsigned long start = HAL_GetTick();
  setFont(Font_7x10);
  setTextColor(WHITE, BLACK);
  writeString(0, 0, (char*)"Hello World!");

  setFont(Font_11x18);
  setTextColor(YELLOW, BLACK);
  writeString(0, 10, (char*)"1234.56");

  setTextColor(RED, BLACK);
  writeString(0, 28, (char*)"0xDEADBEEF");

  setFont(Font_16x26);
  setTextColor(GREEN, BLACK);
  writeString(0, 64, (char*)"Groop");

  setFont(Font_11x18);
  setTextColor(MAGENTA, BLACK);
  writeString(0, 90, (char*)"I implore thee,");

  setFont(Font_7x10);
  setTextColor(LGRAY, BLACK);
  writeString(0, 108, (char*)"And hooptiously drangle me");
  writeString(0, 118, (char*)"with crinkly bindlewurdles,");
  writeString(0, 128, (char*)"Or I will rend thee");
  writeString(0, 138, (char*)"in the gobberwarts");
  writeString(0, 148, (char*)"with my blurglecruncheon,");
  writeString(0, 158, (char*)"see if I don't!");
  return HAL_GetTick() - start;
}

uint32_t TFTLIB_8BIT::testLines(uint16_t color) {
  unsigned long start, t;
  int           x1, y1, x2, y2,
                w = _width,
                h = _height;

  fillScreen(BLACK);

  x1 = y1 = 0;
  y2    = h - 1;
  start = HAL_GetTick();
  for (x2 = 0; x2 < w; x2 += 6) drawLine(x1, y1, x2, y2, color);
  x2    = w - 1;
  for (y2 = 0; y2 < h; y2 += 6) drawLine(x1, y1, x2, y2, color);
  t     = HAL_GetTick() - start; // fillScreen doesn't count against timing

  fillScreen(BLACK);

  x1    = w - 1;
  y1    = 0;
  y2    = h - 1;
  start = HAL_GetTick();
  for (x2 = 0; x2 < w; x2 += 6) drawLine(x1, y1, x2, y2, color);
  x2    = 0;
  for (y2 = 0; y2 < h; y2 += 6) drawLine(x1, y1, x2, y2, color);
  t    += HAL_GetTick() - start;

  fillScreen(BLACK);

  x1    = 0;
  y1    = h - 1;
  y2    = 0;
  start = HAL_GetTick();
  for (x2 = 0; x2 < w; x2 += 6) drawLine(x1, y1, x2, y2, color);
  x2    = w - 1;
  for (y2 = 0; y2 < h; y2 += 6) drawLine(x1, y1, x2, y2, color);
  t    += HAL_GetTick() - start;

  fillScreen(BLACK);

  x1    = w - 1;
  y1    = h - 1;
  y2    = 0;
  start = HAL_GetTick();
  for (x2 = 0; x2 < w; x2 += 6) drawLine(x1, y1, x2, y2, color);
  x2    = 0;
  for (y2 = 0; y2 < h; y2 += 6) drawLine(x1, y1, x2, y2, color);

  return HAL_GetTick() - start;
}

uint32_t TFTLIB_8BIT::testFastLines(uint16_t color1, uint16_t color2) {
  unsigned long start;
  int           x, y, w = _width, h = _height;

  fillScreen(BLACK);
  start = HAL_GetTick();
  for (y = 0; y < h; y += 5) drawFastHLine(0, y, w, color1);
  for (x = 0; x < w; x += 5) drawFastVLine(x, 0, h, color2);

  return HAL_GetTick() - start;
}

uint32_t TFTLIB_8BIT::testRects(uint16_t color) {
  unsigned long start;
  int           n, i, i2,
                cx = _width  / 2,
                cy = _height / 2;

  fillScreen(BLACK);
  n     = min(_width, _height);
  start = HAL_GetTick();
  for (i = 2; i < n; i += 6) {
    i2 = i / 2;
    drawRect(cx - i2, cy - i2, i, i, color);
  }

  return HAL_GetTick() - start;
}

uint32_t TFTLIB_8BIT::testFilledRects(uint16_t color1, uint16_t color2) {
  unsigned long start, t = 0;
  int           n, i, i2,
                cx = _width  / 2 - 1,
                cy = _height / 2 - 1;

  fillScreen(BLACK);
  n = min(_width, _height);
  for (i = n - 1; i > 0; i -= 6) {
    i2    = i / 2;
    start = HAL_GetTick();
    fillRect(cx - i2, cy - i2, i, i, color1);
    t    += HAL_GetTick() - start;
    // Outlines are not included in timing results
    drawRect(cx - i2, cy - i2, i, i, color2);
  }
  return t;
}

uint32_t TFTLIB_8BIT::testFilledCircles(uint8_t radius, uint16_t color) {
  unsigned long start;
  int x, y, w = _width, h = _height, r2 = radius * 2;

  fillScreen(BLACK);
  start = HAL_GetTick();
  for (x = radius; x < w; x += r2) {
    for (y = radius; y < h; y += r2) {
      fillCircle(x, y, radius, color);
    }
  }
  return HAL_GetTick() - start;
}

uint32_t TFTLIB_8BIT::testCircles(uint8_t radius, uint16_t color) {
  unsigned long start;
  int           x, y, r2 = radius * 2,
                      w = _width  + radius,
                      h = _height + radius;

  // Screen is not cleared for this one -- this is
  // intentional and does not affect the reported time.
  start = HAL_GetTick();
  for (x = 0; x < w; x += r2) {
    for (y = 0; y < h; y += r2) {
    	drawCircle(x, y, radius, color);
    }
  }

  return HAL_GetTick() - start;
}

uint32_t TFTLIB_8BIT::testTriangles() {
	unsigned long start;
	int           n, i, cx = _width  / 2 - 1,
				  cy = _height / 2 - 1;

	fillScreen(BLACK);
	n     = min(cx, cy);
	start = HAL_GetTick();

	for (i = 0; i < n; i += 5) {
		drawTriangle(
		cx    , cy - i, // peak
		cx - i, cy + i, // bottom left
		cx + i, cy + i, // bottom right
		color565(0, 0, i));
	}

	return HAL_GetTick() - start;
}

uint32_t TFTLIB_8BIT::testFilledTriangles() {
  unsigned long start, t = 0;
  int           i, cx = _width  / 2 - 1,
                   cy = _height / 2 - 1;

  fillScreen(BLACK);
  start = HAL_GetTick();
  for (i = min(cx, cy); i > 10; i -= 5) {
    start = HAL_GetTick();
    fillTriangle(cx, cy - i, cx - i, cy + i, cx + i, cy + i,
                     color565(0, i, i));
    t += HAL_GetTick() - start;
    drawTriangle(cx, cy - i, cx - i, cy + i, cx + i, cy + i,
                     color565(i, i, 0));
  }

  return t;
}

uint32_t TFTLIB_8BIT::testRoundRects() {
  unsigned long start;
  int           w, i, i2,
                cx = _width  / 2 - 1,
                cy = _height / 2 - 1;

  fillScreen(BLACK);
  w     = min(_height, _width);
  start = HAL_GetTick();
  for (i = 0; i < w; i += 6) {
    i2 = i / 2;
    drawRoundRect(cx - i2, cy - i2, i, i, i / 8, color565(i, 0, 0));
  }

  return HAL_GetTick() - start;
}

uint32_t TFTLIB_8BIT::testFilledRoundRects() {
  unsigned long start;
  int           i, i2,
                cx = _width  / 2 - 1,
                cy = _height / 2 - 1;

  fillScreen(BLACK);
  start = HAL_GetTick();
  for (i = min(_width , _height); i > 20; i -= 6) {
    i2 = i / 2;
    fillRoundRect(cx - i2, cy - i2, i, i, i / 8, color565(0, i, 0));
  }

  return HAL_GetTick() - start;
}

void TFTLIB_8BIT::benchmark(void){
	char buffer[32];
	uint32_t t1 = testFillScreen();
	uint32_t t2 = testText();

	uint32_t t3 = testLines(CYAN);
	uint32_t t4 = testFastLines(RED, BLUE);

	uint32_t t5 = testRects(GREEN);
	uint32_t t6 = testFilledRects(YELLOW, MAGENTA);

	uint32_t t7 = testCircles(10, WHITE);
	uint32_t t8 = testFilledCircles(10, MAGENTA);

	uint32_t t9 = testTriangles();
	uint32_t t10 = testFilledTriangles();

	uint32_t t11 = testRoundRects();
	uint32_t t12 = testFilledRoundRects();

	uint32_t total = t1 + t2 + t3 + t4 + t5 + t6 + t7 + t8 + t9 + t10 + t11 + t12;

	fillScreen(BLACK);

	setFont(Font_11x18);
	setTextColor(RED, BLACK);
	sprintf(buffer, "Fillscreen:         %lums", t1);
	writeString(0, 0, buffer);

	sprintf(buffer, "Text:               %lums", t2);
	writeString(0, 18, buffer);

	sprintf(buffer, "Lines:              %lums", t3);
	writeString(0, 36, buffer);

	sprintf(buffer, "Hor/Vert Lines:     %lums", t4);
	writeString(0, 54, buffer);

	sprintf(buffer, "Rect(outline):      %lums", t5);
	writeString(0, 72, buffer);

	sprintf(buffer, "Rect(filled):       %lums", t6);
	writeString(0, 90, buffer);

	sprintf(buffer, "Circ(outline):      %lums", t7);
	writeString(0, 108, buffer);

	sprintf(buffer, "Circ(filled):       %lums", t8);
	writeString(0, 126, buffer);

	sprintf(buffer, "Tri(outline):       %lums", t9);
	writeString(0, 144, buffer);

	sprintf(buffer, "Tri(filled):        %lums", t10);
	writeString(0, 162, buffer);

	sprintf(buffer, "Round Rects:        %lums", t11);
	writeString(0, 180, buffer);

	sprintf(buffer, "Filled Round Rects: %lums", t12);
	writeString(0, 198, buffer);

	sprintf(buffer, "Time total:         %lums", total);
	writeString(0, 216, buffer);

	HAL_Delay(4000);
}
