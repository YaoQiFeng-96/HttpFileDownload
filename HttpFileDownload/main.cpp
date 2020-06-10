#include <stdio.h>
#include "FileDownload.h"


int main(int argc, char *argv[])
{
	fprintf(stderr, "start. \n");
	getchar();


	const wchar_t *url = L"http://www.nirsoft.net/utils/nircmd.zip";
	const wchar_t *fileName = L"E:\\downloadfile.zip";
	bool tag = false;
	int statusCode = 0;

	CFileDownload	filedownload;

	//tag = true;
	//if (!m_filedownload.Init(url, fileName, tag, &statusCode))
	if (!filedownload.Init(url, nullptr, tag, &statusCode))
	{
		fprintf(stderr, "Init failed. \n");
		getchar();
		return -1;
	}
	fprintf(stderr, "statusCode = %d \n", statusCode);

	int totalSize = filedownload.GetFileSize();
	int recvCount = 0;

	uint8_t *ptr = nullptr;
	int size = 0;
	while (filedownload.GetData(ptr, &size))
	{
		recvCount += size;
		fprintf(stderr, "totalSzie = %6d    recvSize = %6d    GetData Size : %4d \n", totalSize, recvCount, size);
		ptr = nullptr;
		size = 0;
	}

	fprintf(stderr, "end. \n");
	getchar();
	return 0;
}
