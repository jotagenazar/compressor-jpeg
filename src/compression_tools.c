/************************************
* INCLUDES
************************************/

#include "../include/compression_tools.h"
#include <stdio.h>
#include "string.h"


/************************************
* EXTERN VARIABLES
************************************/

// Matriz para fazer a Quantizacao da matriz Y
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

// Matriz para fazer a quantizacao das matrizes Cb e Cr
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

// Matriz para acelerar o preenchimento do vetor na sequencia zigzag
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

// Tabela 3 - Prefixos para Coeficientes DC 
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
    "111111110" // Categoria B (11)
};

// Tabela 4 - Prefixos para o Coeficiente AC
/**
 * Tabela de códigos Huffman para os coeficientes AC de Luminância.
 * Transcrita da Tabela 4 do "Guia JPEG.pdf".
 *
 * Acesso: huffman_ac_lum_codes[run][size]
 * - run: Número de zeros precedentes (0-15)
 * - size: Categoria do coeficiente (0-10)
 *
 * Códigos especiais:
 * - EOB (End of Block) {0,0}: huffman_ac_lum_codes[0][0] -> "1010"
 * - ZRL (16 zeros)   {15,0}: huffman_ac_lum_codes[15][0] -> "11111111001"
 */
const char* huffman_ac_lum_codes[16][11] = {
    // Categoria (size) ->
    // 0(ESP)         1           2           3            4             5              6               7                8                 9                  10
    { "1010",       "00",       "01",       "100",       "1011",      "11010",       "111000",       "1111000",     "111110110",     "11111110110",   "1111111110000011" }, // 0 zeros
    { NULL,         "1100",     "11011",    "1111001",   "11111000",  "1111110011",  "111111110101", "1111111110000100", "1111111110000101", "1111111110000110", NULL               }, // 1 zero
    { NULL,         "111010",   "11111010", "1111110111", "111111110110", "1111111110000111", "1111111110001000", "1111111110001001", NULL,               NULL,                NULL               }, // 2 zeros
    { NULL,         "111011",   "11111011", "11111111000", "1111111110001010", "1111111110001011", "1111111110001100", NULL,               NULL,               NULL,                NULL               }, // 3 zeros
    { NULL,         "1111010",  "111111010", "111111110111", "1111111110001101", "1111111110001110", NULL,               NULL,               NULL,               NULL,                NULL               }, // 4 zeros
    { NULL,         "1111011",  "1111111010", "1111111110010000", "1111111110010001", NULL,               NULL,               NULL,               NULL,               NULL,                NULL               }, // 5 zeros
    { NULL,         "11111001", "1111111011", "1111111110010010", NULL,               NULL,               NULL,               NULL,               NULL,               NULL,                NULL               }, // 6 zeros
    { NULL,         "11111011", "111111110100", "1111111110010011", NULL,               NULL,               NULL,               NULL,               NULL,               NULL,                NULL               }, // 7 zeros
    { NULL,         "111111000", "1111111110010100", NULL,               NULL,               NULL,               NULL,               NULL,               NULL,               NULL,                NULL               }, // 8 zeros
    { NULL,         "1111110010", "1111111110010101", NULL,               NULL,               NULL,               NULL,               NULL,               NULL,               NULL,                NULL               }, // 9 zeros
    { NULL,         "1111110100", "1111111110010110", NULL,               NULL,               NULL,               NULL,               NULL,               NULL,               NULL,                NULL               }, // 10 zeros
    { NULL,         "1111110101", "1111111110010111", NULL,               NULL,               NULL,               NULL,               NULL,               NULL,               NULL,                NULL               }, // 11 zeros
    { NULL,         "1111110110", "1111111110011000", NULL,               NULL,               NULL,               NULL,               NULL,               NULL,               NULL,                NULL               }, // 12 zeros
    { NULL,         "1111111000", "1111111110011001", NULL,               NULL,               NULL,               NULL,               NULL,               NULL,               NULL,                NULL               }, // 13 zeros
    { NULL,         "1111111001", "1111111110011010", NULL,               NULL,               NULL,               NULL,               NULL,               NULL,               NULL,                NULL               }, // 14 zeros
    { "11111111001",  NULL,       NULL,        NULL,        NULL,          NULL,          NULL,            NULL,            NULL,              NULL,                NULL               }  // 15 zeros (ZRL)
};


/************************************
* STATIC FUNCTION PROTOTYPES
************************************/

// Função que aplica a função de quantização de um bloco B usando a matriz Q fornecida e o parametro k
void _quantizar_bloco(double bloco[8][8], const int Q[8][8], double k);

// Função que aplica a inversa da quantizacao
void _desquantizar_bloco(double bloco[8][8], const int Q[8][8], double k);

// Função que aplica a quantizacao em todos os blocos da matriz passada como parâmetro e retorna a nova matriz alocada com os
// coeficientes calculados em cada bloco
double** _quantizar_matriz(double** matriz, int altura, int largura, const int Q[8][8], double k);

// Função que aplica a inversa da quantizacao em todos os coeficientes nos blocos da matriz passada como parâmetro e retorna 
// a nova matriz alocada com os blocos originais em cada posição
double** _desquantizar_matriz(double** matriz, int altura, int largura, const int Q[8][8], double k);

// Percorre um bloco fazendo um zigzag e retorna um vetor com os valores do bloco seguindo a ordem percorrida 
int* _aplicar_zigzag(double bloco[8][8]);

// Recebe um vetor de inteiros e forma os pares RLE e retorna a quantidade de pares formados
int _codificar_ac_rle(int zig_zag_vetor[64], Par_RLE* pares_saida);

// Recebe um bloco e calcula a diferenca entre o DC do bloco atual e anterior e calcula os pares RLE para os 63 valores AC
BLOCO_CODIFICADO _codificar_bloco_entropia(double bloco_quantizado[8][8], int dc_anterior);

// Implementacao da tabela 2 em codigo
// Recebe o coeficiente DC ou AC e retorna a categoria a qual pertence
int _get_category(int coeficiente);

// Função para escrever o meu numero. Le o coeficiente que é o numero, verifica a categoria que ele pertence e salva no mantissa_str
// Função para gerar a string da mantissa
void _get_mantissa(int coeficiente, int categoria, char* mantissa_str); 

// Funcao que escreve um byte no arquivo quando temos 8 bits
void _write_bits(BitWriter* writer, const char* bit_string); 

// Função para escrever os bits restantes no final do processo
void _flush_bits(BitWriter* writer); 

// Inicializa o leitor de bits do arquivo
void _init_bit_reader(BitReader *reader, FILE *file);

// Lê um bit do arquivo e armazena na struct
int _read_bit(BitReader *reader);

// Lê multiplos bits de um arquivo e armazena na struct
int _read_bits(BitReader *reader, int n, int *value_out);

int _decode_huffman_dc(BitReader *reader);
void _inverter_zigzag(int vetor[64], double bloco[8][8]);
int _read_mantissa(BitReader *reader, int categoria);

/************************************
* STATIC FUNCTIONS
************************************/

void _quantizar_bloco(double bloco[8][8], const int Q[8][8], double k)
{
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            bloco[i][j] = round(bloco[i][j] / (k * Q[i][j]));
        }
    }
}

void _desquantizar_bloco(double bloco[8][8], const int Q[8][8], double k)
{
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            bloco[i][j] = bloco[i][j] * Q[i][j] * k;
        }
    }
}

double** _quantizar_matriz(double** matriz, int altura, int largura, const int Q[8][8], double k)
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
            _quantizar_bloco(bloco, Q, k);

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

double** _desquantizar_matriz(double** matriz, int altura, int largura, const int Q[8][8], double k)
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
            _desquantizar_bloco(bloco, Q, k);

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

int* _aplicar_zigzag(double bloco[8][8])
{
    int* vetor = malloc(64 * sizeof(int));

    for (int i = 0; i < 64; i++) {
        int x = zigzag[i][0];
        int y = zigzag[i][1];
        vetor[i] = (int)round(bloco[x][y]);  // pós-quantização, valores já são inteiros
    }

    return vetor;
}


int _codificar_ac_rle(int zig_zag_vetor[64], Par_RLE* pares_saida) 
{
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

BLOCO_CODIFICADO _codificar_bloco_entropia(double bloco_quantizado[8][8], int dc_anterior)
{
    BLOCO_CODIFICADO resultado; // Cria a struct que será retornada

    // 1. Aplica o Zig-Zag
    int* zig_zag_vetor = _aplicar_zigzag(bloco_quantizado);

    // 2. Calcula a diferença do DC
    int dc_atual = zig_zag_vetor[0];
    resultado.DC_dif = dc_atual - dc_anterior;

    // 3. Chama a nova função de RLE para preencher o vetor de pares AC
    // A função retorna a quantidade de pares, que armazenamos em 'qtd_pares'
    resultado.qtd_pares = _codificar_ac_rle(zig_zag_vetor, resultado.vetor_par_rle);
    
    // 4. Libera a memória
    free(zig_zag_vetor);

    // Retorna a struct completamente preenchida
    return resultado;
}

int _get_category(int coeficiente) 
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

    // Problema que enfrentamos:
    // Tabela 2 apresenta categoria B para DC, tabela 3 não possui. Criamos categoria B na tabela DC
    // seguindo o padrão estabelecido e contornamos a inconsistência

    // Se o número for maior que o suportado, retorna um erro ou um valor inválido.
    // Para este trabalho, os valores não devem exceder 2047.
    return -1; // Valor de erro
}


void _get_mantissa(int coeficiente, int categoria, char* mantissa_str) 
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

void _write_bits(BitWriter* writer, const char* bit_string) 
{
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

void _flush_bits(BitWriter* writer) 
{
    if (writer->bit_count > 0) {
        // Preenche os bits restantes com 1's (padrão JPEG)
        writer->byte_buffer <<= (8 - writer->bit_count);
        writer->byte_buffer |= ((1 << (8 - writer->bit_count)) - 1);
        fwrite(&writer->byte_buffer, 1, 1, writer->output_file);
        writer->bit_count = 0;
    }
}

void _init_bit_reader(BitReader *reader, FILE *file) {
    reader->input_file = file;
    reader->bit_count = 0;
    reader->byte_buffer = 0;
}

int _read_bit(BitReader *reader) {
    if (reader->bit_count == 0) {
        size_t lidos = fread(&reader->byte_buffer, 1, 1, reader->input_file);
        if (lidos != 1) return -1; // EOF ou erro
        reader->bit_count = 8;
    }

    int bit = (reader->byte_buffer >> 7) & 1;
    reader->byte_buffer <<= 1;
    reader->bit_count--;
    return bit;
}

int _read_bits(BitReader *reader, int n, int *value_out) {
    *value_out = 0;
    for (int i = 0; i < n; i++) {
        int bit = _read_bit(reader);
        if (bit == -1) return -1; // EOF ou erro
        *value_out = (*value_out << 1) | bit;
    }
    return 0;
}

int _decode_huffman_dc(BitReader *reader) {
    char code[17] = {0};
    int bit_val;
    
    for (int i = 0; i < 16; i++) {
        bit_val = _read_bit(reader);
        if (bit_val == -1) return -1; // Fim do arquivo
        code[i] = bit_val + '0';
        code[i+1] = '\0';
        
        // OBS: Tabela DC de luminância usada para tudo (simplificação do projeto)
        for (int cat = 0; cat < 12; cat++) {
            if (huffman_dc_lum_codes[cat] && strcmp(code, huffman_dc_lum_codes[cat]) == 0) {
                return cat; // Retorna a categoria encontrada
            }
        }
    }
    return -1; // Código não encontrado
}

int _read_mantissa(BitReader *reader, int categoria) {
    if (categoria == 0) return 0;

    int valor;
    _read_bits(reader, categoria, &valor);

    // Verifica se o bit mais significativo é 0 → valor negativo (mantissa invertida)
    if ((valor >> (categoria - 1)) == 0) {
        valor -= (1 << categoria) - 1;
    }

    return valor;
}

int _decode_huffman_ac(BitReader *reader, int *run, int *category) {
    char code[17] = {0};
    int bit_val;

    for (int i = 0; i < 16; i++) {
        bit_val = _read_bit(reader);
        if (bit_val == -1) return -1;
        code[i] = bit_val + '0';
        code[i+1] = '\0';

        // OBS: Tabela AC de luminância usada para tudo (simplificação do projeto)
        for (int r = 0; r < 16; r++) { // run
            for (int s = 0; s < 11; s++) { // size/category
                if (huffman_ac_lum_codes[r][s] && strcmp(code, huffman_ac_lum_codes[r][s]) == 0) {
                    *run = r;
                    *category = s;
                    return 0; // Sucesso
                }
            }
        }
    }
    return -1; // Código não encontrado
}

void _inverter_zigzag(int vetor[64], double bloco[8][8]) {
    for (int i = 0; i < 64; i++) {
        int x = zigzag[i][0];
        int y = zigzag[i][1];
        bloco[x][y] = vetor[i];
    }
}

void _decodificar_bloco(BitReader *reader, int *dc_anterior, double bloco_saida[8][8]) {
    int zig_zag_vetor[64] = {0};

    // 1. Decodificar Coeficiente DC
    int dc_cat = _decode_huffman_dc(reader);
    if (dc_cat == -1) return; // Erro ou fim de arquivo

    int dc_diff = _read_mantissa(reader, dc_cat);
    int dc_atual = *dc_anterior + dc_diff;
    zig_zag_vetor[0] = dc_atual;
    *dc_anterior = dc_atual;

    // 2. Decodificar 63 Coeficientes AC
    int i = 1;
    while (i < 64) {
        int run, category;
        if (_decode_huffman_ac(reader, &run, &category) != 0) {
            break; // Erro ou fim do arquivo
        }

        if (run == 0 && category == 0) { // EOB (End of Block)
            break; // Resto do vetor já é 0, então podemos parar
        }
        
        if (run == 15 && category == 0) { // ZRL (Zero Run Length)
            i += 16; // Pula 16 zeros
            continue;
        }

        // Adiciona os zeros precedentes
        i += run;

        if (i < 64) {
            int ac_val = _read_mantissa(reader, category);
            zig_zag_vetor[i] = ac_val;
            i++;
        }
    }
    
    // 3. Inverter o Zig-Zag para reconstruir o bloco 8x8
    _inverter_zigzag(zig_zag_vetor, bloco_saida);
}

/************************************
* GLOBAL FUNCTIONS
************************************/

YCbCrImg quantizar_imagem(YCbCrImg img_dct, double k)
{
    YCbCrImg quantizado;

    quantizado.height = img_dct.height;
    quantizado.width = img_dct.width;

    // Canal Y usa matriz Q_Y
    quantizado.Y = _quantizar_matriz(img_dct.Y, img_dct.height, img_dct.width, Q_Y, k);

    // Cb e Cr usam matriz Q_C (tamanhos reduzidos)
    quantizado.Cb = _quantizar_matriz(img_dct.Cb, img_dct.height / 2, img_dct.width / 2, Q_C, k);
    quantizado.Cr = _quantizar_matriz(img_dct.Cr, img_dct.height / 2, img_dct.width / 2, Q_C, k);

    return quantizado;
}


YCbCrImg desquantizar_imagem(YCbCrImg quantizado, double k)
{
    YCbCrImg desquantizado;

    desquantizado.height = quantizado.height;
    desquantizado.width = quantizado.width;

    // Canal Y usa matriz Q_Y
    desquantizado.Y = _desquantizar_matriz(quantizado.Y, quantizado.height, quantizado.width, Q_Y, k);

    // Cb e Cr usam matriz Q_C (tamanhos reduzidos)
    desquantizado.Cb = _desquantizar_matriz(quantizado.Cb, quantizado.height / 2, quantizado.width / 2, Q_C, k);
    desquantizado.Cr = _desquantizar_matriz(quantizado.Cr, quantizado.height / 2, quantizado.width / 2, Q_C, k);

    return desquantizado;
}

/**
 * @brief Executa todo o processo de codificação entrópica para uma imagem e escreve o bitstream final.
 * Processa os componentes Y, Cb e Cr, gerando o arquivo binário comprimido.
 * @param img_quantizada A estrutura YCbCrImg com os dados após a quantização.
 * @param nome_arquivo_saida O nome do arquivo a ser criado (ex: "imagem.minhajpeg").
 */
void executar_codificacao_entropica(YCbCrImg img_quantizada, FILE *arquivo, double k) 
{
    // 1. Inicializa o escritor de bits
    BitWriter writer = {0, 0, arquivo};
    
    // Variáveis de controle para a codificação diferencial de cada componente
    int dc_anterior_Y = 0;
    int dc_anterior_Cb = 0;
    int dc_anterior_Cr = 0;

    double bloco_atual[8][8];
    char mantissa_buffer[17]; // Buffer para a string da mantissa (max 16 bits + \0)

    int divisor = DOWNSAMPLE_DIVISOR; // variavel que indica o divisor das matrizes Cb e Cr caso tenha ocorrido compressao jpeg e consequente downsampling
    if(k == 0) divisor = 1;

    // ======================================================================
    // === PASSO 1: Processa o Canal de Luminância (Y)
    // ======================================================================
    for (int i_bloco = 0; i_bloco < img_quantizada.height; i_bloco += 8) {
        for (int j_bloco = 0; j_bloco < img_quantizada.width; j_bloco += 8) {
            
            // Copia o bloco 8x8 atual do canal Y
            for (int i = 0; i < 8; i++) for (int j = 0; j < 8; j++)
                bloco_atual[i][j] = img_quantizada.Y[i_bloco + i][j_bloco + j];

            // Codifica o bloco para obter DC_dif e os pares RLE para os ACs
            BLOCO_CODIFICADO bloco_codificado = _codificar_bloco_entropia(bloco_atual, dc_anterior_Y);
            dc_anterior_Y += bloco_codificado.DC_dif; // ATENÇÃO: Atualiza o DC anterior para a PRÓXIMA iteração

            // --- Escreve os bits do bloco no arquivo ---

            // A. Codifica o coeficiente DC
            int dc_cat = _get_category(bloco_codificado.DC_dif);

            printf("DC_dif: %d -> dc_cat: %d\n", bloco_codificado.DC_dif, dc_cat);
            if (dc_cat < 0 || dc_cat > 11) {
                printf("ERRO: Categoria DC inválida!\n");
                exit(1); // Força a saída para vermos o erro
            }

            const char* dc_prefixo = huffman_dc_lum_codes[dc_cat];
            _get_mantissa(bloco_codificado.DC_dif, dc_cat, mantissa_buffer);
            _write_bits(&writer, dc_prefixo);
            _write_bits(&writer, mantissa_buffer);
            
            // B. Codifica os coeficientes AC
            for(int i = 0; i < bloco_codificado.qtd_pares; i++) { // Renomeei 'qtd_trinca' para 'qtd_pares' para consistência
                Par_RLE par = bloco_codificado.vetor_par_rle[i];

                // --- Lógica para tratar os símbolos especiais ---
                if (par.zeros == 0 && par.coeficiente == 0) { // Símbolo EOB
                    _write_bits(&writer, huffman_ac_lum_codes[0][0]);
                    break; 
                } 
                else if (par.zeros == 15 && par.coeficiente == 0) { // Símbolo ZRL
                    _write_bits(&writer, huffman_ac_lum_codes[15][0]);
                } 
                else { // Par RLE normal
                    int ac_cat = _get_category(par.coeficiente);

                    printf("  AC Coef: %d, Zeros: %d -> ac_cat: %d\n", par.coeficiente, par.zeros, ac_cat);
                    if (ac_cat < 0 || ac_cat > 10) {
                         printf("ERRO: Categoria AC inválida!\n");
                         exit(1);
                    }
                    if (par.zeros < 0 || par.zeros > 15) {
                        printf("ERRO: Quantidade de zeros AC inválida!\n");
                        exit(1);
                    }
                    
                    const char* ac_prefixo = huffman_ac_lum_codes[par.zeros][ac_cat];
                    _get_mantissa(par.coeficiente, ac_cat, mantissa_buffer);
                    _write_bits(&writer, ac_prefixo);
                    _write_bits(&writer, mantissa_buffer);
                }
            }
        }
    }
    
    // ======================================================================
    // === PASSO 2: Processa o Canal de Crominância (Cb)
    // ======================================================================
    int altura_chroma = img_quantizada.height / divisor;
    int largura_chroma = img_quantizada.width / divisor;

    for (int i_bloco = 0; i_bloco < altura_chroma; i_bloco += 8) {
        for (int j_bloco = 0; j_bloco < largura_chroma; j_bloco += 8) {
            for (int i = 0; i < 8; i++) for (int j = 0; j < 8; j++)
                bloco_atual[i][j] = img_quantizada.Cb[i_bloco + i][j_bloco + j];
            
            BLOCO_CODIFICADO bloco_codificado = _codificar_bloco_entropia(bloco_atual, dc_anterior_Cb);
            dc_anterior_Cb += bloco_codificado.DC_dif;

            int dc_cat = _get_category(bloco_codificado.DC_dif);
            _write_bits(&writer, huffman_dc_lum_codes[dc_cat]); // Usando tabela de Luma
            _get_mantissa(bloco_codificado.DC_dif, dc_cat, mantissa_buffer);
            _write_bits(&writer, mantissa_buffer);

            for(int i = 0; i < bloco_codificado.qtd_pares; i++) {
                Par_RLE par = bloco_codificado.vetor_par_rle[i];
                if (par.zeros == 0 && par.coeficiente == 0) {
                    _write_bits(&writer, huffman_ac_lum_codes[0][0]); break;
                } else if (par.zeros == 15 && par.coeficiente == 0) {
                    _write_bits(&writer, huffman_ac_lum_codes[15][0]);
                } else {
                    int ac_cat = _get_category(par.coeficiente);
                    _write_bits(&writer, huffman_ac_lum_codes[par.zeros][ac_cat]); // Usando tabela de Luma
                    _get_mantissa(par.coeficiente, ac_cat, mantissa_buffer);
                    _write_bits(&writer, mantissa_buffer);
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

            BLOCO_CODIFICADO bloco_codificado = _codificar_bloco_entropia(bloco_atual, dc_anterior_Cr);
            dc_anterior_Cr += bloco_codificado.DC_dif;
            
            int dc_cat = _get_category(bloco_codificado.DC_dif);
            _write_bits(&writer, huffman_dc_lum_codes[dc_cat]); // Usando tabela de Luma
            _get_mantissa(bloco_codificado.DC_dif, dc_cat, mantissa_buffer);
            _write_bits(&writer, mantissa_buffer);

            for(int i = 0; i < bloco_codificado.qtd_pares; i++) {
                Par_RLE par = bloco_codificado.vetor_par_rle[i];
                if (par.zeros == 0 && par.coeficiente == 0) {
                    _write_bits(&writer, huffman_ac_lum_codes[0][0]); break;
                } else if (par.zeros == 15 && par.coeficiente == 0) {
                    _write_bits(&writer, huffman_ac_lum_codes[15][0]);
                } else {
                    int ac_cat = _get_category(par.coeficiente);
                    _write_bits(&writer, huffman_ac_lum_codes[par.zeros][ac_cat]); // Usando tabela de Luma
                    _get_mantissa(par.coeficiente, ac_cat, mantissa_buffer);
                    _write_bits(&writer, mantissa_buffer);
                }
            }
        }
    }

    // 2. Finaliza a escrita, descarregando quaisquer bits restantes no buffer
    _flush_bits(&writer);

}

void executar_decodificacao_entropica(YCbCrImg img, FILE *arquivo, double k) {
    BitReader reader;
    _init_bit_reader(&reader, arquivo);

    int divisor = (k == 0) ? 1 : DOWNSAMPLE_DIVISOR;

    int dc_anterior_Y = 0;
    int dc_anterior_Cb = 0;
    int dc_anterior_Cr = 0;

    double bloco_decodificado[8][8];

    // === Canal Y ===
    for (int i_bloco = 0; i_bloco < img.height; i_bloco += 8) {
        for (int j_bloco = 0; j_bloco < img.width; j_bloco += 8) {
            _decodificar_bloco(&reader, &dc_anterior_Y, bloco_decodificado);
            
            // Copia o bloco decodificado para a matriz Y da imagem
            for (int i = 0; i < 8; i++) {
                for (int j = 0; j < 8; j++) {
                    img.Y[i_bloco + i][j_bloco + j] = bloco_decodificado[i][j];
                }
            }
        }
    }

    // === Canal Cb ===
    int altura_chroma = img.height / divisor;
    int largura_chroma = img.width / divisor;

    for (int i_bloco = 0; i_bloco < altura_chroma; i_bloco += 8) {
        for (int j_bloco = 0; j_bloco < largura_chroma; j_bloco += 8) {
            _decodificar_bloco(&reader, &dc_anterior_Cb, bloco_decodificado);

            // Copia o bloco decodificado para a matriz Cb da imagem
            for (int i = 0; i < 8; i++) {
                for (int j = 0; j < 8; j++) {
                    img.Cb[i_bloco + i][j_bloco + j] = bloco_decodificado[i][j];
                }
            }
        }
    }

    // === Canal Cr ===
    for (int i_bloco = 0; i_bloco < altura_chroma; i_bloco += 8) {
        for (int j_bloco = 0; j_bloco < largura_chroma; j_bloco += 8) {
            _decodificar_bloco(&reader, &dc_anterior_Cr, bloco_decodificado);

            // Copia o bloco decodificado para a matriz Cr da imagem
            for (int i = 0; i < 8; i++) {
                for (int j = 0; j < 8; j++) {
                    img.Cr[i_bloco + i][j_bloco + j] = bloco_decodificado[i][j];
                }
            }
        }
    }
}

