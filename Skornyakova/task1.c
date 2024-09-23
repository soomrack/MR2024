#include <stdio.h> 
#include <math.h> 
 
typedef long  Money; 
Money flat_cost = 1500*1000;
int a;
 
typedef struct  { 
    Money salary ; 
    Money capital; 
    Money apartment_payment;  // ежемесечная плата за проживание 
    Money others_expences;  // ежемесячные дополнительные  расходы 
    int mortgage; //1 - есть ипотека, 2 - ипотеки нет 
} People; 
  
People init (int mortgage, Money begin_capital, Money monthly_payment, People name) { 
    
    name.capital = begin_capital; 
    name.salary = 250*1000; 
    name.others_expences = 40*1000; 
    name.mortgage = mortgage;
    name.apartment_payment = monthly_payment;
    return name; 
    
}  
People year_inflation (People name) { 
    
    if(name.mortgage == 2)
        name.apartment_payment *= 1.08;  // инфляция влияет на арендную плату  
    
    name.others_expences *= 1.08;  //  инфляция влияет на прочие расходы 
    name.salary *= 1.08;  // индексация зарплаты
    
    flat_cost *= 1.08; // индексация стоимости квартиры 
    a++;
    return name;  
    }
    
People Simulation(People name, int start_year, int start_month){
    int end_year = start_year + 30;
    int month = start_month;
    int year = start_year;
    while(! ((year == end_year)&&(month == start_month )))    {
        name.capital = name.capital + name.salary - name.others_expences - name.apartment_payment;
        name.capital *= 1.0167;
        month ++;
        
        if (month == 13){ 
        year_inflation(name);

        year ++;
        month = 1;}
      } 
    return name;
    }
void person_printf(People Alice,People Bob)
  {
    printf("Alice capital = %ld\n", Alice.capital + flat_cost);
    printf("Bob capital = %ld\n", Bob.capital);
    printf(" %ld\n", a);
    
  }
int main(){
  People Alice = init(1,0,178293,Alice);
  People Bob = init(2,1000*1000,30000,Bob);
  Alice = Simulation(Alice,2024,9);
  Bob = Simulation(Bob,2024,9);
  person_printf(Alice,Bob);
}  
 
 
