cmake_minimum_required(VERSION 3.0)
project(alure)

IF(COMMAND CMAKE_POLICY)
    CMAKE_POLICY(SET CMP0003 NEW)
    CMAKE_POLICY(SET CMP0005 NEW)
    IF(POLICY CMP0042)
        CMAKE_POLICY(SET CMP0042 NEW)
    ENDIF(POLICY CMP0042)
ENDIF(COMMAND CMAKE_POLICY)

set(CMAKE_MODULE_PATH "${alure_SOURCE_DIR}/cmake")

include(CheckCXXCompilerFlag)
include(CheckCXXSourceCompiles)
include(GNUInstallDirs)

find_package(OpenAL REQUIRED)

set(CXX_FLAGS )

option(ALURE_DISABLE_RTTI "Disable run-time type information" OFF)
if(MSVC)
    if(ALURE_DISABLE_RTTI)
        check_cxx_compiler_flag(/GR- HAVE_GRX_SWITCH)
        if(HAVE_GRX_SWITCH)
            set(CXX_FLAGS ${CXX_FLAGS} /GR-)
        endif()
    endif()
else()
    check_cxx_compiler_flag(-std=c++11 HAVE_STD_CXX11)
    if(HAVE_STD_CXX11)
        set(CXX_FLAGS -std=c++11 ${CXX_FLAGS})
    endif()

    check_cxx_compiler_flag(-Wall HAVE_WALL_SWITCH)
    if(HAVE_WALL_SWITCH)
        set(CXX_FLAGS ${CXX_FLAGS} -Wall)
    endif()
    check_cxx_compiler_flag(-Wextra HAVE_WEXTRA_SWITCH)
    if(HAVE_WEXTRA_SWITCH)
        set(CXX_FLAGS ${CXX_FLAGS} -Wextra)
    endif()

    if(ALURE_DISABLE_RTTI)
        check_cxx_compiler_flag(-fno-rtti HAVE_NO_RTTI_SWITCH)
        if(HAVE_NO_RTTI_SWITCH)
            set(CXX_FLAGS ${CXX_FLAGS} -fno-rtti)
        endif()
    endif()
endif()

unset(EXPORT_DECL)
set(VISIBILITY_FLAGS )
if(WIN32)
    set(EXPORT_DECL "__declspec(dllexport)")
else()
    set(OLD_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS}")
    # Yes GCC, really don't accept visibility modes you don't support
    set(CMAKE_REQUIRED_FLAGS "${OLD_REQUIRED_FLAGS} -Wattributes -Werror")

    check_cxx_source_compiles("int foo() __attribute__((visibility(\"default\")));
                                int main() {return 0;}" HAVE_GCC_DEFAULT_VISIBILITY)
    if(HAVE_GCC_DEFAULT_VISIBILITY)
        set(EXPORT_DECL "__attribute__((visibility(\"default\")))")
    endif()

    if(HAVE_GCC_DEFAULT_VISIBILITY)
        check_cxx_compiler_flag(-fvisibility=hidden HAVE_VISIBILITY_HIDDEN_SWITCH)
        if(HAVE_VISIBILITY_HIDDEN_SWITCH)
            set(VISIBILITY_FLAGS ${VISIBILITY_FLAGS} -fvisibility=hidden)
        endif()
    endif()

    set(CMAKE_REQUIRED_FLAGS "${OLD_REQUIRED_FLAGS}")
endif()


set(LINKER_OPTS )
if(MSVC)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SAFESEH:NO")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /SAFESEH:NO")
    set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} /SAFESEH:NO")
else()
    option(ALURE_STATIC_GCCRT "Static-link libgcc and libstdc++ runtimes" OFF)
    if(ALURE_STATIC_GCCRT)
        set(OLD_REQUIRED_LIBRARIES ${CMAKE_REQUIRED_LIBRARIES})
        set(CMAKE_REQUIRED_LIBRARIES ${CMAKE_REQUIRED_LIBRARIES} -static-libgcc -static-libstdc++)
        check_cxx_source_compiles(
"#include <vector>
int main()
{
    std::vector<int> test;
    return test.size();
}"
            HAVE_STATIC_GCCRT_SWITCHES
        )
        if(HAVE_STATIC_GCCRT_SWITCHES)
            set(LINKER_OPTS ${LINKER_OPTS} -static-libgcc -static-libstdc++)
        endif()
        set(CMAKE_REQUIRED_LIBRARIES ${OLD_REQUIRED_LIBRARIES})
        unset(OLD_REQUIRED_LIBRARIES)
    endif()
endif()


set(alure_srcs src/devicemanager.cpp
               src/device.cpp
               src/context.cpp
               src/buffer.cpp
               src/source.cpp
               src/sourcegroup.cpp
               src/auxeffectslot.cpp
               src/effect.cpp
)
set(alure_libs ${OPENAL_LIBRARY})
set(decoder_incls )

unset(HAVE_WAVE)
unset(HAVE_VORBISFILE)
unset(HAVE_LIBFLAC)
unset(HAVE_OPUSFILE)
unset(HAVE_LIBSNDFILE)
unset(HAVE_MPG123)

option(ALURE_ENABLE_WAVE    "Enables the built-in wave file decoder"     ON)
option(ALURE_ENABLE_VORBIS  "Enables the built-in libvorbisfile decoder" ON)
option(ALURE_ENABLE_FLAC    "Enables the built-in FLAC decoder"          ON)
option(ALURE_ENABLE_OPUS    "Enables the built-in libopusfile decoder"   ON)
option(ALURE_ENABLE_SNDFILE "Enables the built-in libsndfile decoder"    ON)
option(ALURE_ENABLE_MPG123  "Enables the built-in libmpg123 decoder"     ON)

if(ALURE_ENABLE_WAVE)
    set(alure_srcs ${alure_srcs} src/decoders/wave.cpp)
    set(HAVE_WAVE 1)
endif()

find_package(Ogg)
if(OGG_FOUND)
    set(decoder_incls ${decoder_incls} ${OGG_INCLUDE_DIRS})
    find_package(Vorbis)
    if(VORBIS_FOUND AND ALURE_ENABLE_VORBIS)
        set(decoder_incls ${decoder_incls} ${VORBIS_INCLUDE_DIRS})
        set(alure_libs ${alure_libs} ${VORBIS_LIBRARIES})
        set(alure_srcs ${alure_srcs} src/decoders/vorbisfile.cpp)
        set(HAVE_VORBISFILE 1)
    endif()

    find_package(FLAC)
    if(FLAC_FOUND AND ALURE_ENABLE_FLAC)
        set(decoder_incls ${decoder_incls} ${FLAC_INCLUDE_DIRS})
        set(alure_libs ${alure_libs} ${FLAC_LIBRARIES})
        set(alure_srcs ${alure_srcs} src/decoders/flac.cpp)
        set(HAVE_LIBFLAC 1)
    endif()

    find_package(Opus)
    if(OPUS_FOUND AND ALURE_ENABLE_OPUS)
        set(decoder_incls ${decoder_incls} ${OPUS_INCLUDE_DIRS})
        set(alure_libs ${alure_libs} ${OPUS_LIBRARIES})
        set(alure_srcs ${alure_srcs} src/decoders/opusfile.cpp)
        set(HAVE_OPUSFILE 1)
    endif()
    set(alure_libs ${alure_libs} ${OGG_LIBRARIES})
endif()

find_package(SndFile)
if(SNDFILE_FOUND AND ALURE_ENABLE_SNDFILE)
    set(decoder_incls ${decoder_incls} ${SNDFILE_INCLUDE_DIRS})
    set(alure_libs ${alure_libs} ${SNDFILE_LIBRARIES})
    set(alure_srcs ${alure_srcs} src/decoders/sndfile.cpp)
    set(HAVE_LIBSNDFILE 1)
endif()

find_package(MPG123)
if(MPG123_FOUND AND ALURE_ENABLE_MPG123)
    set(decoder_incls ${decoder_incls} ${MPG123_INCLUDE_DIRS})
    set(alure_libs ${alure_libs} ${MPG123_LIBRARIES})
    set(alure_srcs ${alure_srcs} src/decoders/mpg123.cpp)
    set(HAVE_MPG123 1)
endif()


CONFIGURE_FILE(
    "${alure_SOURCE_DIR}/config.h.in"
    "${alure_BINARY_DIR}/config.h")


add_library(alure2 SHARED ${alure_srcs})
if(EXPORT_DECL)
    target_compile_definitions(alure2 PRIVATE ALURE_API=${EXPORT_DECL} ALURE_TEMPLATE=template
                                              NOMINMAX)
endif()
target_include_directories(alure2
    PUBLIC $<BUILD_INTERFACE:${alure_SOURCE_DIR}/include/AL> ${OPENAL_INCLUDE_DIR}
    PRIVATE ${alure_SOURCE_DIR}/include ${alure_SOURCE_DIR}/src ${alure_BINARY_DIR}
            ${decoder_incls}
)
target_compile_options(alure2 PRIVATE ${CXX_FLAGS} ${VISIBILITY_FLAGS})
target_link_libraries(alure2 PUBLIC ${alure_libs} PRIVATE ${LINKER_OPTS})

add_library(alure2_s STATIC ${alure_srcs})
target_compile_definitions(alure2_s PUBLIC ALURE_STATIC_LIB PRIVATE NOMINMAX)
target_include_directories(alure2_s
    PUBLIC $<BUILD_INTERFACE:${alure_SOURCE_DIR}/include/AL> ${OPENAL_INCLUDE_DIR}
    PRIVATE ${alure_SOURCE_DIR}/include ${alure_SOURCE_DIR}/src ${alure_BINARY_DIR}
            ${decoder_incls}
)
target_compile_options(alure2_s PRIVATE ${CXX_FLAGS} ${VISIBILITY_FLAGS})
target_link_libraries(alure2_s PUBLIC ${alure_libs})

install(TARGETS alure2 alure2_s EXPORT alure2
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} ${CMAKE_INSTALL_INCLUDEDIR}/AL
)
export(
    TARGETS alure2 alure2_s
    NAMESPACE Alure2::
    FILE Alure2Config.cmake
)
install(
    EXPORT alure2
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Alure2
    NAMESPACE Alure2::
    FILE Alure2Config.cmake
)
install(FILES
    include/AL/alure2.h
    include/AL/alure2-aliases.h
    include/AL/alure2-typeviews.h
    include/AL/alure2-alext.h
    include/AL/efx.h
    include/AL/efx-presets.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/AL
)


option(ALURE_BUILD_EXAMPLES "Build example programs" ON)
if(ALURE_BUILD_EXAMPLES)
    add_executable(alure-enum examples/alure-enum.cpp)
    target_compile_options(alure-enum PRIVATE ${CXX_FLAGS})
    target_link_libraries(alure-enum PRIVATE alure2 ${LINKER_OPTS})

    add_executable(alure-play examples/alure-play.cpp)
    target_compile_options(alure-play PRIVATE ${CXX_FLAGS})
    target_link_libraries(alure-play PRIVATE alure2 ${LINKER_OPTS})

    add_executable(alure-stream examples/alure-stream.cpp)
    target_compile_options(alure-stream PRIVATE ${CXX_FLAGS})
    target_link_libraries(alure-stream PRIVATE alure2 ${LINKER_OPTS})

    add_executable(alure-reverb examples/alure-reverb.cpp)
    target_compile_options(alure-reverb PRIVATE ${CXX_FLAGS})
    target_link_libraries(alure-reverb PRIVATE alure2 ${LINKER_OPTS})

    add_executable(alure-hrtf examples/alure-hrtf.cpp)
    target_compile_options(alure-hrtf PRIVATE ${CXX_FLAGS})
    target_link_libraries(alure-hrtf PRIVATE alure2 ${LINKER_OPTS})

    find_package(PhysFS)
    if(PHYSFS_FOUND)
        add_executable(alure-physfs examples/alure-physfs.cpp)
        target_include_directories(alure-physfs PRIVATE ${PHYSFS_INCLUDE_DIR})
        target_compile_options(alure-physfs PRIVATE ${CXX_FLAGS})
        target_link_libraries(alure-physfs PRIVATE alure2 ${PHYSFS_LIBRARY} ${LINKER_OPTS})
    endif()

    find_package(DUMB)
    if(DUMB_FOUND)
        add_executable(alure-dumb examples/alure-dumb.cpp)
        target_include_directories(alure-dumb PRIVATE ${DUMB_INCLUDE_DIRS})
        target_compile_options(alure-dumb PRIVATE ${CXX_FLAGS})
        target_link_libraries(alure-dumb PRIVATE alure2 ${DUMB_LIBRARIES} ${LINKER_OPTS})
    endif()
endif()
