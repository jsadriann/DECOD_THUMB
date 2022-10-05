#include "thumb.h"
//constants
char resister[8][3] = {"r0","r1","r2","r3","r4","r5","r6","r7"};
//buffer
char _buffer[2000];

int decod_thumb(char *name,FILE *in,FILE *out){ // utilizando operações BITWISE
    in = fopen(name,"r"); // abre arquivo para leitura
	if(in == NULL){ // caso o arquivo não exista
		return -1;
	}
    out = fopen("out.txt","w+"); // cria o arquivo gue gerará a saida

    
    long int int_thumb; // irá armazenar a conversão da instrução thumb asc na instrução thumb binary
    char memory[100],linha[100];
    char* ptr,*temp;
    int result = 0,aux = 0;
    
    //Const EOF define o fim do arquivo
    while (fscanf(in, "%s%s", memory,linha) != EOF)
    {
        char transfer[100];
        int_thumb = strtol(linha,&ptr,16); // transforma thumb asc para thumb hexa
        aux = (int_thumb >> 12); // pega os 4 bit mais significativo dos 16 bits, irá representar a instrução
        switch(aux){ // buscando instrução
         char transfer[100];
            case 0x0:
                if((int_thumb >> 11) & 0x1) //instruction LSR Ld,Lm,#immed5
                    strcpy(transfer,"LSR ");
                else //instruction LSL Ld,Lm,#immed5
                    strcpy(transfer,"LSL "); 
                //buscando operando
                strcat(transfer,resister[(int_thumb)&0x7]); // Ld
                strcat(transfer,",");
                strcat(transfer,resister[(int_thumb>>3)&0x7]); // Lm
                aux = (int_thumb >> 6) & 0x1f; //immed5
                strcat(transfer,",#");
                sprintf(_buffer,"%d",aux);
                strcat(transfer,_buffer);
                fprintf(out,"%s\n",transfer);
            break;

            case 0x1:
                if((int_thumb >> 11) & 0x1){
                    if((int_thumb >> 10) & 0x1){
                        if((int_thumb >> 10) & 0x1) //instruction SUB Ld,Ln,#immed3
                            strcpy(transfer,"SUB ");
                        else // instruction ADD Ld,Ln,#immed3
                            strcpy(transfer,"ADD ");
                        
                        strcat(transfer,resister[(int_thumb)&0x7]); // Ld
                        strcat(transfer,",");
                        strcat(transfer,resister[(int_thumb>>3)&0x7]); // Ln
                        aux = (int_thumb >> 6) & 0x7; // value immed3
                        strcat(transfer,",#");
                        sprintf(_buffer,"%d",aux);
                        strcat(transfer,_buffer);
                        fprintf(out,"%s\n",transfer);
                    }else{
                        if((int_thumb >> 10) & 0x1) //instruction SUB Ld,Ln,Lm
                            strcpy(transfer,"SUB ");
                        else //instruction ADD Ld,Ln,Lm
                            strcpy(transfer,"ADD ");

                        strcat(transfer,resister[(int_thumb)&0x7]); // Ld
                        strcat(transfer,",");
                        strcat(transfer,resister[(int_thumb>>3)&0x7]); // Ln
                        strcat(transfer,",");
                        strcat(transfer,resister[(int_thumb>>6)&0x7]); // Lm
                        fprintf(out,"%s\n",transfer);
                    }

                }else{ //instruction ASR Ld,Lm,#immed5
                    strcpy(transfer,"ASR ");
                    //buscando operando
                    strcat(transfer,resister[(int_thumb)&0x7]); // Ld
                    strcat(transfer,",");
                    strcat(transfer,resister[(int_thumb>>3)&0x7]); // Lm
                    aux = (int_thumb >> 6) & 0x1f; // value immed5
                    strcat(transfer,",#");
                    sprintf(_buffer,"%d",aux);
                    strcat(transfer,_buffer);
                    fprintf(out,"%s\n",transfer);
                }
            break;

            case 0x2:
                if((int_thumb >> 11) & 0x1) //instruction CMP Ld/Ln,#immed8
                    strcpy(transfer,"CMP "); 
                else //instruction MOV Ld/Ln,#immed8
                    strcpy(transfer,"MOV ");
                //buscando operando
                strcat(transfer,resister[(int_thumb>>8)&0x7]); // Ld/Ln
                strcat(transfer,",");
                aux = (int_thumb) & 0xff; // value immed8
                strcat(transfer,"#");
                sprintf(_buffer,"%d",aux);
                strcat(transfer,_buffer);
                fprintf(out,"%s\n",transfer);
            break;

            case 0x3:
                if((int_thumb >> 11) & 0x1) //instruction CMP Ld/Ln,#immed8
                    strcpy(transfer,"SUB "); 
                else //instruction MOV Ld/Ln,#immed8
                    strcpy(transfer,"ADD ");
                //buscando operando
                strcat(transfer,resister[(int_thumb>>8)&0x7]); // Ld
                strcat(transfer,",#");
                aux = (int_thumb) & 0xff; //value immed8
                sprintf(_buffer,"%d",aux);
                strcat(transfer,_buffer);
                fprintf(out,"%s\n",transfer);

            break;

            case 0x4:
                if((int_thumb >> 11) & 0x1){ // instruction LDR Ld,[pc,#immed*4]
                    strcpy(transfer,"LDR ");
                    //buscando operando
                    strcat(transfer,resister[(int_thumb>>8)&0x7]); // Ld
                    strcat(transfer,",[pc,#");
                    aux = (int_thumb) & 0xff;
                    sprintf(_buffer,"%d",aux);
                    strcat(transfer,_buffer);
                    fprintf(out,"%s]\n",transfer);
                }else{
                    //ainda por fazer
                }
            break;

            case 0x5:
                if((int_thumb >> 11) & 0x1){ // instruction LDR'x' Ld,Lm,Ln
                    if((int_thumb >> 9) & 0x3 == 0x0) strcpy(transfer,"LDR ");
                    else if((int_thumb >> 9) & 0x3 == 0x1) strcpy(transfer,"LDRH ");
                    else if((int_thumb >> 9) & 0x3 == 0x2) strcpy(transfer,"LDRB ");
                    else strcpy(transfer,"LDRSH ");
                }else{ // instruction STR'x' Ld,Lm,Ln
                    if((int_thumb >> 9) & 0x3 == 0x0) strcpy(transfer,"STR ");
                    else if((int_thumb >> 9) & 0x3 == 0x1) strcpy(transfer,"STRH ");
                    else if((int_thumb >> 9) & 0x3 == 0x2) strcpy(transfer,"STRB ");
                    else strcpy(transfer,"LTRSB ");
                }
                //buscando operando
                strcat(transfer,resister[(int_thumb)&0x7]); // Ld
                strcat(transfer,",");
                strcat(transfer,resister[(int_thumb>>6)&0x7]); // Lm
                strcat(transfer,",");
                strcat(transfer,resister[(int_thumb>>3)&0x7]); // Ln
                fprintf(out,"%s\n",transfer);
            break;

            case 0x6:
                if((int_thumb >> 11) & 0x1) // instruction LDR Ld,[Ln,#immed5*4]
                    strcpy(transfer,"LDR ");
                else // instruction LDR Ld,[Ln,#immed5*4]
                    strcpy(transfer,"STR ");
                //buscando operando
                strcat(transfer,resister[(int_thumb)&0x7]); // Ld
                strcat(transfer,",[");
                strcat(transfer,resister[(int_thumb>>3)&0x7]); // Ln
                strcat(transfer,",#");
                aux = (int_thumb >> 6) & 0x1f; // value immed5
                sprintf(_buffer,"%d",aux);
                strcat(transfer,_buffer);
                fprintf(out,"%s]\n",transfer);
            break;

            case 0x7:
                if((int_thumb >> 11) & 0x1) // instruction LDRB Ld,[Ln,#immed5]
                    strcpy(transfer,"LDRB ");
                else // instruction LDRB Ld,[Ln,#immed5]
                    strcpy(transfer,"STRB ");
                //buscando operando
                strcat(transfer,resister[(int_thumb)&0x7]); // Ld
                strcat(transfer,",[");
                strcat(transfer,resister[(int_thumb>>3)&0x7]); // Ln
                strcat(transfer,",#");
                aux = (int_thumb >> 6) & 0x1f; // value immed5
                sprintf(_buffer,"%d",aux);
                strcat(transfer,_buffer);
                fprintf(out,"%s]\n",transfer);
            break;

            case 0x8:
                if((int_thumb >> 11) & 0x1) // instruction LDRH Ld,[Ln,#immed5*2]
                    strcpy(transfer,"LDRH ");
                else // instruction LDRH Ld,[Ln,#immed5*2]
                    strcpy(transfer,"STRH ");
                //buscando operando
                strcat(transfer,resister[(int_thumb)&0x7]); // Ld
                strcat(transfer,",[");
                strcat(transfer,resister[(int_thumb>>3)&0x7]); // Ln
                strcat(transfer,",#");
                aux = (int_thumb >> 6) & 0x1f; // value immed5
                sprintf(_buffer,"%d",aux);
                strcat(transfer,_buffer);
                fprintf(out,"%s]\n",transfer);

            break;

            case 0x9:
                if ((int_thumb >> 11) & 0x1) strcpy(transfer,"LDR ");//LDR
                else strcpy(transfer,"STR ");//STR
                strcat(transfer,resister[(int_thumb >> 7) & 0x7]); //Ld
                strcat(transfer,", [sp, #");
                aux = (int_thumb & 0xff)*4; //Immed8
                sprintf(_buffer,"%d",aux);
                strcat(transfer,_buffer);
                strcat(transfer,"]"); 
                fprintf(out,"%s\n",transfer);
                    
            break;

            case 0xa: // instruction ADD Ln,sp|pc,#immed8*4
                strcpy(transfer,"ADD ");
                //buscando  operando
                strcat(transfer,resister[(int_thumb>>8)&0x7]); // Ld
                if((int_thumb >> 11) & 0x1) strcat(transfer,",sp,#");
                else strcat(transfer,",pc,#");
                aux = (int_thumb) & 0xff; // value immed8
                sprintf(_buffer,"%d",(aux*4));
                strcat(transfer,_buffer);
                fprintf(out,"%s\n",transfer);
            break;

            case 0xb:
            break;

            case 0xc:
            break;

            case 0xd:
            break;

            case 0xe:
            break;

            case 0xf:
            break;

            default:
            break;

            
        }

    }
    fclose(in);
    fclose(out);
    return 0;
    
}