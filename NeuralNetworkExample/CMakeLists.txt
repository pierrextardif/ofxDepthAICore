# ======================= ofxCMake Vers. 0.2 =============
#  PUT THIS FILE INTO YOUR OPENFRAMEWORKS PROJECT FOLDER

# ========================================================
# ===================== CMake Settings ===================
# ========================================================
cmake_minimum_required( VERSION 3.3 )

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project( openframeworks )

# ========================================================
# ===================== User Settings ====================
# ========================================================
# ---------------------- App name  -----------------------
# Set the name of your app here:
set( APP_NAME   depthAICoreNNExample )

# ------------------------ OF Path -----------------------
#
# This MUST be a RELATIVE path to OF!!!
set( OF_DIRECTORY_BY_USER "../../.." )

# --------------------- Source Files ---------------------

# Automatically add all sources in the src folder
file(   GLOB_RECURSE
        APP_SRC
        "src/*.cpp"
        )

set( ${APP_NAME}_SOURCE_FILES
        ${APP_SRC} )


# ------------------------ AddOns  -----------------------

# Uncomment this entry to activate addOns. See the
# project's readme.md for more information.

set( OFX_ADDONS_ACTIVE
        ofxDepthAICore
        )


# -----------------------Custom Libs  --------------------

# If you need to link against additional libraries, add
# them here:


# set( USER_LIBS
#      your/lib/path/lib.a
#      )

# Use this call to add extra include directories:

#include_directories("include/myIncludeFolder")

# =========================================================================
# ============================== OpenFrameworks ===========================
# =========================================================================
include( ${OF_DIRECTORY_BY_USER}/addons/ofxCMake/modules/main.cmake )
# =========================================================================



