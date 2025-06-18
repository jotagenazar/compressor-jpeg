/************************************
* INCLUDES
************************************/

#include "../include/compression_tools.h"
#include <stdio.h>
#include "string.h"


/************************************
* EXTERN VARIABLES
************************************/

const int Q_Y[8][8] = { // Matriz para fazer a Quantizacao de Y
    {16,11,10,16,24,40,51,61},
    {12,12,14,19,26,58,60,55},
    {14,13,16,24,40,57,69,56},
    {14,17,22,29,51,87,80,62},
    {18,22,37,56,68,109,103,77},
    {24,35,55,64,81,104,113,92},
    {49,64,78,87,103,121,120,101},
    {72,92,95,98,112,100,103,99}
};

const int Q_C[8][8] = { // Matriz para fazer a quantizacao de Cb e Cr
    {17,18,24,47,99,99,99,99},
    {18,21,26,66,99,99,99,99},
    {24,26,56,99,99,99,99,99},
    {47,66,99,99,99,99,99,99},
    {99,99,99,99,99,99,99,99},
    {99,99,99,99,99,99,99,99},
    {99,99,99,99,99,99,99,99},
    {99,99,99,99,99,99,99,99}
};

const int zigzag[64][2] = { // Matriz para acelerar o preenchimento do vetor na sequencia zigzag
    {0,0}, {0,1}, {1,0}, {2,0}, {1,1}, {0,2}, {0,3}, {1,2},
    {2,1}, {3,0}, {4,0}, {3,1}, {2,2}, {1,3}, {0,4}, {0,5},
    {1,4}, {2,3}, {3,2}, {4,1}, {5,0}, {6,0}, {5,1}, {4,2},
    {3,3}, {2,4}, {1,5}, {0,6}, {0,7}, {1,6}, {2,5}, {3,4},
    {4,3}, {5,2}, {6,1}, {7,0}, {7,1}, {6,2}, {5,3}, {4,4},
    {3,5}, {2,6}, {1,7}, {2,7}, {3,6}, {4,5}, {5,4}, {6,3},
    {7,2}, {7,3}, {6,4}, {5,5}, {4,6}, {3,7}, {4,7}, {5,6},
    {6,5}, {7,4}, {7,5}, {6,6}, {5,7}, {6,7}, {7,6}, {7,7}
};

// * essa é a tabela 3. 
// TODO: VOLTAR AQUI E PENSAR SE VAMOS USAR ELA ASSIM
// Códigos de prefixo Huffman para as categorias dos coeficientes DC (Luminância)
const char* huffman_dc_lum_codes[12] = {
    "010",      // Categoria 0
    "011",      // Categoria 1
    "100",      // Categoria 2
    "00",       // Categoria 3
    "101",      // Categoria 4
    "110",      // Categoria 5
    "1110",     // Categoria 6
    "11110",    // Categoria 7
    "111110",   // Categoria 8
    "1111110",  // Categoria 9
    "11111110", // Categoria A (10)
    "111111110" // Categoria B (11) - Não está na sua tabela, mas completando o padrão
};

// TODO: Revisar se a tabela está correta. Me parece que não
// Códigos de prefixo Huffman para os coeficientes AC (Luminância)
// Formato: huffman_ac_lum_codes[qtde_zeros][categoria]
// Categoria 0 é o EOB (End of Block)
// Categoria > 10 são mais raros e podem ser omitidos inicialmente
const char* huffman_ac_lum_codes[16][11] = {
    // size (categoria)
    // 0(ESP)    1        2         3          4           5            6             7              8               9                  10
    {"1010",   "00",    "01",     "100",     "1011",    "11010",     "111000",     "1111000",     "111110110",   "11111110110",   "1111111110000011"}, // 0 zeros
    {NULL,     "1100",  "11011",  "1111001", "11111000", "1111110011", "111111110101", "1111111110000100", "1111111110000101", "1111111110000110", NULL},                // 1 zero
    {NULL,     "111010", "11111010", "1111110111", "111111110110", "1111111110000111", "1111111110001000", "1111111110001001", NULL,               NULL,                NULL},                // 2 zeros
    {NULL,     "111011", "11111011", "11111111000", "1111111110001010", "1111111110001011", "1111111110001100", NULL,               NULL,               NULL,                NULL},                // 3 zeros
    {NULL,     "1111010", "111111010", "111111110111", "1111111110001101", "1111111110001110", NULL,               NULL,               NULL,               NULL,                NULL},                // 4 zeros
    {NULL,     "1111011", "1111111010", "1111111110010000", "1111111110010001", NULL,               NULL,               NULL,               NULL,               NULL,                NULL},                // 5 zeros
    {NULL,     "11111001", "1111111011", "1111111110010010", NULL,               NULL,               NULL,               NULL,               NULL,               NULL,                NULL},                // 6 zeros
    {NULL,     "11111011", "111111110100", "1111111110010011", NULL,               NULL,               NULL,               NULL,               NULL,               NULL,                NULL},                // 7 zeros
    {NULL,     "111111000", "1111111110010100", NULL,               NULL,               NULL,               NULL,               NULL,               NULL,               NULL,                NULL},                // 8 zeros
    {NULL,     "1111110010", "1111111110010101", NULL,               NULL,               NULL,               NULL,               NULL,               NULL,               NULL,                NULL},                // 9 zeros
    {NULL,     "1111110100", "1111111110010110", NULL,               NULL,               NULL,               NULL,               NULL,               NULL,               NULL,                NULL},                // 10 zeros
    {NULL,     "1111110101", "1111111110010111", NULL,               NULL,               NULL,               NULL,               NULL,               NULL,               NULL,                NULL},                // 11 zeros
    {NULL,     "1111110110", "1111111110011000", NULL,               NULL,               NULL,               NULL,               NULL,               NULL,               NULL,                NULL},                // 12 zeros
    {NULL,     "1111111000", "1111111110011001", NULL,               NULL,               NULL,               NULL,               NULL,               NULL,               NULL,                NULL},                // 13 zeros
    {NULL,     "1111111001", "1111111110011010", NULL,               NULL,               NULL,               NULL,               NULL,               NULL,               NULL,                NULL},                // 14 zeros
    {"11111111001", NULL,     NULL,      NULL,      NULL,        NULL,         NULL,          NULL,          NULL,            NULL,                NULL}                 // 15 zeros (ZRL)
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

// ! até aqui tudo certo. Novas funçoes a abaixo

// Pega os 63 valores do vetor pós zigzag e forma os pares RLE
int codificar_ac_rle(int zig_zag_vetor[64], Par_RLE* pares_saida) {
    int qtd_pares = 0;
    int zero_count = 0;

    for (int i = 1; i < 64; i++) {
        if (zero_count == 16) {
            pares_saida[qtd_pares].zeros = 15;
            pares_saida[qtd_pares].coeficiente = 0; // ZRL
            qtd_pares++;
            zero_count = 0;
        }

        if (zig_zag_vetor[i] == 0) {
            zero_count++;
        } else {
            pares_saida[qtd_pares].zeros = zero_count;
            pares_saida[qtd_pares].coeficiente = zig_zag_vetor[i];
            qtd_pares++;
            zero_count = 0;
        }
    }

    // Adiciona o marcador de Fim de Bloco (EOB)
    pares_saida[qtd_pares].zeros = 0;
    pares_saida[qtd_pares].coeficiente = 0;
    qtd_pares++;

    return qtd_pares;
}

BLOCO_CODIFICADO codificar_bloco_entropia(double bloco_quantizado[8][8], int dc_anterior) {
    BLOCO_CODIFICADO resultado; // Cria a struct que será retornada

    // 1. Aplica o Zig-Zag
    int* zig_zag_vetor = aplicar_zigzag(bloco_quantizado);

    // 2. Calcula a diferença do DC
    int dc_atual = zig_zag_vetor[0];
    resultado.DC_dif = dc_atual - dc_anterior;

    // 3. Chama a nova função de RLE para preencher o vetor de pares AC
    // A função retorna a quantidade de pares, que armazenamos em 'qtd_pares'
    resultado.qtd_pares = codificar_ac_rle(zig_zag_vetor, resultado.vetor_par_rle);
    
    // 4. Libera a memória
    free(zig_zag_vetor);

    // Retorna a struct completamente preenchida
    return resultado;
}

// Implementacao da tabela 2 em codigo
int get_category(int coeficiente) 
{
    // A categoria depende do valor absoluto do coeficiente
    int abs_val = abs(coeficiente);

    if (abs_val == 0) return 0; // Faixa de valores: 0
    if (abs_val <= 1) return 1; // Faixa de valores: -1, 1
    if (abs_val <= 3) return 2; // Faixa de valores: -3, -2, 2, 3
    if (abs_val <= 7) return 3; // Faixa de valores: -7,...,-4, 4, ..., 7
    if (abs_val <= 15) return 4; // Faixa de valores: -15, ..., -8, 8, ..., 15
    if (abs_val <= 31) return 5; // Faixa de valores: -31, ..., -16, 16, ..., 31
    if (abs_val <= 63) return 6; // Faixa de valores: -63, ..., -32, 32, ..., 63
    if (abs_val <= 127) return 7; // Faixa de valores: -127, ..., -64, 64, ..., 127
    if (abs_val <= 255) return 8; // Faixa de valores: -255, ..., -128, 128, ..., 255
    if (abs_val <= 511) return 9; // Faixa de valores: -511, ..., -256, 256, ..., 511
    if (abs_val <= 1023) return 10; // Categoria 'A' -> Faixa de valores: -1023, ..., -512, 512, ..., 1023
    if (abs_val <= 2047) return 11; // Categoria 'B' -> Faixa de valores: -2047, ..., -1024, 1024, ..., 2047

    // Se o número for maior que o suportado, retorna um erro ou um valor inválido.
    // Para este trabalho, os valores não devem exceder 2047.
    return -1; // Valor de erro
}

// Função para escrever o meu numero. Le o coeficiente que é o numero, verifica a categoria que ele pertence e salva no mantissa_str
// Função para gerar a string da mantissa
void get_mantissa(int coeficiente, int categoria, char* mantissa_str) 
{
    if (categoria == 0) {
        mantissa_str[0] = '\0'; // Mantissa de 0 bits
        return;
    }

    int valor_base = (coeficiente > 0) ? coeficiente : abs(coeficiente);
    
    // Converte o valor base para uma string binária
    for (int i = categoria - 1; i >= 0; i--) {
        mantissa_str[i] = (valor_base % 2) + '0';
        valor_base /= 2;
    }
    mantissa_str[categoria] = '\0';

    // Se o coeficiente for negativo, inverte os bits
    if (coeficiente < 0) {
        for (int i = 0; i < categoria; i++) {
            mantissa_str[i] = (mantissa_str[i] == '0') ? '1' : '0';
        }
    }
}

void write_bits(BitWriter* writer, const char* bit_string) {
    for (int i = 0; i < strlen(bit_string); i++) {
        // Desloca o buffer para a esquerda para abrir espaço
        writer->byte_buffer <<= 1;
        // Adiciona o novo bit (0 ou 1)
        if (bit_string[i] == '1') {
            writer->byte_buffer |= 1;
        }
        writer->bit_count++;

        // Se o buffer estiver cheio, escreva-o no arquivo
        if (writer->bit_count == 8) {
            fwrite(&writer->byte_buffer, 1, 1, writer->output_file);
            writer->bit_count = 0;
            writer->byte_buffer = 0;
        }
    }
}

// Função para escrever os bits restantes no final do processo
void flush_bits(BitWriter* writer) {
    if (writer->bit_count > 0) {
        // Preenche os bits restantes com 1's (padrão JPEG)
        writer->byte_buffer <<= (8 - writer->bit_count);
        writer->byte_buffer |= ((1 << (8 - writer->bit_count)) - 1);
        fwrite(&writer->byte_buffer, 1, 1, writer->output_file);
        writer->bit_count = 0;
    }
}

/**
 * @brief Executa todo o processo de codificação entrópica para uma imagem e escreve o bitstream final.
 * Processa os componentes Y, Cb e Cr, gerando o arquivo binário comprimido.
 * @param img_quantizada A estrutura YCbCrImg com os dados após a quantização.
 * @param nome_arquivo_saida O nome do arquivo a ser criado (ex: "imagem.minhajpeg").
 */
void executar_compressao_entropica(YCbCrImg img_quantizada, const char* nome_arquivo_saida) {
    
    FILE* arquivo = fopen(nome_arquivo_saida, "wb");
    if (!arquivo) {
        printf("Erro critico: Nao foi possivel criar o arquivo de saida '%s'.\n", nome_arquivo_saida);
        return;
    }

    // 1. Inicializa o escritor de bits
    BitWriter writer = {0, 0, arquivo};
    
    // Variáveis de controle para a codificação diferencial de cada componente
    int dc_anterior_Y = 0;
    int dc_anterior_Cb = 0;
    int dc_anterior_Cr = 0;

    double bloco_atual[8][8];
    char mantissa_buffer[17]; // Buffer para a string da mantissa (max 16 bits + \0)

    // ======================================================================
    // === PASSO 1: Processa o Canal de Luminância (Y)
    // ======================================================================
    for (int i_bloco = 0; i_bloco < img_quantizada.height; i_bloco += 8) {
        for (int j_bloco = 0; j_bloco < img_quantizada.width; j_bloco += 8) {
            
            // Copia o bloco 8x8 atual do canal Y
            for (int i = 0; i < 8; i++) for (int j = 0; j < 8; j++)
                bloco_atual[i][j] = img_quantizada.Y[i_bloco + i][j_bloco + j];

            // Codifica o bloco para obter DC_dif e os pares RLE para os ACs
            BLOCO_CODIFICADO bloco_codificado = codificar_bloco_entropia(bloco_atual, dc_anterior_Y);
            dc_anterior_Y += bloco_codificado.DC_dif; // ATENÇÃO: Atualiza o DC anterior para a PRÓXIMA iteração

            // --- Escreve os bits do bloco no arquivo ---

            // A. Codifica o coeficiente DC
            int dc_cat = get_category(bloco_codificado.DC_dif);
            const char* dc_prefixo = huffman_dc_lum_codes[dc_cat];
            get_mantissa(bloco_codificado.DC_dif, dc_cat, mantissa_buffer);
            write_bits(&writer, dc_prefixo);
            write_bits(&writer, mantissa_buffer);
            
            // B. Codifica os coeficientes AC
            for(int i = 0; i < bloco_codificado.qtd_pares; i++) { // Renomeei 'qtd_trinca' para 'qtd_pares' para consistência
                Par_RLE par = bloco_codificado.vetor_par_rle[i];

                // --- Lógica para tratar os símbolos especiais ---
                if (par.zeros == 0 && par.coeficiente == 0) { // Símbolo EOB
                    write_bits(&writer, huffman_ac_lum_codes[0][0]);
                    break; 
                } 
                else if (par.zeros == 15 && par.coeficiente == 0) { // Símbolo ZRL
                    write_bits(&writer, huffman_ac_lum_codes[15][0]);
                } 
                else { // Par RLE normal
                    int ac_cat = get_category(par.coeficiente);
                    const char* ac_prefixo = huffman_ac_lum_codes[par.zeros][ac_cat];
                    get_mantissa(par.coeficiente, ac_cat, mantissa_buffer);
                    write_bits(&writer, ac_prefixo);
                    write_bits(&writer, mantissa_buffer);
                }
            }
        }
    }
    
    // ======================================================================
    // === PASSO 2: Processa o Canal de Crominância (Cb)
    // ======================================================================
    int altura_chroma = img_quantizada.height / 2;
    int largura_chroma = img_quantizada.width / 2;

    for (int i_bloco = 0; i_bloco < altura_chroma; i_bloco += 8) {
        for (int j_bloco = 0; j_bloco < largura_chroma; j_bloco += 8) {
            for (int i = 0; i < 8; i++) for (int j = 0; j < 8; j++)
                bloco_atual[i][j] = img_quantizada.Cb[i_bloco + i][j_bloco + j];
            
            BLOCO_CODIFICADO bloco_codificado = codificar_bloco_entropia(bloco_atual, dc_anterior_Cb);
            dc_anterior_Cb += bloco_codificado.DC_dif;

            int dc_cat = get_category(bloco_codificado.DC_dif);
            write_bits(&writer, huffman_dc_lum_codes[dc_cat]); // Usando tabela de Luma
            get_mantissa(bloco_codificado.DC_dif, dc_cat, mantissa_buffer);
            write_bits(&writer, mantissa_buffer);

            for(int i = 0; i < bloco_codificado.qtd_pares; i++) {
                Par_RLE par = bloco_codificado.vetor_par_rle[i];
                if (par.zeros == 0 && par.coeficiente == 0) {
                    write_bits(&writer, huffman_ac_lum_codes[0][0]); break;
                } else if (par.zeros == 15 && par.coeficiente == 0) {
                    write_bits(&writer, huffman_ac_lum_codes[15][0]);
                } else {
                    int ac_cat = get_category(par.coeficiente);
                    write_bits(&writer, huffman_ac_lum_codes[par.zeros][ac_cat]); // Usando tabela de Luma
                    get_mantissa(par.coeficiente, ac_cat, mantissa_buffer);
                    write_bits(&writer, mantissa_buffer);
                }
            }
        }
    }

    // ======================================================================
    // === PASSO 3: Processa o Canal de Crominância (Cr)
    // ======================================================================
    for (int i_bloco = 0; i_bloco < altura_chroma; i_bloco += 8) {
        for (int j_bloco = 0; j_bloco < largura_chroma; j_bloco += 8) {
            for (int i = 0; i < 8; i++) for (int j = 0; j < 8; j++)
                bloco_atual[i][j] = img_quantizada.Cr[i_bloco + i][j_bloco + j];

            BLOCO_CODIFICADO bloco_codificado = codificar_bloco_entropia(bloco_atual, dc_anterior_Cr);
            dc_anterior_Cr += bloco_codificado.DC_dif;
            
            int dc_cat = get_category(bloco_codificado.DC_dif);
            write_bits(&writer, huffman_dc_lum_codes[dc_cat]); // Usando tabela de Luma
            get_mantissa(bloco_codificado.DC_dif, dc_cat, mantissa_buffer);
            write_bits(&writer, mantissa_buffer);

            for(int i = 0; i < bloco_codificado.qtd_pares; i++) {
                Par_RLE par = bloco_codificado.vetor_par_rle[i];
                if (par.zeros == 0 && par.coeficiente == 0) {
                    write_bits(&writer, huffman_ac_lum_codes[0][0]); break;
                } else if (par.zeros == 15 && par.coeficiente == 0) {
                    write_bits(&writer, huffman_ac_lum_codes[15][0]);
                } else {
                    int ac_cat = get_category(par.coeficiente);
                    write_bits(&writer, huffman_ac_lum_codes[par.zeros][ac_cat]); // Usando tabela de Luma
                    get_mantissa(par.coeficiente, ac_cat, mantissa_buffer);
                    write_bits(&writer, mantissa_buffer);
                }
            }
        }
    }

    // 2. Finaliza a escrita, descarregando quaisquer bits restantes no buffer
    flush_bits(&writer);

    fclose(arquivo);
    printf("Imagem comprimida e salva com sucesso em '%s'\n", nome_arquivo_saida);
}


// ! -----------------------------------------
