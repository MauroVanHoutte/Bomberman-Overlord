#include "stdafx.h"
#include "SpriteFontLoader.h"
#include "BinaryReader.h"
#include "ContentManager.h"
#include "TextureData.h"

SpriteFont* SpriteFontLoader::LoadContent(const std::wstring& assetFile)
{
	auto pBinReader = new BinaryReader();
	pBinReader->Open(assetFile);

	if (!pBinReader->Exists())
	{
		Logger::LogFormat(LogLevel::Error, L"SpriteFontLoader::LoadContent > Failed to read the assetFile!\nPath: \'%s\'", assetFile.c_str());
		return nullptr;
	}

	//See BMFont Documentation for Binary Layout

	//Parse the Identification bytes (B,M,F)
	//If Identification bytes doesn't match B|M|F,
	//Log Error (SpriteFontLoader::LoadContent > Not a valid .fnt font) &
	//return nullptr
	//...
	char B = pBinReader->Read<char>();
	char M = pBinReader->Read<char>();
	char F = pBinReader->Read<char>();
	if (B != 'B' || M != 'M' || F != 'F')
	{
		Logger::LogError(L"SpriteFontLoader::LoadContent > Not a valid .fnt font");
		return nullptr;
	}

	//Parse the version (version 3 required)
	//If version is < 3,
	//Log Error (SpriteFontLoader::LoadContent > Only .fnt version 3 is supported)
	//return nullptr
	//...
	char version = pBinReader->Read<char>();
	if (version != 3)
	{
		Logger::LogError(L"SpriteFontLoader::LoadContent > only .fnt version 3 is supported");
		return nullptr;
	}

	//Valid .fnt file
	auto pSpriteFont = new SpriteFont();
	//SpriteFontLoader is a friend class of SpriteFont
	//That means you have access to its privates (pSpriteFont->m_FontName = ... is valid)

	//**********
	// BLOCK 0 *
	//**********
	//Retrieve the blockId and blockSize
	//Retrieve the FontSize (will be -25, BMF bug) [SpriteFont::m_FontSize]
	//Move the binreader to the start of the FontName [BinaryReader::MoveBufferPosition(...) or you can set its position using BinaryReader::SetBufferPosition(...))
	//Retrieve the FontName [SpriteFont::m_FontName]
	//...
	pBinReader->Read<char>();
	pBinReader->Read<int>();
	short int fontSize = pBinReader->Read<short int>();
	pBinReader->MoveBufferPosition(12);
	auto fontName = pBinReader->ReadNullString();

	pSpriteFont->m_FontSize = fontSize;
	pSpriteFont->m_FontName = fontName;

	//**********
	// BLOCK 1 *
	//**********
	//Retrieve the blockId and blockSize
	//Retrieve Texture Width & Height [SpriteFont::m_TextureWidth/m_TextureHeight]
	//Retrieve PageCount
	//> if pagecount > 1
	//> Log Error (SpriteFontLoader::LoadContent > SpriteFont (.fnt): Only one texture per font allowed)
	//Advance to Block2 (Move Reader)
	//...
	pBinReader->Read<char>();
	pBinReader->Read<int>();
	pBinReader->MoveBufferPosition(4);
	pSpriteFont->m_TextureWidth = pBinReader->Read<unsigned short int>();
	pSpriteFont->m_TextureHeight = pBinReader->Read<unsigned short int>();
	unsigned short int nrPages = pBinReader->Read<unsigned short int>();

	if (nrPages > 1)
	{
		Logger::LogError(L"SpriteFontLoader::LoadContent > SpriteFont (.fnt): Only one texture per font allowed");
		return nullptr;
	}
	pBinReader->MoveBufferPosition(5);


	//**********
	// BLOCK 2 *
	//**********
	//Retrieve the blockId and blockSize
	//Retrieve the PageName (store Local)
	//	> If PageName is empty
	//	> Log Error (SpriteFontLoader::LoadContent > SpriteFont (.fnt): Invalid Font Sprite [Empty])
	//>Retrieve texture filepath from the assetFile path
	//> (ex. c:/Example/somefont.fnt => c:/Example/) [Have a look at: wstring::rfind()]
	//>Use path and PageName to load the texture using the ContentManager [SpriteFont::m_pTexture]
	//> (ex. c:/Example/ + 'PageName' => c:/Example/somefont_0.png)
	//...
	pBinReader->Read<char>();
	pBinReader->Read<int>();
	std::wstring pageName = pBinReader->ReadNullString();

	if (pageName.empty())
	{
		Logger::LogError(L"SpriteFontLoader::LoadContent > SpriteFont (.fnt): Invalid Font Sprite");
		return nullptr;
	}

	auto pos = assetFile.rfind('/');

	std::wstring fontPath = assetFile.substr(0, pos+1) + pageName;

	pSpriteFont->m_pTexture = ContentManager::Load<TextureData>(fontPath);

	//**********
	// BLOCK 3 *
	//**********
	//Retrieve the blockId and blockSize
	//Retrieve Character Count (see documentation)
	//Retrieve Every Character, For every Character:
	//> Retrieve CharacterId (store Local) and cast to a 'wchar_t'
	//> Check if CharacterId is valid (SpriteFont::IsCharValid), Log Warning and advance to next character if not valid
	//> Retrieve the corresponding FontMetric (SpriteFont::GetMetric) [REFERENCE!!!]
	//> Set IsValid to true [FontMetric::IsValid]
	//> Set Character (CharacterId) [FontMetric::Character]
	//> Retrieve Xposition (store Local)
	//> Retrieve Yposition (store Local)
	//> Retrieve & Set Width [FontMetric::Width]
	//> Retrieve & Set Height [FontMetric::Height]
	//> Retrieve & Set OffsetX [FontMetric::OffsetX]
	//> Retrieve & Set OffsetY [FontMetric::OffsetY]
	//> Retrieve & Set AdvanceX [FontMetric::AdvanceX]
	//> Retrieve & Set Page [FontMetric::Page]
	//> Retrieve Channel (BITFIELD!!!) 
	//	> See documentation for BitField meaning [FontMetrix::Channel]
	//> Calculate Texture Coordinates using Xposition, Yposition, TextureWidth & TextureHeight [FontMetric::TexCoord]
	//...

	pBinReader->Read<char>();
	int size = pBinReader->Read<int>();

	int nrChars = size / 20; //20 per characters

	for (int i = 0; i < nrChars; i++)
	{
		wchar_t id = (wchar_t)pBinReader->Read<int>();
		if (!pSpriteFont->IsCharValid(id))
		{
			Logger::LogWarning(L"SprintFontLoader::LoadContent > Char is not valid, id: " + id);
		}

		FontMetric& metric = pSpriteFont->GetMetric(id);
		metric.IsValid = true;
		metric.Character = id;
		unsigned short xPos = pBinReader->Read<unsigned short>();
		unsigned short yPos = pBinReader->Read<unsigned short>();
		metric.Width = pBinReader->Read<unsigned short>();
		metric.Height = pBinReader->Read<unsigned short>();
		metric.OffsetX = pBinReader->Read<short>();
		metric.OffsetY = pBinReader->Read<short>();
		metric.AdvanceX = pBinReader->Read<short>();
		metric.Page = pBinReader->Read<unsigned char>();
		byte channelBitmask = pBinReader->Read<byte>();
		if (channelBitmask & 1)	metric.Channel = 2;
		else if (channelBitmask & 2) metric.Channel = 1;
		else if (channelBitmask & 4) metric.Channel = 0;
		else if (channelBitmask & 8) metric.Channel = 3;
		else if (channelBitmask & 16) metric.Channel = 4;
		metric.TexCoord.x = (float)xPos / pSpriteFont->m_TextureWidth;
		metric.TexCoord.y = (float)yPos / pSpriteFont->m_TextureHeight;
	}

	//DONE :)

	delete pBinReader;
	return pSpriteFont;
}

void SpriteFontLoader::Destroy(SpriteFont* objToDestroy)
{
	SafeDelete(objToDestroy);
}
