#pragma once


struct COLOR_HSL
{
	double hue;
	double saturation;
	double luminance;

	COLOR_HSL() : hue(0), saturation(0), luminance(0)
	{}
} ;

COLOR_HSL RGBtoHSL( COLORREF );
COLORREF HSLtoRGB( COLOR_HSL );