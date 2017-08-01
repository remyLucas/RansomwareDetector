
new-alias compiler "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\bin\cl.exe"
new-alias linker "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\bin\link.exe"

$INCLUDE = "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\include"
$INCLUDE_UM = "C:\Program Files (x86)\Windows Kits\8.1\Include\um"
$INCLUDE_SHARED = "C:\Program Files (x86)\Windows Kits\8.1\Include\shared"
$INCLUDE_UCRT = "C:\Program Files (x86)\Windows Kits\10\Include\10.0.10150.0\ucrt"

$LIB_DIR="C:\Program Files (x86)\Windows Kits\8.1\Lib\winv6.3\um\x86"
$LIB_DIR_2 = "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\lib"
$LIB_UCRT_DIR="C:\Program Files (x86)\Windows Kits\10\Lib\10.0.10240.0\ucrt\x86"

$sources = Get-ChildItem "src"
for($i=0;$i -lt $sources.count;$i++)
{
	$filename = $sources[$i].name
	compiler /c /nologo /W4 /WX- /Od /D WIN32 /D _CONSOLE /Foobj\ /Tc src\$filename /I.\inc /I$INCLUDE /I$INCLUDE_UM /I$INCLUDE_SHARED /I$INCLUDE_UCRT 
	#compiler /c /nologo /W4 /WX- /Od /D WIN32 /D _WINDOWS /Foobj\ /Tc src\$filename /I.\inc /I$INCLUDE /I$INCLUDE_UM /I$INCLUDE_SHARED /I$INCLUDE_UCRT 
}

$objects = Get-ChildItem "obj"
$OBJ = New-Object System.Collections.ArrayList
for($i=0;$i -lt $objects.count;$i++)
{
	$filename = $objects[$i].name
	$OBJ.Add("obj/$filename")
}

linker Advapi32.lib User32.lib $OBJ /OUT:bin\installer.exe /LIBPATH:$LIB_DIR /LIBPATH:$LIB_DIR_2 /LIBPATH:$LIB_UCRT_DIR /SUBSYSTEM:CONSOLE

