#pragma once

#include "../lzmasdk/C/7zTypes.h"
#include "../lzmasdk/CPP/7zip/ICoder.h"
#include "../lzmasdk/CPP/7zip/Archive/IArchive.h"
#include <Windows.h>

STDAPI CreateObject(const GUID* clsid, const GUID* iid, void** outObject);

STDAPI GetHandlerProperty(PROPID propID, PROPVARIANT* value);
STDAPI GetNumberOfFormats(UINT32* numFormats);
STDAPI GetHandlerProperty2(UInt32 formatIndex, PROPID propID, PROPVARIANT* value);
STDAPI GetIsArc(UInt32 formatIndex, Func_IsArc* isArc);

STDAPI GetNumberOfMethods(UINT32* numCodecs);
STDAPI GetMethodProperty(UInt32 codecIndex, PROPID propID, PROPVARIANT* value);
STDAPI CreateDecoder(UInt32 index, const GUID* iid, void** outObject);
STDAPI CreateEncoder(UInt32 index, const GUID* iid, void** outObject);

STDAPI GetHashers(IHashers** hashers);

STDAPI SetCodecs(ICompressCodecsInfo*);

STDAPI SetLargePageMode();
STDAPI SetCaseSensitive(Int32 caseSensitive);
