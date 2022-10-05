#include "thumb.h"

int main(void){
ini:
	FILE *in,*out;
	char name[100];
	printf("digite o nome do arquivo: ");
	scanf("%s",name);
	
	if(decod_thumb(name,in,out) == -1) // função que fará a conversão thumb
		printf("arquivo não encontrado\n");
	printf("decodificar novamente? (sim|não)\n");
	printf("-> ");
	scanf("%s",name);
	if(strncmp(name,"sim",1) == 0)
		goto ini;

	return 0;
}
