#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <cstdlib> 
#include <time.h> 
#include <Windows.h>
#include <tchar.h>
#include <string>
#include <cstring>

using namespace std;

#define SIZE 9000
TCHAR name[] = TEXT("BOARD");
HANDLE accessFile;
HANDLE timeMUT;
HANDLE mutBoard;
HANDLE mapping;



int main(int argc, char* argv[])
{
	//number of student
	int studentNum = atoi(argv[1]);
	cout << "i`m student #" << studentNum << ".\n";

	//info to write in file
	char Data[30+sizeof(char)];
	std::sprintf(Data, "student # %d. Idea# ", studentNum);

	timeMUT = CreateMutexW(NULL, false, (LPCWSTR)"timeMUT");
	mutBoard = CreateMutexW(NULL, false, (LPCWSTR)"boardMUT");

	char* charPointer = NULL;
	char* access = new char[2];
	DWORD rd;
	int numOfIdea = 0;


	while (true)
	{
		WaitForSingleObject(timeMUT, INFINITE);
		accessFile = CreateFileA("..\\..\\access.txt", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		ReadFile(accessFile, access, 2, &rd, NULL);
		CloseHandle(accessFile);
		ReleaseMutex(timeMUT);
		if (access[0] == '0')
		{
			cout << "Time's over." << endl;
			break;
		}
		else
		{
			WaitForSingleObject(mutBoard, INFINITE);

			//open mapping
			mapping = OpenFileMapping(FILE_MAP_ALL_ACCESS, true, name);
			if (mapping == NULL) { return -1; }
			LPVOID pBuf = (void*)MapViewOfFile(mapping, FILE_MAP_ALL_ACCESS, 0, 0, SIZE);
			if (pBuf == nullptr) { return -1; }

			//set pointer in the end of file
			char* newArr = (char*)pBuf;
			int lastn = 0;
			for (int i = 0; i < SIZE; i++) {
				if (newArr[i] == '\n') {
					lastn = i;
				}
			}
			charPointer = (char*)pBuf;
			charPointer += lastn + 1;
			pBuf = (void*)charPointer;

			//add extra info to write in file
			char newData[sizeof(Data)];
			strcpy(newData, Data);
			char ideas[3 + sizeof(char)];
			numOfIdea++;
			std::sprintf(ideas, "%d\n", numOfIdea);
			strcat(newData, ideas);

			//write to file
			snprintf((char*)pBuf, sizeof(newData), newData);
			cout << newData;

			Sleep(20000);
			ReleaseMutex(mutBoard);
		}
	}
	//getchar();
	UnmapViewOfFile(mapping);
	CloseHandle(mutBoard);

	return 0;
}