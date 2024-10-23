#include "./funcoesArvB.h"
#include "./funcoesCriadas.h"

//funcao que escreve o cabecalho da arvore B
void escreve_cabecalho_arvb(FILE *arquivo,CabecalhoArvB CAB){
    int i;
    char lixo='$';

    fwrite(&CAB.status, 1, 1, arquivo);
    fwrite(&CAB.noRaiz, 4, 1, arquivo);
    fwrite(&CAB.RRNproxNo, 4, 1, arquivo);
    for(i=0;i<84;i++)
        fwrite(&lixo, 1, 1, arquivo);
}
//funcao que escreve uma pagina da arvore B
void escreve_no_arvb(FILE *arquivo,No no){
    int i;
    
    fwrite(&no.folha, 1, 1, arquivo);
    fwrite(&no.nroChavesNo, 4, 1, arquivo);
    fwrite(&no.RRNdoNo, 4, 1, arquivo);
    //escreve os ponteiros e as chaves
    for(i=0;i<m;i++){
        fwrite(&no.P[i], 4, 1, arquivo);

        if(i!=m-1){
            fwrite(&no.C[i], 8, 1, arquivo);
            fwrite(&no.Pr[i], 8, 1, arquivo);
        }
    }
}
//funcao que le o cabecalho de uma arvore B e retorna esse cabecalho
CabecalhoArvB le_cab_arvb(FILE *arquivo){
    CabecalhoArvB CAB;
    fread(&CAB.status, 1, 1, arquivo);
    fread(&CAB.noRaiz, 4, 1, arquivo);
    fread(&CAB.RRNproxNo, 4, 1, arquivo);

    return CAB;
}
//funcao que le uma pagina de uma arvore B e retorna essa pagina
No le_no_arvb(FILE *arquivo){
    int i;
    No no;
    fread(&no.folha, 1, 1, arquivo);
    fread(&no.nroChavesNo, 4, 1, arquivo);
    fread(&no.RRNdoNo, 4, 1, arquivo);
    //le os ponteiros e as chaves
    for(i=0;i<m;i++){
        fread(&no.P[i], 4, 1, arquivo);

        //nao realiza a leitura da chave e dos byteoffsets quando chegar no fim do vetor (que tem tamanho m-1)
        if(i!=m-1){
            fread(&no.C[i], 8, 1, arquivo);
            fread(&no.Pr[i], 8, 1, arquivo);
        }

    }
    return no;
}