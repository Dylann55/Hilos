#include <cstdlib>
#include <cstdio>
#include <omp.h>


void init_vec(int *a,int n,int c)
{
    #pragma omp parallel for
    for(int i=0; i<n; ++i)
    {
        a[i]=c*i;
    }
}
//Funcion saxpy con parallel for
void saxpy1(int *s,int *x,int *y,int n,int a,int cs)
{

    //#pragma omp parallel for
    for(int i=0; i<n; ++i)
    {
        s[i]= a*x[i] + y[i];
    }
}
//Funcion saxpy manual con particion de segmentos continuos
void saxpy2(int *s,int *x,int *y,int n,int a,int nt)
{
    #pragma omp parallel shared(s,x,y,n,a,nt)
    {
        int tid = omp_get_thread_num();
        int subsize=(n+nt-1)/nt;
        int start=subsize*tid;
        //printf("Hilo %i start %i subsize %i\n",tid,start,subsize);
        // x1 x2 x3 x4 x5 x6 x7 x8 x9
        // h1 h1 h1 h2 h2 h2 h3 h3 h3

        for(int i=start; i<start+subsize && i<n; i++)
        {
            s[i]=a*x[i]+y[i];
        }
    }
}
//Funcion saxpy manual con particion de segmentos intercalados
void saxpy3(int *s,int *x,int *y,int n,int a,int nt)
{
    #pragma omp parallel shared(s,x,y,n,a,nt)
    {
        // x1 x2 x3 x4 x5 x6 x7 x8 x9
        // h1 h2 h3 h1 h2 h3 h1 h2 h3
        int tid = omp_get_thread_num();
        for(int i=tid; i<n; i=i+nt)
        {
            s[i]=a*x[i]+y[i];
        }

    }
}
void print_vec(int *a,int n, const char *msg)
{
    if(n>32){return;}
    printf("%s\n[",msg);
        for(int i=0; i<n; ++i)
        {
            printf("%i ",a[i]);
        }
        printf("]\n");
}

int main(int argc, char** argv)
{
    long N;
    int nt;
    int m;
    int cs;
    printf("Ingrese Tamano del Vector\n");
    scanf("%i",&N);
    printf("Ingrese Numero de Hilos\n");
    scanf("%i",&nt);
    printf("Ingrese Metodo\n");
    scanf("%i",&m);
    printf("Ingrese Chunksize\n");
    scanf("%d",&cs);
    printf("Por defecto la constante es igual 45\n");

    //funciones OpenMP comienzan con omp_...
    omp_set_num_threads(nt);
    double t1=0.0,t2=0.0;

    //creacion de los vectores
    int *x = new int[N];
    int *y = new int[N];
    int *s = new int[N];

    //inicializar vectores
    init_vec(x,N,1);
    print_vec(x,N,"Vector x");
    init_vec(y,N,2);
    print_vec(y,N,"Vector y");
    init_vec(s,N,0);

    int Constante=45;
    printf("calculando SAXPY con constante=%i.......",Constante);
    t1 = omp_get_wtime();

    //calculo saxpy
    switch(m)
    {
    case 1:
        printf("saxpy1\n");
        saxpy1(s,x,y,N,Constante,cs);
        break;
    case 2:
        printf("saxpy2\n");
        saxpy2(s,x,y,N,Constante,nt);
        break;
    case 3:
        printf("saxpy3\n");
        saxpy3(s,x,y,N,Constante,nt);
        break;

    }
    print_vec(s,N,"Vector s");
    t2= omp_get_wtime();

    printf("Tamano del Vector=%i Numero de Hilos=%i %f segundos\n",N, nt,t2-t1);
    return 0;
}




