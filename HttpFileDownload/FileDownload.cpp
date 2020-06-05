#include "FileDownload.h"

CFileDownload::CFileDownload() :
	m_buffer(nullptr), m_bufLength(1024),
	m_hSession(nullptr), m_hConnect(nullptr), m_hRequest(nullptr), fp(nullptr),
	m_dwContentSize(0), m_receiveCount(0), m_dwSizeDW(sizeof(DWORD)), m_dwIndex(0)
{
	m_buffer = (uint8_t*)malloc(m_bufLength);
	memset(m_buffer, 0, m_bufLength);

	memset(&m_urlInfo, 0, sizeof(URL_INFO));
	memset(&m_pUrlCom, 0, sizeof(URL_COMPONENTS));
	m_pUrlCom.dwStructSize	= sizeof(m_pUrlCom);
	m_pUrlCom.lpszExtraInfo = m_urlInfo.szExtraInfo;
	m_pUrlCom.lpszHostName	= m_urlInfo.szHostName;
	m_pUrlCom.lpszPassword	= m_urlInfo.szPassword;
	m_pUrlCom.lpszScheme	= m_urlInfo.szScheme;
	m_pUrlCom.lpszUrlPath	= m_urlInfo.szUrlPath;
	m_pUrlCom.lpszUserName	= m_urlInfo.szUserName;

	m_pUrlCom.dwExtraInfoLength = 512;
	m_pUrlCom.dwHostNameLength	= 512;
	m_pUrlCom.dwPasswordLength	= 512;
	m_pUrlCom.dwSchemeLength	= 512;
	m_pUrlCom.dwUrlPathLength	= 512;
	m_pUrlCom.dwUserNameLength	= 512;
}


CFileDownload::~CFileDownload()
{
	if (nullptr != m_buffer)
	{
		free(m_buffer);
		m_buffer = nullptr;
	}

	if (nullptr != m_hRequest)
	{
		WinHttpCloseHandle(m_hRequest);
		m_hRequest = nullptr;
	}
	if (nullptr != m_hConnect)
	{
		WinHttpCloseHandle(m_hConnect);
		m_hConnect = nullptr;
	}
	if (nullptr != m_hSession)
	{
		WinHttpCloseHandle(m_hSession);
		m_hSession = nullptr;
	}
	if (nullptr != fp)
	{
		CloseHandle(fp);
		fp = nullptr;
	}
}

bool CFileDownload::Init(const wchar_t * url, const wchar_t * fileName, bool tag)
{
	WinHttpCrackUrl(url, 0, ICU_ESCAPE, &m_pUrlCom);

	if (tag)
	{
		fp = CreateFileW(fileName, 
			GENERIC_WRITE, 
			FILE_SHARE_READ, 
			NULL, 
			CREATE_ALWAYS, 
			FILE_ATTRIBUTE_NORMAL, 
			NULL);
		if (nullptr == fp)
		{
			fprintf(stderr, "file create failed. \n");
			return false;
		}
	}
		
	m_hSession = WinHttpOpen(NULL, WINHTTP_ACCESS_TYPE_NO_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
	if (nullptr == m_hSession)
		return false;
		

	m_hConnect = WinHttpConnect(m_hSession, m_pUrlCom.lpszHostName, m_pUrlCom.nPort, 0);
	if (nullptr == m_hConnect)
		return false;

	m_hRequest = WinHttpOpenRequest(m_hConnect, L"HEAD", m_pUrlCom.lpszUrlPath, L"HTTP/1.1", WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_REFRESH);
	if (nullptr == m_hRequest)
		return false;

	WinHttpSendRequest(m_hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0);
	if (!WinHttpReceiveResponse(m_hRequest, 0))
		return false;


	DWORD statusCode = 0;
	DWORD dwSize = sizeof(DWORD);
	DWORD index = 0;

	WinHttpQueryHeaders(m_hRequest,
		WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
		NULL, &statusCode, &dwSize, &index);
	fprintf(stderr, "status code = %d \n", statusCode);

	if (404 == statusCode)
	{
		fprintf(stderr, "404 Not Found. \n");
		return false;
	}
	if (200 != statusCode)
		return false;

	WinHttpCloseHandle(m_hRequest);

	m_hRequest = WinHttpOpenRequest(m_hConnect, L"HEAD", m_pUrlCom.lpszUrlPath, L"HTTP/1.1", WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_REFRESH);
	if (nullptr == m_hRequest)
		return false;
	WinHttpSendRequest(m_hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0);
	WinHttpReceiveResponse(m_hRequest, 0);
	WinHttpQueryHeaders(m_hRequest, WINHTTP_QUERY_CONTENT_LENGTH | WINHTTP_QUERY_FLAG_NUMBER, NULL, &m_dwContentSize, &m_dwSizeDW, &m_dwIndex);
	WinHttpCloseHandle(m_hRequest);
	fprintf(stderr, "file size = %d \n", m_dwContentSize);

	m_hRequest = WinHttpOpenRequest(m_hConnect, L"GET", m_pUrlCom.lpszUrlPath, L"HTTP/1.1", WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_REFRESH);
	if (nullptr == m_hRequest)
		return false;

	WinHttpSendRequest(m_hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0);
	WinHttpReceiveResponse(m_hRequest, 0);

	return true;
}

DWORD CFileDownload::GetFileSize()
{
	return m_dwContentSize;
}

bool CFileDownload::GetData(uint8_t * &pData, int * size)
{
	DWORD readByte = 0;
	WinHttpReadData(m_hRequest, m_buffer, m_bufLength, &readByte);
	if (!readByte)
		return false;

	m_receiveCount += readByte;

	if (nullptr != fp)
		WriteFile(fp, m_buffer, readByte, &readByte, NULL);

	pData = m_buffer;
	*size = readByte;
	
	return true;
}
