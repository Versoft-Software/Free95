i686-w64-mingw32-gcc -nostdlib -o open.exe applications/program.c applications/appinclude/print.c -Iapplications/appinclude
i686-w64-mingw32-gcc -nostdlib -o hello.exe applications/hello.c applications/appinclude/print.c -Iapplications/appinclude
i686-w64-mingw32-gcc -nostdlib -o freever.exe applications/buildno.c applications/appinclude/print.c -Iapplications/appinclude
i686-w64-mingw32-gcc -nostdlib -o lsbin.exe applications/lsbin.c applications/appinclude/print.c -Iapplications/appinclude
i686-w64-mingw32-gcc -nostdlib -o stop.exe applications/shutdown.c
i686-w64-mingw32-gcc -nostdlib -o reboot.exe applications/reboot.c
i686-w64-mingw32-gcc -nostdlib -o bsod.exe applications/bsod.c applications/appinclude/print.c -Iapplications/appinclude
