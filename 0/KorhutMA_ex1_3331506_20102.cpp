#include <iostream>

int main()
{
    setlocale(LC_ALL, "RU");

    float a = 0;
    float b = 1000000;
    float s = 220000; //зп изменил, тк иначе у Alice кончаются деньги в первый же год 
    int h = 13000000; //квартира
    float i1 = 0.09; //инфляция
    float i2 = 0.07; //индексация
    int n = 30; //срок
    float z = 0.16; //ставка
    float d = 0.2; //депозит
    int e = 15000; //еда
    int k = 8000; //коммуналка
    int c = 17000; //прочие расходы
    float g = 161370.84; //платеж по ипотеке взят из калькулятора calcus.ru
    float r = 30000; //аренда

    for (n; n > 0; n--) {
        for (int m = 12; m > 0; m--) {
            a = a + s; //ЗП
            b = b + s;
            a = a - (g + e + k + c); //расходы за 12 мес.
            b = b - (r + e + k + c);
        }
        b = b * (1 + d); //депозит
        a = a * (1 + d); //Alice тоже кладет в банк оставшиеся деньги?
        g = g * (1 + i1); //инфляция
        a = a * (1 + i1);
        b = b * (1 + i1);
        e = e * (1 + i1);
        k = k * (1 + i1);
        c = c * (1 + i1);
        r = r * (1 + i1);
        h = h * (1 + i1);
        s = s * (1 + i2);
        std::cout << "\nAlice: " << a << " dom: " << h << "\nBob: " << b << "\n " << s; //проверка
    }

    std::cout << "\nAlice: " << a + h << "\nBob: " << b;
    if (a + h > b)
        std::cout << "\nИпотечное рабство лучше съемного";
    else if (a + h == b)
        std::cout << "\nРабство-плохо";
    else
        std::cout << "\nСъемное рабство выгоднее ипотечного";

    return 0;
}