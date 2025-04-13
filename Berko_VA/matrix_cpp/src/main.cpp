#include <iostream>
#include <stdexcept>
#include <limits>
#include <functional>
#include <chrono>

#include "matrix.hpp"

void run_operation(const std::string& description, const std::function<void()>& operation) {
    std::cout << "\n=== " << description << " ===\n";
    try {
        operation();
    } catch (const std::runtime_error& e) {
        std::cout << "Ошибка: " << e.what() << "\n";
    } catch (const std::out_of_range& e) {
        std::cout << "Ошибка индекса: " << e.what() << "\n";
    } catch (const std::bad_alloc&) {
        std::cout << "Ошибка: Не удалось выделить память\n";
    } catch (...) {
        std::cout << "Неизвестная ошибка\n";
    }
}


int main() {

    Matrix().set_log_level(LogLevel::ERR);

    // 1. Создание матрицы и базовые геттеры
    run_operation("Создание матрицы 2x3 и проверка геттеров", [] {
        Matrix m(2, 3);
        m(0, 0) = 1.0; m(0, 1) = 2.0; m(0, 2) = 3.0;
        m(1, 0) = 4.0; m(1, 1) = 5.0; m(1, 2) = 6.0;
        std::cout << "Матрица:\n";
        m.print();
        std::cout << "Строки: " << m.rows() << ", Столбцы: " << m.cols() << "\n";
        std::cout << "Пустая: " << (m.is_empty() ? "да" : "нет") << "\n";
        std::cout << "Квадратная: " << (m.is_square() ? "да" : "нет") << "\n";
    });

    // 2. Копирование и перемещение
    run_operation("Копирование и перемещение матрицы", [] {
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
        std::cout << (m.is_empty() ? "пустая\n" : "не пустая\n");
    });

    // 3. Оператор сложения
    run_operation("Сложение матриц", [] {
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
    run_operation("Вычитание матриц", [] {
        Matrix a(2, 2);
        a(0, 0) = 1.0; a(0, 1) = 2.0;
        a(1, 0) = 3.0; a(1, 1) = 4.0;
		a.print();
        Matrix b = a;
        Matrix diff = a - b;
        std::cout << "Разность матриц:\n";
        diff.print();
    });

    // 5. Оператор умножения
    run_operation("Умножение матриц", [] {
        Matrix a(2, 2);
        a(0, 0) = 1.0; a(0, 1) = 2.0;
        a(1, 0) = 3.0; a(1, 1) = 4.0;
		a.print();
        Matrix b = a;
        Matrix prod = a * b;
        std::cout << "Произведение матриц:\n";
        prod.print();
    });

    // 6. Умножение на скаляр
    run_operation("Умножение матрицы на скаляр", [] {
        Matrix a(2, 2);
        a(0, 0) = 1.0; a(0, 1) = 2.0;
        a(1, 0) = 3.0; a(1, 1) = 4.0;
		a.print();
        Matrix scaled = a * 2.0;
        std::cout << "Матрица, умноженная на 2:\n";
        scaled.print();
        scaled = 2.0 * a;
        std::cout << "Скаляр 2, умноженный на матрицу:\n";
        scaled.print();
    });

    // 7. Заполнение нулями
    run_operation("Заполнение матрицы нулями", [] {
        Matrix m(2, 2);
        m(0, 0) = 1.0; m(0, 1) = 2.0;
        m(1, 0) = 3.0; m(1, 1) = 4.0;
        MatrixStatus status = m.set_zeros();
        std::cout << "Статус: " << (status == MatrixStatus::OK ? "Успех" : "Ошибка") << "\n";
        std::cout << "Матрица после set_zeros:\n";
        m.print();
    });

    // 8. Создание единичной матрицы
    run_operation("Создание единичной матрицы", [] {
        Matrix m(2, 2);
        MatrixStatus status = m.set_identity();
        std::cout << "Статус: " << (status == MatrixStatus::OK ? "Успех" : "Ошибка") << "\n";
        std::cout << "Единичная матрица:\n";
        m.print();
    });

    // 9. Транспонирование
    run_operation("Транспонирование матрицы", [] {
        Matrix m(2, 2);
        m(0, 0) = 1.0; m(0, 1) = 2.0;
        m(1, 0) = 3.0; m(1, 1) = 4.0;
		m.print();
        MatrixStatus status = m.transpose();
        std::cout << "Статус: " << (status == MatrixStatus::OK ? "Успех" : "Ошибка") << "\n";
        std::cout << "Транспонированная матрица:\n";
        m.print();
    });

    // 10. Возведение в степень
    run_operation("Возведение матрицы в степень 3", [] {
        Matrix m(2, 2);
        m(0, 0) = 1.0; m(0, 1) = 1.0;
        m(1, 0) = 1.0; m(1, 1) = 0.0;
		m.print();
        MatrixStatus status = m.pow(3);
        std::cout << "Статус: " << (status == MatrixStatus::OK ? "Успех" : "Ошибка") << "\n";
        std::cout << "Матрица в степени 3:\n";
        m.print();
    });

    // 11. Вычисление экспоненты
    run_operation("Вычисление экспоненты матрицы", [] {
        Matrix m(2, 2);
        m.set_zeros();
		m.print();
        MatrixStatus status = m.exp();
        std::cout << "Статус: " << (status == MatrixStatus::OK ? "Успех" : "Ошибка") << "\n";
        std::cout << "Экспонента матрицы:\n";
        m.print();
    });

    // 12. Вычисление определителя
    run_operation("Вычисление определителя", [] {
        Matrix m(2, 2);
        m(0, 0) = 1.0; m(0, 1) = 2.0;
        m(1, 0) = 3.0; m(1, 1) = 4.0;
		m.print();
        double det;
        MatrixStatus status = m.det(det);
        std::cout << "Статус: " << (status == MatrixStatus::OK ? "Успех" : "Ошибка") << "\n";
        std::cout << "Определитель: " << det << "\n";
    });

	// 13. Тест на заполнение случайными значениями
    run_operation("Заполнение матрицы случайными значениями", [] {
        Matrix a(5, 5);
        a.fill_random(16, 173);
		a.print();
    });

    // 14. Тест на ошибку (несовместимые размеры)
    run_operation("Тест ошибки: сложение матриц разных размеров", [] {
        Matrix a(2, 2);
        Matrix b(3, 3);
        Matrix sum = a + b;
    });

    // 15. Тест на NaN
    run_operation("Тест ошибки: матрица с NaN", [] {
        Matrix m(2, 2);
        m(0, 0) = std::numeric_limits<double>::quiet_NaN();
        Matrix sum = m + m;
    });

    // 16. Тест на доступ за границы
    run_operation("Тест ошибки: доступ за границы", [] {
        Matrix m(2, 2);
        double value = m(2, 3);
    });

    // 17. Проверка времени выполнения умножения
    run_operation("Умножение матриц с и без оптимизации", [] {
        Matrix a(100, 100);
		Matrix b(100,100);
		a.fill_random(1, 100);
		b.fill_random(1, 100);
		auto t1 = std::chrono::high_resolution_clock::now();
		Matrix res_opt = a.multiply(b, MultiplyMode::BLOCK);
		auto t2 = std::chrono::high_resolution_clock::now();
		Matrix res_unopt = a.multiply(b, MultiplyMode::STANDARD);
		auto t3 = std::chrono::high_resolution_clock::now();
		std::cout << "Время с оптимизацией: " << std::chrono::duration<double, std::milli>(t2-t1).count() << std::endl;
		std::cout << "Время без оптимизации: " << std::chrono::duration<double, std::milli>(t3-t2).count() << std::endl;
		//res_opt.print();
		//res_unopt.print();

    });

    return 0;
}
