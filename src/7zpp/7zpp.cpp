#include "stdafx.h"
#include "7zpp.h"
#include "CArchiveOpenCallback.h"
#include "CArchiveUpdateCallback.h"
#include "CArchiveExtractCallback.h"
#include <jlib/win32/UnicodeTool.h>
#include <functional>
#ifdef MY7ZPPWITH7ZRASTATIC_EXPORTS
#include "../7zra_static/7zra_static.h"
#endif

#ifdef _WIN32
HINSTANCE g_hInstance = 0;
#endif

// Tou can find the list of all GUIDs in Guid.txt file.
// use another CLSIDs, if you want to support other formats (zip, rar, ...).
// {23170F69-40C1-278A-1000-000110070000}

DEFINE_GUID(CLSID_CFormat7z,
			0x23170F69, 0x40C1, 0x278A, 0x10, 0x00, 0x00, 0x01, 0x10, 0x07, 0x00, 0x00);
DEFINE_GUID(CLSID_CFormatXz,
			0x23170F69, 0x40C1, 0x278A, 0x10, 0x00, 0x00, 0x01, 0x10, 0x0C, 0x00, 0x00);

#define CLSID_Format CLSID_CFormat7z
// #define CLSID_Format CLSID_CFormatXz

    
static const char* const kCopyrightString =
"\n"
"7-Zip"
" (" kDllName " client)"
" " "19.00"
" : " "Igor Pavlov : Public domain : 2019-02-21"
"\n";

static const char* const kHelpString =
"Usage: 7zcl.exe [a | l | x] archive.7z [fileName ...]\n"
"Examples:\n"
"  7zcl.exe a archive.7z f1.txt f2.txt  : compress two files to archive.7z\n"
"  7zcl.exe l archive.7z   : List contents of archive.7z\n"
"  7zcl.exe x archive.7z   : eXtract files from archive.7z\n";


#ifdef MY7ZPPWITH7ZRASTATIC_EXPORTS
#include "7zip/Common/RegisterArc.h"
#include "7zip/Archive/7z/7zHandler.h"

namespace NArchive {
namespace N7z {

static Byte k_Signature_Dec[kSignatureSize] = { '7' + 1, 'z', 0xBC, 0xAF, 0x27, 0x1C };

REGISTER_ARC_IO_DECREMENT_SIG(
    "7z", "7z", NULL, 7,
    k_Signature_Dec,
    0,
    NArcInfoFlags::kFindSignature,
    NULL);

}
}

#include "../C/7zCrc.h"
struct CCRCTableInit { CCRCTableInit() { CrcGenerateTable(); } } g_CRCTableInit;


#include "7zip/Common/RegisterCodec.h"

#include "7zip/Compress/LzmaDecoder.h"
#include "7zip/Compress/Lzma2Decoder.h"

#ifndef EXTRACT_ONLY
#include "7zip/Compress/LzmaEncoder.h"
#include "7zip/Compress/Lzma2Encoder.h"
#endif

namespace NCompress {
namespace NLzma {

REGISTER_CODEC_E(LZMA,
                 CDecoder(),
                 CEncoder(),
                 0x30101,
                 "LZMA")

}
namespace NLzma2 {

REGISTER_CODEC_E(LZMA2,
                 CDecoder(),
                 CEncoder(),
                 0x21,
                 "LZMA2")

}
}

#endif

static FString CmdStringToFString(const char* s)
{
    return us2fs(GetUnicodeString(s));
}

int j7zpp_compress_folder(const std::wstring& archive, const std::wstring& folder)
{
	return 0;
}


j7zpp_res j7zpp_compress_files(const std::wstring& archive,
                               const std::vector<std::wstring>& files,
                               const std::wstring& dirPrefix4NotShowInArchive, 
                               ArchiveUpdateProgressCallback cb)
{
#ifndef MY7ZPPWITH7ZRASTATIC_EXPORTS
    NDLL::CLibrary lib;
    if (!lib.Load(NDLL::GetModuleDirPrefix() + FTEXT(kDllName))) {
        PrintError("Can not load 7-zip library");
        return j7zpp_res::load_7zra_dll_fail;
    }

    Func_CreateObject createObjectFunc = (Func_CreateObject)lib.GetProc("CreateObject");
    if (!createObjectFunc) {
        PrintError("Can not get CreateObject");
        return j7zpp_res::create_obj_fail;
    }
#endif

    CObjectVector<CDirItem> dirItems;
    for (const auto& file : files) {
        CDirItem di;
        FString name = (file.c_str());

        NFind::CFileInfo fi;
        if (!fi.Find(name)) {
            PrintError("Can't find file", name);
            return j7zpp_res::open_file_fail;
        }

        di.Attrib = fi.Attrib;
        di.Size = fi.Size;
        di.CTime = fi.CTime;
        di.ATime = fi.ATime;
        di.MTime = fi.MTime;
        di.FullPath = name;
        

        std::wstring basename;
        auto pos = file.find(dirPrefix4NotShowInArchive);
        if (pos != std::string::npos) {
            basename = file.substr(dirPrefix4NotShowInArchive.size());
            while (basename.front() == L'\\' || basename.front() == L'/') {
                basename = basename.substr(1);
            }
        } else {
            auto pos = file.find_last_of(L'\\');
            if (pos == std::string::npos) {
                pos = file.find_last_of(L'/');
            }
            if (pos != std::string::npos) {
                basename = file.substr(pos + 1);
            } else {
                basename = file;
            }
        }
        
        di.Name = fs2us((basename.c_str()));
        dirItems.Add(di);
    }

    COutFileStream* outFileStreamSpec = new COutFileStream;
    CMyComPtr<IOutStream> outFileStream = outFileStreamSpec;
    FString archiveName = (archive.c_str());
    if (!outFileStreamSpec->Create(archiveName, false))     {
        PrintError("can't create archive file");
        return j7zpp_res::create_file_fail;
    }


#ifdef MY7ZPPWITH7ZRASTATIC_EXPORTS
    CMyComPtr<IOutArchive> outArchive;
    if (CreateObject(&CLSID_Format, &IID_IOutArchive, (void**)&outArchive) != S_OK) {
        PrintError("Can not get class object");
        return j7zpp_res::get_class_obj_fail;
    }
#else
    CMyComPtr<IOutArchive> outArchive;
    if (createObjectFunc(&CLSID_Format, &IID_IOutArchive, (void**)&outArchive) != S_OK) {
        PrintError("Can not get class object");
        return j7zpp_res::get_class_obj_fail;
    }
#endif
    

    CArchiveUpdateCallback* updateCallbackSpec = new CArchiveUpdateCallback;
    CMyComPtr<IArchiveUpdateCallback2> updateCallback(updateCallbackSpec);
    updateCallbackSpec->Init(&dirItems);
    updateCallbackSpec->setArchiveUpdateProgressCallback(cb);

    // updateCallbackSpec->PasswordIsDefined = true;
    // updateCallbackSpec->Password = L"1";

    //
    //{
    //  const wchar_t *names[] =
    //  {
    //    L"s",
    //    L"x"
    //  };
    //  const unsigned kNumProps = ARRAY_SIZE(names);
    //  NCOM::CPropVariant values[kNumProps] =
    //  {
    //    false,    // solid mode OFF
    //    (UInt32)9 // compression level = 9 - ultra
    //  };
    //  CMyComPtr<ISetProperties> setProperties;
    //  outArchive->QueryInterface(IID_ISetProperties, (void **)&setProperties);
    //  if (!setProperties)
    //  {
    //    PrintError("ISetProperties unsupported");
    //    return 1;
    //  }
    //  RINOK(setProperties->SetProperties(names, values, kNumProps));
    //}
    

    HRESULT result = outArchive->UpdateItems(outFileStream, dirItems.Size(), updateCallback);
    if (result != S_OK) {
        PrintError("Update Error");
        return j7zpp_res::update_fail;
    }

    result = updateCallbackSpec->Finilize();
    if (result != S_OK)     {
        PrintError("Finilize Error");
        return j7zpp_res::finalize_fail;
    }

    FOR_VECTOR(i, updateCallbackSpec->FailedFiles)
    {
        PrintNewLine();
        PrintError("Error for file", updateCallbackSpec->FailedFiles[i]);
    }

    if (updateCallbackSpec->FailedFiles.Size() != 0)
        return j7zpp_res::update_fail;

    return j7zpp_res::ok;
}

static bool jfi_find_dir(jfile_info& all, const std::wstring& dirname, jfile_info*& dir)
{
    for (const auto& jfi : all.subitems) {
        if (jfi->isdir) {
            if (jfi->name == dirname) {
                dir = jfi;
                return true;
            }
        }
    }

    auto jfi = new jfile_info();
    jfi->isdir = true;
    jfi->name = dirname;
    all.subitems.push_back(jfi);
    dir = jfi;

    return true;
}

static void jfi_resolve_dir(jfile_info& all, const std::wstring& dir)
{
    auto pos = dir.find('\\');
    if (pos == std::string::npos) {
        auto jfi = new jfile_info();
        jfi->isdir = true;
        jfi->name = dir;
        all.subitems.push_back(jfi);
    } else {
        jfile_info* jfi = nullptr;
        if (jfi_find_dir(all, dir.substr(0, pos), jfi) && jfi) {
            jfi_resolve_dir(*jfi, dir.substr(pos + 1));
        } 
    }
}

static void jfi_resolve_file(jfile_info& all, const std::wstring& name, uint64_t size)
{
    auto pos = name.find('\\');
    if (pos == std::string::npos) {
        auto jfi = new jfile_info();
        jfi->isdir = false;
        jfi->name = name;
        jfi->size = size;
        all.subitems.push_back(jfi);
    } else {
        jfile_info* jfi = nullptr;
        if (jfi_find_dir(all, name.substr(0, pos), jfi) && jfi) {
            jfi_resolve_file(*jfi, name.substr(pos + 1), size);
        }
    }
}

void j7zpp_release_jfile_info(jfile_info* jfi)
{
    if (jfi) {
        if (jfi->isdir) {
            for (auto i : jfi->subitems) {
                j7zpp_release_jfile_info(i);
            }
            jfi->subitems.clear();
        }
        delete jfi;
    }
}

j7zpp_res j7zpp_list_files(const std::wstring& archiveName_, jfile_info** jfinfo)
{
#ifdef MY7ZPPWITH7ZRASTATIC_EXPORTS
    CMyComPtr<IInArchive> archive;
    if (CreateObject(&CLSID_Format, &IID_IInArchive, (void**)&archive) != S_OK) {
        PrintError("Can not get class object");
        return j7zpp_res::get_class_obj_fail;
    }

#else
    NDLL::CLibrary lib;
    if (!lib.Load(NDLL::GetModuleDirPrefix() + FTEXT(kDllName))) {
        PrintError("Can not load 7-zip library");
        return j7zpp_res::load_7zra_dll_fail;
    }

    Func_CreateObject createObjectFunc = (Func_CreateObject)lib.GetProc("CreateObject");
    if (!createObjectFunc) {
        PrintError("Can not get CreateObject");
        return j7zpp_res::create_obj_fail;
    }

    CMyComPtr<IInArchive> archive;
    if (createObjectFunc(&CLSID_Format, &IID_IInArchive, (void**)&archive) != S_OK) {
        PrintError("Can not get class object");
        return j7zpp_res::get_class_obj_fail;
    }

#endif

    CInFileStream* fileSpec = new CInFileStream;
    CMyComPtr<IInStream> file = fileSpec;
    FString archiveName = (archiveName_.c_str());
    if (!fileSpec->Open(archiveName)) {
        PrintError("Can not open archive file", archiveName);
        return j7zpp_res::open_file_fail;
    }

    {
        CArchiveOpenCallback* openCallbackSpec = new CArchiveOpenCallback;
        CMyComPtr<IArchiveOpenCallback> openCallback(openCallbackSpec);
        openCallbackSpec->PasswordIsDefined = false;
        // openCallbackSpec->PasswordIsDefined = true;
        // openCallbackSpec->Password = L"1";

        const UInt64 scanSize = 1 << 23;
        if (archive->Open(file, &scanSize, openCallback) != S_OK) {
            PrintError("Can not open file as archive", archiveName);
            return j7zpp_res::open_file_fail;
        }
    }

    UInt32 numItems = 0;
    archive->GetNumberOfItems(&numItems);

    jfile_info* all = nullptr;
    if (numItems > 0) {
        all = *jfinfo = new jfile_info();
        all->isdir = true;
        all->name = L"/";
    }

    for (UInt32 i = 0; i < numItems; i++) {
        NCOM::CPropVariant prop;
        archive->GetProperty(i, kpidIsDir, &prop);
        if (prop.boolVal) {
            prop.Clear();
            archive->GetProperty(i, kpidPath, &prop);
            if (prop.vt == VT_BSTR) {
                Print(prop.bstrVal);
                std::wstring name(prop.bstrVal, SysStringLen(prop.bstrVal));
                jfi_resolve_dir(*all, (name));
            } else if (prop.vt != VT_EMPTY) {
                Print("ERROR!");
                continue;
            }
        } else {
            prop.Clear();
            archive->GetProperty(i, kpidSize, &prop);
            char s[32];
            ConvertPropVariantToShortString(prop, s);
            Print(s);
            Print("  ");
            auto size = prop.uhVal.QuadPart;

            prop.Clear();
            archive->GetProperty(i, kpidPath, &prop);
            if (prop.vt == VT_BSTR) {
                Print(prop.bstrVal);
                std::wstring name(prop.bstrVal, SysStringLen(prop.bstrVal));
                jfi_resolve_file(*all, (name), size);
            } else if (prop.vt != VT_EMPTY) {
                Print("ERROR!");
                continue;
            }
        }
        
        PrintNewLine();
    }

    return j7zpp_res::ok;
}

j7zpp_res j7zpp_extract_some(const std::wstring& archiveName_, 
                             const std::vector<std::wstring>& files_to_extract, 
                             const std::wstring& out_dir,
                             ArchiveExtractCallback cb)
{
    if (files_to_extract.empty()) {
        return j7zpp_res::ok;
    }

#ifdef MY7ZPPWITH7ZRASTATIC_EXPORTS
    CMyComPtr<IInArchive> archive;
    if (CreateObject(&CLSID_Format, &IID_IInArchive, (void**)&archive) != S_OK) {
        PrintError("Can not get class object");
        return j7zpp_res::get_class_obj_fail;
    }

#else
    NDLL::CLibrary lib;
    if (!lib.Load(NDLL::GetModuleDirPrefix() + FTEXT(kDllName))) {
        PrintError("Can not load 7-zip library");
        return j7zpp_res::load_7zra_dll_fail;
    }

    Func_CreateObject createObjectFunc = (Func_CreateObject)lib.GetProc("CreateObject");
    if (!createObjectFunc) {
        PrintError("Can not get CreateObject");
        return j7zpp_res::create_obj_fail;
    }

    CMyComPtr<IInArchive> archive;
    if (createObjectFunc(&CLSID_Format, &IID_IInArchive, (void**)&archive) != S_OK) {
        PrintError("Can not get class object");
        return j7zpp_res::get_class_obj_fail;
    }

#endif


    CInFileStream* fileSpec = new CInFileStream;
    CMyComPtr<IInStream> file = fileSpec;
    FString archiveName = (archiveName_.c_str());
    if (!fileSpec->Open(archiveName)) {
        PrintError("Can not open archive file", archiveName);
        return j7zpp_res::open_file_fail;
    }

    {
        CArchiveOpenCallback* openCallbackSpec = new CArchiveOpenCallback;
        CMyComPtr<IArchiveOpenCallback> openCallback(openCallbackSpec);
        openCallbackSpec->PasswordIsDefined = false;
        // openCallbackSpec->PasswordIsDefined = true;
        // openCallbackSpec->Password = L"1";

        const UInt64 scanSize = 1 << 23;
        if (archive->Open(file, &scanSize, openCallback) != S_OK) {
            PrintError("Can not open file as archive", archiveName);
            return j7zpp_res::open_file_fail;
        }
    }

    //UInt32* indices = new UInt32[files_to_extract.size()];
    //size_t index_of_indices = 0;

    std::vector<UInt32> indices;

    // find files' indeces
    {
        // List command
        UInt32 numItems = 0;
        archive->GetNumberOfItems(&numItems);
        for (UInt32 i = 0; i < numItems; i++) {
            // Get name of file
            NCOM::CPropVariant prop;
            archive->GetProperty(i, kpidPath, &prop);
            if (prop.vt == VT_BSTR) {
                Print(prop.bstrVal);
                std::wstring name(prop.bstrVal, SysStringLen(prop.bstrVal));
                //auto mbcsname = jlib::win32::utf16_to_mbcs(name);
                for (const auto& name_ : files_to_extract) {
                    if (name_ == name) {
                        if (indices.size() >= files_to_extract.size()) {
                            return j7zpp_res::extract_find_file_fail;
                        }
                        indices.push_back(i);
                        break;
                    }
                }
            } else if (prop.vt != VT_EMPTY)
                Print("ERROR!");

            PrintNewLine();

            if (indices.size() == files_to_extract.size()) {
                break;
            }
        }
    }

    if (indices.size() != files_to_extract.size()) {
        return j7zpp_res::extract_find_file_fail;
    }

    // Extract command
    CArchiveExtractCallback* extractCallbackSpec = new CArchiveExtractCallback;
    CMyComPtr<IArchiveExtractCallback> extractCallback(extractCallbackSpec);
    extractCallbackSpec->Init(archive, (out_dir.c_str())); // second parameter is output folder path
    extractCallbackSpec->PasswordIsDefined = false;
    extractCallbackSpec->setArchiveUpdateProgressCallback(cb);
    // extractCallbackSpec->PasswordIsDefined = true;
    // extractCallbackSpec->Password = "1";

    /*
    const wchar_t *names[] =
    {
      L"mt",
      L"mtf"
    };
    const unsigned kNumProps = sizeof(names) / sizeof(names[0]);
    NCOM::CPropVariant values[kNumProps] =
    {
      (UInt32)1,
      false
    };
    CMyComPtr<ISetProperties> setProperties;
    archive->QueryInterface(IID_ISetProperties, (void **)&setProperties);
    if (setProperties)
      setProperties->SetProperties(names, values, kNumProps);
    */

    HRESULT result = archive->Extract(indices.data(), indices.size(), false, extractCallback);

    if (result != S_OK) {
        PrintError("Extract Error");
        return j7zpp_res::extract_fail;
    }

    return j7zpp_res::ok;
}
