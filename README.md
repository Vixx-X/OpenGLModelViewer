# OpenGLModelViewer

[![image](https://user-images.githubusercontent.com/40926097/197443849-c8bee922-4cb8-4d9c-852b-814ed258b697.png)](https://github.com/Vixx-X/OpenGLModelViewer)

## Clone recursively

```
git clone https://github.com/Vixx-X/OpenGLModelViewer.git --recursive
```

## Window

Run premake

For vs 2022:

```
call vendor\bin\premake\premake5.exe vs2022
```

Other options are:

```
-- Options -------------- Generate project files for ----------------
   gmake    |   Generate GNU makefiles for POSIX, MinGW, and Cygwin
   vs2005   |   Generate Visual Studio 2005 project files
   vs2008   |   Generate Visual Studio 2008 project files
   vs2010   |   Generate Visual Studio 2010 project files
   vs2012   |   Generate Visual Studio 2012 project files
   vs2013   |   Generate Visual Studio 2013 project files
   vs2015   |   Generate Visual Studio 2015 project files
   vs2017   |   Generate Visual Studio 2017 project files
   vs2019   |   Generate Visual Studio 2019 project files
   vs2022   |   Generate Visual Studio 2022 project files
```

Open project file, compile project and run.

## Linux

Run premake

```
./vendor/bin/premake/premake5 gmake
```

Compile project and run.

```
make && ./bin/Debug/OpenGLModelViewer
```
