#include "./funcoesArvB.h"
#include "./funcoesCriadas.h"

void cria_arq_indices(){
    char nome_arqdados[31];
    char nome_arqindices[31];

    //variaveis de cabecalho e de dado de cada tipo de arquivo
    cabecalho CAB_DADOS;
    dados     DADO;

    CabecalhoArvB CAB_ARVB;
    No            NO;

    //variaveis do rrn de cada registro do arquivo de dados e para a criacao da chave do dado
    int RRN;
    char *linha;
    char *nome;
    long chave;

    scanf("%s",nome_arqdados);
    scanf("%s",nome_arqindices);

    FILE *arquivodados;
    FILE *arquivoindices;

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
    
    arquivoindices = fopen(nome_arqindices,"ab");   //cria o arquivo de indices em arvore-b

    CAB_ARVB.status='0';
    CAB_ARVB.noRaiz=-1;
    CAB_ARVB.RRNproxNo=0;
    escreve_cabecalho_arvb(arquivoindices,CAB_ARVB);//reserva o espaco do cabecalho do arquivo de indices

    fclose(arquivoindices);                         //fecha o arquivo apos reservar o cabecalho

    RRN=-1;
    //loop de leitura dos registros do arquivo de dados
    while(1){

        DADO=le_registro(arquivodados); //le o dado do arquivo de dados e atualiza o RRN relativo a esse dado
        RRN++;                                          
        if(DADO.removido=='2')          //sai do loop de leitura de dados caso chegue no fim do arquivo de dados, usa a variavel DADO.removido como flag dentro da funcao le_registro
            break;
        if(DADO.removido=='1')          //se o dado lido estiver logicamente removido volta para o começo do loop para ler o proximo registro
            continue;

        linha = strdup(DADO.variavel);  // separa o campo do nome
        nome = strsep(&linha, "#");

        chave=converteNome(nome);       //cria a chave com a funcao fornecida

        //CRIA A ARVORE B
        //insere_chave(chave,RRN,nome_arqindices);      //funcao que insere a chave no arquivo com a arvore
        
    }
    fclose(arquivodados);   //fecha o arquivo de dados apos ter terminado de ler tudo dele

    //ATUALIZACAO DO CABECALHO DO ARQUIVO DE INDICES

    binarioNaTela(nome_arqindices); //binario da tela mostrando o resultado
}
