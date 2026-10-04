#pragma once
namespace aether::storage {
int moduleCount();
const char* moduleName(int index);
const char* moduleClass(int index);
bool writeRegistry();
}
