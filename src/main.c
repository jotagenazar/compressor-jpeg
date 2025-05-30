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

        YCbCrImg YCbCr_img = alocar_YCbCr(bmp_info_header.biWidth, bmp_info_header.biHeight);

        RGB2YCbCr(YCbCr_img, rgb_img);

        YCbCrImg YCbCr_img_reduced = downsamplig(YCbCr_img);

    fclose(input_file);

    exportar_bmp("imagem_saida.bmp", bmp_file_header, bmp_info_header, rgb_img);
    printf("Imagem salva como imagem_saida.bmp\n");

    liberar_RGB(rgb_img);
    liberar_YCbCr(YCbCr_img);
    liberar_YCbCr_reduced(YCbCr_img_reduced);

    return 0;
}