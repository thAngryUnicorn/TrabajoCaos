//TODO LO RANDMO LO HE HECHO CON EL SRAND

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>



#define N1 1000
#define N2 1000

//de mayor a menor
//#define PD_T 1.5 //Beneficio no coopera en encuentro coopera-no coopera   ESTO HAY QUE IR CAMBIANDOLO, LO COMENTO Y LO PONGO COMO VARIABLE GLOBAL O_O
#define PD_R 1.0 //ambos cooperan
#define PD_P 0.0 //No coopera con no coopera
#define PD_S 0.0 //Perdida coopera; encuentro: No coopera - No coopera

//de mayor a menor
//#define SD_W 1.5 //Traicion gana
#define SD_T 1.0
#define SD_L 0.0
#define SD_X -0.4//Castigo traicion traicion

#define p_link 0.005 //esto hay que irlo variando tambien, pero no rallarse de momento
#define conectividad_promedio 6


struct persona{
    int group; //1 o 2
    int cooperate;  //1 (si) o 0 (no)
    double payoff;  //recompensa total al jugar al juego
}; typedef struct persona Persona;




//Genera el array de adyacencia
//La poblacion de cada familia sera un array
//Habra dos arrays, uno para cada familia
void matrix_to_0(int matriz_adyacencia[N1+N2][N1+N2]); //funcion que enlaza la red compleja, realiza la tarea en tres regiones

void link_nodes(int matriz_adyacencia[N1+N2][N1+N2]);
    void links_pop_1(int matriz_adyacencia[N1+N2][N1+N2]);
    void links_pop_2(int matriz_adyacencia[N1+N2][N1+N2]);
    void links_interpop(int matriz_adyacencia[N1+N2][N1+N2]);
    void diag_0(int matriz_adyacencia[N1+N2][N1+N2]);
    int has_link_interpop();
    int has_link_intrapop(int members);

void print_matrix(int matriz_adyacencia[N1+N2][N1+N2]);



//realiza una ronda de interacciones
void round_of_game(int matriz_adyacencia[N1+N2][N1+N2], Persona population_tot[N1+N2]);
    int same_family(int person, int neighbour);
    void PD(Persona population_tot[N1+N2], int person, int neighbour);
    void SD(Persona population_tot[N1+N2], int person, int neighbour);  




//estudio payoffs y nueva poblacion
void study_results(int matriz_adyacencia[N1+N2][N1+N2], Persona population_tot[N1+N2]);//estudia resultados de la array y realiza los cambios a nuevo array
    double maximo_payoff(Persona population_tot[N1+N2]);//halla maximo payoff de entre todas las parejas
    int compare_with_neighbour(Persona population_tot[N1+N2], Persona new_population[N1+N2],int matriz_adyacencia[N1+N2][N1+N2], int person, double max_diff_payoff);//compara payoff 2 vecinos
    void copy_chain(Persona *x, Persona *y, int length);//copia un array de structs en otro







double porcentaje_colaboradores(Persona population_tot[N1+N2]);
void generar_poblacion_0(Persona population_tot[N1+N2]);


//procesado de resultados
double average(double resultados[], int iteraciones);


int matriz_adyacencia[N1+N2][N1+N2]; //inicializar matriz adyacencia




Persona population_tot[N1+N2];

//vamos aumentando el casitgo sometido, tendera a 0
double PD_T = 0.7;
double SD_W = 0.7;


#define poblaciones_parametro 50
int main(){

    time_t start = time(NULL);

    //generar condicion inicial
    generar_poblacion_0(population_tot);
    
    srand(time(NULL));
    FILE *fp;
    fp = fopen("corelite005.txt", "w");

    //loopear durante varios valores de b y obtenere el porcentaje de cooperadores al termalizar

    double var_b = -0.1;
    for(int i = 0; i < 230; i++){
        //este bucle va cambiando el valor de PD_T
        
        PD_T += var_b;
        SD_W += var_b;
        double poblaciones_estudiadas[poblaciones_parametro];
        if (i % 10 == 0)
        printf("%d\n", i);
        //estudia como queda el porcentaje de poblaciones de muchas poblaciones diferentes
        //las poblaciones son distintas, pero coincide PD_T
        for (int j = 0; j < poblaciones_parametro; j++){
            //en cada iteracion comienza una poblacion
            generar_poblacion_0(population_tot);    
            //empezar en cada caso en una situacion aleatoria, genera los links de la poblacion
            matrix_to_0(matriz_adyacencia);
            link_nodes(matriz_adyacencia);
            
            
            //termalizamos el resultado
            for (int k = 0; k < 500; k++){
                //realizamos ronda de juegos para ver los payoffs de cada persona
                round_of_game(matriz_adyacencia,population_tot);
                //tras una ronda
                //estudiamos los resultados de la partida y hacemos cambio en la poblacion
                study_results(matriz_adyacencia,population_tot);
            }
            double fraccion_colaboradores = porcentaje_colaboradores(population_tot);
            poblaciones_estudiadas[j] = fraccion_colaboradores;

        }
        //tras estudiar el resultad de estudiar muchas poblaciones y vemos los colaborades promedio tras mucho tiempo
        double promedio = average(poblaciones_estudiadas, poblaciones_parametro);
        //printf("%lf \n", promedio);
        fprintf(fp, "%lf \t %lf \t\n", PD_T, promedio);

    }

    fclose(fp);    
    //Meter un gif?? podria quedar bonito Opcional
    time_t end = time(NULL);
    printf("%lf", (end-start)/(double)60);
}
















//Evalua si hay que tirar un link entre dos individuos de poblaciones diferentes o no
int has_link_interpop(){

    //generate random numberm
    double random_number = random()/((double)RAND_MAX+1);

    //metodo altura, para condicionar a la variable aleatoria
    //comparamos metodo altura con la probabilidad definida para intrafamilias
    //if random < prob --> return
    if(random_number < p_link) 
        return 1;
    //else
    return 0;

}

int has_link_intrapop(int members){//members es N1 o N2

    //generate random number
    double random_number = random()/((double)RAND_MAX+1);

    //metodo altura, para condicionar a la variable aleatoria
    //probabilidad a  conenctividad promedio (6) red ER:   p = <k>/(N-1)
    double probabiliy_k = conectividad_promedio/(double)(members-1);
    
    //if random < prob --> return yes
    if(random_number < probabiliy_k) 
        return 1;
    //else
    return 0;

}

void matrix_to_0(int matriz_adyacencia[N1+N2][N1+N2]){  //Inicializa la matriz a 0 en todos casillas
    
    for(int i = 0; i < N1+N2; i++){
        for(int j = 0; j < N1 + N2 ; j++){
            matriz_adyacencia[i][j] = 0;
        }
    }
}


void copy_chain(Persona *x, Persona *y, int length){    //copia contenidos una struct en otra
    for(int i=0;i<length;i++){
        y[i].cooperate=x[i].cooperate;
        y[i].payoff=x[i].payoff;
    }
}





/*Añadir un readme con la matriz para que se vea de forma visual*/
void links_pop_1(int matriz_adyacencia[N1+N2][N1+N2]){
    int people_pop_1 = N1;

    //Generar links entre individuos misma poblacion
    for (int i = 0; i < people_pop_1;i++){//individuo que lanza el link

        for(int j = 1; j < people_pop_1; j++){//individuo que recibe el link

            if (has_link_intrapop(N1)){
                matriz_adyacencia[i][j] = 1;//escribir link en matriz en region poblacion 1 con poblaicon 1
                matriz_adyacencia[j][i] = 1;//Asegurar sea simetrica (bidireccional)
            }
        }
    }

}

void links_pop_2(int matriz_adyacencia[N1+N2][N1+N2]){
    int people_pop_2 = N2;

    //Generar links entre individuos misma poblacion
    for (int i = 0; i < people_pop_2;i++){//individuo que lanza el link

        for(int j = 1; j < people_pop_2; j++){//individuo que recibe el link

            if (has_link_intrapop(N2)){
                matriz_adyacencia[i+N1][j+N1] = 1;//escribir link matriz, region poblacion 2 con poblacion 2
                matriz_adyacencia[j+N1][i+N1] = 1;//Asegurar sea simetrica (bidireccional)
            }
        }
    }

}


void links_interpop(int matriz_adyacencia[N1+N2][N1+N2]){
    int people_pop_1 = N1;
    int people_pop_2 = N2;

    //Generar links entre individuos misma poblacion
    for (int i = 0; i < people_pop_1;i++){//individuo que lanza el link

        for(int j = 0; j < people_pop_2; j++){//individuo que recibe el link

            if (has_link_interpop()){
                matriz_adyacencia[i+N1][j] = 1;//escribir link matriz, region poblacion 1 con poblacion 2
                matriz_adyacencia[j][i+N1] = 1;//escribir link matriz, region poblacion 2 con poblacion 1
            }
        }
    }
}

void diag_0(int matriz_adyacencia[N1+N2][N1+N2]){
    for(int i = 0; i < N1+N2;i++){
        matriz_adyacencia[i][i] = 0;
    }
}

void link_nodes(int matriz_adyacencia[N1+N2][N1+N2]){
    links_pop_1(matriz_adyacencia);
    links_pop_2(matriz_adyacencia);
    links_interpop(matriz_adyacencia);
    diag_0(matriz_adyacencia);
}

void print_matrix(int matriz_adyacencia[N1+N2][N1+N2]){
    for(int i = 0; i < N1+N2;i++){
        for (int j = 0; j < N1+N2; j++){
            printf("%d " , matriz_adyacencia[i][j]);
        }
        printf("\n");
    }
}



















void round_of_game(int matriz_adyacencia[N1+N2][N1+N2], Persona population_tot[N1+N2]){
    //bruteforces
    for(int person = 0; person < N1+N2; person++){//every person in population

        for(int neighbour = 0; neighbour < N1+N2; neighbour++){//Every possible neighbour the person plays with

            if (matriz_adyacencia[person][neighbour] == 1){ // if they are linked, then play
                if (same_family(person, neighbour))
                    PD(population_tot,person,neighbour);
                else
                {
                    SD(population_tot, person, neighbour);
                }
                
            }
            //otherwise, they are not neighbours

        }
    }
    /*
    Por como esta programado, cada pareja de vecinos juega 2 veces, no influye si todos van multiplicados por un factor, pero para corregir
    */
   for (int persona = 0; persona < N1+N2; persona++){
       population_tot[persona].payoff /= 2;
   }
}

int same_family(int person, int neighbour){
    if (person < N1 && neighbour < N1) 
        return 1;//ambos pertenecen a la familia 1
    if (person > N1 && neighbour > N1)
        return 1;//ambos pertenencen a la familia 2

    //en cualquier otro caso
    return 0;//familias diferentes
    
}



void PD(Persona population_tot[N1+N2], int person, int neighbour){     //Dilema del Prisionero
    if(population_tot[person].cooperate==1 && population_tot[neighbour].cooperate==1){
        population_tot[person].payoff+=PD_R;
        population_tot[neighbour].payoff+=PD_R;
    }
    if(population_tot[person].cooperate==1 && population_tot[neighbour].cooperate==0){
        population_tot[person].payoff+=PD_S;
        population_tot[neighbour].payoff+=PD_T;
    }
    if(population_tot[person].cooperate==0 && population_tot[neighbour].cooperate==1){
        population_tot[person].payoff+=PD_T;
        population_tot[neighbour].payoff+=PD_S;
    }
    if(population_tot[person].cooperate==0 && population_tot[neighbour].cooperate==0){
        population_tot[person].payoff+=PD_P;
        population_tot[neighbour].payoff+=PD_P;
    }
}

void SD(Persona population_tot[N1+N2], int person, int neighbour){     //Halcón-Paloma
    if(population_tot[person].cooperate==1 && population_tot[neighbour].cooperate==1){
        population_tot[person].payoff+=SD_T;
        population_tot[neighbour].payoff+=SD_T;
    }
    if(population_tot[person].cooperate==1 && population_tot[neighbour].cooperate==0){
        population_tot[person].payoff+=SD_L;
        population_tot[neighbour].payoff+=SD_W;
    }
    if(population_tot[person].cooperate==0 && population_tot[neighbour].cooperate==1){
        population_tot[person].payoff+=SD_W;
        population_tot[neighbour].payoff+=SD_L;
    }
    if(population_tot[person].cooperate==0 && population_tot[neighbour].cooperate==0){
        population_tot[person].payoff+=SD_X;
        population_tot[neighbour].payoff+=SD_X;
    }

}




























void study_results(int matriz_adyacencia[N1+N2][N1+N2], Persona population_tot[N1+N2]){
    
    Persona new_population[N1+N2];
    copy_chain(population_tot,new_population,N1+N2);//copiamos la buena en la temporar
    double max_diff_payoff = maximo_payoff(population_tot);//halla la maxima diferencia de payoff
    
    for (int person = 0; person < N1 + N2 ; person++){
        compare_with_neighbour(population_tot, new_population, matriz_adyacencia, person, max_diff_payoff);
    }
    
    //ahora tendremos ya la nueva poblacion (cambios de cooperacion)
    copy_chain(new_population, population_tot, N1+N2);//copiamos la temporal en la buena para tener la nueva

    //al acabar reset all payoff 0
    for(int i = 0; i < N1+N2; i++){
        population_tot[i].payoff = 0;
    }
    
}

double maximo_payoff(Persona population_tot[N1+N2]){
    double max = 0;
    double min = 0;
    for(int person = 0; person < N1+N2; person++){
        if (population_tot[person].payoff > max)
            max = population_tot[person].payoff;
        if (population_tot[person].payoff < min)
            min = population_tot[person].payoff;
        
    }

    return  max-min;
}


//compara con el vecino y devuelve si cambia o no
int compare_with_neighbour(Persona population_tot[N1+N2], Persona new_population[N1+N2],int matriz_adyacencia[N1+N2][N1+N2], int person, double max_diff_payoff){
    int neighbours[N1+N2];
    int index_neigh = 0;
    int at_least_one_link = 0;

    
    //obtener listado vecinos
    for (int i = 0; i < N1+N2; i++){
        if (matriz_adyacencia[person][i]){
            neighbours[index_neigh] = i;
            index_neigh += 1;
        }

    }



    
    //seleccionamos al azar el vecino con el que comparar
    if (index_neigh != 0) //evita que de error de coma flotante al dividir entre 0
        index_neigh = rand() % index_neigh; //genera un integre aleatorio entre 0 e index_neig-1
    //printf("%d\n", index_neigh);

    //Nuestro vecino sera: neighbours[index_neigh]

    int vecino = neighbours[index_neigh];
    
    
    double payoff_rival = population_tot[vecino].payoff;
    double diff_payoff = payoff_rival-population_tot[person].payoff ;
    //si rival tiene mas dineros
    
    if (diff_payoff > 0){
        //proporcional a la diferneicai de dinero, metodo altura
        double aleatoria = rand()/((double)RAND_MAX+1);
        double comparar = diff_payoff/max_diff_payoff;
        if (aleatoria < comparar){
            new_population[person].cooperate = population_tot[vecino].cooperate;
        }
    }

    
    

}












double porcentaje_colaboradores(Persona population_tot[N1+N2]){
    double colaboradores = 0;
    for (int i = 0; i < N1+N2; i++){
        if (population_tot[i].cooperate)
            colaboradores+=1;
    }
    return colaboradores/(double)(N1+N2);
}











void generar_poblacion_0(Persona population_tot[N1+N2]){
    for (int i = 0; i < N1+N2; i++){
        population_tot[i].cooperate = random()%2;
        population_tot[i].payoff = 0;
        population_tot[i].group = 0;
    }
}



double average(double resultados[], int iteraciones){
    double sum = 0;
    
    for (int i = 0; i < iteraciones; i++){
        sum += resultados[i];
    }
    return sum/((double)iteraciones);
}