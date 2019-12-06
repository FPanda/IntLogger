// IntLogger.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "IntLogger.h"
#include <sstream>
#include <algorithm>

bool LogNodeCompare(const LogNode& rhs, const LogNode& lhs)
{
	return rhs.time < lhs.time;
}

DWORD WINAPI IntLogger::logWriteThreadProc(LPVOID lpParameter)
{
	if (lpParameter == NULL)
	{
		printf("Didn't get the correct log information.");
		return -1;
	}

	IntLogger* pLogger = (IntLogger*)lpParameter;

	DWORD dwWaitResult;

	dwWaitResult = WaitForSingleObject(pLogger->m_hEvent, pLogger->GetWindowTime());

	switch (dwWaitResult)
	{
	case WAIT_TIMEOUT:
		pLogger->Lock();
		pLogger->SaveLog2File();
		pLogger->Unlock();
		break;
	default:
		printf("Unknow state of the mutex, final state is: %d\n", dwWaitResult);
		break;
	}

	return 0;
}

IntLogger::IntLogger(string& filename, int window)
{
	DWORD dwThreadID;

	m_filename = filename;
	m_window = window;

	if (NULL == m_hMutex)
	{
		m_hMutex = CreateMutex(
			NULL,              // default security attributes
			FALSE,             // initially not owned
			NULL);             // unnamed mutex

		if (m_hMutex == NULL)
		{
			printf("CreateMutex error: %d\n", GetLastError());
		}
	}
	if (NULL == m_hEvent)
	{
		m_hEvent = CreateEvent(
			NULL,
			FALSE,
			FALSE,
			NULL);

		if (m_hEvent == NULL)
		{
			printf("CreateEvent error: %d\n", GetLastError());
		}
	}
 
	m_hLogWriteThread = CreateThread(
		NULL,     
		0,     
		this->logWriteThreadProc,
		this,
		0,
		&dwThreadID);

	if (m_hLogWriteThread == NULL)
	{
		printf("CreateThread failed (%d)\n", GetLastError());
		return;
	}
}

int IntLogger::GetWindowTime()
{
	return m_window;
}

HANDLE IntLogger::GetWaitEvent()
{
	return m_hEvent;
}

string IntLogger::GetFileName()
{
	return m_filename;
}

IntLogger::~IntLogger()
{
	if (NULL != m_hMutex)
	{
		CloseHandle(m_hMutex);
		m_hMutex = NULL;
	}

	if (NULL != m_hEvent)
	{
		SetEvent(m_hEvent);
		CloseHandle(m_hEvent);
		m_hEvent = NULL;
	}

	if (NULL != m_hLogWriteThread)
	{
		CloseHandle(m_hLogWriteThread);
		m_hLogWriteThread = NULL;
	}
}

void IntLogger::SaveLog2File()
{
	if (mVecLog.size() != 0)
	{
		sort(mVecLog.begin(), mVecLog.end(), LogNodeCompare);
	}

	stringstream ss;
	ss << "Time" << ',' << "Value" << endl;

	for (unsigned int i = 0; i < mVecLog.size(); i++)
	{
		ss << mVecLog[i].time << ',' << mVecLog[i].value << endl;
	}

	FILE* fp;

	fopen_s(&fp, m_filename.c_str(), "w");

	if (fp != NULL)
	{
		fwrite(ss.str().c_str(),sizeof(char),ss.str().size(),fp);
	}

	fflush(fp);

	fclose(fp);

	return;
}

DWORD IntLogger::Lock()
{
	int iWriteTimeOut = 10;
	return WaitForSingleObject(this->m_hMutex, iWriteTimeOut);
}

void IntLogger::Unlock()
{
	ReleaseMutex(this->m_hMutex);
	return;
}

void IntLogger::Log(int time, int value)
{
	DWORD dwWaitResult;
	dwWaitResult = Lock();

	switch (dwWaitResult)
	{
	case WAIT_OBJECT_0:
		mVecLog.push_back(LogNode(time, value));
		break;
	default:
		printf("Unknow state of the mutex, final state is: %d\n", dwWaitResult);
		break;
	}

	Unlock();
	return;
}


