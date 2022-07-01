/*
 * ili9341_drv.h
 *
 *  Created on: Feb 17, 2022
 *  Updated on: Feb 20, 2022
 *      Author: asz
 */

{
	CS_H();
	WR_H();
	HAL_Delay(5);

	writeCommand8(SWRESET);
	HAL_Delay(50);

	writeCommand8(DISPOFF);

	writeCommand8(PWCTR1);
	writeSmallData8(0x23);

	writeCommand8(PWCTR2);
	writeSmallData8(0x10);

	writeCommand8(VMCTR1);
	writeSmallData8(0x2B);
	writeSmallData8(0x2B);

	writeCommand8(VMCTR2);
	writeSmallData8(0xC0);

	setRotation(3);

	writeCommand8(COLMOD);
	writeSmallData8(COLOR_MODE_16bit);

	writeCommand8(FRMCTR1);
	writeSmallData8(0x00);
	writeSmallData8(0x16); //0x1B Default 70Hz

	writeCommand8(SLPOUT);
	HAL_Delay(5);

	writeCommand8(DISPON);
}
