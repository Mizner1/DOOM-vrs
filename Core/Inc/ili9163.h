/**
 * @file ili9163lcd.h
 * @brief ILI9163 128x128 LCD Driver (Header file)
 *
 * This code has been ported from the ili9163lcd library for avr made
 * by Simon Inns, to run on a msp430.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * @author Simon Inns <simon.inns@gmail.com>
 * @author Christopher Vagnetoft (NoccyLabs)
 * @copyright (C) 2012 Simon Inns
 * @copyright parts (C) 2012 NoccyLabs
 */

#ifndef ILI9163LCD_H_
#define ILI9163LCD_H_

#include <stdint.h>

// Definitions for data-bus (port D)
#define LCD_DB0	(1 << 0)
#define LCD_DB1	(1 << 1)
#define LCD_DB2	(1 << 2)
#define LCD_DB3	(1 << 3)
#define LCD_DB4	(1 << 4)
#define LCD_DB5	(1 << 5)
#define LCD_DB6	(1 << 6)
#define LCD_DB7	(1 << 7)

// Definitions for control lines (port C)
#define LCD_WR		(1 << 2)
#define LCD_RS		(1 << 4)
#define LCD_RD		(1 << 5)
#define LCD_CS		(1 << 6)
#define LCD_RESET	(1 << 7)

// Screen orientation defines:
// 0 = Ribbon at top
// 1 = Ribbon at left
// 2 = Ribbon at right
// 3 = Ribbon at bottom
#define LCD_ORIENTATION0	0	// na sirku opacne
#define LCD_ORIENTATION1	96	// na vysku opacne
#define LCD_ORIENTATION2	160 // na vysku
#define LCD_ORIENTATION3	192 // na sirku (preferred)

// ILI9163 LCD Controller Commands
#define NOP 					0x00
#define SOFT_RESET 				0x01
#define GET_RED_CHANNEL 		0x06
#define GET_GREEN_CHANNEL 		0x07
#define GET_BLUE_CHANNEL 		0x08
#define GET_PIXEL_FORMAT 		0x0C
#define GET_POWER_MODE 			0x0A
#define GET_ADDRESS_MODE 		0x0B
#define GET_DISPLAY_MODE 		0x0D
#define GET_SIGNAL_MODE 		0x0E
#define GET_DIAGNOSTIC_RESULT 	0x0F
#define ENTER_SLEEP_MODE 		0x10
#define EXIT_SLEEP_MODE 		0x11
#define ENTER_PARTIAL_MODE 		0x12
#define ENTER_NORMAL_MODE 		0x13
#define EXIT_INVERT_MODE 		0x20
#define ENTER_INVERT_MODE 		0x21
#define SET_GAMMA_CURVE 		0x26
#define SET_DISPLAY_OFF			0x28
#define SET_DISPLAY_ON 			0x29
#define SET_COLUMN_ADDRESS 		0x2A
#define SET_PAGE_ADDRESS 		0x2B
#define WRITE_MEMORY_START 		0x2C
#define WRITE_LUT 				0x2D
#define READ_MEMORY_START 		0x2E
#define SET_PARTIAL_AREA 		0x30
#define SET_SCROLL_AREA 		0x33
#define SET_TEAR_OFF 			0x34
#define SET_TEAR_ON 			0x35
#define SET_ADDRESS_MODE 		0x36
#define SET_SCROLL_START 		0X37
#define EXIT_IDLE_MODE 			0x38
#define ENTER_IDLE_MODE 		0x39
#define SET_PIXEL_FORMAT 		0x3A
#define WRITE_MEMORY_CONTINUE 	0x3C
#define READ_MEMORY_CONTINUE 	0x3E
#define SET_TEAR_SCANLINE 		0x44
#define GET_SCANLINE 			0x45
#define READ_ID1				0xDA
#define READ_ID2				0xDB
#define READ_ID3				0xDC
#define FRAME_RATE_CONTROL1		0xB1
#define FRAME_RATE_CONTROL2		0xB2
#define FRAME_RATE_CONTROL3		0xB3
#define DISPLAY_INVERSION		0xB4
#define SOURCE_DRIVER_DIRECTION	0xB7
#define GATE_DRIVER_DIRECTION	0xB8
#define POWER_CONTROL1			0xC0
#define POWER_CONTROL2			0xC1
#define POWER_CONTROL3			0xC2
#define POWER_CONTROL4			0xC3
#define POWER_CONTROL5			0xC4
#define VCOM_CONTROL1			0xC5
#define VCOM_CONTROL2			0xC6
#define VCOM_OFFSET_CONTROL		0xC7
#define WRITE_ID4_VALUE			0xD3
#define NV_MEMORY_FUNCTION1		0xD7
#define NV_MEMORY_FUNCTION2		0xDE
#define POSITIVE_GAMMA_CORRECT	0xE0
#define NEGATIVE_GAMMA_CORRECT	0xE1
#define GAM_R_SEL				0xF2

// Macros and in-lines:

// Translates a 3 byte RGB value into a 2 byte value for the LCD (values should be 0-31)
uint16_t decodeRgbValue(uint8_t r, uint8_t g, uint8_t b);

// This routine takes a row number from 0 to 20 and
// returns the x coordinate on the screen (0-127) to make
// it easy to place text
uint8_t lcdTextX(uint8_t x);

// This routine takes a column number from 0 to 20 and
// returns the y coordinate on the screen (0-127) to make
// it easy to place text
uint8_t lcdTextY(uint8_t y);

//	LCD function prototypes
void lcdReset(void);
void lcdWriteCommand(uint8_t address);
void lcdWriteParameter(int8_t parameter);
void lcdWriteData(uint8_t dataByte1, uint8_t dataByte2);
void lcdInitialise(uint8_t orientation);

void lcdClearDisplay(uint16_t colour);
void lcdPlot(int16_t x, int16_t y, uint16_t colour);
void lcdLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t colour);
void lcdDottedLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t colour, uint16_t step);
void lcdPolyline(int16_t *points, int16_t lines, uint16_t colour);
void lcdDottedPolyline(int16_t *points, int16_t lines, uint16_t colour, uint16_t step);
void lcdRectangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t colour);
void lcdDottedRectangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t colour, uint16_t step);
void lcdFilledRectangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t colour);
void lcdFilledDottedRectangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t colour, uint16_t step);
void lcdPolygon(int16_t *points, int16_t sides, uint16_t colour);
void lcdDottedPolygon(int16_t *points, int16_t sides, uint16_t colour, uint16_t step);
void lcdFilledTriangle(int16_t *points, uint16_t colour);
void lcdFilledDottedTriangle(int16_t *points, uint16_t colour, uint16_t step);
void lcdFilledPolygon(int16_t *points, int16_t sides, uint16_t colour);
void lcdFilledDottedPolygon(int16_t *points, int16_t sides, uint16_t colour, uint16_t step);
void lcdCircle(int16_t xCentre, int16_t yCentre, int16_t radius, uint16_t colour);
void lcdDottedCircle(int16_t xCentre, int16_t yCentre, int16_t radius, uint16_t colour, uint16_t step);
void lcdFilledCircle(int16_t xCentre, int16_t yCentre, int16_t radius, uint16_t colour);
void lcdFilledDottedCircle(int16_t xCentre, int16_t yCentre, int16_t radius, uint16_t colour, uint16_t step);

void lcdPutCh(unsigned char character, int16_t x, int16_t y, uint16_t fgColour, uint16_t bgColour);
void lcdPutChSized(unsigned char character, int16_t x, int16_t y, uint16_t fgColour, uint16_t bgColour, uint8_t size);
void lcdPutS(const char *string, int16_t x, int16_t y, uint16_t fgColour, uint16_t bgColour);
void lcdPutSSized(const char *string, int16_t x, int16_t y, uint16_t fgColour, uint16_t bgColour, uint8_t size);
uint8_t threeDto2D(int16_t *points, float *playerCoord, float alpha, float *twoDCoords);
void lcd3DPolygon(int16_t *points, int16_t sides, uint16_t colour, float px, float py, float pz, float alpha, int8_t dotted, int8_t step);
void lcd3DPolyline(int16_t *points, int16_t sides, uint16_t colour, float px, float py, float pz, float alpha, int8_t dotted, int8_t step);
//void demoPlot();

#endif /* ILI9163LCD_H_ */
