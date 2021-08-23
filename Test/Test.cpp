/*
Copyright (c) 2021 Lior Lahav

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

void SaveToFile(const FreeType::FreeTypeConnector::Bitmap& textBitmap, const std::wstring& filePath)
{
	using namespace FreeType;
	BitmapBuffer bitmapBuffer{ textBitmap.buffer.data()
		,static_cast<uint8_t>(textBitmap.PixelSize * CHAR_BIT)
		,textBitmap.width
		, textBitmap.height
			, textBitmap.rowPitch
	};

	Bitmap bitmap(bitmapBuffer);
	bitmap.SaveToFile(filePath);
}



int main()
{
	using namespace FreeType;
	using namespace LLUtils;
	FreeTypeConnector::TextCreateParams params{};
	//Several test cases, for now mostly checks for out of bounds errors, if there's no exceptions test is considered passed.
	{
		params.DPIx = 120;
		params.DPIy = 120;
		params.fontPath = L"C:/Windows/Fonts/segoeuib.ttf";
		params.text = L"<textcolor=#00ff00ff>|This| is זה משהו\n באמת משהו\nabcdefghijklmnopqrstuvwwxyz\nABCDEFGHIJKLMNOPQVWXYZ\n|!#_+";

		params.text = L"<textcolor=#4a80e2>Welcome to <textcolor=#dd0f1d>OIV\n"\
			"<textcolor=#25bc25>Drag <textcolor=#4a80e2>here an image to start\n"\
			"Press <textcolor=#25bc25>F1<textcolor=#4a80e2> to show key bindings";


		params.backgroundColor = LLUtils::Colors::White;
		params.fontSize = 44;
		params.renderMode = FreeTypeConnector::RenderMode::Antialiased;
		params.outlineWidth = 3;
		params.padding = 0;
		FreeTypeConnector::Bitmap textBitmap;
		FreeTypeConnector::GetSingleton().CreateBitmap(params, textBitmap);
		SaveToFile(textBitmap, L"d:/test1.bmp");
	}

	{
		//params.text = L"3000 X 1712 X 32 BPP | loaded in 92.7 ms";
		params.text = L"Texel: 1218.3 X  584.6";
		params.fontPath = L"C:/Windows/Fonts/consola.ttf";
		params.renderMode = FreeTypeConnector::RenderMode::SubpixelAntiAliased;
		params.fontSize = 11;
		params.backgroundColor = { 255, 255, 255, 192 };
		params.DPIx = 120;
		params.DPIy = 120;
		//params.padding = 1;
		FreeTypeConnector::Bitmap textBitmap;
		FreeTypeConnector::GetSingleton().CreateBitmap(params, textBitmap);
		SaveToFile(textBitmap, L"d:/test2.bmp");
	}

	{
		params.text = L"<textcolor=#ff8930>windowed";
		params.fontPath = L"C:/Windows/Fonts/segoeuib.ttf";
		params.renderMode = FreeTypeConnector::RenderMode::SubpixelAntiAliased;
		params.fontSize = 11;
		params.backgroundColor = { 255, 255, 255, 192 };
		params.DPIx = 120;
		params.DPIy = 120;
		params.padding = 0;
		FreeTypeConnector::Bitmap textBitmap;
		FreeTypeConnector::GetSingleton().CreateBitmap(params, textBitmap);
		SaveToFile(textBitmap, L"d:/test3.bmp");
	}

	//Test Fixed width font
	{
		params.text = L"<textcolor=#ff8930>444";
		params.fontPath = L"C:/Windows/Fonts/consola.ttf";
		params.renderMode = FreeTypeConnector::RenderMode::Antialiased;
		params.fontSize = 11;
		params.backgroundColor = { 255, 255, 255, 192 };
		params.DPIx = 120;
		params.DPIy = 120;
		params.padding = 0;

		FreeTypeConnector::Bitmap textBitmap4;
		FreeTypeConnector::GetSingleton().CreateBitmap(params, textBitmap4);
		SaveToFile(textBitmap4, L"d:/test4_1.bmp");

		params.text = L"<textcolor=#ff8930>555";
		FreeTypeConnector::Bitmap textBitmap5;
		FreeTypeConnector::GetSingleton().CreateBitmap(params, textBitmap5);
		SaveToFile(textBitmap5, L"d:/test4_2.bmp");
	}

	FreeTypeConnector::Bitmap textBitmap;
	FreeTypeConnector::GetSingleton().CreateBitmap(params, textBitmap);


}
