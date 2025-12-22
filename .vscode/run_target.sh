#!/bin/bash

# 引数の受け取り
FILE_PATH="$1"          # フルパス
FILENAME_NO_EXT="$2"    # 拡張子なしファイル名 (例: simulate_comparison)
FILE_DIR="$3"           # ディレクトリパス
EXT="$4"                # 拡張子 (例: .cpp)
WORKSPACE="$5"          # プロジェクトルート

# --- 1. Pythonファイルの場合 ---
if [ "$EXT" == ".py" ]; then
    echo "[INFO] Running Python script..."
    python3 "$FILE_PATH"
    exit 0
fi

# --- 2. C++ファイルの場合 ---
if [ "$EXT" == ".cpp" ]; then
    echo "[INFO] Detected C++ file. Preparing CMake..."

    # CMake初期化チェック (build.ninja か Makefile がなければ初期化)
    if [ ! -f "$WORKSPACE/build/build.ninja" ] && [ ! -f "$WORKSPACE/build/Makefile" ]; then
        echo "[INFO] Configuring CMake..."
        cmake -B "$WORKSPACE/build" -S "$WORKSPACE"
    fi

    # ビルド実行
    echo "[INFO] Building..."
    cmake --build "$WORKSPACE/build" || exit 1

    # --- A. テストコードの場合 ---
    if [[ "$FILENAME_NO_EXT" == *"test"* ]]; then
        echo "[INFO] Running Tests (CTest)..."
        ctest --test-dir "$WORKSPACE/build" --output-on-failure
        exit 0
    fi

    # --- B. シミュレーションの場合 ---
    # 実行ファイルのパス (Windowsの.exe対応)
    EXEC_PATH="$WORKSPACE/build/$FILENAME_NO_EXT"
    if [ -f "${EXEC_PATH}.exe" ]; then 
        EXEC_PATH="${EXEC_PATH}.exe"
    fi

    if [ -f "$EXEC_PATH" ]; then
        echo "[INFO] Running Executable: $FILENAME_NO_EXT"
        "$EXEC_PATH" || exit 1

        # ★命名規則に基づくPython可視化スクリプトの自動実行★
        # ルール: [C++ファイル名]_visualize.py があれば実行
        VISUALIZE_SCRIPT="$FILE_DIR/${FILENAME_NO_EXT}_visualize.py"
        
        if [ -f "$VISUALIZE_SCRIPT" ]; then
            echo "[INFO] Found visualization script. Running..."
            python3 "$VISUALIZE_SCRIPT"
        else
            echo "[INFO] No visualization script found ($VISUALIZE_SCRIPT). Done."
        fi
    else
        echo "[ERROR] Executable not found: $EXEC_PATH"
        echo "Make sure add_executable($FILENAME_NO_EXT ...) is defined in CMakeLists.txt"
        exit 1
    fi
    exit 0
fi

echo "[WARN] Unsupported file type: $EXT"