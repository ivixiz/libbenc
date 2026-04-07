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
sed -i 's/main\.c/main.cpp/g' "$CUBEMX_CMAKE"
echo "Updated stm32cubemx CMakeLists.txt"

# -------------------------------
# Update root CMakeLists.txt
# -------------------------------
PROJECT_DIR="$(cd "$(dirname "$0")" && pwd)"
# Path to Core
CORE_INC="$PROJECT_DIR/Core/Inc"
CORE_SRC="$PROJECT_DIR/Core/Src"
# Path to CMakeLists.txt
CMAKE_FILE="$PROJECT_DIR/CMakeLists.txt"
# --- Include List Directories ---
INC_LIST=""
if [ -d "$CORE_INC" ]; then
    INC_LIST="    Core/Inc"
fi
# --- Source List Generation ---
SRC_LIST=""
if [ -d "$CORE_SRC" ]; then
    for src_file in "$CORE_SRC"/*.c; do
        [ -f "$src_file" ] || continue
        rel_path="${src_file#$PROJECT_DIR/}"
        SRC_LIST="$SRC_LIST
    $rel_path"
    done
fi
# --- Update CMakeLists.txt ---
if [ ! -f "$CMAKE_FILE" ]; then
    echo "Updating CMakeLists.txt..."
    touch "$CMAKE_FILE"
fi
TMP_FILE="$CMAKE_FILE.tmp"
awk -v inc="$INC_LIST" -v src="$SRC_LIST" '
/target_link_directories\(.*PRIVATE/ {
    print "target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE"
    print inc
    print ")"
    getline;
    while ($0 !~ /\)/) { getline }
    next
}

/target_sources\(.*PRIVATE/ {
    print "target_sources(${CMAKE_PROJECT_NAME} PRIVATE"
    print src
    print ")"
    getline;
    while ($0 !~ /\)/) { getline }
    next
}
{ print }
' "$CMAKE_FILE" > "$TMP_FILE"
mv "$TMP_FILE" "$CMAKE_FILE"
echo "CMakeLists.txt updated!"
echo "Done."
