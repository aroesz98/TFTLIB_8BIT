#ifndef _GFXFONT_H_
#define _GFXFONT_H_

typedef struct { // Data stored PER GLYPH
	uint32_t bitmapOffset;     // Pointer into GFXfont->bitmap
	uint8_t  width, height;    // Bitmap dimensions in pixels
	uint8_t  xAdvance;         // Distance to advance cursor (x axis)
	int8_t   xOffset, yOffset; // Dist from cursor pos to UL corner
} GFXglyph;

typedef struct { // Data stored for FONT AS A WHOLE:
	uint8_t  *bitmap;      // Glyph bitmaps, concatenated
	GFXglyph *glyph;       // Glyph array
	uint16_t  first, last; // ASCII extents
	uint8_t   yAdvance;    // Newline distance (y axis)
} GFXfont;

// Original Adafruit_GFX "Free Fonts"
#include <GFXFF/Custom/TomThumb.h>  // TT1

#include <GFXFF/FreeMono9pt7b.h>  // FF1 or FM9
#include <GFXFF/FreeMono12pt7b.h> // FF2 or FM12
#include <GFXFF/FreeMono18pt7b.h> // FF3 or FM18
#include <GFXFF/FreeMono24pt7b.h> // FF4 or FM24

#include <GFXFF/FreeMonoOblique9pt7b.h>  // FF5 or FMO9
#include <GFXFF/FreeMonoOblique12pt7b.h> // FF6 or FMO12
#include <GFXFF/FreeMonoOblique18pt7b.h> // FF7 or FMO18
#include <GFXFF/FreeMonoOblique24pt7b.h> // FF8 or FMO24

#include <GFXFF/FreeMonoBold9pt7b.h>  // FF9  or FMB9
#include <GFXFF/FreeMonoBold12pt7b.h> // FF10 or FMB12
#include <GFXFF/FreeMonoBold18pt7b.h> // FF11 or FMB18
#include <GFXFF/FreeMonoBold24pt7b.h> // FF12 or FMB24

#include <GFXFF/FreeMonoBoldOblique9pt7b.h>  // FF13 or FMBO9
#include <GFXFF/FreeMonoBoldOblique12pt7b.h> // FF14 or FMBO12
#include <GFXFF/FreeMonoBoldOblique18pt7b.h> // FF15 or FMBO18
#include <GFXFF/FreeMonoBoldOblique24pt7b.h> // FF16 or FMBO24

// Sans serif fonts
#include <GFXFF/FreeSans9pt7b.h>  // FF17 or FSS9
#include <GFXFF/FreeSans12pt7b.h> // FF18 or FSS12
#include <GFXFF/FreeSans18pt7b.h> // FF19 or FSS18
#include <GFXFF/FreeSans24pt7b.h> // FF20 or FSS24

#include <GFXFF/FreeSansOblique9pt7b.h>  // FF21 or FSSO9
#include <GFXFF/FreeSansOblique12pt7b.h> // FF22 or FSSO12
#include <GFXFF/FreeSansOblique18pt7b.h> // FF23 or FSSO18
#include <GFXFF/FreeSansOblique24pt7b.h> // FF24 or FSSO24

#include <GFXFF/FreeSansBold9pt7b.h>  // FF25 or FSSB9
#include <GFXFF/FreeSansBold12pt7b.h> // FF26 or FSSB12
#include <GFXFF/FreeSansBold18pt7b.h> // FF27 or FSSB18
#include <GFXFF/FreeSansBold24pt7b.h> // FF28 or FSSB24

#include <GFXFF/FreeSansBoldOblique9pt7b.h>  // FF29 or FSSBO9
#include <GFXFF/FreeSansBoldOblique12pt7b.h> // FF30 or FSSBO12
#include <GFXFF/FreeSansBoldOblique18pt7b.h> // FF31 or FSSBO18
#include <GFXFF/FreeSansBoldOblique24pt7b.h> // FF32 or FSSBO24

// Serif fonts
#include <GFXFF/FreeSerif9pt7b.h>  // FF33 or FS9
#include <GFXFF/FreeSerif12pt7b.h> // FF34 or FS12
#include <GFXFF/FreeSerif18pt7b.h> // FF35 or FS18
#include <GFXFF/FreeSerif24pt7b.h> // FF36 or FS24

#include <GFXFF/FreeSerifItalic9pt7b.h>  // FF37 or FSI9
#include <GFXFF/FreeSerifItalic12pt7b.h> // FF38 or FSI12
#include <GFXFF/FreeSerifItalic18pt7b.h> // FF39 or FSI18
#include <GFXFF/FreeSerifItalic24pt7b.h> // FF40 or FSI24

#include <GFXFF/FreeSerifBold9pt7b.h>  // FF41 or FSB9
#include <GFXFF/FreeSerifBold12pt7b.h> // FF42 or FSB12
#include <GFXFF/FreeSerifBold18pt7b.h> // FF43 or FSB18
#include <GFXFF/FreeSerifBold24pt7b.h> // FF44 or FSB24

#include <GFXFF/FreeSerifBoldItalic9pt7b.h>  // FF45 or FSBI9
#include <GFXFF/FreeSerifBoldItalic12pt7b.h> // FF46 or FSBI12
#include <GFXFF/FreeSerifBoldItalic18pt7b.h> // FF47 or FSBI18
#include <GFXFF/FreeSerifBoldItalic24pt7b.h> // FF48 or FSBI24

//Custom fonts
#include <GFXFF/Custom/Orbitron_Light_24.h>
#include <GFXFF/Custom/Orbitron_Light_32.h>
#include <GFXFF/Custom/Roboto_Thin_24.h>
#include <GFXFF/Custom/Satisfy_24.h>
#include <GFXFF/Custom/Yellowtail_32.h>

#include <GFXFF/Custom/SegoeScript/SegoeScript8.h>
#include <GFXFF/Custom/SegoeScript/SegoeScript10.h>
#include <GFXFF/Custom/SegoeScript/SegoeScript12.h>
#include <GFXFF/Custom/SegoeScript/SegoeScript14.h>
#include <GFXFF/Custom/SegoeScript/SegoeScript16.h>
#include <GFXFF/Custom/SegoeScript/SegoeScript18.h>
#include <GFXFF/Custom/SegoeScript/SegoeScript20.h>
#include <GFXFF/Custom/SegoeScript/SegoeScript22.h>
#include <GFXFF/Custom/SegoeScript/SegoeScript24.h>
#include <GFXFF/Custom/SegoeScript/SegoeScript28.h>
#include <GFXFF/Custom/SegoeScript/SegoeScript32.h>
#include <GFXFF/Custom/SegoeScript/SegoeScript36.h>
#include <GFXFF/Custom/SegoeScript/SegoeScript40.h>
#include <GFXFF/Custom/SegoeScript/SegoeScript48.h>

#include <GFXFF/Custom/ArthurGothic/ArthurGothic8.h>
#include <GFXFF/Custom/ArthurGothic/ArthurGothic10.h>
#include <GFXFF/Custom/ArthurGothic/ArthurGothic12.h>
#include <GFXFF/Custom/ArthurGothic/ArthurGothic14.h>
#include <GFXFF/Custom/ArthurGothic/ArthurGothic16.h>
#include <GFXFF/Custom/ArthurGothic/ArthurGothic18.h>
#include <GFXFF/Custom/ArthurGothic/ArthurGothic20.h>
#include <GFXFF/Custom/ArthurGothic/ArthurGothic22.h>
#include <GFXFF/Custom/ArthurGothic/ArthurGothic24.h>
#include <GFXFF/Custom/ArthurGothic/ArthurGothic28.h>
#include <GFXFF/Custom/ArthurGothic/ArthurGothic32.h>
#include <GFXFF/Custom/ArthurGothic/ArthurGothic36.h>
#include <GFXFF/Custom/ArthurGothic/ArthurGothic40.h>
#include <GFXFF/Custom/ArthurGothic/ArthurGothic48.h>

#endif // _GFXFONT_H_
