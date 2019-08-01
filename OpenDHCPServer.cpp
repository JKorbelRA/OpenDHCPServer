/**************************************************************************
*   Copyright (C) 2005 by Achal Dhir                                      *
*   achaldhir@gmail.com                                                   *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                     a                                                   *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
***************************************************************************/
// Dual Service.cpp
#include <stdio.h>
#include <winsock2.h>
#include <time.h>
#include <tchar.h>
#include <ws2tcpip.h>
#include <limits.h>
#include <iphlpapi.h>
#include <process.h>
#include <math.h>
#include "OpenDHCPServer.h"

//Global Variables
//bool dhcpService = true;
bool kRunning = true;
bool verbatim = false;
SERVICE_STATUS serviceStatus;
SERVICE_STATUS_HANDLE serviceStatusHandle = 0;
HANDLE stopServiceEvent = 0;
//data1 network;
data1 network;
data2 cfig;
data9 token;
data9 dhcpr;
data71 lump;
dhcpMap dhcpCache;
//expiryMap dhcpAge;
char serviceName[] = "OpenDHCPServer";
char displayName[] = "Open DHCP Server";
//char tempbuff[512];
//char extbuff[512];
//char logBuff[512];
char htmlTitle[256] = "";
char filePATH[_MAX_PATH];
char iniFile[_MAX_PATH];
char leaFile[_MAX_PATH];
char logFile[_MAX_PATH];
char htmFile[_MAX_PATH];
char lnkFile[_MAX_PATH];
char tempFile[_MAX_PATH];
char cliFile[_MAX_PATH];
time_t t = time(NULL);
timeval tv;
fd_set readfds;
fd_set writefds;
HANDLE lEvent;
HANDLE fEvent;
HANDLE rEvent;

//constants
const char NBSP = 32;
const char RANGESET[] = "RANGE_SET";
const char GLOBALOPTIONS[] = "GLOBAL_OPTIONS";
const char base64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
//const char send403[] = "HTTP/1.1 403 Forbidden\r\nDate: %s\r\nLast-Modified: %s\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n";
const char send403[] = "HTTP/1.1 403 Forbidden\r\n\r\n<h1>403 Forbidden</h1>";
const char send404[] = "HTTP/1.1 404 Not Found\r\n\r\n<h1>404 Not Found</h1>";
const char td200[] = "<td>%s</td>";
const char tdnowrap200[] = "<td nowrap>%s</td>";
const char sVersion[] = "Open DHCP Server Version 1.75 Windows Build 1052";
const char htmlStart[] = "<html>\n<head>\n<title>%s</title>\n<meta http-equiv=\"refresh\" content=\"60\">\n<meta http-equiv=\"cache-control\" content=\"no-cache\">\n</head>\n";
//const char bodyStart[] = "<body bgcolor=\"#cccccc\"><table border=\"0\"><tr><td>\n<table width=\"100%%\" border=\"0\"><tr><td colspan=\"2\" align=\"center\"><font size=\"5\"><b>%s</b></font></td></tr><tr><td align=\"left\"><b>Server:</b> %s %s</td><td align=\"right\"><a target=\"_new\" href=\"http://dhcpserver.sourceforge.net\">http://dhcpserver.sourceforge.net</td></tr></table>";
const char bodyStart[] = "<body bgcolor=\"#cccccc\"><table border=\"0\"><tr><td>\n<table width=\"100%%\" border=\"0\"><tr><td nowrap colspan=\"2\" align=\"center\"><font size=\"6\"><b>%s</b></font></td></tr><tr><td align=\"left\"><b>Server:</b> %s %s</td><td align=\"right\"><a target=\"_new\" href=\"http://dhcpserver.sourceforge.net\">http://dhcpserver.sourceforge.net</td></tr></table>";
//const char bodyStart[] = "<body bgcolor=\"#cccccc\"><table width=640><tr><td align=\"center\"><font size=\"5\"><b>%s</b></font></td></tr><tr><td align=\"center\"><font size=\"5\">%s</font></td></tr></table>";
const data4 opData[] =
    {
		{ "SubnetMask", 1, 3, 1},
		{ "TimeOffset", 2, 4, 1},
		{ "Router", 3, 3, 1},
		{ "TimeServer", 4, 3, 1},
		{ "NameServer", 5, 3, 1},
		{ "DomainServer", 6, 3, 1},
		{ "LogServer", 7, 3, 1},
		{ "QuotesServer", 8, 3, 1},
		{ "LPRServer", 9, 3, 1},
		{ "ImpressServer", 10, 3, 1},
		{ "RLPServer", 11, 3, 1},
		{ "Hostname", 12, 1, 1},
		{ "BootFileSize", 13, 5, 1},
		{ "MeritDumpFile", 14, 1, 1},
		{ "DomainName", 15, 1, 1},
		{ "SwapServer", 16, 3, 1},
		{ "RootPath", 17, 1, 1},
		{ "ExtensionFile", 18, 1, 1},
		{ "ForwardOn/Off", 19, 7, 1},
		{ "SrcRteOn/Off", 20, 7, 1},
		{ "PolicyFilter", 21, 8, 1},
		{ "MaxDGAssembly", 22, 5, 1},
		{ "DefaultIPTTL", 23, 6, 1},
		{ "MTUTimeout", 24, 4, 1},
		{ "MTUPlateau", 25, 2, 1},
		{ "MTUInterface", 26, 5, 1},
		{ "MTUSubnet", 27, 7, 1},
		{ "BroadcastAddress", 28, 3, 1},
		{ "MaskDiscovery", 29, 7, 1},
		{ "MaskSupplier", 30, 7, 1},
		{ "RouterDiscovery", 31, 7, 1},
		{ "RouterRequest", 32, 3, 1},
		{ "StaticRoute", 33, 8, 1},
		{ "Trailers", 34, 7, 1},
		{ "ARPTimeout", 35, 4, 1},
		{ "Ethernet", 36, 7, 1},
		{ "DefaultTCPTTL", 37, 6, 1},
		{ "KeepaliveTime", 38, 4, 1},
		{ "KeepaliveData", 39, 7, 1},
		{ "NISDomain", 40, 1, 1},
		{ "NISServers", 41, 3, 1},
		{ "NTPServers", 42, 3, 1},
		{ "VendorSpecificInf", 43, 2, 0},
		{ "NETBIOSNameSrv", 44, 3, 1},
		{ "NETBIOSDistSrv", 45, 3, 1},
		{ "NETBIOSNodeType", 46, 6, 1},
		{ "NETBIOSScope", 47, 1, 1},
		{ "XWindowFont", 48, 1, 1},
		{ "XWindowManager", 49, 3, 1},
		{ "AddressRequest", 50, 3, 0},
		{ "AddressTime", 51, 4, 1},
		{ "OverLoad", 52, 7, 0},
		{ "DHCPMsgType", 53, 6, 0},
		{ "DHCPServerId", 54, 3, 0},
		{ "ParameterList", 55, 2, 0},
		{ "DHCPMessage", 56, 1, 0},
		{ "DHCPMaxMsgSize", 57, 5, 0},
		{ "RenewalTime", 58, 4, 1},
		{ "RebindingTime", 59, 4, 1},
		{ "ClassId", 60, 1, 0},
		{ "ClientId", 61, 2, 0},
		{ "NetWareIPDomain", 62, 1, 1},
		{ "NetWareIPOption", 63, 2, 1},
		{ "NISDomainName", 64, 1, 1},
		{ "NISServerAddr", 65, 3, 1},
		{ "TFTPServerName", 66, 1, 1},
		{ "BootFileOption", 67, 1, 1},
		{ "HomeAgentAddrs", 68, 3, 1},
		{ "SMTPServer", 69, 3, 1},
		{ "POP3Server", 70, 3, 1},
		{ "NNTPServer", 71, 3, 1},
		{ "WWWServer", 72, 3, 1},
		{ "FingerServer", 73, 3, 1},
		{ "IRCServer", 74, 3, 1},
		{ "StreetTalkServer", 75, 3, 1},
		{ "STDAServer", 76, 3, 1},
		{ "UserClass", 77, 1, 0},
		{ "DirectoryAgent", 78, 1, 1},
		{ "ServiceScope", 79, 1, 1},
		{ "RapidCommit", 80, 2, 0},
		{ "ClientFQDN", 81, 2, 0},
		{ "RelayAgentInformation", 82, 2, 0},
		{ "iSNS", 83, 1, 1},
		{ "NDSServers", 85, 3, 1},
		{ "NDSTreeName", 86, 1, 1},
		{ "NDSContext", 87, 1, 1},
		{ "LDAP", 95, 1, 1},
		{ "PCode", 100, 1, 1},
		{ "TCode", 101, 1, 1},
		{ "NetInfoAddress", 112, 3, 1},
		{ "NetInfoTag", 113, 1, 1},
		{ "URL", 114, 1, 1},
		{ "AutoConfig", 116, 7, 1},
		{ "NameServiceSearch", 117, 2, 1},
		{ "SubnetSelectionOption", 118, 3, 1},
		{ "DomainSearch", 119, 1, 1},
		{ "SIPServersDHCPOption", 120, 1, 1},
//		{ "121", 121, 1, 1},
		{ "CCC", 122, 1, 1},
		{ "TFTPServerIPaddress", 128, 3, 1},
		{ "CallServerIPaddress", 129, 3, 1},
		{ "DiscriminationString", 130, 1, 1},
		{ "RemoteStatisticsServerIPAddress", 131, 3, 1},
		{ "HTTPProxyPhone", 135, 3, 1},
		{ "OPTION_CAPWAP_AC_V4", 138, 1, 1},
		{ "OPTIONIPv4_AddressMoS", 139, 1, 1},
		{ "OPTIONIPv4_FQDNMoS", 140, 1, 1},
		{ "SIPUAServiceDomains", 141, 1, 1},
		{ "OPTIONIPv4_AddressANDSF", 142, 1, 1},
		{ "IPTelephone", 176, 1, 1},
		{ "ConfigurationFile", 209, 1, 1},
		{ "PathPrefix", 210, 1, 1},
		{ "RebootTime", 211, 4, 1},
		{ "OPTION_6RD", 212, 1, 1},
		{ "OPTION_V4_ACCESS_DOMAIN", 213, 1, 1},
		{ "BootFileName", 253, 1, 1},
        { "NextServer", 254, 3, 1},
    };

void WINAPI ServiceControlHandler(DWORD controlCode)
{
	switch (controlCode)
	{
		case SERVICE_CONTROL_INTERROGATE:
			break;

		case SERVICE_CONTROL_SHUTDOWN:
		case SERVICE_CONTROL_STOP:
			serviceStatus.dwCurrentState = SERVICE_STOP_PENDING;
			serviceStatus.dwWaitHint = 20000;
			serviceStatus.dwCheckPoint = 1;
			SetServiceStatus(serviceStatusHandle, &serviceStatus);
			kRunning = false;

			SetEvent(stopServiceEvent);
			return;

		case SERVICE_CONTROL_PAUSE:
			break;

		case SERVICE_CONTROL_CONTINUE:
			break;

		default:
			if (controlCode >= 128 && controlCode <= 255)
				break;
			else
				break;
	}

	SetServiceStatus(serviceStatusHandle, &serviceStatus);
}

void WINAPI ServiceMain(DWORD /*argc*/, TCHAR* /*argv*/[])
{
	char logBuff[512];
	serviceStatus.dwServiceType = SERVICE_WIN32;
	serviceStatus.dwCurrentState = SERVICE_STOPPED;
	serviceStatus.dwControlsAccepted = 0;
	serviceStatus.dwWin32ExitCode = NO_ERROR;
	serviceStatus.dwServiceSpecificExitCode = NO_ERROR;
	serviceStatus.dwCheckPoint = 0;
	serviceStatus.dwWaitHint = 0;

	serviceStatusHandle = RegisterServiceCtrlHandler(serviceName, ServiceControlHandler);

	if (serviceStatusHandle)
	{
		serviceStatus.dwCurrentState = SERVICE_START_PENDING;
		SetServiceStatus(serviceStatusHandle, &serviceStatus);

		if (_beginthread(init, 0, 0) == 0)
		{
			if (verbatim || cfig.dhcpLogLevel)
			{
				sprintf(logBuff, "Thread Creation Failed");
				logDHCPMess(logBuff, 1);
			}
			Sleep(1000);
			exit(-1);
		}

		tv.tv_sec = 20;
		tv.tv_usec = 0;

		stopServiceEvent = CreateEvent(0, FALSE, FALSE, 0);
		serviceStatus.dwControlsAccepted |= (SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN);
		serviceStatus.dwCurrentState = SERVICE_RUNNING;
		SetServiceStatus(serviceStatusHandle, &serviceStatus);

		do
		{
			if (!network.ready)
			{
				Sleep(1000);
				network.busy = false;
				continue;
			}

			if (!network.dhcpConn[0].ready)
			{
				Sleep(1000);
				network.busy = false;
				continue;
			}

			//Sleep(200000);
			//debug("good");

			FD_ZERO(&readfds);
			network.busy = true;

			if (network.httpConn.ready)
				FD_SET(network.httpConn.sock, &readfds);

			for (int i = 0; i < MAX_SERVERS && network.dhcpConn[i].ready; i++)
				FD_SET(network.dhcpConn[i].sock, &readfds);

			if (cfig.dhcpReplConn.ready)
				FD_SET(cfig.dhcpReplConn.sock, &readfds);

			if (select(network.maxFD, &readfds, NULL, NULL, &tv))
			{
				t = time(NULL);

				if (network.httpConn.ready && FD_ISSET(network.httpConn.sock, &readfds))
				{
					data19 *req = (data19*)calloc(1, sizeof(data19));

					if (req)
					{
						req->sockLen = sizeof(req->remote);
						req->sock = accept(network.httpConn.sock, (sockaddr*)&req->remote, &req->sockLen);

						if (req->sock == INVALID_SOCKET)
						{
							sprintf(logBuff, "Accept Failed, Error=%u\n", WSAGetLastError());
							logDHCPMess(logBuff, 1);
							free(req);
						}
						else
							procHTTP(req);
					}
					else
					{
						sprintf(logBuff, "Memory Error");
						logDHCPMess(logBuff, 1);
					}
				}

				for (int i = 0; i < MAX_SERVERS && network.dhcpConn[i].ready; i++)
				{
					if (FD_ISSET(network.dhcpConn[i].sock, &readfds) && gdmess(&dhcpr, i) && sdmess(&dhcpr))
						alad(&dhcpr);
				}

				if (cfig.dhcpReplConn.ready && FD_ISSET(cfig.dhcpReplConn.sock, &readfds))
				{
					errno = 0;
					dhcpr.sockLen = sizeof(dhcpr.remote);
					dhcpr.bytes = recvfrom(cfig.dhcpReplConn.sock,
										   dhcpr.raw,
										   sizeof(dhcpr.raw),
										   0,
										   (sockaddr*)&dhcpr.remote,
										   &dhcpr.sockLen);

					errno = WSAGetLastError();

					if (errno || dhcpr.bytes <= 0)
						cfig.dhcpRepl = 0;
				}
			}
			else
				t = time(NULL);
		}
		while (WaitForSingleObject(stopServiceEvent, 0) == WAIT_TIMEOUT);

		serviceStatus.dwCurrentState = SERVICE_STOP_PENDING;
		//serviceStatus.dwCheckPoint = 2;
		//serviceStatus.dwWaitHint = 1000;
		SetServiceStatus(serviceStatusHandle, &serviceStatus);
		sprintf(logBuff, "Closing Network Connections...");
		logDHCPMess(logBuff, 1);
		closeConn();

        if (cfig.dhcpReplConn.ready)
            closesocket(cfig.dhcpReplConn.sock);

		sprintf(logBuff, "Open DHCP Server Stopped !\n");
		logDHCPMess(logBuff, 1);

		Sleep(2000);

		WSACleanup();

		serviceStatus.dwControlsAccepted &= ~(SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN);
		serviceStatus.dwCurrentState = SERVICE_STOPPED;
		SetServiceStatus(serviceStatusHandle, &serviceStatus);

		CloseHandle(stopServiceEvent);
		stopServiceEvent = 0;
	}
}

void closeConn()
{
	if (network.httpConn.ready)
		closesocket(network.httpConn.sock);

	for (int i = 0; i < MAX_SERVERS && network.dhcpConn[i].loaded; i++)
		if (network.dhcpConn[i].ready)
			closesocket(network.dhcpConn[i].sock);
}

void runService()
{
	SERVICE_TABLE_ENTRY serviceTable[] =
	    {
	        {serviceName, ServiceMain},
	        {0, 0}
	    };

	StartServiceCtrlDispatcher(serviceTable);
}

void showError(MYDWORD enumber)
{
	LPTSTR lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		enumber,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR)&lpMsgBuf,
		0,
		NULL
	);
	printf("%s\n", lpMsgBuf);
}

bool stopService(SC_HANDLE service)
{
	if (service)
	{
		SERVICE_STATUS serviceStatus;
		QueryServiceStatus(service, &serviceStatus);
		if (serviceStatus.dwCurrentState != SERVICE_STOPPED)
		{
			ControlService(service, SERVICE_CONTROL_STOP, &serviceStatus);
			printf("Stopping Service.");
			for (int i = 0; i < 100; i++)
			{
				QueryServiceStatus(service, &serviceStatus);
				if (serviceStatus.dwCurrentState == SERVICE_STOPPED)
				{
					printf("Stopped\n");
					return true;
				}
				else
				{
					Sleep(500);
					printf(".");
				}
			}
			printf("Failed\n");
			return false;
		}
	}
	return true;
}

void installService()
{
	SC_HANDLE serviceControlManager = OpenSCManager(0, 0, SC_MANAGER_CREATE_SERVICE | SERVICE_START);

	if (serviceControlManager)
	{
		TCHAR path[ _MAX_PATH + 1 ];
		if (GetModuleFileName(0, path, sizeof(path) / sizeof(path[0])) > 0)
		{
			SC_HANDLE service = CreateService(serviceControlManager,
											  serviceName, displayName,
											  SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS,
											  SERVICE_AUTO_START, SERVICE_ERROR_IGNORE, path,
											  0, 0, 0, 0, 0);
			if (service)
			{
				printf("Successfully installed.. !\n");
				StartService(service, 0, NULL);
				CloseServiceHandle(service);
			}
			else
			{
				showError(GetLastError());
			}
		}
		CloseServiceHandle(serviceControlManager);
	}
}

void uninstallService()
{
	SC_HANDLE serviceControlManager = OpenSCManager(0, 0, SC_MANAGER_CONNECT);

	if (serviceControlManager)
	{
		SC_HANDLE service = OpenService(serviceControlManager,
		                                serviceName, SERVICE_QUERY_STATUS | SERVICE_STOP | DELETE);
		if (service)
		{
			if (stopService(service))
			{
				if (DeleteService(service))
					printf("Successfully Removed !\n");
				else
					showError(GetLastError());
			}
			else
				printf("Failed to Stop Service..\n");

			CloseServiceHandle(service);
		}
		else
			printf("Service Not Found..\n");

		CloseServiceHandle(serviceControlManager);
	}
}

int main(int argc, TCHAR* argv[])
{
	OSVERSIONINFO osvi;
	osvi.dwOSVersionInfoSize = sizeof(osvi);
	bool result = GetVersionEx(&osvi);

	if (result && osvi.dwPlatformId >= VER_PLATFORM_WIN32_NT)
	{
		if (argc > 1 && lstrcmpi(argv[1], TEXT("-i")) == 0)
		{
			installService();
		}
		else if (argc > 1 && lstrcmpi(argv[1], TEXT("-u")) == 0)
		{
			uninstallService();
		}
		else if (argc > 1 && lstrcmpi(argv[1], TEXT("-v")) == 0)
		{
			SC_HANDLE serviceControlManager = OpenSCManager(0, 0, SC_MANAGER_CONNECT);
			bool serviceStopped = true;

			if (serviceControlManager)
			{
				SC_HANDLE service = OpenService(serviceControlManager, serviceName, SERVICE_QUERY_STATUS | SERVICE_STOP);

				if (service)
				{
					serviceStopped = stopService(service);
					CloseServiceHandle(service);
				}
				CloseServiceHandle(serviceControlManager);
			}

			if (serviceStopped)
			{
				verbatim = true;
				runProg();
			}
			else
				printf("Failed to Stop Service\n");
		}
		else
			runService();
	}
	else if (argc == 1 || lstrcmpi(argv[1], TEXT("-v")) == 0)
	{
		verbatim = true;
		runProg();
	}
	else
		printf("This option is not available on Windows95/98/ME\n");

	return 0;
}

void runProg()
{
	//printf("%i\n",t);
	//printf("%i\n",sizeof(data7));
	//printf("%d\n",dnsCache[currentInd].max_size());

	char logBuff[512];

	if (_beginthread(init, 0, 0) == 0)
	{
		if (verbatim || cfig.dhcpLogLevel)
		{
			sprintf(logBuff, "Thread Creation Failed");
			logDHCPMess(logBuff, 1);
		}

		Sleep(1000);
		exit(-1);
	}

	tv.tv_sec = 20;
	tv.tv_usec = 0;

	do
	{
		FD_ZERO(&readfds);

		if (!network.ready)
		{
			Sleep(1000);
			network.busy = false;
			continue;
		}

		if (!network.dhcpConn[0].ready)
		{
			Sleep(1000);
			network.busy = false;
			continue;
		}

		network.busy = true;

		if (network.httpConn.ready)
			FD_SET(network.httpConn.sock, &readfds);

		for (int i = 0; i < MAX_SERVERS && network.dhcpConn[i].ready; i++)
			FD_SET(network.dhcpConn[i].sock, &readfds);

		if (cfig.dhcpReplConn.ready)
			FD_SET(cfig.dhcpReplConn.sock, &readfds);

		if (select(network.maxFD, &readfds, NULL, NULL, &tv))
		{
			t = time(NULL);

			if (network.httpConn.ready && FD_ISSET(network.httpConn.sock, &readfds))
			{
				data19 *req = (data19*)calloc(1, sizeof(data19));

				if (req)
				{
					req->sockLen = sizeof(req->remote);
					req->sock = accept(network.httpConn.sock, (sockaddr*)&req->remote, &req->sockLen);

					if (req->sock == INVALID_SOCKET)
					{
						sprintf(logBuff, "Accept Failed, Error=%u\n", WSAGetLastError());
						logDHCPMess(logBuff, 1);
						free(req);
					}
					else
						procHTTP(req);
				}
				else
				{
					sprintf(logBuff, "Memory Error");
					logDHCPMess(logBuff, 1);
				}
			}

			for (int i = 0; i < MAX_SERVERS && network.dhcpConn[i].ready; i++)
			{
				if (FD_ISSET(network.dhcpConn[i].sock, &readfds) && gdmess(&dhcpr, i) && sdmess(&dhcpr))
					alad(&dhcpr);
			}

			if (cfig.dhcpReplConn.ready && FD_ISSET(cfig.dhcpReplConn.sock, &readfds))
			{
				errno = 0;
				dhcpr.sockLen = sizeof(dhcpr.remote);
				dhcpr.bytes = recvfrom(cfig.dhcpReplConn.sock,
									   dhcpr.raw,
									   sizeof(dhcpr.raw),
									   0,
									   (sockaddr*)&dhcpr.remote,
									   &dhcpr.sockLen);

				errno = WSAGetLastError();

				if (errno || dhcpr.bytes <= 0)
					cfig.dhcpRepl = 0;
			}
		}
		else
			t = time(NULL);
	}
	while (kRunning);

	kRunning = false;
    sprintf(logBuff, "Closing Network Connections...");
    logDHCPMess(logBuff, 1);
	closeConn();

	if (cfig.dhcpReplConn.ready)
		closesocket(cfig.dhcpReplConn.sock);

    sprintf(logBuff, "Open DHCP Server Stopped !\n");
    logDHCPMess(logBuff, 1);

	WSACleanup();
}

MYWORD fUShort(void *raw)
{
	return ntohs(*((MYWORD*)raw));
}

MYDWORD fULong(void *raw)
{
	return ntohl(*((MYDWORD*)raw));
}

MYDWORD fIP(void *raw)
{
	return(*((MYDWORD*)raw));
}

MYBYTE pUShort(void *raw, MYWORD data)
{
	*((MYWORD*)raw) = htons(data);
	return sizeof(MYWORD);
}

MYBYTE pULong(void *raw, MYDWORD data)
{
	*((MYDWORD*)raw) = htonl(data);
	return sizeof(MYDWORD);
}

MYBYTE pIP(void *raw, MYDWORD data)
{
	*((MYDWORD*)raw) = data;
	return sizeof(MYDWORD);
}

void procHTTP(data19 *req)
{
	//debug("procHTTP");
	char logBuff[256];
	char tempbuff[512];
	req->ling.l_onoff = 1; //0 = off (l_linger ignored), nonzero = on
	req->ling.l_linger = 30; //0 = discard data, nonzero = wait for data sent
	setsockopt(req->sock, SOL_SOCKET, SO_LINGER, (const char*)&req->ling, sizeof(req->ling));

	timeval tv1;
	fd_set readfds1;
	FD_ZERO(&readfds1);
	tv1.tv_sec = 1;
	tv1.tv_usec = 0;
	FD_SET(req->sock, &readfds1);

	if (!select((req->sock + 1), &readfds1, NULL, NULL, &tv1))
	{
		sprintf(logBuff, "HTTP Client %s, Message Receive failed", IP2String(tempbuff, req->remote.sin_addr.s_addr));
		logDHCPMess(logBuff, 1);
		closesocket(req->sock);
		free(req);
		return;
	}

	char buffer[2048];
	req->bytes = recv(req->sock, buffer, sizeof(buffer), 0);
	errno = WSAGetLastError();

	if (verbatim || cfig.dhcpLogLevel >= 1 && (errno || req->bytes <= 0))
	{
		sprintf(logBuff, "HTTP Client %s, Message Receive failed, WSAError %d", IP2String(tempbuff, req->remote.sin_addr.s_addr), errno);
		logDHCPMess(logBuff, 1);
		closesocket(req->sock);
		free(req);
		return;
	}
	else if (verbatim || cfig.dhcpLogLevel >= 2)
	{
		sprintf(logBuff, "HTTP Client %s, Request Processed", IP2String(tempbuff, req->remote.sin_addr.s_addr));
		logDHCPMess(logBuff, 2);
		//printf("%s\n", buffer);
	}

	if (cfig.httpClients[0] && !findServer(cfig.httpClients, 8, req->remote.sin_addr.s_addr))
	{
		if (verbatim || cfig.dhcpLogLevel >= 2)
		{
			sprintf(logBuff, "HTTP Client %s, Access Denied", IP2String(tempbuff, req->remote.sin_addr.s_addr));
			logDHCPMess(logBuff, 2);
		}

		req->dp = (char*)calloc(1, sizeof(send403));
		req->memSize = sizeof(send403);
		req->bytes = sprintf(req->dp, send403);
		return;
	}

	buffer[sizeof(buffer) - 1] = 0;
	char *fp = NULL;
	char *end = strchr(buffer, '\n');

	if (end && end > buffer && (*(end - 1) == '\r'))
	{
		*(end - 1) = 0;

		if (myTokenize(buffer, buffer, " ", true) > 1)
			fp = myGetToken(buffer, 1);
	}

	if (fp && !strcasecmp(fp, "/"))
		sendStatus(req);
//	else if (fp && !strcasecmp(fp, "/scopestatus"))
//		sendScopeStatus(req);
	else
	{
		if (fp && (verbatim || cfig.dhcpLogLevel >= 2))
		{
			sprintf(logBuff, "HTTP Client %s, %s not found", IP2String(tempbuff, req->remote.sin_addr.s_addr), fp);
			logDHCPMess(logBuff, 2);
		}
		else if (verbatim || cfig.dhcpLogLevel >= 2)
		{
			sprintf(logBuff, "HTTP Client %s, Invalid request", IP2String(tempbuff, req->remote.sin_addr.s_addr));
			logDHCPMess(logBuff, 2);
		}

		req->dp = (char*)calloc(1, sizeof(send404));
		req->bytes = sprintf(req->dp, send404);
		req->memSize = sizeof(send404);
		_beginthread(sendHTTP, 0, (void*)req);
		return;
	}
}

void sendStatus(data19 *req)
{
	//debug("sendStatus");
	char ipbuff[16];
	char extbuff[16];
	char logBuff[512];
	char tempbuff[512];

	dhcpMap::iterator p;
	MYDWORD iip = 0;
	data7 *dhcpEntry = NULL;
	//data7 *cache = NULL;
	//printf("%d=%d\n", dhcpCache.size(), cfig.dhcpSize);
	req->memSize = 2048 + (135 * dhcpCache.size()) + (cfig.dhcpSize * 26);
	req->dp = (char*)calloc(1, req->memSize);

	if (!req->dp)
	{
		sprintf(logBuff, "Memory Error");
		logDHCPMess(logBuff, 1);
		closesocket(req->sock);
		free(req);
		return;
	}

	char *fp = req->dp;
	char *maxData = req->dp + (req->memSize - 512);
	//tm *ttm = gmtime(&t);
	//strftime(tempbuff, sizeof(tempbuff), "%a, %d %b %Y %H:%M:%S GMT", ttm);
	//fp += sprintf(fp, send200, tempbuff, tempbuff);
	//fp += sprintf(fp, send200);
	//char *contentStart = fp;
	fp += sprintf(fp, htmlStart, htmlTitle);

	//const char bodyStart[] = "<body bgcolor=\"#cccccc\"><table border=\"0\"><tr><td>\n<table width=\"100%%\" border=\"0\"><tr><td colspan=\"2\" align=\"center\"><font size=\"5\"><b>%s</b></font></td></tr><tr><td align=\"left\">Server Name: %s%s</td><td align=\"right\"><a target=\"_new\" href=\"http://dhcp-dns-server.sourceforge.net\">http://dhcp-dns-server.sourceforge.net</td></tr></table>";

	if (cfig.replication == 1)
		fp += sprintf(fp, bodyStart, sVersion, cfig.servername, "(Primary)");
	else if (cfig.replication == 2)
		fp += sprintf(fp, bodyStart, sVersion, cfig.servername, "(Secondary)");
	else
		fp += sprintf(fp, bodyStart, sVersion, cfig.servername, "");

	//fp += sprintf(fp, bodyStart, sVersion, cfig.servername);
	fp += sprintf(fp, "\n<table border=\"1\" cellpadding=\"1\" width=\"100%%\" bgcolor=\"#b8b8b8\">\n");

	if (cfig.dhcpRepl > t)
	{
		fp += sprintf(fp, "<tr><th colspan=\"5\"><font size=\"5\"><i>Active Leases</i></font></th></tr>\n");
		fp += sprintf(fp, "<tr><th>Mac Address</th><th>IP</th><th>Lease Expiry</th><th>Hostname (first 20 chars)</th><th>Server</th></tr>\n");
	}
	else
	{
		fp += sprintf(fp, "<tr><th colspan=\"4\"><font size=\"5\"><i>Active Leases</i></font></th></tr>\n");
		fp += sprintf(fp, "<tr><th>Mac Address</th><th>IP</th><th>Lease Expiry</th><th>Hostname (first 20 chars)</th></tr>\n");
	}

	for (p = dhcpCache.begin(); kRunning && p != dhcpCache.end() && fp < maxData; p++)
	{
		//if ((dhcpEntry = p->second) && dhcpEntry->display)
		if ((dhcpEntry = p->second) && dhcpEntry->display && dhcpEntry->expiry >= t)
		{
			fp += sprintf(fp, "<tr>");
			fp += sprintf(fp, td200, dhcpEntry->mapname);
			fp += sprintf(fp, td200, IP2String(tempbuff, dhcpEntry->ip));

			if (dhcpEntry->expiry >= INT_MAX)
				fp += sprintf(fp, td200, "Infinity");
			else
			{
				tm *ttm = localtime(&dhcpEntry->expiry);
				strftime(tempbuff, sizeof(tempbuff), "%d-%b-%y %X", ttm);
				fp += sprintf(fp, tdnowrap200, tempbuff);
			}

			if (dhcpEntry->hostname[0])
			{
				strcpy(tempbuff, dhcpEntry->hostname);
				tempbuff[20] = 0;
				fp += sprintf(fp, td200, tempbuff);
			}
			else
				fp += sprintf(fp, td200, "&nbsp;");

			if (cfig.dhcpRepl > t)
			{
				if (dhcpEntry->local && cfig.replication == 1)
					fp += sprintf(fp, td200, "Primary");
				else if (dhcpEntry->local && cfig.replication == 2)
					fp += sprintf(fp, td200, "Secondary");
				else if (cfig.replication == 1)
					fp += sprintf(fp, td200, "Secondary");
				else
					fp += sprintf(fp, td200, "Primary");
			}

			fp += sprintf(fp, "</tr>\n");
		}
	}

	fp += sprintf(fp, "</table>\n<br>\n<table border=\"1\" cellpadding=\"1\" width=\"100%%\" bgcolor=\"#b8b8b8\">\n");
	fp += sprintf(fp, "<tr><th colspan=\"4\"><font size=\"5\"><i>Free Dynamic Leases</i></font></th></tr>\n");
	fp += sprintf(fp, "<tr><td><b>DHCP Range</b></td><td><b>Mask</b></td><td align=\"right\"><b>Available Leases</b></td><td align=\"right\"><b>Free Leases</b></td></tr>\n");

	for (char rangeInd = 0; kRunning && rangeInd < cfig.rangeCount && fp < maxData; rangeInd++)
	{
		MYWORD ipused = 0;
		MYWORD ipfree = 0;
		MYWORD ind = 0;

		for (MYDWORD iip = cfig.dhcpRanges[rangeInd].rangeStart; iip <= cfig.dhcpRanges[rangeInd].rangeEnd; iip++, ind++)
		{
			if (cfig.dhcpRanges[rangeInd].expiry[ind] != INT_MAX)
			{
				if (cfig.dhcpRanges[rangeInd].expiry[ind] < t)
					ipfree++;
				else
					ipused++;
			}
		}

		IP2String(tempbuff, ntohl(cfig.dhcpRanges[rangeInd].rangeStart));
		IP2String(ipbuff, ntohl(cfig.dhcpRanges[rangeInd].rangeEnd));
		IP2String(extbuff, cfig.dhcpRanges[rangeInd].mask);
		fp += sprintf(fp, "<tr><td nowrap>%s - %s</td><td nowrap>%s</td><td align=\"right\">%i</td><td align=\"right\">%i</td></tr>\n", tempbuff, ipbuff, extbuff, (ipused + ipfree), ipfree);
	}

	fp += sprintf(fp, "</table>\n<br>\n<table border=\"1\" width=\"100%%\" cellpadding=\"1\" bgcolor=\"#b8b8b8\">\n");
	fp += sprintf(fp, "<tr><th colspan=\"4\"><font size=\"5\"><i>Free Static Leases</i></font></th></tr>\n");
	fp += sprintf(fp, "<tr><th>Mac Address</th><th>IP</th><th>Mac Address</th><th>IP</th></tr>\n");

	MYBYTE colNum = 0;

	for (p = dhcpCache.begin(); kRunning && p != dhcpCache.end() && fp < maxData; p++)
	{
		if ((dhcpEntry = p->second) && dhcpEntry->fixed && dhcpEntry->expiry < t)
		{
			if (!colNum)
			{
				fp += sprintf(fp, "<tr>");
				colNum = 1;
			}
			else if (colNum == 1)
			{
				colNum = 2;
			}
			else if (colNum == 2)
			{
				fp += sprintf(fp, "</tr>\n<tr>");
				colNum = 1;
			}

			fp += sprintf(fp, td200, dhcpEntry->mapname);
			fp += sprintf(fp, td200, IP2String(tempbuff, dhcpEntry->ip));
		}
	}

	if (colNum)
		fp += sprintf(fp, "</tr>\n");

	fp += sprintf(fp, "</table></td></tr></table>\n</body>\n</html>");
	//MYBYTE x = sprintf(tempbuff, "%u", (fp - contentStart));
	//memcpy((contentStart - 12), tempbuff, x);
	req->bytes = fp - req->dp;
	_beginthread(sendHTTP, 0, (void*)req);
	return;
}

/*
void sendScopeStatus(data19 *req)
{
	//debug("sendScopeStatus");

	MYBYTE rangeCount = 0;
	req->memSize = 1536 + (150 * cfig.rangeCount);
	req->dp = (char*)calloc(1, req->memSize);

	if (!req->dp)
	{
		sprintf(logBuff, "Memory Error");
		logDHCPMess(logBuff, 1);
		closesocket(req->sock);
		free(req);
		return;
	}

	char *fp = req->dp;
	char *maxData = req->dp + (req->memSize - 512);
	tm *ttm = gmtime(&t);
	strftime(tempbuff, sizeof(tempbuff), "%a, %d %b %Y %H:%M:%S GMT", ttm);
	fp += sprintf(fp, send200, tempbuff, tempbuff);
	char *contentStart = fp;
	fp += sprintf(fp, htmlStart, htmlTitle);
	fp += sprintf(fp, bodyStart, sVersion);
	fp += sprintf(fp, "<table border=\"1\" cellpadding=\"1\" width=\"640\" bgcolor=\"#b8b8b8\">\n");
	fp += sprintf(fp, "<tr><th colspan=\"4\"><font size=\"5\"><i>Scope Status</i></font></th></tr>\n");
	fp += sprintf(fp, "<tr><td><b>DHCP Range</b></td><td align=\"right\"><b>IPs Used</b></td><td align=\"right\"><b>IPs Free</b></td><td align=\"right\"><b>%% Free</b></td></tr>\n");
	MYBYTE colNum = 0;

	for (char rangeInd = 0; kRunning && rangeInd < cfig.rangeCount && fp < maxData; rangeInd++)
	{
		float ipused = 0;
		float ipfree = 0;
		int ind = 0;

		for (MYDWORD iip = cfig.dhcpRanges[rangeInd].rangeStart; iip <= cfig.dhcpRanges[rangeInd].rangeEnd; iip++, ind++)
		{
			if (cfig.dhcpRanges[rangeInd].expiry[ind] > t)
				ipused++;
			else
				ipfree++;
		}

		IP2String(tempbuff, ntohl(cfig.dhcpRanges[rangeInd].rangeStart));
		IP2String(extbuff, ntohl(cfig.dhcpRanges[rangeInd].rangeEnd));
		fp += sprintf(fp, "<tr><td>%s - %s</td><td align=\"right\">%5.0f</td><td align=\"right\">%5.0f</td><td align=\"right\">%2.2f</td></tr>\n", tempbuff, extbuff, ipused, ipfree, ((ipfree * 100)/(ipused + ipfree)));
	}

	fp += sprintf(fp, "</table>\n</body>\n</html>");
	memcpy((contentStart - 12), tempbuff, sprintf(tempbuff, "%u", (fp - contentStart)));
	req->bytes = fp - req->dp;

	_beginthread(sendHTTP, 0, (void*)req);
	return;
}
*/

/*
void __cdecl sendHTTP(void *lpParam)
{
	data19 *req = (data19*)lpParam;

	//sprintf(logBuff, "sendHTTP memsize=%d bytes=%d", req->memSize, req->bytes);
	//(logBuff);

	char *dp = req->dp;
	timeval tv1;
	fd_set writefds1;
	char header[16];
	int sent = 0;
	byte sendheader = true;

	while (kRunning && req->bytes > 0)
	{
		tv1.tv_sec = 5;
		tv1.tv_usec = 0;
		FD_ZERO(&writefds1);
		FD_SET(req->sock, &writefds1);

		if (select((req->sock + 1), NULL, &writefds1, NULL, &tv1))
		{
			if (sendheader)
			{
				char send200[] = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: close\r\nTransfer-Encoding: chunked\r\n";
				send(req->sock, send200, strlen(send200), 0);
				sendheader = false;
			}
			else if (req->bytes > 1024)
			{
				sprintf(header, "\r\n%04x\r\n", 1024);
				send(req->sock, header, 8, 0);
				sent  = send(req->sock, dp, 1024, 0);
			}
			else if (req->bytes > 0)
			{
				sprintf(header, "\r\n%04x\r\n", req->bytes);
				send(req->sock, header, 8, 0);
				sent  = send(req->sock, dp, req->bytes, 0);
			}
			else
			{
				sprintf(header, "\r\n%04x\r\n", 0);
				send(req->sock, header, 8, 0);
				break;
			}

			errno = WSAGetLastError();

			if (errno || sent < 0)
				break;

			dp += sent;
			req->bytes -= sent;
		}
		else
			break;
	}
	//Sleep(10*1000);
	//shutdown(req->sock, 2);
	closesocket(req->sock);
	free(req->dp);
	free(req);
	_endthread();
	return;
}
*/

/*
void __cdecl sendHTTP(void *lpParam)
{
	data19 *req = (data19*)lpParam;

	//sprintf(logBuff, "sendHTTP memsize=%d bytes=%d", req->memSize, req->bytes);
	//(logBuff);

	char *dp = req->dp;
	timeval tv1;
	fd_set writefds1;
	int sent = 0;
	bool sendheader = true;

	while (kRunning && req->bytes > 0)
	{
		tv1.tv_sec = 5;
		tv1.tv_usec = 0;
		FD_ZERO(&writefds1);
		FD_SET(req->sock, &writefds1);

		if (select((req->sock + 1), NULL, &writefds1, NULL, &tv1))
		{
			if (sendheader)
			{
				char send200[] = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n";
				send(req->sock, send200, strlen(send200), 0);
				sendheader = false;
			}
			else if (req->bytes > 1024)
				sent  = send(req->sock, dp, 1024, 0);
			else if (req->bytes > 0)
				sent  = send(req->sock, dp, req->bytes, 0);
			else
				break;

			errno = WSAGetLastError();

			if (errno || sent < 0)
				break;

			dp += sent;
			req->bytes -= sent;
		}
		else
			break;
	}
	//Sleep(10*1000);
	//shutdown(req->sock, 2);
	closesocket(req->sock);
	free(req->dp);
	free(req);
	_endthread();
	return;
}
*/

void __cdecl sendHTTP(void *lpParam)
{
	data19 *req = (data19*)lpParam;

	//sprintf(logBuff, "sendHTTP memsize=%d bytes=%d", req->memSize, req->bytes);
	//(logBuff);

	char *dp = req->dp;
	timeval tv1;
	fd_set writefds1;
	int sent = 0;
	bool sendheader = true;

	while (kRunning && req->bytes > 0)
	{
		tv1.tv_sec = 5;
		tv1.tv_usec = 0;
		FD_ZERO(&writefds1);
		FD_SET(req->sock, &writefds1);

		if (select((req->sock + 1), NULL, &writefds1, NULL, &tv1))
		{
			if (sendheader)
			{
				char send200[] = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: close\r\nContent-Length: %i\r\n\r\n";
				char header[256];
				sprintf(header, send200, req->bytes);
				send(req->sock, header, strlen(header), 0);
				sendheader = false;
			}
			else if (req->bytes > 1024)
				sent  = send(req->sock, dp, 1024, 0);
			else if (req->bytes > 0)
				sent  = send(req->sock, dp, req->bytes, 0);
			else
				break;

			errno = WSAGetLastError();

			if (errno || sent < 0)
				break;

			dp += sent;
			req->bytes -= sent;
		}
		else
			break;
	}
	//Sleep(10*1000);
	//shutdown(req->sock, 2);
	closesocket(req->sock);
	free(req->dp);
	free(req);
	_endthread();
	return;
}

bool checkRange(data17 *rangeData, char rangeInd)
{
	//debug("checkRange");

	if (!cfig.hasFilter)
		return true;

	MYBYTE rangeSetInd = cfig.dhcpRanges[rangeInd].rangeSetInd;
	data14 *rangeSet = &cfig.rangeSet[rangeSetInd];
	//printf("checkRange entering, rangeInd=%i rangeSetInd=%i\n", rangeInd, rangeSetInd);
	//printf("checkRange entered, macFound=%i vendFound=%i userFound=%i\n", macFound, vendFound, userFound);

	if((!rangeData->macFound && !rangeSet->macSize[0]) || (rangeData->macFound && rangeData->macArray[rangeSetInd]))
		if((!rangeData->vendFound && !rangeSet->vendClassSize[0]) || (rangeData->vendFound && rangeData->vendArray[rangeSetInd]))
			if((!rangeData->userFound && !rangeSet->userClassSize[0]) || (rangeData->userFound && rangeData->userArray[rangeSetInd]))
				if((!rangeData->subnetFound && !rangeSet->subnetIP[0]) || (rangeData->subnetFound && rangeData->subnetArray[rangeSetInd]))
					return true;

	//printf("checkRange, returning false rangeInd=%i rangeSetInd=%i\n", rangeInd, rangeSetInd);
	return false;
}

bool checkIP(data9 *req, data17 *rangeData, MYDWORD ip)
{
	MYDWORD rangeStart;
	MYDWORD rangeEnd;

	char rangeInd = getRangeInd(ip);

	if (rangeInd < 0)
		return false;

	if (!checkRange(rangeData, rangeInd))
		return false;

	MYWORD ind = getIndex(rangeInd, ip);
	data13 *range = &cfig.dhcpRanges[rangeInd];
	data7 *dhcpEntry = range->dhcpEntry[ind];

	if ((req->dhcpEntry != dhcpEntry && range->expiry[ind] > t) or range->expiry[ind] == INT_MAX)
		return false;

	if(req->subnetIP)
	{
		if(cfig.rangeSet[range->rangeSetInd].subnetIP[0])
		{
			rangeStart = range->rangeStart;
			rangeEnd = range->rangeEnd;
		}
		else
		{
			calcRangeLimits(req->subnetIP, range->mask, &rangeStart, &rangeEnd);

			if (rangeStart < range->rangeStart)
				rangeStart = range->rangeStart;

			if (rangeEnd > range->rangeEnd)
				rangeEnd = range->rangeEnd;
		}

		if (htonl(ip) >= rangeStart && htonl(ip) <= rangeEnd)
			return true;
	}
	else
	{
		calcRangeLimits(network.dhcpConn[req->sockInd].server, range->mask, &rangeStart, &rangeEnd);

		if (rangeStart < range->rangeStart)
			rangeStart = range->rangeStart;

		if (rangeEnd > range->rangeEnd)
			rangeEnd = range->rangeEnd;

		if (htonl(ip) >= rangeStart && htonl(ip) <= rangeEnd)
			return true;
	}
	return false;
}

MYDWORD resad(data9 *req)
{
	//debug("resad");
	char logBuff[512];
	char tempbuff[512];

	if (req->dhcpp.header.bp_giaddr)
	{
		lockIP(req->dhcpp.header.bp_giaddr);
		lockIP(req->remote.sin_addr.s_addr);
	}

	req->dhcpEntry = findDHCPEntry(req->chaddr);

	if (req->dhcpEntry && req->dhcpEntry->fixed)
	{
		if (req->dhcpEntry->ip)
		{
			setTempLease(req->dhcpEntry);
			return req->dhcpEntry->ip;
		}
		else
		{
			if (verbatim || cfig.dhcpLogLevel)
			{
				sprintf(logBuff, "Static DHCP Host %s (%s) has No IP, DHCPDISCOVER ignored", req->chaddr, req->hostname);
				logDHCPMess(logBuff, 1);
			}
			return 0;
		}
	}

	MYDWORD rangeStart = 0;
	MYDWORD rangeEnd = 0;
	MYDWORD iipNew = 0;
	MYDWORD iipExp = 0;
	bool rangeFound = false;
	data17 rangeData;
	memset(&rangeData, 0, sizeof(data17));

	if (cfig.hasFilter)
	{
		for (MYBYTE rangeSetInd = 0; rangeSetInd < MAX_RANGE_SETS && cfig.rangeSet[rangeSetInd].active; rangeSetInd++)
		{
			data14 *rangeSet = &cfig.rangeSet[rangeSetInd];

			for (MYBYTE i = 0; i < MAX_RANGE_FILTERS && rangeSet->macSize[i]; i++)
			{
				//printf("%s\n", hex2String(tempbuff, rangeSet->macStart[i], rangeSet->macSize[i]));
				//printf("%s\n", hex2String(tempbuff, rangeSet->macEnd[i], rangeSet->macSize[i]));

				if(memcmp(req->dhcpp.header.bp_chaddr, rangeSet->macStart[i], rangeSet->macSize[i]) >= 0 && memcmp(req->dhcpp.header.bp_chaddr, rangeSet->macEnd[i], rangeSet->macSize[i]) <= 0)
				{
					rangeData.macArray[rangeSetInd] = 1;
					rangeData.macFound = true;
					//printf("mac Found, rangeSetInd=%i\n", rangeSetInd);
					break;
				}
			}

			for (MYBYTE i = 0; i < MAX_RANGE_FILTERS && req->vendClass.size && rangeSet->vendClassSize[i]; i++)
			{
				if(rangeSet->vendClassSize[i] == req->vendClass.size && !memcmp(req->vendClass.value, rangeSet->vendClass[i], rangeSet->vendClassSize[i]))
				{
					rangeData.vendArray[rangeSetInd] = 1;
					rangeData.vendFound = true;
					//printf("vend Found, rangeSetInd=%i\n", rangeSetInd);
					break;
				}
			}

			for (MYBYTE i = 0; i < MAX_RANGE_FILTERS && req->userClass.size && rangeSet->userClassSize[i]; i++)
			{
				if(rangeSet->userClassSize[i] == req->userClass.size && !memcmp(req->userClass.value, rangeSet->userClass[i], rangeSet->userClassSize[i]))
				{
					rangeData.userArray[rangeSetInd] = 1;
					rangeData.userFound = true;
					//printf("user Found, rangeSetInd=%i\n", rangeSetInd);
					break;
				}
			}

			for (MYBYTE i = 0; i < MAX_RANGE_FILTERS && req->subnetIP && rangeSet->subnetIP[i]; i++)
			{
				if(req->subnetIP == rangeSet->subnetIP[i])
				{
					rangeData.subnetArray[rangeSetInd] = 1;
					rangeData.subnetFound = true;
					//printf("subnet Found, rangeSetInd=%i\n", rangeSetInd);
					break;
				}
			}
		}

	}

//	printArray("macArray", (char*)cfig.macArray);
//	printArray("vendArray", (char*)cfig.vendArray);
//	printArray("userArray", (char*)cfig.userArray);

	if (!iipNew && req->reqIP && checkIP(req, &rangeData, req->reqIP))
		iipNew = ntohl(req->reqIP);

	if (!iipNew && req->dhcpEntry && req->dhcpEntry->ip && checkIP(req, &rangeData, req->dhcpEntry->ip))
		iipNew = ntohl(req->dhcpEntry->ip);

	for (char k = 0; !iipNew && k < cfig.rangeCount; k++)
	{
		data13 *range = &cfig.dhcpRanges[k];

		if (checkRange(&rangeData, k))
		{
			if (!cfig.rangeSet[range->rangeSetInd].subnetIP[0])
			{
				if (req->subnetIP)
					calcRangeLimits(req->subnetIP, range->mask, &rangeStart, &rangeEnd);
				else
					calcRangeLimits(network.dhcpConn[req->sockInd].server, network.dhcpConn[req->sockInd].mask, &rangeStart, &rangeEnd);

				if (rangeStart < range->rangeStart)
					rangeStart = range->rangeStart;

				if (rangeEnd > range->rangeEnd)
					rangeEnd = range->rangeEnd;
			}
			else
			{
				rangeStart = range->rangeStart;
				rangeEnd = range->rangeEnd;
			}

			if (rangeStart <= rangeEnd)
			{
				//sprintf(logBuff, "Start=%u End=%u", rangeStart, rangeEnd);
				//logMess(logBuff, 1);
				rangeFound = true;

				if (cfig.replication == 2)
				{
					for (MYDWORD m = rangeEnd; m >= rangeStart; m--)
					{
						int ind = m - range->rangeStart;

						if (!range->expiry[ind])
						{
							iipNew = m;
							break;
						}
						else if (!iipExp && range->expiry[ind] < t)
						{
							iipExp = m;
						}
					}
				}
				else
				{
					for (MYDWORD m = rangeStart; m <= rangeEnd; m++)
					{
						int ind = m - range->rangeStart;

						//sprintf(logBuff, "Ind=%u Exp=%u", m, range->expiry[ind]);
						//logMess(logBuff, 1);

						if (!range->expiry[ind])
						{
							iipNew = m;
							break;
						}
						else if (!iipExp && range->expiry[ind] < t)
						{
							iipExp = m;
						}
					}
				}
			}
		}
	}

	//sprintf(logBuff, "New=%u Old=%u", iipNew, iipExp);
	//logMess(logBuff, 1);

	if (!iipNew && iipExp)
			iipNew = iipExp;

	if (iipNew)
	{
		if (!req->dhcpEntry)
		{
			memset(&lump, 0, sizeof(data71));
			lump.mapname = req->chaddr;
			lump.hostname = req->hostname;
			req->dhcpEntry = createCache(&lump);

			if (!req->dhcpEntry)
				return 0;

			dhcpCache[req->dhcpEntry->mapname] = req->dhcpEntry;
		}

		req->dhcpEntry->ip = htonl(iipNew);
		req->dhcpEntry->rangeInd = getRangeInd(req->dhcpEntry->ip);
		req->dhcpEntry->subnetFlg = !!req->subnetIP;
		setTempLease(req->dhcpEntry);
		return req->dhcpEntry->ip;
	}

	if (verbatim || cfig.dhcpLogLevel)
	{
		if (rangeFound)
		{
			if (req->dhcpp.header.bp_giaddr)
				sprintf(logBuff, "No free leases for DHCPDISCOVER for %s (%s) from RelayAgent %s", req->chaddr, req->hostname, IP2String(tempbuff, req->dhcpp.header.bp_giaddr));
			else
				sprintf(logBuff, "No free leases for DHCPDISCOVER for %s (%s) from interface %s", req->chaddr, req->hostname, IP2String(tempbuff, network.dhcpConn[req->sockInd].server));
		}
		else
		{
			if (req->dhcpp.header.bp_giaddr)
				sprintf(logBuff, "No Matching DHCP Range for DHCPDISCOVER for %s (%s) from RelayAgent %s", req->chaddr, req->hostname, IP2String(tempbuff, req->dhcpp.header.bp_giaddr));
			else
				sprintf(logBuff, "No Matching DHCP Range for DHCPDISCOVER for %s (%s) from interface %s", req->chaddr, req->hostname, IP2String(tempbuff, network.dhcpConn[req->sockInd].server));
		}
		logDHCPMess(logBuff, 1);
	}
	return 0;
}

MYDWORD chkaddr(data9 *req)
{
	//debug("chaddr");
	//debug(req->remote.sin_addr.s_addr);
	//debug(req->dhcpp.header.bp_ciaddr);

	req->dhcpEntry = findDHCPEntry(req->chaddr);

	if (!req->dhcpEntry || !req->dhcpEntry->ip)
		return 0;

	//debug(req->dhcpEntry->subnetFlg);

	req->dhcpEntry->rangeInd = getRangeInd(req->dhcpEntry->ip);

	if (req->dhcpEntry->fixed)
		return req->dhcpEntry->ip;

	MYDWORD rangeStart = 0;
	MYDWORD rangeEnd = 0;

	if (req->dhcpEntry->rangeInd >= 0)
	{
		data17 rangeData;
		memset(&rangeData, 0, sizeof(data17));
		data13 *range = &cfig.dhcpRanges[req->dhcpEntry->rangeInd];
		int ind = getIndex(req->dhcpEntry->rangeInd, req->dhcpEntry->ip);
		bool rangeOK = true;

		if (cfig.hasFilter)
		{
			for (MYBYTE rangeSetInd = 0; rangeSetInd < MAX_RANGE_SETS && cfig.rangeSet[rangeSetInd].active; rangeSetInd++)
			{
				data14 *rangeSet = &cfig.rangeSet[rangeSetInd];

				for (MYBYTE i = 0; i < MAX_RANGE_FILTERS && rangeSet->macSize[i]; i++)
				{
					//printf("%s\n", hex2String(tempbuff, rangeSet->macStart[i], rangeSet->macSize[i]));
					//printf("%s\n", hex2String(tempbuff, rangeSet->macEnd[i], rangeSet->macSize[i]));

					if(memcmp(req->dhcpp.header.bp_chaddr, rangeSet->macStart[i], rangeSet->macSize[i]) >= 0 && memcmp(req->dhcpp.header.bp_chaddr, rangeSet->macEnd[i], rangeSet->macSize[i]) <= 0)
					{
						rangeData.macArray[rangeSetInd] = 1;
						rangeData.macFound = true;
						//printf("mac Found, rangeSetInd=%i\n", rangeSetInd);
						break;
					}
				}

				for (MYBYTE i = 0; i < MAX_RANGE_FILTERS && req->vendClass.size && rangeSet->vendClassSize[i]; i++)
				{
					if(rangeSet->vendClassSize[i] == req->vendClass.size && !memcmp(req->vendClass.value, rangeSet->vendClass[i], rangeSet->vendClassSize[i]))
					{
						rangeData.vendArray[rangeSetInd] = 1;
						rangeData.vendFound = true;
						//printf("vend Found, rangeSetInd=%i\n", rangeSetInd);
						break;
					}
				}

				for (MYBYTE i = 0; i < MAX_RANGE_FILTERS && req->userClass.size && rangeSet->userClassSize[i]; i++)
				{
					if(rangeSet->userClassSize[i] == req->userClass.size && !memcmp(req->userClass.value, rangeSet->userClass[i], rangeSet->userClassSize[i]))
					{
						rangeData.userArray[rangeSetInd] = 1;
						rangeData.userFound = true;
						//printf("user Found, rangeSetInd=%i\n", rangeSetInd);
						break;
					}
				}

				for (MYBYTE i = 0; i < MAX_RANGE_FILTERS && req->subnetIP && rangeSet->subnetIP[i]; i++)
				{
					if(req->subnetIP == rangeSet->subnetIP[i])
					{
						rangeData.subnetArray[rangeSetInd] = 1;
						rangeData.subnetFound = true;
						//printf("subnet Found, rangeSetInd=%i\n", rangeSetInd);
						break;
					}
				}
			}

			MYBYTE rangeSetInd = range->rangeSetInd;
			data14 *rangeSet = &cfig.rangeSet[rangeSetInd];
			rangeOK = false;

			if((!rangeData.macFound && !rangeSet->macSize[0]) || (rangeData.macFound && rangeData.macArray[rangeSetInd]))
				if((!rangeData.vendFound && !rangeSet->vendClassSize[0]) || (rangeData.vendFound && rangeData.vendArray[rangeSetInd]))
					if((!rangeData.userFound && !rangeSet->userClassSize[0]) || (rangeData.userFound && rangeData.userArray[rangeSetInd]))
						rangeOK = true;
		}

		if (range->dhcpEntry[ind] == req->dhcpEntry && rangeOK)
		{
			if(rangeData.subnetFound)
			{
				if (rangeData.subnetArray[range->rangeSetInd])
					return req->dhcpEntry->ip;
				else
					return 0;
			}
			else if(req->subnetIP)
			{
				calcRangeLimits(req->subnetIP, range->mask, &rangeStart, &rangeEnd);

				if (rangeStart < range->rangeStart)
					rangeStart = range->rangeStart;

				if (rangeEnd > range->rangeEnd)
					rangeEnd = range->rangeEnd;

				if (htonl(req->dhcpEntry->ip) >= rangeStart && htonl(req->dhcpEntry->ip) <= rangeEnd)
					return req->dhcpEntry->ip;
			}
			else if(!req->dhcpEntry->subnetFlg && !cfig.rangeSet[range->rangeSetInd].subnetIP[0])
			{
				calcRangeLimits(network.dhcpConn[req->sockInd].server, range->mask, &rangeStart, &rangeEnd);

				if (rangeStart < range->rangeStart)
					rangeStart = range->rangeStart;

				if (rangeEnd > range->rangeEnd)
					rangeEnd = range->rangeEnd;

				if (htonl(req->dhcpEntry->ip) >= rangeStart && htonl(req->dhcpEntry->ip) <= rangeEnd)
					return req->dhcpEntry->ip;
			}
			else if(req->dhcpEntry->subnetFlg)
				return req->dhcpEntry->ip;
		}
	}

	return 0;
}

MYDWORD sdmess(data9 *req)
{
	//sprintf(logBuff, "sdmess, Request Type = %u",req->req_type);
	//debug(logBuff);
	char tempbuff[512];
	char logBuff[256];

	if (req->req_type == DHCP_MESS_NONE)
	{
		req->dhcpp.header.bp_yiaddr = chkaddr(req);

		if (req->dhcpp.header.bp_yiaddr && req->dhcpEntry && req->dhcpEntry->fixed)
			req->lease = UINT_MAX;
		else
		{
			if (verbatim || cfig.dhcpLogLevel)
			{
				sprintf(logBuff, "No Static Entry found for BOOTPREQUEST from Host %s", req->chaddr);
				logDHCPMess(logBuff, 1);
			}

			return 0;
		}
	}
	else if (req->req_type == DHCP_MESS_DECLINE)
	{
		/* Thanks to Timo for fixing issue here */
		if (req->reqIP && chkaddr(req) == req->reqIP)
		{
			lockIP(req->reqIP);
			req->dhcpEntry->ip = 0;
			req->dhcpEntry->expiry = INT_MAX;
			req->dhcpEntry->display = false;
			req->dhcpEntry->local = false;

			if (verbatim || cfig.dhcpLogLevel)
			{
				sprintf(logBuff, "IP Address %s declined by Host %s (%s), locked", IP2String(tempbuff, req->reqIP), req->chaddr, req->dhcpEntry->hostname);
				logDHCPMess(logBuff, 1);
			}
		}

		return 0;
	}
	else if (req->req_type == DHCP_MESS_RELEASE)
	{
		if (req->dhcpp.header.bp_ciaddr && chkaddr(req) == req->dhcpp.header.bp_ciaddr)
		{
			req->dhcpEntry->display = false;
			req->dhcpEntry->local = false;
			setLeaseExpiry(req->dhcpEntry, 0);

			_beginthread(updateStateFile, 0, (void*)req->dhcpEntry);

			if (verbatim || cfig.dhcpLogLevel)
			{
				sprintf(logBuff, "IP Address %s released by Host %s (%s)", IP2String(tempbuff, req->dhcpp.header.bp_ciaddr), req->chaddr, req->hostname);
				logDHCPMess(logBuff, 1);
			}
		}

		return 0;
	}
	else if (req->req_type == DHCP_MESS_INFORM)
	{
		//printf("repl0=%s\n", IP2String(tempbuff, cfig.zoneServers[0]));
		//printf("repl1=%s\n", IP2String(tempbuff, cfig.zoneServers[1]));
		//printf("IP=%s bytes=%u replication=%i\n", IP2String(tempbuff, req->remote.sin_addr.s_addr), req->bytes, cfig.replication);

		if ((cfig.replication == 1 && req->remote.sin_addr.s_addr == cfig.zoneServers[1]) || (cfig.replication == 2 && req->remote.sin_addr.s_addr == cfig.zoneServers[0]))
			recvRepl(req);

		return 0;
	}
	//else if (req->req_type == DHCP_MESS_DISCOVER && strcasecmp(req->hostname, cfig.servername))
	else if (req->req_type == DHCP_MESS_DISCOVER)
	{
		req->dhcpp.header.bp_yiaddr = resad(req);

		if (!req->dhcpp.header.bp_yiaddr)
			return 0;

		req->resp_type = DHCP_MESS_OFFER;
	}
	else if (req->req_type == DHCP_MESS_REQUEST)
	{
		//printf("%s\n", IP2String(tempbuff, req->dhcpp.header.bp_ciaddr));

		if (req->server)
		{
			if (req->server == network.dhcpConn[req->sockInd].server)
			{
				if (req->reqIP && req->reqIP == chkaddr(req) && req->dhcpEntry->expiry > t)
				{
					req->resp_type = DHCP_MESS_ACK;
					req->dhcpp.header.bp_yiaddr = req->reqIP;
				}
				else if (req->dhcpp.header.bp_ciaddr && req->dhcpp.header.bp_ciaddr == chkaddr(req) && req->dhcpEntry->expiry > t)
				{
					req->resp_type = DHCP_MESS_ACK;
					req->dhcpp.header.bp_yiaddr = req->dhcpp.header.bp_ciaddr;
					req->dhcpp.header.bp_ciaddr = 0;
				}
				else
				{
					req->resp_type = DHCP_MESS_NAK;
					req->dhcpp.header.bp_yiaddr = 0;

					if (verbatim || cfig.dhcpLogLevel)
					{
						sprintf(logBuff, "DHCPREQUEST from Host %s (%s) without Discover, NAKed", req->chaddr, req->hostname);
						logDHCPMess(logBuff, 1);
					}
				}
			}
			else
				return 0;
		}
		else if (req->dhcpp.header.bp_ciaddr && req->dhcpp.header.bp_ciaddr == chkaddr(req) && req->dhcpEntry->expiry > t)
		{
			req->resp_type = DHCP_MESS_ACK;
			req->dhcpp.header.bp_yiaddr = req->dhcpp.header.bp_ciaddr;
			req->dhcpp.header.bp_ciaddr = 0;
		}
		else if (req->reqIP && req->reqIP == chkaddr(req) && req->dhcpEntry->expiry > t)
		{
			req->resp_type = DHCP_MESS_ACK;
			req->dhcpp.header.bp_yiaddr = req->reqIP;
		}
		else
		{
			req->resp_type = DHCP_MESS_NAK;
			req->dhcpp.header.bp_yiaddr = 0;

			if (verbatim || cfig.dhcpLogLevel)
			{
				sprintf(logBuff, "DHCPREQUEST from Host %s (%s) without Discover, NAKed", req->chaddr, req->hostname);
				logDHCPMess(logBuff, 1);
			}
		}
	}
	else
		return 0;

	addOptions(req);
	int packSize = req->vp - (MYBYTE*)&req->dhcpp;
	packSize++;

	if (req->req_type == DHCP_MESS_NONE)
		packSize = req->messsize;

	//debug(req->dhcpEntry->rangeInd);
	//debug(cfig.dhcpRanges[req->dhcpEntry->rangeInd].rangeSetInd);

	if (req->subnetIP && req->dhcpEntry && req->dhcpEntry->rangeInd >= 0)
	{
		MYBYTE rangeSetInd = cfig.dhcpRanges[req->dhcpEntry->rangeInd].rangeSetInd;
		req->targetIP = cfig.rangeSet[rangeSetInd].targetIP;
	}

	if (req->targetIP)
	{
		req->remote.sin_port = htons(IPPORT_DHCPS);
		req->remote.sin_addr.s_addr = req->targetIP;
	}
	else if (req->dhcpp.header.bp_giaddr)
	{
		req->remote.sin_port = htons(IPPORT_DHCPS);
		req->remote.sin_addr.s_addr = req->dhcpp.header.bp_giaddr;
	}
	else if (req->dhcpp.header.bp_broadcast || !req->remote.sin_addr.s_addr)
	{
		req->remote.sin_port = htons(IPPORT_DHCPC);
		req->remote.sin_addr.s_addr = INADDR_BROADCAST;
	}
	else
	{
		req->remote.sin_port = htons(IPPORT_DHCPC);
	}

	req->dhcpp.header.bp_op = BOOTP_REPLY;
	errno = 0;

	if (req->req_type == DHCP_MESS_DISCOVER && !req->remote.sin_addr.s_addr)
	{
		req->bytes = sendto(network.dhcpConn[req->sockInd].sock,
							req->raw,
							packSize,
							MSG_DONTROUTE,
							(sockaddr*)&req->remote,
							sizeof(req->remote));
	}
	else
	{
		req->bytes = sendto(network.dhcpConn[req->sockInd].sock,
							req->raw,
							packSize,
							0,
							(sockaddr*)&req->remote,
							sizeof(req->remote));
	}

	if (errno || req->bytes <= 0)
		return 0;

	//printf("goes=%s %i\n",IP2String(tempbuff, req->dhcpp.header.bp_yiaddr),req->sockInd);
	return req->dhcpp.header.bp_yiaddr;
}

MYDWORD alad(data9 *req)
{
	//debug("alad");
	//printf("in alad hostname=%s\n", req->hostname);
	char tempbuff[512];
	char logBuff[256];

	if (req->dhcpEntry && (req->req_type == DHCP_MESS_NONE || req->resp_type == DHCP_MESS_ACK))
	{
		MYDWORD hangTime = req->lease;

		if (req->rebind > req->lease)
			hangTime = req->rebind;

		req->dhcpEntry->display = true;
		req->dhcpEntry->local = true;
		setLeaseExpiry(req->dhcpEntry, hangTime);

		_beginthread(updateStateFile, 0, (void*)req->dhcpEntry);

		if (verbatim || cfig.dhcpLogLevel >= 1)
		{
			if (req->lease && req->reqIP)
			{
				sprintf(logBuff, "Host %s (%s) allotted %s for %u seconds", req->chaddr, req->hostname, IP2String(tempbuff, req->dhcpp.header.bp_yiaddr), req->lease);
			}
			else if (req->req_type)
			{
				sprintf(logBuff, "Host %s (%s) renewed %s for %u seconds", req->chaddr, req->hostname, IP2String(tempbuff, req->dhcpp.header.bp_yiaddr), req->lease);
			}
			else
			{
				sprintf(logBuff, "BOOTP Host %s (%s) allotted %s", req->chaddr, req->hostname, IP2String(tempbuff, req->dhcpp.header.bp_yiaddr));
			}
			logDHCPMess(logBuff, 1);
		}

		if (cfig.replication && cfig.dhcpRepl > t)
			sendRepl(req);

		return req->dhcpEntry->ip;
	}
	else if ((verbatim || cfig.dhcpLogLevel >= 2) && req->resp_type == DHCP_MESS_OFFER)
	{
		sprintf(logBuff, "Host %s (%s) offered %s", req->chaddr, req->hostname, IP2String(tempbuff, req->dhcpp.header.bp_yiaddr));
		logDHCPMess(logBuff, 2);
	}
	//printf("%u=out\n", req->resp_type);
	return 0;
}

void addOptions(data9 *req)
{
	//debug("addOptions");

	data3 op;
	int i;

	if (req->req_type && req->resp_type)
	{
		op.opt_code = DHCP_OPTION_MESSAGETYPE;
		op.size = 1;
		op.value[0] = req->resp_type;
		pvdata(req, &op);
	}

	if (req->dhcpEntry && req->resp_type != DHCP_MESS_DECLINE && req->resp_type != DHCP_MESS_NAK)
	{
		strcpy(req->dhcpp.header.bp_sname, cfig.servername);

		if (req->dhcpEntry->fixed)
		{
			//printf("%u,%u\n", req->dhcpEntry->options, *req->dhcpEntry->options);
			MYBYTE *opPointer = req->dhcpEntry->options;

			if (opPointer)
			{
				MYBYTE requestedOnly = *opPointer;
				opPointer++;

				while (*opPointer && *opPointer != DHCP_OPTION_END)
				{
					op.opt_code = *opPointer;
					opPointer++;
					op.size = *opPointer;
					opPointer++;

					if (!requestedOnly || req->paramreqlist[*opPointer])
					{
						memcpy(op.value, opPointer, op.size);
						pvdata(req, &op);
					}
					opPointer += op.size;
				}
			}
		}

		if (req->req_type && req->resp_type)
		{
			if (req->dhcpEntry->rangeInd >= 0)
			{
				MYBYTE *opPointer = cfig.dhcpRanges[req->dhcpEntry->rangeInd].options;
				//printf("Range=%i Pointer=%u\n", req->dhcpEntry->rangeInd,opPointer);

				if (opPointer)
				{
					MYBYTE requestedOnly = *opPointer;
					opPointer++;

					while (*opPointer && *opPointer != DHCP_OPTION_END)
					{
						op.opt_code = *opPointer;
						opPointer++;
						op.size = *opPointer;
						opPointer++;

						if (!requestedOnly || req->paramreqlist[*opPointer])
						{
							memcpy(op.value, opPointer, op.size);
							pvdata(req, &op);
						}
						opPointer += op.size;
					}
				}
			}

			MYBYTE *opPointer = cfig.options;

			if (opPointer)
			{
				MYBYTE requestedOnly = *opPointer;

				opPointer++;
				while (*opPointer && *opPointer != DHCP_OPTION_END)
				{
					op.opt_code = *opPointer;
					opPointer++;
					op.size = *opPointer;
					opPointer++;

					if (!requestedOnly || req->paramreqlist[*opPointer])
					{
						memcpy(op.value, opPointer, op.size);
						pvdata(req, &op);
					}
					opPointer += op.size;
				}
			}

			op.opt_code = DHCP_OPTION_SERVERID;
			op.size = 4;
			pIP(op.value, network.dhcpConn[req->sockInd].server);
			pvdata(req, &op);

			if (!req->opAdded[DHCP_OPTION_IPADDRLEASE])
			{
				op.opt_code = DHCP_OPTION_IPADDRLEASE;
				op.size = 4;
				pULong(op.value, cfig.lease);
				pvdata(req, &op);
			}

			if (!req->opAdded[DHCP_OPTION_NETMASK])
			{
				op.opt_code = DHCP_OPTION_NETMASK;
				op.size = 4;

				if (req->dhcpEntry->rangeInd >= 0)
					pIP(op.value, cfig.dhcpRanges[req->dhcpEntry->rangeInd].mask);
				else
					pIP(op.value, cfig.mask);

				pvdata(req, &op);
			}

/*
			if (!req->opAdded[DHCP_OPTION_ROUTER])
			{
				op.opt_code = DHCP_OPTION_ROUTER;
				op.size = 4;
				pIP(op.value, network.dhcpConn[req->sockInd].server);
				pvdata(req, &op);
			}

			if (req->clientId.opt_code == DHCP_OPTION_CLIENTID)
				pvdata(req, &req->clientId);
*/
			if (req->subnet.opt_code == DHCP_OPTION_SUBNETSELECTION)
				pvdata(req, &req->subnet);

			if (req->agentOption.opt_code == DHCP_OPTION_RELAYAGENTINFO)
				pvdata(req, &req->agentOption);
		}

		if (req->hostname[0])
			strcpy(req->dhcpEntry->hostname, req->hostname);
		else if (req->dhcpEntry->hostname[0])
			strcpy(req->hostname, req->dhcpEntry->hostname);
		else
		{
			genHostName(req->hostname, req->dhcpp.header.bp_chaddr, req->dhcpp.header.bp_hlen);
			strcpy(req->dhcpEntry->hostname, req->hostname);
		}
	}

	*(req->vp) = DHCP_OPTION_END;
}

void pvdata(data9 *req, data3 *op)
{
	//debug("pvdata");
	MYBYTE opsize = op->size;

	if (!req->opAdded[op->opt_code] && ((req->vp - (MYBYTE*)&req->dhcpp) + opsize < req->messsize))
	{
		if (op->opt_code == DHCP_OPTION_NEXTSERVER)
			req->dhcpp.header.bp_siaddr = fIP(op->value);
		else if (op->opt_code == DHCP_OPTION_BP_FILE)
		{
			if (opsize <= 128)
				memcpy(req->dhcpp.header.bp_file, op->value, opsize);
		}
		else if(opsize)
		{
			if (op->opt_code == DHCP_OPTION_IPADDRLEASE)
			{
				if (!req->lease || req->lease > fULong(op->value))
					req->lease = fULong(op->value);

				if (req->lease >= INT_MAX)
					req->lease = UINT_MAX;

				pULong(op->value, req->lease);
			}
			else if (op->opt_code == DHCP_OPTION_REBINDINGTIME)
				req->rebind = fULong(op->value);
			else if (op->opt_code == DHCP_OPTION_HOSTNAME)
			{
				memcpy(req->hostname, op->value, opsize);
				req->hostname[opsize] = 0;
				req->hostname[64] = 0;

				if (char *ptr = strchr(req->hostname, '.'))
					*ptr = 0;

				opsize = strlen(req->hostname) + 1;
				memcpy(op->value, req->hostname, opsize);
			}

			opsize += 2;
			memcpy(req->vp, op, opsize);
			(req->vp) += opsize;
		}
		req->opAdded[op->opt_code] = true;
	}
}

void setTempLease(data7 *dhcpEntry)
{
	if (dhcpEntry && dhcpEntry->ip)
	{
		dhcpEntry->display = false;
		dhcpEntry->local = false;
		dhcpEntry->expiry = t + 20;

		int ind = getIndex(dhcpEntry->rangeInd, dhcpEntry->ip);

		if (ind >= 0)
		{
			if (cfig.dhcpRanges[dhcpEntry->rangeInd].expiry[ind] != INT_MAX)
				cfig.dhcpRanges[dhcpEntry->rangeInd].expiry[ind] = dhcpEntry->expiry;

			cfig.dhcpRanges[dhcpEntry->rangeInd].dhcpEntry[ind] = dhcpEntry;
		}
	}
}

void setLeaseExpiry(data7 *dhcpEntry, MYDWORD lease)
{
	//printf("%d=%d\n", t, lease);
	if (dhcpEntry && dhcpEntry->ip)
	{
		if (lease > (MYDWORD)(INT_MAX - t))
			dhcpEntry->expiry = INT_MAX;
		else
			dhcpEntry->expiry = t + lease;

		int ind = getIndex(dhcpEntry->rangeInd, dhcpEntry->ip);

		if (ind >= 0)
		{
			if (cfig.dhcpRanges[dhcpEntry->rangeInd].expiry[ind] != INT_MAX)
				cfig.dhcpRanges[dhcpEntry->rangeInd].expiry[ind] = dhcpEntry->expiry;

			cfig.dhcpRanges[dhcpEntry->rangeInd].dhcpEntry[ind] = dhcpEntry;
		}
	}
}

void setLeaseExpiry(data7 *dhcpEntry)
{
	if (dhcpEntry && dhcpEntry->ip)
	{
		int ind = getIndex(dhcpEntry->rangeInd, dhcpEntry->ip);

		if (ind >= 0)
		{
			if (cfig.dhcpRanges[dhcpEntry->rangeInd].expiry[ind] != INT_MAX)
				cfig.dhcpRanges[dhcpEntry->rangeInd].expiry[ind] = dhcpEntry->expiry;

			cfig.dhcpRanges[dhcpEntry->rangeInd].dhcpEntry[ind] = dhcpEntry;
		}
	}
}


void lockIP(MYDWORD ip)
{
	MYDWORD iip = htonl(ip);

	for (char rangeInd = 0; rangeInd < cfig.rangeCount; rangeInd++)
	{
		if (iip >= cfig.dhcpRanges[rangeInd].rangeStart && iip <= cfig.dhcpRanges[rangeInd].rangeEnd)
		{
			int ind = iip - cfig.dhcpRanges[rangeInd].rangeStart;

			if (cfig.dhcpRanges[rangeInd].expiry[ind] != INT_MAX)
				cfig.dhcpRanges[rangeInd].expiry[ind] = INT_MAX;

			break;
		}
	}
}

void holdIP(MYDWORD ip)
{
	if (ip)
	{
		MYDWORD iip = htonl(ip);

		for (char rangeInd = 0; rangeInd < cfig.rangeCount; rangeInd++)
		{
			if (iip >= cfig.dhcpRanges[rangeInd].rangeStart && iip <= cfig.dhcpRanges[rangeInd].rangeEnd)
			{
				int ind = iip - cfig.dhcpRanges[rangeInd].rangeStart;

				if (cfig.dhcpRanges[rangeInd].expiry[ind] == 0)
					cfig.dhcpRanges[rangeInd].expiry[ind] = 1;

				break;
			}
		}
	}
}

void __cdecl sendToken(void *lpParam)
{
	//debug("Send Token");
	Sleep(1000 * 10);

	while (kRunning)
	{
		errno = 0;

		sendto(cfig.dhcpReplConn.sock,
				token.raw,
				token.bytes,
				0,
				(sockaddr*)&token.remote,
				sizeof(token.remote));

		//errno = WSAGetLastError();
		//debug(errno);
		//debug("Token Sent");

		Sleep(1000 * 300);
	}

	_endthread();
	return;
}


MYDWORD sendRepl(data9 *req)
{
	char logBuff[512];
	char ipbuff[32];
	data3 op;

	MYBYTE *opPointer = req->dhcpp.vend_data;

	while ((*opPointer) != DHCP_OPTION_END && opPointer < req->vp)
	{
		if ((*opPointer) == DHCP_OPTION_MESSAGETYPE)
		{
			*(opPointer + 2) = DHCP_MESS_INFORM;
			break;
		}
		opPointer = opPointer + *(opPointer + 1) + 2;
	}

	if (!req->opAdded[DHCP_OPTION_MESSAGETYPE])
	{
		op.opt_code = DHCP_OPTION_MESSAGETYPE;
		op.size = 1;
		op.value[0] = DHCP_MESS_INFORM;
		pvdata(req, &op);
	}

	if (req->hostname[0] && !req->opAdded[DHCP_OPTION_HOSTNAME])
	{
		op.opt_code = DHCP_OPTION_HOSTNAME;
		op.size = strlen(req->hostname) + 1;
		memcpy(op.value, req->hostname, op.size);
		pvdata(req, &op);
	}

//	op.opt_code = DHCP_OPTION_SERIAL;
//	op.size = 4;
//	pULong(op.value, cfig.serial1);
//	pvdata(req, &op);

	*(req->vp) = DHCP_OPTION_END;
	req->vp++;
	req->bytes = req->vp - (MYBYTE*)req->raw;

	req->dhcpp.header.bp_op = BOOTP_REQUEST;
	errno = 0;

	req->bytes = sendto(cfig.dhcpReplConn.sock,
	                    req->raw,
	                    req->bytes,
	                    0,
						(sockaddr*)&token.remote,
						sizeof(token.remote));

	errno = WSAGetLastError();

	if (errno || req->bytes <= 0)
	{
		cfig.dhcpRepl = 0;

		if (verbatim || cfig.dhcpLogLevel >= 1)
		{
			if (cfig.replication == 1)
				sprintf(logBuff, "WSAError %i Sending DHCP Update to Secondary Server", errno);
			else
				sprintf(logBuff, "WSAError %i Sending DHCP Update to Primary Server", errno);

			logDHCPMess(logBuff, 1);
		}

		return 0;
	}
	else if (verbatim || cfig.dhcpLogLevel >= 2)
	{
		if (cfig.replication == 1)
			sprintf(logBuff, "DHCP Update for host %s (%s) sent to Secondary Server", req->dhcpEntry->mapname, IP2String(ipbuff, req->dhcpEntry->ip));
		else
			sprintf(logBuff, "DHCP Update for host %s (%s) sent to Primary Server", req->dhcpEntry->mapname, IP2String(ipbuff, req->dhcpEntry->ip));

		logDHCPMess(logBuff, 2);
	}

	return req->dhcpp.header.bp_yiaddr;
}

/*
MYDWORD sendRepl(data7 *dhcpEntry)
{
	data9 req;
	memset(&req, 0, sizeof(data9));
	req.vp = req.dhcpp.vend_data;
	req.messsize = sizeof(dhcp_packet);
	req.dhcpEntry = dhcpEntry;

	req.dhcpp.header.bp_op = BOOTP_REQUEST;
	req.dhcpp.header.bp_xid = t;
	req.dhcpp.header.bp_ciaddr = dhcpEntry->ip;
	req.dhcpp.header.bp_yiaddr = dhcpEntry->ip;
	req.dhcpp.header.bp_hlen = 16;
	getHexValue(req.dhcpp.header.bp_chaddr, req.dhcpEntry->mapname, &(req.dhcpp.header.bp_hlen));
	req.dhcpp.header.bp_magic_num[0] = 99;
	req.dhcpp.header.bp_magic_num[1] = 130;
	req.dhcpp.header.bp_magic_num[2] = 83;
	req.dhcpp.header.bp_magic_num[3] = 99;
	strcpy(req.hostname, dhcpEntry->hostname);

	return sendRepl(&req);
}
*/

void recvRepl(data9 *req)
{
	char ipbuff[32];
	char logBuff[512];
	cfig.dhcpRepl = t + 650;

	MYDWORD ip = req->dhcpp.header.bp_yiaddr ? req->dhcpp.header.bp_yiaddr : req->dhcpp.header.bp_ciaddr;

	if (!ip || !req->dhcpp.header.bp_hlen)
	{
//		if (verbatim || cfig.dhcpLogLevel >= 2)
//		{
//			sprintf(logBuff, "Token Received");
//			logDHCPMess(logBuff, 2);
//		}

		if (cfig.replication == 1)
		{
			errno = 0;

			sendto(cfig.dhcpReplConn.sock,
					token.raw,
					token.bytes,
					0,
					(sockaddr*)&token.remote,
					sizeof(token.remote));

//			errno = WSAGetLastError();
//
//			if (!errno && (verbatim || cfig.dhcpLogLevel >= 2))
//			{
//				sprintf(logBuff, "Token Responded");
//				logDHCPMess(logBuff, 2);
//			}
		}

		return;
	}

	char rInd = getRangeInd(ip);
	req->dhcpEntry = findDHCPEntry(req->chaddr);

	if (req->dhcpEntry && req->dhcpEntry->ip != ip)
	{
		if (req->dhcpEntry->fixed || rInd < 0)
		{
			if (cfig.replication == 1)
				sprintf(logBuff, "DHCP Update ignored for %s (%s) from Secondary Server", req->chaddr, IP2String(ipbuff, ip));
			else
				sprintf(logBuff, "DHCP Update ignored for %s (%s) from Primary Server", req->chaddr, IP2String(ipbuff, ip));

			logDHCPMess(logBuff, 1);
			return;
		}
		else if (req->dhcpEntry->rangeInd >= 0)
		{
			int ind  = getIndex(req->dhcpEntry->rangeInd, req->dhcpEntry->ip);

			if (ind >= 0)
				cfig.dhcpRanges[req->dhcpEntry->rangeInd].dhcpEntry[ind] = 0;
		}
	}

	if (!req->dhcpEntry && rInd >= 0)
	{
		memset(&lump, 0, sizeof(data71));
		lump.mapname = req->chaddr;
		lump.hostname = req->hostname;
		req->dhcpEntry = createCache(&lump);

		if (req->dhcpEntry)
			dhcpCache[req->dhcpEntry->mapname] = req->dhcpEntry;
/*
		req->dhcpEntry = (data7*)calloc(1, sizeof(data7));

		if (!req->dhcpEntry)
		{
			sprintf(logBuff, "Memory Allocation Error");
			logDHCPMess(logBuff, 1);
			return;
		}

		req->dhcpEntry->mapname = cloneString(req->chaddr);

		if (!req->dhcpEntry->mapname)
		{
			sprintf(logBuff, "Memory Allocation Error");
			free(req->dhcpEntry);
			logDHCPMess(logBuff, 1);
			return;
		}
*/
	}

	if (req->dhcpEntry)
	{
		req->dhcpEntry->ip = ip;
		req->dhcpEntry->rangeInd = rInd;
		req->dhcpEntry->display = true;
		req->dhcpEntry->local = false;

		MYDWORD hangTime = req->lease;

		if (hangTime)
		{
			if (req->rebind > hangTime)
				hangTime = req->rebind;
		}
		else
			hangTime = UINT_MAX;

		setLeaseExpiry(req->dhcpEntry, hangTime);
		strcpy(req->dhcpEntry->hostname, req->hostname);

		_beginthread(updateStateFile, 0, (void*)req->dhcpEntry);

		if (verbatim || cfig.dhcpLogLevel >= 2)
		{
			if (cfig.replication == 1)
				sprintf(logBuff, "DHCP Update received for %s (%s) from Secondary Server", req->chaddr, IP2String(ipbuff, ip));
			else
				sprintf(logBuff, "DHCP Update received for %s (%s) from Primary Server", req->chaddr, IP2String(ipbuff, ip));

			logDHCPMess(logBuff, 2);
		}
	}
	else
	{
		if (cfig.replication == 1)
			sprintf(logBuff, "DHCP Update ignored for %s (%s) from Secondary Server", req->chaddr, IP2String(ipbuff, ip));
		else
			sprintf(logBuff, "DHCP Update ignored for %s (%s) from Primary Server", req->chaddr, IP2String(ipbuff, ip));

		logDHCPMess(logBuff, 1);
		return;
	}
}

char getRangeInd(MYDWORD ip)
{
	if (ip)
	{
		MYDWORD iip = htonl(ip);

		for (char k = 0; k < cfig.rangeCount; k++)
			if (iip >= cfig.dhcpRanges[k].rangeStart && iip <= cfig.dhcpRanges[k].rangeEnd)
				return k;
	}
	return -1;
}

int getIndex(char rangeInd, MYDWORD ip)
{
	if (ip && rangeInd >= 0 && rangeInd < cfig.rangeCount)
	{
		MYDWORD iip = htonl(ip);
		if (iip >= cfig.dhcpRanges[rangeInd].rangeStart && iip <= cfig.dhcpRanges[rangeInd].rangeEnd)
			return (iip - cfig.dhcpRanges[rangeInd].rangeStart);
	}
	return -1;
}

void loadOptions(FILE *f, const char *sectionName, data20 *optionData)
{
	optionData->ip = 0;
	optionData->mask = 0;
	MYBYTE maxInd = sizeof(opData) / sizeof(data4);
	MYWORD buffsize = sizeof(dhcp_packet) - sizeof(dhcp_header);
	MYBYTE *dp = optionData->options;
	MYBYTE op_specified[256];

	memset(op_specified, 0, 256);
	*dp = 0;
	dp++;

	char raw[512];
	char name[512];
	char value[512];
	char logBuff[512];

	while (readSection(raw, f))
	{
		MYBYTE *ddp = dp;
		MYBYTE hoption[256];
		MYBYTE valSize = sizeof(hoption) - 1;
		MYBYTE opTag = 0;
		MYBYTE opType = 0;
		MYBYTE valType = 0;
		bool tagFound = false;

		mySplit(name, value, raw, '=');

		//printf("%s=%s\n", name, value);

		if (!name[0])
		{
			sprintf(logBuff, "Warning: section [%s] invalid option %s ignored", sectionName, raw);
			logDHCPMess(logBuff, 1);
			continue;
		}

		if (!strcasecmp(name, "DHCPRange"))
		{
			if (!strcasecmp(sectionName, RANGESET))
				addDHCPRange(value);
			else
			{
				sprintf(logBuff, "Warning: section [%s] option %s not allowed in this section, option ignored", sectionName, raw);
				logDHCPMess(logBuff, 1);
			}
			continue;
		}
		else if (!strcasecmp(name, "IP"))
		{
			if (!strcasecmp(sectionName, GLOBALOPTIONS) || !strcasecmp(sectionName, RANGESET))
			{
				sprintf(logBuff, "Warning: section [%s] option %s not allowed in this section, option ignored", sectionName, raw);
				logDHCPMess(logBuff, 1);
			}
			else if (!isIP(value) && strcasecmp(value, "0.0.0.0"))
			{
				sprintf(logBuff, "Warning: section [%s] option Invalid IP Addr %s option ignored", sectionName, value);
				logDHCPMess(logBuff, 1);
			}
			else
				optionData->ip = inet_addr(value);

			continue;
		}
		else if (!strcasecmp(name, "FilterMacRange"))
		{
			if (!strcasecmp(sectionName, RANGESET))
				addMacRange(optionData->rangeSetInd, value);
			else
			{
				sprintf(logBuff, "Warning: section [%s] option %s not allowed in this section, option ignored", sectionName, raw);
				logDHCPMess(logBuff, 1);
			}
			continue;
		}

		if (!value[0])
			valType = 9;
		else if (value[0] == '"' && value[strlen(value)-1] == '"')
		{
			valType = 2;
			value[0] = NBSP;
			value[strlen(value) - 1] = NBSP;
			myTrim(value, value);

			if (strlen(value) <= UCHAR_MAX)
				valSize = strlen(value);
			else
			{
				sprintf(logBuff, "Warning: section [%s] option %s value too big, option ignored", sectionName, raw);
				logDHCPMess(logBuff, 1);
				continue;
			}
		}
		else if (strchr(value, ':'))
		{
			valType = 2;
			valSize = sizeof(hoption) - 1;
			char *errorPos = getHexValue(hoption, value, &valSize);

			if (errorPos)
			{
				valType = 1;
				valSize = strlen(value);
			}
			else
				memcpy(value, hoption, valSize);
		}
		else if (isInt(value) && atol(value) > USHRT_MAX)
			valType = 4;
		else if (isInt(value) && atoi(value) > UCHAR_MAX)
			valType = 5;
		else if (isInt(value))
			valType = 6;
		else if (strchr(value, '.') || strchr(value, ','))
		{
			valType = 2;
			char buff[1024];
			int numbytes = myTokenize(buff, value, "/,.", true);

			if (numbytes > 255)
			{
				sprintf(logBuff, "Warning: section [%s] option %s, too many bytes, entry ignored", sectionName, raw);
				logDHCPMess(logBuff, 1);
				continue;
			}
			else
			{
				char *ptr = buff;
				valSize = 0;

				for (; *ptr; ptr = myGetToken(ptr, 1))
				{
					//printf("%s:", ptr);
					if (isInt(ptr) && atoi(ptr) <= UCHAR_MAX)
					{
						hoption[valSize] = atoi(ptr);
						valSize++;
					}
					else
						break;
				}

				if (!(*ptr))
					memcpy(value, hoption, valSize);
				else
				{
					valType = 1;
					valSize = strlen(value);
				}
			}
		}
		else
		{
			if (strlen(value) <= UCHAR_MAX)
			{
				valSize = strlen(value);
				valType = 1;
			}
			else
			{
				sprintf(logBuff, "Warning: section [%s] option %s value too long, option ignored", sectionName, raw);
				logDHCPMess(logBuff, 1);
				continue;
			}
		}

		if (!strcasecmp(name, "FilterVendorClass"))
		{
			if (!strcasecmp(sectionName, RANGESET))
				addVendClass(optionData->rangeSetInd, value, valSize);
			else
			{
				sprintf(logBuff, "Warning: section [%s] option %s not allowed in this section, option ignored", sectionName, raw);
				logDHCPMess(logBuff, 1);
			}
			continue;
		}
		else if (!strcasecmp(name, "FilterUserClass"))
		{
			if (!strcasecmp(sectionName, RANGESET))
				addUserClass(optionData->rangeSetInd, value, valSize);
			else
			{
				sprintf(logBuff, "Warning: section [%s] option %s not allowed in this section, option ignored", sectionName, raw);
				logDHCPMess(logBuff, 1);
			}
			continue;
		}
		else if (!strcasecmp(name, "FilterSubnetSelection"))
		{
			if (valSize != 4)
			{
				sprintf(logBuff, "Warning: section [%s] invalid value %s, option ignored", sectionName, raw);
				logDHCPMess(logBuff, 1);
			}
			else if (!strcasecmp(sectionName, RANGESET))
			{
				addServer(cfig.rangeSet[optionData->rangeSetInd].subnetIP, MAX_RANGE_FILTERS, fIP(value));
				cfig.hasFilter = 1;
			}
			else
			{
				sprintf(logBuff, "Warning: section [%s] option %s not allowed in this section, option ignored", sectionName, raw);
				logDHCPMess(logBuff, 1);
			}
			continue;
		}
		else if (!strcasecmp(name, "TargetRelayAgent"))
		{
			if (valSize != 4)
			{
				sprintf(logBuff, "Warning: section [%s] invalid value %s, option ignored", sectionName, raw);
				logDHCPMess(logBuff, 1);
			}
			else if (!strcasecmp(sectionName, RANGESET))
			{
				cfig.rangeSet[optionData->rangeSetInd].targetIP = fIP(value);
				//printf("TARGET IP %s set RangeSetInd  %d\n", IP2String(ipbuff, cfig.rangeSet[optionData->rangeSetInd].targetIP), optionData->rangeSetInd);
			}
			else
			{
				sprintf(logBuff, "Warning: section [%s] option %s not allowed in this section, option ignored", sectionName, raw);
				logDHCPMess(logBuff, 1);
			}
			continue;
		}

		opTag = 0;

		if (isInt(name))
		{
			if (atoi(name) < 1 || atoi(name) >= 254)
			{
				sprintf(logBuff, "Warning: section [%s] invalid option %s, ignored", sectionName, raw);
				logDHCPMess(logBuff, 1);
				continue;
			}

			opTag = atoi(name);
			opType = 0;
		}

		for (MYBYTE i = 0; i < maxInd; i++)
			if (!strcasecmp(name, opData[i].opName) || (opTag && opTag == opData[i].opTag))
			{
				opTag = opData[i].opTag;
				opType = opData[i].opType;
				tagFound = true;
				break;
			}

		if (!opTag)
		{
			sprintf(logBuff, "Warning: section [%s] invalid option %s, ignored", sectionName, raw);
			logDHCPMess(logBuff, 1);
			continue;
		}

		if (!opType)
			opType = valType;

		//sprintf(logBuff, "Tag %i ValType %i opType %i value=%s size=%u", opTag, valType, opType, value, valSize);
		//logDHCPMess(logBuff, 1);

		if (op_specified[opTag])
		{
			sprintf(logBuff, "Warning: section [%s] duplicate option %s, ignored", sectionName, raw);
			logDHCPMess(logBuff, 1);
			continue;
		}

		//printf("Option=%u opType=%u valueType=%u valSize=%u\n", opTag, opType, valType, valSize);

		op_specified[opTag] = true;

		if (valType == 9)
		{
			if (buffsize > 2)
			{
				*dp = opTag;
				dp++;
				*dp = 0;
				dp++;
				buffsize -= 2;
			}
			else
			{
				sprintf(logBuff, "Warning: section [%s] option %s, no more space for options", sectionName, raw);
				logDHCPMess(logBuff, 1);
			}
			continue;
		}

		switch (opType)
		{
			case 1:
			{
				value[valSize] = 0;
				valSize++;

				if (valType != 1 && valType != 2)
				{
					sprintf(logBuff, "Warning: section [%s] option %s, need string value, option ignored", sectionName, raw);
					logDHCPMess(logBuff, 1);
				}
				else if (!strcasecmp(serviceName, "DUALServer") && opTag == DHCP_OPTION_DOMAINNAME)
				{
					sprintf(logBuff, "Warning: section [%s] Domain Name %s should be in section [DOMAIN_NAME], ignored", sectionName, value);
					logDHCPMess(logBuff, 1);
					continue;
				}
				else if (buffsize > valSize + 2)
				{
					*dp = opTag;
					dp++;
					*dp = valSize;
					dp++;
					memcpy(dp, value, valSize);
					dp += valSize;
					buffsize -= (valSize + 2);
				}
				else
				{
					sprintf(logBuff, "Warning: section [%s] option %s, no more space for options", sectionName, raw);
					logDHCPMess(logBuff, 1);
				}
			}
			break;

			case 3:
			case 8:
			{
				if (valType == 2)
				{
					if (opType == 3 && valSize % 4)
					{
						sprintf(logBuff, "Warning: section [%s] option %s, missing/extra bytes/octates in IP, option ignored", sectionName, raw);
						logDHCPMess(logBuff, 1);
						continue;
					}
					else if (opType == 8 && valSize % 8)
					{
						sprintf(logBuff, "Warning: section [%s] option %s, some values not in IP/Mask form, option ignored", sectionName, raw);
						logDHCPMess(logBuff, 1);
						continue;
					}

					if (opTag == DHCP_OPTION_NETMASK)
					{
						if (valSize != 4 || !checkMask(fIP(value)))
						{
							sprintf(logBuff, "Warning: section [%s] Invalid subnetmask %s, option ignored", sectionName, raw);
							logDHCPMess(logBuff, 1);
							continue;
						}
						else
							optionData->mask = fIP(value);
					}

					if (buffsize > valSize + 2)
					{
						*dp = opTag;
						dp++;
						*dp = valSize;
						dp++;
						memcpy(dp, value, valSize);
						dp += valSize;
						buffsize -= (valSize + 2);
					}
					else
					{
						sprintf(logBuff, "Warning: section [%s] option %s, no more space for options", sectionName, raw);
						logDHCPMess(logBuff, 1);
					}
				}
				else
				{
					sprintf(logBuff, "Warning: section [%s] option %s, Invalid value, should be one or more IP/4 Bytes", sectionName, raw);
					logDHCPMess(logBuff, 1);
				}
			}
			break;

			case 4:
			{
				MYDWORD j;

				if (valType == 2 && valSize == 4)
					j = fULong(value);
				else if (valType >= 4 && valType <= 6)
					j = atol(value);
				else
				{
					sprintf(logBuff, "Warning: section [%s] option %s, value should be integer between 0 & %u or 4 bytes, option ignored", sectionName, name, UINT_MAX);
					logDHCPMess(logBuff, 1);
					continue;
				}

				if (opTag == DHCP_OPTION_IPADDRLEASE)
				{
					if (j == 0)
						j = UINT_MAX;

					if (!strcasecmp(serviceName, "DUALServer"))
					{
						if (!strcasecmp(sectionName, GLOBALOPTIONS))
						{
							sprintf(logBuff, "Warning: section [%s] option %s not allowed in this section, please set it in [TIMINGS] section", sectionName, raw);
							logDHCPMess(logBuff, 1);
							continue;
						}
						else if (j < cfig.lease)
						{
							sprintf(logBuff, "Warning: section [%s] option %s value should be more then %u (Default Lease), ignored", sectionName, name, cfig.lease);
							logDHCPMess(logBuff, 1);
							continue;
						}
					}
					else if (!strcasecmp(serviceName, "OpenDHCPServer") && !strcasecmp(sectionName, GLOBALOPTIONS))
						cfig.lease = j;
				}

				if (buffsize > 6)
				{
					*dp = opTag;
					dp++;
					*dp = 4;
					dp++;
					dp += pULong(dp, j);
					buffsize -= 6;
					//printf("%s=%u=%u\n",opData[op_index].opName,opData[op_index].opType,htonl(j));
				}
				else
				{
					sprintf(logBuff, "Warning: section [%s] option %s, no more space for options", sectionName, raw);
					logDHCPMess(logBuff, 1);
				}
			}
			break;

			case 5:
			{
				MYWORD j;

				if (valType == 2 && valSize == 2)
					j = fUShort(value);
				else if (valType == 5 || valType == 6)
					j = atol(value);
				else
				{
					sprintf(logBuff, "Warning: section [%s] option %s, value should be between 0 & %u or 2 bytes, option ignored", sectionName, name, USHRT_MAX);
					logDHCPMess(logBuff, 1);
					continue;
				}

				if (buffsize > 4)
				{
					*dp = opTag;
					dp++;
					*dp = 2;
					dp++;
					dp += pUShort(dp, j);
					buffsize -= 4;
				}
				else
				{
					sprintf(logBuff, "Warning: section [%s] option %s, no more space for options", sectionName, raw);
					logDHCPMess(logBuff, 1);
				}
			}
			break;

			case 6:
			{
				MYBYTE j;

				if (valType == 2 && valSize == 1)
					j = *value;
				else if (valType == 6)
					j = atol(value);
				else
				{
					sprintf(logBuff, "Warning: section [%s] option %s, value should be between 0 & %u or single byte, option ignored", sectionName, name, UCHAR_MAX);
					logDHCPMess(logBuff, 1);
					continue;
				}

				if (buffsize > 3)
				{
					*dp = opTag;
					dp++;
					*dp = 1;
					dp++;
					*dp = j;
					dp++;
					buffsize -= 3;
				}
				else
				{
					sprintf(logBuff, "Warning: section [%s] option %s, no more space for options", sectionName, raw);
					logDHCPMess(logBuff, 1);
				}
			}
			break;

			case 7:
			{
				MYBYTE j;

				if (valType == 2 && valSize == 1 && *value < 2)
					j = *value;
				else if (valType == 1 && (!strcasecmp(value, "yes") || !strcasecmp(value, "on") || !strcasecmp(value, "true")))
					j = 1;
				else if (valType == 1 && (!strcasecmp(value, "no") || !strcasecmp(value, "off") || !strcasecmp(value, "false")))
					j = 0;
				else if (valType == 6 && atoi(value) < 2)
					j = atoi(value);
				else
				{
					sprintf(logBuff, "Warning: section [%s] option %s, value should be yes/on/true/1 or no/off/false/0, option ignored", sectionName, raw);
					logDHCPMess(logBuff, 1);
					continue;
				}

				if (buffsize > 3)
				{
					*dp = opTag;
					dp++;
					*dp = 1;
					dp++;
					*dp = j;
					dp++;
					buffsize -= 3;
				}
				else
				{
					sprintf(logBuff, "Warning: section [%s] option %s, no more space for options", sectionName, raw);
					logDHCPMess(logBuff, 1);
				}
			}
			break;

			default:
			{
				if (valType == 6)
				{
					valType = 2;
					valSize = 1;
					*value = atoi(value);
				}

				if (opType == 2 && valType != 2)
				{
					sprintf(logBuff, "Warning: section [%s] option %s, value should be comma separated bytes or hex string, option ignored", sectionName, raw);
					logDHCPMess(logBuff, 1);
					continue;
				}
				else if (buffsize > valSize + 2)
				{
					*dp = opTag;
					dp++;
					*dp = valSize;
					dp++;
					memcpy(dp, value, valSize);
					dp += valSize;
					buffsize -= (valSize + 2);
				}
				else
				{
					sprintf(logBuff, "Warning: section [%s] option %s, no more space for options", sectionName, raw);
					logDHCPMess(logBuff, 1);
				}
			}
			break;
		}

		//printf("%s Option=%u opType=%u valType=%u  valSize=%u\n", raw, opTag, opType, valType, valSize);
		//printf("%s %s\n", name, hex2String(tempbuff, ddp, valSize+2, ':'));
	}

	//printf("%s=%s\n", sectionName, optionData->vendClass);

	*dp = DHCP_OPTION_END;
	dp++;
	optionData->optionSize = (dp - optionData->options);
	//printf("section=%s buffersize = %u option size=%u\n", sectionName, buffsize, optionData->optionSize);
}

void lockOptions(FILE *f)
{
	char raw[512];
	char name[512];
	char value[512];

	while (readSection(raw, f))
	{
		mySplit(name, value, raw, '=');

		if (!name[0] || !value[0])
			continue;

		int op_index;
		MYBYTE n = sizeof(opData) / sizeof(data4);

		for (op_index = 0; op_index < n; op_index++)
			if (!strcasecmp(name, opData[op_index].opName) || (opData[op_index].opTag && atoi(name) == opData[op_index].opTag))
				break;

		if (op_index >= n)
			continue;

		if (opData[op_index].opType == 3)
		{
			if (myTokenize(value, value, "/,.", true))
			{
				char *ptr = value;
				char hoption[256];
				MYBYTE valueSize = 0;

				for (; *ptr; ptr = myGetToken(ptr, 1))
				{
					if (valueSize >= UCHAR_MAX)
						break;
					else if (isInt(ptr) && atoi(ptr) <= UCHAR_MAX)
					{
						hoption[valueSize] = atoi(ptr);
						valueSize++;
					}
					else
						break;
				}

				if (*ptr)
					continue;

				if (valueSize % 4)
					continue;

				for (MYBYTE i = 0; i < valueSize; i += 4)
				{
					MYDWORD ip = *((MYDWORD*)&(hoption[i]));

					if (ip != INADDR_ANY && ip != INADDR_NONE)
						lockIP(ip);
				}
			}
		}
	}
}

void addDHCPRange(char *dp)
{
	char logBuff[512];
	MYDWORD rs = 0;
	MYDWORD re = 0;
	char name[512];
	char value[512];
	mySplit(name, value, dp, '-');

	if (isIP(name) && isIP(value))
	{
		rs = htonl(inet_addr(name));
		re = htonl(inet_addr(value));

		if (rs && re && rs <= re)
		{
			data13 *range;
			MYBYTE m = 0;

			for (; m < MAX_DHCP_RANGES && cfig.dhcpRanges[m].rangeStart; m++)
			{
				range = &cfig.dhcpRanges[m];

				if ((rs >= range->rangeStart && rs <= range->rangeEnd)
						|| (re >= range->rangeStart && re <= range->rangeEnd)
						|| (range->rangeStart >= rs && range->rangeStart <= re)
						|| (range->rangeEnd >= rs && range->rangeEnd <= re))
				{
					sprintf(logBuff, "Warning: DHCP Range %s overlaps with another range, ignored", dp);
					logDHCPMess(logBuff, 1);
					return;
				}
			}

			if (m < MAX_DHCP_RANGES)
			{
				cfig.dhcpSize += (re - rs + 1);
				range = &cfig.dhcpRanges[m];
				range->rangeStart = rs;
				range->rangeEnd = re;
				range->expiry = (time_t*)calloc((re - rs + 1), sizeof(time_t));
				range->dhcpEntry = (data7**)calloc((re - rs + 1), sizeof(data7*));

				if (!range->expiry || !range->dhcpEntry)
				{
					if (range->expiry)
						free(range->expiry);

					if (range->dhcpEntry)
						free(range->dhcpEntry);

					sprintf(logBuff, "DHCP Ranges Load, Memory Allocation Error");
					logDHCPMess(logBuff, 1);
					return;
				}
			}
		}
		else
		{
			sprintf(logBuff, "Section [%s] Invalid DHCP range %s in ini file, ignored", RANGESET, dp);
			logDHCPMess(logBuff, 1);
		}
	}
	else
	{
		sprintf(logBuff, "Section [%s] Invalid DHCP range %s in ini file, ignored", RANGESET, dp);
		logDHCPMess(logBuff, 1);
	}
}

void addVendClass(MYBYTE rangeSetInd, char *vendClass, MYBYTE vendClassSize)
{
	char logBuff[512];
	data14 *rangeSet = &cfig.rangeSet[rangeSetInd];

	MYBYTE i = 0;

	for (; i <= MAX_RANGE_FILTERS && rangeSet->vendClassSize[i]; i++);

	if (i >= MAX_RANGE_FILTERS || !vendClassSize)
		return;

	rangeSet->vendClass[i] = (MYBYTE*)calloc(vendClassSize, 1);

	if(!rangeSet->vendClass[i])
	{
		sprintf(logBuff, "Vendor Class Load, Memory Allocation Error");
		logDHCPMess(logBuff, 1);
	}
	else
	{
		cfig.hasFilter = true;
		rangeSet->vendClassSize[i] = vendClassSize;
		memcpy(rangeSet->vendClass[i], vendClass, vendClassSize);
		//printf("Loaded Vendor Class %s Size=%i rangeSetInd=%i Ind=%i\n", rangeSet->vendClass[i], rangeSet->vendClassSize[i], rangeSetInd, i);
		//printf("Loaded Vendor Class %s Size=%i rangeSetInd=%i Ind=%i\n", hex2String(tempbuff, rangeSet->vendClass[i], rangeSet->vendClassSize[i], ':'), rangeSet->vendClassSize[i], rangeSetInd, i);
	}
}

void addUserClass(MYBYTE rangeSetInd, char *userClass, MYBYTE userClassSize)
{
	char logBuff[512];
	data14 *rangeSet = &cfig.rangeSet[rangeSetInd];

	MYBYTE i = 0;

	for (; i <= MAX_RANGE_FILTERS && rangeSet->userClassSize[i]; i++);

	if (i >= MAX_RANGE_FILTERS || !userClassSize)
		return;

	rangeSet->userClass[i] = (MYBYTE*)calloc(userClassSize, 1);

	if(!rangeSet->userClass[i])
	{
		sprintf(logBuff, "Vendor Class Load, Memory Allocation Error");
		logDHCPMess(logBuff, 1);
	}
	else
	{
		cfig.hasFilter = true;
		rangeSet->userClassSize[i] = userClassSize;
		memcpy(rangeSet->userClass[i], userClass, userClassSize);
		//printf("Loaded User Class %s Size=%i rangeSetInd=%i Ind=%i\n", hex2String(tempbuff, rangeSet->userClass[i], rangeSet->userClassSize[i], ':'), rangeSet->vendClassSize[i], rangeSetInd, i);
	}
}

void addMacRange(MYBYTE rangeSetInd, char *macRange)
{
	char logBuff[512];

	if (macRange[0])
	{
		data14 *rangeSet = &cfig.rangeSet[rangeSetInd];

		MYBYTE i = 0;

		for (; i <= MAX_RANGE_FILTERS && rangeSet->macSize[i]; i++);

		if (i >= MAX_RANGE_FILTERS)
			return;

		char name[256];
		char value[256];

		mySplit(name, value, macRange, '-');

		//printf("%s=%s\n", name, value);

		if(!name[0] || !value[0])
		{
			sprintf(logBuff, "Section [%s], invalid Filter_Mac_Range %s, ignored", RANGESET, macRange);
			logDHCPMess(logBuff, 1);
		}
		else
		{
			MYBYTE macSize1 = 16;
			MYBYTE macSize2 = 16;
			MYBYTE *macStart = (MYBYTE*)calloc(1, macSize1);
			MYBYTE *macEnd = (MYBYTE*)calloc(1, macSize2);

			if(!macStart || !macEnd)
			{
				sprintf(logBuff, "DHCP Range Load, Memory Allocation Error");
				logDHCPMess(logBuff, 1);
			}
			else if (getHexValue(macStart, name, &macSize1) || getHexValue(macEnd, value, &macSize2))
			{
				sprintf(logBuff, "Section [%s], Invalid character in Filter_Mac_Range %s", RANGESET, macRange);
				logDHCPMess(logBuff, 1);
				free(macStart);
				free(macEnd);
			}
			else if (memcmp(macStart, macEnd, 16) > 0)
			{
				sprintf(logBuff, "Section [%s], Invalid Filter_Mac_Range %s, (higher bound specified on left), ignored", RANGESET, macRange);
				logDHCPMess(logBuff, 1);
				free(macStart);
				free(macEnd);
			}
			else if (macSize1 != macSize2)
			{
				sprintf(logBuff, "Section [%s], Invalid Filter_Mac_Range %s, (start/end size mismatched), ignored", RANGESET, macRange);
				logDHCPMess(logBuff, 1);
				free(macStart);
				free(macEnd);
			}
			else
			{
				cfig.hasFilter = true;
				rangeSet->macSize[i] = macSize1;
				rangeSet->macStart[i] = macStart;
				rangeSet->macEnd[i] = macEnd;
				//printf("Mac Loaded, Size=%i Start=%s rangeSetInd=%i Ind=%i\n", rangeSet->macSize[i], hex2String(tempbuff, rangeSet->macStart[i], rangeSet->macSize[i]), rangeSetInd, i);
			}
		}
	}
}

void loadDHCP()
{
	char ipbuff[32];
	char logBuff[512];
	data7 *dhcpEntry = NULL;
	data8 dhcpData;
	char mapname[64];
	FILE *f = NULL;
	FILE *ff = NULL;

	if (f = openSection(GLOBALOPTIONS, 1))
	{
		data20 optionData;
		loadOptions(f, GLOBALOPTIONS, &optionData);
		cfig.options = (MYBYTE*)calloc(1, optionData.optionSize);
		memcpy(cfig.options, optionData.options, optionData.optionSize);
		cfig.mask = optionData.mask;
	}

	if (!cfig.mask)
		cfig.mask = inet_addr("255.255.255.0");

	for (MYBYTE i = 1; i <= MAX_RANGE_SETS ; i++)
	{
		if (f = openSection(RANGESET, i))
		{
			MYBYTE m = cfig.rangeCount;
			data20 optionData;
			optionData.rangeSetInd = i - 1;
			loadOptions(f, RANGESET, &optionData);
			MYBYTE *options = NULL;
			cfig.rangeSet[optionData.rangeSetInd].active = true;

			if (optionData.optionSize > 3)
			{
				options = (MYBYTE*)calloc(1, optionData.optionSize);
				memcpy(options, optionData.options, optionData.optionSize);
			}

			for (; m < MAX_DHCP_RANGES && cfig.dhcpRanges[m].rangeStart; m++)
			{
				cfig.dhcpRanges[m].rangeSetInd = optionData.rangeSetInd;
				cfig.dhcpRanges[m].options = options;
				cfig.dhcpRanges[m].mask = optionData.mask;
			}
			cfig.rangeCount = m;
		}
		else
			break;
	}

	//printf("%s\n", IP2String(ipbuff, cfig.mask));

	for (char rangeInd = 0; rangeInd < cfig.rangeCount; rangeInd++)
	{
		if (!cfig.dhcpRanges[rangeInd].mask)
			cfig.dhcpRanges[rangeInd].mask = cfig.mask;

		for (MYDWORD iip = cfig.dhcpRanges[rangeInd].rangeStart; iip <= cfig.dhcpRanges[rangeInd].rangeEnd; iip++)
		{
			MYDWORD ip = htonl(iip);

			if ((cfig.dhcpRanges[rangeInd].mask | (~ip)) == UINT_MAX || (cfig.dhcpRanges[rangeInd].mask | ip) == UINT_MAX)
				cfig.dhcpRanges[rangeInd].expiry[iip - cfig.dhcpRanges[rangeInd].rangeStart] = INT_MAX;
		}
	}

	if (f = openSection(GLOBALOPTIONS, 1))
		lockOptions(f);

	for (MYBYTE i = 1; i <= MAX_RANGE_SETS ;i++)
	{
		if (f = openSection(RANGESET, i))
			lockOptions(f);
		else
			break;
	}

	ff = fopen(iniFile, "rt");

	if (ff)
	{
		char sectionName[512];

		while (fgets(sectionName, 510, ff))
		{
			if (*sectionName == '[')
			{
				char *secend = strchr(sectionName, ']');

				if (secend)
				{
					*secend = 0;
					sectionName[0] = NBSP;
					myTrim(sectionName, sectionName);
				}
				else
					continue;
			}
			else
				continue;

			if (!strchr(sectionName, ':'))
				continue;

			//printf("%s\n", sectionName);

			MYBYTE hexValue[UCHAR_MAX];
			MYBYTE hexValueSize = sizeof(hexValue);
			data20 optionData;

			if (strlen(sectionName) <= 48 && !getHexValue(hexValue, sectionName, &hexValueSize))
			{
				if (hexValueSize <= 16)
				{
					dhcpEntry = findDHCPEntry(hex2String(mapname, hexValue, hexValueSize));

					if (!dhcpEntry)
					{
						if (f = openSection(sectionName, 1))
							loadOptions(f, sectionName, &optionData);
						if (f = openSection(sectionName, 1))
							lockOptions(f);

						dhcpMap::iterator p = dhcpCache.begin();

						for (; p != dhcpCache.end(); p++)
						{
							if (p->second && p->second->ip && p->second->ip == optionData.ip)
								break;
						}

						if (p == dhcpCache.end())
						{
							memset(&lump, 0, sizeof(data71));
							lump.mapname = mapname;
							lump.optionSize = optionData.optionSize;
							lump.options = optionData.options;
							dhcpEntry = createCache(&lump);

							if (!dhcpEntry)
								return;

							dhcpEntry->ip = optionData.ip;
							dhcpEntry->rangeInd = getRangeInd(optionData.ip);
							dhcpEntry->fixed = 1;
							dhcpEntry->expiry = 0;
							dhcpCache[dhcpEntry->mapname] = dhcpEntry;
							setLeaseExpiry(dhcpEntry);
							lockIP(optionData.ip);
							//printf("%s=%s=%s size=%u %u\n", mapname, dhcpEntry->mapname, IP2String(ipbuff, optionData.ip), optionData.optionSize, dhcpEntry->options);
						}
						else
						{
							sprintf(logBuff, "Static DHCP Host [%s] Duplicate IP Address %s, Entry ignored", sectionName, IP2String(ipbuff, optionData.ip));
							logDHCPMess(logBuff, 1);
						}
					}
					else
					{
						sprintf(logBuff, "Duplicate Static DHCP Host [%s] ignored", sectionName);
						logDHCPMess(logBuff, 1);
					}
				}
				else
				{
					sprintf(logBuff, "Invalid Static DHCP Host MAC Addr size, ignored", sectionName);
					logDHCPMess(logBuff, 1);
				}
			}
			else
			{
				sprintf(logBuff, "Invalid Static DHCP Host MAC Addr [%s] ignored", sectionName);
				logDHCPMess(logBuff, 1);
			}

			if (!optionData.ip)
			{
				sprintf(logBuff, "Warning: No IP Address for DHCP Static Host %s specified", sectionName);
				logDHCPMess(logBuff, 1);
			}
		}

		fclose(ff);
	}

	f = fopen(leaFile, "rb");

	if (f)
	{
		while (fread(&dhcpData, sizeof(data8), 1, f))
		{
			char rangeInd = -1;
			int ind = -1;
			//sprintf(logBuff, "Loading %s=%s", dhcpData.hostname, IP2String(ipbuff, dhcpData.ip));
			//logDHCPMess(logBuff, 1);

			if (dhcpData.expiry > (t - 31*24*3600) && dhcpData.bp_hlen <= 16 && !findServer(network.allServers, MAX_SERVERS, dhcpData.ip))
			{
				hex2String(mapname, dhcpData.bp_chaddr, dhcpData.bp_hlen);

				dhcpMap::iterator p = dhcpCache.begin();

				for (; p != dhcpCache.end(); p++)
				{
					dhcpEntry = p->second;

					if (!dhcpEntry || (!strcasecmp(mapname, dhcpEntry->mapname) || dhcpEntry->ip == dhcpData.ip))
						break;
				}

				if ((p != dhcpCache.end()) && (strcasecmp(mapname, dhcpEntry->mapname) || dhcpEntry->ip != dhcpData.ip))
					continue;

				dhcpEntry = findDHCPEntry(mapname);
				rangeInd = getRangeInd(dhcpData.ip);

				if(!dhcpEntry && rangeInd >= 0)
				{
					memset(&lump, 0, sizeof(data71));
					lump.mapname = mapname;
					dhcpEntry = createCache(&lump);
				}

				if (dhcpEntry)
				{
					dhcpCache[dhcpEntry->mapname] = dhcpEntry;
					dhcpEntry->subnetFlg = dhcpData.subnetFlg;
					dhcpEntry->ip = dhcpData.ip;
					dhcpEntry->rangeInd = rangeInd;
					dhcpEntry->expiry = dhcpData.expiry;
					dhcpEntry->local = dhcpData.local;
					dhcpEntry->display = true;
					strcpy(dhcpEntry->hostname, dhcpData.hostname);
					setLeaseExpiry(dhcpEntry);

					//printf("Loaded %s=%s\n", dhcpData.hostname, IP2String(ipbuff, dhcpData.ip));
				}
			}
		}

		fclose(f);
	}

	f = fopen(leaFile, "wb");
	fclose(f);
	f = fopen(leaFile, "rb+");

	if (f)
	{
		dhcpMap::iterator p = dhcpCache.begin();

		for (; p != dhcpCache.end(); p++)
		{
			if ((dhcpEntry = p->second)  && dhcpEntry->expiry > (t - 31*24*3600))
			{
				memset(&dhcpData, 0, sizeof(data8));
				MYBYTE bp_hlen = 16;
				getHexValue(dhcpData.bp_chaddr, dhcpEntry->mapname, &bp_hlen);
				dhcpData.bp_hlen = bp_hlen;
				dhcpData.ip = dhcpEntry->ip;
				dhcpData.subnetFlg = dhcpEntry->subnetFlg;
				dhcpData.expiry = dhcpEntry->expiry;
				dhcpData.local = dhcpEntry->local;
				strcpy(dhcpData.hostname, dhcpEntry->hostname);
				cfig.dhcpInd += 1;
				dhcpEntry->dhcpInd = cfig.dhcpInd;
				dhcpData.dhcpInd = dhcpEntry->dhcpInd;

				if (fseek(f, (dhcpData.dhcpInd - 1)*sizeof(data8), SEEK_SET) >= 0)
					fwrite(&dhcpData, sizeof(data8), 1, f);
			}
		}

		fclose(f);
	}
}

bool getSection(const char *sectionName, char *buffer, MYBYTE serial, char *fileName)
{
	//printf("%s=%s\n",fileName,sectionName);
	char section[128];
	sprintf(section, "[%s]", sectionName);
	myUpper(section);
	FILE *f = fopen(fileName, "rt");
	char buff[512];
	MYBYTE found = 0;

	if (f)
	{
		while (fgets(buff, 511, f))
		{
			myUpper(buff);
			myTrim(buff, buff);

			if (strstr(buff, section) == buff)
			{
				found++;
				if (found == serial)
				{
					//printf("%s=%s\n",fileName,sectionName);
					while (fgets(buff, 511, f))
					{
						myTrim(buff, buff);

						if (strstr(buff, "[") == buff)
							break;

						if ((*buff) >= '0' && (*buff) <= '9' || (*buff) >= 'A' && (*buff) <= 'Z' || (*buff) >= 'a' && (*buff) <= 'z' || ((*buff) && strchr("/\\?*", (*buff))))
						{
							buffer += sprintf(buffer, "%s", buff);
							buffer++;
						}
					}
					break;
				}
			}
		}
		fclose(f);
	}

	*buffer = 0;
	*(buffer + 1) = 0;
	return (found == serial);
}

FILE *openSection(const char *sectionName, MYBYTE serial)
{
	char logBuff[512];
	char tempbuff[512];
	char section[128];
	sprintf(section, "[%s]", sectionName);
	myUpper(section);
	FILE *f = NULL;
	f = fopen(iniFile, "rt");

	if (f)
	{
		//printf("opened %s=%d\n", tempbuff, f);
		char buff[512];
		MYBYTE found = 0;

		while (fgets(buff, 511, f))
		{
			myUpper(buff);
			myTrim(buff, buff);

			if (strstr(buff, section) == buff)
			{
				found++;

				if (found == serial)
				{
					MYDWORD fpos = ftell(f);

					if (fgets(buff, 511, f))
					{
						myTrim(buff, buff);

						if (buff[0] == '@')
						{
							fclose(f);
							f = NULL;

							buff[0] = NBSP;
							myTrim(buff, buff);

							if (strchr(buff, '\\') || strchr(buff, '/'))
								strcpy(tempbuff, buff);
							else
								sprintf(tempbuff, "%s%s", filePATH, buff);

							f = fopen(tempbuff, "rt");

							if (f)
								return f;
							else
							{
								sprintf(logBuff, "Error: Section [%s], file %s not found", sectionName, tempbuff);
								logDHCPMess(logBuff, 1);
								return NULL;
							}
						}
						else
						{
							fseek(f, fpos, SEEK_SET);
							return f;
						}
					}
				}
			}
		}
		fclose(f);
	}
	return NULL;
}

char *readSection(char* buff, FILE *f)
{
	while (fgets(buff, 511, f))
	{
		myTrim(buff, buff);

		if (*buff == '[')
			break;

		if ((*buff) >= '0' && (*buff) <= '9' || (*buff) >= 'A' && (*buff) <= 'Z' || (*buff) >= 'a' && (*buff) <= 'z' || ((*buff) && strchr("/\\?*", (*buff))))
			return buff;
	}

	fclose(f);
	return NULL;
}

char* myGetToken(char* buff, MYBYTE index)
{
	while (*buff)
	{
		if (index)
			index--;
		else
			break;

		buff += strlen(buff) + 1;
	}

	return buff;
}

MYWORD myTokenize(char *target, char *source, const char *sep, bool whiteSep)
{
	bool found = true;
	char *dp = target;
	MYWORD kount = 0;

	while (*source)
	{
		if (sep && sep[0] && strchr(sep, (*source)))
		{
			found = true;
			source++;
			continue;
		}
		else if (whiteSep && (*source) <= NBSP)
		{
			found = true;
			source++;
			continue;
		}

		if (found)
		{
			if (target != dp)
			{
				*dp = 0;
				dp++;
			}
			kount++;
		}

		found = false;
		*dp = *source;
		dp++;
		source++;
	}

	*dp = 0;
	dp++;
	*dp = 0;

	//printf("%s\n", target);

	return kount;
}

char *cloneString(char *string)
{
	if (!string)
		return NULL;

	char *s = (char*)calloc(1, strlen(string) + 1);

	if (s)
	{
		strcpy(s, string);
	}
	return s;
}

char* myTrim(char *target, char *source)
{
	while ((*source) && (*source) <= NBSP)
		source++;

	int i = 0;

	for (; i < 511 && source[i]; i++)
		target[i] = source[i];

	target[i] = source[i];
	i--;

	for (; i >= 0 && target[i] <= NBSP; i--)
		target[i] = 0;

	return target;
}

void mySplit(char *name, char *value, char *source, char splitChar)
{
	int i = 0;
	int j = 0;
	int k = 0;

	for (; source[i] && j <= 510 && source[i] != splitChar; i++, j++)
	{
		name[j] = source[i];
	}

	if (source[i])
	{
		i++;
		for (; k <= 510 && source[i]; i++, k++)
		{
			value[k] = source[i];
		}
	}

	name[j] = 0;
	value[k] = 0;

	myTrim(name, name);
	myTrim(value, value);
	//printf("%s %s\n", name, value);
}

char *IP2String(char *target, MYDWORD ip, MYBYTE dnsType)
{
	char *dp = target;
	(*dp) = dnsType;
	dp++;
	data15 inaddr;
	inaddr.ip = ip;
	sprintf(dp, "%u.%u.%u.%u", inaddr.octate[0], inaddr.octate[1], inaddr.octate[2], inaddr.octate[3]);
	//MYBYTE *octate = (MYBYTE*)&ip;
	//sprintf(target, "%u.%u.%u.%u", octate[0], octate[1], octate[2], octate[3]);
	return target;
}

char *IP2String(char *target, MYDWORD ip)
{
	data15 inaddr;
	inaddr.ip = ip;
	sprintf(target, "%u.%u.%u.%u", inaddr.octate[0], inaddr.octate[1], inaddr.octate[2], inaddr.octate[3]);
	//MYBYTE *octate = (MYBYTE*)&ip;
	//sprintf(target, "%u.%u.%u.%u", octate[0], octate[1], octate[2], octate[3]);
	return target;
}

MYBYTE addServer(MYDWORD *array, MYBYTE maxServers, MYDWORD ip)
{
	if (ip)
	{
		for (MYBYTE i = 0; i < maxServers; i++)
		{
			if (array[i] == ip)
				return i;
			else if (!array[i])
			{
				array[i] = ip;
				return i;
			}
		}
	}
	return maxServers;
}

MYDWORD *findServer(MYDWORD *array, MYBYTE maxServers, MYDWORD ip)
{
	if (ip)
	{
		for (MYBYTE i = 0; i < maxServers && array[i]; i++)
		{
			if (array[i] == ip)
				return &(array[i]);
		}
	}
	return NULL;
}

bool isInt(char *str)
{
	if (!str || !(*str))
		return false;

	for(; *str; str++)
		if (*str <  '0' || *str > '9')
			return false;

	return true;
}

bool isIP(char *str)
{
	if (!str || !(*str))
		return false;

	MYDWORD ip = inet_addr(str);

	if (ip == INADDR_NONE || ip == INADDR_ANY)
		return false;

	int j = 0;

	for (; *str; str++)
	{
		if (*str == '.' && *(str + 1) != '.')
			j++;
		else if (*str < '0' || *str > '9')
			return false;
	}

	if (j == 3)
		return true;
	else
		return false;
}

/*
char *toBase64(MYBYTE *source, MYBYTE length)
{
	MYBYTE a = 0, b = 0, i = 0;
	char *dp = tempbuff;

	for (; length; length--, source++)
	{
		i += 2;
		a = (*source) >> i;
		*dp = base64[a + b];
		dp++;
		b = (*source) << (8 - i);
		b >>= 2;
		if (i == 6)
		{
			*dp = base64[b];
			dp++;
			i = b = 0;
		}
	}
	if (i)
	{
		*dp = base64[b];
		dp++;
	}
	*dp = 0;
	//printf("%s\n",tempbuff);
	return tempbuff;
}

MYBYTE getBaseValue(MYBYTE a)
{
	if (a >= 'A' && a <= 'Z')
		a -= 'A';
	else if (a >= 'a' && a <= 'z')
		a = a - 'a' + 26;
	else if (a >= '0' && a <= '9')
		a = a - '0' + 52;
	else if (a == '+')
		a = 62;
	else if (a == '/')
		a = 63;
	else
		a = UCHAR_MAX;

	return a;
}

MYBYTE fromBase64(MYBYTE *target, char *source)
{
	//printf("SOURCE=%s\n", source);
	MYBYTE b = 0;
	MYBYTE shift = 4;
	MYBYTE bp_hlen = (3 * strlen(source))/4;
	*target = 0;

	if (*source)
	{
		b = getBaseValue(*source);
		*target = b << 2;
		source++;

		while (*source)
		{
			b = getBaseValue(*source);
			(*target) += (b >> (8 - shift));
			target++;
			(*target) = (b << shift);
			shift += 2;

			if (shift > 8)
			{
				source++;

				if (*source)
				{
					b = getBaseValue(*source);
					*target = b << 2;
					shift = 4;
				}
				else
					break;
			}

			source++;
		}
	}
	//printf("SIZE=%u\n", bp_hlen);
	return bp_hlen;
}

char *toUUE(char *tempbuff, MYBYTE *source, MYBYTE length)
{
	MYBYTE a = 0, b = 0, i = 0;
	char *dp = tempbuff;

	for (; length; length--, source++)
	{
		i += 2;
		a = (*source) >> i;
		*dp = a + b + NBSP;
		dp++;
		b = (*source) << (8 - i);
		b >>= 2;
		if (i == 6)
		{
			*dp = b + NBSP;
			dp++;
			i = b = 0;
		}
	}
	if (i)
	{
		*dp = b + NBSP;
		dp++;
	}
	*dp = 0;
	//printf("%s\n",tempbuff);
	return tempbuff;
}

MYBYTE fromUUE(MYBYTE *target, char *source)
{
	//printf("SOURCE=%s\n", source);
	MYBYTE b = 0;
	MYBYTE shift = 4;
	MYBYTE bp_hlen = (3 * strlen(source))/4;
	*target = 0;

	if (*source)
	{
		b = *source - NBSP;
		*target = b << 2;
		source++;

		while (*source)
		{
			b = *source - NBSP;
			(*target) += (b >> (8 - shift));
			target++;
			(*target) = (b << shift);
			shift += 2;

			if (shift > 8)
			{
				source++;

				if (*source)
				{
					b = *source - NBSP;
					*target = b << 2;
					shift = 4;
				}
				else
					break;
			}

			source++;
		}
	}
	//printf("SIZE=%u\n", bp_hlen);
	return bp_hlen;
}
*/
char *hex2String(char *target, MYBYTE *hex, MYBYTE bytes)
{
	char *dp = target;

	if (bytes)
		dp += sprintf(target, "%02x", *hex);
	else
		*target = 0;

	for (MYBYTE i = 1; i < bytes; i++)
			dp += sprintf(dp, ":%02x", *(hex + i));

	return target;
}

char *genHostName(char *target, MYBYTE *hex, MYBYTE bytes)
{
	char *dp = target;

	if (bytes)
		dp += sprintf(target, "Host%02x", *hex);
	else
		*target = 0;

	for (MYBYTE i = 1; i < bytes; i++)
			dp += sprintf(dp, "%02x", *(hex + i));

	return target;
}

/*
char *IP62String(char *target, MYBYTE *source)
{
	MYWORD *dw = (MYWORD*)source;
	char *dp = target;
	MYBYTE markbyte;

	for (markbyte = 4; markbyte > 0 && !dw[markbyte - 1]; markbyte--);

	for (MYBYTE i = 0; i < markbyte; i++)
		dp += sprintf(dp, "%x:", ntohs(dw[i]));

	for (markbyte = 4; markbyte < 8 && !dw[markbyte]; markbyte++);

	for (MYBYTE i = markbyte; i < 8; i++)
		dp += sprintf(dp, ":%x", htons(dw[i]));

	return target;
}
*/

char *IP62String(char *target, MYBYTE *source)
{
	char *dp = target;
	bool zerostarted = false;
	bool zeroended = false;

	for (MYBYTE i = 0; i < 16; i += 2, source += 2)
	{
		if (source[0])
		{
			if (zerostarted)
				zeroended = true;

			if (zerostarted && zeroended)
			{
				dp += sprintf(dp, "::");
				zerostarted = false;
			}
			else if (dp != target)
				dp += sprintf(dp, ":");

			dp += sprintf(dp, "%x", source[0]);
			dp += sprintf(dp, "%02x", source[1]);
		}
		else if (source[1])
		{
			if (zerostarted)
				zeroended = true;

			if (zerostarted && zeroended)
			{
				dp += sprintf(dp, "::");
				zerostarted = false;
			}
			else if (dp != target)
				dp += sprintf(dp, ":");

			dp += sprintf(dp, "%0x", source[1]);
		}
		else if (!zeroended)
			zerostarted = true;
	}

	return target;
}

char *getHexValue(MYBYTE *target, char *source, MYBYTE *size)
{
	if (*size)
		memset(target, 0, (*size));

	for ((*size) = 0; (*source) && (*size) < UCHAR_MAX; (*size)++, target++)
	{
		if ((*source) >= '0' && (*source) <= '9')
		{
			(*target) = (*source) - '0';
		}
		else if ((*source) >= 'a' && (*source) <= 'f')
		{
			(*target) = (*source) - 'a' + 10;
		}
		else if ((*source) >= 'A' && (*source) <= 'F')
		{
			(*target) = (*source) - 'A' + 10;
		}
		else
		{
			return source;
		}

		source++;

		if ((*source) >= '0' && (*source) <= '9')
		{
			(*target) *= 16;
			(*target) += (*source) - '0';
		}
		else if ((*source) >= 'a' && (*source) <= 'f')
		{
			(*target) *= 16;
			(*target) += (*source) - 'a' + 10;
		}
		else if ((*source) >= 'A' && (*source) <= 'F')
		{
			(*target) *= 16;
			(*target) += (*source) - 'A' + 10;
		}
		else if ((*source) == ':' || (*source) == '-')
		{
			source++;
			continue;
		}
		else if (*source)
		{
			return source;
		}
		else
		{
			continue;
		}

		source++;

		if ((*source) == ':' || (*source) == '-')
		{
			source++;
		}
		else if (*source)
			return source;
	}

	if (*source)
		return source;

	//printf("macfucked in=%s\n", tSource);
	//printf("macfucked out=%s\n", hex2String(tempbuff, tTarget, *size));
	return NULL;
}

char *myUpper(char *string)
{
	char diff = 'a' - 'A';
	MYWORD len = strlen(string);
	for (int i = 0; i < len; i++)
		if (string[i] >= 'a' && string[i] <= 'z')
			string[i] -= diff;
	return string;
}

char *myLower(char *string)
{
	char diff = 'a' - 'A';
	MYWORD len = strlen(string);
	for (int i = 0; i < len; i++)
		if (string[i] >= 'A' && string[i] <= 'Z')
			string[i] += diff;
	return string;
}

void listDhcpCache()
{
	char logBuff[512];
	dhcpMap::iterator p = dhcpCache.begin();
	data7 *cache = NULL;

	while (p != dhcpCache.end())
	{
		cache = p->second;
		sprintf(logBuff, cache->mapname);
		logDHCPMess(logBuff, 1);
		p++;
	}
}

void calcRangeLimits(MYDWORD ip, MYDWORD mask, MYDWORD *rangeStart, MYDWORD *rangeEnd)
{
	*rangeStart = htonl(ip & mask) + 1;
	*rangeEnd = htonl(ip | (~mask)) - 1;
}

bool checkMask(MYDWORD mask)
{
	mask = htonl(mask);

	while (mask)
	{
		if (mask < (mask << 1))
			return false;

		mask <<= 1;
	}
	return true;
}

MYDWORD calcMask(MYDWORD rangeStart, MYDWORD rangeEnd)
{
	data15 ip1, ip2, mask;

	ip1.ip = htonl(rangeStart);
	ip2.ip = htonl(rangeEnd);

	for (MYBYTE i = 0; i < 4; i++)
	{
		mask.octate[i] = ip1.octate[i] ^ ip2.octate[i];

		if (i && mask.octate[i - 1] < 255)
			mask.octate[i] = 0;
		else if (mask.octate[i] == 0)
			mask.octate[i] = 255;
		else if (mask.octate[i] < 2)
			mask.octate[i] = 254;
		else if (mask.octate[i] < 4)
			mask.octate[i] = 252;
		else if (mask.octate[i] < 8)
			mask.octate[i] = 248;
		else if (mask.octate[i] < 16)
			mask.octate[i] = 240;
		else if (mask.octate[i] < 32)
			mask.octate[i] = 224;
		else if (mask.octate[i] < 64)
			mask.octate[i] = 192;
		else if (mask.octate[i] < 128)
			mask.octate[i] = 128;
		else
			mask.octate[i] = 0;
	}

	return mask.ip;
}

data7 *findDHCPEntry(char *key)
{
	//printf("finding %u=%s\n",ind,key);
	myLower(key);
	dhcpMap::iterator it = dhcpCache.find(key);

	if (it != dhcpCache.end() && it->second)
		return it->second;

	return NULL;
}

void __cdecl init(void *lpParam)
{
	FILE *f = NULL;
	char raw[512];
	char name[512];
	char value[512];
	char ipbuff[32];
	char logBuff[512];
	char tempbuff[512];

	memset(&cfig, 0, sizeof(cfig));
	memset(&network, 0, sizeof(network));
	GetModuleFileName(NULL, filePATH, _MAX_PATH);
	char *fileExt = strrchr(filePATH, '.');
	*fileExt = 0;
	sprintf(leaFile, "%s.state", filePATH);
	sprintf(iniFile, "%s.ini", filePATH);
	sprintf(lnkFile, "%s.url", filePATH);
	sprintf(htmFile, "%s.htm", filePATH);
	sprintf(tempFile, "%s.tmp", filePATH);
	fileExt = strrchr(filePATH, '\\');
	*fileExt = 0;
	fileExt++;
	sprintf(logFile, "%s\\log\\%s%%Y%%m%%d.log", filePATH, fileExt);
	sprintf(cliFile, "%s\\log\\%%s.log", filePATH);
	strcat(filePATH, "\\");

	cfig.dhcpLogLevel = 1;

	lEvent = CreateEvent(
		NULL,                  // default security descriptor
		FALSE,                 // ManualReset
		TRUE,                  // Signalled
		TEXT("AchalOpenDHCPLogEvent"));  // object name

	if (lEvent == NULL)
	{
		printf("CreateEvent error: %d\n", GetLastError());
		Sleep(1000);
		exit(-1);
	}
	else if ( GetLastError() == ERROR_ALREADY_EXISTS )
	{
		sprintf(logBuff, "CreateEvent opened an existing Event\nServer May already be Running");
		logDHCPMess(logBuff, 0);
		Sleep(1000);
		exit(-1);
	}

	SetEvent(lEvent);

	if (f = openSection("LOGGING", 1))
	{
		tempbuff[0] = 0;

		while (readSection(raw, f))
		{
			mySplit(name, value, raw, '=');

			if (name[0] && value[0])
			{
				if (!strcasecmp(name, "DHCPLogLevel") || !strcasecmp(name, "LogLevel"))
				{
					if (!strcasecmp(value, "None"))
						cfig.dhcpLogLevel = 0;
					else if (!strcasecmp(value, "Normal"))
						cfig.dhcpLogLevel = 1;
					else if (!strcasecmp(value, "All"))
						cfig.dhcpLogLevel = 2;
//					else if (!strcasecmp(value, "Debug"))
//						cfig.dhcpLogLevel = 3;
					else
						sprintf(tempbuff, "Section [LOGGING], Invalid DHCPLogLevel: %s", value);
				}
				else
					sprintf(tempbuff, "Section [LOGGING], Invalid Entry %s ignored", raw);
			}
			else
				sprintf(tempbuff, "Section [LOGGING], Invalid Entry %s ignored", raw);
		}

		if (tempbuff[0])
			logDHCPMess(tempbuff, 1);

		sprintf(logBuff, "%s Starting...", sVersion);
		logDHCPMess(logBuff, 1);
	}
	else
	{
		sprintf(logBuff, "%s Starting...", sVersion);
		logDHCPMess(logBuff, 1);
	}

	MYWORD wVersionRequested = MAKEWORD(1, 1);
	WSAStartup(wVersionRequested, &cfig.wsaData);

	if (cfig.wsaData.wVersion != wVersionRequested)
	{
		sprintf(logBuff, "WSAStartup Error");
		logDHCPMess(logBuff, 1);
	}

	if (cfig.dhcpLogLevel == 3)
		sprintf(logBuff, "Logging: Debug");
	else if (cfig.dhcpLogLevel == 2)
		sprintf(logBuff, "Logging: All");
	else if (cfig.dhcpLogLevel == 1)
		sprintf(logBuff, "Logging: Normal");
	else
		sprintf(logBuff, "Logging: None");

	logDHCPMess(logBuff, 1);

	if (f = openSection("LISTEN_ON", 1))
	{
		while (readSection(raw, f))
		{
			if (isIP(raw))
			{
				MYDWORD addr = inet_addr(raw);
				addServer(cfig.specifiedServers, MAX_SERVERS, addr);
			}
			else
			{
				sprintf(logBuff, "Warning: Section [LISTEN_ON], Invalid Interface Address %s, ignored", raw);
				logDHCPMess(logBuff, 1);
			}
		}
	}

	getInterfaces(&network);

	if (f = openSection("REPLICATION_SERVERS", 1))
	{
		while (readSection(raw, f))
		{
			mySplit(name, value, raw, '=');

			if (name[0] && value[0])
			{
				if (!isIP(name) && isIP(value))
				{
					if (!strcasecmp(name, "Primary"))
						cfig.zoneServers[0] = inet_addr(value);
					else if (!strcasecmp(name, "Secondary"))
						cfig.zoneServers[1] = inet_addr(value);
					else
					{
						sprintf(logBuff, "Section [REPLICATION_SERVERS] Invalid Entry: %s ignored", raw);
						logDHCPMess(logBuff, 1);
					}
				}
				else
				{
					sprintf(logBuff, "Section [REPLICATION_SERVERS] Invalid Entry: %s ignored", raw);
					logDHCPMess(logBuff, 1);
				}
			}
			else
			{
				sprintf(logBuff, "Section [REPLICATION_SERVERS], Missing value, entry %s ignored", raw);
				logDHCPMess(logBuff, 1);
			}
		}
	}


	if (!cfig.zoneServers[0] && cfig.zoneServers[1])
	{
		sprintf(logBuff, "Section [REPLICATION_SERVERS] Missing Primary Server");
		logDHCPMess(logBuff, 1);
	}
	else if (cfig.zoneServers[0] && !cfig.zoneServers[1])
	{
		sprintf(logBuff, "Section [REPLICATION_SERVERS] Missing Secondary Server");
		logDHCPMess(logBuff, 1);
	}
	else if (cfig.zoneServers[0] && cfig.zoneServers[1])
	{
		if (findServer(network.staticServers, MAX_SERVERS, cfig.zoneServers[0]) && findServer(network.staticServers, MAX_SERVERS, cfig.zoneServers[1]))
		{
			sprintf(logBuff, "Section [REPLICATION_SERVERS] Primary & Secondary should be Different Boxes");
			logDHCPMess(logBuff, 1);
		}
		else if (findServer(network.staticServers, MAX_SERVERS, cfig.zoneServers[0]))
			cfig.replication = 1;
		else if (findServer(network.staticServers, MAX_SERVERS, cfig.zoneServers[1]))
			cfig.replication = 2;
		else
		{
			sprintf(logBuff, "Section [REPLICATION_SERVERS] No Server IP not found on this Machine");
			logDHCPMess(logBuff, 1);
		}
	}

	if (cfig.replication)
	{
		lockIP(cfig.zoneServers[0]);
		lockIP(cfig.zoneServers[1]);

		cfig.dhcpReplConn.sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

		if (cfig.dhcpReplConn.sock == INVALID_SOCKET)
		{
			sprintf(logBuff, "Failed to Create DHCP Replication Socket");
			logDHCPMess(logBuff, 1);
		}
		else
		{
			//printf("Socket %u\n", cfig.dhcpReplConn.sock);

			if (cfig.replication == 1)
				cfig.dhcpReplConn.server = cfig.zoneServers[0];
			else
				cfig.dhcpReplConn.server = cfig.zoneServers[1];

			cfig.dhcpReplConn.addr.sin_family = AF_INET;
			cfig.dhcpReplConn.addr.sin_addr.s_addr = cfig.dhcpReplConn.server;
			cfig.dhcpReplConn.addr.sin_port = 0;

			int nRet = bind(cfig.dhcpReplConn.sock, (sockaddr*)&cfig.dhcpReplConn.addr, sizeof(struct sockaddr_in));

			if (nRet == SOCKET_ERROR)
			{
				cfig.dhcpReplConn.ready = false;
				sprintf(logBuff, "DHCP Replication Server, Bind Failed");
				logDHCPMess(logBuff, 1);
			}
			else
			{
				cfig.dhcpReplConn.port = IPPORT_DHCPS;
				cfig.dhcpReplConn.loaded = true;
				cfig.dhcpReplConn.ready = true;

				data3 op;
				memset(&token, 0, sizeof(data9));
				token.vp = token.dhcpp.vend_data;
				token.messsize = sizeof(dhcp_packet);

				token.dhcpp.header.bp_op = BOOTP_REQUEST;
				token.dhcpp.header.bp_xid = t;
				token.dhcpp.header.bp_magic_num[0] = 99;
				token.dhcpp.header.bp_magic_num[1] = 130;
				token.dhcpp.header.bp_magic_num[2] = 83;
				token.dhcpp.header.bp_magic_num[3] = 99;

				op.opt_code = DHCP_OPTION_MESSAGETYPE;
				op.size = 1;
				op.value[0] = DHCP_MESS_INFORM;
				pvdata(&token, &op);

				token.vp[0] = DHCP_OPTION_END;
				token.vp++;
				token.bytes = token.vp - (MYBYTE*)token.raw;

				token.remote.sin_port = htons(IPPORT_DHCPS);
				token.remote.sin_family = AF_INET;

				if (cfig.replication == 1)
					token.remote.sin_addr.s_addr = cfig.zoneServers[1];
				else
					token.remote.sin_addr.s_addr = cfig.zoneServers[0];

				if (cfig.replication == 2)
					_beginthread(sendToken, 0, 0);
			}
		}
	}

	cfig.lease = 36000;
	loadDHCP();

	fEvent = CreateEvent(
		NULL,                  // default security descriptor
		FALSE,                 // ManualReset
		TRUE,                  // Signalled
		TEXT("AchalOpenDHCPServerFileEvent"));  // object name

	if (fEvent == NULL)
	{
		printf("CreateEvent error: %d\n", GetLastError());
		Sleep(1000);
		exit(-1);
	}
	else if ( GetLastError() == ERROR_ALREADY_EXISTS )
	{
		sprintf(logBuff, "CreateEvent opened an existing Event, Server may already be Running");
		logDHCPMess(logBuff, 0);
		Sleep(1000);
		exit(-1);
	}
	//SetEvent(fEvent);

/*
	rEvent = CreateEvent(
		NULL,                  // default security descriptor
		FALSE,                 // ManualReset
		TRUE,                  // Signalled
		TEXT("AchalDualServerReplicationEvent"));  // object name

	if (rEvent == NULL)
	{
		printf("CreateEvent error: %d\n", GetLastError());
		exit(-1);
	}
	else if ( GetLastError() == ERROR_ALREADY_EXISTS )
	{
		sprintf(logBuff, "CreateEvent opened an existing Event\nServer May already be Running");
		logDHCPMess(logBuff, 0);
		exit(-1);
	}
	//SetEvent(rEvent);
*/

/*
	for (int i = 0; i < cfig.rangeCount; i++)
	{
		char *logPtr = logBuff;
		logPtr += sprintf(logPtr, "DHCP Range: ");
		logPtr += sprintf(logPtr, "%s", IP2String(ipbuff, htonl(cfig.dhcpRanges[i].rangeStart)));
		logPtr += sprintf(logPtr, "-%s", IP2String(ipbuff, htonl(cfig.dhcpRanges[i].rangeEnd)));
		logPtr += sprintf(logPtr, "/%s", IP2String(ipbuff, cfig.dhcpRanges[i].mask));
		logDHCPMess(logBuff, 1);
	}
*/

	cfig.dhcpReplConn.sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (cfig.dhcpReplConn.sock == INVALID_SOCKET)
	{
		sprintf(logBuff, "Failed to Create DHCP Replication Socket");
		logDHCPMess(logBuff, 1);
	}
	else
	{
		//printf("Socket %u\n", cfig.dhcpReplConn.sock);

		if (cfig.replication == 1)
			cfig.dhcpReplConn.server = cfig.zoneServers[0];
		else
			cfig.dhcpReplConn.server = cfig.zoneServers[1];

		cfig.dhcpReplConn.addr.sin_family = AF_INET;
		cfig.dhcpReplConn.addr.sin_addr.s_addr = cfig.dhcpReplConn.server;
		cfig.dhcpReplConn.addr.sin_port = 0;

		int nRet = bind(cfig.dhcpReplConn.sock, (sockaddr*)&cfig.dhcpReplConn.addr, sizeof(struct sockaddr_in));

		if (nRet == SOCKET_ERROR)
		{
			cfig.dhcpReplConn.ready = false;
			sprintf(logBuff, "DHCP Replication Server, Bind Failed");
			logDHCPMess(logBuff, 1);
		}
		else
		{
			cfig.dhcpReplConn.port = IPPORT_DHCPS;
			cfig.dhcpReplConn.loaded = true;
			cfig.dhcpReplConn.ready = true;

			data3 op;
			memset(&token, 0, sizeof(data9));
			token.vp = token.dhcpp.vend_data;
			token.messsize = sizeof(dhcp_packet);

			token.remote.sin_port = htons(IPPORT_DHCPS);
			token.remote.sin_family = AF_INET;

			if (cfig.replication == 1)
				token.remote.sin_addr.s_addr = cfig.zoneServers[1];
			else if (cfig.replication == 2)
				token.remote.sin_addr.s_addr = cfig.zoneServers[0];

			token.dhcpp.header.bp_op = BOOTP_REQUEST;
			token.dhcpp.header.bp_xid = t;
			strcpy(token.dhcpp.header.bp_sname, cfig.servername);
			token.dhcpp.header.bp_magic_num[0] = 99;
			token.dhcpp.header.bp_magic_num[1] = 130;
			token.dhcpp.header.bp_magic_num[2] = 83;
			token.dhcpp.header.bp_magic_num[3] = 99;

			op.opt_code = DHCP_OPTION_MESSAGETYPE;
			op.size = 1;
			op.value[0] = DHCP_MESS_INFORM;
			pvdata(&token, &op);

			//op.opt_code = DHCP_OPTION_HOSTNAME;
			//op.size = strlen(cfig.servername);
			//memcpy(op.value, cfig.servername, op.size);
			//pvdata(&token, &op);

			token.vp[0] = DHCP_OPTION_END;
			token.vp++;
			token.bytes = token.vp - (MYBYTE*)token.raw;

			if (cfig.replication == 2)
				_beginthread(sendToken, 0, 0);
		}
	}

	if (cfig.lease >= INT_MAX)
		sprintf(logBuff, "Default Lease: Infinity");
	else
		sprintf(logBuff, "Default Lease: %u (sec)", cfig.lease);

	logDHCPMess(logBuff, 1);

	if (cfig.replication == 1)
		sprintf(logBuff, "Server Name: %s (Primary)", cfig.servername);
	else if (cfig.replication == 2)
		sprintf(logBuff, "Server Name: %s (Secondary)", cfig.servername);
	else
		sprintf(logBuff, "Server Name: %s", cfig.servername);

	logDHCPMess(logBuff, 1);

	sprintf(logBuff, "Detecting Static Interfaces..");
	logDHCPMess(logBuff, 1);

	do
	{
		closeConn();
		getInterfaces(&network);

		network.maxFD = 0;
		bool bindfailed = false;

		int i = 0;

		for (int j = 0; j < MAX_SERVERS && network.listenServers[j]; j++)
		{
			network.dhcpConn[i].sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

			if (network.dhcpConn[i].sock == INVALID_SOCKET)
			{
				bindfailed = true;
				sprintf(logBuff, "Failed to Create Socket");
				logDHCPMess(logBuff, 1);
				continue;
			}

			//printf("Socket %u\n", network.dhcpConn[i].sock);

			network.dhcpConn[i].addr.sin_family = AF_INET;
			network.dhcpConn[i].addr.sin_addr.s_addr = network.listenServers[j];
			network.dhcpConn[i].addr.sin_port = htons(IPPORT_DHCPS);

			network.dhcpConn[i].broadCastVal = TRUE;
			network.dhcpConn[i].broadCastSize = sizeof(network.dhcpConn[i].broadCastVal);
			setsockopt(network.dhcpConn[i].sock, SOL_SOCKET, SO_BROADCAST, (char*)(&network.dhcpConn[i].broadCastVal), network.dhcpConn[i].broadCastSize);

			int nRet = bind(network.dhcpConn[i].sock,
							(sockaddr*)&network.dhcpConn[i].addr,
							sizeof(struct sockaddr_in)
						   );

			if (nRet == SOCKET_ERROR)
			{
				bindfailed = true;
				closesocket(network.dhcpConn[i].sock);
				sprintf(logBuff, "Warning: %s UDP Port 67 already in use", IP2String(ipbuff, network.listenServers[j]));
				logDHCPMess(logBuff, 1);
				continue;
			}

			network.dhcpConn[i].loaded = true;
			network.dhcpConn[i].ready = true;

			if (network.maxFD < network.dhcpConn[i].sock)
				network.maxFD = network.dhcpConn[i].sock;

			network.dhcpConn[i].server = network.listenServers[j];
			network.dhcpConn[i].mask = network.listenMasks[j];
			network.dhcpConn[i].port = IPPORT_DHCPS;

			i++;
		}

		network.httpConn.port = 6789;
		network.httpConn.server = inet_addr("127.0.0.1");

		if (f = openSection("HTTP_INTERFACE", 1))
		{
			while (readSection(raw, f))
			{
				mySplit(name, value, raw, '=');

				if (!strcasecmp(name, "HTTPServer"))
				{
					mySplit(name, value, value, ':');

					if (isIP(name))
					{
						network.httpConn.server = inet_addr(name);
					}
					else
					{
						network.httpConn.loaded = false;
						sprintf(logBuff, "Warning: Section [HTTP_INTERFACE], Invalid IP Address %s, ignored", name);
						logDHCPMess(logBuff, 1);
					}

					if (value[0])
					{
						if (atoi(value))
							network.httpConn.port = atoi(value);
						else
						{
							network.httpConn.loaded = false;
							sprintf(logBuff, "Warning: Section [HTTP_INTERFACE], Invalid port %s, ignored", value);
							logDHCPMess(logBuff, 1);
						}
					}

					if (network.httpConn.server != inet_addr("127.0.0.1") && !findServer(network.allServers, MAX_SERVERS, network.httpConn.server))
					{
						bindfailed = true;
						network.httpConn.loaded = false;
						sprintf(logBuff, "Warning: Section [HTTP_INTERFACE], %s not available, ignored", raw);
						logDHCPMess(logBuff, 1);
					}
				}
				else if (!strcasecmp(name, "HTTPClient"))
				{
					if (isIP(value))
						addServer(cfig.httpClients, 8, inet_addr(value));
					else
					{
						sprintf(logBuff, "Warning: Section [HTTP_INTERFACE], invalid client IP %s, ignored", raw);
						logDHCPMess(logBuff, 1);
					}
				}
				else if (!strcasecmp(name, "HTTPTitle"))
				{
					strncpy(htmlTitle, value, 255);
					htmlTitle[255] = 0;
				}
				else
				{
					sprintf(logBuff, "Warning: Section [HTTP_INTERFACE], invalid entry %s, ignored", raw);
					logDHCPMess(logBuff, 1);
				}
			}
		}

		if (!htmlTitle[0])
			sprintf(htmlTitle, "Open DHCP Server on %s", cfig.servername);

		network.httpConn.sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

		if (network.httpConn.sock == INVALID_SOCKET)
		{
			bindfailed = true;
			sprintf(logBuff, "Failed to Create Socket");
			logDHCPMess(logBuff, 1);
		}
		else
		{
			//printf("Socket %u\n", network.httpConn.sock);

			network.httpConn.addr.sin_family = AF_INET;
			network.httpConn.addr.sin_addr.s_addr = network.httpConn.server;
			network.httpConn.addr.sin_port = htons(network.httpConn.port);

			int nRet = bind(network.httpConn.sock, (sockaddr*)&network.httpConn.addr, sizeof(struct sockaddr_in));

			if (nRet == SOCKET_ERROR)
			{
				bindfailed = true;
				sprintf(logBuff, "Http Interface %s TCP Port %u not available", IP2String(ipbuff, network.httpConn.server), network.httpConn.port);
				logDHCPMess(logBuff, 1);
				closesocket(network.httpConn.sock);
			}
			else
			{
				nRet = listen(network.httpConn.sock, SOMAXCONN);

				if (nRet == SOCKET_ERROR)
				{
					bindfailed = true;
					sprintf(logBuff, "%s TCP Port %u Error on Listen", IP2String(ipbuff, network.httpConn.server), network.httpConn.port);
					logDHCPMess(logBuff, 1);
					closesocket(network.httpConn.sock);
				}
				else
				{
					network.httpConn.loaded = true;
					network.httpConn.ready = true;

					if (network.httpConn.sock > network.maxFD)
						network.maxFD = network.httpConn.sock;
				}
			}
		}

		network.maxFD++;

		for (MYBYTE m = 0; m < MAX_SERVERS && network.allServers[m]; m++)
			lockIP(network.allServers[m]);

		if (bindfailed)
			cfig.failureCount++;
		else
			cfig.failureCount = 0;

		if (!network.dhcpConn[0].ready)
		{
			sprintf(logBuff, "No Static Interface ready, Waiting...");
			logDHCPMess(logBuff, 1);
			continue;
		}

		sprintf(logBuff, "Lease Status URL: http://%s:%u", IP2String(ipbuff, network.httpConn.server), network.httpConn.port);
		logDHCPMess(logBuff, 1);
		FILE *f = fopen(htmFile, "wt");

		if (f)
		{
			fprintf(f, "<html><head><meta http-equiv=\"refresh\" content=\"0;url=http://%s:%u\"</head></html>", IP2String(ipbuff, network.httpConn.server), network.httpConn.port);
			fclose(f);
		}
		else
		{
			FILE *f = fopen(htmFile, "wt");

			if (f)
			{
				fprintf(f, "<html><body><h2>DHCP/HTTP Service is not running</h2></body></html>");
				fclose(f);
			}
		}

		for (int i = 0; i < MAX_SERVERS && network.staticServers[i]; i++)
		{
			for (MYBYTE j = 0; j < MAX_SERVERS; j++)
			{
				if (network.dhcpConn[j].server == network.staticServers[i])
				{
					sprintf(logBuff, "Listening On: %s", IP2String(ipbuff, network.staticServers[i]));
					logDHCPMess(logBuff, 1);
					break;
				}
			}
		}

	} while (kRunning && detectChange());

	_endthread();
	return;
}

bool detectChange()
{
	char logBuff[512];
	//debug("Calling detectChange()");

	network.ready = true;

	if (cfig.failureCount)
	{
		MYDWORD eventWait = (MYDWORD)(10000 * pow(2, cfig.failureCount));
		Sleep(eventWait);
		sprintf(logBuff, "Retrying failed Listening Interfaces..");
		logDHCPMess(logBuff, 1);
		network.ready = false;

		while (network.busy)
			Sleep(500);

		return true;
	}

	DWORD ret = NotifyAddrChange(NULL, NULL);

	if ((errno = WSAGetLastError()) && errno != WSA_IO_PENDING)
	{
		sprintf(logBuff, "NotifyAddrChange error...%d", errno);
		logDHCPMess(logBuff, 1);
	}

	Sleep(1000);
	sprintf(logBuff, "Network changed, re-detecting Static Interfaces..");
	logDHCPMess(logBuff, 1);
	network.ready = false;

	while (network.busy)
		Sleep(500);

	return true;
}

void getInterfaces(data1 *network)
{
	char logBuff[512];
	char ipbuff[32];

	memset(network, 0, sizeof(data1));

	SOCKET sd = WSASocket(PF_INET, SOCK_DGRAM, 0, 0, 0, 0);

	if (sd == INVALID_SOCKET)
		return;

	INTERFACE_INFO InterfaceList[MAX_SERVERS];
	unsigned long nBytesReturned;

	if (WSAIoctl(sd, SIO_GET_INTERFACE_LIST, 0, 0, &InterfaceList, sizeof(InterfaceList), &nBytesReturned, 0, 0) == SOCKET_ERROR)
		return;

	int nNumInterfaces = nBytesReturned / sizeof(INTERFACE_INFO);

	for (int i = 0; i < nNumInterfaces; ++i)
	{
		sockaddr_in *pAddress = (sockaddr_in*)&(InterfaceList[i].iiAddress);
		u_long nFlags = InterfaceList[i].iiFlags;

		//if (!((nFlags & IFF_POINTTOPOINT)))
		if (!((nFlags & IFF_POINTTOPOINT) || (nFlags & IFF_LOOPBACK)))
		{
			addServer(network->allServers, MAX_SERVERS, pAddress->sin_addr.s_addr);
		}
	}

	closesocket(sd);

	PIP_ADAPTER_INFO pAdapterInfo;
	PIP_ADAPTER_INFO pAdapter;

	pAdapterInfo = (IP_ADAPTER_INFO*) calloc(1, sizeof(IP_ADAPTER_INFO));
	DWORD ulOutBufLen = sizeof(IP_ADAPTER_INFO);

	if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW)
	{
		free(pAdapterInfo);
		pAdapterInfo = (IP_ADAPTER_INFO*)calloc(1, ulOutBufLen);
	}

	if ((GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) == NO_ERROR)
	{
		pAdapter = pAdapterInfo;
		while (pAdapter)
		{
			if (!pAdapter->DhcpEnabled)
			{
				IP_ADDR_STRING *sList = &pAdapter->IpAddressList;
				while (sList)
				{
					MYDWORD iaddr = inet_addr(sList->IpAddress.String);

					if (iaddr)
					{
						for (MYBYTE k = 0; k < MAX_SERVERS; k++)
						{
							if (network->staticServers[k] == iaddr)
								break;
							else if (!network->staticServers[k])
							{
								network->staticServers[k] = iaddr;
								network->staticMasks[k] = inet_addr(sList->IpMask.String);
								break;
							}
						}
					}
					sList = sList->Next;
				}
			}
			pAdapter = pAdapter->Next;
		}
		free(pAdapterInfo);
	}

	if (cfig.specifiedServers[0])
	{
		for (MYBYTE i = 0; i < MAX_SERVERS && cfig.specifiedServers[i]; i++)
		{
			MYBYTE j = 0;

			for (; j < MAX_SERVERS && network->staticServers[j]; j++)
			{
				if (network->staticServers[j] == cfig.specifiedServers[i])
				{
					MYBYTE k = addServer(network->listenServers, MAX_SERVERS, network->staticServers[j]);

					if (k < MAX_SERVERS)
						network->listenMasks[k] = network->staticMasks[j];

					break;
				}
			}

			if (j == MAX_SERVERS || !network->staticServers[j])
			{
				if (findServer(network->allServers, MAX_SERVERS, cfig.specifiedServers[i]))
					sprintf(logBuff, "Warning: Section [LISTEN_ON] Interface %s is not static, ignored", IP2String(ipbuff, cfig.specifiedServers[i]));
				else
					sprintf(logBuff, "Warning: Section [LISTEN_ON] Interface %s is not found, ignored", IP2String(ipbuff, cfig.specifiedServers[i]));

				logDHCPMess(logBuff, 2);
			}
		}
	}
	else
	{
		for (MYBYTE i = 0; i < MAX_SERVERS && network->allServers[i]; i++)
		{
			MYBYTE j = 0;

			for (; j < MAX_SERVERS && network->staticServers[j]; j++)
			{
				if (network->staticServers[j] == network->allServers[i])
				{
					MYBYTE k = addServer(network->listenServers, MAX_SERVERS, network->staticServers[j]);

					if (k < MAX_SERVERS)
						network->listenMasks[k] = network->staticMasks[j];

					break;
				}
			}

			if (j == MAX_SERVERS || !network->staticServers[j])
			{
				sprintf(logBuff, "Warning: Interface %s is not Static, ignored", IP2String(ipbuff, network->allServers[i]));
				logDHCPMess(logBuff, 2);
			}
		}
	}

	FIXED_INFO *FixedInfo;
	IP_ADDR_STRING *pIPAddr;

	FixedInfo = (FIXED_INFO*)GlobalAlloc(GPTR, sizeof(FIXED_INFO));
	ulOutBufLen = sizeof(FIXED_INFO);

	if (ERROR_BUFFER_OVERFLOW == GetNetworkParams(FixedInfo, &ulOutBufLen))
	{
		GlobalFree(FixedInfo);
		FixedInfo = (FIXED_INFO*)GlobalAlloc(GPTR, ulOutBufLen);
	}

	if (!GetNetworkParams(FixedInfo, &ulOutBufLen))
	{
		if (!cfig.servername[0])
			strcpy(cfig.servername, FixedInfo->HostName);

		//printf("d=%u=%s", strlen(FixedInfo->DomainName), FixedInfo->DomainName);

		GlobalFree(FixedInfo);
	}

	return;
}

void __cdecl updateStateFile(void *lpParam)
{
	WaitForSingleObject(fEvent, INFINITE);

	data7 *dhcpEntry = (data7*)lpParam;
	data8 dhcpData;
	memset(&dhcpData, 0, sizeof(data8));
	MYBYTE bp_hlen =  16;
	getHexValue(dhcpData.bp_chaddr, dhcpEntry->mapname, &bp_hlen);
	dhcpData.bp_hlen =  bp_hlen;
	dhcpData.ip = dhcpEntry->ip;
	dhcpData.subnetFlg = dhcpEntry->subnetFlg;
	dhcpData.expiry = dhcpEntry->expiry;
	dhcpData.local = dhcpEntry->local;
	strcpy(dhcpData.hostname, dhcpEntry->hostname);

	if (!dhcpEntry->dhcpInd)
	{
		cfig.dhcpInd += 1;
		dhcpEntry->dhcpInd = cfig.dhcpInd;
	}

	dhcpData.dhcpInd = dhcpEntry->dhcpInd;
	FILE *f = fopen(leaFile, "rb+");

	if (f)
	{
		if (fseek(f, (dhcpData.dhcpInd - 1)*sizeof(data8), SEEK_SET) >= 0)
			fwrite(&dhcpData, sizeof(data8), 1, f);

		fclose(f);
	}

	SetEvent(fEvent);
	_endthread();
	return;
}

MYWORD gdmess(data9 *req, MYBYTE sockInd)
{
	//debug("gdmess");
	char ipbuff[32];
	char logBuff[512];
	memset(req, 0, sizeof(data9));
	req->sockInd = sockInd;
	req->sockLen = sizeof(req->remote);
	errno = 0;

	req->bytes = recvfrom(network.dhcpConn[req->sockInd].sock,
	                      req->raw,
	                      sizeof(req->raw),
	                      0,
	                      (sockaddr*)&req->remote,
	                      &req->sockLen);

	//printf("IP=%s bytes=%u\n", IP2String(ipbuff,req->remote.sin_addr.s_addr), req->bytes);

	errno = WSAGetLastError();

	//printf("errno=%u\n", errno);

	if (errno || req->bytes <= 0)
	{
		sprintf(logBuff, "DHCP Message Recv Failed..");
		logDHCPMess(logBuff, 1);
		return 0;
	}

	if (req->dhcpp.header.bp_op != BOOTP_REQUEST)
		return 0;

	hex2String(req->chaddr, req->dhcpp.header.bp_chaddr, req->dhcpp.header.bp_hlen);

	data3 *op;
	MYBYTE *raw = req->dhcpp.vend_data;
	MYBYTE *rawEnd = raw + (req->bytes - sizeof(dhcp_header));

	for (; raw < rawEnd && *raw != DHCP_OPTION_END;)
	{
		op = (data3*)raw;
		//printf("OpCode=%u,MessType=%u\n", op->opt_code, op->value[0]);

		switch (op->opt_code)
		{
			case DHCP_OPTION_PAD:
				raw++;
				continue;

			case DHCP_OPTION_PARAMREQLIST:
				for (int ix = 0; ix < op->size; ix++)
					req->paramreqlist[op->value[ix]] = 1;
				break;

			case DHCP_OPTION_MESSAGETYPE:
				req->req_type = op->value[0];
				break;

			case DHCP_OPTION_SERVERID:
				req->server = fIP(op->value);
				break;

			case DHCP_OPTION_IPADDRLEASE:
				req->lease = fULong(op->value);
				break;

			case DHCP_OPTION_MAXDHCPMSGSIZE:
				req->messsize = fUShort(op->value);
				break;

			case DHCP_OPTION_REQUESTEDIPADDR:
				req->reqIP = fIP(op->value);
				break;

			case DHCP_OPTION_HOSTNAME:
				memcpy(req->hostname, op->value, op->size);
				req->hostname[op->size] = 0;
				req->hostname[64] = 0;

				if (char *ptr = strchr(req->hostname, '.'))
					*ptr = 0;

				break;

			case DHCP_OPTION_VENDORCLASSID:
				memcpy(&req->vendClass, op, op->size + 2);
				break;

			case DHCP_OPTION_USERCLASS:
				memcpy(&req->userClass, op, op->size + 2);
				break;

			case DHCP_OPTION_RELAYAGENTINFO:
				memcpy(&req->agentOption, op, op->size + 2);
				break;

			case DHCP_OPTION_CLIENTID:
				memcpy(&req->clientId, op, op->size + 2);
				break;

			case DHCP_OPTION_SUBNETSELECTION:
				memcpy(&req->subnet, op, op->size + 2);
				req->subnetIP = fIP(op->value);
				break;

			case DHCP_OPTION_REBINDINGTIME:
				req->rebind = fULong(op->value);
				break;
		}
		raw += 2;
		raw += op->size;
	}

	if (!req->subnetIP)
		req->subnetIP = req->dhcpp.header.bp_giaddr;

	if (!req->messsize)
	{
		if (req->req_type == DHCP_MESS_NONE)
			req->messsize = req->bytes;
		else
			req->messsize = sizeof(dhcp_packet);
	}

//	if (!req->hostname[0] && req->dhcpp.header.bp_ciaddr)
//	{
//		data7* cache = findEntry(IP2String(ipbuff, htonl(req->dhcpp.header.bp_ciaddr)), DNS_TYPE_PTR);
//
//		if (cache)
//			strcpy(req->hostname, cache->hostname);
//	}
//
//	if ((req->req_type == 1 || req->req_type == 3) && cfig.dhcpLogLevel == 3)
//	{
//		data9 *req1 = (data9*)calloc(1, sizeof(data9));
//		memcpy(req1, req, sizeof(data9));
//		_beginthread(logDebug, 0, req1);
//	}

	if (verbatim || cfig.dhcpLogLevel >= 2)
	{
		if (req->req_type == DHCP_MESS_NONE)
		{
			if (req->dhcpp.header.bp_giaddr)
				sprintf(logBuff, "BOOTPREQUEST for %s (%s) from RelayAgent %s received", req->chaddr, req->hostname, IP2String(ipbuff, req->dhcpp.header.bp_giaddr));
			else
				sprintf(logBuff, "BOOTPREQUEST for %s (%s) from interface %s received", req->chaddr, req->hostname, IP2String(ipbuff, network.dhcpConn[req->sockInd].server));

			logDHCPMess(logBuff, 2);
		}
		else if (req->req_type == DHCP_MESS_DISCOVER)
		{
			if (req->dhcpp.header.bp_giaddr)
				sprintf(logBuff, "DHCPDISCOVER for %s (%s) from RelayAgent %s received", req->chaddr, req->hostname, IP2String(ipbuff, req->dhcpp.header.bp_giaddr));
			else
				sprintf(logBuff, "DHCPDISCOVER for %s (%s) from interface %s received", req->chaddr, req->hostname, IP2String(ipbuff, network.dhcpConn[req->sockInd].server));

			logDHCPMess(logBuff, 2);
		}
		else if (req->req_type == DHCP_MESS_REQUEST)
		{
			if ((!req->server) || req->server == network.dhcpConn[req->sockInd].server)
			{
				if (req->dhcpp.header.bp_giaddr)
					sprintf(logBuff, "DHCPREQUEST for %s (%s) from RelayAgent %s received", req->chaddr, req->hostname, IP2String(ipbuff, req->dhcpp.header.bp_giaddr));
				else
					sprintf(logBuff, "DHCPREQUEST for %s (%s) from interface %s received", req->chaddr, req->hostname, IP2String(ipbuff, network.dhcpConn[req->sockInd].server));

				logDHCPMess(logBuff, 2);
			}
		}
	}

	req->vp = req->dhcpp.vend_data;
	memset(req->vp, 0, sizeof(dhcp_packet) - sizeof(dhcp_header));
	//printf("end bytes=%u\n", req->bytes);

	return 1;
}

void debug(int i)
{
	char t[254];
	sprintf(t, "%i", i);
	logDHCPMess(t, 1);
}

void debug(const char *mess)
{
	char t[254];
	strcpy(t, mess);
	logDHCPMess(t, 1);
}

void logDirect(char *mess)
{
	tm *ttm = localtime(&t);
	char buffer[_MAX_PATH];
	strftime(buffer, sizeof(buffer), logFile, ttm);

	if (strcmp(cfig.logFileName, buffer))
	{
		if (cfig.logFileName[0])
		{
			FILE *f = fopen(cfig.logFileName, "at");

			if (f)
			{
				fprintf(f, "Logging Continued on file %s\n", buffer);
				fclose(f);
			}

			strcpy(cfig.logFileName, buffer);
			f = fopen(cfig.logFileName, "at");

			if (f)
			{
				fprintf(f, "%s\n\n", sVersion);
				fclose(f);
			}
		}

		strcpy(cfig.logFileName, buffer);
		WritePrivateProfileString("InternetShortcut","URL", buffer, lnkFile);
		WritePrivateProfileString("InternetShortcut","IconIndex", "0", lnkFile);
		WritePrivateProfileString("InternetShortcut","IconFile", buffer, lnkFile);
	}

	FILE *f = fopen(cfig.logFileName, "at");

	if (f)
	{
		strftime(buffer, sizeof(buffer), "%d-%b-%y %X", ttm);
		fprintf(f, "[%s] %s\n", buffer, mess);
		fclose(f);
	}
//	else
//	{
//		cfig.dnsLogLevel = 0;
//		cfig.dhcpLogLevel = 0;
//	}

	return;
}

void __cdecl logThread(void *lpParam)
{
	WaitForSingleObject(lEvent, INFINITE);
	char *mess = (char*)lpParam;
	time_t t = time(NULL);
	tm *ttm = localtime(&t);
	char buffer[_MAX_PATH];
	strftime(buffer, sizeof(buffer), logFile, ttm);

	if (strcmp(cfig.logFileName, buffer))
	{
		if (cfig.logFileName[0])
		{
			FILE *f = fopen(cfig.logFileName, "at");

			if (f)
			{
				fprintf(f, "Logging Continued on file %s\n", buffer);
				fclose(f);
			}

			strcpy(cfig.logFileName, buffer);
			f = fopen(cfig.logFileName, "at");

			if (f)
			{
				fprintf(f, "%s\n\n", sVersion);
				fclose(f);
			}
		}

		strcpy(cfig.logFileName, buffer);
		WritePrivateProfileString("InternetShortcut","URL", buffer, lnkFile);
		WritePrivateProfileString("InternetShortcut","IconIndex", "0", lnkFile);
		WritePrivateProfileString("InternetShortcut","IconFile", buffer, lnkFile);
	}

	FILE *f = fopen(cfig.logFileName, "at");

	if (f)
	{
		strftime(buffer, sizeof(buffer), "%d-%b-%y %X", ttm);
		fprintf(f, "[%s] %s\n", buffer, mess);
		fclose(f);
	}
//	else
//	{
//		cfig.dnsLogLevel = 0;
//		cfig.dhcpLogLevel = 0;
//	}

	free(mess);
	SetEvent(lEvent);

	_endthread();
	return;
}

/*
void __cdecl logDebug(void *lpParam)
{
	char localBuff[1024];
	char localreq->extbuff[256];
	data9 *req = (data9*)lpParam;
	genHostName(localBuff, req->dhcpp.header.bp_chaddr, req->dhcpp.header.bp_hlen);
	sprintf(localreq->extbuff, cliFile, localBuff);
	FILE *f = fopen(localreq->extbuff, "at");

	if (f)
	{
		tm *ttm = localtime(&t);
		strftime(localreq->extbuff, sizeof(localreq->extbuff), "%d-%m-%y %X", ttm);

		char *s = localBuff;
		s += sprintf(s, localreq->extbuff);
		s += sprintf(s, " SourceMac=%s", req->chaddr);
		s += sprintf(s, " ClientIP=%s", IP2String(localreq->extbuff, req->dhcpp.header.bp_ciaddr));
		s += sprintf(s, " SourceIP=%s", IP2String(localreq->extbuff, req->remote.sin_addr.s_addr));
		s += sprintf(s, " RelayAgent=%s", IP2String(localreq->extbuff, req->dhcpp.header.bp_giaddr));
		fprintf(f, "%s\n", localBuff);

		data3 *op;
		MYBYTE *raw = req->dhcpp.vend_data;
		MYBYTE *rawEnd = raw + (req->bytes - sizeof(dhcp_header));
		MYBYTE maxInd = sizeof(opData) / sizeof(data4);

		for (; raw < rawEnd && *raw != DHCP_OPTION_END;)
		{
			op = (data3*)raw;

			BYTE opType = 2;
			char opName[40] = "Private";

			for (MYBYTE i = 0; i < maxInd; i++)
				if (op->opt_code == opData[i].opTag)
				{
					strcpy(opName, opData[i].opName);
					opType = opData[i].opType;
					break;
				}

			s = localBuff;
			s += sprintf(s, "\t%d\t%s\t", op->opt_code, opName);
			//printf("OpCode=%u,OpLen=%u,OpType=%u\n", op->opt_code, op->size, opType);

			switch (opType)
			{
				case 1:
					memcpy(localreq->extbuff, op->value, op->size);
					localreq->extbuff[op->size] = 0;
					sprintf(s, "%s", localreq->extbuff);
					break;
				case 3:
					for (BYTE x = 4; x <= op->size; x += 4)
					{
						IP2String(localreq->extbuff, fIP(op->value));
						s += sprintf(s, "%s,", localreq->extbuff);
					}
					break;
				case 4:
					sprintf(s, "%u", fULong(op->value));
					break;
				case 5:
					sprintf(s, "%u", fUShort(op->value));
					break;
				case 6:
				case 7:
					sprintf(s, "%u", op->value[0]);
					break;
				default:
					if (op->size == 1)
						sprintf(s, "%u", op->value[0]);
					else
						hex2String(s, op->value, op->size);
					break;
			}

			fprintf(f, "%s\n", localBuff);
			raw += 2;
			raw += op->size;
		}
		fclose(f);
	}
	free(req);
}
*/

void logDHCPMess(char *logBuff, MYBYTE logLevel)
{
	if (verbatim)
		printf("%s\n", logBuff);

	if (logLevel <= cfig.dhcpLogLevel)
	{
		char *mess = cloneString(logBuff);
		_beginthread(logThread, 0, mess);
	}
}

data7 *createCache(data71 *lump)
{
	MYWORD dataSize = 4 + sizeof(data7) + strlen(lump->mapname);
	data7 *cache = NULL;

	dataSize += 64;
	dataSize += lump->optionSize;
	cache = (data7*)calloc(1, dataSize);

	if (!cache)
		return NULL;

	MYBYTE *dp = &cache->data;
	cache->mapname = (char*)dp;
	strcpy(cache->mapname, lump->mapname);
	myLower(cache->mapname);
	dp += strlen(cache->mapname);
	dp++;
	cache->hostname = (char*)dp;

	if (lump->hostname)
		strcpy(cache->hostname, lump->hostname);

	dp += 65;

	if (lump->options && lump->optionSize >= 5)
	{
		cache->options = dp;
		memcpy(cache->options, lump->options, lump->optionSize);
	}

	//sprintf(logBuff, "New Cache cType=%d dnsType=%u name=%s", cache->cType, cache->dnsType, cache->name);
	//logDHCPMess(logBuff, 1);
	return cache;
}
