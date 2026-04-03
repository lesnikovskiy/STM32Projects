/*
 * main.c
 *
 *  Created on: 8 січ. 2026 р.
 *      Author: Lesnikovskiy
 */
#include <stdio.h>

int main(void)
{
	unsigned char distanceA2B = 160;
	unsigned char distanceB2C = 40;

	unsigned char totalDistance = distanceA2B + distanceB2C;

	printf("Total distance from A to C is %d\n", totalDistance);

	signed char numNegative = 0b11100111; // -25
	unsigned char numPositive = 0b00011001; // 25
	printf("%d\n", numNegative);
	printf("%d\n", numPositive);

	signed char signedMinValue = 0b10000000;
	signed char signedMaxValue = 0b11111111;

	printf("char signed min value = %d (%X)\n", signedMinValue, signedMinValue);
	printf("char signed max value = %d (%X)\n", signedMaxValue, signedMaxValue);

	unsigned char unsignedMinValue = 0b00000000;
	unsigned char unsignedMaxValue = 0b01111111;

	printf("char unsigned min value = %d (%X)\n", unsignedMinValue, unsignedMinValue);
	printf("char unsigned max value = %d (%X)\n", unsignedMaxValue, unsignedMaxValue);

	signed short signedShortVal = 0b0000000000011001;
	unsigned short unsignedShortVal = 0b1111111111100110;

	printf("signed short %d (%X)\n", signedShortVal, signedShortVal);
	printf("unsigned short %d (%X)\n", unsignedShortVal, unsignedShortVal);

	return 0;
}
