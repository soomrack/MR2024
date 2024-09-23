// task1.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <stdio.h>
#include<cmath>
typedef long long int Money;

struct person 
{
    Money salary;
    Money capital;
    double interest_percentage;
    double inflation;
    Money rent;
    Money flat;
    Money credit;
    Money deposit;
    const char* name;
    Money utility;

};

void init_Alice(struct person* Alice) {
    Alice->salary = 200 * 1000 * 100;
    Alice->capital = 1000 * 1000 * 100;
    Alice->rent = 0;
    Alice->interest_percentage = 0.16;
    Alice->inflation=0.09;
    Alice->utility=10*1000*100;
    Alice->flat = 13 * 1000 * 1000 * 100;
    Alice->credit = 13 * 1000 * 1000 * 100;
    Alice->deposit = 100 * 1000 * 100;
    Alice->name = "Alice";
}

void init_Bob(struct person* Bob) {
    Bob->salary = 200 * 1000 * 100;
    Bob->capital = 1000 * 1000 * 100;
    Bob->rent = 30 * 1000 * 100 ;
    Bob->inflation = 0.09;
    Bob->utility = 10 * 1000 * 100;
    Bob->flat = 0;
    Bob->name = "Bob";
}

void salary(struct person* client) {
    client->capital+=client->salary;
}

void ipoteka_calculation(struct person* client) {
    client->rent = (client->credit-client->deposit)/(12*30)*(1+client->interest_percentage);
}

void deposit_pay(struct person* client) {
    client->capital -= client->deposit;
}

void utility_pay(struct person* client) {
    client->capital -= client->utility;
}

void rent_pay(struct person* client) {
    client->capital -= client->rent;
}

void result_print(struct person client1, struct person client2) 
{
    printf("%s", client1.name);
    printf(" have capital= %lli\n", client1.capital + client1.flat);
    printf("%s", client2.name);
    printf(" have capital= %lli\n", client2.capital + client2.flat);
    if (client1.capital+client1.flat > client2.capital+client2.flat) {
        printf("be liked %s\n", client1.name);
    } else if (client1.capital + client1.flat < client2.capital + client2.flat) {
        printf("be liked %s\n", client2.name);
    }
    else {
        printf("no differens");
    }

}
void simulation() {
    struct person Alice, Bob;
    init_Bob(&Bob);
    init_Alice(&Alice);
    int year, month;
    month = 10;
    year = 2024;
    deposit_pay(&Alice);
    ipoteka_calculation(&Alice);
    while (year!=2054 || month!=10) {
        salary(&Alice);
        rent_pay(&Alice);
        utility_pay(&Alice);

        salary(&Bob);
        rent_pay(&Bob);
        utility_pay(&Bob);

        month++;
        //каждый год инфляция растет
        if (month == 12) {
            month = 1;
            year++;
            Alice.utility *= Alice.inflation + 1;
            Bob.utility *= Bob.inflation + 1;
        }
        //раз в 3 года повышается зарплата
        if ((year - 2024) % 3 == 0) {
            Alice.salary += 30 * 1000 * 100;
            Bob.salary += 30 * 1000 * 100;
        }
        //повышение квартплаты
        if ((year-2024)%4==0) {
            Bob.rent += 5 * 1000 * 100;
        }
    }
    printf("Month: %i\n", month);
    printf("Year: %i\n", year);
    result_print(Alice, Bob);
}
int main()
{
    simulation();
    return 0;
}

// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
