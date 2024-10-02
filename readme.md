# Compilador C - Documentação do Analisador Léxico

## Visão Geral

Este projeto implementa um analisador léxico para uma linguagem de programação similar a Pascal. O analisador é responsável por tokenizar o código-fonte de entrada, identificando e categorizando os elementos léxicos da linguagem.

## Estruturas de Dados

### TokenType (Enum)

Define os tipos de tokens que o analisador pode reconhecer, incluindo palavras-chave, operadores, pontuação e outros elementos léxicos.

### Token (Struct)

Representa um token individual com os seguintes campos:
- `type`: O tipo do token (TokenType)
- `lexeme`: A sequência de caracteres que forma o token
- `line`: A linha onde o token foi encontrado
- `column`: A coluna onde o token foi encontrado

### HashTable (Struct)

Implementa uma tabela hash para armazenar as palavras-chave da linguagem, facilitando a identificação rápida desses tokens.

## Funções Principais

### create_token

```c
Token *create_token(TokenType type, const char *lexeme, int line, int column)
```

Cria um novo token com os parâmetros fornecidos.

### free_token

```c
void free_token(Token *token)
```

Libera a memória alocada para um token.

### get_next_token

```c
Token *get_next_token(FILE *file, int *line, int *column)
```

Função principal do analisador léxico. Lê o próximo token do arquivo de entrada e o retorna.

### init_keyword_table

```c
void init_keyword_table()
```

Inicializa a tabela hash com as palavras-chave da linguagem.

### get_keyword_token

```c
TokenType get_keyword_token(const char *lexeme)
```

Verifica se um lexema é uma palavra-chave e retorna o tipo de token correspondente.

## Fluxo do Programa

1. O programa principal (`main`) abre o arquivo de entrada e cria um arquivo de saída.
2. Inicializa a tabela de palavras-chave.
3. Entra em um loop, chamando `get_next_token` repetidamente até encontrar o fim do arquivo ou um erro.
4. Cada token é impresso no arquivo de saída e no console.
5. Ao final, os recursos são liberados e os arquivos são fechados.

## Análise dos Resultados

Analisando o arquivo `saidas.txt`, podemos observar o seguinte:

1. **Arquivos corretos (certo1.pas, certo2.pas, certo3.pas)**:
   - O analisador léxico foi capaz de identificar corretamente todos os tokens, incluindo palavras-chave, identificadores, literais e operadores.
   - A posição (linha e coluna) de cada token foi registrada com precisão.
   - Tokens especiais como EOF (fim de arquivo) foram corretamente identificados.

2. **Arquivos com erros (errado1.pas, errado2.pas, errado3.pas)**:
   - Em `errado1.pas`, o analisador detectou um comentário não fechado, demonstrando sua capacidade de identificar erros léxicos.
   - Em `errado2.pas`, um caractere inválido ('\') foi detectado como um erro.
   - Em `errado3.pas`, um operador desconhecido ('!') foi identificado como um erro.

3. **Desempenho geral**:
   - O analisador demonstrou consistência na identificação de tokens em diferentes contextos.
   - Tokens complexos, como literais de string e operadores compostos (':='), foram corretamente reconhecidos.
   - A detecção de erros léxicos funcionou conforme esperado, interrompendo a análise quando encontrou problemas no código-fonte.

## Conclusão

O analisador léxico implementado demonstra ser robusto e capaz de lidar com uma variedade de casos, tanto em código correto quanto em situações de erro. A estrutura modular do código, com funções bem definidas para cada responsabilidade, contribui para a clareza e manutenibilidade do projeto. A utilização de uma tabela hash para palavras-chave é uma escolha eficiente que provavelmente melhora o desempenho da análise léxica.

No geral, o analisador léxico cumpre seu propósito de forma eficaz, fornecendo uma base sólida para as etapas subsequentes de um compilador ou interpretador.
