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
TFTLIB_8BIT::TFTLIB_8BIT(GPIO_TypeDef *parallel_bus, uint8_t drv, GPIO_TypeDef *GPIO_RD_PORT, uint16_t GPIO_RD_PIN, GPIO_TypeDef *GPIO_WR_PORT, uint16_t GPIO_WR_PIN, GPIO_TypeDef *GPIO_DC_PORT, uint16_t GPIO_DC_PIN, GPIO_TypeDef *GPIO_CS_PORT, uint16_t GPIO_CS_PIN) {
	_PARALLEL_PORT = parallel_bus;

	CS_PORT	 = GPIO_CS_PORT;
	CS_PIN	 = GPIO_CS_PIN;

	DC_PORT	 = GPIO_DC_PORT;
	DC_PIN	 = GPIO_DC_PIN;

	WR_PORT = GPIO_WR_PORT;
	WR_PIN  = GPIO_WR_PIN;

	RD_PORT = GPIO_RD_PORT;
	RD_PIN  = GPIO_RD_PIN;

	_type = drv;

	PIN_OUTPUT(_PARALLEL_PORT, 0x00FF);

	RD_H();
}

/***************************************************************************************
** Function name:           ~TFTLIB_8BIT
** Description:             TFTLIB_8BIT Destructor
***************************************************************************************/
TFTLIB_8BIT::~TFTLIB_8BIT() {
	delete[] _buffer;
}

uint16_t TFTLIB_8BIT::width(void){
	return _width;
}

uint16_t TFTLIB_8BIT::height(void){
	return _height;
}

uint16_t TFTLIB_8BIT::readID(void) {
	uint8_t data1, data2;
	uint16_t id;

	switch(_type) {
		case NT35510_PARALLEL:
			writeCommand16(0xDB00);
			PIN_INPUT(GPIOB, 0x00FF);
			readByte();
			data1 = readByte();
			PIN_OUTPUT(GPIOB, 0x00FF);

			writeCommand16(0xDC00);
			PIN_INPUT(GPIOB, 0x00FF);
			readByte();
			data2 = readByte();
			PIN_OUTPUT(GPIOB, 0x00FF);

			id = data1 << 8 | data2;
		break;

		default:
			writeCommand8(0xD3);

			PIN_INPUT(GPIOB, 0x00FF);

			readByte();
			readByte();
			data1 = readByte();
			data2 = readByte();
			PIN_OUTPUT(GPIOB, 0x00FF);

			id = data1 << 8 | data2;
		break;
	}

	return id;
}

/***************************************************************************************
** Function name:           init
** Description:             Init display with selected driver
***************************************************************************************/
void TFTLIB_8BIT::init(void){
	_display_width  = 480;
	_display_height = 800;

	if(_type == ILI9341_PARALLEL) {
		_display_width  = 240;
		_display_height = 320;
		#include "lcd_drivers/ili9341_parallel_drv.h"
	}

	else if(_type == ILI9327_PARALLEL) {
		_display_width  = 240;
		_display_height = 400;
		#include "lcd_drivers/ili9327_drv.h"
	}

	else if(_type == NT35510_PARALLEL) {
		_display_width  = 480;
		_display_height = 800;
		#include "lcd_drivers/nt35510_parallel_drv.h"
	}

	_tx0 = _tx1 = _ty0 = _ty1 = 0xFFFFFFFF;

	setRotation(3);
	fillScreen(GREEN);
	setFreeFont(&FreeSerif9pt7b);
	setCursor(0, 0);

	textfont  = 1;
	textsize  = 1;
	textcolor   =  0xFFFF; // White
	textbgcolor =  0x0000; // Black
	padX = 0;             // No padding
	isDigits   = false;   // No bounding box adjustment
	textwrapX  = true;    // Wrap text at end of line when using print stream
	textwrapY  = true;    // Wrap text at bottom of screen when using print stream
	textdatum = TL_DATUM; // Top Left text alignment is default
	_utf8     = true;
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

	b = (_PARALLEL_PORT->IDR & 0x00FF);

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

void TFTLIB_8BIT::writeCommand16 (uint16_t cmmd) {
	DC_L();
	CS_L();

	write16(cmmd);

	CS_H();
	DC_H();
}

/***************************************************************************************
** Function name:           writeData8
** Description:             Write 8bit data to parallel display
***************************************************************************************/
void TFTLIB_8BIT::writeData8(uint8_t *data, uint32_t len) {
	CS_L();
	DC_H();

	while(len > 31) {
		write8(*data++); write8(*data++); write8(*data++); write8(*data++);
		write8(*data++); write8(*data++); write8(*data++); write8(*data++);
		write8(*data++); write8(*data++); write8(*data++); write8(*data++);
		write8(*data++); write8(*data++); write8(*data++); write8(*data++);
		write8(*data++); write8(*data++); write8(*data++); write8(*data++);
		write8(*data++); write8(*data++); write8(*data++); write8(*data++);
		write8(*data++); write8(*data++); write8(*data++); write8(*data++);
		write8(*data++); write8(*data++); write8(*data++); write8(*data++);
		len-=32;
	}

	while(len > 7) {
		write8(*data++); write8(*data++); write8(*data++); write8(*data++);
		write8(*data++); write8(*data++); write8(*data++); write8(*data++);
		len -= 8;
	}

	while(len-- > 0) write8(*data++);

	CS_H();
}

/***************************************************************************************
** Function name:           writeData16
** Description:             Write 16bit data to parallel display
***************************************************************************************/
void TFTLIB_8BIT::writeData16(uint16_t *data, uint32_t len) {
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

	write16(data);

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
	_rotation = m;

	if(_type == ILI9327_PARALLEL) {
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

	else if(_type == ILI9341_PARALLEL) {
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

	else if(_type == NT35510_PARALLEL) {
		writeCommand16(0x3600);
		switch (m) {
			case 0:
				writeSmallData16(MADCTL_RGB);
				_width  = _display_width;
				_height = _display_height;
			break;

			case 1:
				writeSmallData16(MADCTL_MX | MADCTL_MV | MADCTL_RGB);
				_width  = _display_height;
				_height = _display_width;
			break;

			case 2:
				writeSmallData16(MADCTL_MX | MADCTL_MY | MADCTL_RGB);
				_width  = _display_width;
				_height = _display_height;
			break;

			case 3:
				writeSmallData16(MADCTL_MV | MADCTL_MY | MADCTL_RGB);
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
	return (((fgc * alpha) + (bgc * (255 - alpha))) >> 8);
}

/***************************************************************************************
** Function name:           setWindow8
** Description:             Set start/end address of drawed window (For parallel display)
***************************************************************************************/
void TFTLIB_8BIT::setWindow8(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
	CS_L();

	switch(_type) {
		case NT35510_PARALLEL:
			if(_tx0 != x0 || _tx1 != x1) {
				CS_L();
				DC_L();
				write16(0x2A00);
				DC_H();
				write8(0x00);
				write8(Byte8H(x0));

				DC_L();
				write16(0x2A01);
				DC_H();
				write8(0x00);
				write8(Byte8L(x0));

				DC_L();
				write16(0x2A02);
				DC_H();
				write8(0x00);
				write8(Byte8H(x1));

				DC_L();
				write16(0x2A03);
				DC_H();
				write8(0x00);
				write8(Byte8L(x1));
				_tx0 = x0;
				_tx1 = x1;
			}

			if(_ty0 != y0 || _ty1 != y1) {
				DC_L();
				write16(0x2B00);
				DC_H();
				write8(0x00);
				write8(Byte8H(y0));

				DC_L();
				write16(0x2B01);
				DC_H();
				write8(0x00);
				write8(Byte8L(y0));

				DC_L();
				write16(0x2B02);
				DC_H();
				write8(0x00);
				write8(Byte8H(y1));

				DC_L();
				write16(0x2B03);
				DC_H();
				write8(0x00);
				write8(Byte8L(y1));
				_ty0 = y0;
				_ty1 = y1;
			}

			DC_L();
			write16(0x2C00);
			DC_H();
			CS_H();
		break;

		case ILI9327_PARALLEL:
		    if (_rotation == 2) y0 += 32, y1 += 32;
		    if (_rotation == 3) x0 += 32, x1 += 32;

			if (_tx0 != x0 || _tx1 != x1) {
				writeCommand8(CASET);
				writeSmallData32(((uint32_t) x0 << 16) | x1);
				_tx0 = x0;
				_tx1 = x1;
			}

			if (_ty0 != y0 || _ty1 != y1) {
				writeCommand8(RASET);
				writeSmallData32(((uint32_t) y0 << 16) | y1);
				_ty0 = y0;
				_ty1 = y1;
			}

			writeCommand8(RAMWR);
		break;

		case ILI9341_PARALLEL:
			if (_tx0 != x0 || _tx1 != x1) {
				writeCommand8(CASET);
				writeSmallData32(((uint32_t) x0 << 16) | x1);
				_tx0 = x0;
				_tx1 = x1;
			}

			if (_ty0 != y0 || _ty1 != y1) {
				writeCommand8(RASET);
				writeSmallData32(((uint32_t) y0 << 16) | y1);
				_ty0 = y0;
				_ty1 = y1;
			}

			writeCommand8(RAMWR);
		break;
	}
}

/***************************************************************************************
** Function name:           setWindow8
** Description:             Set start/end address of drawed window (For parallel display)
***************************************************************************************/
void TFTLIB_8BIT::readWindow8(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
	if ((x0 > x1) || (x1 > _width) || (y0 > y1) || (y1 > _height)) return;

	CS_L();

	switch(_type) {
		case NT35510_PARALLEL:
			if(_tx0 != x0 || _tx1 != x1) {
				CS_L();
				DC_L();
				write16(0x2A00);
				DC_H();
				write8(0x00);
				write8(Byte8H(x0));

				DC_L();
				write16(0x2A01);
				DC_H();
				write8(0x00);
				write8(Byte8L(x0));

				DC_L();
				write16(0x2A02);
				DC_H();
				write8(0x00);
				write8(Byte8H(x1));

				DC_L();
				write16(0x2A03);
				DC_H();
				write8(0x00);
				write8(Byte8L(x1));
				_tx0 = x0;
				_tx1 = x1;
			}

			if(_ty0 != y0 || _ty1 != y1) {
				DC_L();
				write16(0x2B00);
				DC_H();
				write8(0x00);
				write8(Byte8H(y0));

				DC_L();
				write16(0x2B01);
				DC_H();
				write8(0x00);
				write8(Byte8L(y0));

				DC_L();
				write16(0x2B02);
				DC_H();
				write8(0x00);
				write8(Byte8H(y1));

				DC_L();
				write16(0x2B03);
				DC_H();
				write8(0x00);
				write8(Byte8L(y1));
				_ty0 = y0;
				_ty1 = y1;
			}

			DC_L();
			write16(0x2E00);
			DC_H();
			CS_H();
		break;

		case ILI9327_PARALLEL:
		    if (_rotation == 2) y0 += 32, y1 += 32;
		    if (_rotation == 3) x0 += 32, x1 += 32;

			if (_tx0 != x0 || _tx1 != x1) {
				writeCommand8(CASET);
				writeSmallData32(((uint32_t) x0 << 16) | x1);
				_tx0 = x0;
				_tx1 = x1;
			}

			if (_ty0 != y0 || _ty1 != y1) {
				writeCommand8(RASET);
				writeSmallData32(((uint32_t) y0 << 16) | y1);
				_ty0 = y0;
				_ty1 = y1;
			}

			writeCommand8(RAMRD);
		break;

		case ILI9341_PARALLEL:
			if (_tx0 != x0 || _tx1 != x1) {
				writeCommand8(CASET);
				writeSmallData32(((uint32_t) x0 << 16) | x1);
				_tx0 = x0;
				_tx1 = x1;
			}

			if (_ty0 != y0 || _ty1 != y1) {
				writeCommand8(RASET);
				writeSmallData32(((uint32_t) y0 << 16) | y1);
				_ty0 = y0;
				_ty1 = y1;
			}

			writeCommand8(RAMRD);
		break;
	}
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
	setWindow8(0, 0, _width - 1, _height - 1);
	pushBlock16(color, _width * _height);
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

	setTextColor(RED);
	setTextSize(1);
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
** Function name:           readPixel (PARALLEL DISPLAY ONLY!)
** Description:             Read single pixel at coords x&y
***************************************************************************************/
uint16_t TFTLIB_8BIT::readPixel(int32_t x0, int32_t y0)
{
	uint8_t data[3];

	readWindow8(x0, y0, x0, y0);

	// Set masked pins D0- D7 to input
	PIN_INPUT(_PARALLEL_PORT, 0x00FF);

	readByte();
	data[0] = readByte();
	data[1] = readByte();
	data[2] = readByte();

	// Set masked pins D0- D7 to output
	PIN_OUTPUT(_PARALLEL_PORT, 0x00FF);

	return (((data[0] & 0xF8) << 8) | ((data[1] & 0xFC) << 3) | (data[2]  >> 3));
}

/***************************************************************************************
** Function name:           drawPixel
** Description:             Draw single pixel at coords x&y
***************************************************************************************/
void TFTLIB_8BIT::drawPixel(int32_t x, int32_t y, uint16_t color)
{
	if((x < 0) || (y < 0) ||(x >= _width) || (y >= _height)) return;

	setWindow8(x, y, x, y);
	pushBlock16(color, 1);
}

/***************************************************************************************
** Function name:           drawPixel (alpha blended)
** Description:             Draw a pixel blended with the screen or bg pixel colour
***************************************************************************************/
uint16_t TFTLIB_8BIT::drawPixel(int32_t x, int32_t y, uint32_t color, uint8_t alpha, uint32_t bg_color)
{
	if (bg_color == 0x00FFFFFF) bg_color = readPixel(x, y);
	color = alphaBlend(alpha, color, bg_color);
	drawPixel(x, y, color);
	return color;
}

/***************************************************************************************
** Function name:           drawPixelAlpha
** Description:             Draw single pixel with alpha channel
***************************************************************************************/
void TFTLIB_8BIT::drawPixelAlpha(int16_t x, int16_t y, uint16_t color, float alpha) {
	if ((x < 0) || (x >= _width) || (y < 0) || (y >= _height))	return;

	uint16_t px = readPixel(x, y);
	drawPixel(x, y, alphaBlend((uint8_t)alpha, color, px));
}

/***************************************************************************************
** Function name:           drawSpot - maths intensive, so for small filled circles
** Description:             Draw an anti-aliased filled circle at ax,ay with radius r
***************************************************************************************/
void TFTLIB_8BIT::drawSpot(float ax, float ay, float r, uint32_t color) {
	// Filled circle can be created by the wide line function with zero line length
	drawWedgeLine( ax, ay, ax, ay, r, r, color, 0x00FFFFFF);
}

/***************************************************************************************
** Function name:           drawFastHLine
** Description:             Fast drawing Horizontal Line
***************************************************************************************/
void TFTLIB_8BIT::drawFastHLine(int32_t x, int32_t y, int32_t w, uint16_t color) {
	if ((y < 0) || (x >= _width) || (y >= _height)) return;
	if (x < 0) { w += x; x = 0; }
	if ((x + w) > _width)  w = _width  - x;
	if (w < 1) return;

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
		_buffer[i] = alphaBlend(alpha, color, readPixel(x + i, y));
		i++;
	}

	setWindow8(x, y, x + w, y);
	writeData16(_buffer, w);
}

/***************************************************************************************
** Function name:           drawFastVLine
** Description:             Drawing Vertical Line
***************************************************************************************/
void TFTLIB_8BIT::drawFastVLine(int32_t x, int32_t y, int32_t h, uint16_t color) {
	if ((x < 0) || (x >= _width) || (y >= _height)) return;
	if (y < 0) { h += y; y = 0; }
	if ((y + h) > _height) h = _height - y;
	if (h < 1) return;

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
		_buffer[i] = alphaBlend(alpha, color, readPixel(x + i, y));
		i++;
	}

	setWindow8(x, y, x, y + h - 1);
	writeData16(_buffer, h);
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
void TFTLIB_8BIT::drawWideLine(float ax, float ay, float bx, float by, float wd, uint16_t fg_color, uint16_t bg_color) {
	drawWedgeLine( ax, ay, bx, by, wd/2.0, wd/2.0, fg_color, bg_color);
}

/***************************************************************************************
** Function name:           drawWedgeLine
** Description:             Draw anti-aliased line with single color
***************************************************************************************/
void TFTLIB_8BIT::drawWedgeLine(float ax, float ay, float bx, float by, float ar, float br, uint32_t fg_color, uint32_t bg_color)
{
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

	uint16_t bg = bg_color;
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
			if (!endX) { endX = true; xs = xp; }
			if (alpha > HiAlphaTheshold) {
				if (swin) {
					setWindow8(xp, yp, width()-1, yp);
					swin = false;
				}

				pushBlock16(fg_color);
				continue;
			}
			//Blend color with background and plot
			if (bg_color == 0x00FFFFFF) {
				bg = readPixel(xp, yp); swin = true;
			}

			if (swin) {
				setWindow8(xp, yp, width()-1, yp);
				swin = false;
			}

			pushBlock16(alphaBlend((uint8_t)(alpha * PixelAlphaGain), fg_color, bg));
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
			if (!endX) { endX = true; xs = xp; }
			if (alpha > HiAlphaTheshold) {
				if (swin) { setWindow8(xp, yp, width()-1, yp); swin = false; }
				pushBlock16(fg_color);
				continue;
			}
			//Blend color with background and plot
			if (bg_color == 0x00FFFFFF) {
			bg = readPixel(xp, yp); swin = true;
			}
			if (swin) { setWindow8(xp, yp, width()-1, yp); swin = false; }
			pushBlock16(alphaBlend((uint8_t)(alpha * PixelAlphaGain), fg_color, bg));
		}
	}
}

/***************************************************************************************
** Function name:           fillSmoothCircle
** Description:             Draw a filled anti-aliased circle
***************************************************************************************/
void TFTLIB_8BIT::fillSmoothCircle(int32_t x, int32_t y, int32_t r, uint32_t color, uint32_t bg_color)
{
	if (r <= 0) return;

	drawFastHLine(x - r, y, 2 * r + 1, color);
	int32_t xs = 1;
	int32_t cx = 0;

	int32_t r1 = r * r;
	r++;
	int32_t r2 = r * r;

	for (int32_t cy = r - 1; cy > 0; cy--) {
		int32_t dy2 = (r - cy) * (r - cy);
		for (cx = xs; cx < r; cx++) {
			int32_t hyp2 = (r - cx) * (r - cx) + dy2;
			if (hyp2 <= r1) break;
			if (hyp2 >= r2) continue;
			float alphaf = (float)r - sqrtf(hyp2);
			if (alphaf > HiAlphaTheshold) break;
			xs = cx;
			if (alphaf < LoAlphaTheshold) continue;
			uint8_t alpha = alphaf * 255;

			if (bg_color == 0x00FFFFFF) {
				drawPixel(x + cx - r, y + cy - r, color, alpha, bg_color);
				drawPixel(x - cx + r, y + cy - r, color, alpha, bg_color);
				drawPixel(x - cx + r, y - cy + r, color, alpha, bg_color);
				drawPixel(x + cx - r, y - cy + r, color, alpha, bg_color);
			}

			else {
				uint16_t pcol = drawPixel(x + cx - r, y + cy - r, color, alpha, bg_color);
				drawPixel(x - cx + r, y + cy - r, pcol);
				drawPixel(x - cx + r, y - cy + r, pcol);
				drawPixel(x + cx - r, y - cy + r, pcol);
			}
		}
		drawFastHLine(x + cx - r, y + cy - r, 2 * (r - cx) + 1, color);
		drawFastHLine(x + cx - r, y - cy + r, 2 * (r - cx) + 1, color);
	}
}


/***************************************************************************************
** Function name:           fillSmoothRoundRect
** Description:             Draw a filled anti-aliased rounded corner rectangle
***************************************************************************************/
void TFTLIB_8BIT::fillSmoothRoundRect(int32_t x, int32_t y, int32_t w, int32_t h, int32_t r, uint32_t color, uint32_t bg_color)
{
	int32_t xs = 0;
	int32_t cx = 0;

	// Limit radius to half width or height
	if (r > w/2) r = w/2;
	if (r > h/2) r = h/2;

	y += r;
	h -= 2*r;

	fillRect(x, y, w, h, color);

	h--;
	x += r;
	w -= 2*r+1;
	int32_t r1 = r * r;

	r++;
	int32_t r2 = r * r;

	for (int32_t cy = r - 1; cy > 0; cy--) {
		int32_t dy2 = (r - cy) * (r - cy);
		for (cx = xs; cx < r; cx++) {
			int32_t hyp2 = (r - cx) * (r - cx) + dy2;

			if (hyp2 <= r1) break;
			if (hyp2 >= r2) continue;

			float alphaf = (float)r - sqrtf(hyp2);
			if (alphaf > HiAlphaTheshold) break;
			xs = cx;

			if (alphaf < LoAlphaTheshold) continue;
			uint8_t alpha = alphaf * 255;

			drawPixel(x + cx - r, y + cy - r, color, alpha, bg_color);
			drawPixel(x - cx + r + w, y + cy - r, color, alpha, bg_color);
			drawPixel(x - cx + r + w, y - cy + r + h, color, alpha, bg_color);
			drawPixel(x + cx - r, y - cy + r + h, color, alpha, bg_color);
		}

		drawFastHLine(x + cx - r, y + cy - r, 2 * (r - cx) + 1 + w, color);
		drawFastHLine(x + cx - r, y - cy + r + h, 2 * (r - cx) + 1 + w, color);
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
void TFTLIB_8BIT::fillRect(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t color) {
	if ((x >= _width) || (y >= _height)) return;
	if (x < 0) { w += x; x = 0; }
	if (y < 0) { h += y; y = 0; }
	if ((x + w) > _width)  w = _width  - x;
	if ((y + h) > _height) h = _height - y;

	if ((w < 1) || (h < 1)) return;

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
	drawWideLine(x, y + h - 1, x + w - 1, y + h - 1, 1, color);

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

	for(uint16_t j = 0; j < h; j++) {

		for(uint16_t i = 0; i < w; i++) {
			_buffer[i] = readPixel(x + i, y + j);
		}

		for(uint16_t i = 0; i < w; i++) {
			_buffer[i] = alphaBlend(alpha, color, _buffer[i]);
		}

		setWindow8(x, y + j, x + w - 1, y + j);
		writeData16(_buffer, w);
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
void TFTLIB_8BIT::fillEllipse(int16_t x0, int16_t y0, int32_t rx, int32_t ry, uint16_t color) {
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
void TFTLIB_8BIT::drawBitmap(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t *bitmap, uint16_t color) {
	if(x < 0 || y < 0 || w < 0 || h < 0 || x > _width || y > _height || x + w > _width || y + h > _height) return;

	int32_t i, j, byteWidth = (w + 7) / 8;

	for (j = 0; j < h; j++) {
		for (i = 0; i < w; i++ ) {
			if (read_byte(bitmap + j * byteWidth + i / 8) & (128 >> (i & 7))) {
				drawPixel(x + i, y + j, color);
			}
		}
	}
}

/***************************************************************************************
** Function name:           drawBitmap
** Description:             Draw an image stored in an array on the TFT
***************************************************************************************/
void TFTLIB_8BIT::drawBitmap(int16_t x, int16_t y, int16_t w, int16_t h, const uint8_t *bitmap, uint16_t color) {
	if(x < 0 || y < 0 || w < 0 || h < 0 || x > _width || y > _height || x + w > _width || y + h > _height) return;

	int32_t i, j, byteWidth = (w + 7) / 8;

	for (j = 0; j < h; j++) {
		for (i = 0; i < w; i++ ) {
			if (read_byte(bitmap + j * byteWidth + i / 8) & (128 >> (i & 7))) {
				drawPixel(x + i, y + j, color);
			}
		}
	}
}


/***************************************************************************************
** Function name:           drawBitmap
** Description:             Draw an image stored in an array on the TFT
***************************************************************************************/
void TFTLIB_8BIT::drawBitmap(int16_t x, int16_t y, int16_t w, int16_t h, const uint8_t *bitmap, uint16_t fgcolor, uint16_t bgcolor)
{
	if(x < 0 || y < 0 || w < 0 || h < 0 || x > _width || y > _height || x + w > _width || y + h > _height) return;

	int32_t i, j, byteWidth = (w + 7) / 8;

	for (j = 0; j < h; j++) {
		for (i = 0; i < w; i++ ) {
			if (read_byte(bitmap + j * byteWidth + i / 8) & (128 >> (i & 7)))  drawPixel(x + i, y + j, fgcolor);
			else drawPixel(x + i, y + j, bgcolor);
		}
	}
}

/***************************************************************************************
** Function name:           drawXBitmap
** Description:             Draw an image stored in an XBM array onto the TFT
***************************************************************************************/
void TFTLIB_8BIT::drawXBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color)
{
	int32_t i, j, byteWidth = (w + 7) / 8;

	for (j = 0; j < h; j++) {
		for (i = 0; i < w; i++ ) {
			if (read_byte(bitmap + j * byteWidth + i / 8) & (1 << (i & 7))) {
				drawPixel(x + i, y + j, color);
			}
		}
	}
}


/***************************************************************************************
** Function name:           drawXBitmap
** Description:             Draw an XBM image with foreground and background colors
***************************************************************************************/
void TFTLIB_8BIT::drawXBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color, uint16_t bgcolor)
{
	int32_t i, j, byteWidth = (w + 7) / 8;

	for (j = 0; j < h; j++) {
		for (i = 0; i < w; i++ ) {
			if (read_byte(bitmap + j * byteWidth + i / 8) & (1 << (i & 7)))
				drawPixel(x + i, y + j,   color);
			else drawPixel(x + i, y + j, bgcolor);
		}
	}
}

/***************************************************************************************
** Function name:           decodeUTF8
** Description:             Serial UTF-8 decoder with fall-back to extended ASCII
*************************************************************************************x*/
uint16_t TFTLIB_8BIT::decodeUTF8(uint8_t c) {
	// 7 bit Unicode Code Point
	if ((c & 0x80) == 0x00) {
		decoderState = 0;
		return (uint16_t)c;
	}

	if (decoderState == 0) {
		// 11 bit Unicode Code Point
		if ((c & 0xE0) == 0xC0) {
			decoderBuffer = ((c & 0x1F)<<6);
			decoderState = 1;
			return 0;
		}
		// 16 bit Unicode Code Point
		if ((c & 0xF0) == 0xE0) {
			decoderBuffer = ((c & 0x0F)<<12);
			decoderState = 2;
			return 0;
		}
	}

	else {
		if (decoderState == 2) {
			decoderBuffer |= ((c & 0x3F)<<6);
			decoderState--;
			return 0;
		}
		else {
			decoderBuffer |= (c & 0x3F);
			decoderState = 0;
			return decoderBuffer;
		}
	}

	decoderState = 0;

	return (uint16_t)c; // fall-back to extended ASCII
}


/***************************************************************************************
** Function name:           decodeUTF8
** Description:             Line buffer UTF-8 decoder with fall-back to extended ASCII
*************************************************************************************x*/
uint16_t TFTLIB_8BIT::decodeUTF8(uint8_t *buf, uint16_t *index, uint16_t remaining) {
	uint16_t c = buf[(*index)++];
	//Serial.print("Byte from string = 0x"); Serial.println(c, HEX);

	// 7 bit Unicode
	if ((c & 0x80) == 0x00) return c;

	// 11 bit Unicode
	if (((c & 0xE0) == 0xC0) && (remaining > 1))
	return ((c & 0x1F)<<6) | (buf[(*index)++]&0x3F);

	// 16 bit Unicode
	if (((c & 0xF0) == 0xE0) && (remaining > 2)) {
		c = ((c & 0x0F)<<12) | ((buf[(*index)++]&0x3F)<<6);
		return  c | ((buf[(*index)++]&0x3F));
	}

	return c; // fall-back to extended ASCII
}

int16_t TFTLIB_8BIT::textWidth(const char *string) {
	return textWidth(string, textfont);
}

int16_t TFTLIB_8BIT::textWidth(const char *string, uint8_t font) {
	int32_t str_width = 0;
	uint16_t uniCode  = 0;

	if(gfxFont) { // New font
		while (*string) {
			uniCode = decodeUTF8(*string++);
			if ((uniCode >= read_word(&gfxFont->first)) && (uniCode <= read_word(&gfxFont->last ))) {
				uniCode -= read_word(&gfxFont->first);
				GFXglyph *glyph  = &(((GFXglyph *)read_dword(&gfxFont->glyph))[uniCode]);
				// If this is not the  last character or is a digit then use xAdvance
				if (*string  || isDigits) str_width += read_byte(&glyph->xAdvance);
				// Else use the offset plus width since this can be bigger than xAdvance
				else str_width += ((int8_t)read_byte(&glyph->xOffset) + read_byte(&glyph->width));
			}
		}
	}
	else {
		while (*string++) str_width += 6;
	}
	isDigits = false;
	return str_width * textsize;
}

/***************************************************************************************
** Function name:           getCursor
** Description:             Get the text cursor x & y position
***************************************************************************************/
void TFTLIB_8BIT::getCursor(int16_t* x, int16_t* y) {
	 *x = cursor_x;
	 *y = cursor_y;
}

/***************************************************************************************
** Function name:           setCursor
** Description:             Set the text cursor x,y position
***************************************************************************************/
void TFTLIB_8BIT::setCursor(int16_t x, int16_t y) {
	cursor_x = x;
	cursor_y = y + gfxFont->yAdvance;
}

/***************************************************************************************
** Function name:           setTextSize
** Description:             Set the text size multiplier
***************************************************************************************/
void TFTLIB_8BIT::setTextSize(uint8_t s) {
	if (s>7) s = 7; // Limit the maximum size multiplier so byte variables can be used for rendering
	textsize = (s > 0) ? s : 1; // Don't allow font size 0
}

/***************************************************************************************
** Function name:           setFreeFont
** Descriptions:            Sets the GFX free font to use
***************************************************************************************/

void TFTLIB_8BIT::setFreeFont(const GFXfont *f) {
	textfont = 1;
	gfxFont = (GFXfont *)f;

	glyph_ab = 0;
	glyph_bb = 0;
	uint16_t numChars = read_word(&gfxFont->last) - read_word(&gfxFont->first);

	// Find the biggest above and below baseline offsets
	for (uint8_t c = 0; c < numChars; c++) {
		GFXglyph *glyph1  = &(((GFXglyph *)read_dword(&gfxFont->glyph))[c]);
		int8_t ab = -read_byte(&glyph1->yOffset);
		if (ab > glyph_ab) glyph_ab = ab;
		int8_t bb = read_byte(&glyph1->height) - ab;
		if (bb > glyph_bb) glyph_bb = bb;
	}
	setTextDatum(TL_DATUM);
}

/***************************************************************************************
** Function name:           setTextColor
** Description:             Set the font foreground colour (background is transparent)
***************************************************************************************/
void TFTLIB_8BIT::setTextColor(uint16_t c) {
	// For 'transparent' background, we'll set the bg
	// to the same as fg instead of using a flag
	textcolor = textbgcolor = c;
}

/***************************************************************************************
** Function name:           setTextWrap
** Description:             Define if text should wrap at end of line
***************************************************************************************/
void TFTLIB_8BIT::setTextWrap(bool wrapX, bool wrapY) {
	textwrapX = wrapX;
	textwrapY = wrapY;
}

/***************************************************************************************
** Function name:           setTextColor
** Description:             Set the font foreground and background colour
***************************************************************************************/
void TFTLIB_8BIT::setTextColor(uint16_t c, uint16_t b) {
	textcolor   = c;
	textbgcolor = b;
}

/***************************************************************************************
** Function name:           setTextDatum
** Description:             Set the text position reference datum
***************************************************************************************/
void TFTLIB_8BIT::setTextDatum(uint8_t d) {
	textdatum = d;
}


/***************************************************************************************
** Function name:           setTextPadding
** Description:             Define padding width (aids erasing old text and numbers)
***************************************************************************************/
void TFTLIB_8BIT::setTextPadding(uint16_t x_width) {
	padX = x_width;
}

int16_t TFTLIB_8BIT::fontHeight(int16_t font) {
	return read_byte(&gfxFont->yAdvance) * textsize;
}

int16_t TFTLIB_8BIT::fontHeight(void) {
	return fontHeight(textfont);
}

/***************************************************************************************
** Function name:           write
** Description:             draw characters piped through serial stream
***************************************************************************************/
size_t TFTLIB_8BIT::write(uint8_t utf8) {
	if (utf8 == '\r') return 1;

	uint16_t uniCode = utf8;

	if (_utf8) uniCode = decodeUTF8(utf8);

	if (uniCode == 0) return 1;

	if (uniCode == '\n') uniCode+=22; // Make it a valid space character to stop errors
	else if (uniCode < 32) return 1;

	if(utf8 == '\n') {
		cursor_x  = 0;
		cursor_y += (int16_t)textsize *
		(uint8_t)read_byte(&gfxFont->yAdvance);
	}
	else {
		if (uniCode > read_word(&gfxFont->last )) return 1;
		if (uniCode < read_word(&gfxFont->first)) return 1;

		uint16_t   c2    = uniCode - read_word(&gfxFont->first);
		GFXglyph *glyph = &(((GFXglyph *)read_dword(&gfxFont->glyph))[c2]);
		uint8_t   w     = read_byte(&glyph->width),
		h     = read_byte(&glyph->height);
		if((w > 0) && (h > 0)) { // Is there an associated bitmap?
			int16_t xo = (int8_t)read_byte(&glyph->xOffset);
			if(textwrapX && ((cursor_x + textsize * (xo + w)) > _width)) {
				// Drawing character would go off right edge; wrap to new line
				cursor_x  = 0;
				cursor_y += (int16_t)textsize *
				(uint8_t)read_byte(&gfxFont->yAdvance);
			}
			if (textwrapY && (cursor_y >= (int32_t)_height)) cursor_y = 0;
			drawChar(cursor_x, cursor_y, uniCode, textcolor, textbgcolor, textsize);
		}
		cursor_x += read_byte(&glyph->xAdvance) * (int16_t)textsize;
	}
	return 1;
}


/***************************************************************************************
** Function name:           drawChar
** Description:             draw a Unicode glyph onto the screen
***************************************************************************************/
// Any UTF-8 decoding must be done before calling drawChar()
int16_t TFTLIB_8BIT::drawChar(uint16_t uniCode, int32_t x, int32_t y) {
	if (!uniCode) return 0;

	drawChar(x, y, uniCode, textcolor, textbgcolor, textsize);
	if((uniCode >= read_word(&gfxFont->first)) && (uniCode <= read_word(&gfxFont->last) )) {
		uint16_t   c2    = uniCode - read_word(&gfxFont->first);
		GFXglyph *glyph = &(((GFXglyph *)read_dword(&gfxFont->glyph))[c2]);
		return read_byte(&glyph->xAdvance) * textsize;
	}
	else return 0;

	if (((uniCode < 32) || (uniCode > 127))) return 0;

	int32_t width  = 0;
	int32_t height = 0;
	uint32_t flash_address = 0;
	uniCode -= 32;

	int32_t w = width;
	int32_t pY      = y;
	uint8_t line = 0;

	w *= height; // Now w is total number of pixels in the character
	if ((textsize != 1) || (textcolor == textbgcolor)) {
		if (textcolor != textbgcolor) fillRect(x, pY, width * textsize, textsize * height, textbgcolor);
		int32_t px = 0, py = pY; // To hold character block start and end column and row values
		int32_t pc = 0; // Pixel count
		uint8_t np = textsize * textsize; // Number of pixels in a drawn pixel

		uint8_t tnp = 0; // Temporary copy of np for while loop
		uint8_t ts = textsize - 1; // Temporary copy of textsize
		// 16 bit pixel count so maximum font size is equivalent to 180x180 pixels in area
		// w is total number of pixels to plot to fill character block
		while (pc < w) {
			line = read_byte((uint8_t *)flash_address);
			flash_address++;
			if (line & 0x80) {
				line &= 0x7F;
				line++;
				if (ts) {
					px = x + textsize * (pc % width); // Keep these px and py calculations outside the loop as they are slow
					py = y + textsize * (pc / width);
				}
				else {
					px = x + pc % width; // Keep these px and py calculations outside the loop as they are slow
					py = y + pc / width;
				}
				while (line--) { // In this case the while(line--) is faster
					pc++; // This is faster than putting pc+=line before while()?
					setWindow8(px, py, px + ts, py + ts);

					if (ts) {
						tnp = np;
						while (tnp--) {write16(textcolor);}
					}
					else write16(textcolor);

					px += textsize;

					if (px >= (x + width * textsize)) {
						px = x;
						py += textsize;
					}
				}
			}
			else {
				line++;
				pc += line;
			}
		}
	}
	else {
		// Text colour != background && textsize = 1 and character is within screen area
		// so use faster drawing of characters and background using block write
		if ((x >= 0) && (x + width <= _width) && (y >= 0) && (y + height <= _height)) {
			setWindow8(x, y, x + width - 1, y + height - 1);

			// Maximum font size is equivalent to 180x180 pixels in area
			while (w > 0) {
				line = read_byte((uint8_t *)flash_address++); // 8 bytes smaller when incrementing here
				if (line & 0x80) {
					line &= 0x7F;
					line++; w -= line;
					pushBlock16(textcolor,line);
				}
				else {
					line++; w -= line;
					pushBlock16(textbgcolor,line);
				}
			}
		}
		else {
			int32_t px = x, py = y;  // To hold character block start and end column and row values
			int32_t pc = 0;          // Pixel count
			int32_t pl = 0;          // Pixel line length
			uint16_t pcol = 0;       // Pixel color

			while (pc < w) {
				line = read_byte((uint8_t *)flash_address);
				flash_address++;
				if (line & 0x80) { pcol = textcolor; line &= 0x7F; }
				else pcol = textbgcolor;
				line++;
				px = x + pc % width;
				py = y + pc / width;

				pl = 0;
				pc += line;
				while (line--) { // In this case the while(line--) is faster
					pl++;
					if ((px+pl) >= (x + width)) {
						drawFastHLine(px, py, pl, pcol);
						pl = 0;
						px = x;
						py ++;
					}
				}
				if (pl)drawFastHLine(px, py, pl, pcol);
			}
		}
	}
	// End of RLE font rendering
	return width * textsize;    // x +
}

void TFTLIB_8BIT::drawChar(int32_t x, int32_t y, uint16_t c, uint32_t color, uint32_t bg, uint8_t size) {
	if ((x >= _width)			|| // Clip right
	(y >= _height)				|| // Clip bottom
	((x + 6 * size - 1) < 0)	|| // Clip left
	((y + 8 * size - 1) < 0))   // Clip top
	return;

	if (c < 32) return;
	if ((c >= read_word(&gfxFont->first)) && (c <= read_word(&gfxFont->last ))) {
		c -= read_word(&gfxFont->first);
		GFXglyph *glyph  = &(((GFXglyph *)read_dword(&gfxFont->glyph))[c]);
		uint8_t  *bitmap = (uint8_t *)read_dword(&gfxFont->bitmap);

		uint32_t bo = read_word(&glyph->bitmapOffset);
		uint8_t  w  = read_byte(&glyph->width),
		h  = read_byte(&glyph->height);
		//xa = read_byte(&glyph->xAdvance);
		int8_t   xo = read_byte(&glyph->xOffset),
		yo = read_byte(&glyph->yOffset);
		uint8_t  xx, yy, bits=0, bit=0;
		int16_t  xo16 = 0, yo16 = 0;

		if(size > 1) {
			xo16 = xo;
			yo16 = yo;
		}

		// GFXFF rendering speed up
		uint16_t hpc = 0; // Horizontal foreground pixel count
		for(yy=0; yy<h; yy++) {
			for(xx=0; xx<w; xx++) {
				if(bit == 0) {
					bits = read_byte(&bitmap[bo++]);
					bit  = 0x80;
				}
				if(bits & bit) hpc++;
				else {
					if (hpc) {
						if(size == 1) drawFastHLine(x+xo+xx-hpc, y+yo+yy, hpc, color);
						else fillRect(x+(xo16+xx-hpc)*size, y+(yo16+yy)*size, size*hpc, size, color);
						hpc=0;
					}
				}
				bit >>= 1;
			}
			// Draw pixels for this line as we are about to increment yy
			if (hpc) {
				if(size == 1) drawFastHLine(x+xo+xx-hpc, y+yo+yy, hpc, color);
				else fillRect(x+(xo16+xx-hpc)*size, y+(yo16+yy)*size, size*hpc, size, color);
				hpc=0;
			}
		}
	}
}


/***************************************************************************************
** Function name:           drawString (with or without user defined font)
** Description :            draw string with padding if it is defined
***************************************************************************************/
// With font number. Note: font number is over-ridden if a smooth font is loaded
int16_t TFTLIB_8BIT::drawString(const char *string, int32_t poX, int32_t poY) {
	int16_t sumX = 0;
	uint8_t padding = 1, baseline = 0;
	uint16_t cwidth = textWidth(string, textfont); // Find the pixel width of the string in the font
	uint16_t cheight = 8 * textsize;

	bool freeFont = gfxFont;

	if (freeFont) {
		cheight = glyph_ab * textsize;
		poY += cheight; // Adjust for baseline datum of free fonts
		baseline = cheight;
		padding =101; // Different padding method used for Free Fonts

		// We need to make an adjustment for the bottom of the string (eg 'y' character)
		if ((textdatum == BL_DATUM) || (textdatum == BC_DATUM) || (textdatum == BR_DATUM)) {
			cheight += glyph_bb * textsize;
		}
	}

	if (textdatum || padX) {
		switch(textdatum) {
			case TC_DATUM:
				poX -= cwidth/2;
				padding += 1;
			break;

			case TR_DATUM:
				poX -= cwidth;
				padding += 2;
			break;

			case ML_DATUM:
				poY -= cheight/2;
			break;

			case MC_DATUM:
				poX -= cwidth/2;
				poY -= cheight/2;
				padding += 1;
			break;

			case MR_DATUM:
				poX -= cwidth;
				poY -= cheight/2;
				padding += 2;
			break;

			case BL_DATUM:
				poY -= cheight;
			break;

			case BC_DATUM:
				poX -= cwidth/2;
				poY -= cheight;
				padding += 1;
			break;

			case BR_DATUM:
				poX -= cwidth;
				poY -= cheight;
				padding += 2;
			break;

			case L_BASELINE:
				poY -= baseline;
			break;

			case C_BASELINE:
				poX -= cwidth/2;
				poY -= baseline;
				padding += 1;
			break;

			case R_BASELINE:
				poX -= cwidth;
				poY -= baseline;
				padding += 2;
			break;
		}
		// Check coordinates are OK, adjust if not
		if (poX < 0) poX = 0;
		if (poX+cwidth > width())   poX = width() - cwidth;
		if (poY < 0) poY = 0;
		if (poY+cheight-baseline> height()) poY = height() - cheight;
	}


	int8_t xo = 0;
	if (freeFont && (textcolor!=textbgcolor)) {
		cheight = (glyph_ab + glyph_bb) * textsize;
		// Get the offset for the first character only to allow for negative offsets
		uint16_t c2 = 0;
		uint16_t len = strlen(string);
		uint16_t n = 0;

		while (n < len && c2 == 0) c2 = decodeUTF8((uint8_t*)string, &n, len - n);

		if((c2 >= read_word(&gfxFont->first)) && (c2 <= read_word(&gfxFont->last) )) {
			c2 -= read_word(&gfxFont->first);
			GFXglyph *glyph = &(((GFXglyph *)read_dword(&gfxFont->glyph))[c2]);
			xo = read_byte(&glyph->xOffset) * textsize;
			// Adjust for negative xOffset
			if (xo > 0) xo = 0;
			else cwidth -= xo;
			// Add 1 pixel of padding all round
			//cheight +=2;
			//fillRect(poX+xo-1, poY - 1 - glyph_ab * textsize, cwidth+2, cheight, textbgcolor);
			fillRect(poX+xo, poY - glyph_ab * textsize, cwidth, cheight, textbgcolor);
		}
		padding -=100;
	}

	uint16_t len = strlen(string);
	uint16_t n = 0;

	while (n < len) {
		uint16_t uniCode = decodeUTF8((uint8_t*)string, &n, len - n);
		sumX += drawChar(uniCode, poX+sumX, poY);
	}

	if((padX>cwidth) && (textcolor!=textbgcolor)) {
		int16_t padXc = poX+cwidth+xo;
		if (freeFont) {
			poX +=xo; // Adjust for negative offset start character
			poY -= glyph_ab * textsize;
			sumX += poX;
		}
		switch(padding) {
			case 1:
				fillRect(padXc,poY,padX-cwidth,cheight, textbgcolor);
			break;

			case 2:
				fillRect(padXc,poY,(padX-cwidth)>>1,cheight, textbgcolor);
				padXc = (padX-cwidth)>>1;
				if (padXc>poX) padXc = poX;
				fillRect(poX - padXc,poY,(padX-cwidth)>>1,cheight, textbgcolor);
			break;

			case 3:
				if (padXc>padX) padXc = padX;
				fillRect(poX + cwidth - padXc,poY,padXc-cwidth,cheight, textbgcolor);
			break;
		}
	}
	return sumX;
}


/***************************************************************************************
** Function name:           drawCentreString (deprecated, use setTextDatum())
** Descriptions:            draw string centred on dX
***************************************************************************************/
int16_t TFTLIB_8BIT::drawCentreString(const char *string, int32_t dX, int32_t poY) {
	uint8_t tempdatum = textdatum;
	int32_t sumX = 0;
	textdatum = MC_DATUM;
	sumX = drawString(string, dX, poY);
	textdatum = tempdatum;
	return sumX;
}


/***************************************************************************************
** Function name:           drawRightString (deprecated, use setTextDatum())
** Descriptions:            draw string right justified to dX
***************************************************************************************/

int16_t TFTLIB_8BIT::drawRightString(const char *string, int32_t dX, int32_t poY) {
	uint8_t tempdatum = textdatum;
	int16_t sumX = 0;
	textdatum = TR_DATUM;
	sumX = drawString(string, dX, poY);
	textdatum = tempdatum;
	return sumX;
}


/***************************************************************************************
** Function name:           drawNumber
** Description:             draw a long integer
***************************************************************************************/
int16_t TFTLIB_8BIT::drawNumber(long long_num, int32_t poX, int32_t poY) {
	isDigits = true; // Eliminate jiggle in monospaced fonts
	char str[12];
	itoa(long_num, str, 10);
	return drawString(str, poX, poY);
}

/***************************************************************************************
** Function name:           drawFloat
** Descriptions:            drawFloat, prints 7 non zero digits maximum
***************************************************************************************/
// Assemble and print a string, this permits alignment relative to a datum
// looks complicated but much more compact and actually faster than using print class
int16_t TFTLIB_8BIT::drawFloat(float floatNumber, uint8_t dp, int32_t poX, int32_t poY) {
	isDigits = true;
	char str[14];               // Array to contain decimal string
	uint8_t ptr = 0;            // Initialise pointer for array
	int8_t  digits = 1;         // Count the digits to avoid array overflow
	float rounding = 0.5;       // Round up down delta

	if (dp > 7) dp = 7; // Limit the size of decimal portion

	// Adjust the rounding value
	for (uint8_t i = 0; i < dp; ++i) rounding /= 10.0;

	if (floatNumber < -rounding) {   // add sign, avoid adding - sign to 0.0!
		str[ptr++] = '-'; // Negative number
		str[ptr] = 0; // Put a null in the array as a precaution
		digits = 0;   // Set digits to 0 to compensate so pointer value can be used later
		floatNumber = -floatNumber; // Make positive
	}

	floatNumber += rounding; // Round up or down

	// For error put ... in string and return (all TFT_eSPI library fonts contain . character)
	if (floatNumber >= 2147483647) {
		strcpy(str, "...");
		return drawString(str, poX, poY);
	}
	// No chance of overflow from here on

	// Get integer part
	uint32_t temp = (uint32_t)floatNumber;

	// Put integer part into array
	itoa(temp, str + ptr, 10);

	// Find out where the null is to get the digit count loaded
	while ((uint8_t)str[ptr] != 0) ptr++; // Move the pointer along
	digits += ptr;                  // Count the digits

	str[ptr++] = '.'; // Add decimal point
	str[ptr] = '0';   // Add a dummy zero
	str[ptr + 1] = 0; // Add a null but don't increment pointer so it can be overwritten

	// Get the decimal portion
	floatNumber = floatNumber - temp;

	// Get decimal digits one by one and put in array
	// Limit digit count so we don't get a false sense of resolution
	uint8_t i = 0;
	while ((i < dp) && (digits < 9)) { // while (i < dp) for no limit but array size must be increased
		i++;
		floatNumber *= 10;       // for the next decimal
		temp = floatNumber;      // get the decimal
		itoa(temp, str + ptr, 10);
		ptr++; digits++;         // Increment pointer and digits count
		floatNumber -= temp;     // Remove that digit
	}

	// Finally we can plot the string and return pixel length
	return drawString(str, poX, poY);
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
	setFreeFont(&FreeSerif9pt7b);
	unsigned long start = HAL_GetTick();
	setTextColor(WHITE);
	setTextSize(1);
	setTextDatum(TL_DATUM);
	drawString("Hello World!", 0, 0);

	setTextColor(YELLOW);
	setTextSize(2);
	drawFloat(1234.56, 2, 0, 16);

	setCursor(0, 48);
	setTextColor(RED);
	setTextSize(3);
	println(0xDEADBEEF, HEX);

	setCursor(0, 96);
	setTextColor(GREEN);
	setTextSize(5);
	println("Groop");

	setCursor(0, 176);
	setTextSize(2);
	println("I implore thee,");

	setCursor(0, 192);
	setTextSize(1);
	println("my foonting turlingdromes.");
	println("And hooptiously drangle me");
	println("with crinkly bindlewurdles,");
	println("Or I will rend thee");
	println("in the gobberwarts");
	println("with my blurglecruncheon,");
	println("see if I don't!");
	return HAL_GetTick() - start;
}

uint32_t TFTLIB_8BIT::testLines(uint16_t color) {
	unsigned long	start, t;
	int				x1, y1, x2, y2,
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
	unsigned long	start;
	int				n, i, i2,
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
	unsigned long	start, t = 0;
	int				n, i, i2,
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
	unsigned long	start;
	int				x, y, w = _width, h = _height, r2 = radius * 2;

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
	unsigned long	start;
	int				x, y, r2 = radius * 2,
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
	unsigned long	start;
	int				n, i, cx = _width  / 2 - 1,
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
	unsigned long	start, t = 0;
	int				i, cx = _width  / 2 - 1,
					cy = _height / 2 - 1;

	fillScreen(BLACK);
	start = HAL_GetTick();
	for (i = min(cx, cy); i > 10; i -= 5) {
		start = HAL_GetTick();
		fillTriangle(cx, cy - i, cx - i, cy + i, cx + i, cy + i, color565(0, i, i));
		t += HAL_GetTick() - start;
		drawTriangle(cx, cy - i, cx - i, cy + i, cx + i, cy + i, color565(i, i, 0));
	}

	return t;
}

uint32_t TFTLIB_8BIT::testRoundRects() {
	unsigned long	start;
	int				w, i, i2,
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
	unsigned long	start;
	int				i, i2,
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

	setFreeFont(&FreeSerif9pt7b);
	setTextColor(RED);
	setTextDatum(TL_DATUM);

	sprintf(buffer, "Fillscreen:         %lums", t1);
	drawString(buffer, 0, 0);

	sprintf(buffer, "Text:               %lums", t2);
	drawString(buffer, 0, 16);

	sprintf(buffer, "Lines:              %lums", t3);
	drawString(buffer, 0, 32);

	sprintf(buffer, "Hor/Vert Lines:     %lums", t4);
	drawString(buffer, 0, 48);

	sprintf(buffer, "Rect(outline):      %lums", t5);
	drawString(buffer, 0, 64);

	sprintf(buffer, "Rect(filled):       %lums", t6);
	drawString(buffer, 0, 80);

	sprintf(buffer, "Circ(outline):      %lums", t7);
	drawString(buffer, 0, 96);

	sprintf(buffer, "Circ(filled):       %lums", t8);
	drawString(buffer, 0, 112);

	sprintf(buffer, "Tri(outline):       %lums", t9);
	drawString(buffer, 0, 128);

	sprintf(buffer, "Tri(filled):        %lums", t10);
	drawString(buffer, 0, 144);

	sprintf(buffer, "Round Rects:        %lums", t11);
	drawString(buffer, 0, 160);

	sprintf(buffer, "Filled Round Rects: %lums", t12);
	drawString(buffer, 0, 176);

	sprintf(buffer, "Time total:         %lums", total);
	drawString(buffer, 0, 192);

	HAL_Delay(4000);
}
