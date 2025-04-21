#include "commandgenerator.h"

CommandGenerator::CommandGenerator(const BuilderConfiguration &config)
    : m_config(config)
{
}

QString CommandGenerator::generateCMakeCommand() const
{
    QString command = "/Applications/CMake.app/Contents/bin/cmake";
    
    // Set compiler paths
    if (m_config.useXcodeGcc()) {
        command += " -DLLVM_USE_LINKER=\"/usr/bin/ld\" -DCMAKE_LIBTOOL=\"/usr/bin/libtool\" "
                   "-DCMAKE_CXX_COMPILER=\"/usr/bin/g++\" -DCMAKE_C_COMPILER=\"/usr/bin/gcc\" "
                   "-DLLVM_LOCAL_RPATH=\"" + m_config.installPath() + "/lib\" "
                   "-DCMAKE_OBJDUMP=\"/usr/bin/objdump\" -DCMAKE_NM=\"/usr/bin/nm\" "
                   "-DCMAKE_STRIP=\"/usr/bin/strip\" -DCMAKE_AR=\"/usr/bin/ar\" "
                   "-DCMAKE_INSTALL_NAME_TOOL=\"/usr/bin/install_name_tool\"";
    } else {
        command += " -DLLVM_USE_LINKER=\"" + m_config.compilerPath() + "/bin/" + m_config.linker() + "\" "
                   "-DLD64_EXECUTABLE=\"" + m_config.compilerPath() + "/bin/lld\" "
                   "-DCMAKE_LIBTOOL=\"" + m_config.compilerPath() + "/bin/llvm-libtool-darwin\" "
                   "-DCMAKE_CXX_COMPILER=\"" + m_config.compilerPath() + "/bin/" + m_config.cxxCompiler() + "\" "
                   "-DCMAKE_C_COMPILER=\"" + m_config.compilerPath() + "/bin/" + m_config.compiler() + "\" "
                   "-DLLVM_LOCAL_RPATH=\"" + m_config.installPath() + "/lib\" "
                   "-DLLVM_INSTALL_BINUTILS_SYMLINKS=\"ON\" "
                   "-DCMAKE_OBJDUMP=\"" + m_config.compilerPath() + "/bin/llvm-objdump\" "
                   "-DCMAKE_OBJCOPY=\"" + m_config.compilerPath() + "/bin/llvm-objcopy\" "
                   "-DCMAKE_NM=\"" + m_config.compilerPath() + "/bin/llvm-nm\" "
                   "-DCMAKE_STRIP=\"" + m_config.compilerPath() + "/bin/llvm-strip\" "
                   "-DCMAKE_AR=\"" + m_config.compilerPath() + "/bin/llvm-ar\" "
                   "-DCMAKE_INSTALL_NAME_TOOL=\"" + m_config.compilerPath() + "/bin/llvm-install-name-tool\"";
    }
    
    // Build flags
    QString commonFlags = "-fno-stack-protector -fno-common -O" + m_config.optLevel();
    command += " -DCMAKE_C_FLAGS_RELEASE=\"" + commonFlags + "\" "
               "-DCMAKE_CXX_FLAGS_RELEASE=\"" + commonFlags + "\" "
               "-DCMAKE_ASM_FLAGS_RELEASE=\"" + commonFlags + "\"";
    
    // Set architecture
    command += " -DLLVM_TARGETS_TO_BUILD=\"" + m_config.arch() + "\" "
               "-DCMAKE_OSX_ARCHITECTURES=\"" + m_config.osxArch() + "\"";
    
    // Common flags for macOS builds
    command += " -DLLVM_INSTALL_CCTOOLS_SYMLINKS=\"ON\" -DLLVM_INSTALL_UTILS=\"ON\" "
               "-DLIBCLANG_BUILD_STATIC=\"ON\" -DCMAKE_MACOSX_RPATH=\"ON\" "
               "-DCLANG_DEFAULT_RTLIB=\"compiler-rt\" -DCMAKE_CXX_STANDARD=\"20\" "
               "-DLLVM_PARALLEL_LINK_JOBS=\"24\" -DLLVM_PARALLEL_COMPILE_JOBS=\"24\" "
               "-DDEFAULT_SYSROOT=\"/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk\" "
               "-DCLANG_SPAWN_CC1=\"ON\" -DCOMPILER_RT_BUILD_BUILTINS=\"OFF\" "
               "-DCOMPILER_RT_USE_BUILTINS_LIBRARY=\"OFF\" -DLLDB_USE_SYSTEM_DEBUGSERVER=\"ON\" "
               "-DLLDB_EMBED_PYTHON_HOME=\"OFF\" -DLLDB_ENABLE_LZMA=\"OFF\" -DLLVM_ENABLE_ZSTD=\"OFF\" "
               "-DLLDB_ENABLE_CURSES=\"OFF\" -DLLVM_ENABLE_LIBEDIT=\"OFF\" -DLLVM_ENABLE_Z3_SOLVER=\"OFF\"";
    
    // Warnings
    if (m_config.doNotWarn()) {
        command += " -DLLVM_ENABLE_WARNINGS=\"OFF\"";
    } else {
        command += " -DLLVM_ENABLE_WARNINGS=\"ON\"";
    }
    
    // Tests
    if (m_config.doTesting()) {
        command += " -DLLVM_BUILD_TESTS=\"ON\" -DLLDB_INCLUDE_TESTS=\"ON\" "
                   "-DMLIR_INCLUDE_INTEGRATION_TEST=\"ON\" -DMLIR_INCLUDE_TESTS=\"ON\" "
                   "-DCLANG_INCLUDE_TESTS=\"ON\" -DFLANG_INCLUDE_TESTS=\"ON\" "
                   "-DLLVM_INCLUDE_TESTS=\"ON\" -DLLVM_TOOL_CROSS_PROJECT_TESTS_BUILD=\"ON\" "
                   "-DLLVM_INDIVIDUAL_TEST_COVERAGE=\"ON\"";
    } else {
        command += " -DLLVM_BUILD_TESTS=\"OFF\" -DLLDB_INCLUDE_TESTS=\"OFF\" "
                   "-DMLIR_INCLUDE_INTEGRATION_TEST=\"OFF\" -DMLIR_INCLUDE_TESTS=\"OFF\" "
                   "-DCLANG_INCLUDE_TESTS=\"OFF\" -DFLANG_INCLUDE_TESTS=\"OFF\" "
                   "-DLLVM_INCLUDE_TESTS=\"OFF\" -DLLVM_TOOL_CROSS_PROJECT_TESTS_BUILD=\"OFF\" "
                   "-DLLVM_INDIVIDUAL_TEST_COVERAGE=\"OFF\"";
    }
    
    // Benchmark
    if (m_config.benchmark()) {
        command += " -DLLVM_INCLUDE_BENCHMARKS=\"ON\" -DLLVM_BUILD_BENCHMARKS=\"ON\"";
    } else {
        command += " -DLLVM_INCLUDE_BENCHMARKS=\"OFF\" -DLLVM_BUILD_BENCHMARKS=\"OFF\"";
    }
    
    // Python configuration
    if (m_config.useLocalPython()) {
        command += " -DPython3_EXECUTABLE=\"/Library/Frameworks/Python.framework/Versions/Current/bin/python3\" "
                   "-DPYTHON_LIBRARY=\"/Library/Frameworks/Python.framework/Versions/Current/Python\" "
                   "-DPYTHON_INCLUDE_DIR=\"/Library/Frameworks/Python.framework/Versions/Current/Headers\"";
    } else {
        command += " -DPython3_EXECUTABLE=\"/Applications/Xcode.app/Contents/Developer/Library/Frameworks/Python3.framework/Versions/Current/bin/python3\" "
                   "-DPYTHON_LIBRARY=\"/Applications/Xcode.app/Contents/Developer/Library/Frameworks/Python3.framework/Versions/Current/Python\" "
                   "-DPYTHON_INCLUDE_DIR=\"/Applications/Xcode.app/Contents/Developer/Library/Frameworks/Python3.framework/Versions/Current/Headers\"";
    }
    
    // Projects and runtimes
    command += " -DCMAKE_INSTALL_PREFIX=\"" + m_config.installPath() + "\" "
               "-DLLVM_ENABLE_RUNTIMES=\"" + m_config.runtimes() + "\" "
               "-DLLVM_ENABLE_PROJECTS=\"" + m_config.projects() + "\"";
    
    // Backtraces
    if (m_config.backtraces()) {
        command += " -DLLVM_ENABLE_BACKTRACES=\"ON\"";
    } else {
        command += " -DLLVM_ENABLE_BACKTRACES=\"OFF\"";
    }
    
    // Modules
    if (m_config.modules()) {
        command += " -DLLVM_ENABLE_MODULES=\"ON\"";
    } else {
        command += " -DLLVM_ENABLE_MODULES=\"OFF\"";
    }
    
    // Terminfo
    if (m_config.terminfo()) {
        command += " -DLLVM_ENABLE_TERMINFO=\"ON\" "
                   "-DTerminfo_LIBRARIES=\"/Applications/Xcode-beta.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/lib/libcurses.tbd\"";
    } else {
        command += " -DLLVM_ENABLE_TERMINFO=\"OFF\"";
    }
    
    // FFI
    if (m_config.ffi()) {
        command += " -DLLVM_ENABLE_FFI=\"ON\" "
                   "-DFFI_INCLUDE_DIR=\"/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/include/ffi\" "
                   "-DFFI_LIBRARY_DIR=\"/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/lib/libffi.tbd\"";
    } else {
        command += " -DLLVM_ENABLE_FFI=\"OFF\"";
    }
    
    // XML2
    if (m_config.xml2()) {
        command += " -DLLVM_ENABLE_LIBXML2=\"ON\" -DLLDB_ENABLE_LIBXML2=\"ON\" "
                   "-DLIBXML2_INCLUDE_DIR=\"/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/include/libxml\" "
                   "-DLIBXML2_LIBRARY=\"/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/lib/libxml2.2.tbd\"";
    } else {
        command += " -DLLVM_ENABLE_LIBXML2=\"OFF\" -DLLDB_ENABLE_LIBXML2=\"OFF\"";
    }
    
    // ZLIB
    if (m_config.zlib()) {
        command += " -DLLVM_ENABLE_ZLIB=\"ON\" "
                   "-DZLIB_INCLUDE_DIR=\"/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/include\" "
                   "-DZLIB_LIBRARY_RELEASE=\"/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/lib/libz.1.tbd\"";
    } else {
        command += " -DLLVM_ENABLE_ZLIB=\"OFF\"";
    }
    
    // LTO
    if (m_config.noLto()) {
        command += " -DLLVM_ENABLE_LTO=\"Off\"";
    } else if (m_config.fullLto()) {
        command += " -DLLVM_ENABLE_LTO=\"Full\"";
    } else {
        command += " -DLLVM_ENABLE_LTO=\"Thin\"";
    }
    
    // DYLIB
    if (m_config.useDylib()) {
        command += " -DLLVM_BUILD_LLVM_DYLIB=\"ON\" -DLLVM_LINK_LLVM_DYLIB=\"ON\"";
    } else {
        command += " -DLLVM_BUILD_LLVM_DYLIB=\"OFF\" -DLLVM_LINK_LLVM_DYLIB=\"OFF\"";
    }
    
    // Xcode Toolchain
    if (m_config.xcodeToolchain()) {
        command += " -DLLVM_CREATE_XCODE_TOOLCHAIN=\"ON\"";
    } else {
        command += " -DLLVM_CREATE_XCODE_TOOLCHAIN=\"OFF\"";
    }
    
    // Build system
    if (m_config.useMake()) {
        command += " -DCMAKE_BUILD_TYPE=\"Release\" -G \"Unix Makefiles\" -S \"" + 
                   m_config.llvmDir() + "/llvm\" -B \"" + m_config.buildDir() + "\"";
    } else {
        command += " -DCMAKE_BUILD_TYPE=\"Release\" -GNinja -S \"" + 
                   m_config.llvmDir() + "/llvm\" -B \"" + m_config.buildDir() + "\"";
    }
    
    return command;
}

QString CommandGenerator::generateBuildExecutionCommand() const
{
    QString command;
    
    if (m_config.useMake()) {
        command = "make -j24 -l24";
    } else {
        command = "ninja -j24";
    }
    
    return command;
}

QString CommandGenerator::generateInstallCommand() const
{
    QString command;
    
    if (m_config.useMake()) {
        if (m_config.sudoInstall()) {
            command = "sudo make install -j24 -l24";
        } else {
            command = "make install -j24 -l24";
        }
    } else {
        if (m_config.sudoInstall()) {
            command = "sudo ninja install -j24 -l24";
        } else {
            command = "ninja install -j24 -l24";
        }
    }
    
    return command;
}

QString CommandGenerator::generateBuildCommand() const
{
    if (m_config.dryRun()) {
        return generateCMakeCommand();
    }
    
    QString command = "#!/bin/bash\n\n";
    
    // Git pull if needed
    if (!m_config.skipGitPull()) {
        command += "cd " + m_config.llvmDir() + "\n";
        command += "/usr/bin/time -h git pull\n\n";
    }
    
    // Change to build directory
    command += "cd " + m_config.buildDir() + "\n\n";
    
    // Clean build directory if needed
    if (m_config.cleanBuildDir()) {
        command += "rm -rf " + m_config.buildDir() + "/*\n\n";
    }
    
    // CMake configuration
    command += generateCMakeCommand() + "\n\n";
    
    // Build
    command += "printf \"STARTING COMPILE WITH CLANG IN DIR=" + m_config.compilerPath() + "\\n\" >> " + m_config.timerFile() + "\n";
    command += "/usr/bin/time -a -o " + m_config.timerFile() + " " + generateBuildExecutionCommand() + "\n";
    command += "printf \"DONE\\n\" >> " + m_config.timerFile() + "\n\n";
    
    // Install if needed
    if (m_config.doInstall()) {
        command += generateInstallCommand() + "\n";
    }
    
    return command;
}

QString CommandGenerator::generateFullScript() const
{
    return generateBuildCommand();
}
