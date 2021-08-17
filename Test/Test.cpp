/*
Copyright (c) 2020 Lior Lahav

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/


#include <iostream>
#include <array>
#include <FreeTypeWrapper/FreeTypeConnector.h>
#include <FreeTypeWrapper/BitmapFile.h>
#include <LLUtils/Colors.h>

//button support for multiple keyboards


int main()
{
	using namespace FreeType;
	using namespace LLUtils;
	FreeTypeConnector::TextCreateParams params{};
	params.DPIx = 120;
	params.DPIy = 120;
	//params.fontPath = L"C:/Windows/Fonts/consola.ttf";
	params.fontPath = L"C:/Windows/Fonts/segoeui.ttf";
	
	

	params.fontSize = 16;
	params.outlineWidth = 2;

	params.outlineColor = Colors::Darkmagenta;
	params.backgroundColor = Colors::AbsoluteZero; ; 

	
	params.text = L"<textcolor=#00ff00ff>|This| is זה משהו\n באמת משהו\nabcdefghijklmnopqrstuvwwxyz\nABCDEFGHIJKLMNOPQVWXYZ\n|!#_+";
	
	params.renderMode = FreeTypeConnector::RenderMode::Antialiased;


	FreeTypeConnector::Bitmap textBitmap;
	FreeTypeConnector::GetSingleton().CreateBitmap(params, textBitmap);

	BitmapBuffer bitmapBuffer{ textBitmap.buffer.data()
		,textBitmap.PixelSize * CHAR_BIT
		,textBitmap.width
		, textBitmap.height
			, textBitmap.rowPitch
	};

	Bitmap bitmap(bitmapBuffer);
	bitmap.SaveToFile(L"d:/test.bmp");

}
