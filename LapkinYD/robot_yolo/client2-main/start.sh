#!/bin/bash

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="$SCRIPT_DIR/build"

# PID list
PIDS=()

cleanup() {
    echo -e "\n\nПолучен sigint. Завершение процессов"
    
    for pid in "${PIDS[@]}"; do
        if kill -0 "$pid" 2>/dev/null; then
            echo "Завершение процесса PID: $pid"
            kill -TERM "$pid" 2>/dev/null
        fi
    done
    
    sleep 1
    for pid in "${PIDS[@]}"; do
        if kill -0 "$pid" 2>/dev/null; then
            echo "⚫ Принудительное завершение PID: $pid"
            kill -9 "$pid" 2>/dev/null
        fi
    done
    
    echo "Все процессы завершены."
    exit 0
}

trap cleanup SIGINT SIGTERM EXIT

if [ ! -d "$BUILD_DIR" ]; then
    echo "Ошибка: Директория build не найдена: $BUILD_DIR"
    exit 1
fi

cd "$BUILD_DIR" || exit 1

APPS=("heartbeat.exe" "text_server.exe")

YOLO_MODEL="$SCRIPT_DIR/yolov8n.onnx"
if [ -f "./client.exe" ]; then
    "./client.exe" "$YOLO_MODEL" &
    PID=$!
    PIDS+=($PID)
    echo "Запущен: client.exe с YOLO моделью (PID: $PID)"
else
    echo "Файл не найден: client.exe"
fi

for app in "${APPS[@]}"; do
    if [ -f "$app" ]; then
        "./$app" &
        PID=$!
        PIDS+=($PID)
        echo "Запущен: $app (PID: $PID)"
    else
        echo "Файл не найден: $app"
    fi
done

wait
echo "Все процессы завершили работу."