/************************************
* INCLUDES
************************************/

#include "../include/compression_tools.h"
#include <stdio.h>


/************************************
* EXTERN VARIABLES
************************************/

const int Q_Y[8][8] = {
    {16,11,10,16,24,40,51,61},
    {12,12,14,19,26,58,60,55},
    {14,13,16,24,40,57,69,56},
    {14,17,22,29,51,87,80,62},
    {18,22,37,56,68,109,103,77},
    {24,35,55,64,81,104,113,92},
    {49,64,78,87,103,121,120,101},
    {72,92,95,98,112,100,103,99}
};

const int Q_C[8][8] = {
    {17,18,24,47,99,99,99,99},
    {18,21,26,66,99,99,99,99},
    {24,26,56,99,99,99,99,99},
    {47,66,99,99,99,99,99,99},
    {99,99,99,99,99,99,99,99},
    {99,99,99,99,99,99,99,99},
    {99,99,99,99,99,99,99,99},
    {99,99,99,99,99,99,99,99}
};

const int zigzag[64][2] = {
    {0,0}, {0,1}, {1,0}, {2,0}, {1,1}, {0,2}, {0,3}, {1,2},
    {2,1}, {3,0}, {4,0}, {3,1}, {2,2}, {1,3}, {0,4}, {0,5},
    {1,4}, {2,3}, {3,2}, {4,1}, {5,0}, {6,0}, {5,1}, {4,2},
    {3,3}, {2,4}, {1,5}, {0,6}, {0,7}, {1,6}, {2,5}, {3,4},
    {4,3}, {5,2}, {6,1}, {7,0}, {7,1}, {6,2}, {5,3}, {4,4},
    {3,5}, {2,6}, {1,7}, {2,7}, {3,6}, {4,5}, {5,4}, {6,3},
    {7,2}, {7,3}, {6,4}, {5,5}, {4,6}, {3,7}, {4,7}, {5,6},
    {6,5}, {7,4}, {7,5}, {6,6}, {5,7}, {6,7}, {7,6}, {7,7}
};

const HuffmanCode HUFFMAN_DC[12] = {
    {0b00,         2},  // Categoria 0
    {0b010,        3},  // Categoria 1
    {0b011,        3},  // Categoria 2
    {0b100,        3},  // Categoria 3
    {0b101,        3},  // Categoria 4
    {0b110,        3},  // Categoria 5
    {0b1110,       4},  // Categoria 6
    {0b11110,      5},  // Categoria 7
    {0b111110,     6},  // Categoria 8
    {0b1111110,    7},  // Categoria 9
    {0b11111110,   8},  // Categoria 10
    {0b111111110,  9}   // Categoria 11
};


/************************************
* PRIVATE MACROS AND DEFINES
************************************/

/************************************
* PRIVATE TYPEDEFS
************************************/

/************************************
* STATIC VARIABLES
************************************/

/************************************
* GLOBAL VARIABLES
************************************/

/************************************
* STATIC FUNCTION PROTOTYPES
************************************/

/************************************
* STATIC FUNCTIONS
************************************/

/************************************
* GLOBAL FUNCTIONS
************************************/

void quantizar_bloco(double bloco[8][8], const int Q[8][8], double k)
{
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            bloco[i][j] = round(bloco[i][j] / (k * Q[i][j]));
        }
    }
}


double** quantizar_matriz(double** matriz, int altura, int largura, const int Q[8][8], double k)
{
    double** saida = malloc(altura * sizeof(double*));
    for (int i = 0; i < altura; i++) {
        saida[i] = malloc(largura * sizeof(double));
    }

    double bloco[8][8];

    for (int i_bloco = 0; i_bloco < altura; i_bloco += 8) {
        for (int j_bloco = 0; j_bloco < largura; j_bloco += 8) {

            // Copia o bloco original
            for (int i = 0; i < 8; i++) {
                for (int j = 0; j < 8; j++) {
                    bloco[i][j] = matriz[i_bloco + i][j_bloco + j];
                }
            }

            // Aplica quantização
            quantizar_bloco(bloco, Q, k);

            // Copia de volta
            for (int i = 0; i < 8; i++) {
                for (int j = 0; j < 8; j++) {
                    saida[i_bloco + i][j_bloco + j] = bloco[i][j];
                }
            }
        }
    }

    return saida;
}


YCbCrImg quantizar_imagem(YCbCrImg img_dct, double k)
{
    YCbCrImg quantizado;

    quantizado.height = img_dct.height;
    quantizado.width = img_dct.width;

    // Canal Y usa matriz Q_Y
    quantizado.Y = quantizar_matriz(img_dct.Y, img_dct.height, img_dct.width, Q_Y, k);

    // Cb e Cr usam matriz Q_C (tamanhos reduzidos)
    quantizado.Cb = quantizar_matriz(img_dct.Cb, img_dct.height / 2, img_dct.width / 2, Q_C, k);
    quantizado.Cr = quantizar_matriz(img_dct.Cr, img_dct.height / 2, img_dct.width / 2, Q_C, k);

    return quantizado;
}


void desquantizar_bloco(double bloco[8][8], const int Q[8][8], double k)
{
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            bloco[i][j] = bloco[i][j] * Q[i][j] * k;
        }
    }
}


double** desquantizar_matriz(double** matriz, int altura, int largura, const int Q[8][8], double k)
{
    double** saida = malloc(altura * sizeof(double*));
    for (int i = 0; i < altura; i++) {
        saida[i] = malloc(largura * sizeof(double));
    }

    double bloco[8][8];

    for (int i_bloco = 0; i_bloco < altura; i_bloco += 8) {
        for (int j_bloco = 0; j_bloco < largura; j_bloco += 8) {

            // Copia o bloco original
            for (int i = 0; i < 8; i++) {
                for (int j = 0; j < 8; j++) {
                    bloco[i][j] = matriz[i_bloco + i][j_bloco + j];
                }
            }

            // Aplica desquantização
            desquantizar_bloco(bloco, Q, k);

            // Copia de volta
            for (int i = 0; i < 8; i++) {
                for (int j = 0; j < 8; j++) {
                    saida[i_bloco + i][j_bloco + j] = bloco[i][j];
                }
            }
        }
    }

    return saida;
}


YCbCrImg desquantizar_imagem(YCbCrImg quantizado, double k)
{
    YCbCrImg desquantizado;

    desquantizado.height = quantizado.height;
    desquantizado.width = quantizado.width;

    // Canal Y usa matriz Q_Y
    desquantizado.Y = desquantizar_matriz(quantizado.Y, quantizado.height, quantizado.width, Q_Y, k);

    // Cb e Cr usam matriz Q_C (tamanhos reduzidos)
    desquantizado.Cb = desquantizar_matriz(quantizado.Cb, quantizado.height / 2, quantizado.width / 2, Q_C, k);
    desquantizado.Cr = desquantizar_matriz(quantizado.Cr, quantizado.height / 2, quantizado.width / 2, Q_C, k);

    return desquantizado;
}


int* aplicar_zigzag(double bloco[8][8])
{
    int* vetor = malloc(64 * sizeof(int));

    for (int i = 0; i < 64; i++) {
        int x = zigzag[i][0];
        int y = zigzag[i][1];
        vetor[i] = (int)round(bloco[x][y]);  // pós-quantização, valores já são inteiros
    }

    return vetor;
}

int calcular_diferenca_dc(int atual, int anterior)
{
    return atual - anterior;
}

int aplicar_rle_ac(int vetor[64], Par_RLE* pares)
{
    int pos = 0;  // posição no vetor de saída
    int run = 0;

    for (int i = 1; i < 64; i++) {
        if (vetor[i] == 0) {
            run++;
            if (run == 16) {
                pares[pos++] = (Par_RLE){15, 0}; // código especial: 16 zeros
                run = 0;
            }
        } else {
            pares[pos++] = (Par_RLE){run, vetor[i]};
            run = 0;
        }
    }

    if (run > 0) {
        pares[pos++] = (Par_RLE){0, 0}; // End of Block
    }

    return pos; // quantidade de pares gerados
}

int codificar_bloco(int bloco[8][8], int dc_anterior, int* diferenca_dc, Par_RLE* ac_pares)
{
     int *vetor = aplicar_zigzag(bloco); // nova função

    *diferenca_dc = calcular_diferenca_dc(vetor[0], dc_anterior);

    int n_pares = aplicar_rle_ac(vetor, ac_pares);

    free(vetor); // liberar memória alocada
    return n_pares;
}


