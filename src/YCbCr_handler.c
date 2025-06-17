/************************************
* INCLUDES
************************************/

    #include "../include/YCbCr_handler.h"
    #include <stdio.h>

/************************************
* EXTERN VARIABLES
************************************/

const double C[8][8] = 
{  
    {0.354,  0.354,  0.354,  0.354,  0.354,  0.354,  0.354,  0.354},
    {0.490,  0.416,  0.278,  0.098, -0.098, -0.278, -0.416, -0.490},
    {0.462,  0.191, -0.191, -0.462, -0.462, -0.191,  0.191,  0.462},
    {0.416, -0.098, -0.490, -0.278,  0.278,  0.490,  0.098, -0.416},
    {0.354, -0.354, -0.354,  0.354,  0.354, -0.354, -0.354,  0.354},
    {0.278, -0.490,  0.098,  0.416, -0.416, -0.098,  0.490, -0.278},
    {0.191, -0.462,  0.462, -0.191, -0.191,  0.462, -0.462,  0.191},
    {0.098, -0.278,  0.416, -0.490,  0.490, -0.416,  0.278, -0.098}
};

double const Ct[8][8] = {
    { 0.354,  0.490,  0.462,  0.416,  0.354,  0.278,  0.191,  0.098 },
    { 0.354,  0.416,  0.191, -0.098, -0.354, -0.490, -0.462, -0.278 },
    { 0.354,  0.278, -0.191, -0.490, -0.354,  0.098,  0.462,  0.416 },
    { 0.354,  0.098, -0.462, -0.278,  0.354,  0.416, -0.191, -0.490 },
    { 0.354, -0.098, -0.462,  0.278,  0.354, -0.416, -0.191,  0.490 },
    { 0.354, -0.278, -0.191,  0.490, -0.354, -0.098,  0.462, -0.416 },
    { 0.354, -0.416,  0.191,  0.098, -0.354,  0.490, -0.462,  0.278 },
    { 0.354, -0.490,  0.462, -0.416,  0.354, -0.278,  0.191, -0.098 }
};

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

    RGBImg alocar_RGB(int width, int height) 
    {
        RGBImg rgb_img;

        rgb_img.height = height;
        rgb_img.width = width;

        rgb_img.R = malloc(rgb_img.height * sizeof(unsigned char*));
        rgb_img.G = malloc(rgb_img.height * sizeof(unsigned char*));
        rgb_img.B = malloc(rgb_img.height * sizeof(unsigned char*));

        for (int i = 0; i < height; i++) 
        {
            rgb_img.R[i] = malloc(rgb_img.width * sizeof(unsigned char));
            rgb_img.G[i] = malloc(rgb_img.width * sizeof(unsigned char));
            rgb_img.B[i] = malloc(rgb_img.width * sizeof(unsigned char));
        }

        return rgb_img;
    }


    void liberar_RGB(RGBImg rgb_img)
    {
        for (int i = 0; i < rgb_img.height; i++) 
        {
            free(rgb_img.R[i]);
            free(rgb_img.G[i]);
            free(rgb_img.B[i]);
        }

        free(rgb_img.R); free(rgb_img.G); free(rgb_img.B);
    }


    YCbCrImg alocar_YCbCr(int width, int height) 
    {
        YCbCrImg YCbCr_img;

        YCbCr_img.height = height;
        YCbCr_img.width = width;

        YCbCr_img.Y = malloc(YCbCr_img.height * sizeof(double*));
        YCbCr_img.Cb = malloc(YCbCr_img.height * sizeof(double*));
        YCbCr_img.Cr = malloc(YCbCr_img.height * sizeof(double*));

        for (int i = 0; i < height; i++) 
        {
            YCbCr_img.Y[i] = malloc(YCbCr_img.width * sizeof(double));
            YCbCr_img.Cb[i] = malloc(YCbCr_img.width * sizeof(double));
            YCbCr_img.Cr[i] = malloc(YCbCr_img.width * sizeof(double));
        }

        return YCbCr_img;
    }

/* Essa função não está sendo usada. 
Eu achei que ia precisar por causa da Downsampling mas eu crio na mao as matriz
E seto os ponteiros. acho que nem vamos usar. por mim pode apagar.*/ 
YCbCrImg alocar_YCbCr_reduzida(int width, int height)
{
    YCbCrImg YCbCr_img;

    YCbCr_img.height = height;
    YCbCr_img.width = width;

    YCbCr_img.Y = malloc(YCbCr_img.height * sizeof(double*));
    YCbCr_img.Cb = malloc((YCbCr_img.height/2) * sizeof(double*));
    YCbCr_img.Cr = malloc((YCbCr_img.height/2) * sizeof(double*));

    for (int i = 0; i < height; i++) {
        YCbCr_img.Y[i] = malloc(width * sizeof(double));
    }

    for (int i = 0; i < height / 2; i++) {
        YCbCr_img.Cb[i] = malloc((width / 2) * sizeof(double));
        YCbCr_img.Cr[i] = malloc((width / 2) * sizeof(double));
    }

    return YCbCr_img;
}

void liberar_YCbCr(YCbCrImg YCbCr_img)
{
    for (int i = 0; i < YCbCr_img.height; i++) 
    {
        free(YCbCr_img.Y[i]);
        free(YCbCr_img.Cb[i]);
        free(YCbCr_img.Cr[i]);
    }

    free(YCbCr_img.Y); free(YCbCr_img.Cb); free(YCbCr_img.Cr);
}

void liberar_YCbCr_reduced(YCbCrImg YCbCr_img)
{
    for (int i = 0; i < YCbCr_img.height; i++) 
    {
        free(YCbCr_img.Y[i]);
    }

    for (int i = 0; i < YCbCr_img.height/2; i++)
    {
        free(YCbCr_img.Cb[i]);
        free(YCbCr_img.Cr[i]);
    } 

    free(YCbCr_img.Y); free(YCbCr_img.Cb); free(YCbCr_img.Cr);
}


void RGB2YCbCr(YCbCrImg YCbCr_img, RGBImg rgb_img)
{
    //double R, G, B;
    double Y, Cb, Cr;

    for (int i = 0; i < rgb_img.height; i++) {
        for (int j = 0; j < rgb_img.width; j++) {

            Y  = 0.299 * rgb_img.R[i][j] + 0.587 * rgb_img.G[i][j] + 0.114 * rgb_img.B[i][j];
            Cb = 0.564 * (rgb_img.B[i][j] - Y);
            Cr = 0.713 * (rgb_img.R[i][j] - Y);

            YCbCr_img.Y[i][j]  = Y - 128.0;
            YCbCr_img.Cb[i][j] = Cb;
            YCbCr_img.Cr[i][j] = Cr;
        }
    }
}

void YCbCr2RGB(YCbCrImg YCbCr_img, RGBImg rgb_img)
{
    double Y, Cb, Cr;
    int R, G, B;

    for (int i = 0; i < YCbCr_img.height; i++) {
        for (int j = 0; j < YCbCr_img.width; j++) {
            Y  = YCbCr_img.Y[i][j] + 128.0;  // Reverte o level shifting
            Cb = YCbCr_img.Cb[i][j];
            Cr = YCbCr_img.Cr[i][j];

            R = round(Y + 1.402 * Cr);
            G = round(Y - 0.344 * Cb - 0.714 * Cr);
            B = round(Y + 1.772 * Cb);

            // Clamp entre 0 e 255
            if (R < 0) R = 0; 
            if (R > 255) R = 255;
            if (G < 0) G = 0; 
            if (G > 255) G = 255;
            if (B < 0) B = 0; 
            if (B > 255) B = 255;

            rgb_img.R[i][j] = (unsigned char)R;
            rgb_img.G[i][j] = (unsigned char)G;
            rgb_img.B[i][j] = (unsigned char)B;
        }
    }
}

YCbCrImg downsamplig(YCbCrImg YCbCr_img)
{
    YCbCrImg YCbCr_img_reduced;

    int height = YCbCr_img.height;
    int width = YCbCr_img.width;

    // ver se preciso fazer padding na Cb e Cr
    int height_pad;
    if (height % 16 == 0){
        height_pad = height;
    } else {
        height_pad = height + (16 - (height % 16));
    }

    int width_pad;
    if (width % 16 == 0){
        width_pad = width;
    } else {
        width_pad = width + (16 - (width % 16));
    }

    // Alocar espaco para a nova cb e cr que recebera o padding para poder fazer o downsampling
    double **Cb_padded = malloc(height_pad * sizeof(double*));
    double **Cr_padded = malloc(height_pad * sizeof(double*));

    for (int i = 0; i < height_pad; i++) {
        // Cria espaco na matriz
        Cb_padded[i] = malloc(width_pad * sizeof(double));
        Cr_padded[i] = malloc(width_pad * sizeof(double));

        for (int j = 0; j < width_pad; j++) {
            // Copia a matriz original e se precisar vai copiando a ultima linha para o padding
            int i_orig = i;
            int j_orig = j;
            if (i >= height) i_orig = height - 1;
            if (j >= width) j_orig = width - 1;
            Cb_padded[i][j] = YCbCr_img.Cb[i_orig][j_orig];
            Cr_padded[i][j] = YCbCr_img.Cr[i_orig][j_orig];
        }
    }

    // Aloca espaco para o Cb e Cr com metade do tamanho
    int reduced_height = height_pad / 2;
    int reduced_width = width_pad / 2;

    double** Cb_reduzida = malloc(reduced_height * sizeof(double*));
    double** Cr_reduzida = malloc(reduced_height * sizeof(double*));
    for (int i = 0; i < reduced_height; i++) {
        Cb_reduzida[i] = malloc(reduced_width * sizeof(double));
        Cr_reduzida[i] = malloc(reduced_width * sizeof(double));
    }
    
    // Faz o downsampling
    for (int i = 0; i < height_pad; i += 2) {
        for (int j = 0; j < width_pad; j += 2) {
            double cb_soma = Cb_padded[i][j] + Cb_padded[i+1][j] + Cb_padded[i][j+1] + Cb_padded[i+1][j+1];
            double cr_soma = Cr_padded[i][j] + Cr_padded[i+1][j] + Cr_padded[i][j+1] + Cr_padded[i+1][j+1];

            Cb_reduzida[i/2][j/2] = cb_soma / 4.0;
            Cr_reduzida[i/2][j/2] = cr_soma / 4.0;
        }
    }

    // Cria espaco para Y e copia
    double** Y_padded = malloc(height_pad * sizeof(double*));
    for (int i = 0; i < height_pad; i++) {
        Y_padded[i] = malloc(width_pad * sizeof(double));
        for (int j = 0; j < width_pad; j++) {
            int i_orig = i;
            int j_orig = j;
            if (i >= height) 
                i_orig = height - 1;
            if (j >= width) 
                j_orig = width - 1;

            Y_padded[i][j] = YCbCr_img.Y[i_orig][j_orig];
        }
    }

    // Seta os ponteiros
    YCbCr_img_reduced.height = height_pad;
    YCbCr_img_reduced.width = width_pad;
    YCbCr_img_reduced.Y = Y_padded;
    YCbCr_img_reduced.Cb = Cb_reduzida;
    YCbCr_img_reduced.Cr = Cr_reduzida;

    // Free da matriz cb e cr padded
    for (int i = 0; i < height_pad; i++) {
        free(Cb_padded[i]);
        free(Cr_padded[i]);
    }
    free(Cb_padded);
    free(Cr_padded);

    // Return
    return YCbCr_img_reduced; 
}


void aplicar_DCT_bloco(double B[8][8], double F[8][8])
{
    double temp[8][8];

    // temp = C × B
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            temp[i][j] = 0.0;
            for (int k = 0; k < 8; k++) {
                temp[i][j] += C[i][k] * B[k][j];
            }
        }
    }

    // F = temp × Ct
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            F[i][j] = 0.0;
            for (int k = 0; k < 8; k++) {
                F[i][j] += temp[i][k] * Ct[k][j];
            }
        }
    }
}

double** aplicar_DCT(double** entrada, int altura, int largura)
{
    // Alocando espaço para a matriz passada
    double** saida = malloc(altura * sizeof(double*));
    for (int i = 0; i < altura; i++) {
        saida[i] = malloc(largura * sizeof(double));
    }

    double B[8][8];
    double F[8][8];

    for (int i_bloco = 0; i_bloco < altura; i_bloco += 8) {
        for (int j_bloco = 0; j_bloco < largura; j_bloco += 8) {

            // Copia o bloco da matriz original
            for (int i = 0; i < 8; i++) {
                for (int j = 0; j < 8; j++) {
                    B[i][j] = entrada[i_bloco + i][j_bloco + j];
                }
            }

            // Aplica DCT no bloco
            aplicar_DCT_bloco(B, F);

            // Copia resultado para matriz final
            for (int i = 0; i < 8; i++) {
                for (int j = 0; j < 8; j++) {
                    saida[i_bloco + i][j_bloco + j] = F[i][j];
                }
            }
        }
    }

    return saida;
}

YCbCrImg executar_DCT(YCbCrImg entrada)
{
    YCbCrImg transformado;

    transformado.width = entrada.width;
    transformado.height = entrada.height;

    transformado.Y  = aplicar_DCT(entrada.Y, entrada.height, entrada.width);
    transformado.Cb = aplicar_DCT(entrada.Cb, entrada.height/2, entrada.width/2);
    transformado.Cr = aplicar_DCT(entrada.Cr, entrada.height/2, entrada.width/2);

    return transformado;
}

void aplicar_IDCT_bloco(double F[8][8], double B[8][8])
{
    double temp[8][8];

    // temp = Ct × F
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            temp[i][j] = 0.0;
            for (int k = 0; k < 8; k++) {
                temp[i][j] += Ct[i][k] * F[k][j];
            }
        }
    }

    // B = temp × C
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            B[i][j] = 0.0;
            for (int k = 0; k < 8; k++) {
                B[i][j] += temp[i][k] * C[k][j];
            }
        }
    }
}

double** aplicar_IDCT(double** entrada, int altura, int largura)
{
    double** saida = malloc(altura * sizeof(double*));
    for (int i = 0; i < altura; i++) {
        saida[i] = malloc(largura * sizeof(double));
    }

    double F[8][8];
    double B[8][8];

    for (int i_bloco = 0; i_bloco < altura; i_bloco += 8) {
        for (int j_bloco = 0; j_bloco < largura; j_bloco += 8) {

            // Copia o bloco transformado
            for (int i = 0; i < 8; i++) {
                for (int j = 0; j < 8; j++) {
                    F[i][j] = entrada[i_bloco + i][j_bloco + j];
                }
            }

            // Aplica a IDCT
            aplicar_IDCT_bloco(F, B);

            // Copia o resultado para a matriz final
            for (int i = 0; i < 8; i++) {
                for (int j = 0; j < 8; j++) {
                    saida[i_bloco + i][j_bloco + j] = B[i][j];
                }
            }
        }
    }

    return saida;
}

YCbCrImg executar_IDCT(YCbCrImg entrada)
{
    YCbCrImg reconstruido;

    reconstruido.width = entrada.width;
    reconstruido.height = entrada.height;

    reconstruido.Y  = aplicar_IDCT(entrada.Y, entrada.height, entrada.width);
    reconstruido.Cb = aplicar_IDCT(entrada.Cb, entrada.height/2, entrada.width/2);
    reconstruido.Cr = aplicar_IDCT(entrada.Cr, entrada.height/2, entrada.width/2);

    return reconstruido;
}

YCbCrImg upsampling(YCbCrImg reduzido)
{
    YCbCrImg completo;

    completo.width = reduzido.width;
    completo.height = reduzido.height;

    // Alocar Y (mesmo tamanho)
    completo.Y = malloc(completo.height * sizeof(double*));
    for (int i = 0; i < completo.height; i++) {
        completo.Y[i] = malloc(completo.width * sizeof(double));
        for (int j = 0; j < completo.width; j++) {
            completo.Y[i][j] = reduzido.Y[i][j];
        }
    }

    // Alocar Cb e Cr (tamanho cheio)
    completo.Cb = malloc(completo.height * sizeof(double*));
    completo.Cr = malloc(completo.height * sizeof(double*));
    for (int i = 0; i < completo.height; i++) {
        completo.Cb[i] = malloc(completo.width * sizeof(double));
        completo.Cr[i] = malloc(completo.width * sizeof(double));
    }

    // Preencher Cb e Cr com replicação de blocos 2x2
    for (int i = 0; i < reduzido.height/2; i++) {
        for (int j = 0; j < reduzido.width/2; j++) {
            double cb_val = reduzido.Cb[i][j];
            double cr_val = reduzido.Cr[i][j];

            int i2 = i * 2;
            int j2 = j * 2;

            completo.Cb[i2][j2]     = cb_val;
            completo.Cb[i2+1][j2]   = cb_val;
            completo.Cb[i2][j2+1]   = cb_val;
            completo.Cb[i2+1][j2+1] = cb_val;

            completo.Cr[i2][j2]     = cr_val;
            completo.Cr[i2+1][j2]   = cr_val;
            completo.Cr[i2][j2+1]   = cr_val;
            completo.Cr[i2+1][j2+1] = cr_val;
        }
    }

    return completo;
}

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


