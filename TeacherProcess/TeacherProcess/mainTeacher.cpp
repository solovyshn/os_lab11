#define _CRT_SECURE_NO_WARNINGS
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
	//�����, ��� ����� �� ����� ���� ���������
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

	cout << "Time's over.\n\nWait for loading ideas...\n\n";

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


	//�����������
	cout << "\nWait for the results of voting...\n\n";
	//����� ��� ����������� ���������� � ����
    Sleep(1 * 60 * 1500);

	//looking for the end
	LPVOID pBuf2 = (void*)MapViewOfFile(mapping2, FILE_MAP_READ, 0, 0, SIZE);
	if (pBuf2 == nullptr) { return -1; }
	char* newArr2 = (char*)pBuf2;
	int* nums = new int[k+1];
	for (int i = 0; i < k + 1; i++) {
		nums[i] = 0;
	}
	
	int lastn2 = 0;
	for (int i = 0; i < SIZE; i++) {
		if (newArr2[i] == '\n') {
			lastn2 = i;
		}
	}

	for (int i = 0; i < lastn2; i++) {
		for (int j = 0; j <= k; j++) {
			int l = j + 1;
			if (newArr2[i] == to_string(l)[0]) {
				nums[l]++;
			}
		}
	}
	//for (int i = 0; i < lastn2; i++) {
	//	cout << "Idea: "<< i << " Votes: " << nums[i] << endl;
	//}

	//selecting top 3

	//find max quontity of votes for one idea
	int maxVotes = 0;
	for (int i = 0; i < lastn2; i++) {
		if (maxVotes < nums[i]) maxVotes = nums[i];
	}

	//find indexes of top-3 ideas
	int topIndexes[3] = {0,0,0};
	int currentPosition = 0;
	while (currentPosition <= 3 && maxVotes>0) {
		for (int i = 0; i < lastn2; i++) {
			if (maxVotes == nums[i]) {
				topIndexes[currentPosition] = i;
				currentPosition++;
			}
		}
		maxVotes--;
	}

	cout << "Top-3:\n" << endl;
	
		//looking for the end
	 pBuf = (void*)MapViewOfFile(mapping, FILE_MAP_READ, 0, 0, SIZE);
	if (pBuf == nullptr) { return -1; }
	 newArr = (char*)pBuf;
	lastn = 0;
	for (int i = 0; i < SIZE; i++) {
		if (newArr[i] == '\n') {
			lastn = i;
		}
	}


	//reading file, write to console
	newArr = (char*)pBuf;
	string result = "\nTop-3:\n";
	for (int p = 0; p < 3; p++) {
		k = 1;
		bool boolka = false;
		for (int i = 0; i < lastn + 1; i++) {
			if (boolka == false) {
				if (i > 0 && newArr[i - 1] == '\n') {
					k++;
					if (k == topIndexes[p]) {
						cout << k << ". ";
						cout << newArr[i];
						result += to_string(k)+ ". ";
						result += newArr[i];
						boolka = true;
					}
				}

			}
			else if (boolka) {
				cout << newArr[i];
				result += newArr[i];
				if (newArr[i + 1] == '\n') {
					cout << endl;
					result += "\n";
					boolka = false; 
				}
			}
		}
	}

	//cout << result;
	//open mapping
	char* charPointerV = NULL;
	HANDLE mappingV = OpenFileMapping(FILE_MAP_ALL_ACCESS, true, name);
	if (mappingV == NULL) { return -1; }
	LPVOID pBufV = (void*)MapViewOfFile(mappingV, FILE_MAP_ALL_ACCESS, 0, 0, SIZE);
	if (pBufV == nullptr) { return -1; }

	//set pointer in the end of file
	newArr = (char*)pBufV;
	lastn = 0;
	for (int i = 0; i < SIZE; i++) {
		if (newArr[i] == '\n') {
			lastn = i;
		}
	}
	charPointerV = (char*)pBufV;
	charPointerV += lastn + 1;
	pBufV = (void*)charPointerV;


	//write to file
	snprintf((char*)pBufV, sizeof(char)* result.length(), result.c_str());
	
	getchar();
	getchar();
	getchar();

	UnmapViewOfFile(mapping);
	UnmapViewOfFile(mapping2);
	CloseHandle(boardFile);
	CloseHandle(voteFile);

	return 0;
}
