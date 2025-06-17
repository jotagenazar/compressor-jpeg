/************************************
* INCLUDES
************************************/

    #include "../include/entropia.h"
    #include <stdio.h>

/************************************
* EXTERN VARIABLES
************************************/

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


