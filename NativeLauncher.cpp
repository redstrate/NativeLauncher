#define WIN32_LEAN_AND_MEAN 1

#include <iostream>
#include <windows.h>
#include <accctrl.h>
#include <aclapi.h>
#include <iomanip>
#include <sstream>
#include <winsock2.h>

bool disable_debug_privilege() {
   HANDLE hToken = NULL;
   LUID luidDebugPrivilege;
   PRIVILEGE_SET RequiredPrivileges;
   BOOL bResult;

   if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES, &hToken)) {
      std::cerr << "OpenProcessToken failed: " << GetLastError() << std::endl;
      return false;
   }

   if (!LookupPrivilegeValue(NULL, L"SeDebugPrivilege", &luidDebugPrivilege)) {
      std::cerr << "LookupPrivilegeValue failed: " << GetLastError() << std::endl;
      return false;
   }

   RequiredPrivileges.PrivilegeCount = 1;
   RequiredPrivileges.Control = PRIVILEGE_SET_ALL_NECESSARY;

   RequiredPrivileges.Privilege[0].Luid = luidDebugPrivilege;
   RequiredPrivileges.Privilege[0].Attributes = SE_PRIVILEGE_ENABLED;

   PrivilegeCheck(hToken, &RequiredPrivileges, &bResult);

   if (bResult) { // SeDebugPrivilege is enabled; try disabling it
      TOKEN_PRIVILEGES TokenPrivileges;
      TokenPrivileges.PrivilegeCount = 1;
      TokenPrivileges.Privileges[0].Luid = luidDebugPrivilege;
      TokenPrivileges.Privileges[0].Attributes = SE_PRIVILEGE_REMOVED;

      if (!AdjustTokenPrivileges(hToken, FALSE, &TokenPrivileges, 0, NULL, 0)) {
         std::cerr << "AdjustTokenPrivileges failed: " << GetLastError() << std::endl;
         return false;
      }
   }

   return true;
}

struct handle_data {
   unsigned long process_id;
   HWND window_handle;
};

BOOL CALLBACK enum_windows_callback(HWND handle, LPARAM lParam) {
   handle_data& data = *(handle_data*)lParam;
   unsigned long process_id = 0;
   GetWindowThreadProcessId(handle, &process_id);
   if (data.process_id != process_id)
      return TRUE;

   data.window_handle = handle;

   return FALSE;
}

bool has_window(unsigned long process_id) {
   handle_data data;
   data.process_id = process_id;
   data.window_handle = 0;
   EnumWindows(enum_windows_callback, (LPARAM)&data);

   return data.window_handle != nullptr;
}

int launch_game(wchar_t* appC, wchar_t* argC) {
   std::wstring app(appC);
   std::wstring arg(argC);

   std::wstring input = app + L" " + arg;
   wchar_t* arg_concat = const_cast<wchar_t*>(input.c_str());
   const wchar_t* app_const = app.c_str();

   TCHAR username[256];
   DWORD size = 256;
   if (!GetUserName((TCHAR*)username, &size)) {
      std::cerr << "GetUserName failed: " << GetLastError() << std::endl;
      return -1;
   }

   EXPLICIT_ACCESS pExplicitAccess;
   ZeroMemory(&pExplicitAccess, sizeof(pExplicitAccess));
   BuildExplicitAccessWithName(&pExplicitAccess, username, 0x001fffdf, GRANT_ACCESS, 0);

   PACL NewAcl;
   SetEntriesInAcl(1u, &pExplicitAccess, nullptr, &NewAcl);

   SECURITY_DESCRIPTOR secDes;
   ZeroMemory(&secDes, sizeof(secDes));
   if (!InitializeSecurityDescriptor(&secDes, 1u)) {
      std::cerr << "InitializeSecurityDescriptor failed: " << GetLastError() << std::endl;
      return -1;
   }

   if (!SetSecurityDescriptorDacl(&secDes, true, NewAcl, false)) {
      std::cerr << "SetSecurityDescriptorDacl failed: " << GetLastError() << std::endl;
      return -1;
   }

   STARTUPINFO si;
   PROCESS_INFORMATION pi;

   ZeroMemory(&si, sizeof(si));
   si.cb = sizeof(si);
   ZeroMemory(&pi, sizeof(pi));

   SECURITY_ATTRIBUTES pSec;
   ZeroMemory(&pSec, sizeof(pSec));
   pSec.nLength = sizeof(pSec);
   pSec.lpSecurityDescriptor = &secDes;
   pSec.bInheritHandle = false;

   if (!CreateProcess(nullptr, arg_concat, &pSec, nullptr, false, 0x20, nullptr, nullptr, &si, &pi)) {
      std::cerr << "CreateProcess failed: " << GetLastError() << std::endl;
      return -1;
   }

   while (!has_window(pi.dwProcessId)) {
      Sleep(10);
   }

   PACL myAcl;
   auto gsi = GetSecurityInfo(GetCurrentProcess(), SE_KERNEL_OBJECT, DACL_SECURITY_INFORMATION, nullptr, nullptr, &myAcl, nullptr, nullptr);
   if (gsi != ERROR_SUCCESS) {
      std::cerr << "GetSecurityInfo failed: " << gsi << std::endl;
      return -1;
   }

   auto ssi = SetSecurityInfo(pi.hProcess, SE_KERNEL_OBJECT, DACL_SECURITY_INFORMATION | UNPROTECTED_DACL_SECURITY_INFORMATION, nullptr, nullptr, myAcl, nullptr);
   if (ssi != ERROR_SUCCESS) {
      std::cerr << "SetSecurityInfo failed: " << ssi << std::endl;
      return -1;
   }

   CloseHandle(pi.hProcess);
   CloseHandle(pi.hThread);

   return pi.dwProcessId;
}

void pipe_to_ipc(int port, int code) {
   WSADATA wsaData {};
   WSAStartup(MAKEWORD(2,2), &wsaData);

   SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

   sockaddr_in serverAddr {};
   serverAddr.sin_family = AF_INET;
   serverAddr.sin_port = htons(port);
   serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

   connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));

   char buffer[50] {};
   sprintf(buffer, "%d", code);

   send(clientSocket, buffer, strlen(buffer), 0 );
}

int wmain(int argc, wchar_t* argv[]) {
   if (argc < 4) {
      std::cerr << "Usage: NativeLauncher.exe [port] [executable] [game args]" << std::endl;
      return -1;
   }

   disable_debug_privilege();

   auto pid = launch_game(argv[2], argv[3]);

   pipe_to_ipc(wcstol(argv[1], 0, 10), pid);

   return 1;
}
