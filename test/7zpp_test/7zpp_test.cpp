#include "../../src/7zpp/7zpp.h"


#define TEST_7ZPP_WITH_7ZRA_STATIC

#ifdef TEST_7ZPP_WITH_7ZRA_STATIC
#define DLL_NAME "7zpps"
#else
#define DLL_NAME "7zpp"
#endif

#ifdef _WIN64
#pragma comment(lib, DLL_NAME "_x64.lib")
#else
#pragma comment(lib, DLL_NAME ".lib")
#endif

void print_jfinfo(const jfile_info& jfi, int space = 0)
{
	if (jfi.isdir) {
		printf("%*c%s\n", space, ' ', jfi.name.c_str());
		for (const auto& sub : jfi.subitems) {
			print_jfinfo(*sub, space + 2);
		}
	} else {
		printf("%*c%s %llu\n", space, ' ', jfi.name.c_str(), jfi.size);
	}
}

void test_compress()
{
	std::vector<std::string> files = {
		R"(C:\Users\Jack\Documents\history.db3)",
		R"(C:\Users\Jack\Documents\VersionNo.ini)",
		R"(C:\Users\Jack\Documents\wx_server_2019-12-25_003732.sql)",

	};
	auto res = j7zpp_compress_files(R"(C:\Users\Jack\Documents\out.7z)", files, R"(C:\Users\Jack\Documents)", [](uint64_t pos, uint64_t total) {
		printf("\r%llu/%llu", pos, total);
	});

	switch (res) {
	case j7zpp_res::ok:
		break;
	case j7zpp_res::load_7zra_dll_fail:
		break;
	case j7zpp_res::create_obj_fail:
		break;
	case j7zpp_res::open_file_fail:
		break;
	case j7zpp_res::create_file_fail:
		break;
	case j7zpp_res::get_class_obj_fail:
		break;
	case j7zpp_res::update_fail:
		break;
	case j7zpp_res::finalize_fail:
		break;
	default:
		break;
	}
}

void test_list()
{
	jfile_info* files;
	auto res = j7zpp_list_files(R"(C:\Users\Jack\Documents\out.7z)", &files);

	printf("\ndone\n");

	switch (res) {
	case j7zpp_res::ok:
		{
			print_jfinfo(*files);
		}
		break;
	case j7zpp_res::load_7zra_dll_fail:
		break;
	case j7zpp_res::create_obj_fail:
		break;
	case j7zpp_res::open_file_fail:
		break;
	case j7zpp_res::create_file_fail:
		break;
	case j7zpp_res::get_class_obj_fail:
		break;
	case j7zpp_res::update_fail:
		break;
	default:
		break;
	}

	j7zpp_release_jfile_info(files);
}


void test_extract_some()
{
	std::vector<std::string> files = {
		R"(history.db3)",
		R"(VersionNo.ini)",
		R"(wx_server_2019-12-25_003732.sql)",

	};

	auto res = j7zpp_extract_some(R"(C:\Users\Jack\Documents\out.7z)",
								  files,
								  R"(C:\Users\Jack\Documents\test_extract_some_out)",
								  [](uint64_t pos, uint64_t total) {
		printf("\r%llu/%llu", pos, total);
	});

	switch (res) {
	case j7zpp_res::ok:
		break;
	case j7zpp_res::load_7zra_dll_fail:
		break;
	case j7zpp_res::create_obj_fail:
		break;
	case j7zpp_res::open_file_fail:
		break;
	case j7zpp_res::create_file_fail:
		break;
	case j7zpp_res::get_class_obj_fail:
		break;
	case j7zpp_res::update_fail:
		break;
	case j7zpp_res::finalize_fail:
		break;
	case j7zpp_res::extract_fail:
		break;
	case j7zpp_res::extract_find_file_fail:
		break;
	default:
		break;
	}
}


int main()
{
	//j7zpp_debug(true);
	//test_compress();	
	test_list();
	//test_extract_some();

}
