# nativelauncher
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

Of course there's always [a prebuilt EXE for you](https://xiv.zone/distrib/nativelauncher/NativeLauncher.exe) if can't compile it.

## Credit
* The XIVQuickLauncher team, who originally wrote this.
