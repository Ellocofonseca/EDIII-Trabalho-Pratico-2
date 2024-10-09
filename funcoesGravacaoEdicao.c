#include "./funcoesCriadas.h"

// funcao que abre um arquivo csv para realizar a leitura dos dados dele para que esses dados sejam salvos
// em um arquivo binario em formato especificado alem de seu cabecalho que tambem eh salvo
void csv_para_bin()
{
    int i, j; // variavel auxiliar
    int insercoes;
    char nomecsv[31]; // nome do arquivo csv que sera aberto
    char nomebin[31]; // nome do arquivo bin que sera criado
    dados REGISTRO;   // variavel de registro
    cabecalho CAB;    // variavel de cabecalho

    char *linha; // variaveis que serao usadas para ler e manipular cada linha do csv
    char tmp[200];

    // tokens/strings que formarao o campo de tamanho variavel do registro de especie
    char *token; // usado para coletar os valores numericos(int e float)
    char limitador[2] = "#";
    char *nome;
    char *especie;
    char *habitat;
    char *tipo;
    char *dieta;
    char *alimento;
    char var[142];

    scanf("%s", nomecsv);
    scanf("%s", nomebin);

    CAB.status = '0';
    insercoes = 0;

    escreve_cabecalho_bin(nomebin, CAB); //'reserva o espaco do cabecalho e marca a flag status='0'

    // LER DO CSV, IR GRAVANDO OS REGISTROS E ATUALIZANDO A STRUCT DO CABECALHO

    // nome,dieta,habitat,populacao,tipo,velocidade,unidadeMedida,tamanho,especie,alimento
    FILE *arqcsv;
    arqcsv = fopen(nomecsv, "r");
    if (arqcsv == NULL)
    {
        printf(ERRO_PADRAO);
    }
    else
    {
        fgets(tmp, 200, arqcsv);        // le e "descarta" a primeira linha
        while (fgets(tmp, 200, arqcsv)) // le as linhas subsequentes ate chegar no fim do arquivo
        {
            linha = strdup(tmp);
            // leitura de cada dado de uma linha do csv
            nome = strsep(&linha, ",");

            dieta = strsep(&linha, ",");

            habitat = strsep(&linha, ",");

            token = strsep(&linha, ","); // populacao
            REGISTRO.populacao = atoi(token);
            if (REGISTRO.populacao == 0) // se o campo estiver vazio coloca o valor invalido -1
                REGISTRO.populacao = -1;

            tipo = strsep(&linha, ",");

            token = strsep(&linha, ","); // velocidade
            REGISTRO.velocidade = atoi(token);
            if (REGISTRO.velocidade == 0) // se o campo estiver vazio coloca o valor invalido -1
                REGISTRO.velocidade = -1;

            token = strsep(&linha, ","); // unidMedida
            REGISTRO.unidadeMedida = token[0];
            if (!strcmp(token, "")) // se o campo estiver vazio coloca o valor invalido $
                REGISTRO.unidadeMedida = '$';

            token = strsep(&linha, ","); // tamanho
            REGISTRO.tamanho = atof(token);
            if (fabs(0 - REGISTRO.tamanho) < 0.001) // se o campo estiver vazio coloca o valor invalido -1
                REGISTRO.tamanho = -1;

            especie = strsep(&linha, ",");

            alimento = strsep(&linha, "\r");

            REGISTRO.encadeamento = -1;
            REGISTRO.removido = '0';

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

            strcpy(REGISTRO.variavel, var); // coloca a string montada na variavel de registro

            for (j = 0; j < 142; j++) // limpa a string montada de tamanho variado
                var[j] = '\0';

            // GUARDA O REGISTRO NO ARQUIVO
            escreve_dado_bin(nomebin, REGISTRO);
            insercoes++;
        }
        fclose(arqcsv); // fecha o csv depois de tudo ter sido lido e guardado
    }

    CAB.topo = -1;
    CAB.proxRRN = insercoes;
    CAB.nroRegRem = 0;
    CAB.nroPagDisco = (1 + (insercoes / 10));
    if (CAB.nroPagDisco * 10 < insercoes + 10)
    {
        CAB.nroPagDisco++;
    }

    CAB.qttCompacta = 0;
    CAB.status = '1';
    atualiza_cabecalho_bin(nomebin, CAB); // atualiza o cabecalho ao final da insercao de todos os dados do csv

    binarioNaTela(nomebin); // binario na tela, resultado
}

// funcao que realiza a remocao logica e encadeia os dados em formato de pilha
// sempre que um dado eh removido ele recebe o valor de topo no encadeamento e seu rrn vira o novo valor de topo atualizado
void remocao_logica()
{

    int nroRemocoes, regRemovidos, encadeamento, topo;
    int i, codigo_campo;
    char nomearq[31];    // nome do arquivo que sera lido
    char nomecampo[21];  // campo que sera checado para pesquisa
    char valorcampo[41]; // valor do campo que sera checado
    int valorint;        // valor em int do campo desejado
    float valorfloat;    // valor em float do campo desejado
    char valorchar;      // valor em char do campo desejado
    dados DADO;          // variavel de registro
    cabecalho CAB;       // variavel do cabecalho

    // ponteiros para uso strdup e strsep para ler a parte variavel do registro de dados
    char *linha;
    char *nome;
    char *especie;
    char *habitat;
    char *tipo;
    char *dieta;
    char *alimento;

    FILE *arquivo;
    scanf("%s", nomearq);            // le o nome do arquivo que sera aberto
    arquivo = fopen(nomearq, "rb+"); // abre o arquivo em modo de leitura binaria + escrita

    if (arquivo == NULL)
    { // checa se o arquivo foi devidamente aberto antes de continuar
        printf(ERRO_PADRAO);
    }
    else
    {
        CAB = le_cabecalho(arquivo); // le o cabecalho e checa se ele esta ok
        if (CAB.status == '0')
        { // se o status n estiver OK nao faz nada no arquivo e fecha ele
            printf(ERRO_PADRAO);
            fclose(arquivo);
        }
        else
        {
            CAB.status = '0'; // se estiver ok atualiza o cabecalho

            fclose(arquivo); // fecha o arquivo apos ler o cabecalho

            atualiza_cabecalho_bin(nomearq, CAB); // atualiza o cabecalho com status 0

            // le o nro de remocoes que serao feitas
            scanf("%d", &nroRemocoes);

            regRemovidos = CAB.nroRegRem; // guarda a qtd de remocoes que foram feitas anteriormente antes de prosseguir
            topo = CAB.topo;              // guarda o RRN do topo da pilha de remocao

            for (i = 0; i < nroRemocoes; i++)
            { // loop das remocoes que serao feitas
                encadeamento = -1;

                arquivo = fopen(nomearq, "rb+"); // abre o arquivo novamente em modo de leitura+escrita

                // coleta qual campo e qual seu valor para realizar as remocoes
                scanf("%s", nomecampo);
                codigo_campo = checa_nome_campo(nomecampo); // FUNCAO QUE TESTA A STRING NOMECAMPO E RETORNA UM INTEIRO DE ACORDO COM O NOME DE CAMPO

                // muda o tipo de leitura baseado no campo especificado
                if (codigo_campo == 9)
                {
                    scanf("%f", &valorfloat);
                }
                else if (codigo_campo == 8 || codigo_campo == 10)
                {
                    scanf("%d", &valorint);
                }
                else if (codigo_campo == 7)
                {
                    scanf("%c", &valorchar);
                }
                else
                {
                    scan_quote_string(valorcampo);
                }

                if (codigo_campo == -1) // se nomecampo for invalido avisa
                    printf(ERRO_COMANDO);

                fseek(arquivo, 1600, SEEK_SET); // ajusta o ponteiro do arquivo para o começo dos dados

                while (codigo_campo != -1) // se nomecampo for invalido nao faz a leitura do arquivo
                {

                    DADO = le_registro(arquivo); // faz a leitura do registro no arquivo binario

                    if (DADO.removido == '2') // a variavel removido eh alterada para '2' dentro da funcao le registro se chegar no fim do arquivo
                        break;

                    encadeamento++; // aumenta o encadeamento (rrn)

                    if (DADO.removido == '0') // entra apenas se uma remocao puder ser feita
                    {

                        linha = strdup(DADO.variavel); // separa os campos de tamanho variado do dado em varias strrings
                        nome = strsep(&linha, "#");
                        especie = strsep(&linha, "#");
                        habitat = strsep(&linha, "#");
                        tipo = strsep(&linha, "#");
                        dieta = strsep(&linha, "#");
                        alimento = strsep(&linha, "#");

                        // ao encontrar um registro que deve ser removido dentro do switch case o seguinte ocorre:
                        // o ponteiro de arquivo volta 160 bytes para que o registro possa ser removido, para isso a
                        // funcao removedadobin eh usada, ela reescreve o registro contendo apenas o status e o encadeamento(topo)
                        // e o resto com $$$$$
                        // depois disso o valor de topo eh atualizado para o valor do rrn do registro que foi removido
                        // por fim a contagem de registros removidos aumenta para a posterior atualizacao do cabecalho

                        switch (codigo_campo) // a depender do tipo de remocao escolhida o switch case age
                        {
                        case 1:
                            if (!strcmp(valorcampo, nome))
                            {
                                fseek(arquivo, -160, SEEK_CUR);
                                remove_dado_bin(arquivo, topo);
                                topo = encadeamento;
                                regRemovidos++;
                            }
                            break;

                        case 2:
                            if (!strcmp(valorcampo, especie))
                            {
                                fseek(arquivo, -160, SEEK_CUR);
                                remove_dado_bin(arquivo, topo);
                                topo = encadeamento;
                                regRemovidos++;
                            }
                            break;

                        case 3:
                            if (!strcmp(valorcampo, habitat))
                            {
                                fseek(arquivo, -160, SEEK_CUR);
                                remove_dado_bin(arquivo, topo);
                                topo = encadeamento;
                                regRemovidos++;
                            }
                            break;

                        case 4:
                            if (!strcmp(valorcampo, tipo))
                            {
                                fseek(arquivo, -160, SEEK_CUR);
                                remove_dado_bin(arquivo, topo);
                                topo = encadeamento;
                                regRemovidos++;
                            }
                            break;

                        case 5:
                            if (!strcmp(valorcampo, dieta))
                            {
                                fseek(arquivo, -160, SEEK_CUR);
                                remove_dado_bin(arquivo, topo);
                                topo = encadeamento;
                                regRemovidos++;
                            }
                            break;

                        case 6:
                            if (!strcmp(valorcampo, alimento))
                            {
                                fseek(arquivo, -160, SEEK_CUR);
                                remove_dado_bin(arquivo, topo);
                                topo = encadeamento;
                                regRemovidos++;
                            }
                            break;

                        case 7:
                            if (valorchar == DADO.unidadeMedida)
                            {
                                fseek(arquivo, -160, SEEK_CUR);
                                remove_dado_bin(arquivo, topo);
                                topo = encadeamento;
                                regRemovidos++;
                            }
                            break;

                        case 8:

                            if (valorint == DADO.velocidade)
                            {
                                fseek(arquivo, -160, SEEK_CUR);
                                remove_dado_bin(arquivo, topo);
                                topo = encadeamento;
                                regRemovidos++;
                            }
                            break;

                        case 9:
                            if (fabs(valorfloat - DADO.tamanho) < 0.001)
                            {
                                fseek(arquivo, -160, SEEK_CUR);
                                remove_dado_bin(arquivo, topo);
                                topo = encadeamento;
                                regRemovidos++;
                            }
                            break;
                        case 10:
                            if (valorint == DADO.populacao)
                            {
                                fseek(arquivo, -160, SEEK_CUR);
                                remove_dado_bin(arquivo, topo);
                                topo = encadeamento;
                                regRemovidos++;
                            }
                            break;
                        default:
                            break;
                        } // FIM DO SWITCH
                    }
                } // FIM DO WHILE
                fclose(arquivo); // fecha o arquivo apos completar um ciclo inteiro de remocao

            } // FIM DO FOR

            CAB.nroRegRem = regRemovidos;
            CAB.topo = topo;
            CAB.status = '1';

            atualiza_cabecalho_bin(nomearq, CAB); // ATUALIZA O CABECALHO COM A NOVA QTD DE REGISTROS LOGICAMENTE REMOVIDOS E COM O RRN DE TOPO NOVO
            binarioNaTela(nomearq);
        }
    }
}

// a funcao a seguir insere registros novos a um arquivo bin ja existente, para fazer isso
// a funcao sobrescreve os dados na ordem da pilha de remocao caso existam arquivos removidos
// se nao houverem arquivos logicamente removidos a insercao e feita de forma normal ao fim
// do arquivo
void insere_registro()
{

    int topo,aux;     // variavel do endereco do topo da pilha de remocao e seu auxiliar que deixa salvo o topo anterior
    int nroInsercoes; // variavel do nro de insercoes que serao feitas no arquivo
    int RRNFinal;     // valor do ultimo rrn do arquivo
    int i;            // auxiliar
    dados DADO;       // variavel de registro
    cabecalho CAB;    // variavel de cabecalho

    char nomebin[31]; // nome do arquivo bin que sera compactado
    char compactado[31] = "compactado.bin";

    FILE *arquivo; // ponteiro do arquivo que sera aberto em modo rb+ para que as modificacoes possam ser feitas

    scanf("%s", nomebin); // scanf do nome do arquivo que sera compactado

    arquivo = fopen(nomebin, "rb+");
    if (arquivo == NULL) // abre o arquivo checando se foi devidamente aberto em modo rb+
    {
        printf(ERRO_PADRAO);
    }
    else
    { // le o cabecalho do arquivo e ajusta o ponteiro para ler os dados em seguida depois de atualizar o cabecalho
        CAB = le_cabecalho(arquivo);
        if (CAB.status == '0')
        { // se o cabecalho tiver status 0 nao faz nada no arquivo e fecha ele
            printf(ERRO_PADRAO);
            fclose(arquivo);
        }
        else
        {
            CAB.status = '0';
            atualiza_cabecalho_bin(nomebin,CAB);//coloca status 0 antes de começar a editar o arquivo

            fseek(arquivo, 1600, SEEK_SET);

            topo = CAB.topo;        // guarda o valor de topo da pilha para remocao
            RRNFinal = CAB.proxRRN; // guarda o valor da qtd de dados

            scanf("%d", &nroInsercoes); // quantidade de dados que serao inseridos

            for (i = 0; i < nroInsercoes; i++)
            {

                // HORA DA INSERCAO DO DADO
                if (topo == -1)
                {
                    fclose(arquivo); // fecha o arquivo para abrir em modo append

                    DADO = le_do_teclado();//le os dados que serao inseridos

                    if (DADO.removido!='3')// DADO.removido usado como flag, se houver uma inconsistencia na leitura de dados eles nao sao inseridos
                    {
                    escreve_dado_bin(nomebin, DADO); // escreve no fim do arquivo
                    RRNFinal++;                      // posicao do ultimo rrn do arquivo eh atualizada
                    } 

                    arquivo = fopen(nomebin, "rb+"); // abre o arquivo de novo em modo rb+ para pesquisar e editar o arquivo
                }
                else
                {

                    fseek(arquivo, 1600 + (160 * topo), SEEK_SET); // fseek na posicao com remocao
                    DADO = le_registro(arquivo);                   // le a posicao para armazenar o valor de topo
                    aux=topo;                                      //salva o topo antigo para caso aconteca um problema na leitura de dados do usuario
                    topo = DADO.encadeamento;                      // armazena o valor de topo novo
                    DADO = le_do_teclado();                        // le os dados que serao inseridos no lugar do registro apagado

                    if (DADO.removido!='3')// DADO.removido usado como flag, se houver uma inconsistencia na leitura de dados eles nao sao inseridos
                    {
                    fseek(arquivo, -160, SEEK_CUR);                // volta o ponteiro para realizar a insercao
                    atualiza_dado_bin(DADO, arquivo);              // escreve a nova informacao por cima do local onde havia uma remocao
                    CAB.nroRegRem--;                               //nro de registros removidos diminui
                    }else{
                        topo=aux;   //volta com o valor de topo anterior caso o usuario faca algo errado
                    }

                }
            }
            fclose(arquivo);

            // ATUALIZA AS INFORMACOES DO CABECALHO
            if (CAB.nroRegRem < 0)
                CAB.nroRegRem = 0;

            CAB.status = '1';

            CAB.proxRRN = RRNFinal;

            CAB.nroPagDisco = (1 + (RRNFinal / 10));
            if (CAB.nroPagDisco * 10 < RRNFinal + 10)
                CAB.nroPagDisco++;

            CAB.topo = topo;

            atualiza_cabecalho_bin(nomebin, CAB);
            binarioNaTela(nomebin);
        }
    }
}

// o compactador quando chamado recebe o nome de um aquivo binario
// esse arquivo eh aberto em modo leitura e seus registros logicamente nao removidos
// e cabecalho sao copiados para um outro arquivo que eh aberto em modo append
// apos o fim do processo o arquivo antigo eh deletado e o novo recebe o nome do antigo
void compactador()
{
    int insercoes; // variavel de proxRRN do arquivo compactado
    dados DADO;    // variavel de registro
    cabecalho CAB; // variavel de cabecalho

    char nomebin[31]; // nome do arquivo bin que sera compactado
    char compactado[31] = "compactado.bin";

    FILE *arqoriginal;   // ponteiro do arquivo original que sera aberto em modo leitura
    FILE *arqcompactado; // ponteiro do arquivo comapctado que sera aberto em modo escrita

    scanf("%s", nomebin); // scanf do nome do arquivo que sera compactado

    arqoriginal = fopen(nomebin, "rb");
    if (arqoriginal == NULL) // abre o arquivo checando se foi devidamente aberto em modo leitura binaria
    {
        printf(ERRO_PADRAO);
    }
    else
    {

        CAB = le_cabecalho(arqoriginal); // le o cabecalho do arquivo original

        if (CAB.status == '0')      //se o cabecalho tiver status 0 nao faz nada no arquivo e printa o erro antes de encerrar o programa
        {
            printf(ERRO_PADRAO);
            fclose(arqoriginal);
        }
        else
        {

            fseek(arqoriginal, 1600, SEEK_SET); // ajusta o ponteiro do arquivo original para ler os registros

            CAB.status = '0';                       // status 0 do cabecalho
            escreve_cabecalho_bin(compactado, CAB); // escreve o cabecalho novo do arquivoi

            while (1)
            {
                DADO = le_registro(arqoriginal); // faz a leitura do registro do arquivo original

                if (DADO.removido == '2') // a variavel removido eh alterada para '2' dentro da funcao le registro se chegar no fim do arquivo
                    break;

                if (DADO.removido == '0')
                { // insere o registro no arquivo compactado se ele nao estiver removido
                    DADO.encadeamento = -1;
                    escreve_dado_bin(compactado, DADO);
                    insercoes++;
                }
            }
            fclose(arqoriginal); // fecha o arquivo original que estava aberto em modo leitura binaria

            // atualiza o cabecalho apos todo o processo
            CAB.status = '1';
            CAB.qttCompacta++;
            CAB.nroRegRem = 0;
            CAB.topo = -1;
            CAB.proxRRN = insercoes;

            CAB.nroPagDisco = (1 + (insercoes / 10));
            if (CAB.nroPagDisco * 10 < insercoes + 10)
            {
                CAB.nroPagDisco++;
            }

            atualiza_cabecalho_bin(compactado, CAB);

            // remove o arquivo antigo e renomeia o arquivo compactado com o nome do antigo
            remove(nomebin);
            rename("compactado.bin", nomebin);

            binarioNaTela(nomebin);
        }
    }
}