#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <math.h>

typedef long long int Money; //kopeek

const double inflation = 0.01;

typedef struct Person
{
    Money bank;
    Money salary;
    Money home;
    Money food_expenses;
    Money utility_expenses;
    Money other_expenses;
    Money rent;
    int duration;
    double deposit_rate;
    char name[6];
} Person;

Person alice;
Person bob;


typedef struct Credit
{
    Money downplayment;
    Money mortgage_pay;
} Credit;

Credit alice_credit;


void alice_init()
{
    alice = (Person){
        .bank = 1000 * 1000 * 100,
        .salary = 250 * 1000 * 100,
        .home = 13 * 1000 * 1000 * 100,
        .food_expenses = 15 * 1000 * 100,
        .utility_expenses = 8000 * 100,
        .other_expenses = 17000 * 100,
        .deposit_rate = 0.2,
        .rent = 0
    };
    strcpy(alice.name, "Alice");
}


void bob_init()
{
    bob = (Person){
        .bank = 1000 * 1000 * 100,
        .salary = 250 * 1000 * 100,
        .home = 0,
        .food_expenses = 15000 * 100,
        .utility_expenses = 8000 * 100,
        .other_expenses = 17000 * 100,
        .deposit_rate = 0.2,
        .rent = 30 * 1000 * 100
    };
    strcpy(bob.name, "Bob");
};


void creedit_init()
{
    alice_credit = (Credit){
        .downplayment = 1000 * 1000 * 100,
        .mortgage_pay = 16137084, // взят из calcus.ru
    };
};


void alice_deposit()
{
    alice.bank += (Money)(alice.bank * alice.deposit_rate / 12.0);
};


void alice_salary(const int year, const int month)
{
    if (month == 12) {
        alice.salary += (Money)(alice.salary * inflation);
    };

    if (year == 2027 && (month == 10 || month == 11)) {

    }
    else {
        alice.bank += alice.salary;
    }
};


void alice_home(const int year, const int month)
{
    if (year == 2024 && month == 9) {
        alice.bank -= alice_credit.downplayment;
    }
    alice.bank -= alice_credit.mortgage_pay;
    alice_credit.mortgage_pay += (Money)(alice_credit.mortgage_pay * inflation / 12.0);
};


void alice_expenses()
{
    alice.bank -=
        (alice.food_expenses + alice.utility_expenses + alice.other_expenses);
    alice.food_expenses += (Money)(alice.food_expenses * inflation / 12.0);
    alice.utility_expenses += (Money)(alice.utility_expenses * inflation);
    alice.other_expenses += (Money)(alice.other_expenses * inflation);
};


void alice_home_index(const int month)
{
    if (month == 12)
        alice.home += (Money)(alice.home * (inflation));
};


void bob_deposit()
{
    bob.bank += (Money)(bob.bank * bob.deposit_rate / 12.0);
}


void bob_salary(const int year, const int month)
{
    if (month == 12)
        bob.salary += (Money)(bob.salary * inflation);
    bob.bank += bob.salary;
}


void bob_rent()
{
    bob.bank -= bob.rent;
}


void bob_expenses()
{
    bob.bank -=
        (bob.food_expenses + bob.utility_expenses + bob.other_expenses);
    bob.food_expenses += (Money)(bob.food_expenses * inflation / 12.0);
    bob.utility_expenses += (Money)(bob.utility_expenses * inflation);
    bob.other_expenses += (Money)(bob.other_expenses * inflation);
};


void simulation(const int start_month, const int start_year)
{
    int year = start_year;
    int month = start_month;
    alice.bank = 0;

    while (!(year == (start_year + 30) && month == start_month)) {
        alice_salary(year, month);
        alice_home(year, month);
        alice_expenses();
        alice_deposit();
        alice_home_index(month);

        bob_salary(year, month);
        bob_rent();
        bob_expenses();
        bob_rent();
        bob_deposit();

        ++month;

        if (month == 13) {
            month = 1;
            ++year;
        }
    }
};


void print_person(const Person person)
{
    printf("%s:\n", person.name);
    printf("  bank = %lld rub\n", (Money)(person.bank / 100));
};


void results()
{
    if (alice.bank + alice.home > bob.bank) {
        printf("\nAlice won!\n");
    }
    else if (alice.bank + alice.home < bob.bank) {
        printf("\nBob won!\n");
    }
    else {
        printf("\nDraw!\n");
    }
}


int main()
{
    alice_init();
    bob_init();
    creedit_init();

    simulation(9, 2024);

    print_person(bob);
    print_person(alice);

    results();

    return 0;
}
