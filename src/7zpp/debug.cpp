#include "stdafx.h"
#include "7zpp.h"

bool debug = false;

void j7zpp_debug(bool debug_)
{
	debug = debug_;
}


void Print(const char* s)
{
	if (debug) {
		fputs(s, stdout);
	}
}

void Print(const AString& s)
{
	if (debug) {
		Print(s.Ptr());
	}
}

void Convert_UString_to_AString(const UString& s, AString& temp)
{
	int codePage = CP_OEMCP;
	/*
	int g_CodePage = -1;
	int codePage = g_CodePage;
	if (codePage == -1)
	  codePage = CP_OEMCP;
	if (codePage == CP_UTF8)
	  ConvertUnicodeToUTF8(s, temp);
	else
	*/
	UnicodeStringToMultiByte2(temp, s, (UINT)codePage);
}

void Print(const UString& s)
{
	if (debug) {
		AString as;
		Convert_UString_to_AString(s, as);
		Print(as);
	}
}

void Print(const wchar_t* s)
{
	if (debug) {
		Print(UString(s));
	}
}

void PrintNewLine()
{
	if (debug) {
		Print("\n");
	}
}

void PrintStringLn(const char* s)
{
	if (debug) {
		Print(s);
		PrintNewLine();
	}
}

void PrintError(const char* message)
{
	if (debug) {
		Print("Error: ");
		PrintNewLine();
		Print(message);
		PrintNewLine();
	}
}

void PrintError(const char* message, const FString& name)
{
	if (debug) {
		PrintError(message);
		Print(name);
	}
}

HRESULT IsArchiveItemProp(IInArchive* archive, UInt32 index, PROPID propID, bool& result)
{
	NCOM::CPropVariant prop;
	RINOK(archive->GetProperty(index, propID, &prop));
	if (prop.vt == VT_BOOL)
		result = VARIANT_BOOLToBool(prop.boolVal);
	else if (prop.vt == VT_EMPTY)
		result = false;
	else
		return E_FAIL;
	return S_OK;
}

HRESULT IsArchiveItemFolder(IInArchive* archive, UInt32 index, bool& result)
{
	return IsArchiveItemProp(archive, index, kpidIsDir, result);
}