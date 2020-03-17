@echo off

set name=instafun

rem set live_code_reloading=true
set live_code_reloading=false

set options=/nologo /Zi /Od /MTd /EHsc

set t=%time:~0,8%
set t=%t::=-%

set current=%cd%

if not exist build\ mkdir build
pushd build

del *.pdb > NUL

if %live_code_reloading% == true (
	set options=%options% /DLIVE_CODE_RELOADING

	echo compiling... > compile.lock
	cl %current%/source/instafun.cpp %options% /LD /INCREMENTAL:NO /link /PDB:"instafun%date%-%t%.pdb" 
	del compile.lock
)

cl /Fe%name% %current%/source/main.cpp %options% /link /INCREMENTAL:NO

popd