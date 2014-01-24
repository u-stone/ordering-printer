#pragma once
#define APPNAME   _T("HongHong")
#define FILENAME _T("ui.ini")

CString GetCurDir(){
	TCHAR path[_MAX_PATH] = {0};
	GetModuleFileName(NULL, path, _MAX_PATH);
	CString strPath = path;
	strPath = strPath.Left(strPath.ReverseFind(_T('\\')));
	return strPath;
}

CString GetIniPath(){
	return GetCurDir() + _T("\\") + FILENAME;
}

CString uiGetInfo(CString strItem){
	TCHAR strValue[_MAX_PATH];
	GetPrivateProfileString(APPNAME, strItem, _T(""), strValue, _MAX_PATH, GetIniPath());
	return strValue;
}

void uiSetInfo(CString strItem, CString value){
	WritePrivateProfileString(APPNAME, strItem, value, GetIniPath());
}

#undef FILENAME