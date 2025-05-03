#include <iostream>
#include <functional>
#include <chrono>
#include <limits>

#include "matrix.hpp"

void runOperation(const std::string& description, const std::function<void()>& operation) {
    std::cout << "\n=== " << description << " ===\n";
    try {
        operation();
    } catch (const MatrixException& e) {
        std::cout << "Ошибка: " << e.what();
        if (auto dims = e.dimensions()) {
            std::cout << " [Размеры: " << dims->first << "x" << dims->second << "]";
        }
        if (auto otherDims = e.other_dimensions()) {
            std::cout << " [Размеры другой матрицы: " << otherDims->first << "x" << otherDims->second << "]";
        }
        if (auto idx = e.index()) {
            std::cout << " [Индекс: (" << idx->first << ", " << idx->second << ")]";
        }
        std::cout << "\n";
    } catch (...) {
        std::cout << "Неизвестная ошибка\n";
    }
}

int main() {
    Matrix().setLogLevel(LogLevel::Error);

    // 1. Создание матрицы и базовые геттеры
    runOperation("Создание матрицы 2x3 и проверка геттеров", [] {
        Matrix m(2, 3);
        m(0, 0) = 1.0; m(0, 1) = 2.0; m(0, 2) = 3.0;
        m(1, 0) = 4.0; m(1, 1) = 5.0; m(1, 2) = 6.0;
        std::cout << "Матрица:\n";
        m.print();
        std::cout << "Строки: " << m.rows() << ", Столбцы: " << m.cols() << "\n";
        std::cout << "Пустая: " << (m.isEmpty() ? "да" : "нет") << "\n";
        std::cout << "Квадратная: " << (m.isSquare() ? "да" : "нет") << "\n";
    });

    // 2. Копирование и перемещение
    runOperation("Копирование и перемещение матрицы", [] {
        Matrix m(2, 2);
        m(0, 0) = 1.0; m(0, 1) = 2.0;
        m(1, 0) = 3.0; m(1, 1) = 4.0;
        Matrix copy = m;
        std::cout << "Копия матрицы:\n";
        copy.print();
        Matrix moved = std::move(m);
        std::cout << "Перемещенная матрица:\n";
        moved.print();
        std::cout << "Исходная после перемещения (должна быть пуста): ";
        std::cout << (m.isEmpty() ? "пустая\n" : "не пустая\n");
    });

    // 3. Оператор сложения
    runOperation("Сложение матриц", [] {
        Matrix a(2, 2);
        a(0, 0) = 1.0; a(0, 1) = 2.0;
        a(1, 0) = 3.0; a(1, 1) = 4.0;
        std::cout << "Исходная матрица:\n";
        a.print();
        Matrix b = a;
        Matrix sum = a + b;
        std::cout << "Сумма матриц:\n";
        sum.print();
    });

    // 4. Оператор вычитания
    runOperation("Вычитание матриц", [] {
        Matrix a(2, 2);
        a(0, 0) = 1.0; a(0, 1) = 2.0;
        a(1, 0) = 3.0; a(1, 1) = 4.0;
        std::cout << "Исходная матрица:\n";
        a.print();
        Matrix b = a;
        Matrix diff = a - b;
        std::cout << "Разность матриц:\n";
        diff.print();
    });

    // 5. Оператор умножения
    runOperation("Умножение матриц", [] {
        Matrix a(2, 2);
        a(0, 0) = 1.0; a(0, 1) = 2.0;
        a(1, 0) = 3.0; a(1, 1) = 4.0;
        std::cout << "Исходная матрица:\n";
        a.print();
        Matrix b = a;
        Matrix prod = a * b;
        std::cout << "Произведение матриц:\n";
        prod.print();
    });

    // 6. Умножение на скаляр
    runOperation("Умножение матрицы на скаляр", [] {
        Matrix a(2, 2);
        a(0, 0) = 1.0; a(0, 1) = 2.0;
        a(1, 0) = 3.0; a(1, 1) = 4.0;
        std::cout << "Исходная матрица:\n";
        a.print();
        Matrix scaled = a * 2.0;
        std::cout << "Матрица, умноженная на 2:\n";
        scaled.print();
        scaled = 2.0 * a;
        std::cout << "Скаляр 2, умноженный на матрицу:\n";
        scaled.print();
    });

    // 7. Заполнение нулями
    runOperation("Заполнение матрицы нулями", [] {
        Matrix m(2, 2);
        m(0, 0) = 1.0; m(0, 1) = 2.0;
        m(1, 0) = 3.0; m(1, 1) = 4.0;
        std::cout << "Исходная матрица:\n";
        m.print();
        m.setZeros();
        std::cout << "Матрица после setZeros:\n";
        m.print();
    });

    // 8. Создание единичной матрицы
    runOperation("Создание единичной матрицы", [] {
        Matrix m(2, 2);
        m.setIdentity();
        std::cout << "Единичная матрица:\n";
        m.print();
    });

    // 9. Транспонирование
    runOperation("Транспонирование матрицы", [] {
        Matrix m(2, 3);
        m(0, 0) = 1.0; m(0, 1) = 2.0; m(0, 2) = 3.0;
        m(1, 0) = 4.0; m(1, 1) = 5.0; m(1, 2) = 6.0;
        std::cout << "Исходная матрица:\n";
        m.print();
        m.transpose();
        std::cout << "Транспонированная матрица:\n";
        m.print();
    });

    // 10. Возведение в степень
    runOperation("Возведение матрицы в степень 3", [] {
        Matrix m(2, 2);
        m(0, 0) = 1.0; m(0, 1) = 1.0;
        m(1, 0) = 1.0; m(1, 1) = 0.0;
        std::cout << "Исходная матрица:\n";
        m.print();
        m.pow(3);
        std::cout << "Матрица в степени 3:\n";
        m.print();
    });

    // 11. Вычисление экспоненты
    runOperation("Вычисление экспоненты матрицы", [] {
        Matrix m(2, 2);
        m.setZeros();
        std::cout << "Исходная матрица:\n";
        m.print();
        m.exp();
        std::cout << "Экспонента матрицы:\n";
        m.print();
    });

    // 12. Вычисление определителя
    runOperation("Вычисление определителя", [] {
        Matrix m(2, 2);
        m(0, 0) = 1.0; m(0, 1) = 2.0;
        m(1, 0) = 3.0; m(1, 1) = 4.0;
        std::cout << "Исходная матрица:\n";
        m.print();
        double det = m.det();
        std::cout << "Определитель: " << det << "\n";
    });

    // 13. Тест на заполнение случайными значениями
    runOperation("Заполнение матрицы случайными значениями", [] {
        Matrix a(5, 5);
        a.fillRandom(16, 173);
        std::cout << "Матрица со случайными значениями:\n";
        a.print();
    });

    // 14. Тест на ошибку (несовместимые размеры)
    runOperation("Тест ошибки: сложение матриц разных размеров", [] {
        Matrix a(2, 2);
        Matrix b(3, 3);
        Matrix sum = a + b;
    });

    // 15. Тест на NaN
    runOperation("Тест ошибки: матрица с NaN", [] {
        Matrix m(2, 2);
        m(0, 0) = std::numeric_limits<double>::quiet_NaN();
        Matrix sum = m + m;
    });

    // 16. Тест на доступ за границы
    runOperation("Тест ошибки: доступ за границы", [] {
        Matrix m(2, 2);
        double value = m(2, 3);
    });

    // 17. Проверка времени выполнения умножения
    runOperation("Умножение матриц с и без оптимизации", [] {
		
		int MATRIX_SIZE = 200;
		int NUM_ITERATIONS = 10;

		double tfirst = 0;
		double tsecond = 0;
		double tthird = 0;
		double tfourth = 0;
		double tfifth = 0;
		double tsixth = 0;
		double tseventh = 0;

        Matrix a(MATRIX_SIZE, MATRIX_SIZE);
        Matrix b(MATRIX_SIZE, MATRIX_SIZE);

		for (int idx = 0; idx < NUM_ITERATIONS; ++idx) {
		    a.fillRandom(1, 5931);
		    b.fillRandom(1, 9385);
		    auto t1 = std::chrono::high_resolution_clock::now();
		    Matrix resStd = a.multiply(b, MultiplyMode::Standard);
		    auto t2 = std::chrono::high_resolution_clock::now();
		    Matrix resBlock = a.multiply(b, MultiplyMode::Block);
		    auto t3 = std::chrono::high_resolution_clock::now();
		    Matrix resTrans = a.multiply(b, MultiplyMode::Transpose);
		    auto t4 = std::chrono::high_resolution_clock::now();
		    Matrix resBlockTrans = a.multiply(b, MultiplyMode::BlockTranspose);
		    auto t5 = std::chrono::high_resolution_clock::now();
		    Matrix resOpt = a.multiply(b, MultiplyMode::ParallelOptimized);
		    auto t6 = std::chrono::high_resolution_clock::now();
		    Matrix resRec = a.multiply(b, MultiplyMode::Recursive);
		    auto t7 = std::chrono::high_resolution_clock::now();
			Matrix resAuto = a * b;
			auto t8 = std::chrono::high_resolution_clock::now();
			tfirst += std::chrono::duration<double, std::milli>(t2 - t1).count();
			tsecond += std::chrono::duration<double, std::milli>(t3 - t2).count();
			tthird += std::chrono::duration<double, std::milli>(t4 - t3).count();
			tfourth += std::chrono::duration<double, std::milli>(t5 - t4).count();
			tfifth += std::chrono::duration<double, std::milli>(t6 - t5).count();
			tsixth += std::chrono::duration<double, std::milli>(t7 - t6).count();
			tseventh += std::chrono::duration<double, std::milli>(t8 - t7).count();
			
		}
        std::cout << "Время стандартного умножения: " << tfirst/NUM_ITERATIONS << " мс\n";
        std::cout << "Время блочного умножения: " << tsecond/NUM_ITERATIONS << " мс\n";
        std::cout << "Время умножения с транспонированием: " << tthird/NUM_ITERATIONS << " мс\n";
        std::cout << "Время блочного умножения с транспонированием: " << tfourth/NUM_ITERATIONS<< " мс\n";
        std::cout << "Время оптимизированного умножения: " << tfifth/NUM_ITERATIONS << " мс\n";
        std::cout << "Время рекурсивного умножения: " << tsixth/NUM_ITERATIONS << " мс\n";
		std::cout << "Время умножения с автоматическим выбором метода: " << tseventh/NUM_ITERATIONS << " мс\n";
    });

    return 0;
}
