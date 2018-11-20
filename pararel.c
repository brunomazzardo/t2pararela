#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

//#define SIZE 1000000      // trabalho final com o valores 10.000, 100.000, 1.000.000

// BUBBLE SORT
void BubbleSort(int n, int * vetor) {
  int c =0, d, troca, trocou =1;

  while (c < (n-1) && trocou ) {
        trocou = 0;
        for (d = 0 ; d < n - c - 1; d++)
            if (vetor[d] > vetor[d+1]) {
                troca      = vetor[d];
                vetor[d]   = vetor[d+1];
                vetor[d+1] = troca;
                trocou = 1;
            }
        c++;
  }
}

void JuntaFilho(int n, int *filhoA, int *filhoB,int *pai){
    // Método que ordena dois arrays ordenados em um terceiro array
    int i = 0;
    int contA = 0;
    int contB = 0;

    while(i < n){
        if(filhoA[contA] > filhoB[contB]){
            pai[i] = filhoB[contB];
            contB++;
        }else{
             pai[i] = filhoA[contA];
             contA++;
        }
        i++;
    }
}

void logicaPai(int id, int *array,int np){   
  //Método que contém a logica do pai, se o numero de processos for 0, ele executa um bubble sort  
 double elapsed_time = - MPI_Wtime ();
 if(np !=1){
    int a = id * 2 + 1 ;
    int b = a + 1;

    MPI_Status success;  

    int len  = SIZE/2;
    int arrayA[len], arrayB[len];

    MPI_Send(&len,1,MPI_INT,a,0,MPI_COMM_WORLD);   
    MPI_Send(&len,1,MPI_INT,b,0,MPI_COMM_WORLD);  

    MPI_Send(array,len,MPI_INT,a,0,MPI_COMM_WORLD);   
    MPI_Send(array + len,len,MPI_INT,b,0,MPI_COMM_WORLD);  

    MPI_Recv(arrayA,len,MPI_INT,a,0,MPI_COMM_WORLD,&success);
    MPI_Recv(arrayB,len,MPI_INT,b,0,MPI_COMM_WORLD,&success); 
    JuntaFilho(SIZE,arrayA,arrayB,array);
 }else {
    BubbleSort(SIZE, &array[0]);
 }
  elapsed_time += MPI_Wtime();
  printf("tempo: %lf \n",elapsed_time);
}

void logicaTronco(int id){

//Método que realizada logica do tronco

 MPI_Status success;  

//Busca o ID do pai dele
 int pai = (id - 1 ) / 2;


//Defini o id dos processos que serão seus filhos
 int a = id * 2 + 1 ,b = a + 1;

 int len;


//Recebe o tamanho do array que vai dividir pros filhos do pai
 MPI_Recv(&len,1,MPI_INT,pai,0,MPI_COMM_WORLD,&success);

 int array[len]; 


//Recebe o array do pai
 MPI_Recv(array,len,MPI_INT,pai,0,MPI_COMM_WORLD,&success); 

 int len_filho = len/2;

 int arrayA[len_filho], arrayB[len_filho];


//Envia o tamnho do array para os filhos
 MPI_Send(&len_filho,1,MPI_INT,a,0,MPI_COMM_WORLD);   
 MPI_Send(&len_filho,1,MPI_INT,b,0,MPI_COMM_WORLD);  


//Envia o array para os filhos
 MPI_Send(array,len_filho,MPI_INT,a,0,MPI_COMM_WORLD);   
 MPI_Send(array + len_filho,len_filho,MPI_INT,b,0,MPI_COMM_WORLD);  


//Recebe o array dos filhos
 MPI_Recv(arrayA,len_filho,MPI_INT,a,0,MPI_COMM_WORLD,&success);
 MPI_Recv(arrayB,len_filho,MPI_INT,b,0,MPI_COMM_WORLD,&success); 


//Ordena os arrays
 JuntaFilho(len,arrayA,arrayB,array);


//Envia o array ordernado para o pai
 MPI_Send(array,len,MPI_INT,pai,0,MPI_COMM_WORLD);

}
void logicaFolha(int id){


//Logica da folha
 MPI_Status success;  



 int pai = (id - 1 ) / 2;

 int a = id * 2 + 1 ,b = a + 1;

 int len;

 MPI_Recv(&len,1,MPI_INT,pai,0,MPI_COMM_WORLD,&success);

 int array[len]; 

 MPI_Recv(array,len,MPI_INT,pai,0,MPI_COMM_WORLD,&success); 


//Ordena com o bubble sort fornecido no codigo inicial
 BubbleSort(len, &array[0]);

 MPI_Send(array,len,MPI_INT,pai,0,MPI_COMM_WORLD);   
    
}

int main(int argc, char *argv[]) {
  int array[SIZE], p, id,i ;

  MPI_Init(&argc,&argv);
  MPI_Comm_size(MPI_COMM_WORLD, &p);
  MPI_Comm_rank(MPI_COMM_WORLD, &id);


  if(id == 0){
   for (i=0 ; i<SIZE; i++)
      array[i] = SIZE-i;
   logicaPai(id,array,p);
  }else if(p/2 > id){
    logicaTronco(id);
  }else{
    logicaFolha(id);
  }
           
  if(id == 0){
    // VERIFICA SE A ORDENAÇÃO FUNCIONOU
    for (i=0 ; i<SIZE-1; i++)
        if (array[i] > array[i+1])
            return 1;
  }
  MPI_Finalize();
  return 0;
}