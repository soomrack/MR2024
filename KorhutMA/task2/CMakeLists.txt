cmake_minimum_required(VERSION 3.22)
project(MatrixLibrary)

# Установка стандарта C
set(CMAKE_C_STANDARD 11)

# Добавляем библиотеку из исходников в папке matrix/
add_library(matrix STATIC matrix/matrix.c)

# Указываем путь к заголовочным файлам для библиотеки
target_include_directories(matrix PUBLIC matrix)

# Создаём исполняемый файл для main.c
add_executable(matrix_app main.c)

# Линкуем библиотеку с исполняемым файлом
target_link_libraries(matrix_app PRIVATE matrix)

# Линкуем с математической библиотекой (libm)
target_link_libraries(matrix_app PRIVATE m)
