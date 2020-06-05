#include <stdio.h>
#include "FileDownload.h"


int main(int argc, char *argv[])
{
	fprintf(stderr, "start. \n");
	getchar();


	const wchar_t *url = L"http://www.nirsoft.net/utils/nircmd.zip";
	//const wchar_t *url = L"http://60.191.59.13:7800/ftp/0.mp4";
	const wchar_t *fileName = L"E:\\downloadfile.zip";
	bool tag = false;

	CFileDownload	m_filedownload;
	if (!m_filedownload.Init(url, fileName, tag))
	{
		fprintf(stderr, "init failed. \n");
		getchar();
		return -1;
	}

	uint8_t *ptr = nullptr;
	int size = 0;
	while (m_filedownload.GetData(ptr, &size))
	{
		ptr = nullptr;
		size = 0;
	}

	fprintf(stderr, "end. \n");
	getchar();
	return 0;
}
