## Сборка из исходников | logMat
---
### Windows
1. Установите [Инструменты сборки для Visual Studio 2022](https://visualstudio.microsoft.com/ru/downloads/)
2. Установите [CMake](https://cmake.org/download/)
3. Установите [git](https://git-scm.com/downloads/win)
4. Установите [vcpkg](https://vcpkg.io/en/) через [git](https://git-scm.com)
	```
	C:\> cd \
	C:\> mkdir dev
	C:\> cd dev
	C:\> git clone https://github.com/microsoft/vcpkg
	С:\> cd vcpkg
	C:\> .\bootstrap-vcpkg.bat
	```
	Настройте переменную среды VCPKG_ROOT.
	```
	$env:VCPKG_ROOT = "C:\dev\vcpkg"
	$env:PATH = "$env:VCPKG_ROOT;$env:PATH"
	```
5. Установите [drogon](https://github.com/drogonframework/drogon?ysclid=m8zzrgpw1t969713409) через [vcpkg](https://vcpkg.io/en/)
	```
	C:\> vcpkg install jsoncpp:x64-windows zlib:x64-windows openssl:x64-windows sqlite3:x64-windows drogon[core,ctl,sqlite3,orm]:x64-windows
	```
6. Установите [Boost](https://www.boost.org/) через [vcpkg](https://vcpkg.io/en/)
	```
	C:\> vcpkg install boost:x64-windows
	```
7. Установите libjpeg-turbo через [vcpkg](https://vcpkg.io/en/)
	```
	C:\> vcpkg install libjpeg-turbo:x64-windows
	```
8. Установите libqrencode через [vcpkg](https://vcpkg.io/en/)
	```
	C:\> vcpkg install libqrencode:x64-windows
	```
9. Добавьте директори в переменную среды PATH
	```
	C:\dev\vcpkg\installed\x64-windows\tools\drogon
	C:\dev\vcpkg\installed\x64-windows\bin
	C:\dev\vcpkg\installed\x64-windows\lib
	C:\dev\vcpkg\installed\x64-windows\include
	C:\dev\vcpkg\installed\x64-windows\share
	C:\dev\vcpkg\installed\x64-windows\debug\bin
	C:\dev\vcpkg\installed\x64-windows\debug\lib
	```
10. Произведите сборку
	```
	C:\> cmake -B build .
	C:\> cd build
	C:\> cmake --build . -j %NUMBER_OF_PROCESSORS%
	```