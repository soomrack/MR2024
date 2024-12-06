#include <stdio.h>


#define MONTHS_IN_YEAR 12
#define SALARY_INDEXATION 1.07
#define INFLATION 1.09


typedef long long int Money; // копейки


typedef struct {
    Money sum;
    Money first_pay;
    Money monthly_payment;
} Mortgage;

typedef struct {
    Money pledge;
    Money monthly_chek;
} Rent;

typedef struct {
    Money price;
    Money petrol;
    Money technical;
    Money insurance;
    Money tax;
} Car;

typedef struct {
    Money salary;
    Money count;

    Mortgage mortgage;
    Rent rent;
    Car car;
    Money house_price;
    double house_tax;
    Money utilities;

    Money food_cost;
    Money selth_expenses;

    double deposit_rate;
} Person;


Person alice;
Person bob;


Money apply_inflation(Money value) {
    return value * INFLATION;
}

Person add_to_deposit(Person p) {
    p.count += p.count * p.deposit_rate / MONTHS_IN_YEAR;
    return p;
}


Person alice_init() {
    Person alice;
    alice.salary = 200 * 1000 * 100;  
    alice.count = 1000 * 1000 * 100;

    alice.mortgage.sum = 14 * 1000 * 1000 * 100;
    alice.mortgage.first_pay = 1000 * 1000 * 100;
    alice.mortgage.monthly_payment = 241582 * 100; 
    alice.count -= alice.mortgage.first_pay;

    alice.house_price = alice.mortgage.sum;
    alice.house_tax = 0.001; 
    alice.utilities = 4 * 1000 * 100;

    alice.food_cost = 15 * 1000 * 100;
    alice.selth_expenses = 10 * 1000 * 100;

    alice.deposit_rate = 0.2;
    return alice;
}


Person bob_init() {
    Person bob;
    bob.salary = 200 * 1000 * 100; 
    bob.count = 1000 * 1000 * 100;

    bob.rent.pledge = 30 * 1000 * 100;
    bob.rent.monthly_chek = 37 * 1000 * 100;
    bob.count -= bob.rent.pledge;

    bob.utilities = 4 * 1000 * 100;

    bob.food_cost = 18 * 1000 * 100;
    bob.selth_expenses = 5 * 1000 * 100;

    bob.car.price = 2 * 1000 * 1000 * 100;
    bob.car.petrol = 15 * 1000 * 100;
    bob.car.technical = 10 * 1000 * 100;
    bob.car.insurance = 7 * 1000 * 100;
    bob.car.tax = 8 * 1000 * 100;

    bob.deposit_rate = 0.2;
    return bob;
}


Person update_salary(Person p, int month) {
    if (month == 1) {
        p.salary *= SALARY_INDEXATION;
    }
    p.count += p.salary;
    return p;
}


Person pay_mortgage(Person p, int month) {
    p.count -= p.mortgage.monthly_payment;
    return p;
}


Person monthly_expenses(Person p, int month) {
    if (month == 12) {
        p.food_cost = apply_inflation(p.food_cost);
        p.utilities = apply_inflation(p.utilities);
        p.selth_expenses = apply_inflation(p.selth_expenses);
    }
    p.count -= p.food_cost + p.utilities + p.selth_expenses;
    return p;
}


Person update_house(Person p, int month, int year) {
    if (month == 1) p.house_price *= 1.07;
    if ((month == 9 && year == 2030) || (month == 5 && year == 2027)) {
        p.house_price *= 1.15;
    }
    if (month == 12) {
        p.count -= p.house_price * p.house_tax;
    }
    return p;
}


Person manage_car(Person p, int month, int year) {
    if (month == 5 && year == 2026) {
        p.count -= p.car.price;
    }
    if (year > 2026 && year < 2030) {
        if (month == 12) {
            p.car.petrol = apply_inflation(p.car.petrol);
            p.count -= p.car.tax + p.car.insurance + p.car.petrol;
        }
        if (month == 5 || month == 11) {
            p.count -= p.car.technical;
        }
    }
    return p;
}


void print_person(Person p, const char *name) {
    printf("%s capital - %lld руб\n", name, p.count / 100);
    printf("%s capital - %lld млн руб\n", name, p.count / 100 / 1000 / 1000);
}


void simulation() {
    int month = 9;
    int year = 2024;

    while (!(year == 2054 && month == 9)) {
        alice = update_salary(alice, month);
        alice = pay_mortgage(alice, month);
        alice = monthly_expenses(alice, month);
        alice = update_house(alice, month, year);
        alice = add_to_deposit(alice);

        bob = update_salary(bob, month);
        bob = pay_mortgage(bob, month);
        bob = monthly_expenses(bob, month);
        bob = manage_car(bob, month, year);
        bob = add_to_deposit(bob);

        month++;
        if (month > 12) {
            month = 1;
            year++;
        }
    }
}

int main() {
    alice = alice_init();
    bob = bob_init();
    simulation();
    print_person(alice, "Alice");
    print_person(bob, "Bob");
    return 0;
}
