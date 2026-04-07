#!/usr/bin/env bash

PROJECT_DIR="$(cd "$(dirname "$0")" && pwd)"
MAIN_C="$PROJECT_DIR/Core/Src/main.c"
MAIN_CPP="$PROJECT_DIR/Core/Src/main.cpp"
CUBEMX_CMAKE="$PROJECT_DIR/cmake/stm32cubemx/CMakeLists.txt"

if [ ! -f "$MAIN_C" ]; then
    echo "No main.c generated"
    exit 0
fi

echo "Updating STM32 project to C++..."
TMP=$(mktemp)

if [ -f "$MAIN_CPP" ]; then
    echo "Merging USER CODE blocks..."
    awk '
    ################################################
    # PASS 1: read USER CODE blocks from main.cpp
    ################################################
    FNR==NR {
        if(match($0,/\/\* USER CODE BEGIN (.*) \*\//,m)) {
            block=m[1]
            data=""
            next
        }
        if(match($0,/\/\* USER CODE END (.*) \*\//,m)) {
            blocks[block]=data
            block=""
            next
        }
        if(block!="")
            data=data $0 "\n"
        next
    }
    ################################################
    # PASS 2: rebuild main.c
    ################################################
    {
        if(match($0,/\/\* USER CODE BEGIN (.*) \*\//,m)) {
            name=m[1]
            print
            if(name in blocks)
                printf "%s", blocks[name]
            skip=1
            next
        }
        if(match($0,/\/\* USER CODE END (.*) \*\//)) {
            skip=0
            print
            next
        }
        if(!skip)
            print
    }
    ' "$MAIN_CPP" "$MAIN_C" > "$TMP"
else
    echo "No previous main.cpp, converting main.c -> main.cpp"
    cp "$MAIN_C" "$TMP"
fi

mv "$TMP" "$MAIN_CPP"
rm -f "$MAIN_C"
echo "Renamed main.c -> main.cpp"

if [ -f "$CUBEMX_CMAKE" ]; then
    sed -i 's|\${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Src/main\.c|\${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Src/main.cpp|' "$CUBEMX_CMAKE"
    echo "Updated stm32cubemx CMakeLists.txt"
fi

echo "Done."
