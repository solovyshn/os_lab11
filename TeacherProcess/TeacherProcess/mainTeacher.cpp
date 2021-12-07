#include <iostream>
#include <cstdlib> 
#include <time.h> 
#include <Windows.h>
#include <tchar.h>
#include <string>

using namespace std;

#define SIZE 9000
TCHAR name[] = TEXT("BOARD");
TCHAR name2[] = TEXT("VOTING");

class Student
{
public:
	STARTUPINFOA si;
	PROCESS_INFORMATION pi;
};

HANDLE mapping;
HANDLE mapping2;
HANDLE boardFile;
HANDLE voteFile;
HANDLE accessFile;
HANDLE timeMUT;
HANDLE accessToBoardMUT;
Student* students = NULL;


int main(int argc, char* argv[])
{
	cout << "Hello, i'm teacher" << endl;

	boardFile = CreateFileA("..\\..\\board.txt", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	voteFile = CreateFileA("..\\..\\voting.txt", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	mapping = CreateFileMapping(boardFile, NULL, PAGE_READWRITE, 0, SIZE, name);
	if (mapping == nullptr) { return -1; }

	mapping2 = CreateFileMapping(voteFile, NULL, PAGE_READWRITE, 0, SIZE, name2);
	if (mapping2 == nullptr) { return -1; }

	cout << "Enter count of students: " << endl;
	int countOfStudents;
	cin >> countOfStudents;
	//думаю, тут можна ще меншу межу поставити
	if (countOfStudents > 99) {
		cout << "Too big number of students.\nMaximum number is 99\n";
		return -1;
	}

	students = new Student[countOfStudents];
	for (int i = 0; i < countOfStudents; ++i) {
		ZeroMemory(&(students[i].si), sizeof(STARTUPINFO));
		students[i].si.cb = sizeof(STARTUPINFO);
		ZeroMemory(&(students[i].pi), sizeof(PROCESS_INFORMATION));
	}

	string studentFilePath = "..\\..\\StudentProcess\\Debug\\StudentProcess.exe ";

	accessFile = CreateFileA("..\\..\\access.txt", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	DWORD wr;
	char* access = new char[2];
	access[0] = '1';
	access[1] = '\0';
	WriteFile(accessFile, access, 2, &wr, NULL);
	CloseHandle(accessFile);



	for (int i = 0; i < countOfStudents; i++)
	{
		string newStudentFilePath = studentFilePath;

		//number of student
		int k = i + 1;
		string r = std::to_string(k);
		newStudentFilePath.push_back(r[0]);
		if (k > 9) {
			newStudentFilePath.push_back(r[1]);
		}
		LPSTR studentFilePathLPSTR = const_cast<char*>((newStudentFilePath).c_str());

		//creating process
		CreateProcessA(NULL, studentFilePathLPSTR, NULL, NULL, true, CREATE_NEW_CONSOLE, NULL, NULL, &(students[i].si), &(students[i].pi));
	}


	Sleep(2 * 60 * 1000);//2 minutes wait

	timeMUT = CreateMutexW(NULL, false, (LPCWSTR)"timeMUT");
	WaitForSingleObject(timeMUT, INFINITE);
	accessFile = CreateFileA("..\\..\\access.txt", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	access[0] = '0';
	access[1] = '\0';
	WriteFile(accessFile, access, 2, &wr, NULL);
	CloseHandle(accessFile);
	ReleaseMutex(timeMUT);
	//kill processes
	for (int i = 0; i < countOfStudents; i++) {
		TerminateProcess(&(students[i].pi.hProcess), 0);
	}

	cout << "Time's over.\nWait for loading ideas...\n";

	Sleep(1 * 60 * 1500);//1 minute wait

	//looking for the end
	LPVOID pBuf = (void*)MapViewOfFile(mapping, FILE_MAP_READ, 0, 0, SIZE);
	if (pBuf == nullptr) { return -1; }
	char* newArr = (char*)pBuf;
	int lastn = 0;
	for (int i = 0; i < SIZE; i++) {
		if (newArr[i] == '\n') {
			lastn = i;
		}
	}

	//reading file, write to console
	newArr = (char*)pBuf;
	int k = 0;
	cout << "Ideas from the board:\n" << k + 1 << ".\t";
	k++;
	for (int i = 0; i < lastn + 1; i++) {
		if (i > 0 && newArr[i - 1] == '\n') {
			cout << k + 1 << ".\t";
			k++;
		}
		cout << newArr[i];
	}


	//голосування

	//чекаєм шоб голосування записалося у файл
	Sleep(10);

	//looking for the end
	LPVOID pBuf2 = (void*)MapViewOfFile(mapping2, FILE_MAP_READ, 0, 0, SIZE);
	if (pBuf2 == nullptr) { return -1; }
	char* newArr2 = (char*)pBuf2;
	int lastn2 = 0;
	for (int i = 0; i < SIZE; i++) {
		if (newArr2[i] == '\n') {
			lastn2 = i;
		}
	}

	//reading file, write to console
	newArr2 = (char*)pBuf2;

	for (int i = 0; i < lastn2 + 1; i++)
	{
		cout << newArr2[i] << "\t";
	}



	getchar();
	getchar();
	getchar();

	UnmapViewOfFile(mapping);
	UnmapViewOfFile(mapping2);
	CloseHandle(boardFile);
	CloseHandle(voteFile);

	return 0;
}
