#pragma once

#include <stdint.h>

typedef void(*ArchiveUpdateProgressCallback)(uint64_t completed, uint64_t total);
typedef ArchiveUpdateProgressCallback ArchiveExtractCallback;

