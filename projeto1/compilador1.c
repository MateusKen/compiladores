/*Implementacao do compilador da linguagem Pascal+-
Compiladores 2024/02 Turma 6G
NOME: Mateus Kenzo Iochimoto
RA: 10400995

para compilar
gcc -g -Og -Wall compilador1.c -o compilador1
para rodar
./compilador1 arquivo.txt
*/
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h> // exit(0);

#define MAX_ID_LENGTH 15

typedef enum {
    ERRO,
    COMENTARIO,
    IDENTIFICADOR,
    NUMERO,
    OP_RELACIONAL,
    OP_SOMA,
    OP_SUB,
    OP_MULT,
    OP_DIV,
    PONTO_VIRGULA,
    DOIS_PONTOS,
    VIRGULA,
    PONTO,
    ABRE_PARENTESES,
    FECHA_PARENTESES,
    MENOR,
    IGUAL,
    MAIOR,
    DIVIDIDO_IGUAL,
    MAIOR_IGUAL,
    MENOR_IGUAL,
    EOS,
    AND, 
    BEGIN, 
    BOOLEAN, 
    ELIF, 
    END, 
    FALSE, 
    FOR, 
    IF, 
    INTEGER, 
    NOT, 
    OF, 
    OR, 
    PROGRAM, 
    READ, 
    SET, 
    TO, 
    TRUE, 
    WRITE
} TAtomo;

typedef struct {
    TAtomo atomo;
    int linha;
    int atributo_numero;  // Para armazenar o valor numérico em decimal
    char atributo_ID[MAX_ID_LENGTH + 1];  // Para armazenar o lexema de identificadores
} TInfoAtomo;

char *msgAtomo[] = {
    "ERRO LEXICO", "COMENTARIO", "IDENTIFICADOR", "NUMERO", "OP_RELACIONAL", "+", "-", "*", "/", 
    ";", ":", ",", ".", "(", ")","<", "=", ">", "/=", ">=", "<=", "EOS",
    "AND", "BEGIN", "BOOLEAN", "ELIF", "END", "FALSE", "FOR", "IF", "INTEGER",
    "NOT", "OF", "OR", "PROGRAM", "READ", "SET", "TO", "TRUE", "WRITE"
};

char *palavras_reservadas[] = {
    "and", "begin", "boolean", "elif", "end", "false", "for", "if", "integer",
    "not", "of", "or", "program", "read", "set", "to", "true", "write"
};

TAtomo atomos_reservados[] = {
    AND, BEGIN, BOOLEAN, ELIF, END, FALSE, FOR, IF, INTEGER, NOT, OF, OR, PROGRAM, READ, SET, TO, TRUE, WRITE
};

// Buffer de entrada e contagem de linhas
char *buffer = NULL;
int contaLinha = 1;

// declaracoes para o lexico
TInfoAtomo obter_atomo(); //👍
TInfoAtomo reconhece_id(); //👍
TInfoAtomo reconhece_num(); //👍
void ignora_comentario(); //👍
int eh_palavra_reservada(char *lexema); //👍
int eh_op_relacional(char *lexema); //👍
int converte_binario_para_decimal(const char *binario); //👍

//declaracoes para o sintatico
TAtomo lookahead;
TInfoAtomo info_atomo;
void consome(TAtomo atomo);
void programa(); //👍
void bloco(); //👍
void declaracao_de_variaveis(); //👍
void tipo(); //👍
void lista_variavel(); //👍
void comando_composto(); //👍
void comando(); //👍
void comando_atribuicao(); //👍
void comando_condicional(); //👍
void comando_repeticao(); //👍
void comando_entrada(); //👍
void comando_saida(); //👍
void expressao(); //👍
void expressao_logica(); //👍
void expressao_relacional(); //👍
void op_relacional(); //👍
void expressao_simples(); //👍
void termo(); //👍
void fator(); //👍
void E();

char* carregar_arquivo(const char* nome_arquivo) {
    FILE *arquivo = fopen(nome_arquivo, "r");
    if (!arquivo) {
        printf("Erro ao abrir o arquivo.\n");
        exit(1);
    }

    fseek(arquivo, 0, SEEK_END);
    long tamanho_arquivo = ftell(arquivo);
    rewind(arquivo);

    char *buffer = (char *)malloc((tamanho_arquivo + 1) * sizeof(char));
    if (!buffer) {
        printf("Erro ao alocar memória.\n");
        exit(1);
    }

    fread(buffer, sizeof(char), tamanho_arquivo, arquivo);
    buffer[tamanho_arquivo] = '\0'; 

    fclose(arquivo);
    return buffer;
}

int main() {
    buffer = carregar_arquivo("arquivo.txt");

    info_atomo = obter_atomo();
    lookahead = info_atomo.atomo;

    programa(); 
    consome(EOS);

    printf("\n%d linhas analisadas, programa sintaticamente correto", info_atomo.linha);

    free(buffer);
    return 0;
}

//#########################
// INICIO: LEXICO
TInfoAtomo obter_atomo() {
    TInfoAtomo info_atomo;

    while (*buffer == ' ' || *buffer == '\n' || *buffer == '\t' || *buffer == '\r') {
        if (*buffer == '\n') contaLinha++;
        buffer++;
    }

    //para comentarios
    if (*buffer == '#') {
        while (*buffer != '\n') {
            buffer++;
        }
        info_atomo.atomo = COMENTARIO;
        printf("# %d: %s\n",contaLinha,msgAtomo[info_atomo.atomo]);
        return obter_atomo();
    } else if (*buffer == '{' && *(buffer + 1) == '-') {
        buffer += 2;
        info_atomo.atomo = COMENTARIO;
        printf("# %d: %s\n",contaLinha,msgAtomo[info_atomo.atomo]);
        ignora_comentario();
        
        return obter_atomo();
    } 
    
    else if (islower(*buffer)) {
        info_atomo = reconhece_id();
    } else if (*buffer == '0' && *(buffer + 1) == 'b') {
        info_atomo = reconhece_num();

    }else if (*buffer == '>') {
        if(*buffer == '='){ // ">="
            info_atomo.atomo = MAIOR_IGUAL;
            buffer+=2;
        }
        else{ //">"
            info_atomo.atomo = MAIOR;
            buffer++;
        }
    }else if (*buffer == '<') {
        if(*buffer == '='){ //"<="
            info_atomo.atomo = MENOR_IGUAL;
            buffer+=2;
        }
        else{ //"<"
            info_atomo.atomo = MENOR;
            buffer++;
        }
    }else if (*buffer == '=') {
        info_atomo.atomo = IGUAL;
        buffer++;
    }else if (*buffer == ':') {
        info_atomo.atomo = DOIS_PONTOS;
        buffer++;
    }else if (*buffer == '+') {
        info_atomo.atomo = OP_SOMA;
        buffer++;
    } else if (*buffer == '-') {
        info_atomo.atomo = OP_SUB;
        buffer++;
    } else if (*buffer == '*') {
        info_atomo.atomo = OP_MULT;
        buffer++;
    }else if (*buffer == '/') {
        if(*buffer == '='){ // "/="
            info_atomo.atomo = DIVIDIDO_IGUAL;
            buffer+=2;
        }
        else{ // "/"
            info_atomo.atomo = OP_DIV;
            buffer++;
        }
    }else if (*buffer == ';') {
        info_atomo.atomo = PONTO_VIRGULA;
        buffer++;
    }else if (*buffer == '.') {
        info_atomo.atomo = PONTO;
        buffer++;
    }else if (*buffer == ',') {
        info_atomo.atomo = VIRGULA;
        buffer++;
    }else if (*buffer == '(') {
        info_atomo.atomo = ABRE_PARENTESES;
        buffer++;
    }else if (*buffer == ')') {
        info_atomo.atomo = FECHA_PARENTESES;
        buffer++;
    }else if (*buffer == '\0') {
        info_atomo.atomo = EOS;
        printf("cai aqui");
    } else {
        info_atomo.atomo = ERRO;
    }

    info_atomo.linha = contaLinha;
    if( info_atomo.atomo == IDENTIFICADOR)
        printf("# %d: %s | %s\n",info_atomo.linha,msgAtomo[info_atomo.atomo], info_atomo.atributo_ID);
    else if(info_atomo.atomo == NUMERO)
        printf("# %d: %s | %d\n",info_atomo.linha,msgAtomo[info_atomo.atomo], info_atomo.atributo_numero);
    else
        printf("# %d: %s\n",info_atomo.linha,msgAtomo[info_atomo.atomo]);
    return info_atomo;
}

void ignora_comentario() {
    while (!(*buffer == '-' && *(buffer + 1) == '}') && *buffer != '\0') {
        if (*buffer == '\n') contaLinha++;
        buffer++;
    }
    if (*buffer == '-' && *(buffer + 1) == '}') {
        buffer += 2;
    }
}

// IDENTIFICADOR -> letra(letra|digito|_)*
TInfoAtomo reconhece_id() {
    TInfoAtomo info_atomo;
    info_atomo.atomo = ERRO;
    char *iniID = buffer;
    int tamanho = 0;

    buffer++;
    tamanho++;

    while (islower(*buffer) || isdigit(*buffer) || *buffer == '_') {
        buffer++;
        tamanho++;
        if (tamanho > MAX_ID_LENGTH) {
            info_atomo.atomo = ERRO;
            return info_atomo;
        }
    }

    strncpy(info_atomo.atributo_ID, iniID, tamanho);
    info_atomo.atributo_ID[tamanho] = '\0';

    int indice_palavra_reservada = eh_palavra_reservada(info_atomo.atributo_ID);
    if (indice_palavra_reservada != -1) {
        info_atomo.atomo = atomos_reservados[indice_palavra_reservada];
    } else {
        info_atomo.atomo = IDENTIFICADOR;
    }

    return info_atomo;
}

int eh_palavra_reservada(char *lexema) {
    for (int i = 0; i < sizeof(palavras_reservadas) / sizeof(char *); i++) {
        if (strcmp(lexema, palavras_reservadas[i]) == 0) {
            return i;
        }
    }
    return -1;
}

// NUMERO -> 0b(0|1)+
TInfoAtomo reconhece_num() {
    TInfoAtomo info_atomo;
    info_atomo.atomo = ERRO;

    buffer += 2; 

    char binario[33];
    int tamanho = 0;

    while (*buffer == '0' || *buffer == '1') {
        if (tamanho < 32) {
            binario[tamanho++] = *buffer;
        }
        buffer++;
    }
    binario[tamanho] = '\0';

    info_atomo.atributo_numero = converte_binario_para_decimal(binario);
    info_atomo.atomo = NUMERO;

    return info_atomo;
}

int converte_binario_para_decimal(const char *binario) {
    return (int)strtol(binario, NULL, 2);
}
//#########################
// FIM: LEXICO

//#########################
// INICIO: SINTATICO
void consome(TAtomo atomo){
    if(lookahead==atomo){
        info_atomo = obter_atomo();
        lookahead=info_atomo.atomo;
    }
    else{
        printf("# %d:Erro sintatico, esperado [%s] encontrado [%s] \n",info_atomo.linha,msgAtomo[atomo],msgAtomo[lookahead]);
        exit(0);
    }
}

// <programa> ::= program identificador “;” <bloco> “.” 👍
void programa(){
    consome(PROGRAM);
    consome(IDENTIFICADOR);
    consome(PONTO_VIRGULA);
    bloco();
    consome(PONTO);
}

//<bloco>::= <declaracao_de_variaveis> <comando_composto> 👍
void bloco(){
    declaracao_de_variaveis();
    comando_composto();
}

//<declaracao_de_variaveis> ::= {<tipo> <lista_variavel> “;”} 👍
void declaracao_de_variaveis(){
    //verificar {}
    while(lookahead == INTEGER || lookahead == BOOLEAN){
        tipo();
        lista_variavel();
        consome(PONTO_VIRGULA);
    }
}

//<tipo> ::= integer | boolean 👍
void tipo(){
    switch (lookahead)
    {
    case INTEGER:
        consome(INTEGER);
        break;
    default:
        consome(BOOLEAN);
        break;
    }
}

//<lista_variavel> ::= identificador { “,” identificador } 👍
void lista_variavel(){
    consome(IDENTIFICADOR);
    //verificar {}
    while(lookahead == VIRGULA){
        consome(VIRGULA);
        consome(IDENTIFICADOR);
    }
}

//<comando_composto> ::= begin <comando> {“;”<comando>} end 👍
void comando_composto(){
    consome(BEGIN);
    comando();
    while (lookahead == PONTO_VIRGULA)
    {
        consome(PONTO_VIRGULA);
        comando();
    }
    consome(END);
}

/*<comando> ::= <comando_atribuicao> |
<comando_condicional> |
<comando_repeticao> |
<comando_entrada> |
<comando_saida> |
<comando_composto> 👍*/
void comando(){
    switch (lookahead)
    {
    case SET:
        comando_atribuicao();
        break;
    case IF:
        comando_condicional();
        break;
    case FOR:
        comando_repeticao();
        break;
    case READ:
        comando_entrada();
        break;
    case WRITE:
        comando_saida();
        break;
    default:
        comando_composto();
        break;
    }
}

//<comando_atribuicao> ::= set identificador to <expressao> 👍
void comando_atribuicao(){
    consome(SET);
    consome(IDENTIFICADOR);
    consome(TO);
    expressao();
}

//<comando_condicional> ::= if <expressao> “:”<comando> [elif <comando>] 👍
void comando_condicional(){
    consome(IF);
    expressao();
    consome(DOIS_PONTOS);
    comando();
    if(lookahead == ELIF){
        consome(ELIF);
        comando();
    }
}

//<comando_repeticao> ::= for identificador of <expressão> to <expressão> “:” <comando> 👍
void comando_repeticao(){
    consome(FOR);
    consome(IDENTIFICADOR);
    consome(OF);
    expressao();
    consome(TO);
    expressao();
    consome(DOIS_PONTOS);
    comando();
}

//<comando_entrada> ::= read “(“ <lista_variavel> “)”👍
void comando_entrada(){
    consome(READ);
    consome(ABRE_PARENTESES);
    lista_variavel();
    consome(FECHA_PARENTESES);
}

//<comando_saida> ::= write “(“ <expressao> { “,” <expressao> } “)” 👍
void comando_saida(){
    consome(WRITE);
    consome(ABRE_PARENTESES);
    expressao();
    while (lookahead == VIRGULA)
    {
        consome(VIRGULA);
        expressao();
    }
    consome(FECHA_PARENTESES);
}

//<expressao> ::= <expressao_logica> { or <expressao_logica> } 👍
void expressao(){
    expressao_logica();
    while (lookahead == OR)
    {
        consome(OR);
        expressao_logica();
    }
    
}

//<expressao_logica>::= <expressao_relacional> { and <expressao_relacional> } 👍
void expressao_logica(){
    expressao_relacional();
    while (lookahead ==  AND)
    {
        consome(AND);
        expressao_relacional();
    }
}

//<expressão_relacional> ::= <expressao_simples> [<op_relacional> <expressao_simples>] 👍
void expressao_relacional(){
    expressao_simples();
    if(lookahead == MAIOR || lookahead == IGUAL || lookahead == MENOR || lookahead == MAIOR_IGUAL || lookahead == DIVIDIDO_IGUAL || lookahead == MENOR_IGUAL){
        op_relacional();
        expressao_simples();
    }
}

//<op_relacional> ::= “<” | “<=” | “=” | “/=” | “>” | “>=” 👍
void op_relacional(){
    switch (lookahead)
    {
    case MAIOR:
        consome(MAIOR);
        break;
    case MENOR:
        consome(MENOR);
        break;
    case IGUAL:
        consome(IGUAL);
        break;
    case MAIOR_IGUAL:
        consome(MAIOR_IGUAL);
        break;
    case DIVIDIDO_IGUAL:
        consome(DIVIDIDO_IGUAL);
        break;
    default:
        consome(MENOR_IGUAL);
        break;
    }
}

//<expressao_simples> ::= <termo> { (“+” | “−” ) <termo> } 👍
void expressao_simples(){
    termo();
    while(lookahead == OP_SOMA || lookahead == OP_SUB){
        consome(lookahead);
        termo();
    }
}

//<termo> ::= <fator> { ( “*” | “/” ) <fator> } 👍
void termo(){
    fator();
    while(lookahead == OP_MULT || lookahead == OP_DIV){
        consome(lookahead);
        fator();
    }
}

/*
<fator> ::= identificador |
numero |
true |
false |
not <fator> |
“(“ <expressao> “)” 👍
*/ 
void fator(){
    switch (lookahead)
    {
    case IDENTIFICADOR:
        consome(IDENTIFICADOR);
        break;
    case NUMERO:
        consome(NUMERO);
        break;
    case TRUE:
        consome(TRUE);
        break;
    case FALSE:
        consome(FALSE);
        break;
    case NOT:
        consome(NOT);
        fator();
        break;
    default:
        consome(ABRE_PARENTESES);
        expressao();
        consome(FECHA_PARENTESES);
        break;
    }
}
// E::=numero|identificador|+EE|*EE
void E(){
    switch(lookahead){
        case OP_SOMA:
            consome(OP_SOMA);
            E();E();
            break;
        case OP_MULT:
            consome(OP_MULT);
            E();E();
            break;
        case IDENTIFICADOR:
            consome(IDENTIFICADOR);
            break;
        default:
            consome(NUMERO);
    }
}

//#########################
// FIM: SINTATICO
