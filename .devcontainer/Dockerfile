FROM mcr.microsoft.com/devcontainers/cpp:0-ubuntu-22.04

# タイムゾーン設定などでの対話を防止
ARG DEBIAN_FRONTEND=noninteractive

# 追加パッケージのインストール
# - cmake, build-essential, git: 基本ツール
# - python3, python3-pip: シミュレーション可視化用
# - python3-tk: matplotlibの表示用バックエンド
RUN apt-get update && apt-get install -y \
    cmake \
    build-essential \
    git \
    python3 \
    python3-pip \
    python3-tk \
    && apt-get clean -y && rm -rf /var/lib/apt/lists/*

# Pythonライブラリのインストール (可視化用)
RUN pip3 install pandas matplotlib

# PlatformIO Coreのインストール (ESP32用)
# ※VS Code拡張機能が自動で入れる場合もあるが、CLI用に入れておく
RUN python3 -c "$(curl -fsSL https://raw.githubusercontent.com/platformio/platformio/master/scripts/get-platformio.py)"

# PATHを通す
ENV PATH="/root/.platformio/penv/bin:${PATH}"