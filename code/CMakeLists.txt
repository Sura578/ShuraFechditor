cmake_minimum_required(VERSION 3.16)

project(ShuraFechditor LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(shurafechditor
    main.cpp
    ui.cpp
    palette.cpp
    profilemaker.cpp
)
target_include_directories(shurafechditor PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(shurafechditor PRIVATE -Wall -Wextra -Wpedantic)
endif()
#Copy data file with the bin.
foreach(data_file colores.conf color.options.colors config.jsonc)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${data_file}")
        configure_file(
            "${CMAKE_CURRENT_SOURCE_DIR}/${data_file}"
            "${CMAKE_CURRENT_BINARY_DIR}/${data_file}"
            COPYONLY
        )
    endif()
endforeach()

#Copy profile images next to the binary so profilechecker can validate them.
file(GLOB profile_images CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/logos/*.png")
if(profile_images)
    file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/logos")
    foreach(image_file ${profile_images})
        get_filename_component(image_name "${image_file}" NAME)
        configure_file(
            "${image_file}"
            "${CMAKE_CURRENT_BINARY_DIR}/logos/${image_name}"
            COPYONLY
        )
    endforeach()
endif()
