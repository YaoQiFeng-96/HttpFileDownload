#pragma once
#include <stdint.h>
#include <memory>
#include <Windows.h>
#include <winhttp.h>
#pragma comment(lib,"Winhttp.lib")

typedef struct _URL_INFO
{
	WCHAR szScheme[512];
	WCHAR szHostName[512];
	WCHAR szUserName[512];
	WCHAR szPassword[512];
	WCHAR szUrlPath[512];
	WCHAR szExtraInfo[512];
}URL_INFO, *PURL_INFO;

class CFileDownload
{
public:
	CFileDownload();
	~CFileDownload();

	bool Init(const wchar_t *url, const wchar_t *fileName, bool tag);
	DWORD GetFileSize();
	bool GetData(uint8_t* &pData, int *size);


private:
	uint8_t				*m_buffer;
	const int			m_bufLength;
	URL_INFO			m_urlInfo;
	URL_COMPONENTS		m_pUrlCom;

	HINTERNET			m_hSession;
	HINTERNET			m_hConnect;
	HINTERNET			m_hRequest;
	HANDLE				fp;

	DWORD				m_dwContentSize;
	int					m_receiveCount;
	DWORD				m_dwSizeDW;
	DWORD				m_dwIndex;
};

