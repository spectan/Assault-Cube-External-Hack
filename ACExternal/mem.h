#pragma once
#include "stdafx.h"

namespace mem {

	void PatchEx(BYTE* dst, BYTE* src, unsigned int size, HANDLE hProcess);
	void NopEX(BYTE* dst, unsigned int size, HANDLE hProcess);
}
