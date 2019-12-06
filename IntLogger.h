#pragma once
#include "stdafx.h"
#include "Windows.h"
#include <string>
#include <vector>

using namespace std;

struct LogNode
{
	int time;
	int value;

	LogNode(int ttime, int tvalue)
	{
		time = ttime;
		value = tvalue;
	};
};

class IntLogger {
public:
	IntLogger(string& filename, int window);
	~IntLogger();
	void Log(int time, int value);
	DWORD Lock();
	void Unlock();
	void SaveLog2File();
	int GetWindowTime();
	string GetFileName();
	HANDLE GetWaitEvent();

private:
	string m_filename;
	int m_window;
	vector<LogNode> mVecLog;
	HANDLE m_hLogWriteThread;
	HANDLE m_hEvent;
	HANDLE m_hMutex;
	static DWORD WINAPI logWriteThreadProc(LPVOID lpParameter);
};