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

Atualização nessa v2:
Encontramos o problema, que estava relacionado com as Tabelas 2 e 3 da especificação. Implementamos errado a tabela 4,  a tabela 3 não apresentava a classificação B para os valores até 2047 em DC que a a tabela 2 descrevia. Enfrentadas essas situações, e devendo algumas correções de leaks de memória, a compressão e descompressão com perdas está funcional no trabalho. Ainda existem correções necessárias para a compressão sem perdas, que não está funcionando corretamente

## Modo de Uso

Modelo de imagens de entrada:
- Arquivos BMP, sem compressão.
- 24 bits para cor.
- Altura e largura sempre múltiplos de 8.
- Dimensões mínimas: 8 x 8 pixels.
- Dimensões máximas: 1280 x 800 pixels.

O programa apresenta um menu ao ser executado, e permite a escolha de compressão com ou sem perdas e de descompressão de uma imagem comprimida. O nome dos arquivos e a taxa de compressão serão perguntados a partir da escolha de operação selecionada.

Na opção 2, com perdas, a compressão com k = 5 apresentou taxas altas de compressão, mas que não descaracterizavam os objetos das imagens de forma definitiva, chegando a taxas próximas de 20:1. Para compressões com perdas mais imperceptíveis mas taxas de compressão significativas, encontramos k = 2 como um valor que preserva bem as características da imagem.

## Instruções de Compilação

Compile o programa normalmente utilizando make all e execute usando make run