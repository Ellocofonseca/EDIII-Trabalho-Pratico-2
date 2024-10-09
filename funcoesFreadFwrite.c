#include "./funcoesCriadas.h"


//FUNCOES DE ARQUIVO BINARIO

//a funcao le os campos do cabecalho do arquivo a partir da posicao que o ponteiro se encontra no momento
//depois de ler retorna a variavel do tipo cabecalho
cabecalho le_cabecalho(FILE *arquivo){
    cabecalho CAB;

    fread(&CAB.status,1,1,arquivo);
    fread(&CAB.topo,4,1,arquivo);
    fread(&CAB.proxRRN,4,1,arquivo);
    fread(&CAB.nroRegRem,4,1,arquivo);
    fread(&CAB.nroPagDisco,4,1,arquivo);
    fread(&CAB.qttCompacta,4,1,arquivo);
    return CAB;
}

//a funcao le os campos de dados do arquivo a partir da posicao que o ponteiro se encontra no momento
//caso o arquivo acabe o campo 'removido' da variavel DADO eh usado como flag para informar que o arquivo acabou
//retorna a variavel do tipo dados
//essa tatica com a flag impede que um arquivo que acaba do nada tenha seu dado incompleto inserido
dados le_registro(FILE *arquivo){

    dados DADO;

    if(fread(&DADO.removido,1,1,arquivo)==0){
        DADO.removido='2';
        return DADO;
    }
    if(fread(&DADO.encadeamento,4,1,arquivo)==0){
        DADO.removido='2';
        return DADO;
    }
    if(fread(&DADO.populacao,4,1,arquivo)==0){
        DADO.removido='2';
        return DADO;
    }
    if(fread(&DADO.tamanho,4,1,arquivo)==0){
        DADO.removido='2';
        return DADO;
    }
    if(fread(&DADO.unidadeMedida,1,1,arquivo)==0){
        DADO.removido='2';
        return DADO;
    }
    if(fread(&DADO.velocidade,4,1,arquivo)==0){
        DADO.removido='2';
        return DADO;
    }
    if(fread(DADO.variavel,142,1,arquivo)==0){
        DADO.removido='2';
        return DADO;
    }
    return DADO;
}

//a funcao escreve e "reserva" uma porcao inicial de um arquivo binario para a area do cabecalho e insere o lixo tambem
void escreve_cabecalho_bin(char nomebin[31],cabecalho CAB){
    FILE *arquivobin;
    char lixo='$';
    arquivobin = fopen(nomebin, "ab");  //cria o arquivo caso n exista
    fwrite(&CAB.status, 1, 1, arquivobin);
    fwrite(&CAB.topo, 4, 1, arquivobin);
    fwrite(&CAB.proxRRN, 4, 1, arquivobin);
    fwrite(&CAB.nroRegRem, 4, 1, arquivobin);
    fwrite(&CAB.nroPagDisco, 4, 1, arquivobin);
    fwrite(&CAB.qttCompacta, 4, 1, arquivobin);
    for(int i=0;i<1579;i++)
        fwrite(&lixo, 1, 1, arquivobin);
    fclose(arquivobin);
}
//a funcao atualiza um cabecalho de acordo com as edicoes que foram feitas num arquivo binario
void atualiza_cabecalho_bin(char nomebin[31],cabecalho CAB){
    FILE *arquivobin;
    arquivobin = fopen(nomebin, "rb+"); //abre o arquivo para atualizar o cabecalho, o modo rb+ é necessario para isso
    if(arquivobin==NULL){
        printf(ERRO_PADRAO);
    }
    else{
    fwrite(&CAB.status, 1, 1, arquivobin);
    fwrite(&CAB.topo, 4, 1, arquivobin);
    fwrite(&CAB.proxRRN, 4, 1, arquivobin);
    fwrite(&CAB.nroRegRem, 4, 1, arquivobin);
    fwrite(&CAB.nroPagDisco, 4, 1, arquivobin);
    fwrite(&CAB.qttCompacta, 4, 1, arquivobin);
    fclose(arquivobin);
    }
}
//a funcao escreve um dado no arquivo binario que esta sendo criado, abre em modo append para escrever no fim do arquivo
void escreve_dado_bin(char nomebin[31],dados DADO){
    FILE *arquivobin;
    arquivobin = fopen(nomebin, "ab");
    fwrite(&DADO.removido, 1, 1, arquivobin);
    fwrite(&DADO.encadeamento, 4, 1, arquivobin);
    fwrite(&DADO.populacao, 4, 1, arquivobin);
    fwrite(&DADO.tamanho, 4, 1, arquivobin);
    fwrite(&DADO.unidadeMedida, 1, 1, arquivobin);
    fwrite(&DADO.velocidade, 4, 1, arquivobin);
    fwrite(DADO.variavel, 142, 1, arquivobin);
    fclose(arquivobin);
}

//a funcao atualiza um dado removendo ele logicamente e preenchendo o resto dos campos com o lixo "$"
void remove_dado_bin(FILE *arquivobin,int encadeamento){
    char lixo='$';
    char remo='1';
    int i;
    fwrite(&remo, 1, 1, arquivobin);
    fwrite(&encadeamento, 4, 1, arquivobin);
    for(i=0;i<155;i++)
        fwrite(&lixo, 1, 1, arquivobin);

}
//a funcao atualiza um dado num determinado campo do arquivo binario, o ponteiro com a localizacao
//de onde sera inserido o dado eh parametro da funcao
void atualiza_dado_bin(dados DADO,FILE *arquivobin){

    fwrite(&DADO.removido, 1, 1, arquivobin);
    fwrite(&DADO.encadeamento, 4, 1, arquivobin);
    fwrite(&DADO.populacao, 4, 1, arquivobin);
    fwrite(&DADO.tamanho, 4, 1, arquivobin);
    fwrite(&DADO.unidadeMedida, 1, 1, arquivobin);
    fwrite(&DADO.velocidade, 4, 1, arquivobin);
    fwrite(DADO.variavel, 142, 1, arquivobin);
}

