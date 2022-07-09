# TFTLIB_8BIT
LCD 8-bit Parallel Interface Library for STM32

At first create CubeMX project. Configure 4 pins (RD, WR, RS(DC), CS) and 1 GPIO port (i.e. PB0-PB7) for display.

Then make project and copy library project folder. Add library path to "Include paths" in GCC and G++ compiler options. Convert project to C++ and rename main.c to main.cpp

Simply create TFTLIB_SPI object by calling:

TFTLIB_8BIT tft(LCD_PORT, TFT_DRIVER::ILI9327_PARALLEL, RD_GPIO_Port, RD_Pin, WR_GPIO_Port, WR_Pin, DC_GPIO_Port, DC_Pin, GPIO_Port, CS_Pin);

List of all usable functions:
    
    void init(void);
    
    uint16_t readID(void);
    void setRotation(uint8_t m);
		uint16_t color565(uint8_t r, uint8_t g, uint8_t b);
		uint16_t color16to8(uint16_t c);
		uint16_t color8to16(uint8_t color);
		uint16_t alphaBlend(uint8_t alpha, uint16_t fgc, uint16_t bgc);

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
