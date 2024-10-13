#include <stdio.h> 
#include <math.h> 

int main() 
{
    
    int start = 1000000; 
    float kvar = 20000000; 
    float vklad = 15; 
    float ipoteka = 16; 
    int year = 30; 
    int months = 12; 
    float zp = 200000; 
    float rent = 30000; 
    float rashod = 30000; 
    float inf = 9; 
    
    int S = kvar - start; 
    float r = (ipoteka / 100) / 12; 
    int n = year * 12; 
    float result = pow((1 + r), (float)n); 
    float plata = S * ((r * result) / (result - 1)); 

    
    
    float zp_Bob = zp;
    float rashod_Bob = rashod;
    float ost_Bob;
    float schet_Bob = start; 

    for (int i = 1; i <= year; i++) {         
        for (int j = 1; j <= months; j++) { 
            ost_Bob = zp_Bob - (rent + rashod_Bob); 
            schet_Bob = schet_Bob + ost_Bob; 
            schet_Bob = schet_bob * (1 + (vklad / 100) / 12); 
        }
        zp_Bob = zp_Bob * (1 + (inf / 100)); 
        rashod_Bob = rashod_Bob * (1 + (inf / 100)); 
        rent = rent * (1 + (inf / 100)); 
    }
    
        

   
    float zp_Alice = zp;
    float rashod_Alice = rashod;
    float ost_Alice;
    float schet_Alice = 0; 

    for (int i = 1; i <= year; i++) { 
        for (int j = 1; j <= months; j++) { 
            ost_Alice = zp_Alice - (plata + rashod_Alice); 
            schet_Alice = (schet_Alice + ost_Alice); 
            schet_Alice = schet_Alice * (1 + (vklad / 100) / 12); 
        }
        zp_Alice = zp_Alice * (1 + (inf / 100)); 
        rashod_Alice = rashod_Alice * (1 + (inf / 100)); 
        kvar = kvar * (1 + (inf / 100)); 
    }
    schet_Alice = schet_Alice + kvar;
    


    
    printf("Alice:                                 %.0f rub.\n", schet_Alice);
    printf("Bob:                                   %.0f rub. \n", schet_Bob);
   
  
    getchar();getchar(); 
    return 0; 
    
}