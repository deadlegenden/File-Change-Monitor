#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <Windows.h>
#include <string>
#include <time.h>
#define PROGRAM_NAME "File Change Monitor"
#define MAX_PATH 1024
std::string getCurDir()
{
	TCHAR buffer[MAX_PATH];
	GetCurrentDirectoryA(sizeof(buffer), buffer);
	std::basic_string<TCHAR> dir = buffer;
	return dir;
}
std::string getCurTime()
{
	char buffer[80];
	time_t seconds = time(NULL);
	tm* timeinfo = localtime(&seconds);
	const char* format = "[%H:%M:%S] ";
	strftime(buffer, 80, format, timeinfo);
	std::string str(buffer);
	return str;
}
int fileWatcher(std::string dir)
{
	DWORD cbBytes;
	char fileName[MAX_PATH];
	char fileNameNew[MAX_PATH];
	char notify[1024];
	int count = 0;
	HANDLE dirHandle = CreateFile(dir.c_str(),GENERIC_READ | GENERIC_WRITE | FILE_LIST_DIRECTORY,FILE_SHARE_READ | FILE_SHARE_WRITE,NULL,OPEN_EXISTING,FILE_FLAG_BACKUP_SEMANTICS,NULL);
	if (dirHandle == INVALID_HANDLE_VALUE)
	{
		std::cout << "Error:" << GetLastError() << std::endl;
		return 0;
	}
	memset(notify, 0, strlen(notify));
	FILE_NOTIFY_INFORMATION* pnotify = (FILE_NOTIFY_INFORMATION*)notify;
	std::cout << PROGRAM_NAME << " successfully launched!" << std::endl;
	std::cout << "Current directory: " << dir << std::endl;
	while (true)
	{
		if (ReadDirectoryChangesW(dirHandle,&notify, MAX_PATH,true,FILE_NOTIFY_CHANGE_FILE_NAME| FILE_NOTIFY_CHANGE_SIZE | FILE_NOTIFY_CHANGE_DIR_NAME,&cbBytes,NULL,NULL))//чтение изменений
		{
			if (pnotify->FileName){memset(fileName, 0, strlen(fileName));WideCharToMultiByte(CP_ACP, 0, pnotify->FileName, pnotify->FileNameLength / 2, fileName, 99, NULL, NULL);}
			if (pnotify->NextEntryOffset != 0 && (pnotify->FileNameLength > 0 && pnotify->FileNameLength < MAX_PATH)){PFILE_NOTIFY_INFORMATION p = (PFILE_NOTIFY_INFORMATION)((char*)pnotify + pnotify->NextEntryOffset);memset(fileNameNew, 0, sizeof(fileNameNew));WideCharToMultiByte(CP_ACP, 0, p->FileName, p->FileNameLength / 2, fileNameNew, 99, NULL, NULL);}
			switch (pnotify->Action)
			{
			case FILE_ACTION_ADDED:
				std::cout << getCurTime() << "File Added: '" << fileName << "'" << std::endl;
				break;
			case FILE_ACTION_MODIFIED:
				std::cout << getCurTime() << "File Modified: '" << fileName << "'" << std::endl;
				break;
			case FILE_ACTION_REMOVED:
				std::cout << getCurTime() << "File Deleted: '" << fileName << "'" << std::endl;
				break;
			case FILE_ACTION_RENAMED_OLD_NAME:
				std::cout << getCurTime() << "File Renamed: '" << fileName << "' to '" << fileNameNew << "'" << std::endl;
				break;
			default:
				std::cout << "Unknow command!" << std::endl;
			}
		}
	}
	CloseHandle(dirHandle);
}
int main()
{
	setlocale(LC_ALL, "");
	SetConsoleTitleA(PROGRAM_NAME);
	fileWatcher("C:\\");
	system("pause");
	return 0;
}