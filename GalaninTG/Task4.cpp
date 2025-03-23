int main()
{
    double arr1[] = {5, 6, 7, 8}; 
    double arr2[] = {1, 2, 3, 4}; 

    Matrix A(2, 2, arr1); 
    Matrix B(2, 2, arr2); 

    printf("A = \n");
    A.print(); 

    printf("B = \n");
    B.print(); 

    Matrix C = A + B; 
    printf("C = A + B =\n");
    C.print(); 

    printf("D = A - B =\n");
    Matrix D = A - B; 
    D.print(); 

    printf("E = A * B =\n");
    Matrix E = A * B; 
    E.print(); 

    printf("F = A * 5 =\n"); 
    Matrix F = A * 5; 
    F.print(); 

    printf("transp(A) =\n");
    Matrix T = B.transp();
    T.print();

    printf("det(A) = \n");
    double res = A.determinant(); 

    printf("exp(A) = \n");
    Matrix expA = A.exp(10);
    expA.print();

    printf("A ^ 3 = \n");
    Matrix S = A^3;
    S.print();

    return 0;
}
