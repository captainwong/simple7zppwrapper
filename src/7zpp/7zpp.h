#pragma once


#if defined(MY7ZPP_EXPORTS) || defined(MY7ZPPWITH7ZRASTATIC_EXPORTS)
#define MY7ZPP_FUNC _declspec(dllexport)
#else
#define MY7ZPP_FUNC _declspec(dllimport)
#endif

#include <string>
#include <vector>
#include "callback.h"


MY7ZPP_FUNC void j7zpp_debug(bool debug = false);

enum class j7zpp_res {
	ok, 
	load_7zra_dll_fail,
	create_obj_fail,
	open_file_fail,
	create_file_fail,
	get_class_obj_fail,
	update_fail,
	finalize_fail,
	extract_fail,
	extract_find_file_fail,
};


// mbcs
MY7ZPP_FUNC int j7zpp_compress_folder(const std::wstring& archive, const std::wstring& folder);
// mbcs
MY7ZPP_FUNC j7zpp_res j7zpp_compress_files(const std::wstring& archive, 
										   const std::vector<std::wstring>& files, 
										   const std::wstring& dirPrefix4NotShowInArchive = {},
										   ArchiveUpdateProgressCallback cb = nullptr);

struct jfile_info;
struct jfile_info {
	// mbcs
	std::wstring name{}; 
	uint64_t size{};
	bool isdir = false;
	std::vector<jfile_info*> subitems{};
};

// mbcs. call release_jfile_info to release jfinfo if succeeded
MY7ZPP_FUNC j7zpp_res j7zpp_list_files(const std::wstring& archive, jfile_info** jfinfo);

MY7ZPP_FUNC void j7zpp_release_jfile_info(jfile_info* jfi);

// mbcs
// use j7zpp_list_files to get a file list, then pickup some file name and pass them to files_to_extract
// make sure out_dir is exists before call me or fail
// doesn't matter if out_dir/files exists or not, will overwrite them
MY7ZPP_FUNC j7zpp_res j7zpp_extract_some(const std::wstring& archive, 
										 const std::vector<std::wstring>& files_to_extract, 
										 const std::wstring& out_dir = {},
										 ArchiveExtractCallback cb = nullptr);
