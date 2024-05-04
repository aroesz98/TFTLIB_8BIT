/*
 * ili9327_drv.h
 *
 *  Created on: Mar 2, 2022
 *      Author: Arkadiusz Szlanta
 */

#ifndef INC_ILI9327_DRV_H_
#define INC_ILI9327_DRV_H_

{
	writeCommand8(SWRESET);
	HAL_Delay(50);

	writeCommand8(DISPOFF);

	writeCommand8(COLMOD);
	writeSmallData8(COLOR_MODE_16bit);

	writeCommand8(0xB0);
	writeSmallData8(0x00);

	writeCommand8(0xC0);
	writeSmallData8(0x00);
	writeSmallData8(0x35);
	writeSmallData8(0x00);
	writeSmallData8(0x00);
	writeSmallData8(0x01);
	writeSmallData8(0x02);

	writeCommand8(0xC1);
	writeSmallData8(0x10);
	writeSmallData8(0x10);
	writeSmallData8(0x02);
	writeSmallData8(0x02);

	writeCommand8(0xC5);
	writeSmallData8(0x00);

	writeCommand8(0xD2);
	writeSmallData8(0x01);
	writeSmallData8(0x04);

	writeCommand8(0xCA);
	writeSmallData8(0x00);

	writeCommand8(0xEA);
	writeSmallData8(0x80);

	writeCommand8(SLPOUT);
	HAL_Delay(150);

	writeCommand8(DISPON);
}

#endif /* INC_ILI9327_DRV_H_ */
