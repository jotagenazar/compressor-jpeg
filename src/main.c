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
#include "../include/compression_tools.h"


void TESTE();


int main() 
{   
    // impressão de opções
    printf("\n\nEscolha a operaçao que deseja realizar:\n\n");
    printf("[1] Comprimir uma imagem *.bmp sem perdas\n");
    printf("[2] Comprimir imagem *.bmp com perdas a partir de um valor k\n");
    printf("[3] Descomprimir uma imagem comprimida pelo programa\n\n");
    printf("[0] Sair\n\n");


    // escolha operação
    int op_id;
    printf("Operação: ");
    scanf(" %d", &op_id);
    printf("\n\n");


    // variáveis comuns às operações
    char input_filename[100];
    char output_filename[100];
    int input_size;
    int output_size;
    double k;

    // execução das operações
    switch(op_id)
    {
        case 1: //comprimir imagem lossless
        {
            printf("Caminho da imagem *.bmp a ser comprimida: ");
            scanf(" %s", input_filename);

            printf("Nome do arquivo da nova imagem comprimida: ");
            scanf(" %s", output_filename);

            // Leitura do arquivo bmp para as structs de header e matriz RGB
            FILE *input_file = abrir_arquivo(input_filename, "rb");

                // leitura e importação do header do arquivo e do header da imagem
                BmpFileHeader bmp_file_header = ler_bmp_file_header(input_file);
                BmpInfoHeader bmp_info_header = ler_bmp_info_header(input_file);

                RGBImg rgb_img = alocar_RGB(bmp_info_header.biWidth, bmp_info_header.biHeight);

                fseek(input_file, bmp_file_header.bfOffBits, SEEK_SET); // Posiciona o ponteiro para o início dos dados da imagem
                ler_bmp_rgb(input_file, rgb_img);

                // calculo tamanho arquivo obtendo valor bit apontado no final do arquivo
                fseek(input_file, 0, SEEK_END);
                input_size = ftell(input_file); 

            fclose(input_file);

            // Criação matriz YCbCr e conversão da matriz RGB lida para ela
            YCbCrImg YCbCr_img = alocar_YCbCr(rgb_img.width, rgb_img.height);
            RGB_to_YCbCr(rgb_img, YCbCr_img);
            liberar_RGB(rgb_img);

            // não houve compressão, o fator k da quantização usado foi 0
            k = 0;

            // Escrita do arquivo binário de saída codificado e comprimido por diferença e huffman
            FILE *output_file = abrir_arquivo(output_filename, "wb");

                // escreve os headers bmp da imagem original
                escrever_headers_bmp(output_file, bmp_file_header, bmp_info_header);


                fwrite(&k, sizeof(double), 1, output_file);

                executar_compressao_entropica(YCbCr_img, output_file, k);

                // calculo tamanho arquivo obtendo valor bit apontado no final do arquivo
                output_size = ftell(output_file); 

            fclose(output_file);

            printf("\nImagem comprimida e salva em %s\n", output_filename);

            double taxa_compressao = (double) input_size / output_size;
            printf("Taxa de Compressão: %.2lf", taxa_compressao);

            liberar_YCbCr(YCbCr_img);

            break;
        }

        case 2: //comprimir imagem lossy
        {
            printf("Caminho da imagem *.bmp a ser comprimida: ");
            scanf(" %s", input_filename);

            printf("Nome do arquivo da nova imagem comprimida: ");
            scanf(" %s", output_filename);

            printf("Fator k de compressão da imagem (recomendado: 5): ");
            scanf(" %lf", &k);


            // Leitura do arquivo bmp para as structs de header e matriz RGB
            FILE *input_file = abrir_arquivo(input_filename, "rb");

                // leitura e importação do header do arquivo e do header da imagem
                BmpFileHeader bmp_file_header = ler_bmp_file_header(input_file);
                BmpInfoHeader bmp_info_header = ler_bmp_info_header(input_file);

                RGBImg rgb_img = alocar_RGB(bmp_info_header.biWidth, bmp_info_header.biHeight);

                fseek(input_file, bmp_file_header.bfOffBits, SEEK_SET); // Posiciona o ponteiro para o início dos dados da imagem
                ler_bmp_rgb(input_file, rgb_img);

                // calculo tamanho arquivo obtendo valor bit apontado no final do arquivo
                fseek(input_file, 0, SEEK_END);
                input_size = ftell(input_file); 

            fclose(input_file);


            // Criação matriz YCbCr e conversão da matriz RGB lida para ela
            YCbCrImg YCbCr_img = alocar_YCbCr(rgb_img.width, rgb_img.height);
            RGB_to_YCbCr(rgb_img, YCbCr_img);
            liberar_RGB(rgb_img);

            // Criação YCbCr downsampled
            YCbCrImg YCbCr_img_downsampled = downsample_YCbCr(YCbCr_img);
            liberar_YCbCr(YCbCr_img);

            // Aplicação transformação DCT na YCbCr
            YCbCrImg YCbCr_freq = aplicar_DCT_YCbCr(YCbCr_img_downsampled);
            liberar_YCbCr_downsampled(YCbCr_img_downsampled);

            // Quantização da imagem YCbCr no domínio das frequencias
            YCbCrImg YCbCr_quantizado = quantizar_imagem(YCbCr_freq, k);
            liberar_YCbCr_downsampled(YCbCr_freq);

            // atualiza os valores do cabeçalho bmp caso a imagem tenha recebido padding
            bmp_info_header.biWidth  = YCbCr_quantizado.width;
            bmp_info_header.biHeight = YCbCr_quantizado.height;

            // Escrita do arquivo binário de saída codificado e comprimido por diferença e huffman
            FILE *output_file = abrir_arquivo(output_filename, "wb");

                // repete os headers bmp da imagem original
                escrever_headers_bmp(output_file, bmp_file_header, bmp_info_header);

                // escreve o fator k usado para comprimir a imagem
                fwrite(&k, sizeof(double), 1, output_file);

                executar_compressao_entropica(YCbCr_quantizado, output_file, k);

                // calculo tamanho arquivo obtendo valor bit apontado no final do arquivo
                output_size = ftell(output_file); 

            fclose(output_file);


            printf("\nImagem comprimida e salva em %s\n", output_filename);

            double taxa_compressao = (double) input_size / output_size;
            printf("Taxa de Compressão: %.2lf", taxa_compressao);

            liberar_YCbCr_downsampled(YCbCr_quantizado);

            break;
        }


        // case 3: // descomprimir imagem
        // {
        //     printf("Caminho da imagem comprimida a ser descomprimida: ");
        //     scanf(" %s", input_filename);

        //     printf("Nome do arquivo da nova imagem *.bmp descomprimida: ");
        //     scanf(" %s", output_filename);

        //     // Leitura do arquivo e headers bmp
        //     FILE *input_file = abrir_arquivo(input_filename, "rb");

        //         // leitura e importação do header do arquivo e do header da imagem
        //         BmpFileHeader bmp_file_header = ler_bmp_file_header(input_file);
        //         BmpInfoHeader bmp_info_header = ler_bmp_info_header(input_file);

        //         // leitura do K
        //         fread(&k, sizeof(double), 1, input_file);

                

        //     fclose(input_file);

        //     if(k != 0)
        //     {
                
        //     }

        //     RGBImg rgb_final = alocar_RGB(YCbCr_up.width, YCbCr_up.height);

        //     YCbCr_to_RGB(YCbCr_up,  rgb_final);

        //     exportar_bmp("imagem_saida.bmp", bmp_file_header, bmp_info_header, rgb_final);
        //     printf("Imagem salva como imagem_saida.bmp\n");

        //     break;
        // }


        case 0: // sair do programa
        {
            return 0;
            break;
        }


        default:
        {
            printf("\nO valor de seleção da operação digitado é inválido.\n");
            break;
        }
    }

    return 0;
}



void TESTE()
{   
    char input_filename[100];
    printf("Caminho da imagem *.bmp a ser comprimida: ");
    scanf(" %s", input_filename);

    // Leitura do arquivo bmp para as structs de header e matriz RGB
     FILE *input_file = abrir_arquivo(input_filename, "rb");

        // leitura e importação do header do arquivo e do header da imagem
        BmpFileHeader bmp_file_header = ler_bmp_file_header(input_file);
        BmpInfoHeader bmp_info_header = ler_bmp_info_header(input_file);

        printf("file header sizer = %d \n", bmp_file_header.bfSize);
        printf("bmp info header size = %d \n", bmp_info_header.biSize);

        RGBImg rgb_img = alocar_RGB(bmp_info_header.biWidth, bmp_info_header.biHeight);

        fseek(input_file, bmp_file_header.bfOffBits, SEEK_SET); // Posiciona o ponteiro para o início dos dados da imagem
        ler_bmp_rgb(input_file, rgb_img);

    fclose(input_file);

    // Criação matriz YCbCr e conversão da matriz RGB lida para ela
    YCbCrImg YCbCr_img = alocar_YCbCr(rgb_img.width, rgb_img.height);
    RGB_to_YCbCr(rgb_img, YCbCr_img);
    liberar_RGB(rgb_img);

    // Criação YCbCr downsampled
    YCbCrImg YCbCr_img_downsampled = downsample_YCbCr(YCbCr_img);
    liberar_YCbCr(YCbCr_img);

    // Aplicação transformação DCT na YCbCr
    YCbCrImg YCbCr_freq = aplicar_DCT_YCbCr(YCbCr_img_downsampled);
    liberar_YCbCr_downsampled(YCbCr_img_downsampled);

    // Quantização da imagem YCbCr no domínio das frequencias
    YCbCrImg YCbCr_quantizado = quantizar_imagem(YCbCr_freq, 1.0);
    liberar_YCbCr_downsampled(YCbCr_freq);


    YCbCrImg YCbCr_desquantizado = desquantizar_imagem(YCbCr_quantizado, 1.0);
    liberar_YCbCr_downsampled(YCbCr_quantizado);


    YCbCrImg YCbCr_IDCT = aplicar_IDCT_YCbCr(YCbCr_desquantizado);
    liberar_YCbCr_downsampled(YCbCr_desquantizado);

    YCbCrImg YCbCr_up = upsample_YCbCr(YCbCr_IDCT);
    liberar_YCbCr_downsampled(YCbCr_IDCT);

    RGBImg rgb_final = alocar_RGB(YCbCr_up.width, YCbCr_up.height);

    YCbCr_to_RGB(YCbCr_up,  rgb_final);

    exportar_bmp("imagem_saida.bmp", bmp_file_header, bmp_info_header, rgb_final);
    printf("Imagem salva como imagem_saida.bmp\n");

    liberar_RGB(rgb_final);
    liberar_YCbCr(YCbCr_up);
}
