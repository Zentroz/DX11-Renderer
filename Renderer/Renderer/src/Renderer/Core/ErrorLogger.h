#pragma once

#include<Windows.h>
#include<iostream>
#include<d3d11.h>
#include<comdef.h>
#include<string>

inline bool CheckDXHRException(HRESULT hr, const char* file, int line) {
    if (FAILED(hr)) {
        _com_error err(hr);
        const TCHAR* tChar = err.ErrorMessage();
        std::wstring wStr = std::wstring(tChar);
        std::string str = std::string(wStr.begin(), wStr.end());

        printf("[ERR | DX11]: (File : %s, Line : %d) - %s \n", file, line, str.c_str());

        return true;
    }

    return false;
}

#define FAILEDLOG( hr ) CheckDXHRException(hr, __FILE__, __LINE__)

