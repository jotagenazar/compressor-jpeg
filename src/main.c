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
    FILE *input_file;
    char input_filename[100];
    scanf("%s", input_filename);

    input_file = abrir_arquivo(input_filename, "rb");

        // leitura e importação do header do arquivo e do header da imagem
        BmpFileHeader bmp_file_header = ler_bmp_file_header(input_file);
        BmpInfoHeader bmp_info_header = ler_bmp_info_header(input_file);

        printf("file header sizer = %d \n", bmp_file_header.bfSize);
        printf("bmp info header size = %d \n", bmp_info_header.biSize);

        RGBImg rgb_img = alocar_RGB(bmp_info_header.biWidth, bmp_info_header.biHeight);

        fseek(input_file, bmp_file_header.bfOffBits, SEEK_SET); // Posiciona o ponteiro para o início dos dados da imagem
        ler_bmp_rgb(input_file, rgb_img);

    fclose(input_file);

    exportar_bmp("imagem_saida.bmp", bmp_file_header, bmp_info_header, rgb_img);
    printf("Imagem salva como imagem_saida.bmp\n");

    liberar_RGB(rgb_img);

    return 0;
}