/************************************
* INCLUDES
************************************/

#include "../include/YCbCr_handler.h"


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


/************************************
* STATIC FUNCTION PROTOTYPES
************************************/

// Função que calcula o padding necessário para uma dimensão das matrizes Cb e Cr que sofrerão diminuição no processo de downsampling
// Usada para criar uma YCbCr com dimensões adaptadas que podem sofrer downsampling sendo divididas por DOWNSAMPLE_DIVISOR
int _calcular_dim_com_padding_YCbCr(int size);

// Função que copia os valores das matrizes YCbCr para as matrizes de outra struct, alvo deve ser
// sempre maior que a matriz fonte, ultimo pixel da menor repetido ate o final da linha da maior
void _copiar_YCbCr(YCbCrImg fonte, YCbCrImg alvo);

// Função que copia o valor apenas da matriz Y para as matrizes da outra struct.
// fonte e alvo são do mesmo tamanho
void _copiar_matriz_Y(YCbCrImg fonte, YCbCrImg alvo);

// Função que aplica a DCT a partir de um bloco B passado como parâmetro e retorna os coeficientes em F
void _aplicar_DCT_bloco(double B[8][8], double F[8][8]);

// Função que aplica a inversa da DCT a partir dos coeficientes em F passados como parâmetro e retorna o bloco
// original B
void _aplicar_IDCT_bloco(double F[8][8], double B[8][8]);

// Função que aplica a DCT em todos os blocos da matriz passada como parâmetro e retorna a nova matriz alocada com os
// coeficientes calculados em cada bloco
double** _aplicar_DCT_matriz(double** matriz, int height, int width);

// Função que aplica a inversa da DCT em todos os coeficientes nos blocos da matriz passada como parâmetro e retorna 
// a nova matriz alocada com os blocos originais em cada posição
double** _aplicar_IDCT_matriz(double** matriz, int height, int width);


/************************************
* STATIC FUNCTIONS
************************************/

int _calcular_dim_com_padding_YCbCr(int size)
{
    int size_pad;
    int divisor = 8 * DOWNSAMPLE_DIVISOR;  // tamanho bloco * divisor do downsample

    if (size % divisor == 0){
        size_pad = size;
    } else {
        size_pad = size + (divisor - (size % divisor));
    }

    return size_pad;
}

void _copiar_YCbCr(YCbCrImg fonte, YCbCrImg alvo) 
{

    for (int i = 0; i < alvo.height; i++) 
    {
        for (int j = 0; j < alvo.width; j++) 
        {
            // guarda os indices atuais para copia repetida do ultimo valor da linha
            // até o final do alvo maior
            int i_orig = i;
            int j_orig = j;

            // se estourou o tamanho da fonte, volta para o valor da ultima linha ou coluna
            if (i >= fonte.height) i_orig = fonte.height - 1;
            if (j >= fonte.width) j_orig = fonte.width - 1;

            alvo.Y[i][j]    = fonte.Y[i_orig][j_orig];
            alvo.Cb[i][j]   = fonte.Cb[i_orig][j_orig];
            alvo.Cr[i][j]   = fonte.Cr[i_orig][j_orig];
        }
    }
}

void _copiar_matriz_Y(YCbCrImg fonte, YCbCrImg alvo)
{
    for (int i = 0; i < alvo.height; i++) 
    {
        for (int j = 0; j < alvo.width; j++) 
        {
            alvo.Y[i][j] = fonte.Y[i][j];
        }
    }
}

void _aplicar_DCT_bloco(double B[8][8], double F[8][8])
{   
    // DCT = C x B x Ct
    
    // matriz auxiliar para realizar a multiplicação
    double temp[8][8];

    // temp = C × B
    for (int i = 0; i < 8; i++) 
    {
        for (int j = 0; j < 8; j++) 
        {
            temp[i][j] = 0.0;

            for (int k = 0; k < 8; k++) 
            {
                temp[i][j] += C[i][k] * B[k][j];
            }
        }
    }

    // F = temp × Ct
    for (int i = 0; i < 8; i++) 
    {
        for (int j = 0; j < 8; j++) 
        {
            F[i][j] = 0.0;

            for (int k = 0; k < 8; k++) {
                F[i][j] += temp[i][k] * Ct[k][j];
            }
        }
    }
}


void _aplicar_IDCT_bloco(double F[8][8], double B[8][8])
{
    // B = Ct x F x C

    // matriz auxiliar para realizar a multiplicação
    double temp[8][8];

    // temp = Ct × F
    for (int i = 0; i < 8; i++) 
    {
        for (int j = 0; j < 8; j++) 
        {
            temp[i][j] = 0.0;

            for (int k = 0; k < 8; k++) 
            {
                temp[i][j] += Ct[i][k] * F[k][j];
            }
        }
    }

    // B = temp × C
    for (int i = 0; i < 8; i++) 
    {
        for (int j = 0; j < 8; j++) 
        {
            B[i][j] = 0.0;
            
            for (int k = 0; k < 8; k++) 
            {
                B[i][j] += temp[i][k] * C[k][j];
            }
        }
    }
}


double** _aplicar_DCT_matriz(double** matriz, int height, int width)
{
    // Alocando espaço para a matriz de resultado
    double** output = malloc(height * sizeof(double*));
    for (int i = 0; i < height; i++) 
    {
        output[i] = malloc(width * sizeof(double));
    }

    double B[8][8];
    double F[8][8];

    for (int i_bloco = 0; i_bloco < height; i_bloco += 8) 
    {
        for (int j_bloco = 0; j_bloco < width; j_bloco += 8) 
        {

            // Copia o bloco da matriz original
            for (int i = 0; i < 8; i++) 
            {
                for (int j = 0; j < 8; j++) 
                {
                    B[i][j] = matriz[i_bloco + i][j_bloco + j];
                }
            }

            // Aplica DCT no bloco
            _aplicar_DCT_bloco(B, F);

            // Copia o resultado para matriz final
            for (int i = 0; i < 8; i++) 
            {
                for (int j = 0; j < 8; j++) 
                {
                    output[i_bloco + i][j_bloco + j] = F[i][j];
                }
            }

        }
    }

    return output;
}


double** _aplicar_IDCT_matriz(double** matriz, int height, int width)
{
    double** output = malloc(height * sizeof(double*));
    for (int i = 0; i < height; i++) 
    {
        output[i] = malloc(width * sizeof(double));
    }

    double F[8][8];
    double B[8][8];

    for (int i_bloco = 0; i_bloco < height; i_bloco += 8) 
    {
        for (int j_bloco = 0; j_bloco < width; j_bloco += 8) 
        {

            // Copia o bloco transformado
            for (int i = 0; i < 8; i++) 
            {
                for (int j = 0; j < 8; j++) 
                {
                    F[i][j] = matriz[i_bloco + i][j_bloco + j];
                }
            }

            // Aplica a IDCT
            _aplicar_IDCT_bloco(F, B);

            // Copia o resultado para a matriz final
            for (int i = 0; i < 8; i++) 
            {
                for (int j = 0; j < 8; j++) 
                {
                    output[i_bloco + i][j_bloco + j] = B[i][j];
                }
            }

        }

    }

    return output;
}

/************************************
* GLOBAL FUNCTIONS
************************************/

RGBImg alocar_RGB(int width, int height) 
{   
    // Criação da instância da struct e preenchimento dos valores
    RGBImg rgb_img;
    rgb_img.height = height;
    rgb_img.width = width;

    // alocação das matrizes rgb da struct
    rgb_img.R = malloc(rgb_img.height * sizeof(unsigned char*));
    rgb_img.G = malloc(rgb_img.height * sizeof(unsigned char*));
    rgb_img.B = malloc(rgb_img.height * sizeof(unsigned char*));

    for (int i = 0; i < rgb_img.height; i++) 
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
    // Criação da instância da struct e preenchimento dos valores
    YCbCrImg YCbCr_img;
    YCbCr_img.height = height;
    YCbCr_img.width = width;

    // alocação das matrizes YCbCr
    YCbCr_img.Y =   malloc(YCbCr_img.height * sizeof(double*));
    YCbCr_img.Cb =  malloc(YCbCr_img.height * sizeof(double*));
    YCbCr_img.Cr =  malloc(YCbCr_img.height * sizeof(double*));

    for (int i = 0; i < height; i++) 
    {
        YCbCr_img.Y[i] =    malloc(YCbCr_img.width * sizeof(double));
        YCbCr_img.Cb[i] =   malloc(YCbCr_img.width * sizeof(double));
        YCbCr_img.Cr[i] =   malloc(YCbCr_img.width * sizeof(double));
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


YCbCrImg alocar_YCbCr_downsampled(int width, int height)
{
    // Criação da instância da struct e preenchimento das dimensoes com o padding adequado
    YCbCrImg YCbCr_downsampled;
    YCbCr_downsampled.height = _calcular_dim_com_padding_YCbCr(height);
    YCbCr_downsampled.width = _calcular_dim_com_padding_YCbCr(width);

    // alocação das matrizes YCbCr a partir dos valores com padding da redução
    YCbCr_downsampled.Y =   malloc(YCbCr_downsampled.height                         * sizeof(double*));
    YCbCr_downsampled.Cb =  malloc((YCbCr_downsampled.height / DOWNSAMPLE_DIVISOR)  * sizeof(double*));
    YCbCr_downsampled.Cr =  malloc((YCbCr_downsampled.height / DOWNSAMPLE_DIVISOR)  * sizeof(double*));

    for (int i = 0; i < YCbCr_downsampled.height; i++) 
    {
        YCbCr_downsampled.Y[i] = malloc(YCbCr_downsampled.width * sizeof(double));
    }
    for (int i = 0; i < YCbCr_downsampled.height / DOWNSAMPLE_DIVISOR; i++) 
    {
        YCbCr_downsampled.Cb[i] = malloc((YCbCr_downsampled.width / DOWNSAMPLE_DIVISOR) * sizeof(double));
        YCbCr_downsampled.Cr[i] = malloc((YCbCr_downsampled.width / DOWNSAMPLE_DIVISOR) * sizeof(double));
    }

    return YCbCr_downsampled;
}


void liberar_YCbCr_downsampled(YCbCrImg YCbCr_downsampled)
{
    for (int i = 0; i < YCbCr_downsampled.height; i++) 
    {
        free(YCbCr_downsampled.Y[i]);
    }
    
    // liberação dos vetores das matrizes reduzidas Cb e Cr
    for (int i = 0; i < (YCbCr_downsampled.height / DOWNSAMPLE_DIVISOR); i++)
    {
        free(YCbCr_downsampled.Cb[i]);
        free(YCbCr_downsampled.Cr[i]);
    } 

    free(YCbCr_downsampled.Y); free(YCbCr_downsampled.Cb); free(YCbCr_downsampled.Cr);
}


void RGB_to_YCbCr(RGBImg rgb_img, YCbCrImg YCbCr_img)
{
    double Y, Cb, Cr;

    // realiza o cálculo dos valores de conversão para cada pixel da imagem em cada matriz
    for (int i = 0; i < rgb_img.height; i++) 
    {
        for (int j = 0; j < rgb_img.width; j++) 
        {

            Y  = 0.299 * rgb_img.R[i][j] + 0.587 * rgb_img.G[i][j] + 0.114 * rgb_img.B[i][j];
            Cb = 0.564 * (rgb_img.B[i][j] - Y);
            Cr = 0.713 * (rgb_img.R[i][j] - Y);

            YCbCr_img.Y[i][j]  = Y - 128.0;
            YCbCr_img.Cb[i][j] = Cb;
            YCbCr_img.Cr[i][j] = Cr;
        }
    }
}


void YCbCr_to_RGB(YCbCrImg YCbCr_img, RGBImg rgb_img)
{
    double Y, Cb, Cr;
    int R, G, B;

    // realiza o cálculo dos valores de conversão para cada pixel da imagem em cada matriz
    for (int i = 0; i < YCbCr_img.height; i++) 
    {
        for (int j = 0; j < YCbCr_img.width; j++) 
        {
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


YCbCrImg downsample_YCbCr(YCbCrImg YCbCr_img)
{   
    // calculo das dimensões com padding
    int width_pad =     _calcular_dim_com_padding_YCbCr(YCbCr_img.width);
    int height_pad =    _calcular_dim_com_padding_YCbCr(YCbCr_img.height);
    
    // alocaçao da ycbcr com padding auxiliar, que será usada para gerar a ycbcr downsampled com dimensões corretas
    YCbCrImg YCbCr_padded  = alocar_YCbCr(width_pad, height_pad);
    _copiar_YCbCr(YCbCr_img, YCbCr_padded);

    // alocação da ycbcr reduzida, com as matrizes cb e cr com tamanho de cada dimensão dividido pelo valor do DOWNSAMPLE_DIVISOR
    YCbCrImg YCbCr_downsampled = alocar_YCbCr_downsampled(width_pad, height_pad);
    
    // Cálculo do downsampling e preenchimento da ycbcr reduzida
    double cb_soma, cr_soma;
    int i_downs, j_downs;
    double qnt_valores = (double) DOWNSAMPLE_DIVISOR * DOWNSAMPLE_DIVISOR;
    for (int i = 0; i < height_pad; i += DOWNSAMPLE_DIVISOR) 
    {
        for (int j = 0; j < width_pad; j += DOWNSAMPLE_DIVISOR) 
        {   
            // calculo da soma dos valores dos pixels dentro daquele bloco que sofrerá downsampling
            cb_soma = 0.0;
            cr_soma = 0.0;
            for(int i_sum = 0; i_sum < DOWNSAMPLE_DIVISOR; i_sum++) 
            {
                for(int j_sum = 0; j_sum < DOWNSAMPLE_DIVISOR; j_sum++) 
                {
                    cb_soma += YCbCr_padded.Cb[i + i_sum][j + j_sum];
                    cr_soma += YCbCr_padded.Cr[i + i_sum][j + j_sum];
                }
            }
            
            // calculo posição final pós downsample
            i_downs = i / DOWNSAMPLE_DIVISOR;
            j_downs = j / DOWNSAMPLE_DIVISOR;

            // valores são a média, soma dividida pela quantidade de valores DOWNSAMPLE_DIVISOR * DOWNSAMPLE_DIVISOR
            YCbCr_downsampled.Cb[i_downs][j_downs] = cb_soma / qnt_valores;
            YCbCr_downsampled.Cr[i_downs][j_downs] = cr_soma / qnt_valores;
        }
    }

    // Copia matriz Y para struct YCbCr_downsampled
    _copiar_matriz_Y(YCbCr_padded, YCbCr_downsampled);

    // Free da YCbCr com padding auxiliar
    liberar_YCbCr(YCbCr_padded);

    return YCbCr_downsampled; 
}


YCbCrImg upsample_YCbCr(YCbCrImg YCbCr_downsampled)
{
    YCbCrImg YCbCr_upsampled = alocar_YCbCr(YCbCr_downsampled.width, YCbCr_downsampled.height);

    // preenchimento da matriz Y com a matriz preservada na imagem downsampled
    _copiar_matriz_Y(YCbCr_downsampled, YCbCr_upsampled);

    // preenchimento das matrizes Cb e Cr com replicação de blocos DOWNSAMPLE_DIVISOR x DOWNSAMPLE_DIVISOR
    for (int i = 0; i < YCbCr_downsampled.height / DOWNSAMPLE_DIVISOR; i++) 
    {
        for (int j = 0; j < YCbCr_downsampled.width / DOWNSAMPLE_DIVISOR; j++) 
        {
            double cb_val = YCbCr_downsampled.Cb[i][j];
            double cr_val = YCbCr_downsampled.Cr[i][j];

            int i2 = i * DOWNSAMPLE_DIVISOR;
            int j2 = j * DOWNSAMPLE_DIVISOR;

            // preenchimento de todas as posições do bloco com o valor correspondente na matriz downsampled
            for(int i_bloco = 0; i_bloco < DOWNSAMPLE_DIVISOR; i_bloco++) 
            {
                for(int j_bloco = 0; j_bloco < DOWNSAMPLE_DIVISOR; j_bloco++) 
                {
                    YCbCr_upsampled.Cb[i2 + i_bloco][j2 + j_bloco] = cb_val;
                    YCbCr_upsampled.Cr[i2 + i_bloco][j2 + j_bloco] = cr_val;
                }
            }

        }
    }

    return YCbCr_upsampled;
}

YCbCrImg aplicar_DCT_YCbCr_downsampled(YCbCrImg YCbCr_img)
{
    YCbCrImg YCbCr_DCT;

    YCbCr_DCT.width = YCbCr_img.width;
    YCbCr_DCT.height = YCbCr_img.height;

    YCbCr_DCT.Y  = _aplicar_DCT_matriz(YCbCr_img.Y, YCbCr_img.height, YCbCr_img.width);
    YCbCr_DCT.Cb = _aplicar_DCT_matriz(YCbCr_img.Cb, YCbCr_img.height / DOWNSAMPLE_DIVISOR, YCbCr_img.width / DOWNSAMPLE_DIVISOR);
    YCbCr_DCT.Cr = _aplicar_DCT_matriz(YCbCr_img.Cr, YCbCr_img.height / DOWNSAMPLE_DIVISOR, YCbCr_img.width / DOWNSAMPLE_DIVISOR);

    return YCbCr_DCT;
}


YCbCrImg aplicar_IDCT_YCbCr_downsampled(YCbCrImg YCbCr_DCT)
{
    YCbCrImg YCbCr_img;

    YCbCr_img.width = YCbCr_DCT.width;
    YCbCr_img.height = YCbCr_DCT.height;

    YCbCr_img.Y  = _aplicar_IDCT_matriz(YCbCr_DCT.Y, YCbCr_DCT.height, YCbCr_DCT.width);
    YCbCr_img.Cb = _aplicar_IDCT_matriz(YCbCr_DCT.Cb, YCbCr_DCT.height / DOWNSAMPLE_DIVISOR, YCbCr_DCT.width / DOWNSAMPLE_DIVISOR);
    YCbCr_img.Cr = _aplicar_IDCT_matriz(YCbCr_DCT.Cr, YCbCr_DCT.height / DOWNSAMPLE_DIVISOR, YCbCr_DCT.width / DOWNSAMPLE_DIVISOR);

    return YCbCr_img;
}