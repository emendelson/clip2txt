// Clip2Txt.cpp 

#include "stdafx.h"
#include <Windows.h>
#include <shellapi.h>
#include <iostream>
#include <fstream>
#include <codecvt> // for wstring_convert
#include <locale>  // for codecvt_byname
#include <sstream>
using namespace std;

// helper gets path to this application
string ExePath() {
	char buffer[MAX_PATH];
	GetModuleFileNameA( NULL, buffer, MAX_PATH );
	string::size_type pos = string( buffer ).find_last_of( "\\/" );
	return string( buffer ).substr( 0, pos);
	//return std::string( buffer ).substr( 0, pos);
}

// set variable for command-line arguments
char **argv = NULL;
// helper to get command-line arguments
int ParseCommandLine() {
	int	argc, BuffSize, i;
	WCHAR	*wcCommandLine;
	LPWSTR	*argw;

	wcCommandLine = GetCommandLineW();
	argw = CommandLineToArgvW( wcCommandLine, &argc);

	argv = (char **)GlobalAlloc( LPTR, argc + 1);

	for( i=0; i < argc; i++) {
		BuffSize = WideCharToMultiByte( CP_ACP, WC_COMPOSITECHECK, argw[i], -1, NULL, 0, NULL, NULL );
		argv[i] = (char *)GlobalAlloc( LPTR, BuffSize );
		WideCharToMultiByte( CP_ACP, WC_COMPOSITECHECK, argw[i], BuffSize * sizeof( WCHAR ),argv[i], BuffSize, NULL, NULL );
	}
	return argc;
}

int CALLBACK WinMain(
	_In_ HINSTANCE hInstance,
	_In_ HINSTANCE hPrevInstance,
	_In_ LPSTR	lpCmdLine,
	_In_ int	nCmdShow)

{
	// for logging in case of error
	int writelog = 0;
	string logtext = ""; 

	// create output filename
	string filename = ExePath() + "\\#clip.txt"; 

	//  get default codepage from Windows, typically 1252
	int iCP=GetACP();
	string sCP;
	ostringstream convert;
	convert << iCP;
	sCP = convert.str();

	// construct string to use for conversion routines (e.g. ".1252")
	string sDefaultCP = "."+sCP;
	string sOutputCP = "."+sCP;

	// read command line for alternate codepage and/or filename
	int i, argc; 
	argc = ParseCommandLine( );

	if (argc > 1) {
		bool bFilenameSet = false;
		bool bCodepageSet = false;
		int cpint = -1;

		for ( i = 1; i < argc && i < 3; i++ ) {
			std::string argstr = argv[i];

			//if string has only digits, use as codepage;
			for (size_t n = 0; n <argstr.length(); n++) {
				if (!isdigit( argstr[ n ]) ) {
					if (!bFilenameSet) {
						filename = argv[i];
						bFilenameSet = true;
					}
				} else {
					// convert codepage string to integer
					if (!bCodepageSet) {
						std::stringstream argss(argv[i]);
						if( (argss >> cpint) || !argss.eof()) {
							argstr = argv[i];
							logtext = logtext + "Requested codepage (if any): " + argstr + "\n";
							//cout is visible only if piped to "more" or similar
							cout << "Requested codepage (if any): " << argstr << endl;
							// check if codepage is valid; if so, use it
							if (IsValidCodePage(cpint)) {
								sCP = argstr;
								sOutputCP = "."+argstr;
							}
							bCodepageSet = true;
						}
					}
				}
			}
		}
	}

	//cout is visible only if piped to "more" or similar
	cout << "Codepage: " + sCP << endl;

	// get clipboard text
	string cliptext = "";

	if (OpenClipboard(NULL)) {
		if(IsClipboardFormatAvailable(CF_TEXT)) {
			HGLOBAL hglb = GetClipboardData(CF_TEXT);
			if (hglb != NULL) {
				LPSTR lptstr = (LPSTR)GlobalLock(hglb);
				if (lptstr != NULL) {
					// read the contents of lptstr
					cliptext = (char*)hglb;
					// release the lock 
					GlobalUnlock(hglb);
				}
			}
		}
		CloseClipboard();
	}
	// create conversion routines
	typedef std::codecvt_byname<wchar_t,char,std::mbstate_t> codecvt;
	std::wstring_convert<codecvt> cp1252(new codecvt(sDefaultCP));
	std::wstring_convert<codecvt> outpage(new codecvt(sOutputCP));
	ofstream OutStream;  // open an output stream
	OutStream.open(filename, std::ios_base::binary | ios::out | ios::trunc);
	// make sure file is successfully opened
	if(!OutStream) {
		writelog = 1;
		logtext = logtext + "Error opening file " + filename + " for writing.\n";
		//return 1;
	} else {
		// convert to DOS/Win codepage number in "outpage"
		OutStream << outpage.to_bytes(cp1252.from_bytes(cliptext)).c_str();
		OutStream.close(); // close output stream
		if (writelog == 1) {
			logtext = logtext + "Output file: " + filename + "\n";
		}
	}

	if (writelog == 1) {
		logtext = logtext + "Codepage used: " + sCP + "\n";
		string LogFile = ExePath() + "\\#log.txt"; 
		ofstream LogStream;
		LogStream.open(LogFile, ios::out | ios::trunc);
		if(!LogStream) {
			// stdout only visible if output is piped to "more" or elsewhere
			cout << "Error opening file " << LogFile << " for writing.\n";
			return 1;
		}
		LogStream << logtext;
		LogStream.close(); // close output stream
	}
	return 0;
} 
