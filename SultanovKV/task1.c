#include<stdio.h>
#include<math.h>

typedef long long int Money;
const double Inflation = 0.08;                // Инфляция
const double Deposite = 0.6;                  // Ставка депозита
const double Indexsation = 0.05;              // Индексация

struct Hero {
	Money salary;                             // Зарплата
	Money food;                               // Еда
	Money expense;                            // Вб, Озон, Различные покупки
	Money transport;                          // Транспорт
	Money deposit;                            // Депозит
	Money flat;                               // Квартира
	Money communal_bills;                     // Комунальные услуги
	Money monthly_mortgage_pay;               // Месячная Оплата Ипотеки
	Money loan_amount;                        // Cумма Кредита
	int annual_rate;                          // Годовая ставка
	int term;                                 // Срок аренды
};


struct Cat {
	int year_of_purchase;
	int month_of_purchase;
	Money cost;
	Money food_cost;
	Money medical_cost;
	Money funeral_cost;
	int duration_of_life;
};


Hero Bob;
Hero Alice;


void Bob_init() {

	Bob.salary =  100*1000;
	Bob.food =   10*1000;
	Bob.expense = 10*1000;
	Bob.transport = 6 * 1000;
	Bob.deposit = 1000 * 10000; 
}


void Bob_flat(const int month) {

	Bob.deposit -= Bob.flat;
	if (month == 12) {
		Bob.flat *= 1. + Inflation;
	}
}


void Bob_salary(const int month) {

	Bob.deposit += Bob.salary;
	if (month == 12)
		Bob.salary *= 1. + Indexsation;
}


void Bob_expense(const int month) {

	Bob.deposit -= Bob.expense;
	if (month == 12) {
		Bob.expense *= 1. + Inflation;
	}
}



void Bob_deposite(const int month)
{
	if (month == 12)
	{
		Bob.deposit *= 1. + Deposite;
	}
}


void Alice_init() {

	Alice.salary = 200 * 1000;
	Alice.food = 25 * 1000;
	Alice.transport = 10 * 1000;
	Alice.deposit = 800* 1000;
	Alice.monthly_mortgage_pay;
	Alice.loan_amount = 15 * 1000 * 1000;
	Alice.annual_rate = 16;
	Alice.term = 30;
	Alice.expense = Alice.food + Alice.transport;
}


void Alice_salary(const int month) {

	Alice.deposit += Alice.salary;
	if (month == 12)
		Alice.salary *= 1. + Indexsation;
}

//
void Alice_cat(const int month, const int year) {

	if ((year > 2030 || (month >= 9 && year = 2029)) && (year < 2045 || (month <= 11 && year = 2045))) {


	}




}


void Alice_expense(const int month){

	Alice.deposit -= Alice.expense;
	if (month == 12) {
		Alice.expense *= 1. + Inflation;
	}
}


void Alice_deposite(const int month){
	
	if (month == 12)
	{
		Alice.deposit *= 1. + Deposite;
	}
}


void Alice_communal_bills(const int month){

	Alice.deposit -= Alice.communal_bills;    
	if (month == 12) {
		Alice.communal_bills *= 1. + Inflation;
	}
}


void Alice_mortgage_monthly_pay() {

	double monthly_rate = Alice.annual_rate / 12.0 / 100.0;
	int total_month = Alice.term * 12;

	Alice.monthly_mortgage_pay = Alice.loan_amount *
		(monthly_rate * pow(1 + monthly_rate, total_month))
		/ (pow(1 + monthly_rate, total_month) - 1);

	Alice.deposit -= Alice.monthly_mortgage_pay;

}


void simulation(int month, int year) {

	int year_now = year;
	int month_now = month;

	while (!(year == (year_now + 30) && month == month_now))
	{
		Bob_salary(month);
		Bob_expense(month);
		Bob_flat(month);
		Bob_deposite(month);

		Alice_salary(month);
		Alice_expense(month);
		Alice_communal_bills(month);
		Alice_deposite(month);
		Alice_mortgage_monthly_pay();

		month++;

		if (month == 13) {
			month = 1;
			++year;
		}
	}
	Alice.deposit += Alice.loan_amount * (1. + Inflation) * Alice.term;
}


void print_result()
{
	printf("Bob capital = % lld RUB \n", (Money)(Bob.deposit));
	printf("Alice capital = % lli RUB\n", (Money)(Alice.deposit));
}


int main()
{
	Alice_init();
	Bob_init();
	simulation(9, 2024);
	print_result();

	return 0;
}
