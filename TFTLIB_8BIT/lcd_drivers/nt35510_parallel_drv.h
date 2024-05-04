/*
 * ili9341_drv.h
 *
 *  Created on: Feb 17, 2022
 *  Updated on: Feb 20, 2022
 *      Author: Arkadiusz Szlanta
 */

{
	HAL_Delay(100);
	writeCommand16(0x0100); //SWRESET
	HAL_Delay(120);

	/* DISPOFF: Display Off */
	writeCommand16(0x2800);
	writeCommand16(0x1000);

	writeCommand16(0xF000); writeSmallData16(0x55);
	writeCommand16(0xF001); writeSmallData16(0xAA);
	writeCommand16(0xF002); writeSmallData16(0x52);
	writeCommand16(0xF003); writeSmallData16(0x08);
	writeCommand16(0xF004); writeSmallData16(0x01);

	/* BT1CTR: BT1 Power Control for AVDD */
	//# AVDD: manual);
	writeCommand16(0xB600); writeSmallData16(0x34);
	writeCommand16(0xB601); writeSmallData16(0x34);
	writeCommand16(0xB602); writeSmallData16(0x34);

	/* SETAVDD: Setting AVDD Voltage */
	writeCommand16(0xB000); writeSmallData16(0x0D);//09
	writeCommand16(0xB001); writeSmallData16(0x0D);
	writeCommand16(0xB002); writeSmallData16(0x0D);

	/* BT2CTR: BT2 Power Control for AVEE */
	//# AVEE: manual); -6V
	writeCommand16(0xB700); writeSmallData16(0x24);
	writeCommand16(0xB701); writeSmallData16(0x24);
	writeCommand16(0xB702); writeSmallData16(0x24);

	/* SETAVEE: Setting AVEE Voltage */
	writeCommand16(0xB100); writeSmallData16(0x0D);
	writeCommand16(0xB101); writeSmallData16(0x0D);
	writeCommand16(0xB102); writeSmallData16(0x0D);

	/* BT3CTR: BT3 Power Control for VCL */
	writeCommand16(0xB800); writeSmallData16(0x24);
	writeCommand16(0xB801); writeSmallData16(0x24);
	writeCommand16(0xB802); writeSmallData16(0x24);

	/* SETVCL: Setting VCL Voltage */
	writeCommand16(0xB200); writeSmallData16(0x00);

	/* BT4CTR: BT4 Power Control for VGH */
	//# VGH: Clamp Enable);
	writeCommand16(0xB900); writeSmallData16(0x24);
	writeCommand16(0xB901); writeSmallData16(0x24);
	writeCommand16(0xB902); writeSmallData16(0x24);

	/* SETVGH: Setting VGH Voltage */
	writeCommand16(0xB300); writeSmallData16(0x05);
	writeCommand16(0xB301); writeSmallData16(0x05);
	writeCommand16(0xB302); writeSmallData16(0x05);

	/* VGHCTR: VGH Output Voltage - commented out */
	///writeCommand16(0xBF00); writeSmallData16(0x01);

	/* BT5CTR: BT5 Power Control for VGLX */
	//# VGL(LVGL):
	writeCommand16(0xBA00); writeSmallData16(0x34);
	writeCommand16(0xBA01); writeSmallData16(0x34);
	writeCommand16(0xBA02); writeSmallData16(0x34);

	/* SETVGL_REG: Setting VGL_REG Voltage */
	//# VGL_REG(VGLO)
	writeCommand16(0xB500); writeSmallData16(0x0B);
	writeCommand16(0xB501); writeSmallData16(0x0B);
	writeCommand16(0xB502); writeSmallData16(0x0B);

	/* SETVGP: Setting VGMP and VGSP Voltage */
	//# VGMP/VGSP:
	writeCommand16(0xBC00); writeSmallData16(0X00);
	writeCommand16(0xBC01); writeSmallData16(0xA3);
	writeCommand16(0xBC02); writeSmallData16(0X00);

	/* SETVGN: Setting VGMN and VGSN Voltage */
	//# VGMN/VGSN
	writeCommand16(0xBD00); writeSmallData16(0x00);
	writeCommand16(0xBD01); writeSmallData16(0xA3);
	writeCommand16(0xBD02); writeSmallData16(0x00);

	/* SETVCMOFF: Setting VCOM Offset Voltage */
	//# VCOM=-0.1
	writeCommand16(0xBE00); writeSmallData16(0x00);
	writeCommand16(0xBE01); writeSmallData16(0x63);//4f
	//  VCOMH+0x01;

  	/* GMRCTR1: Setting Gamma 2.2 Correction for Red (Positive) */
	//#R+
	writeCommand16(0xD100); writeSmallData16(0x00);
	writeCommand16(0xD101); writeSmallData16(0x37);
	writeCommand16(0xD102); writeSmallData16(0x00);
	writeCommand16(0xD103); writeSmallData16(0x52);
	writeCommand16(0xD104); writeSmallData16(0x00);
	writeCommand16(0xD105); writeSmallData16(0x7B);
	writeCommand16(0xD106); writeSmallData16(0x00);
	writeCommand16(0xD107); writeSmallData16(0x99);
	writeCommand16(0xD108); writeSmallData16(0x00);
	writeCommand16(0xD109); writeSmallData16(0xB1);
	writeCommand16(0xD10A); writeSmallData16(0x00);
	writeCommand16(0xD10B); writeSmallData16(0xD2);
	writeCommand16(0xD10C); writeSmallData16(0x00);
	writeCommand16(0xD10D); writeSmallData16(0xF6);
	writeCommand16(0xD10E); writeSmallData16(0x01);
	writeCommand16(0xD10F); writeSmallData16(0x27);
	writeCommand16(0xD110); writeSmallData16(0x01);
	writeCommand16(0xD111); writeSmallData16(0x4E);
	writeCommand16(0xD112); writeSmallData16(0x01);
	writeCommand16(0xD113); writeSmallData16(0x8C);
	writeCommand16(0xD114); writeSmallData16(0x01);
	writeCommand16(0xD115); writeSmallData16(0xBE);
	writeCommand16(0xD116); writeSmallData16(0x02);
	writeCommand16(0xD117); writeSmallData16(0x0B);
	writeCommand16(0xD118); writeSmallData16(0x02);
	writeCommand16(0xD119); writeSmallData16(0x48);
	writeCommand16(0xD11A); writeSmallData16(0x02);
	writeCommand16(0xD11B); writeSmallData16(0x4A);
	writeCommand16(0xD11C); writeSmallData16(0x02);
	writeCommand16(0xD11D); writeSmallData16(0x7E);
	writeCommand16(0xD11E); writeSmallData16(0x02);
	writeCommand16(0xD11F); writeSmallData16(0xBC);
	writeCommand16(0xD120); writeSmallData16(0x02);
	writeCommand16(0xD121); writeSmallData16(0xE1);
	writeCommand16(0xD122); writeSmallData16(0x03);
	writeCommand16(0xD123); writeSmallData16(0x10);
	writeCommand16(0xD124); writeSmallData16(0x03);
	writeCommand16(0xD125); writeSmallData16(0x31);
	writeCommand16(0xD126); writeSmallData16(0x03);
	writeCommand16(0xD127); writeSmallData16(0x5A);
	writeCommand16(0xD128); writeSmallData16(0x03);
	writeCommand16(0xD129); writeSmallData16(0x73);
	writeCommand16(0xD12A); writeSmallData16(0x03);
	writeCommand16(0xD12B); writeSmallData16(0x94);
	writeCommand16(0xD12C); writeSmallData16(0x03);
	writeCommand16(0xD12D); writeSmallData16(0x9F);
	writeCommand16(0xD12E); writeSmallData16(0x03);
	writeCommand16(0xD12F); writeSmallData16(0xB3);
	writeCommand16(0xD130); writeSmallData16(0x03);
	writeCommand16(0xD131); writeSmallData16(0xB9);
	writeCommand16(0xD132); writeSmallData16(0x03);
	writeCommand16(0xD133); writeSmallData16(0xC1);

	/* GMGCTR1: Setting Gamma 2.2 Correction for Green (Positive) */
	//#G+
	writeCommand16(0xD200); writeSmallData16(0x00);
	writeCommand16(0xD201); writeSmallData16(0x37);
	writeCommand16(0xD202); writeSmallData16(0x00);
	writeCommand16(0xD203); writeSmallData16(0x52);
	writeCommand16(0xD204); writeSmallData16(0x00);
	writeCommand16(0xD205); writeSmallData16(0x7B);
	writeCommand16(0xD206); writeSmallData16(0x00);
	writeCommand16(0xD207); writeSmallData16(0x99);
	writeCommand16(0xD208); writeSmallData16(0x00);
	writeCommand16(0xD209); writeSmallData16(0xB1);
	writeCommand16(0xD20A); writeSmallData16(0x00);
	writeCommand16(0xD20B); writeSmallData16(0xD2);
	writeCommand16(0xD20C); writeSmallData16(0x00);
	writeCommand16(0xD20D); writeSmallData16(0xF6);
	writeCommand16(0xD20E); writeSmallData16(0x01);
	writeCommand16(0xD20F); writeSmallData16(0x27);
	writeCommand16(0xD210); writeSmallData16(0x01);
	writeCommand16(0xD211); writeSmallData16(0x4E);
	writeCommand16(0xD212); writeSmallData16(0x01);
	writeCommand16(0xD213); writeSmallData16(0x8C);
	writeCommand16(0xD214); writeSmallData16(0x01);
	writeCommand16(0xD215); writeSmallData16(0xBE);
	writeCommand16(0xD216); writeSmallData16(0x02);
	writeCommand16(0xD217); writeSmallData16(0x0B);
	writeCommand16(0xD218); writeSmallData16(0x02);
	writeCommand16(0xD219); writeSmallData16(0x48);
	writeCommand16(0xD21A); writeSmallData16(0x02);
	writeCommand16(0xD21B); writeSmallData16(0x4A);
	writeCommand16(0xD21C); writeSmallData16(0x02);
	writeCommand16(0xD21D); writeSmallData16(0x7E);
	writeCommand16(0xD21E); writeSmallData16(0x02);
	writeCommand16(0xD21F); writeSmallData16(0xBC);
	writeCommand16(0xD220); writeSmallData16(0x02);
	writeCommand16(0xD221); writeSmallData16(0xE1);
	writeCommand16(0xD222); writeSmallData16(0x03);
	writeCommand16(0xD223); writeSmallData16(0x10);
	writeCommand16(0xD224); writeSmallData16(0x03);
	writeCommand16(0xD225); writeSmallData16(0x31);
	writeCommand16(0xD226); writeSmallData16(0x03);
	writeCommand16(0xD227); writeSmallData16(0x5A);
	writeCommand16(0xD228); writeSmallData16(0x03);
	writeCommand16(0xD229); writeSmallData16(0x73);
	writeCommand16(0xD22A); writeSmallData16(0x03);
	writeCommand16(0xD22B); writeSmallData16(0x94);
	writeCommand16(0xD22C); writeSmallData16(0x03);
	writeCommand16(0xD22D); writeSmallData16(0x9F);
	writeCommand16(0xD22E); writeSmallData16(0x03);
	writeCommand16(0xD22F); writeSmallData16(0xB3);
	writeCommand16(0xD230); writeSmallData16(0x03);
	writeCommand16(0xD231); writeSmallData16(0xB9);
	writeCommand16(0xD232); writeSmallData16(0x03);
	writeCommand16(0xD233); writeSmallData16(0xC1);

	/* GMBCTR1: Setting Gamma 2.2 Correction for Blue (Positive) */
	//#B+
	writeCommand16(0xD300); writeSmallData16(0x00);
	writeCommand16(0xD301); writeSmallData16(0x37);
	writeCommand16(0xD302); writeSmallData16(0x00);
	writeCommand16(0xD303); writeSmallData16(0x52);
	writeCommand16(0xD304); writeSmallData16(0x00);
	writeCommand16(0xD305); writeSmallData16(0x7B);
	writeCommand16(0xD306); writeSmallData16(0x00);
	writeCommand16(0xD307); writeSmallData16(0x99);
	writeCommand16(0xD308); writeSmallData16(0x00);
	writeCommand16(0xD309); writeSmallData16(0xB1);
	writeCommand16(0xD30A); writeSmallData16(0x00);
	writeCommand16(0xD30B); writeSmallData16(0xD2);
	writeCommand16(0xD30C); writeSmallData16(0x00);
	writeCommand16(0xD30D); writeSmallData16(0xF6);
	writeCommand16(0xD30E); writeSmallData16(0x01);
	writeCommand16(0xD30F); writeSmallData16(0x27);
	writeCommand16(0xD310); writeSmallData16(0x01);
	writeCommand16(0xD311); writeSmallData16(0x4E);
	writeCommand16(0xD312); writeSmallData16(0x01);
	writeCommand16(0xD313); writeSmallData16(0x8C);
	writeCommand16(0xD314); writeSmallData16(0x01);
	writeCommand16(0xD315); writeSmallData16(0xBE);
	writeCommand16(0xD316); writeSmallData16(0x02);
	writeCommand16(0xD317); writeSmallData16(0x0B);
	writeCommand16(0xD318); writeSmallData16(0x02);
	writeCommand16(0xD319); writeSmallData16(0x48);
	writeCommand16(0xD31A); writeSmallData16(0x02);
	writeCommand16(0xD31B); writeSmallData16(0x4A);
	writeCommand16(0xD31C); writeSmallData16(0x02);
	writeCommand16(0xD31D); writeSmallData16(0x7E);
	writeCommand16(0xD31E); writeSmallData16(0x02);
	writeCommand16(0xD31F); writeSmallData16(0xBC);
	writeCommand16(0xD320); writeSmallData16(0x02);
	writeCommand16(0xD321); writeSmallData16(0xE1);
	writeCommand16(0xD322); writeSmallData16(0x03);
	writeCommand16(0xD323); writeSmallData16(0x10);
	writeCommand16(0xD324); writeSmallData16(0x03);
	writeCommand16(0xD325); writeSmallData16(0x31);
	writeCommand16(0xD326); writeSmallData16(0x03);
	writeCommand16(0xD327); writeSmallData16(0x5A);
	writeCommand16(0xD328); writeSmallData16(0x03);
	writeCommand16(0xD329); writeSmallData16(0x73);
	writeCommand16(0xD32A); writeSmallData16(0x03);
	writeCommand16(0xD32B); writeSmallData16(0x94);
	writeCommand16(0xD32C); writeSmallData16(0x03);
	writeCommand16(0xD32D); writeSmallData16(0x9F);
	writeCommand16(0xD32E); writeSmallData16(0x03);
	writeCommand16(0xD32F); writeSmallData16(0xB3);
	writeCommand16(0xD330); writeSmallData16(0x03);
	writeCommand16(0xD331); writeSmallData16(0xB9);
	writeCommand16(0xD332); writeSmallData16(0x03);
	writeCommand16(0xD333); writeSmallData16(0xC1);

	/* GMRCTR2: Setting Gamma 2.2 Correction for Red (Negative) */
	//#R-///////////////////////////////////////////
	writeCommand16(0xD400); writeSmallData16(0x00);
	writeCommand16(0xD401); writeSmallData16(0x37);
	writeCommand16(0xD402); writeSmallData16(0x00);
	writeCommand16(0xD403); writeSmallData16(0x52);
	writeCommand16(0xD404); writeSmallData16(0x00);
	writeCommand16(0xD405); writeSmallData16(0x7B);
	writeCommand16(0xD406); writeSmallData16(0x00);
	writeCommand16(0xD407); writeSmallData16(0x99);
	writeCommand16(0xD408); writeSmallData16(0x00);
	writeCommand16(0xD409); writeSmallData16(0xB1);
	writeCommand16(0xD40A); writeSmallData16(0x00);
	writeCommand16(0xD40B); writeSmallData16(0xD2);
	writeCommand16(0xD40C); writeSmallData16(0x00);
	writeCommand16(0xD40D); writeSmallData16(0xF6);
	writeCommand16(0xD40E); writeSmallData16(0x01);
	writeCommand16(0xD40F); writeSmallData16(0x27);
	writeCommand16(0xD410); writeSmallData16(0x01);
	writeCommand16(0xD411); writeSmallData16(0x4E);
	writeCommand16(0xD412); writeSmallData16(0x01);
	writeCommand16(0xD413); writeSmallData16(0x8C);
	writeCommand16(0xD414); writeSmallData16(0x01);
	writeCommand16(0xD415); writeSmallData16(0xBE);
	writeCommand16(0xD416); writeSmallData16(0x02);
	writeCommand16(0xD417); writeSmallData16(0x0B);
	writeCommand16(0xD418); writeSmallData16(0x02);
	writeCommand16(0xD419); writeSmallData16(0x48);
	writeCommand16(0xD41A); writeSmallData16(0x02);
	writeCommand16(0xD41B); writeSmallData16(0x4A);
	writeCommand16(0xD41C); writeSmallData16(0x02);
	writeCommand16(0xD41D); writeSmallData16(0x7E);
	writeCommand16(0xD41E); writeSmallData16(0x02);
	writeCommand16(0xD41F); writeSmallData16(0xBC);
	writeCommand16(0xD420); writeSmallData16(0x02);
	writeCommand16(0xD421); writeSmallData16(0xE1);
	writeCommand16(0xD422); writeSmallData16(0x03);
	writeCommand16(0xD423); writeSmallData16(0x10);
	writeCommand16(0xD424); writeSmallData16(0x03);
	writeCommand16(0xD425); writeSmallData16(0x31);
	writeCommand16(0xD426); writeSmallData16(0x03);
	writeCommand16(0xD427); writeSmallData16(0x5A);
	writeCommand16(0xD428); writeSmallData16(0x03);
	writeCommand16(0xD429); writeSmallData16(0x73);
	writeCommand16(0xD42A); writeSmallData16(0x03);
	writeCommand16(0xD42B); writeSmallData16(0x94);
	writeCommand16(0xD42C); writeSmallData16(0x03);
	writeCommand16(0xD42D); writeSmallData16(0x9F);
	writeCommand16(0xD42E); writeSmallData16(0x03);
	writeCommand16(0xD42F); writeSmallData16(0xB3);
	writeCommand16(0xD430); writeSmallData16(0x03);
	writeCommand16(0xD431); writeSmallData16(0xB9);
	writeCommand16(0xD432); writeSmallData16(0x03);
	writeCommand16(0xD433); writeSmallData16(0xC1);

	/* GMGCTR2: Setting Gamma 2.2 Correction for Green (Negative) */
	//#G-//////////////////////////////////////////////
	writeCommand16(0xD500); writeSmallData16(0x00);
	writeCommand16(0xD501); writeSmallData16(0x37);
	writeCommand16(0xD502); writeSmallData16(0x00);
	writeCommand16(0xD503); writeSmallData16(0x52);
	writeCommand16(0xD504); writeSmallData16(0x00);
	writeCommand16(0xD505); writeSmallData16(0x7B);
	writeCommand16(0xD506); writeSmallData16(0x00);
	writeCommand16(0xD507); writeSmallData16(0x99);
	writeCommand16(0xD508); writeSmallData16(0x00);
	writeCommand16(0xD509); writeSmallData16(0xB1);
	writeCommand16(0xD50A); writeSmallData16(0x00);
	writeCommand16(0xD50B); writeSmallData16(0xD2);
	writeCommand16(0xD50C); writeSmallData16(0x00);
	writeCommand16(0xD50D); writeSmallData16(0xF6);
	writeCommand16(0xD50E); writeSmallData16(0x01);
	writeCommand16(0xD50F); writeSmallData16(0x27);
	writeCommand16(0xD510); writeSmallData16(0x01);
	writeCommand16(0xD511); writeSmallData16(0x4E);
	writeCommand16(0xD512); writeSmallData16(0x01);
	writeCommand16(0xD513); writeSmallData16(0x8C);
	writeCommand16(0xD514); writeSmallData16(0x01);
	writeCommand16(0xD515); writeSmallData16(0xBE);
	writeCommand16(0xD516); writeSmallData16(0x02);
	writeCommand16(0xD517); writeSmallData16(0x0B);
	writeCommand16(0xD518); writeSmallData16(0x02);
	writeCommand16(0xD519); writeSmallData16(0x48);
	writeCommand16(0xD51A); writeSmallData16(0x02);
	writeCommand16(0xD51B); writeSmallData16(0x4A);
	writeCommand16(0xD51C); writeSmallData16(0x02);
	writeCommand16(0xD51D); writeSmallData16(0x7E);
	writeCommand16(0xD51E); writeSmallData16(0x02);
	writeCommand16(0xD51F); writeSmallData16(0xBC);
	writeCommand16(0xD520); writeSmallData16(0x02);
	writeCommand16(0xD521); writeSmallData16(0xE1);
	writeCommand16(0xD522); writeSmallData16(0x03);
	writeCommand16(0xD523); writeSmallData16(0x10);
	writeCommand16(0xD524); writeSmallData16(0x03);
	writeCommand16(0xD525); writeSmallData16(0x31);
	writeCommand16(0xD526); writeSmallData16(0x03);
	writeCommand16(0xD527); writeSmallData16(0x5A);
	writeCommand16(0xD528); writeSmallData16(0x03);
	writeCommand16(0xD529); writeSmallData16(0x73);
	writeCommand16(0xD52A); writeSmallData16(0x03);
	writeCommand16(0xD52B); writeSmallData16(0x94);
	writeCommand16(0xD52C); writeSmallData16(0x03);
	writeCommand16(0xD52D); writeSmallData16(0x9F);
	writeCommand16(0xD52E); writeSmallData16(0x03);
	writeCommand16(0xD52F); writeSmallData16(0xB3);
	writeCommand16(0xD530); writeSmallData16(0x03);
	writeCommand16(0xD531); writeSmallData16(0xB9);
	writeCommand16(0xD532); writeSmallData16(0x03);
	writeCommand16(0xD533); writeSmallData16(0xC1);

	/* GMBCTR2: Setting Gamma 2.2 Correction for Blue (Negative) */
	//#B-///////////////////////////////
	writeCommand16(0xD600); writeSmallData16(0x00);
	writeCommand16(0xD601); writeSmallData16(0x37);
	writeCommand16(0xD602); writeSmallData16(0x00);
	writeCommand16(0xD603); writeSmallData16(0x52);
	writeCommand16(0xD604); writeSmallData16(0x00);
	writeCommand16(0xD605); writeSmallData16(0x7B);
	writeCommand16(0xD606); writeSmallData16(0x00);
	writeCommand16(0xD607); writeSmallData16(0x99);
	writeCommand16(0xD608); writeSmallData16(0x00);
	writeCommand16(0xD609); writeSmallData16(0xB1);
	writeCommand16(0xD60A); writeSmallData16(0x00);
	writeCommand16(0xD60B); writeSmallData16(0xD2);
	writeCommand16(0xD60C); writeSmallData16(0x00);
	writeCommand16(0xD60D); writeSmallData16(0xF6);
	writeCommand16(0xD60E); writeSmallData16(0x01);
	writeCommand16(0xD60F); writeSmallData16(0x27);
	writeCommand16(0xD610); writeSmallData16(0x01);
	writeCommand16(0xD611); writeSmallData16(0x4E);
	writeCommand16(0xD612); writeSmallData16(0x01);
	writeCommand16(0xD613); writeSmallData16(0x8C);
	writeCommand16(0xD614); writeSmallData16(0x01);
	writeCommand16(0xD615); writeSmallData16(0xBE);
	writeCommand16(0xD616); writeSmallData16(0x02);
	writeCommand16(0xD617); writeSmallData16(0x0B);
	writeCommand16(0xD618); writeSmallData16(0x02);
	writeCommand16(0xD619); writeSmallData16(0x48);
	writeCommand16(0xD61A); writeSmallData16(0x02);
	writeCommand16(0xD61B); writeSmallData16(0x4A);
	writeCommand16(0xD61C); writeSmallData16(0x02);
	writeCommand16(0xD61D); writeSmallData16(0x7E);
	writeCommand16(0xD61E); writeSmallData16(0x02);
	writeCommand16(0xD61F); writeSmallData16(0xBC);
	writeCommand16(0xD620); writeSmallData16(0x02);
	writeCommand16(0xD621); writeSmallData16(0xE1);
	writeCommand16(0xD622); writeSmallData16(0x03);
	writeCommand16(0xD623); writeSmallData16(0x10);
	writeCommand16(0xD624); writeSmallData16(0x03);
	writeCommand16(0xD625); writeSmallData16(0x31);
	writeCommand16(0xD626); writeSmallData16(0x03);
	writeCommand16(0xD627); writeSmallData16(0x5A);
	writeCommand16(0xD628); writeSmallData16(0x03);
	writeCommand16(0xD629); writeSmallData16(0x73);
	writeCommand16(0xD62A); writeSmallData16(0x03);
	writeCommand16(0xD62B); writeSmallData16(0x94);
	writeCommand16(0xD62C); writeSmallData16(0x03);
	writeCommand16(0xD62D); writeSmallData16(0x9F);
	writeCommand16(0xD62E); writeSmallData16(0x03);
	writeCommand16(0xD62F); writeSmallData16(0xB3);
	writeCommand16(0xD630); writeSmallData16(0x03);
	writeCommand16(0xD631); writeSmallData16(0xB9);
	writeCommand16(0xD632); writeSmallData16(0x03);
	writeCommand16(0xD633); writeSmallData16(0xC1);

	/* MAUCCTR: Manufacture Command Set enable */
	//#Enable Page0
	writeCommand16(0xF000); writeSmallData16(0x55);
	writeCommand16(0xF001); writeSmallData16(0xAA);
	writeCommand16(0xF002); writeSmallData16(0x52);
	writeCommand16(0xF003); writeSmallData16(0x08);
	writeCommand16(0xF004); writeSmallData16(0x00);

	/* RGBCTR: RGB Interface Signals Control */
	//# RGB I/F Setting
	writeCommand16(0xB000); writeSmallData16(0x08);
	writeCommand16(0xB001); writeSmallData16(0x05);
	writeCommand16(0xB002); writeSmallData16(0x02);
	writeCommand16(0xB003); writeSmallData16(0x05);
	writeCommand16(0xB004); writeSmallData16(0x02);

	/* SDHDTCTR: Source Output Data Hold Time Control */
	//## SDT:
	writeCommand16(0xB600); writeSmallData16(0x08);

	/* DPRSLCTR: Display Resolution Control */
	writeCommand16(0xB500); writeSmallData16(0x50);//0x6b ???? 480x854       0x50 ???? 480x800

	/* GSEQCTR: EQ Control Function for Gate Signals */
	//## Gate EQ:
	writeCommand16(0xB700); writeSmallData16(0x00);
	writeCommand16(0xB701); writeSmallData16(0x00);

	/* SDEQCTR: EQ Control Function for Source Driver */
	//## Source EQ:
	writeCommand16(0xB800); writeSmallData16(0x01);
	writeCommand16(0xB801); writeSmallData16(0x05);
	writeCommand16(0xB802); writeSmallData16(0x05);
	writeCommand16(0xB803); writeSmallData16(0x05);

	/* INVCTR: Inversion Driving Control */
	//# Inversion: Column inversion (NVT)
	writeCommand16(0xBC00); writeSmallData16(0x00);
	writeCommand16(0xBC01); writeSmallData16(0x00);
	writeCommand16(0xBC02); writeSmallData16(0x00);

	/* DPTMCTR12: Display Timing Control 12 */
	//# BOE's Setting(default)
	writeCommand16(0xCC00); writeSmallData16(0x03);
	writeCommand16(0xCC01); writeSmallData16(0x00);
	writeCommand16(0xCC02); writeSmallData16(0x00);

	/* DPFRCTR1: Display Timing Control in Normal / Idle Off Mode */
	//# Display Timing:
	writeCommand16(0xBD00); writeSmallData16(0x01);
	writeCommand16(0xBD01); writeSmallData16(0x84);
	writeCommand16(0xBD02); writeSmallData16(0x07);
	writeCommand16(0xBD03); writeSmallData16(0x31);
	writeCommand16(0xBD04); writeSmallData16(0x00);

	/* SDVPCTR: Source Control in Vertical Porch Time */
	writeCommand16(0xBA00); writeSmallData16(0x01);

	/* MAUCCTR: Manufacture Command Set disable (?) */
	writeCommand16(0xFF00); writeSmallData16(0xAA);
	writeCommand16(0xFF01); writeSmallData16(0x55);
	writeCommand16(0xFF02); writeSmallData16(0x25);
	writeCommand16(0xFF03); writeSmallData16(0x01);

	/* TEON: Tearing Effect Line ON */
	writeCommand16(0x3500); writeSmallData16(0x00);

	/* MADCTL: Memory Data Access Control */
	writeCommand16(0x3600); writeSmallData16(0x00);

	/* COLMOD: Interface Pixel Format */
	/* 	“0101” = 16-bit/pixel
		“0110” = 18-bit/pixel
		“0111” = 24-bit/pixel */
	writeCommand16(0x3a00); writeSmallData16(0x55);  ////55=16?/////66=18?

	/* SLPOUT: Sleep Out */
	writeCommand16(0x1100);
	HAL_Delay(120);

	/* DISPON: Display On */
	writeCommand16(0x2900);
}
