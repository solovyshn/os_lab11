#include <iostream>
#include <cstdlib> 
#include <time.h> 
#include <Windows.h>
#include <tchar.h>
#include <string>

using namespace std;

#define SIZE 9000
TCHAR name[] = TEXT("BOARD");

class Student
{
public:
	STARTUPINFOA si;
	PROCESS_INFORMATION pi;
};

HANDLE mapping;
HANDLE boardFile;
HANDLE accessFile;
HANDLE timeMUT;
HANDLE accessToBoardMUT;
Student* students = NULL;


int main(int argc, char* argv[])
{
	cout << "Hello, i'm teacher" << endl;

	boardFile = CreateFileA("..\\..\\board.txt", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	mapping = CreateFileMapping(boardFile, NULL, PAGE_READWRITE, 0, SIZE, name);
	if (mapping == nullptr) { return -1; }


	cout << "Enter count of students: " << endl;
	int countOfStudents;
	cin >> countOfStudents;

	students = new Student[countOfStudents];
	for (int i = 0; i < countOfStudents; ++i) {
		ZeroMemory(&(students[i].si), sizeof(STARTUPINFO));
		students[i].si.cb = sizeof(STARTUPINFO);
		ZeroMemory(&(students[i].pi), sizeof(PROCESS_INFORMATION));
	}

	string studentFilePath = "..\\..\\StudentProcess\\Debug\\StudentProcess.exe";
	LPSTR studentFilePathLPSTR = const_cast<char*>((studentFilePath).c_str());

	accessFile = CreateFileA("..\\..\\access.txt", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	DWORD wr;
	char* access = new char[2];
	access[0] = '1';
	access[1] = '\0';
	WriteFile(accessFile, access, 2, &wr, NULL);
	CloseHandle(accessFile);



	for (int i = 0; i < countOfStudents; i++)
	{
		CreateProcessA(NULL, studentFilePathLPSTR, NULL, NULL, true, CREATE_NEW_CONSOLE, NULL, NULL, &(students[i].si), &(students[i].pi));
	}


	Sleep(1 * 60 * 200);//3 minutes wait

	timeMUT = CreateMutexW(NULL, false, (LPCWSTR)"timeMUT");
	WaitForSingleObject(timeMUT, INFINITE);
	accessFile = CreateFileA("..\\..\\access.txt", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	access[0] = '0';
	access[1] = '\0';
	WriteFile(accessFile, access, 2, &wr, NULL);
	CloseHandle(accessFile);
	ReleaseMutex(timeMUT);




	getchar();
	getchar();
	getchar();//voting starts here

	UnmapViewOfFile(mapping);
	CloseHandle(boardFile);

	return 0;
}
