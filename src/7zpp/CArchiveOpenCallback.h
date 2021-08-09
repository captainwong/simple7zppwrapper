#pragma once


#include <7zip/IPassword.h>
#include <7zip/Archive/IArchive.h>
#include <7zip/Common/FileStreams.h>

#include <Common/MyCom.h>
#include <Common/MyString.h>

#include <Windows/FileDir.h>
#include <Windows/FileFind.h>
#include <Windows/FileName.h>
#include <Windows/PropVariant.h>
#include <Windows/PropVariantConv.h>


using namespace NWindows;
using namespace NFile;
using namespace NDir;


//////////////////////////////////////////////////////////////
// Archive Open callback class
class CArchiveOpenCallback :
	public IArchiveOpenCallback,
	public ICryptoGetTextPassword,
	public CMyUnknownImp
{
public:
	MY_UNKNOWN_IMP1(ICryptoGetTextPassword)

	STDMETHOD(SetTotal)(const UInt64* files, const UInt64* bytes);
	STDMETHOD(SetCompleted)(const UInt64* files, const UInt64* bytes);

	STDMETHOD(CryptoGetTextPassword)(BSTR* password);

	bool PasswordIsDefined;
	UString Password;

	CArchiveOpenCallback() : PasswordIsDefined(false) {}
};
