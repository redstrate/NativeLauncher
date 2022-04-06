# nativelauncher
[![sourcehut](https://img.shields.io/badge/repository-sourcehut-lightgrey.svg?logo=data:image/svg+xml;base64,PHN2ZyB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHZlcnNpb249IjEuMSINCiAgICB3aWR0aD0iMTI4IiBoZWlnaHQ9IjEyOCI+DQogIDxkZWZzPg0KICAgIDxmaWx0ZXIgaWQ9InNoYWRvdyIgeD0iLTEwJSIgeT0iLTEwJSIgd2lkdGg9IjEyNSUiIGhlaWdodD0iMTI1JSI+DQogICAgICA8ZmVEcm9wU2hhZG93IGR4PSIwIiBkeT0iMCIgc3RkRGV2aWF0aW9uPSIxLjUiDQogICAgICAgIGZsb29kLWNvbG9yPSJibGFjayIgLz4NCiAgICA8L2ZpbHRlcj4NCiAgICA8ZmlsdGVyIGlkPSJ0ZXh0LXNoYWRvdyIgeD0iLTEwJSIgeT0iLTEwJSIgd2lkdGg9IjEyNSUiIGhlaWdodD0iMTI1JSI+DQogICAgICA8ZmVEcm9wU2hhZG93IGR4PSIwIiBkeT0iMCIgc3RkRGV2aWF0aW9uPSIxLjUiDQogICAgICAgIGZsb29kLWNvbG9yPSIjQUFBIiAvPg0KICAgIDwvZmlsdGVyPg0KICA8L2RlZnM+DQogIDxjaXJjbGUgY3g9IjUwJSIgY3k9IjUwJSIgcj0iMzglIiBzdHJva2U9IndoaXRlIiBzdHJva2Utd2lkdGg9IjQlIg0KICAgIGZpbGw9Im5vbmUiIGZpbHRlcj0idXJsKCNzaGFkb3cpIiAvPg0KICA8Y2lyY2xlIGN4PSI1MCUiIGN5PSI1MCUiIHI9IjM4JSIgc3Ryb2tlPSJ3aGl0ZSIgc3Ryb2tlLXdpZHRoPSI0JSINCiAgICBmaWxsPSJub25lIiBmaWx0ZXI9InVybCgjc2hhZG93KSIgLz4NCjwvc3ZnPg0KCg==)](https://git.sr.ht/~redstrate/nativelauncher)
[![GitHub
mirror](https://img.shields.io/badge/mirror-GitHub-black.svg?logo=github)](https://github.com/redstrate/nativelauncher)
[![ryne.moe
mirror](https://img.shields.io/badge/mirror-ryne.moe-red.svg?logo=git)](https://git.ryne.moe/redstrate/nativelauncher)

This is an ACL bypass for Win32 apps, but specifically is used for [Dalamud](https://github.com/goatcorp/Dalamud) injection into FFXIV. This is a C++ alternative for
[FFXIVQuickLauncher's own bypass which is in C#](https://github.com/goatcorp/FFXIVQuickLauncher/blob/master/src/XIVLauncher.Common.Windows/NativeAclFix.cs). This makes it usable for launchers which aren't in C# like [Astra](https://sr.ht/~redstrate/astra). This is mainly used for external use (i.e. calling NativeLauncher as a wrapper).

## Usage
* The first argument is the port number of your TCP server
* The second argument is the executable (usually ffxiv_dx11.exe)
* The rest of the arguments is passed as-is to your chosen executable
* If an error occured, it is printed into stderr

This communicates via IPC through TCP sockets. This is to handle usecases such as running within Wine which not only clogs up stdout but also throws away
the error code of the application which makes those two communication methods impossible.

The only data it will send over the TCP socket (and the port is specified as the second argument) is the PID number, which you can then use to
bootstrap Dalamud or any other injector.

I've been testing this under latest Wine Staging (as of writing, 6.22) and the process still works out-of-the box.

## Building
You can compile this using MinGW or MSVC. If you're using MSVC, just use the vcxproj. If you're trying to compile it using MinGW, use this command:

`x86_64-w64-mingw32-g++ -municode NativeLauncher.cpp -static -lwsock32 -o NativeLauncher.exe`

Of course there's always [a prebuilt EXE for you](https://github.com/redstrate/nativelauncher/releases/latest) if can't compile it.

## Credit
* The XIVQuickLauncher team, who originally wrote this.
