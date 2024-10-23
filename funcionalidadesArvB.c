#include "./funcoesArvB.h"
#include "./funcoesCriadas.h"

//FUNCAO 8
//essa funcao realiza a pesquisa de um registro com base no nome da especie desejada
//para isso a pesquisa eh feita em um arquivo de indices em formato arvore-b, assim achando a localizacao
//do registro no arquivo de dados e exibindo esse dado

//a principal funcionalidade desse programa esta na funcao busca_chave(), que se encontra em um loop while
//realizado ate que a busca chegue ou no "fim" da arvore ou na chave desejada

void pesquisa(){
    int rrn_pesquisa;       //nro do rrn da pagina em que uma determinada pesquisa esta sendo feita
    char nome_arqdados[31];     //nomes dos arquivos a serem abertos
    char nome_arqindices[31];

    //variaveis de cabecalho e de dado de cada tipo de arquivo
    cabecalho CAB_DADOS;
    dados     DADO;
    CabecalhoArvB CAB_ARVB;
    No            NO;

    //variaveis que sao usadas para fazer a pesquisa
    Pesquisa PESQUISA;
    char nome[51];
    char campo[21];
    long chave;

    FILE *arquivodados; 
    FILE *arquivoindices;

    scanf("%s",nome_arqdados);
    scanf("%s",nome_arqindices);

    arquivodados = fopen(nome_arqdados, "rb");       //abre o arquivo de dados em modo leitura
    arquivoindices = fopen(nome_arqindices,"rb");    //abre o arquivo de indices em modo leitura

    if(arquivodados==NULL || arquivoindices==NULL){  //checa a existencia dos arquivos, se um nao existir retorna erro e fecha um possivel arquivo existente
        if (arquivodados!=NULL)
            fclose(arquivodados);
        if(arquivoindices!=NULL)
            fclose(arquivoindices);
        printf(ERRO_PADRAO);
        return;
    }

    CAB_DADOS=le_cabecalho(arquivodados);   //le os cabecalhos de ambos os arquivos
    CAB_ARVB=le_cab_arvb(arquivoindices);

    if(CAB_ARVB.status=='0' || CAB_DADOS.status=='0'){  //checa os status dos arquivos, se um deles estiver inconsistente retorna erro e fecha os arquivos
        fclose(arquivodados);
        fclose(arquivoindices);
        printf(ERRO_PADRAO);
        return;
    }

    scanf("%s",campo);              //pega o campo "nome"
    scan_quote_string(nome);        //pega o nome da especie

    if(checa_nome_campo(campo)!=1 || !strcmp(nome,"")){     //se o campo de pesquisa nao for "nome" nao executa a funcao (ou se o nome for invalido)
        printf(ERRO_PADRAO);
        fclose(arquivodados);
        fclose(arquivoindices);  
        return;
    }

    chave=converteNome(nome);       //cria a chave com o nome da especie

    rrn_pesquisa=CAB_ARVB.noRaiz;   //pesquisa comeca pelo no raiz
    while (rrn_pesquisa!=-1 || PESQUISA.encontrado==1)        //se o no da pesquisa chegar ao fim e nao tiver encontrado a chave, sai do loop
    {
        
        PESQUISA=busca_chave(rrn_pesquisa,chave,arquivoindices);   //faz a pesquisa na pagina atual
        
        if(PESQUISA.encontrado==1)  //se encontrou a chave sai do loop
            break;

        rrn_pesquisa=PESQUISA.RRN_pag;//se nao tiver encontrado na pagina atual pula para a proxima pagina indicada na pesquisa
    }
    fclose(arquivoindices);

    if(rrn_pesquisa==-1){       //se nao tiver encontrado o registro printa que nao encontrou
        printf(ERRO_REGISTRO);
    }

    if(PESQUISA.encontrado==1){ //se tiver encontrado o registro, fseek na posicao dentro do arquivo de dados, le o dado e exibe ele
        fseek(arquivodados,PESQUISA.BYOFF_dado,SEEK_SET);
        DADO=le_registro(arquivodados);                     //as duas funcoes sao do trabalho anterior
        exibe_registro(DADO);
        fclose(arquivodados);                               //fecha o arquivo de dados
    }
    
}


//FUNCAO 7
//essa funcao realiza a insercao de diversas chaves (em um arquivo de indices em arvore B)
//de acordo com dados existentes em um arquivo de dados determinado no formato no ultimo trabalho de ED3. 

//a arvore B possui ordem 5 como especificado no trabalho e possui as rotinas de insercao "normal" e com "split"
//da mesma forma que aprendemos em aula

//as funcionalidades da insercao/criacao no/do arquivo de indices em arvore B estao no arquivo funcaoInsercaoARVB.c
//as chamadas de insercao foram feitas de forma recursiva sem um uso de buffer para armazenar as chamadas de pesquisa

void cria_arq_indices(){
    //nomes dos arquivos a serem abertos/criados
    char nome_arqdados[31];
    char nome_arqindices[31];
    //variavel de pesquisa
    Pesquisa PESQ;

    //variaveis de cabecalho e de dado de cada tipo de arquivo
    cabecalho CAB_DADOS;
    dados     DADO;
    CabecalhoArvB CAB_ARVB;
    No            NO;

    //variaveis do rrn de cada registro do arquivo de dados e para a criacao da chave do dado
    int RRN;
    long byteoffset;
    char *linha;
    char *nome;
    long chave;

    //ponteiros dos dois arquivos
    FILE *arquivodados;
    FILE *arquivoindices;

    //recebe o nome dos arquivos que serao abertos
    scanf("%s",nome_arqdados);
    scanf("%s",nome_arqindices);

    arquivodados = fopen(nome_arqdados, "rb");   //abre o arquivo de dados em modo leitura

    if(arquivodados==NULL){  //termina a execucao se o arquivo de dados nao existir
        printf(ERRO_PADRAO);
        return;
    }

    CAB_DADOS = le_cabecalho(arquivodados); //le o cabecalho do arquivo de dados
    fseek(arquivodados,1600,SEEK_SET);      //pula para o inicio dos registros do arquivo de dados

    if(CAB_DADOS.status=='0'){              //se o arquivo estiver inconsistente nao executa a funcao e fecha o arquivo
        fclose(arquivodados);
        printf(ERRO_PADRAO);
        return;
    }
    
    arquivoindices = fopen(nome_arqindices,"wb+");   
    //cria o arquivo de indices em arvore-b, aberto em modo wb+ para realizar todas as operacoes
    //aberto apenas essa vez com wb+ para evitar abrir e fechar o arquivo varias vezes

    //cria o cabecalho do arquivo de indices em arvore B, com os dados inicializados com os valores padrao e status=0
    CAB_ARVB.status='0';
    CAB_ARVB.noRaiz=-1;
    CAB_ARVB.RRNproxNo=0;
    escreve_cabecalho_arvb(arquivoindices,CAB_ARVB);//escreve o cabecalho do arquivo

    //loop de leitura dos registros do arquivo de dados e insercao das chaves no arq de indices
    RRN=-1; //inicializa a variavel RRN, ela eh usada para calcular o byteoffset que sera inserido no arquivo de indices junto da chave

    while(1){

        DADO=le_registro(arquivodados); //le o dado do arquivo de dados e atualiza o RRN relativo a esse dado
        RRN++;                                          
        if(DADO.removido=='2')          //sai do loop de leitura de dados caso chegue no fim do arquivo de dados, usa a variavel DADO.removido como flag dentro da funcao le_registro
            break;
        if(DADO.removido=='1')          //se o dado lido estiver logicamente removido volta para o começo do loop para ler o proximo registro
            continue;

        linha = strdup(DADO.variavel);  // separa o campo do nome
        nome = strsep(&linha, "#");

        chave=converteNome(nome);                   //cria a chave com a funcao fornecida

        byteoffset = (long)(1600 + RRN*160);        //calcula o byteoffset relativo ao registro que tera a chave inserida

        //CRIA A ARVORE B

        if(CAB_ARVB.noRaiz==-1){                                //caso ainda nao exista raiz no arquivo de indices
            fseek(arquivoindices,93,SEEK_SET);                  //fseek na posicao de inicio das paginas
            cria_raiz(arquivoindices);                          //cria a raiz com a funcao indicada, a pagina consta com todos os valores iniciados como padrao
            CAB_ARVB.noRaiz=0;                                  //atualiza o rrn da raiz
            CAB_ARVB.RRNproxNo=1;                               //atualiza o rrn do proximo no disponivel
            fseek(arquivoindices,0,SEEK_SET);                   //fseek para atualizar o cabecalho
            escreve_cabecalho_arvb(arquivoindices,CAB_ARVB);    //atualiza o cabecalho
        }

        //chamada da funcao recursiva de insercao de uma chave, os parametros sao:
        //a chave que se deseja inserir e seu byteoffset do arquivo de dados;
        //o valor do rrn da primeira pagina da pesquisa, que no caso eh sempre pela raiz (esse valor muda com o decorrer da recursao);
        //o valor do rrn da raiz em si para saber se o split foi na raiz caso ocorra.
        PESQ=insere_chave(chave,byteoffset,arquivoindices,CAB_ARVB.noRaiz,CAB_ARVB.noRaiz);


        CAB_ARVB.noRaiz=PESQ.nova_raiz;         //atualiza a raiz caso ela tenha sofrido split
        CAB_ARVB.RRNproxNo+=PESQ.nova_pagina;   //aumenta o rrn do proximo no caso tenham ocorrido splits

    }
    fclose(arquivodados);   //fecha o arquivo de dados apos ter terminado de ler tudo dele e feito as insercoes

    //ATUALIZACAO DO CABECALHO DO ARQUIVO DE INDICES
    fseek(arquivoindices,0,SEEK_SET);                   //fseek para atualizar o cabecalho
    CAB_ARVB.status='1';                                //muda o status
    escreve_cabecalho_arvb(arquivoindices,CAB_ARVB);    //atualiza o cabecalho do arquivo

    fclose(arquivoindices);                             //fecha o arquivo
    binarioNaTela(nome_arqindices);                     //binario da tela mostrando o resultado
}

//FUNCAO 9
//essa funcao realiza a insercao tanto de chaves quanto de indices em dois arquivos distintos
//para tal as operacoes da funcao 5 foram reutilizadas para a insercao no arquivo de dados

//as insercoes de dados sao iguais ao trabalho passado, a unica diferenca eh que nessa funcao
//a insercao da chave do dado ocorre tambem em conjunto em um arquivo de indices que ja existe previamente
//assim nenhum arquivo eh criado nessa funcao, apenas sao editados

//a parte "relevante" e diferente da funcao esta destacada no codigo na hora da insercao dos dados assim:

/////////////////////////////////////////
//CODIGO NOVO
////////////////////////////////////////

void insere_novo_dado()
{
                        // variaveis de insercao do trabalho passado
    int topo, aux;      // variavel do endereco do topo da pilha de remocao e seu auxiliar que deixa salvo o topo anterior
    int nroInsercoes;   // variavel do nro de insercoes que serao feitas no arquivo
    int RRNFinal;       // valor do ultimo rrn do arquivo
    int i;              // auxiliar
    dados DADO;         // variavel de registro
    cabecalho CAB;      // variavel de cabecalho

    //nomes dos arquivos que serao modificados
    char nome_arqdados[31];
    char nome_arqindices[31];

    // variaveis de cabecalho e de pagina do arquivo de indices
    CabecalhoArvB CAB_ARVB;
    No NO;
    // variavel de pesquisa
    Pesquisa PESQ;

    // variaveis do rrn de cada registro do arquivo de dados e para a criacao da chave do dado
    int RRN;
    long byteoffset;
    char *linha;
    char *nome;
    long chave;
    //ponteiros dos arquivos
    FILE *arquivodados;
    FILE *arquivoindices;

    //leitura dos nomes dos arquivos
    scanf("%s", nome_arqdados);
    scanf("%s", nome_arqindices);


    arquivodados = fopen(nome_arqdados, "rb+");      // abre o arquivo de dados em modo leitura e escrita, para ler o cabecalho e editar ele no incio
    arquivoindices = fopen(nome_arqindices, "rb+");  // abre o arquivo de dados em modo leitura e escrita, para realizar todas as operacoes pertinentes e fecha-lo apenas no fim


    // checa a existencia dos arquivos, se um nao existir retorna erro e fecha um possivel arquivo existente
    if (arquivodados == NULL || arquivoindices == NULL)
    { 
        if (arquivodados != NULL)
            fclose(arquivodados);
        if (arquivoindices != NULL)
            fclose(arquivoindices);
        printf(ERRO_PADRAO);
        return;
    }
    // le os cabecalhos de ambos os arquivos
    CAB = le_cabecalho(arquivodados); 
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

    CAB_ARVB.status='0';                        //coloca status 0 no cabecalho da arvore B
    fseek(arquivoindices,0,SEEK_SET);
    escreve_cabecalho_arvb(arquivoindices,CAB_ARVB);

    // guarda o valor de topo da pilha e do rrnfinal
    // esses valores sao importantes para calcular o BYTEOFFSET das chaves
    // e para inserir os dados novos nos locais certos
    topo = CAB.topo;        
    RRNFinal = CAB.proxRRN; 

    scanf("%d", &nroInsercoes); // quantidade de dados que serao inseridos

    for (i = 0; i < nroInsercoes; i++)
    {

        // HORA DA INSERCAO DO DADO
        if (topo == -1)//checa se existem dados removidos para inserir ou ao fim do arquivo ou na pilha de remocao
        {
            fclose(arquivodados); // fecha o arquivo para abrir em modo append

            DADO = le_do_teclado(); // le os dados que serao inseridos
            
            /////////////////////////////////////////////////////////////////
            //NOVO:separa o nome do dado que se deseja inserir, calcula sua chave e seu byteoffset 
            linha = strdup(DADO.variavel); 
            nome = strsep(&linha, "#");
            chave=converteNome(nome);                       //cria a chave com a funcao fornecida
            byteoffset = (long)(1600 + RRNFinal*160);       //calcula o byteoffset relativo ao registro que tera a chave inserida
            /////////////////////////////////////////////////////////////////

            if (DADO.removido != '3') // DADO.removido usado como flag, se houver uma inconsistencia na leitura de dados eles nao sao inseridos
            {
                escreve_dado_bin(nome_arqdados, DADO); // escreve no fim do arquivo
                RRNFinal++;                             // posicao do ultimo rrn do arquivo eh atualizada ja que a insercao foi no fim do arquivo


                /////////////////////////////////////////////////////////////////////////////////////////////////
                //CHAMADA RECURSIVA DA INSERCAO DA NOVA CHAVE, ALEM DE ATUALIZAR A RAIZ E O PROXIMO RRN VAGO
                PESQ=insere_chave(chave,byteoffset,arquivoindices,CAB_ARVB.noRaiz,CAB_ARVB.noRaiz);
                CAB_ARVB.noRaiz=PESQ.nova_raiz;         //atualiza a raiz caso ela tenha sofrido split
                CAB_ARVB.RRNproxNo+=PESQ.nova_pagina;   //aumenta o rrn do proximo no caso tenham ocorrido splits
                //////////////////////////////////////////////////////////////////////////////////////////////////
            }

            arquivodados = fopen(nome_arqdados, "rb+"); // abre o arquivo de novo em modo rb+ para pesquisar e editar o arquivo
        }
        else    //insere na pilha de remocao
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

    //atualiza o cabecalho da arvore B com o status=1 e as outras informacoes lidas antes
    fseek(arquivoindices,0,SEEK_SET);
    CAB_ARVB.status='1';
    escreve_cabecalho_arvb(arquivoindices,CAB_ARVB);//atualiza o cabecalho do arquivo de indices
    fclose(arquivoindices);                         //fecha o arquivo de indices

    // ATUALIZA AS INFORMACOES DO CABECALHO DE DADOS
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
