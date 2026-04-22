#!/bin/bash

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
BUILD_DIR="${SCRIPT_DIR}/build"
LOGS_DIR="${SCRIPT_DIR}/logs"

SERVICES=(heartbeat robot text_server video_sender)
PIDS=()
RUNNING=true

cleanup() {
    if [ "$RUNNING" = false ]; then
        return
    fi
    RUNNING=false
    
    echo ""
    echo "Остановка всех сервисов..."
    
    for i in "${!SERVICES[@]}"; do
        app="${SERVICES[$i]}"
        pid="${PIDS[$i]}"
        
        if kill -0 "$pid" 2>/dev/null; then
            kill -TERM "$pid" 2>/dev/null
            echo " SIGTERM sent to ${app} (PID: $pid)"
        fi
    done
    
    echo "Ожидание завершения (3 сек)"
    sleep 3
    
    for i in "${!SERVICES[@]}"; do
        app="${SERVICES[$i]}"
        pid="${PIDS[$i]}"
        
        if kill -0 "$pid" 2>/dev/null; then
            kill -9 "$pid" 2>/dev/null
            echo "SIGKILL sent to ${app} (PID: $pid)"
        fi
    done
    
    rm -f "${LOGS_DIR}"/*.pid 2>/dev/null
    
    wait 2>/dev/null
    
    echo "Все сервисы остановлены"
}

trap cleanup SIGINT SIGTERM SIGHUP EXIT

mkdir -p "${LOGS_DIR}"

echo "Проверка сервисов в ${BUILD_DIR}..."
for app in "${SERVICES[@]}"; do
    if [ ! -x "${BUILD_DIR}/${app}" ]; then
        echo "Ошибка: ${BUILD_DIR}/${app} не найден или не исполняемый"
        exit 1
    fi
    echo "${app}"
done

for app in "${SERVICES[@]}"; do
    "${BUILD_DIR}/${app}" >> "${LOGS_DIR}/${app}.log" 2>&1 &
    pid=$!
    PIDS+=($pid)
    echo "$pid" > "${LOGS_DIR}/${app}.pid"
    echo "Запущен: ${app} (PID: $pid)"
done

echo ""
echo "Все сервисы работают"
echo "Логи: ${LOGS_DIR}/*.log"
echo "PID файлы: ${LOGS_DIR}/*.pid"
echo ""
echo "Нажмите Ctrl+C для остановки"
echo ""

while [ "$RUNNING" = true ]; do
    for i in "${!SERVICES[@]}"; do
        app="${SERVICES[$i]}"
        pid="${PIDS[$i]}"
        
        if ! kill -0 "$pid" 2>/dev/null; then
            echo "Сервис ${app} (PID: $pid) завершился!"
        fi
    done
    
    sleep 1
done

wait