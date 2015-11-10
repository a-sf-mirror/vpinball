#ifndef _CVEDIT_
#define _CVEDIT_

enum WordType {eUnknown, eClass, eSub, eFunction, ePropGet, ePropLet, ePropSet};

class UserData
{
private:
public:
	string UniqueKey;
	int LineNum; //Line No. Declaration
	string KeyName; //Name
	WordType eTyping;
	string UniqueParent;
	vector<string> Children;
	string Description;//Brief Description
	string Comment;

public:
	UserData();
	UserData(const int LineNo, const string &Desc, const string &Name, const WordType &TypeIn);
	bool FuncCompareUD (const UserData &first, const UserData &second);
	string lowerCase(string input);
	bool FindOrInsertStringIntoAutolist(vector<string>* ListIn, const string &strIn);
	int FindOrInsertUD( vector<UserData>* ListIn, UserData& udIn);
	int FindUD(vector<UserData>* ListIn, const string &strIn,vector<UserData>::iterator& UDiterOut);
	int FindUDbyKey(vector<UserData>* ListIn, const string &strIn,vector<UserData>::iterator& UDiterOut, int &PosOut);
	int UDKeyIndex(vector<UserData>* ListIn, const string &strIn);
	~UserData();
};

// CodeViewer Preferences 
class CVPrefrence
{
public:
	const char* szControlName;
	LOGFONT LogFont;
	int PointSize;
	COLORREF rgb ;
	bool Highlight;
	const char *szRegName;
	int SciKeywordID;
	int IDC_ChkBox_code;
	int IDC_ColorBut_code;
	int IDC_Font_code;

	CVPrefrence();
	CVPrefrence* FillCVPreference( \
		const char* szCtrlNameIn,const COLORREF &crTextColor, \
		const bool &bDisplay, const char* szRegistryName, \
		const int &szScintillaKeyword, const int &IDC_ChkBox, \
		const int &IDC_ColorBut, const int &IDC_Font);
	void GetPrefsFromReg();
	void SetPrefsToReg();
	void SetCheckBox(const HWND &hwndDlg);
	void ReadCheckBox(const HWND &hwndDlg);
	void SetDefaultFont(const HWND &hwndDlg);
	int GetHeightFromPointSize(const HWND &hwndDlg);
	void ApplyPreferences(const HWND &hwndScin,const CVPrefrence* DefaultPref);
	~CVPrefrence();
};

#endif //_CVEDIT_