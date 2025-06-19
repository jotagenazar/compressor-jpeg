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
O programa não está funcionando corretamente. A função TESTE_funcional implementa a compressão até a quantização e faz a descompressão a partir dela de maneira bem sucedida. A escrita codificada em um binário pela codificação entrópica não foi bem sucedida no trabalho, apesar da implementação parcial estar estruturada e presente mas conter alguma falha que não pudemos identificar em tempo hábil

## Modo de Uso

Modelo de imagens de entrada:
- Arquivos BMP, sem compressão.
- 24 bits para cor.
- Altura e largura sempre múltiplos de 8.
- Dimensões mínimas: 8 x 8 pixels.
- Dimensões máximas: 1280 x 800 pixels.

O programa apresenta um menu ao ser executado, e permite a escolha de compressão com ou sem perdas e de descompressão de uma imagem comprimida. O nome dos arquivos e a taxa de compressão serão perguntados a partir da escolha de operação selecionada

## Instruções de Compilação

Compile o programa normalmente utilizando make all e execute usando make run