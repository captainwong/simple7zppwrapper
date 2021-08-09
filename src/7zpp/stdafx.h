// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

// add headers that you want to pre-compile here

#include <stdio.h>

#include <Common/Common.h>
#include <Common/MyWindows.h>
#include <Common/Defs.h>
#include <Common/MyInitGuid.h>
#include <Common/IntToString.h>
#include <Common/StringConvert.h>

#include <Windows/DLL.h>
#include <Windows/FileDir.h>
#include <Windows/FileFind.h>
#include <Windows/FileName.h>
#include <Windows/NtCheck.h>
#include <Windows/PropVariant.h>
#include <Windows/PropVariantConv.h>

#include <7zip/Common/FileStreams.h>
#include <7zip/Archive/IArchive.h>
#include <7zip/IPassword.h>


using namespace NWindows;
using namespace NFile;
using namespace NDir;

#ifdef _WIN64
#define kDllName "7zra_x64.dll"
#else
#define kDllName "7zra.dll"
#endif // _WIN64


constexpr auto kEmptyFileAlias = L"[Content]";

constexpr auto kIncorrectCommand = "incorrect command";

//////////////////////////////////////////////////////////////
// Archive Extracting callback class

constexpr auto kTestingString = "Testing     ";
constexpr auto kExtractingString = "Extracting  ";
constexpr auto kSkippingString = "Skipping    ";

constexpr auto kUnsupportedMethod = "Unsupported Method";
constexpr auto kCRCFailed = "CRC Failed";
constexpr auto kDataError = "Data Error";
constexpr auto kUnavailableData = "Unavailable data";
constexpr auto kUnexpectedEnd = "Unexpected end of data";
constexpr auto kDataAfterEnd = "There are some data after the end of the payload data";
constexpr auto kIsNotArc = "Is not archive";
constexpr auto kHeadersError = "Headers Error";

void Print(const char* s);
void Print(const AString& s);
void Convert_UString_to_AString(const UString& s, AString& temp);
void Print(const UString& s);
void Print(const wchar_t* s);
void PrintNewLine();
void PrintStringLn(const char* s);
void PrintError(const char* message);
void PrintError(const char* message, const FString& name);
HRESULT IsArchiveItemProp(IInArchive* archive, UInt32 index, PROPID propID, bool& result);
HRESULT IsArchiveItemFolder(IInArchive* archive, UInt32 index, bool& result);

extern bool debug;
#define dprintf if (debug) printf

#endif //PCH_H
