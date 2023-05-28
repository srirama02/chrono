#!/bin/bash

# -------------------------------------------------------------------------------------------------------
# Shell script for building VSG based on the last official releases.
# - Requires cmake, wget, and unzip
# - Place in an arbitrary temporary directory.
# - Specify the locations for the VSG sources OR indicate that these should be downloaded.
# - Specify the install directory.
# - Optionally set the path to the doxygen and dot executables
# - Decide whether to build shared or static libraries, whether to also build debug libraries,
#   and whether to build the VSG documentation.
# - Run the script (sh ./buildVSG.sh).
# - The install directory will contain (under subdirectories of VSG_INSTALL_DIR/lib/shared) all VSG CMake
#   project configuration scripts required to configure Chrono with the Chorno::VSG module enabled.
#
# Notes:
# - This script uses the latest versions of the various codes from their respective repositories, with the
#   only exception being assimp (set at version 5.2.5). This means that pushes to any of the VSG repositories
#   may break the generation of the dependencies for Chrono::VSG.
# - This was tested with the following versions of VSG libraries:
#      VulkanSceneGraph (github.com/vsg-dev/VulkanSceneGraph.git): Commit #c87b4cca
#      vsgXchange (github.com/vsg-dev/vsgXchange.git):             Commit #883f887
#      vsgImGui (github.com/vsg-dev/vsgImGui.git):                 Commit #d9261b1
#      vsgExamples (github.com/vsg-dev/vsgExamples.git):           Commit #7ac782b
#      assimp (github.com/assimp/assimp):                          Tag v5.2.5
# - We suggest using Ninja (ninja-build.org/) and the "Ninja Multi-Config" CMake generator.
#   (otherwise, you will need to explicitly set the CMAKE_BUILD_TYPE variable)
# -------------------------------------------------------------------------------------------------------

DOWNLOAD=ON

VSG_INSTALL_DIR="$HOME/Packages/vsg"

DOXYGEN_EXE="doxygen"
DOT_EXE="dot"

BUILDSHARED=ON
BUILDDOCS=OFF
BUILDDEBUG=ON
BUILDSYSTEM="Ninja Multi-Config"

if [ ${DOWNLOAD} = OFF ]
then    
    VSG_SOURCE_DIR="$HOME/Sources/VulkanSceneGraph"
    VSGXCHANGE_SOURCE_DIR="$HOME/Sources/vsgXchange"
    VSGIMGUI_SOURCE_DIR="$HOME/Sources/vsgImGui"
    VSGEXAMPLES_SOURCE_DIR="$HOME/Sources/vsgExamples"
    ASSIMP_SOURCE_DIR="$HOME/Sources/assimp"
fi

# ------------------------------------------------------------------------

if [ ${DOWNLOAD} = ON ]
then
    echo "Download sources from GitHub"

    rm -rf download_vsg
    mkdir download_vsg

    echo "  ... VulkanSceneGraph"
    #git clone -c advice.detachedHead=false --depth 1 --branch VulkanSceneGraph-1.0.5 "https://github.com/vsg-dev/VulkanSceneGraph" "download_vsg/vsg"
    git clone "https://github.com/vsg-dev/VulkanSceneGraph" "download_vsg/vsg"
    VSG_SOURCE_DIR="download_vsg/vsg"

    echo "  ... vsgXchange"    
    #git clone -c advice.detachedHead=false --depth 1 --branch vsgXchange-1.0.2 "https://github.com/vsg-dev/vsgXchange" "download_vsg/vsgXchange"
    git clone "https://github.com/vsg-dev/vsgXchange" "download_vsg/vsgXchange"
    VSGXCHANGE_SOURCE_DIR="download_vsg/vsgXchange"

    echo "  ... vsgImGui"
    git clone "https://github.com/vsg-dev/vsgImGui" "download_vsg/vsgImGui"
    VSGIMGUI_SOURCE_DIR="download_vsg/vsgImGui"
    
    echo "  ... vsgExamples"
    #git clone -c advice.detachedHead=false --depth 1 --branch vsgExamples-1.0.3 "https://github.com/vsg-dev/vsgExamples" "download_vsg/vsgExamples"
    git clone "https://github.com/vsg-dev/vsgExamples" "download_vsg/vsgExamples"
    VSGEXAMPLES_SOURCE_DIR="download_vsg/vsgExamples"

    echo "  ... assimp"
    git clone -c advice.detachedHead=false --depth 1 --branch v5.2.5 "https://github.com/assimp/assimp" "download_vsg/assimp"
    ASSIMP_SOURCE_DIR="download_vsg/assimp"
else
    echo "Using provided source directories"
fi

echo -e "\nSources in:"
echo "  "  ${VSG_SOURCE_DIR}
echo "  "  ${VSGXCHANGE_SOURCE_DIR}
echo "  "  ${VSGIMGUI_SOURCE_DIR}
echo "  "  ${VSGEXAMPLES_SOURCE_DIR}
echo "  "  ${ASSIMP_SOURCE_DIR}

# ------------------------------------------------------------------------

rm -rf ${VSG_INSTALL_DIR}
mkdir ${VSG_INSTALL_DIR}

# --- assimp -------------------------------------------------------------

echo -e "\n------------------------ Configure assimp\n"
rm -rf build_assimp
cmake -G "${BUILDSYSTEM}" -B build_assimp -S ${ASSIMP_SOURCE_DIR} \
      -DBUILD_SHARED_LIBS:BOOL=OFF \
      -DCMAKE_DEBUG_POSTFIX=_d \
      -DCMAKE_RELWITHDEBINFO_POSTFIX=_rd \
      -DASSIMP_BUILD_TESTS:BOOL=OFF  \
      -DASSIMP_BUILD_ASSIMP_TOOLS:BOOL=OFF \
      -DASSIMP_BUILD_ZLIB:BOOL=ON \
      -DASSIMP_BUILD_DRACO:BOOL=ON

echo -e "\n------------------------ Build and install assimp\n"
cmake --build build_assimp --config Release
cmake --install build_assimp --config Release --prefix ${VSG_INSTALL_DIR}
if [ ${BUILDDEBUG} = ON ]
then
    cmake --build build_assimp --config Debug
    cmake --install build_assimp --config Debug --prefix ${VSG_INSTALL_DIR}
else
    echo "No Debug build of assimp"
fi

# --- vsg ----------------------------------------------------------------

echo -e "\n------------------------ Configure vsg\n"
rm -rf build_vsg
if [ ${BUILDDOCS} = ON ]
then
cmake  -G "${BUILDSYSTEM}" -B build_vsg -S ${VSG_SOURCE_DIR}  \
      -DBUILD_SHARED_LIBS:BOOL=${BUILDSHARED} \
      -DCMAKE_DEBUG_POSTFIX=_d \
      -DCMAKE_RELWITHDEBINFO_POSTFIX=_rd
else    
cmake  -G "${BUILDSYSTEM}" -B build_vsg -S ${VSG_SOURCE_DIR}  \
      -DBUILD_SHARED_LIBS:BOOL=${BUILDSHARED} \
      -DCMAKE_DEBUG_POSTFIX=_d \
      -DCMAKE_RELWITHDEBINFO_POSTFIX=_rd \
      -DDOXYGEN_EXECUTABLE:FILEPATH=${DOXYGEN_EXE} \
      -DDOXYGEN_DOT_EXECUTABLE:FILEPATH=${DOT_EXE}
fi      

echo -e "\n------------------------ Build and install vsg\n"
cmake --build build_vsg --config Release
cmake --install build_vsg --config Release --prefix ${VSG_INSTALL_DIR}
if [ ${BUILDDEBUG} = ON ]
then
    cmake --build build_vsg --config Debug
    cmake --install build_vsg --config Debug --prefix ${VSG_INSTALL_DIR}
else
    echo "No Debug build of vsg"
fi
if [ ${BUILDDOCS} = ON ]
then
    cmake --build build_vsg --config Release --target docs
    cp -r build_vsg/html ${VSG_INSTALL_DIR}/html
fi

# --- vsgXchange ---------------------------------------------------------

echo -e "\n------------------------ Configure vsgXchange\n"
rm -rf build_vsgXchange
cmake  -G "${BUILDSYSTEM}" -B build_vxgXchange -S ${VSGXCHANGE_SOURCE_DIR}  \
      -DBUILD_SHARED_LIBS:BOOL=${BUILDSHARED} \
      -DCMAKE_DEBUG_POSTFIX=_d \
      -DCMAKE_RELWITHDEBINFO_POSTFIX=_rd \
      -Dvsg_DIR:PATH=${VSG_INSTALL_DIR}/lib/cmake/vsg

echo -e "\n------------------------ Build and install vsgXchange\n"
cmake --build build_vxgXchange --config Release
cmake --install build_vxgXchange --config Release --prefix ${VSG_INSTALL_DIR}
if [ ${BUILDDEBUG} = ON ]
then
    cmake --build build_vxgXchange --config Debug
    cmake --install build_vxgXchange --config Debug --prefix ${VSG_INSTALL_DIR}
else
    echo "No Debug build of vsgXchange"
fi

# --- vsgImGui -----------------------------------------------------------

echo -e "\n------------------------ Configure vsgImGui\n"
rm -rf  build_vsgImGui
cmake -G "${BUILDSYSTEM}" -B build_vsgImGui -S ${VSGIMGUI_SOURCE_DIR} \
      -DBUILD_SHARED_LIBS:BOOL=${BUILDSHARED} \
      -DCMAKE_DEBUG_POSTFIX=_d \
      -DCMAKE_RELWITHDEBINFO_POSTFIX=_rd \
      -Dvsg_DIR:PATH=${VSG_INSTALL_DIR}/lib/cmake/vsg

echo -e "\n------------------------ Build and install vsgImGui\n"
cmake --build build_vsgImGui --config Release
cmake --install build_vsgImGui --config Release --prefix ${VSG_INSTALL_DIR}
if [ ${BUILDDEBUG} = ON ]
then
    cmake --build build_vsgImGui --config Debug
    cmake --install build_vsgImGui --config Debug --prefix ${VSG_INSTALL_DIR}
else
    echo "No Debug build of vsgImGui"
fi

# --- vsgExamples --------------------------------------------------------

echo -e "\n------------------------ Configure vsgExamples\n"
rm -rf  build_vsgExamples
cmake -G "${BUILDSYSTEM}" -B build_vsgExamples -S ${VSGEXAMPLES_SOURCE_DIR} \
      -Dvsg_DIR:PATH=${VSG_INSTALL_DIR}/lib/cmake/vsg

echo -e "\n------------------------ Build and install vsgExamples\n"
cmake --build build_vsgExamples --config Release
cmake --install build_vsgExamples --config Release --prefix ${VSG_INSTALL_DIR}

# --- VSG_FILE_PATH ------------------------------------------------------

if [ "${VSG_FILE_PATH}_" = "_" ]
then
    if [ $(basename $SHELL) = "zsh" ]
    then
        # zsh
        echo " Add VSG_FILE_PATH to .zprofile"
        export_line="export VSG_FILE_PATH=\"$VSG_INSTALL_DIR/share/vsgExamples\""
        echo ${export_line}
        echo ${export_line} >> ~/.zprofile
        if [ $(uname) = Darwin -a "${DYLIB_LIBRARY_PATH}_" = "_" ]
        then
            echo " Add DYLIB_LIBRARY_PATH to .zprofile"
            export_line="export DYLD_LIBRARY_PATH=\"$VSG_INSTALL_DIR/lib\""
            echo ${export_line} >> ~/.zprofile
        fi
    else
        # Default: bash
        echo "Add VSG_FILE_PATH to .bashrc"
        export_line="export VSG_FILE_PATH=\"$VSG_INSTALL_DIR/share/vsgExamples\""
        echo ${export_line}
        echo ${export_line} >> ~/.bashrc
    fi
else
    echo "VSG_FILE_PATH already exists and is set to $VSG_FILE_PATH"
    echo "Make sure it is set to $VSG_INSTALL_DIR/share/vsgExamples"
fi
