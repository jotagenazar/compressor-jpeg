/*
********************************************************************************
SCC0261 – Multimídia - 2025.1  
Prof. Rudinei Goularte  
Projeto: Compressão de Imagens

Alunos:  
João Gabriel Manfré Nazar
João Pedro Mori Machado
********************************************************************************
*/

#include <stdio.h>
#include <stdlib.h>

#include "../include/file_handler.h"
#include "../include/YCbCr_handler.h"

void RBG2YCbCr(unsigned char **R, unsigned char **G, unsigned char **B, 
    unsigned char **Y, unsigned char **Cb, unsigned char **Cr, int n_linhas, int n_colunas){

        for (int i=0; i < n_linhas; i++){
            for (int j=0; j < n_colunas; j++){
                Y[i][j] = (0.299 * R[i][j]) + (0.587 * G[i][j]) + (0.144 * B[i][j]);
            }
        }

        for (int i=0; i < n_linhas; i++){
            for (int j=0; j < n_colunas; j++){
                Cb[i][j] = 0.564 * (B[i][j] - Y[i][j]);
            }
        }

        for (int i=0; i < n_linhas; i++){
            for (int j=0; j < n_colunas; j++){
                Cr[i][j] = 0.713 * (R[i][j] - Y[i][j]);
            }
        }

}

void YCbCr2RGB(unsigned char **R, unsigned char **G, unsigned char **B, 
    unsigned char **Y, unsigned char **Cb, unsigned char **Cr, int n_linhas, int n_colunas){

        for (int i=0; i < n_linhas; i++){
            for (int j=0; j < n_colunas; j++){
                R[i][j] = (Y[i][j] + (1.402 * Cr[i][j]) ) ;
            }
        }

        for (int i=0; i < n_linhas; i++){
            for (int j=0; j < n_colunas; j++){
                G[i][j] = (Y[i][j] - (0.344 * Cb[i][j]) - (0.714 * Cr[i][j]));
            }
        }

        for (int i=0; i < n_linhas; i++){
            for (int j=0; j < n_colunas; j++){
                B[i][j] = (Y[i][j] + (1.772 * Cb[i][j]) ) ;
            }
        }
}



int main() 
{   
    FILE *arquivo;
    char nome_arq_img[100];
    scanf("%s", nome_arq_img);
    arquivo = abrir_arquivo(nome_arq_img, "rb");

    // leitura e importação do header do arquivo e do header da imagem
    BmpFileHeader bmp_file_header = ler_bmp_file_header(arquivo);
    BmpInfoHeader bmp_info_header = ler_bmp_info_header(arquivo);

    printf("file header sizer = %d \n", bmp_file_header.bfSize);
    printf("bmp info header size = %d \n", bmp_info_header.biSize);

    /* ------------------------------------------   */

    unsigned char **R, **G, **B;

    int width = bmp_info_header.biWidth;
    int height = bmp_info_header.biHeight;
    int padding = (4 - (width * 3) % 4) % 4; // Cada linha deve ser múltipla de 4 bytes

    // Alocar memória para as matrizes
    R = malloc(height * sizeof(unsigned char*));
    G = malloc(height * sizeof(unsigned char*));
    B = malloc(height * sizeof(unsigned char*));
    for (int i = 0; i < height; i++) {
        R[i] = malloc(width * sizeof(unsigned char));
        G[i] = malloc(width * sizeof(unsigned char));
        B[i] = malloc(width * sizeof(unsigned char));
    }

    // Posiciona o ponteiro para o início dos dados da imagem
    fseek(arquivo, bmp_file_header.bfOffBits, SEEK_SET);

    // Leitura linha por linha, de baixo para cima (formato BMP)
    for (int i = height - 1; i >= 0; i--) {
        for (int j = 0; j < width; j++) {
            unsigned char b, g, r;
            fread(&b, sizeof(unsigned char), 1, arquivo);
            fread(&g, sizeof(unsigned char), 1, arquivo);
            fread(&r, sizeof(unsigned char), 1, arquivo);
            B[i][j] = b;
            G[i][j] = g;
            R[i][j] = r;
        }
        // Descartar bytes de padding no final da linha
        fseek(arquivo, padding, SEEK_CUR);
    }

    exportar_bmp("imagem_saida.bmp", bmp_file_header, bmp_info_header, R, G, B);
    printf("Imagem salva como imagem_saida.bmp\n");

    return 0;
}