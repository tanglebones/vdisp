cmake_minimum_required(VERSION 4.0)
project(vdisp C)

set(CMAKE_C_STANDARD 11)

add_executable(vdisp main.c
        vdisp_oop.h
        vdisp_oop.c
        vdisp_trait.c
        vdisp_trait.h
        ptime.c
        ptime.h)

# Configure aggressive optimizations for Release builds
# Do not force the build type here; CLion profiles control it. We just define flags when Release is chosen.
if (MSVC)
    # MSVC: High optimization, inlining, fast FP, and link-time code generation
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /O2 /Ob3 /Ot /Oi /fp:fast /DNDEBUG /GL")
else()
    # GCC/Clang: O3, native CPU tuning, fast-math, strict aliasing, and LTO
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3 -DNDEBUG -march=native -mtune=native -ffast-math -fstrict-aliasing")
endif()

# Enable interprocedural optimization (LTO) where supported in Release
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)

# Target to disassemble the built executable (intended for Release profile)
# Generates vdisp.disasm.txt in the configuration's build directory
if (MSVC)
    set(_DISASM_CMD dumpbin)
    # Avoid quoting the /OUT: path because dumpbin delegates to link.exe, which treats embedded quotes literally.
    # Paths here have no spaces in our profiles; if they ever do, consider a temporary directory or use redirection instead.
    set(_DISASM_ARGS /DISASM /OUT:${CMAKE_CURRENT_BINARY_DIR}/vdisp.disasm.txt "$<TARGET_FILE:vdisp>")
else()
    # Prefer llvm-objdump if available; otherwise fallback to objdump
    find_program(LLVM_OBJDUMP llvm-objdump)
    if (LLVM_OBJDUMP)
        set(_DISASM_CMD ${LLVM_OBJDUMP})
        # -d: disassemble, -S: mixed source, -C: demangle where applicable
        set(_DISASM_ARGS -d -S -C "$<TARGET_FILE:vdisp>" "-o=${CMAKE_CURRENT_BINARY_DIR}/vdisp.disasm.txt")
        # llvm-objdump does not support output redirection flag consistently; use CMake -E to redirect
        unset(_DISASM_ARGS)
        set(_DISASM_ARGS -d -S -C "$<TARGET_FILE:vdisp>")
        set(_DISASM_REDIRECT "${CMAKE_CURRENT_BINARY_DIR}/vdisp.disasm.txt")
    else()
        find_program(OBJDUMP objdump)
        if (NOT OBJDUMP)
            message(FATAL_ERROR "No objdump tool found (llvm-objdump or objdump). Cannot create disassembly target.")
        endif()
        set(_DISASM_CMD ${OBJDUMP})
        set(_DISASM_ARGS -d -S -C "$<TARGET_FILE:vdisp>")
        set(_DISASM_REDIRECT "${CMAKE_CURRENT_BINARY_DIR}/vdisp.disasm.txt")
    endif()
endif()

# Define the custom target
if (MSVC)
    add_custom_target(disassemble
        COMMAND ${_DISASM_CMD} ${_DISASM_ARGS}
        DEPENDS vdisp
        BYPRODUCTS "${CMAKE_CURRENT_BINARY_DIR}/vdisp.disasm.txt"
        COMMENT "Disassembling vdisp -> ${CMAKE_CURRENT_BINARY_DIR}/vdisp.disasm.txt"
        VERBATIM
    )
else()
    # For non-MSVC, use PowerShell or sh redirection via CMake -E to ensure portability on Windows
    if (WIN32)
        # PowerShell requires a single string; convert CMake list to space-separated
        string(REPLACE ";" " " _DISASM_ARGS_STR "${_DISASM_ARGS}")
        add_custom_target(disassemble
            COMMAND powershell -NoProfile -Command "& \"${_DISASM_CMD}\" ${_DISASM_ARGS_STR} | Out-File -Encoding utf8 \"${_DISASM_REDIRECT}\""
            DEPENDS vdisp
            BYPRODUCTS "${CMAKE_CURRENT_BINARY_DIR}/vdisp.disasm.txt"
            COMMENT "Disassembling vdisp -> ${CMAKE_CURRENT_BINARY_DIR}/vdisp.disasm.txt"
            VERBATIM
        )
    else()
        add_custom_target(disassemble
            COMMAND ${_DISASM_CMD} ${_DISASM_ARGS} > "${_DISASM_REDIRECT}"
            DEPENDS vdisp
            BYPRODUCTS "${CMAKE_CURRENT_BINARY_DIR}/vdisp.disasm.txt"
            COMMENT "Disassembling vdisp -> ${CMAKE_CURRENT_BINARY_DIR}/vdisp.disasm.txt"
            VERBATIM
        )
    endif()
endif()
