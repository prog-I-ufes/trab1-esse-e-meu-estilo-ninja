#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "distancias.h"
#include "rotulos.h"

/* TRABALHO DE PROGRAMAÇAO 2 
Problematizaçao: Implementaçao de um algoritmo de classificaçao de dados
usando a linguagem C. Neste trabalho foi escolhido o algoritmo chamado KNN
(K-Nearest Neighbors) para ser implementado. Haverá também a avaliaçao do
desempenho deste classificador através de duas métricas: Acurácia e Matriz
de Confusao.*/

typedef struct{
    FILE *treino;
    FILE *teste;
} Arquivos;

typedef struct{
    float **mat;
    int lin;
    int col;
} Matriz;

char ** cria_container(){//Cria um vetor de tamanho especifico para armazenar as informacoes lidas em config.txt
    char **KNN = malloc(sizeof(char *) * 3);
    for (int i = 0; i < 3; i++) {
        KNN[i] = malloc(sizeof(char) * 4);
        // KNN[i] = NULL;
    }

    return KNN;
}

Matriz *cria_matriz(int linhas, int colunas){
    Matriz *m = malloc(sizeof(Matriz));
    m->mat = malloc(sizeof(int *) * linhas);
    for (int i = 0; i < linhas; i++)
        m->mat[i] = malloc(sizeof(int) * colunas);

    m->lin = linhas;
    m->col = colunas;
    return m;
}

void free_matriz(Matriz *m){
    for (int i = 0; i < m->lin; i++)
        free(m->mat[i]);
    free(m->mat);
    free(m);
}

void free_arquivos(Arquivos *arquivo){
    fclose(arquivo->treino);
    fclose(arquivo->teste);
    free(arquivo);
}

void free_container(char **KNN){//dá free no container
    for (int i = 0; i < 3; i++)
        if(KNN[i] != NULL) free(KNN[i]);
    free(KNN);
}

char * tira_quebralinha(char *str){//Tira a quebra de linha do final de uma string
    int tam = strlen(str);
    if (str[tam - 1] == '\n')
        str[tam - 1] = '\0';
    return str;
}

int conta_col(FILE *arq){
    rewind(arq);
    int c;
    int col = 0;
    while (c = fgetc(arq), c != '\n' && c != EOF && c != '\0')
        if (c == ',') col++;
    rewind(arq);
    return col;
}

int conta_linha(FILE *arq){
    rewind(arq);
    int linhas = 0;
    while((fscanf(arq, "%*[^\n]\n", NULL)) != EOF) //Ignora o valor da linha
        linhas++;
    rewind(arq);
    return linhas;
}

int numRotulos = 0;
Matriz *le_matriz(Matriz *m, int *rotulos, FILE *f){
    rewind(f);
    float num;
    for (int i = 0; i < m->lin; i++){
        for (int j = 0; j < m->col; j++)
            fscanf(f, "%f, ", &m->mat[i][j]);
        fscanf(f, "%f", &num); rotulos[i] = (int)num; //Transforma o rotulo em int
        if (rotulos[i] > numRotulos) numRotulos = rotulos[i];
    }
    rewind(f);
    return m;
}

void printa_matriz(Matriz m){
    for (int i = 0; i < m.lin; i++){
        for (int j = 0; j < m.col; j++)
            printf("%f ", m.mat[i][j]);
        printf("\n");
    }
}

char ** computa_tipo_de_distancia(char *buff, char **KNN){
    strcpy(KNN[2], ""); //Zera o valor de KNN na terceira posicao
                        //Para o caso de ter sido computado uma distancia de Minkowsky por ultimo

    char *padrao = " ,\t\n\0"; //Padrao para quebrar a string
    char *K = strtok(buff, padrao); //Token gerado apos quebra

    int i = 0;//Posicao onde armazena a informação
    strcpy(KNN[i++], K);

    char *distancia = strtok(NULL, padrao);
    strcpy(KNN[i++], distancia);
    if (distancia[0] == 'M'){
        char *r = strtok(NULL, padrao);
        
        strcpy(KNN[i++], r);
    }

    return KNN;
}

char saida_final[256]; //Variavel global para guardar o caminho da saida
Arquivos *le_configIO(FILE *config){//Le os dados de entrada e saida de config.txt
    char buffer[256]; //Cria um buffer temporário de tamanho 256
    fgets(buffer, 256, config); //Lê a primeira linha do arquivo (representa o arquivo treino)
    char *treino = tira_quebralinha(buffer);

    Arquivos *arqs = malloc(sizeof(Arquivos));
    arqs->treino = fopen(treino, "r"); //Abre o arquivo de treino
    
    fgets(buffer, 256, config); //Lê a segunda linha do arquivo (representa o arquivo teste)
    char *teste = tira_quebralinha(buffer);
    arqs->teste = fopen(teste, "r"); //Abre o arquivo de teste

    fgets(buffer, 256, config); //Lê a terceira linha do arquivo (representa a saída)
    char *saidaTmp = tira_quebralinha(buffer);
    strcpy(saida_final, saidaTmp);

    return arqs;
}

int calcula_rotulo(Rotulo *vizinhos, int K){
    int rotulosFreq[numRotulos+1]; //Inicializa um vetor onde cada posição eh a frequencia do rótulo
    for (int i = 0; i < numRotulos+1; i++) //referente à posiçao
        rotulosFreq[i] = 0;

    for (int i = 0; i < K; i++){
        int rotulo = vizinhos[i].rotulo;//Computa a frequencia dos rotulos

        rotulosFreq[rotulo] += 1;
    }

    int rotuloMaisComum = 0;
    for (int i = 0; i < numRotulos+1; i++){//Checa qual rótulo aparece mais vezes, levando em conta
        int freq = rotulosFreq[i];         //o caso de empate
        if (freq > rotuloMaisComum) rotuloMaisComum = i;
    }
    return rotuloMaisComum;
}

float calcula_acuracia(int *rotulosTeste, int *vetorRotulos, int numAmostras){
    float acertos = 0;

    for (int i = 0; i < numAmostras; i++)
        if (rotulosTeste[i] == vetorRotulos[i]) acertos++;

    float amostrasF = (float)numAmostras;

    return (acertos/amostrasF) * 100;
}

int **cria_matrizConfusao(int numAmostras, int *vetorRotulos, int *rotulosTeste){
    int **matConf = malloc(sizeof(int *) * numRotulos);
    for (int i = 0; i < numRotulos; i++)
        matConf[i] = malloc(sizeof(int) * numRotulos);

    
    for(int i = 0; i < numRotulos; i++)
        for(int j = 0; j < numRotulos; j++)
            matConf[i][j] = 0;    
        
    for (int i = 0; i < numAmostras; i++)
        matConf[vetorRotulos[i] - 1][rotulosTeste[i] - 1]++;

    return matConf;
}

void free_matrizConfusao(int **matriz){
    for (int i = 0; i < numRotulos; i++)
        free(matriz[i]);
    free(matriz);
}

void faz_predicao(int predNum, Rotulo *vizinhos, int numAmostras,
                        int K, int *rotulosTeste, int *vetorRotulos){
    char predicao[50];
    strcpy(predicao, saida_final);//Faz append no diretório de saída com a predição atual
    int tam = strlen(predicao);
    sprintf(predicao + tam, "predicao_%d.txt", predNum);

    FILE *pred = fopen(predicao, "w+");

    float acc = calcula_acuracia(rotulosTeste, vetorRotulos, numAmostras);
    fprintf(pred, "%f\n\n", acc);

    int **matConf = cria_matrizConfusao(numAmostras, vetorRotulos, rotulosTeste);

    for (int i = 0; i < numRotulos; i++){
        for (int j = 0; j < numRotulos; j++)
            fprintf(pred, "%d ", matConf[i][j]);
        fprintf(pred, "\n");
    }

    fprintf(pred, "\n");
    for (int i = 0; i < numAmostras; i++)
        fprintf(pred, "%d\n", vetorRotulos[i]-1); //Escreve os rótulos achados

    free_matrizConfusao(matConf);
    fclose(pred);//Fecha o arquivo
}

void le_configDados(Arquivos *arqs, FILE *config){//Le os dados desejados de config.txt
    char **KNN = cria_container();

    int linha_treino = conta_linha(arqs->treino);
    int linha_teste = conta_linha(arqs->teste); //Processa a quantidade de linhas
    int col = conta_col(arqs->treino);          //e colunas das matrizes

    Matriz *m_teste = cria_matriz(linha_teste, col); 
    Matriz *m_treino = cria_matriz(linha_treino, col);
    
    int *rotulosTeste = malloc(sizeof(int) * linha_teste);//Cria um vetor de rótulos 
                                                              //para comparar com a saida final


    int *rotulosTreino = malloc(sizeof(int) * linha_treino);//Cria um vetor de rótulos
                                                                //para comparar em tempo de execução

    m_teste = le_matriz(m_teste, rotulosTeste, arqs->teste);
    m_treino = le_matriz(m_treino, rotulosTreino, arqs->treino);

    int tamVetorDistancias = linha_treino;
    Rotulo *vetorDistancias = malloc(sizeof(Rotulo) * 
                                tamVetorDistancias); //Vetor de distâncias q será ordenado0
    int vetorRotulos[linha_teste];
    for (int i = 0; i < linha_teste; i++)
        vetorRotulos[i] = 0;

    free_arquivos(arqs);
    
    char buffer[256];
    int predNum = 1;
    while (fgets(buffer, 256, config) != NULL){ //Uma especie de loop principal
        computa_tipo_de_distancia(buffer, KNN);

        int K = atoi(KNN[0]);
        float dist = 0;
        if (KNN[1][0] == 'E'){
            dist = 0;

            for (int j = 0; j < linha_teste; j++){
                for (int i = 0; i < linha_treino; i++){
                    dist = euclid(m_teste->mat[j], m_treino->mat[i], col);
                    
                    vetorDistancias[i] = (Rotulo){rotulosTreino[i], dist};
                }
                    qsort(vetorDistancias, linha_teste, sizeof(Rotulo), cmpfunc);
                    vetorRotulos[j] = calcula_rotulo(vetorDistancias, K);
            }
            faz_predicao(predNum, vetorDistancias, linha_teste,
                            K, rotulosTeste, vetorRotulos);


        }
        else if (KNN[1][0] == 'M'){
            dist = 0;
            float R = atof(KNN[2]);

            for (int j = 0; j < linha_teste; j++){
                for (int i = 0; i < linha_treino; i++){
                    dist = m_dist(m_teste->mat[j], m_treino->mat[i], col, R);
                    
                    vetorDistancias[i] = (Rotulo){rotulosTreino[i], dist};
                }
                    qsort(vetorDistancias, linha_teste, sizeof(Rotulo), cmpfunc);

                    vetorRotulos[j] = calcula_rotulo(vetorDistancias, K);
            }
            faz_predicao(predNum, vetorDistancias, linha_teste,
                            K, rotulosTeste, vetorRotulos);
        }
        else if (KNN[1][0] == 'C'){
            dist = 0;

            for (int j = 0; j < linha_teste; j++){
                for (int i = 0; i < linha_treino; i++){
                    dist = c_dist(m_teste->mat[j], m_treino->mat[i], col);

                    vetorDistancias[i] = (Rotulo){rotulosTreino[i], dist};
                }
                    qsort(vetorDistancias, linha_teste, sizeof(Rotulo), cmpfunc);
                    vetorRotulos[j] = calcula_rotulo(vetorDistancias, K);
            }
            faz_predicao(predNum, vetorDistancias, linha_teste,
                            K, rotulosTeste, vetorRotulos);
        }
        
        predNum++;
    }
    free_matriz(m_teste);
    free_matriz(m_treino);
    free_container(KNN);
    free(rotulosTeste);
    free(rotulosTreino);
    free(vetorDistancias);
}

int main(int argc, char *argv[1]){
    if (argc != 2){
        printf("Passe o arquivo de entrada\n");
        exit(1);
    }
    FILE *config = fopen(argv[1], "r"); //Abre o arquivo pra leitura
    if (config == NULL){ //Checa se abriu o arquivo com sucesso
        printf("Falha ao abrir arquivo\n"); //Exibe uma menssagem de erro
        exit(1);
    }


    Arquivos *arq = le_configIO(config);
    le_configDados(arq, config);


    fclose(config);    

    return 0;
}