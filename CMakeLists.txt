cmake_minimum_required(VERSION 3.16)

project(LLVMBuilderGUI VERSION 1.0 LANGUAGES CXX)

# Set global compiler and linker flags for macOS
if(APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++")
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15")
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)
endif()

# Set Qt automatic handling
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Set the path to Qt
set(QT_PATH "/opt/Qt")
list(APPEND CMAKE_PREFIX_PATH "${QT_PATH}/6.9.0/macos")

# Find Qt packages
find_package(Qt6 REQUIRED COMPONENTS Widgets)

# Set source files
set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    builderconfiguration.cpp
    builderconfiguration.h
    commandgenerator.cpp
    commandgenerator.h
    buildexecutor.cpp
    buildexecutor.h
    configurationdialog.cpp
    configurationdialog.h
    configurationdialog.ui
)

# Add executable
qt_add_executable(LLVMBuilderGUI
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
)

# Set macOS specific properties
if(APPLE)
    set_target_properties(LLVMBuilderGUI PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.llvmtools.buildersgui"
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    )
endif()

# Link libraries
target_link_libraries(LLVMBuilderGUI PRIVATE
    Qt6::Widgets
    "-lc++"
    "-lc++abi"
)

# Set the deployment directory
set(DEPLOY_DIR "${CMAKE_BINARY_DIR}/deploy")

# Add a custom target for deployment
add_custom_target(deploy
    COMMAND ${CMAKE_COMMAND} -E make_directory ${DEPLOY_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:LLVMBuilderGUI> ${DEPLOY_DIR}
    COMMAND ${QT_PATH}/6.9.0/macos/bin/macdeployqt ${DEPLOY_DIR}/$<TARGET_FILE_NAME:LLVMBuilderGUI> -always-overwrite
    DEPENDS LLVMBuilderGUI
    COMMENT "Deploying application with dependencies"
)

# Finalize the executable
qt_finalize_executable(LLVMBuilderGUI)
