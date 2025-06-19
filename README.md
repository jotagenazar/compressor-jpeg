# Compressor e Descompressor JPEG

SCC0261 – Multimídia - 2025.1  
Prof. Rudinei Goularte  
\
**Projeto: Compressão de Imagens**  
\
Alunos:  
João Gabriel Manfré Nazar (...)  
João Pedro Mori Machado (...)  

## Informações Gerais
Esta implementação é a versão final e funcional do programa.

Estrutura de um arquivo resultado da compressão pela opção 1, sem perdas:
- Headers bmp originais da imagem inserida -> fator k = 0 (usado para indicar que foi comprimido sem perdas) -> pixels codificados da matriz R, depois G, depois B

Como a compressão sem perdas está sendo realizada:
- Para cada pixel de cada matriz RGB, calcula-se sua diferença para o valor anterior. Usa-se a Tabela 2 do guia JPEG fornecido, das diferenças DC da codificação entrópica JPEG, para codificar o tamanho da diferença calculada em bits. Para cada pixel, escreve-se o prefixo que indica seu tamanho a partir da tabela, e a diferença calculada como mantissa.

Estrutura de um arquivo resultado da compressão pela opção 2, pipeline JPEG com perdas:
- Headers bmp originais da imagem inserida -> fator k de compressão usado -> blocos pós codificação entrópica de Y, depois de Cb, depois de Cr

Como a compressão com perdas está sendo realizada:
- Seguimos o pipeline JPEG especificado no guia fornecido, usando o k inserido pelo usuário na quantização.

A descompressão funciona aplicando o pipeline correto de acordo com o valor k. Como k = 0 é invalido para compressão JPEG por ser fator multiplicativo no denominador da quantização, o valor de k = 0 em um arquivo comprimido será o indicador de que devemos executar a descompressão sem perdas de pixels das matrizes R G e B de uma imagem.


## Modo de Uso

Modelo de imagens de entrada:
- Arquivos BMP, sem compressão.
- 24 bits para cor.
- Altura e largura sempre múltiplos de 8.
- Dimensões mínimas: 8 x 8 pixels.
- Dimensões máximas: 1280 x 800 pixels.

Para executar o programa, execute o binário "compressor-jpeg" compilado ou simplesmente execute o comando make run.

O programa apresenta um menu ao ser executado, e permite a escolha de compressão com ou sem perdas e de descompressão de uma imagem comprimida. O nome dos arquivos e a taxa de compressão serão perguntados a partir da escolha de operação selecionada.

Na opção 2, com perdas, a compressão com k = 5 apresentou taxas altas de compressão, mas que não descaracterizavam os objetos das imagens de forma definitiva, chegando a taxas entre 15:1 e 20:1. Para compressões com perdas mais imperceptíveis, mas taxas de compressão ainda significativas, encontramos k = 2 como um valor que preserva bem as características da imagem.

## Instruções de Compilação

Compile o programa normalmente utilizando o Makefile disponibilizado através do comando make all.
