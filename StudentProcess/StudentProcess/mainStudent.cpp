#include <iostream>
#include <cstdlib> 
#include <time.h> 
#include <Windows.h>
#include <tchar.h>
#include <string>

using namespace std;

#define SIZE 9000
TCHAR name[] = TEXT("BOARD");
HANDLE accessFile;
HANDLE timeMUT;
HANDLE mutBoard;
HANDLE mapping;


int main(int argc, char* argv[])
{
	cout << "i`m student" << endl;


	mapping = OpenFileMapping(FILE_MAP_ALL_ACCESS, true, name);
	if (mapping == NULL) { return -1; }

	LPVOID pBuf = (void*)MapViewOfFile(mapping, FILE_MAP_ALL_ACCESS, 0, 0, SIZE);
	if (pBuf == nullptr) { return -1; }


	char newData[] = "student idea\n";
	char* charPointer = NULL;




	mutBoard = CreateMutexW(NULL, false, (LPCWSTR)"boardMUT");
	timeMUT = CreateMutexW(NULL, false, (LPCWSTR)"timeMUT");
	
	char* access = new char[2];
	DWORD rd;


	while (true)
	{
		WaitForSingleObject(timeMUT, INFINITE);
		accessFile = CreateFileA("..\\..\\access.txt", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		ReadFile(accessFile, access, 2, &rd, NULL);
		CloseHandle(accessFile);
		ReleaseMutex(timeMUT);
		if (access[0] == '0')
		{
			cout << "access denied" << endl;
			break;
		}
		else
		{
			WaitForSingleObject(mutBoard, INFINITE);
			Sleep(2000);

			snprintf((char*)pBuf, sizeof(newData), newData);
			charPointer = (char*)pBuf;
			charPointer += sizeof(newData);
			pBuf = (void*)charPointer;

			cout << "write in file" << GetCurrentProcessId() << endl;
			ReleaseMutex(mutBoard);
		}
	}

	getchar();

	UnmapViewOfFile(mapping);
	CloseHandle(mutBoard);
	return 0;
}