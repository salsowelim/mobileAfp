# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

cmake_minimum_required(VERSION 3.4.1)
add_library( # Sets the name of the library.
             fingerprint

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
             fingerprint.cpp )


find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log )

# including opencv
set(OpenCV_DIR "../../../../opencv/src/sdk/native/jni")
find_package(OpenCV REQUIRED)
message(STATUS "OpenCV libraries: ${OpenCV_LIBS}")
target_link_libraries(fingerprint ${OpenCV_LIBS})
# end of including opencv

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.
target_include_directories(fingerprint PRIVATE
                           ${distribution_DIR}/include)
target_link_libraries( # Specifies the target library.
                       fingerprint
                       #libboost_system
                       # Links the target library to the log library
                       # included in the NDK.
                       ${log-lib} )