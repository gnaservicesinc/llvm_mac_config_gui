#!/bin/bash


SCRIPT_PATH="${BASH_SOURCE}"
while [ -L "${SCRIPT_PATH}" ]; do
  TARGET="$(readlink "${SCRIPT_PATH}")"
  if [[ "${TARGET}" == /* ]]; then
    SCRIPT_PATH="$TARGET"
  else
    SCRIPT_PATH="$(dirname "${SCRIPT_PATH}")/${TARGET}"
  fi
done

DEFAULT_IS_A_BOT=0
DEFAULT_LLVM_PATH="/opt/llvm/llvm-project"
DEFAULT_PYTHON_BIN_NAME="python3"
DEFAULT_O_LEVEL="2"
DEFAULT_AARCH="AArch64"
DEFAULT_XARCH="arm64"
SCRIPT_DIR="$(dirname "${SCRIPT_PATH}")"
SCRIPT_NAME="$(basename "${SCRIPT_PATH}")"
FFORCE_BYPASS=0
if [ "$1" = "help" ] || [ "$1" = "--help" ] || [ "$1" = "-help" ]
then
SHOWHELP=1;
else
SHOWHELP=0;
fi

SKIPGITPULL=0
DOINSTALL=0
SUDODOINSTALL=0
FFI=0
XML2=0
ZLIB=0
DRYRUN=0
USELTO=1
NOLTO=0
FULLLTO=0
USEMAKE=0
TERMINFO=0
USEDYLIB=0
XCODETOOLCHAIN=0
USEXCODEGCC=0
MODULESON=0
BACKTRACESON=0
USE_LOCALLY_INSTALLED_PYTHON=0
NOWARN=0
ENABLETESTING=0
USEBENCHMARK=0
RBYPAS=0
BOTMODE=0
WANTS_BOT_MODE=0
        FFORCE=0
_GO_GO_GADGET_BOTMODE=0

if [ "$DEFAULT_IS_A_BOT" -eq 1 ]
then
IS_A_BOT=1
WANTS_BOT_MODE=1
else
IS_A_BOT=0

          SHOWHELP=1;
fi



i=1;
j=$#;
while [ $i -le $j ]
do
	case $1 in
	'--compilerpath')
  		A_1="$2";;
	'--llvmdir')
 	 	A_2="$2";;
	'--builddir')
  		A_3="$2";;
	'--path')
  		A_4="$2";;
	'--timer_file')
  		A_5="$2";;
	'--projects')
  		A_6="$2";;
	'--runtime')
  		A_7="$2";;
	'--clean')
  		A_8="$2";;
	'--compiler')
  		A_9="$2";;
	'--cxxcompiler')
  		A_10="$2";;
	'--linker')
  		A_11="$2";;
    '--Olevel')
        A_12="$2";;
    '--arch')
        A_13="$2";;
    '--osx-arch')
        A_14="$2";;
	'--dryrun')
  		DRYRUN=1;;
	'--help')
  		SHOWHELP=1;;
    '--ffi')
          FFI=1;;
	'--modules')
  		MODULESON=1;;
    '--backtraces')
        BACKTRACESON=1;;
	'--xml2')
  		XML2=1;;
	'--zlib')
  		ZLIB=1;;
	'--terminfo')
  		TERMINFO=1;;
	'--nolto')
  		NOLTO=1;;
    '--gcc')
        USEXCODEGCC=1;;
	'--fulllto')
  		FULLLTO=1;;
	'--usedylib')
  		USEDYLIB=1;;
	'--xcodetoolchain')
  		XCODETOOLCHAIN=1;;
	'--usemake')
  		USEMAKE=1;;
	'--skipgitpull')
  		SKIPGITPULL=1;;
	'--doinstall')
  		DOINSTALL=1;;
	'--sudoinstall')
  		SUDODOINSTALL=1;;
    '--uselocalpython')
        USE_LOCALLY_INSTALLED_PYTHON=1;;
    '--donotwarn')
        NOWARN=1;;
    '--dotesting')
        ENABLETESTING=1;;
    '--benchmark')
        USEBENCHMARK=1;;
    '--bypass_no_root')
        RBYPAS=1;;
    '--force')
        FFORCE=1;;
    '--bot')
        IS_A_BOT=1;;
    '--botmode')
        WANTS_BOT_MODE=1;;
    '--hidden_go_go_gadget_botmode')
        _GO_GO_GADGET_BOTMODE=1;;

	esac
    i=$((i + 1));
    shift 1;
done

if [ "$_GO_GO_GADGET_BOTMODE"  -eq 1 ]
then
    BOTMODE=1
         OPTLEVEL="2";
        DEFAULT_LLVM_PATH="/opt/llvm/llvm-project";
        DEFAULT_PYTHON_BIN_NAME="python3";
        DEFAULT_O_LEVEL="2";
        DEFAULT_AARCH="AArch64";
        DEFAULT_XARCH="arm64";
fi
if [ "$RBYPAS" -eq 1 ] || [ "$FFORCE" -eq 1 ]
then
  if [ "$RBYPAS" -eq 0 ] || [ "$FFORCE" -eq 0 ]
  then
      RBYPAS=0
    FFORCE=0
  fi
fi
if [ "$WANTS_BOT_MODE"  -eq 1 ] && [ "$IS_A_BOT" -eq 1 ]
then
    BOTMODE=1
fi

if [ "$EUID"  -eq 0 ]
then
    #If the user has already bypassed us, remain silent.
    if  [ "$RBYPAS" -ne 1 ]
    then
            if [ "$BOTMODE" -eq 0 ]
            then
                if [ "$FFORCE_BYPASS" -eq 0 ]
                then
                    printf "\n\nDid you mean to run \""$0"\" as root?\n"
                fi
            fi
    else
        exit 1
    fi

fi

if [ "$BOTMODE" -eq 1 ]
then

        SUDODOINSTALL=0;
        NOWARN=1;
        NOLTO=1;
        ENABLETESTING=0;
        USEBENCHMARK=0;
        TERMINFO=0;
        IS_A_BOT=1;
        SHOWHELP=0;
        DOINSTALL=1;
        RBYPAS=1
          ZLIB=0;
          XML2=0;
          TERMINFO=0;
          MODULESON=0;
          FFI=0;
          DRYRUN=0;
        NOWARN=1;
        ENABLETESTING=0;
        USEBENCHMARK=0;
        FFORCE=1
fi


function opt_print {
   printf " ["$1"]"
}
function switch_print {
   printf " \t$1\t\t$2\n"
}
function cmd_print {
  printf "\nusage: "$SCRIPT_NAME""
  opt_print '--compilerpath'
  opt_print '--llvmdir'
  opt_print '--builddir'
  opt_print '--path'
  opt_print '--timer_file'
  opt_print '--projects'
  opt_print '--runtime'
  opt_print '--clean'
  opt_print '--dryrun'
  printf "\n\n"
  switch_print '--compiler' 'The c compilier to use for this build. (default: clang)'
  switch_print '--cxxcompiler' 'The cxx compilier to use for this build. (default: clang++)'
  switch_print '--linker' 'The linker to use for this build. (default: ld64.lld)'
  switch_print '--compilerpath' 'Path to the compilier to use for this build. (default: /usr/local)'
  switch_print '--llvmdir' "Path to the LLVM source code repository. (default: "$DEFAULT_LLVM_PATH")"
  switch_print '--builddir'  'Path to the directory to build llvm in (default: tempfile)'
  switch_print '--path' '\tPath to install LLVM in after the build. ninja install must be manually called. (default: /usr/local)'
  switch_print '--timer_file' "Path to the file to log build time report in (default: "$SCRIPT_DIR"/time.txt)"
  switch_print '--projects' 'LLVM projects to enable. (default:clang;lld;openmp;polly;lldb;bolt;clang-tools-extra;cross-project-tests)'
  switch_print '--runtime' 'LLVM runtimes to enable. (default: none)'
  switch_print '--clean' '\tIf true, delete all files in the build directory that already exist before starting. (default: true)'
  switch_print '--dryrun' 'Just print the config that would have been used and exit.'
  switch_print '--ffi' 'Enable ffi. (default: false)'
  switch_print '--zlib' 'Enable zlib. (default: false)'
  switch_print '--terminfo' 'Enable terminfo. (default: false)'
  switch_print '--xml2' 'Enable xml2. (default: false)'
  switch_print '--nolto' 'Disable LTO. Will overide fulllto flag. (default: false)'
  switch_print '--fulllto' 'Use Full LTO instead of thin. (default: false)'
  switch_print '--usemake' 'Use Unix Make files instead of ninja. (default: false)'
  switch_print '--skipgitpull' 'Skip running git pull in the LLVM source code directory. (default: false)'
  switch_print '--doinstall' 'Automatically attempt to run install command after the build. (default: false)'
  switch_print '--usedylib' 'Build and link dylib. (default: false)'
  switch_print '--xcodetoolchain' 'Create the install-xcode-toolchain target containing an xctoolchain directory which can be used to override the default system tools.. (default: false)'
  switch_print '--sudoinstall' 'Use sudo if doinstall is true. (default: false)'
  switch_print '--gcc' 'Use xcode gcc and override compiler name and path. (default: false)'
  switch_print '--modules' 'Enable modules. (default: false)'
  switch_print '--backtraces' 'Enable backtraces. (default: false)'
  switch_print '--uselocalpython' 'Enable using the most current version of python you have installed from python.org manually. (default: use the old included python inside xcode instead.)'
  switch_print '--Olevel' "Set the Optimization level for the buid. (what  \"-O\" value to pass the compilier). (default: "$DEFAULT_O_LEVEL")"
  switch_print '--arch' "Set the CPU arciture to build for. (default: "$DEFAULT_AARCH")"
  switch_print '--osx-arch' "Set the CPU arciture to build for. (default: "$DEFAULT_XARCH")"
  switch_print '--donotwarn' 'Silent the warnings. (default: false)'
  switch_print '--dotesting' 'Enables testing building and running.. (default: false)'
  switch_print '--benchmark' 'Enables benchmarking. (default: false)'
  printf "\n\n"
  printf "Notes:\n\nblank options will use the default so will options set to \"d\" or \"default\"\n\nSupported runtimes are:\nlibc;libunwind;libcxxabi;pstl;libcxx;compiler-rt;openmp;llvm-libgcc;offload\n\nSupported projects are:\nbolt;clang;clang-tools-extra;compiler-rt;cross-project-tests;libc;libclc;lld;lldb;mlir;openmp;polly;pstl;flang\n\n\n"
}
if [ "$SHOWHELP" -eq 1 ]
then
cmd_print
else

COMPILERPATH="${A_1:=/usr/local}"
LLLMDIR="${A_2:="$DEFAULT_LLVM_PATH"}"
BUILDDIR="${A_3:="$(mktemp -d)"}"
INSTALLPATH="${A_4:=/usr/local}"
TIMER_FILE="${A_5:="$SCRIPT_DIR"/time.txt}"
PROJECTS="${A_6:=clang;lld;openmp;polly;lldb;bolt;clang-tools-extra;cross-project-tests}"
RUNTIMES="$A_7"
CLEANBIRDDIR="${A_8:=true}"
COMPILER="${A_9:=clang}"
CXXCOMPILER="${A_10:=clang++}"
LINKER="${A_11:=ld64.lld}"
OPTLEVEL="${A_12:=$DEFAULT_O_LEVEL}"
ARCH="${A_13:=$DEFAULT_AARCH}"
OSX_ARCH="${A_14:=$DEFAULT_XARCH}"

if [ "$FULLLTO" -eq 1 ]
then
USELTO=2
fi

if [ "$NOLTO" -eq 1 ]
then
USELTO=0
fi


if [ "$A_1" = "d" ] || [ "$A_1" = "default" ]
then
COMPILERPATH="/usr/local"
fi
if [ "$A_2" = "d" ] || [ "$A_2" = "default" ]
then
LLLMDIR="$DEFAULT_LLVM_PATH"
fi
if [ "$A_3" = "d" ] || [ "$A_3" = "default" ]
then
BUILDDIR="$(mktemp -d)"
fi
if [ "$A_4" = "d" ] || [ "$A_4" = "default" ]
then
INSTALLPATH="/usr/local"
fi
if [ "$A_5" = "d" ] || [ "$A_5" = "default" ]
then
TIMER_FILE=""$SCRIPT_DIR"/time.txt"
fi
if [ "$A_6" = "d" ] || [ "$A_6" = "default" ]
then
PROJECTS="clang;lld;openmp;polly;lldb;bolt;clang-tools-extra;cross-project-tests"
fi
if [ "$A_7" = "d" ] || [ "$A_7" = "default" ]
then
RUNTIMES=""
fi
if [ "$A_8" = "d" ] || [ "$A_8" = "default" ]
then
CLEANBIRDDIR="true"
fi
if [ "$A_9" = "d" ] || [ "$A_9" = "default" ]
then
COMPILER="clang"
fi
if [ "$A_10" = "d" ] || [ "$A_10" = "default" ]
then
CXXCOMPILER="clang++"
fi
if [ "$A_11" = "d" ] || [ "$A_11" = "default" ]
then
LINKER="ld64.lld"
fi

if [ "$A_12" = "d" ] || [ "$A_12" = "default" ]
then
OPTLEVEL=$DEFAULT_O_LEVEL
else
    OPTLEVEL="2" # // sane failsafe if no valid values passed.

    if [ "$A_12" = "0" ] || [ "$A_12" -eq 0 ]
    then
            OPTLEVEL="0"
    fi
    if [ "$A_12" = "1" ] || [ "$A_12" -eq 1 ]
    then
            OPTLEVEL="1"
    fi
    if [ "$A_12" = "2" ] || [ "$A_12" -eq 2 ]
    then
            OPTLEVEL="2"
    fi
    if [ "$A_12" = "3" ] || [ "$A_12" -eq 3 ]
    then
            OPTLEVEL="3"
    fi
    if [ "$A_12" = "s" ]
    then
            OPTLEVEL="s"
    fi
fi
if [ "$A_13" = "d" ] || [ "$A_13" = "default" ]
then
ARCH=$DEFAULT_AARCH
fi
if [ "$A_14" = "d" ] || [ "$A_14" = "default" ]
then
OSX_ARCH=$DEFAULT_XARCH
fi



MY_COMMONFLAGS_COMC="-fno-stack-protector -fno-common -O"$OPTLEVEL""



    # Start contructing for my local mac builds.
    build_config="/Applications/CMake.app/Contents/bin/cmake"


        # Set compilier paths
if [ "$USEXCODEGCC" -eq 1 ]
then
build_config+=" -DLLVM_USE_LINKER=\"/usr/bin/ld\" -DCMAKE_LIBTOOL=\"/usr/bin/libtool\" -DCMAKE_CXX_COMPILER=\"/usr/bin/g++\" -DCMAKE_C_COMPILER=\"/usr/bin/gcc\"  -DLLVM_LOCAL_RPATH=\""$INSTALLPATH"/lib\" -DCMAKE_OBJDUMP=\"/usr/bin/objdump\" -DCMAKE_NM=\"/usr/bin/nm\" -DCMAKE_STRIP=\"/usr/bin/strip\" -DCMAKE_AR=\"/usr/bin/ar\" -DCMAKE_INSTALL_NAME_TOOL=\"/usr/bin/install_name_tool\""
else
build_config+=" -DLLVM_USE_LINKER=\""$COMPILERPATH"/bin/"$LINKER"\" -DLD64_EXECUTABLE=\""$COMPILERPATH"/bin/lld\" -DCMAKE_LIBTOOL=\""$COMPILERPATH"/bin/llvm-libtool-darwin\" -DCMAKE_CXX_COMPILER=\""$COMPILERPATH"/bin/"$CXXCOMPILER"\" -DCMAKE_C_COMPILER=\""$COMPILERPATH"/bin/"$COMPILER"\" -DLLVM_LOCAL_RPATH=\""$INSTALLPATH"/lib\" -DLLVM_INSTALL_BINUTILS_SYMLINKS=\"ON\"  -DCMAKE_OBJDUMP=\""$COMPILERPATH"/bin/llvm-objdump\" -DCMAKE_OBJCOPY=\""$COMPILERPATH"/bin/llvm-objcopy\" -DCMAKE_NM=\""$COMPILERPATH"/bin/llvm-nm\" -DCMAKE_STRIP=\""$COMPILERPATH"/bin/llvm-strip\" -DCMAKE_AR=\""$COMPILERPATH"/bin/llvm-ar\" -DCMAKE_INSTALL_NAME_TOOL=\""$COMPILERPATH"/bin/llvm-install-name-tool\""
fi
    # Build flags
build_config+=" -DCMAKE_C_FLAGS_RELEASE=\""$MY_COMMONFLAGS_COMC"\" -DCMAKE_CXX_FLAGS_RELEASE=\""$MY_COMMONFLAGS_COMC"\" -DCMAKE_ASM_FLAGS_RELEASE=\""$MY_COMMONFLAGS_COMC"\""

    # Set arch
build_config+=" -DLLVM_TARGETS_TO_BUILD=\""$ARCH"\" -DCMAKE_OSX_ARCHITECTURES=\""$OSX_ARCH"\""

    # flags for my local mac builds.
 build_config+=" -DLLVM_INSTALL_CCTOOLS_SYMLINKS=\"ON\" -DLLVM_INSTALL_UTILS=\"ON\" -DLIBCLANG_BUILD_STATIC=\"ON\" -DCMAKE_MACOSX_RPATH=\"ON\" -DCLANG_DEFAULT_RTLIB=\"compiler-rt\"  -DCMAKE_CXX_STANDARD=\"20\" -DLLVM_PARALLEL_LINK_JOBS=\"24\" -DLLVM_PARALLEL_COMPILE_JOBS=\"24\" -DDEFAULT_SYSROOT=\"/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk\" -DCLANG_SPAWN_CC1=\"ON\" -DCOMPILER_RT_BUILD_BUILTINS=\"OFF\" -DCOMPILER_RT_USE_BUILTINS_LIBRARY=\"OFF\" -DLLDB_USE_SYSTEM_DEBUGSERVER=\"ON\" -DLLDB_EMBED_PYTHON_HOME=\"OFF\" -DLLDB_ENABLE_LZMA=\"OFF\" -DLLVM_ENABLE_ZSTD=\"OFF\" -DLLDB_ENABLE_CURSES=\"OFF\" -DLLVM_ENABLE_LIBEDIT=\"OFF\" -DLLVM_ENABLE_Z3_SOLVER=\"OFF\""

    # warnings
if [ "$NOWARN" -eq 1 ]
then
build_config+=" -DLLVM_ENABLE_WARNINGS=\"OFF\""
else
build_config+=" -DLLVM_ENABLE_WARNINGS=\"ON\""
fi

#tests
if [ "$ENABLETESTING" -eq 1 ]
then
build_config+=" -DLLVM_BUILD_TESTS=\"ON\" -DLLDB_INCLUDE_TESTS=\"ON\" -DMLIR_INCLUDE_INTEGRATION_TEST=\"ON\" -DMLIR_INCLUDE_TESTS=\"ON\" -DCLANG_INCLUDE_TESTS=\"ON\" -DFLANG_INCLUDE_TESTS=\"ON\" -DLLVM_INCLUDE_TESTS=\"ON\" -DLLVM_TOOL_CROSS_PROJECT_TESTS_BUILD=\"ON\" -DLLVM_INDIVIDUAL_TEST_COVERAGE=\"ON\""
else
build_config+=" -DLLVM_BUILD_TESTS=\"OFF\" -DLLDB_INCLUDE_TESTS=\"OFF\" -DMLIR_INCLUDE_INTEGRATION_TEST=\"OFF\" -DMLIR_INCLUDE_TESTS=\"OFF\" -DCLANG_INCLUDE_TESTS=\"OFF\" -DFLANG_INCLUDE_TESTS=\"OFF\" -DLLVM_INCLUDE_TESTS=\"OFF\" -DLLVM_TOOL_CROSS_PROJECT_TESTS_BUILD=\"OFF\" -DLLVM_INDIVIDUAL_TEST_COVERAGE=\"OFF\""
fi

    # benchmark
if [ "$USEBENCHMARK" -eq 1 ]
then
build_config+=" -DLLVM_INCLUDE_BENCHMARKS=\"ON\" -DLLVM_BUILD_BENCHMARKS=\"ON\""
else
build_config+=" -DLLVM_INCLUDE_BENCHMARKS=\"OFF\" -DLLVM_BUILD_BENCHMARKS=\"OFF\""
fi


if [ "$USE_LOCALLY_INSTALLED_PYTHON" -eq 1 ]
then
    #Installs from python.org install into /Library/Frameworks/Python.framework
    DEFAULT_PYTHON_CURRENT=$(realpath "/Library/Frameworks/Python.framework/Versions/Current")
else
    #Else lets use the python that ships with Xcode as llvm defaults to
    DEFAULT_PYTHON_CURRENT=$(realpath "/Applications/Xcode.app/Contents/Developer/Library/Frameworks/Python3.framework/Versions/Current")
fi
#DPYTHON_LIBRARY uses a static name in the framework so we don't use DEFAULT_PYTHON_BIN_NAME in it but we do use it in Python3_EXECUTABLE
build_config+=" -DPython3_EXECUTABLE=\"\""$DEFAULT_PYTHON_CURRENT"\/bin/\""$DEFAULT_PYTHON_BIN_NAME"\"\" -DPYTHON_LIBRARY=\"\""$DEFAULT_PYTHON_CURRENT"\/Python\" -DPYTHON_INCLUDE_DIR=\"\""$DEFAULT_PYTHON_CURRENT"\/Headers\""


build_config+=" -DCMAKE_INSTALL_PREFIX=\""$INSTALLPATH"\" -DLLVM_ENABLE_RUNTIMES=\""$RUNTIMES"\" -DLLVM_ENABLE_PROJECTS=\""$PROJECTS"\""

if [ "$BACKTRACESON" -eq 1 ]
then
build_config+=" -DLLVM_ENABLE_BACKTRACES=\"ON\""
else
build_config+=" -DLLVM_ENABLE_BACKTRACES=\"OFF\""
fi

if [ "$MODULESON" -eq 1 ]
then
build_config+=" -DLLVM_ENABLE_MODULES=\"ON\""
else
build_config+=" -DLLVM_ENABLE_MODULES=\"OFF\""
fi

if [ "$TERMINFO" -eq 1 ]
then
build_config+=" -DLLVM_ENABLE_TERMINFO=\"ON\" -DTerminfo_LIBRARIES=\"/Applications/Xcode-beta.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/lib/libcurses.tbd\""
else
build_config+=" -DLLVM_ENABLE_TERMINFO=\"OFF\""
fi

if [ "$FFI" -eq 1 ]
then
build_config+=" -DLLVM_ENABLE_FFI=\"ON\" -DFFI_INCLUDE_DIR=\"/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/include/ffi\" -DFFI_LIBRARY_DIR=\"/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/lib/libffi.tbd\""
else
build_config+=" -DLLVM_ENABLE_FFI=\"OFF\""
fi

if [ "$XML2" -eq 1 ]
then
build_config+=" -DLLVM_ENABLE_LIBXML2=\"ON\" -DLLDB_ENABLE_LIBXML2=\"ON\" -DLIBXML2_INCLUDE_DIR=\"/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/include/libxml\" -DLIBXML2_LIBRARY=\"/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/lib/libxml2.2.tbd\""
else
build_config+=" -DLLVM_ENABLE_LIBXML2=\"OFF\" -DLLDB_ENABLE_LIBXML2=\"OFF\""
fi

if [ "$ZLIB" -eq 1 ]
then
build_config+=" -DLLVM_ENABLE_ZLIB=\"ON\" -DZLIB_INCLUDE_DIR=\"/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/include\" -DZLIB_LIBRARY_RELEASE=\"/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/lib/libz.1.tbd\""
else
build_config+=" -DLLVM_ENABLE_ZLIB=\"OFF\""
fi

if [ "$USELTO" -eq 1 ]
then
build_config+=" -DLLVM_ENABLE_LTO=\"Thin\""
elif [ "$USELTO" -eq 2 ]
then
build_config+=" -DLLVM_ENABLE_LTO=\"Full\""
else
build_config+=" -DLLVM_ENABLE_LTO=\"Off\""
fi
if [ "$USEDYLIB" -eq 1 ]
then
build_config+=" -DLLVM_BUILD_LLVM_DYLIB=\"ON\" -DLLVM_LINK_LLVM_DYLIB=\"ON\""
else
build_config+=" -DLLVM_BUILD_LLVM_DYLIB=\"OFF\" -DLLVM_LINK_LLVM_DYLIB=\"OFF\""
fi

if [ "$XCODETOOLCHAIN" -eq 1 ]
then
build_config+=" -DLLVM_CREATE_XCODE_TOOLCHAIN=\"ON\""
else
build_config+=" -DLLVM_CREATE_XCODE_TOOLCHAIN=\"OFF\""
fi


if [ "$USEMAKE" -eq 1 ]
then
build_config+=" -DCMAKE_BUILD_TYPE=\"Release\" -G \"Unix Makefiles\" -S \""$LLLMDIR"/llvm\" -B \""$BUILDDIR"\""
else
build_config+=" -DCMAKE_BUILD_TYPE=\"Release\" -GNinja -S \""$LLLMDIR"/llvm\" -B \""$BUILDDIR"\""
fi




if [ "$DRYRUN" -eq 1 ]
then
 printf "\nbuildconfig\n\n$build_config\n\n\n"

else

printf "\nbuildconfig=\n$build_config\n"

if [ "$SKIPGITPULL" -eq 0 ]
then
cd $LLLMDIR
wait
/usr/bin/time -h git pull
wait
fi

cd $BUILDDIR

wait
if [ "$CLEANBIRDDIR" = "true" ]
then
rm -rf $BUILDDIR/*
fi
wait
eval "$build_config"
wait
printf "STARTING COMPILE WITH CLANG IN DIR=$COMPILERPATH\n" >> $TIMER_FILE
if [ "$USEMAKE" -eq 1 ]
then
/usr/bin/time -a -o $TIMER_FILE make -j24 -l24
else
/usr/bin/time -a -o $TIMER_FILE ninja -j24
fi
wait
printf "DONE\n" >> $TIMER_FILE

if [ "$DOINSTALL" -eq 1 ]
then

if [ "$USEMAKE" -eq 1 ]
then
if [ "$SUDODOINSTALL" -eq 1 ]
then
sudo make install -j24 -l24
else
make install -j24 -l24
fi
else
if [ "$SUDODOINSTALL" -eq 1 ]
then
sudo ninja install -j24 -l24
else
ninja install -j24 -l24
fi

fi

fi


fi
fi
