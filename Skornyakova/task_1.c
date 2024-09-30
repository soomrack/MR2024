#include <stdio.h> 
#include <math.h> 
 
typedef long long int  Money; 

typedef struct {
    Money sum;
    Money first_payment;
    Money monthly_payment;
    int rate;
} Mortgage;


typedef struct { 
    Money salary ; 
    Money capital;
    Money deposit;
    Money wallet;
    int deposit_rate; 
    Money apartment_payment;  // ежемесечная плата за проживание 
    Money others_expences;  // ежемесячные дополнительные  расходы 
    Mortgage mortgage;   
} Person;

Person Alice;
Person Bob; 
Mortgage mortgage;
void Alice_init () 
{   
    Alice.wallet = 1000*1000; 
    Alice.salary = 260*1000; 
    Alice.deposit = 0;
    Alice.others_expences = 40*1000; 
    Alice.mortgage.first_payment = 1000*1000;
    Alice.deposit_rate = 0.2; // годовая ставка сберегательного счета 
    Alice.wallet -= Alice.mortgage.first_payment;
    Alice.mortgage.sum = 15 * 1000 * 1000;
    Alice.mortgage.monthly_payment = 178209; // посчитан на сайте https://www.banki.ru/services/calculators/hypothec/
} 
void Bob_init()
{
    Bob.wallet = 0; 
    Bob.salary = 260*1000; 
    Bob.others_expences = 40*1000; 
    Bob.deposit_rate = 0.2;
    Bob.deposit = 1000*1000;
    Bob.apartment_payment = 30*1000;  
}

void Alice_salary(int month)
{
  Alice.wallet += Alice.salary;
  if (month == 12)
    Alice.salary *= 1.08;
}

void Bob_salary(int month)
{
  Bob.wallet += Bob.salary;
   if (month == 12)
   Bob.salary *= 1.08;
}

void Alice_others_expences(int month)
{
  Alice.wallet -= Alice.others_expences;
  if (month == 12)
   Alice.others_expences *=1.08;
 }

void Bob_others_expences(int month)
{
  Bob.wallet -= Bob.others_expences;
  if (month == 12)
   Bob.others_expences *=1.08;
 }

void Alice_accommodation_payment()
{
 Alice.wallet -= Alice.mortgage.monthly_payment; 
} 

void Bob_accommodation_payment(int month)
{
 Bob.wallet -= Bob.apartment_payment;
 if (month == 12)
   Bob.apartment_payment *= 1.08;
} 

void flat_price(int month)
{
  if(month == 12)
    Alice.mortgage.sum *= 1.08;  
}

void Alice_deposit()
{
  Alice.deposit += Alice.wallet ;
  Alice.wallet = 0;
  if (Alice.deposit > 0)
  Alice.deposit *= 1.0167;
  }

void Bob_deposit()
{
  Bob.deposit += Bob.wallet ;
  Bob.wallet = 0;
  Bob.deposit *= 1.0167;
  }
    
void Simulation(int start_year, int start_month)
{
    int end_year = start_year + 25;
    int month = start_month;
    int year = start_year;
    while( !((year == end_year) && (month == start_month ))) {
      Alice_salary(month);
      Alice_others_expences(month);
      Alice_accommodation_payment();
      Alice_deposit();

      Bob_salary(month);
      Bob_others_expences(month);
      Bob_accommodation_payment(month);
      Bob_deposit(year);

      flat_price(month);

        month ++;
        if (month == 13){ 
            year ++;
            month = 1;}
      }
 }
void person_printf()
  {
    printf("Alice capital = %lld\n", Alice.deposit + Alice.mortgage.sum);
    printf("Bob capital = %lld\n", Bob.deposit);
  }

int main(){
  Alice_init();
  Bob_init();

  Simulation(2024,9);
  person_printf();
  
  return 0;
}  
 
 
