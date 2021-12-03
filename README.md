# nativelauncher
Full credit goes to the XIVQuickLauncher team, this is their old ACL bypass before it was rewritten in C#, but
it still works!

If you plan on using this on Linux or macOS, don't fear as I graciously built an [EXE for you](https://github.com/redstrate/nativelauncher/releases/latest). Right now they are built with VS2020 if that means anything to you.

## What is this?
This is a workaround used by Dalamud to implement a specific ACL bypass to allow them to freely read/write memory, or something? Currently, this is written in nice-looking C# but this was unusable for [xivlauncher](https://github.com/redstrate/xivlauncher) because I was using C++, but lo and behold they were actually using _this_ before!

The usage is extremely useful if you want to use this in your own launcher or game projects:

* The first argument is the executable (usually ffxiv_dx11.exe)
* The rest of the arguments is passed as-is to your chosen executable
* If this is done successfully, it will spit out a PID of the new process for you to use. If it fails, it'll tell you why.

I've been testing this under latest Wine Staging (as of writing, 6.22) and the process still works out-of-the box.
