**clip2txt : a Windows utility that saves the clipboard contents to a text file**

Clip2txt.exe is a Windows command-line utility that writes the contents of the Windows clipboard to a text file, optionally in a user-specified codepage for use in MS-DOS applications or other applications that rely on codepages for rendering non-ASCII characters.

To run the program, simply run Clip2txt (or call it from another application). It will write the clipboard contents to a file named #clip.txt in the same folder with Clip2txt.exe. The file will be a text file using the default Windows codepage (typically 1252).

Command-line parameters: You may add a code page number (e.g. 437, 850, 1252, etc.) or a filename (e.g. clipboard.txt) or a full pathname (e.g. C:\Users\Roscoe\Desktop\output.txt) or both (in any order) to the command line.

If you specify a codepage, the program will try to convert the clipboard text to the specified codepage. If the specified codepage is invalid under your copy of Windows, the default codepage will be used instead. If the specified file cannot be created, the program will try to create a log file named #log.txt in the same folder with the application.

This is my first attempt to write a program in C, and the result is an amateurish mix of conventions that will make any competent programmer burst into contemptuous laughter. But the program at least seems to work.

If you want to modify this project for yourself, use Microsoft Visual Studio 2019 or later; create a Win32 project named Clip2txt; replace the contents of Clip2txt.cpp with the code available on this sie; then, in the Properties panel, change the Linker->System->Subsystem from a Windows app to a console app. Since this procedure may be tricky or unreliable, it's probably best to start with the complete VS2019+ build environment that I have posted here.

If you are expert and generous enough to suggest changes in the code, please post a pull request on this page.
