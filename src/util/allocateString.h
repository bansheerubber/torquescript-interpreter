#pragma once

#define ALLOCATE_STRING(source, destination) new((void*)&destination) string(source, 0, source.length());
