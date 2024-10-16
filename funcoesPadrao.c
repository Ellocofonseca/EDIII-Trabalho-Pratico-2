#include "./funcoesCriadas.h"
#include "./funcoesArvB.h"

// funcao que direciona o codigo para cada funcao de acordo com o numero selecionado

void direcionaComando(int codigo)
{
    switch (codigo)
    {

    case 1:
        csv_para_bin();
        break;

    case 2:
        exibe_campos();
        break;

    case 3:
        busca_determinada();
        break;

    case 4:
        remocao_logica();
        break;

    case 5:
        insere_registro();
        break;

    case 6:
        compactador();
        break;

    case 7:
        cria_arq_indices();
        break;

    case 8:
        pesquisa();
        break;

    case 9:
        insere_novo_dado();
        break;

    default:
        printf(ERRO_PADRAO); // PRINT DA MENSAGEM PADRAO DE ERRO CASO O USUARIO DIGITE UM COMANDO INEXISTENTE
        break;
    }
}

// funcao que compara uma string dada com nomes de campos existentes, se a string dada nao for igual a nada retorna -1

int checa_nome_campo(char *string)
{

    if (!strcmp(string, "nome"))
    {
        return 1;
    }
    if (!strcmp(string, "especie"))
    {
        return 2;
    }
    if (!strcmp(string, "habitat"))
    {
        return 3;
    }
    if (!strcmp(string, "tipo"))
    {
        return 4;
    }
    if (!strcmp(string, "dieta"))
    {
        return 5;
    }
    if (!strcmp(string, "alimento"))
    {
        return 6;
    }
    if (!strcmp(string, "unidade"))
    {
        return 7;
    }
    if (!strcmp(string, "velocidade"))
    {
        return 8;
    }
    if (!strcmp(string, "tamanho"))
    {
        return 9;
    }
    if (!strcmp(string, "populacao"))
    {
        return 10;
    }
    return -1;
}
// funcao utilizada no comando 5 que le do teclado dados de um registro que sera inserido num arquivo binario
// caso os campos de insercao recebam a string NULO o dado recebe um valor invalido padrao
//  -1 para int e float, $ para char e "" para string
dados le_do_teclado()
{
    // nome, dieta, habitat, populacao, tipo, velocidade, medidaVelocidade, tamanho, especie, alimento.
    dados DADO;
    int j;
    // strings que formarao o campo de tamanho variavel do registro de especie
    char limitador[2] = "#";
    char nome[41];
    char especie[41];
    char habitat[41];
    char tipo[41];
    char dieta[41];
    char alimento[41];

    char populacao[31];
    char velocidade[31];
    char medVelocidade[31];
    char tamanho[31];
    char var[142];

    // leitura e modificacao dos campos do registro
    scan_quote_string(nome);
    scan_quote_string(dieta);
    scan_quote_string(habitat);
    scan_quote_string(populacao);
    scan_quote_string(tipo);
    scan_quote_string(velocidade);
    scan_quote_string(medVelocidade);
    scan_quote_string(tamanho);
    scan_quote_string(especie);
    scan_quote_string(alimento);

    if ((strlen(nome) + strlen(dieta) + strlen(habitat) + strlen(especie) + strlen(tipo) + strlen(alimento) + 6) > 142)
    {
        DADO.removido == '3'; // INDICA QUE O USUARIO FOI MENINO NOVO E COLOCOU MUITOS CARATERES PARA ENTRAR NO CAMPO DE TAMANHO VARIAVEL
                              // OU EM OUTRO CAMPO

        printf(ERRO_PADRAO " (Muitos caracteres no campo variavel)");
    }
    else
    {

        // STRING
        if (!strcmp(nome, ""))
        {
            nome[0] = '\0';
        }
        if (!strcmp(dieta, ""))
        {
            dieta[0] = '\0';
        }
        if (!strcmp(habitat, ""))
        {
            habitat[0] = '\0';
        }
        if (!strcmp(tipo, ""))
        {
            tipo[0] = '\0';
        }
        if (!strcmp(especie, ""))
        {
            especie[0] = '\0';
        }
        if (!strcmp(alimento, ""))
        {
            alimento[0] = '\0';
        }

        // INT CHAR FLOAT
        if (!strcmp(populacao, ""))
        {
            DADO.populacao = -1;
        }
        else
        {
            DADO.populacao = atoi(populacao);
        }

        if (!strcmp(tamanho, ""))
        {
            DADO.tamanho = -1;
        }
        else
        {
            DADO.tamanho = atof(tamanho);
        }

        if (!strcmp(velocidade, ""))
        {
            DADO.velocidade = -1;
        }
        else
        {
            DADO.velocidade = atoi(velocidade);
        }

        if (!strcmp(medVelocidade, ""))
        {
            DADO.unidadeMedida = '$';
        }
        else
        {
            DADO.unidadeMedida = medVelocidade[0];
        }

        DADO.encadeamento = -1;
        DADO.removido = '0';

        // montagem do campo de tamanho variavel
        var[0] = '\0';
        strcat(var, nome);
        strcat(var, limitador);
        strcat(var, especie);
        strcat(var, limitador);
        strcat(var, habitat);
        strcat(var, limitador);
        strcat(var, tipo);
        strcat(var, limitador);
        strcat(var, dieta);
        strcat(var, limitador);
        strcat(var, alimento);
        strcat(var, limitador);

        for (j = strlen(var); j < 142; j++) // coloca o cifrao no lugar dos espacos em branco
            var[j] = '$';

        strcpy(DADO.variavel, var); // coloca a string montada na variavel de registro
    }
    return DADO;
}
