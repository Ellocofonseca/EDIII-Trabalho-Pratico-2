#include "./funcoesArvB.h"
#include "./funcoesCriadas.h"

void insere_novo_dado()
{

    int topo, aux;    // variavel do endereco do topo da pilha de remocao e seu auxiliar que deixa salvo o topo anterior
    int nroInsercoes; // variavel do nro de insercoes que serao feitas no arquivo
    int RRNFinal;     // valor do ultimo rrn do arquivo
    int i;            // auxiliar
    dados DADO;       // variavel de registro
    cabecalho CAB;    // variavel de cabecalho

    char nome_arqdados[31];
    char nome_arqindices[31];

    // variaveis de cabecalho e de dado de cada tipo de arquivo

    CabecalhoArvB CAB_ARVB;
    No NO;
    Pesquisa PESQ;

    // variaveis do rrn de cada registro do arquivo de dados e para a criacao da chave do dado
    int RRN;
    long byteoffset;
    char *linha;
    char *nome;
    long chave;

    scanf("%s", nome_arqdados);
    scanf("%s", nome_arqindices);

    FILE *arquivodados;
    FILE *arquivoindices;

    arquivodados = fopen(nome_arqdados, "rb+");      // abre o arquivo de dados em modo leitura e escrita
    arquivoindices = fopen(nome_arqindices, "rb+");  // abre o arquivo de dados em modo leitura e escrita

    if (arquivodados == NULL || arquivoindices == NULL)
    { // checa a existencia dos arquivos, se um nao existir retorna erro e fecha um possivel arquivo existente
        if (arquivodados != NULL)
            fclose(arquivodados);
        if (arquivoindices != NULL)
            fclose(arquivoindices);
        printf(ERRO_PADRAO);
        return;
    }

    CAB = le_cabecalho(arquivodados); // le os cabecalhos de ambos os arquivos
    CAB_ARVB = le_cab_arvb(arquivoindices);

    if (CAB_ARVB.status == '0' || CAB.status == '0')
    { // checa os status dos arquivos, se um deles estiver inconsistente retorna erro e fecha os arquivos
        fclose(arquivodados);
        fclose(arquivoindices);
        printf(ERRO_PADRAO);
        return;
    }

    CAB.status = '0';
    atualiza_cabecalho_bin(nome_arqdados, CAB); // coloca status 0 antes de começar a editar o arquivo
    fseek(arquivodados, 1600, SEEK_SET);

    CAB_ARVB.status='0';
    fseek(arquivoindices,0,SEEK_SET);
    escreve_cabecalho_arvb(arquivoindices,CAB_ARVB);


    topo = CAB.topo;        // guarda o valor de topo da pilha para remocao
    RRNFinal = CAB.proxRRN; // guarda o valor da qtd de dados

    scanf("%d", &nroInsercoes); // quantidade de dados que serao inseridos

    for (i = 0; i < nroInsercoes; i++)
    {

        // HORA DA INSERCAO DO DADO
        if (topo == -1)
        {
            fclose(arquivodados); // fecha o arquivo para abrir em modo append

            DADO = le_do_teclado(); // le os dados que serao inseridos
            
            linha = strdup(DADO.variavel);  // separa o campo do nome
            nome = strsep(&linha, "#");
            chave=converteNome(nome);       //cria a chave com a funcao fornecida
            byteoffset = (long)(1600 + RRNFinal*160);     //calcula o byteoffset relativo ao registro que tera a chave inserida

            if (DADO.removido != '3') // DADO.removido usado como flag, se houver uma inconsistencia na leitura de dados eles nao sao inseridos
            {
                escreve_dado_bin(nome_arqdados, DADO); // escreve no fim do arquivo
                
                PESQ=insere_chave(chave,byteoffset,arquivoindices,CAB_ARVB.noRaiz,CAB_ARVB.noRaiz);

                RRNFinal++;                            // posicao do ultimo rrn do arquivo eh atualizada
                CAB_ARVB.noRaiz=PESQ.nova_raiz;         //atualiza a raiz caso ela tenha sofrido split
                CAB_ARVB.RRNproxNo+=PESQ.nova_pagina;   //aumenta o rrn do proximo no caso tenham ocorrido splits
            }

            arquivodados = fopen(nome_arqdados, "rb+"); // abre o arquivo de novo em modo rb+ para pesquisar e editar o arquivo
        }
        else
        {

            fseek(arquivodados, 1600 + (160 * topo), SEEK_SET); // fseek na posicao com remocao
            byteoffset = (long)(1600 + (topo)*160);     //calcula o byteoffset relativo ao registro que tera a chave inserida
            DADO = le_registro(arquivodados);                   // le a posicao para armazenar o valor de topo
            aux = topo;                                         // salva o topo antigo para caso aconteca um problema na leitura de dados do usuario
            topo = DADO.encadeamento;                           // armazena o valor de topo novo
            DADO = le_do_teclado(); // le os dados que serao inseridos

            linha = strdup(DADO.variavel);  // separa o campo do nome
            nome = strsep(&linha, "#");
            chave=converteNome(nome);       //cria a chave com a funcao fornecida
            

            if (DADO.removido != '3') // DADO.removido usado como flag, se houver uma inconsistencia na leitura de dados eles nao sao inseridos
            {
                fseek(arquivodados, -160, SEEK_CUR);   // volta o ponteiro para realizar a insercao
                atualiza_dado_bin(DADO, arquivodados); // escreve a nova informacao por cima do local onde havia uma remocao

                PESQ=insere_chave(chave,byteoffset,arquivoindices,CAB_ARVB.noRaiz,CAB_ARVB.noRaiz);

                CAB_ARVB.noRaiz=PESQ.nova_raiz;         //atualiza a raiz caso ela tenha sofrido split
                CAB_ARVB.RRNproxNo+=PESQ.nova_pagina;   //aumenta o rrn do proximo no caso tenham ocorrido splits
                CAB.nroRegRem--;                       // nro de registros removidos diminui
            }
            else
            {
                topo = aux; // volta com o valor de topo anterior caso o usuario faca algo errado
            }
        }
    }
    fclose(arquivodados);

    fseek(arquivoindices,0,SEEK_SET);
    CAB_ARVB.status='1';
    escreve_cabecalho_arvb(arquivoindices,CAB_ARVB);//atualiza o cabecalho do arquivo de indices

    fclose(arquivoindices);

    // ATUALIZA AS INFORMACOES DO CABECALHO
    if (CAB.nroRegRem < 0)
        CAB.nroRegRem = 0;

    CAB.status = '1';

    CAB.proxRRN = RRNFinal;

    CAB.nroPagDisco = (1 + (RRNFinal / 10));
    if (CAB.nroPagDisco * 10 < RRNFinal + 10)
        CAB.nroPagDisco++;

    CAB.topo = topo;

    atualiza_cabecalho_bin(nome_arqdados, CAB);



    binarioNaTela(nome_arqindices);
}
