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
#include <LLUtils/Exception.h>
#include "xxh3.h"

std::filesystem::path folderToSaveFiles = "d:/testImages/";
bool shouldSaveToFile = false;

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


struct TestParams
{
	std::wstring fileName;
	XXH64_hash_t expectedHash;
	bool saveToFile;
};



void runTest(FreeType::TextCreateParams freetypeParams, TestParams testParams)
{
	using namespace FreeType;
	using namespace LLUtils;
	FreeTypeConnector::Bitmap textBitmap;
	FreeTypeConnector::GetSingleton().CreateBitmap(freetypeParams, textBitmap);
	XXH64_hash_t expected = 7683707409401849002;
	auto hash = XXH3_64bits(static_cast<const void*>(textBitmap.buffer.data()), textBitmap.height * textBitmap.rowPitch);
	
	if (testParams.saveToFile)
		SaveToFile(textBitmap, testParams.fileName);

	if (hash != testParams.expectedHash)
		throw std::exception("test failed"); 

}

int runtests()
{
	using namespace FreeType;
	using namespace LLUtils;
	FreeType::TextCreateParams params{};
	TestParams testParams{};
	testParams.saveToFile = shouldSaveToFile;
	//Several test cases, for now mostly checks for out of bounds errors, if there's no exceptions test is considered passed.

	params.DPIx = 120;
	params.DPIy = 120;
	params.fontPath = L"C:/Windows/Fonts/segoeuib.ttf";
	params.text = L"ijkjojujrjaj";
	params.textColor = LLUtils::Colors::Black;
	params.backgroundColor = LLUtils::Colors::White;
	params.fontSize = 44;
	params.renderMode = FreeType::RenderMode::Antialiased;
	params.outlineWidth = 0;

	testParams.fileName = folderToSaveFiles / "test10.bmp";
	testParams.expectedHash = 7683707409401849002;

	runTest(params, testParams);





	params.DPIx = 120;
	params.DPIy = 120;
	params.fontPath = L"C:/Windows/Fonts/segoeui.ttf";
	params.text = L"<textcolor=#000000ff>|This| is זה משהו\n באמת משהו\nabcdefghijklmnopqrstuvwwxyz\nABCDEFGHIJKLMNOPQVWXYZ\n|!#_+";

	params.backgroundColor = LLUtils::Colors::White;
	params.fontSize = 44;
	params.renderMode = FreeType::RenderMode::Antialiased;
	params.outlineWidth = 0;
	params.padding = 0;
	params.flags = FreeType::TextCreateFlags::UseMetaText |
		FreeType::TextCreateFlags::Bidirectional;

	testParams.fileName = folderToSaveFiles / "test.bmp";
	testParams.expectedHash = 16738229490926515141;

	runTest(params, testParams);



	//Several test cases, for now mostly checks for out of bounds errors, if there's no exceptions test is considered passed.

	params.DPIx = 120;
	params.DPIy = 120;
	params.fontPath = L"C:/Windows/Fonts/segoeuib.ttf";
	params.text = L"<textcolor=#00ff00ff>|This| is זה משהו\n באמת משהו\nabcdefghijklmnopqrstuvwwxyz\nABCDEFGHIJKLMNOPQVWXYZ\n|!#_+";

	params.text = L"<textcolor=#4a80e2>Welcome to <textcolor=#dd0f1d>OIV\n"\
		"<textcolor=#25bc25>Drag <textcolor=#4a80e2>here an image to start\n"\
		"Press <textcolor=#25bc25>F1<textcolor=#4a80e2> to show key bindings";


	params.backgroundColor = LLUtils::Colors::White;
	params.fontSize = 44;
	params.renderMode = FreeType::RenderMode::Antialiased;
	params.outlineWidth = 3;
	params.padding = 0;
	params.flags = FreeType::TextCreateFlags::UseMetaText |
		FreeType::TextCreateFlags::Bidirectional;

	testParams.fileName = folderToSaveFiles / "test1.bmp";
	testParams.expectedHash = 16558562707942498804;

	runTest(params, testParams);





	//params.text = L"3000 X 1712 X 32 BPP | loaded in 92.7 ms";
	params.text = L"Texel: 1218.3 X  584.6";
	params.fontPath = L"C:/Windows/Fonts/consola.ttf";
	params.renderMode = FreeType::RenderMode::SubpixelAntiAliased;
	params.fontSize = 11;
	params.backgroundColor = { 255, 255, 255, 192 };
	params.DPIx = 120;
	params.DPIy = 120;
	//params.padding = 1;
	testParams.fileName = folderToSaveFiles / "test2.bmp";
	testParams.expectedHash = 11320992707252375232;
	runTest(params, testParams);




	params.text = L"<textcolor=#ff8930>windowed";
	params.fontPath = L"C:/Windows/Fonts/segoeuib.ttf";
	params.renderMode = FreeType::RenderMode::SubpixelAntiAliased;
	params.fontSize = 11;
	params.backgroundColor = { 255, 255, 255, 192 };
	params.DPIx = 120;
	params.DPIy = 120;
	params.padding = 0;

	testParams.fileName = folderToSaveFiles / "test3.bmp";
	testParams.expectedHash = 9753445643566658639;
	runTest(params, testParams);



	//Test Fixed width font

	params.text = L"<textcolor=#ff8930>444";
	params.fontPath = L"C:/Windows/Fonts/consola.ttf";
	params.renderMode = FreeType::RenderMode::Antialiased;
	params.fontSize = 11;
	params.backgroundColor = { 255, 255, 255, 192 };
	params.DPIx = 120;
	params.DPIy = 120;
	params.padding = 0;

	testParams.fileName = folderToSaveFiles / "test4_1.bmp";
	testParams.expectedHash = 11631623323771771341;
	runTest(params, testParams);


	params.text = L"<textcolor=#ff8930>555";

	testParams.fileName = folderToSaveFiles / "test4_2.bmp";
	testParams.expectedHash = 17162733075979477580;
	runTest(params, testParams);



	/*if (textBitmap4.width != textBitmap5.width)
		LL_EXCEPTION(LLUtils::Exception::ErrorCode::InvalidState, "mismatch size");*/


		//test very thick outline 
	params.text = L"<textcolor=#ff8930>windowed";
	params.fontPath = L"C:/Windows/Fonts/segoeuib.ttf";
	params.renderMode = FreeType::RenderMode::Antialiased;
	params.fontSize = 11;
	params.backgroundColor = { 255, 255, 255, 192 };
	params.DPIx = 120;
	params.DPIy = 120;
	params.padding = 0;
	params.outlineWidth = 20;

	testParams.fileName = folderToSaveFiles / "test5.bmp";
	testParams.expectedHash = 4822496049661565882;
	runTest(params, testParams);


	//Lower dpi mode
	params.text = L"abcdefg.tif";
	params.fontPath = L"C:/Windows/Fonts/segoeuib.ttf";
	params.renderMode = FreeType::RenderMode::Antialiased;
	params.fontSize = 12;
	params.backgroundColor = { 255, 255, 255, 192 };
	params.DPIx = 96;
	params.DPIy = 96;
	params.padding = 0;
	params.outlineWidth = 2;

	testParams.fileName = folderToSaveFiles / "test6.bmp";
	testParams.expectedHash = 3439216908320477038;
	runTest(params, testParams);

	return 0;
}

int main()
{

	try
	{
		if (runtests() == 0)
			std::cout << "All tests passed successfully.";
	}
	catch (...)
	{
		std::cout << "One or more of the test have failed.";
	}
}
