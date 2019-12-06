#include "stdafx.h"
#include "IntLogger.h"

DWORD WINAPI TestThreadProc(LPVOID lpParameter)
{
	if (NULL == lpParameter)
	{
		return -1;
	}

	IntLogger* pIntLogger = (IntLogger*)lpParameter;

	Sleep(500);
	for (int i = 0; i < 10; i++)
	{
		Sleep(1000);
		pIntLogger->Log(GetCurrentTime(), i);
	}
	return 0;
}

int main()
{
	DWORD dwThreadPool[5];
	HANDLE hThreadPool[5];
	string filename = "test.csv";
	int window = 20000;
	IntLogger logger = IntLogger(filename, window);

	for (int i = 0; i < 5; i++)
	{
		hThreadPool[i] = CreateThread(
			NULL,
			0,
			TestThreadProc,
			&logger,
			0,
			&dwThreadPool[i]);
	}

	Sleep(20000);

	return 0;
}