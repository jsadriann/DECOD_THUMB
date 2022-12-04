#include "thumb.h"
//constants
char resister[8][3] = {"r0","r1","r2","r3","r4","r5","r6","r7"};
char resister_high[8][3] = {"r8","r9","r10","r11","r12","r13","r14","r15"};
char resister_all[16][3] = {"r0","r1","r2","r3","r4","r5","r6","r7","r8","r9","r10","r11","r12","r13","r14","r15"};
char condition[15][4] = {"EQ","NE","CS","CC","MI","PL","VS","VC","HI","LS","GE","LT","GT","LE","AL"};
//buffer
char _buffer[2000];

int decod_thumb(char *name,FILE *in,FILE *out){ // utilizando operações BITWISE
    in = fopen(name,"r"); // abre arquivo para leitura
	if(in == NULL){ // caso o arquivo não exista
		return -1;
	}
    out = fopen("out.txt","w+"); // cria o arquivo que gerará a saida

    
    long int int_thumb; // irá armazenar a conversão da instrução thumb asc na instrução thumb binary
    int ins_addr,pl = 0,poff;
    char memory[100],linha[100];
    char* ptr,*temp;
    int result = 0,aux = 0;
    long int ins[2];
    
    fprintf(out,"%s\n",".thumb");
    //Const EOF define o fim do arquivo
    while (fscanf(in, "%s%s", memory,linha) != EOF)
    {
        char transfer[100];
        int_thumb = strtol(linha,&ptr,16); // transforma asc para hexa
        ins_addr = strtol(memory,&ptr,16);
        ins[0] = int_thumb & 0x0000ffff;
        ins[1] = int_thumb >> 16;
        for(int i = 0; i < 2; i++){
            aux = (ins[i] >> 12); // pega os 4 bit mais significativo dos 16 bits, irá representar a instrução
            switch(aux){ // buscando instrução
            char transfer[100];
                case 0x0:
                    if((ins[i] >> 11) & 0x1) //instruction LSR Ld,Lm,#immed5
                        strcpy(transfer,"LSR ");
                    else //instruction LSL Ld,Lm,#immed5
                        strcpy(transfer,"LSL "); 
                    //buscando operando
                    strcat(transfer,resister[(ins[i])&0x7]); // Ld
                    strcat(transfer,",");
                    strcat(transfer,resister[(ins[i]>>3)&0x7]); // Lm
                    aux = (ins[i] >> 6) & 0x1f; //immed5
                    strcat(transfer,",#");
                    sprintf(_buffer,"%d",aux);
                    strcat(transfer,_buffer);
                    fprintf(out,"%s\n",transfer);
                break;

                case 0x1:
                    if((ins[i] >> 11) & 0x1){
                        if((ins[i] >> 10) & 0x1){
                            if((ins[i] >> 10) & 0x1) //instruction SUB Ld,Ln,#immed3
                                strcpy(transfer,"SUB ");
                            else // instruction ADD Ld,Ln,#immed3
                                strcpy(transfer,"ADD ");
                            
                            strcat(transfer,resister[(ins[i])&0x7]); // Ld
                            strcat(transfer,",");
                            strcat(transfer,resister[(ins[i]>>3)&0x7]); // Ln
                            aux = (ins[i] >> 6) & 0x7; // value immed3
                            strcat(transfer,",#");
                            sprintf(_buffer,"%d",aux);
                            strcat(transfer,_buffer);
                            fprintf(out,"%s\n",transfer);
                        }else{
                            if((ins[i] >> 10) & 0x1) //instruction SUB Ld,Ln,Lm
                                strcpy(transfer,"SUB ");
                            else //instruction ADD Ld,Ln,Lm
                                strcpy(transfer,"ADD ");

                            strcat(transfer,resister[(ins[i])&0x7]); // Ld
                            strcat(transfer,",");
                            strcat(transfer,resister[(ins[i]>>3)&0x7]); // Ln
                            strcat(transfer,",");
                            strcat(transfer,resister[(ins[i]>>6)&0x7]); // Lm
                            fprintf(out,"%s\n",transfer);
                        }

                    }else{ //instruction ASR Ld,Lm,#immed5
                        strcpy(transfer,"ASR ");
                        //buscando operando
                        strcat(transfer,resister[(ins[i])&0x7]); // Ld
                        strcat(transfer,",");
                        strcat(transfer,resister[(ins[i]>>3)&0x7]); // Lm
                        aux = (ins[i] >> 6) & 0x1f; // value immed5
                        strcat(transfer,",#");
                        sprintf(_buffer,"%d",aux);
                        strcat(transfer,_buffer);
                        fprintf(out,"%s\n",transfer);
                    }
                break;

                case 0x2:
                    if((ins[i] >> 11) & 0x1) //instruction CMP Ld/Ln,#immed8
                        strcpy(transfer,"CMP "); 
                    else //instruction MOV Ld/Ln,#immed8
                        strcpy(transfer,"MOV ");
                    //buscando operando
                    strcat(transfer,resister[(ins[i]>>8)&0x7]); // Ld/Ln
                    strcat(transfer,",");
                    aux = (ins[i]) & 0xff; // value immed8
                    strcat(transfer,"#");
                    sprintf(_buffer,"%d",aux);
                    strcat(transfer,_buffer);
                    fprintf(out,"%s\n",transfer);
                break;

                case 0x3:
                    if((ins[i] >> 11) & 0x1) //instruction CMP Ld/Ln,#immed8
                        strcpy(transfer,"SUB "); 
                    else //instruction MOV Ld/Ln,#immed8
                        strcpy(transfer,"ADD ");
                    //buscando operando
                    strcat(transfer,resister[(ins[i]>>8)&0x7]); // Ld
                    strcat(transfer,",#");
                    aux = (ins[i]) & 0xff; //value immed8
                    sprintf(_buffer,"%d",aux);
                    strcat(transfer,_buffer);
                    fprintf(out,"%s\n",transfer);

                break;

                case 0x4:
                    if((ins[i] >> 11) & 0x1){ // instruction LDR Ld,[pc,#immed*4]
                        strcpy(transfer,"LDR ");
                        //buscando operando
                        strcat(transfer,resister[(ins[i]>>8)&0x7]); // Ld
                        strcat(transfer,",[pc,#");
                        aux = (ins[i]) & 0xff;
                        sprintf(_buffer,"%d",aux);
                        strcat(transfer,_buffer);
                        fprintf(out,"%s]\n",transfer);
                    }else{
                        switch ((ins[i]>>8)&0x7)
                        {
                        case 0x0:// instruction AND/EOR/LSL/LSR Lm/Ls,Ld
                            if(((ins[i]>>6) & 0x3) == 0x0)
                                strcpy(transfer,"AND ");
                            else if(((ins[i]>>6) & 0x3) == 0x1)
                                strcpy(transfer,"EOR ");
                            else if(((ins[i]>>6) & 0x3) == 0x2)
                                strcpy(transfer,"LSL ");
                            else
                                strcpy(transfer,"LSR ");
                            
                            //buscando operando
                            strcat(transfer,resister[(ins[i]>>3)&0x7]); // Lm
                            strcat(transfer,",");
                            strcat(transfer,resister[(ins[i])&0x7]); // Ld
                            fprintf(out,"%s]\n",transfer);
                            break;
                        
                        case 0x1:// instruction ASR/ADC/SBC/ROR Lm/Ls,Ld
                            if(((ins[i]>>6) & 0x3) == 0x0)
                                strcpy(transfer,"ASR ");
                            else if(((ins[i]>>6) & 0x3) == 0x1)
                                strcpy(transfer,"ADC ");
                            else if(((ins[i]>>6) & 0x3) == 0x2)
                                strcpy(transfer,"SBC ");
                            else
                                strcpy(transfer,"ROR ");

                            //buscando operando
                            strcat(transfer,resister[(ins[i]>>3)&0x7]); // Lm
                            strcat(transfer,",");
                            strcat(transfer,resister[(ins[i])&0x7]); // Ld
                            fprintf(out,"%s]\n",transfer);
                        break;

                        case 0x2:
                            if(((ins[i]>>6) & 0x3) == 0x0)
                                strcpy(transfer,"TST ");
                            else if(((ins[i]>>6) & 0x3) == 0x1)
                                strcpy(transfer,"NEG ");
                            else if(((ins[i]>>6) & 0x3) == 0x2)
                                strcpy(transfer,"CMP ");
                            else
                                strcpy(transfer,"CMN ");

                            //buscando operando
                            strcat(transfer,resister[(ins[i]>>3)&0x7]); // Lm
                            strcat(transfer,",");
                            strcat(transfer,resister[(ins[i])&0x7]); // Ld
                            fprintf(out,"%s]\n",transfer);
                        break;

                        case 0x3:
                            if(((ins[i]>>6) & 0x3) == 0x0)
                                strcpy(transfer,"ORR ");
                            else if(((ins[i]>>6) & 0x3) == 0x1)
                                strcpy(transfer,"MUL ");
                            else if(((ins[i]>>6) & 0x3) == 0x2)
                                strcpy(transfer,"BIC ");
                            else
                                strcpy(transfer,"MVN ");

                            //buscando operando
                            strcat(transfer,resister[(ins[i]>>3)&0x7]); // Lm
                            strcat(transfer,",");
                            strcat(transfer,resister[(ins[i])&0x7]); // Ld
                            fprintf(out,"%s]\n",transfer);
                        break;
                        
                        case 0x4:
                            switch ((ins[i]>>6) & 0x3)
                            {
                            case 0x1:
                                strcpy(transfer,"ADD ");
                                strcat(transfer,resister_high[(ins[i]>>3)&0x7]); // Lm
                                strcat(transfer,",");
                                strcat(transfer,resister[(ins[i])&0x7]); // Ld
                                fprintf(out,"%s]\n",transfer);
                            break;
                            
                            case 0x2:
                                strcpy(transfer,"ADD ");
                                strcat(transfer,resister[(ins[i]>>3)&0x7]); // Lm
                                strcat(transfer,",");
                                strcat(transfer,resister_high[(ins[i])&0x7]); // Ld
                                fprintf(out,"%s]\n",transfer);
                            break;

                            case 0x3:
                                strcpy(transfer,"ADD ");
                                strcat(transfer,resister_high[(ins[i]>>3)&0x7]); // Lm
                                strcat(transfer,",");
                                strcat(transfer,resister_high[(ins[i])&0x7]); // Ld
                                fprintf(out,"%s]\n",transfer);
                            default:
                                strcpy(transfer,"undefined");
                                fprintf(out,"%s\n",transfer);
                            break;
                            }
                        break;

                        case 0x5:
                            switch ((ins[i]>>6) & 0x3){
                                case 0x1:
                                    strcpy(transfer,"CMP ");
                                    strcat(transfer,resister_high[(ins[i]>>3)&0x7]); // Lm
                                    strcat(transfer,",");
                                    strcat(transfer,resister[(ins[i])&0x7]); // Ld
                                    fprintf(out,"%s]\n",transfer);
                                break;

                                case 0x2:
                                    strcpy(transfer,"CMP ");
                                    strcat(transfer,resister[(ins[i]>>3)&0x7]); // Lm
                                    strcat(transfer,",");
                                    strcat(transfer,resister_high[(ins[i])&0x7]); // Ld
                                    fprintf(out,"%s]\n",transfer);
                                break;

                                case 0x3:
                                    strcpy(transfer,"CMP ");
                                    strcat(transfer,resister_high[(ins[i]>>3)&0x7]); // Lm
                                    strcat(transfer,",");
                                    strcat(transfer,resister_high[(ins[i])&0x7]); // Ld
                                    fprintf(out,"%s]\n",transfer);
                                break;

                                default:
                                    strcpy(transfer,"undefined");
                                    fprintf(out,"%s\n",transfer);
                                break;
                            }
                        break;

                        case 0x6:
                            switch ((ins[i]>>6) & 0x3)
                            {
                            case 0x0:
                                strcpy(transfer,"CPY ");
                                strcat(transfer,resister[(ins[i]>>3)&0x7]); // Lm
                                strcat(transfer,",");
                                strcat(transfer,resister[(ins[i])&0x7]); // Ld
                                fprintf(out,"%s]\n",transfer);
                            break;
                            
                            case 0x1:
                                strcpy(transfer,"MOV ");
                                strcat(transfer,resister_high[(ins[i]>>3)&0x7]); // Lm
                                strcat(transfer,",");
                                strcat(transfer,resister[(ins[i])&0x7]); // Ld
                                fprintf(out,"%s]\n",transfer);
                            break;

                            case 0x2:
                                strcpy(transfer,"MOV ");
                                strcat(transfer,resister[(ins[i]>>3)&0x7]); // Lm
                                strcat(transfer,",");
                                strcat(transfer,resister_high[(ins[i])&0x7]); // Ld
                                fprintf(out,"%s]\n",transfer);
                            break;

                            case 0x3:
                                strcpy(transfer,"MOV ");
                                strcat(transfer,resister_high[(ins[i]>>3)&0x7]); // Lm
                                strcat(transfer,",");
                                strcat(transfer,resister_high[(ins[i])&0x7]); // Ld
                                fprintf(out,"%s]\n",transfer);
                            break;
                            default:
                                break;
                            }
                        break;
                        
                        case 0x7: // instruction BX/BLX Rm
                            if(((ins[i] >> 0x7) & 0x1) == 0x1)
                                strcpy(transfer,"BLX ");
                            else    
                                strcpy(transfer,"BX ");
                            //buscando operando
                            strcat(transfer,resister_all[(ins[i]>>3)&0xf]); // Lm
                            fprintf(out,"%s]\n",transfer);
                        break;

                        default:
                        break;
                        }
                    }
                break;

                case 0x5:
                    if((ins[i] >> 11) & 0x1){ // instruction LDR'x' Ld,Lm,Ln
                        if((ins[i] >> 9) & 0x3 == 0x0) strcpy(transfer,"LDR ");
                        else if((ins[i] >> 9) & 0x3 == 0x1) strcpy(transfer,"LDRH ");
                        else if((ins[i] >> 9) & 0x3 == 0x2) strcpy(transfer,"LDRB ");
                        else strcpy(transfer,"LDRSH ");
                    }else{ // instruction STR'x' Ld,Lm,Ln
                        if((ins[i] >> 9) & 0x3 == 0x0) strcpy(transfer,"STR ");
                        else if((ins[i] >> 9) & 0x3 == 0x1) strcpy(transfer,"STRH ");
                        else if((ins[i] >> 9) & 0x3 == 0x2) strcpy(transfer,"STRB ");
                        else strcpy(transfer,"LTRSB ");
                    }
                    //buscando operando
                    strcat(transfer,resister[(ins[i])&0x7]); // Ld
                    strcat(transfer,",");
                    strcat(transfer,resister[(ins[i]>>6)&0x7]); // Lm
                    strcat(transfer,",");
                    strcat(transfer,resister[(ins[i]>>3)&0x7]); // Ln
                    fprintf(out,"%s\n",transfer);
                break;

                case 0x6:
                    if((ins[i] >> 11) & 0x1) // instruction LDR Ld,[Ln,#immed5*4]
                        strcpy(transfer,"LDR ");
                    else // instruction LDR Ld,[Ln,#immed5*4]
                        strcpy(transfer,"STR ");
                    //buscando operando
                    strcat(transfer,resister[(ins[i])&0x7]); // Ld
                    strcat(transfer,",[");
                    strcat(transfer,resister[(ins[i]>>3)&0x7]); // Ln
                    strcat(transfer,",#");
                    aux = (ins[i] >> 6) & 0x1f; // value immed5
                    sprintf(_buffer,"%d",aux);
                    strcat(transfer,_buffer);
                    fprintf(out,"%s]\n",transfer);
                break;

                case 0x7:
                    if((ins[i] >> 11) & 0x1) // instruction LDRB Ld,[Ln,#immed5]
                        strcpy(transfer,"LDRB ");
                    else // instruction LDRB Ld,[Ln,#immed5]
                        strcpy(transfer,"STRB ");
                    //buscando operando
                    strcat(transfer,resister[(ins[i])&0x7]); // Ld
                    strcat(transfer,",[");
                    strcat(transfer,resister[(ins[i]>>3)&0x7]); // Ln
                    strcat(transfer,",#");
                    aux = (ins[i] >> 6) & 0x1f; // value immed5
                    sprintf(_buffer,"%d",aux);
                    strcat(transfer,_buffer);
                    fprintf(out,"%s]\n",transfer);
                break;

                case 0x8:
                    if((ins[i] >> 11) & 0x1) // instruction LDRH Ld,[Ln,#immed5*2]
                        strcpy(transfer,"LDRH ");
                    else // instruction LDRH Ld,[Ln,#immed5*2]
                        strcpy(transfer,"STRH ");
                    //buscando operando
                    strcat(transfer,resister[(ins[i])&0x7]); // Ld
                    strcat(transfer,",[");
                    strcat(transfer,resister[(ins[i]>>3)&0x7]); // Ln
                    strcat(transfer,",#");
                    aux = (ins[i] >> 6) & 0x1f; // value immed5
                    sprintf(_buffer,"%d",aux);
                    strcat(transfer,_buffer);
                    fprintf(out,"%s]\n",transfer);

                break;

                case 0x9:
                    if ((ins[i] >> 11) & 0x1) strcpy(transfer,"LDR ");//LDR
                    else strcpy(transfer,"STR ");//STR
                    strcat(transfer,resister[(ins[i] >> 7) & 0x7]); //Ld
                    strcat(transfer,", [sp, #");
                    aux = (ins[i] & 0xff)*4; //Immed8
                    sprintf(_buffer,"%d",aux);
                    strcat(transfer,_buffer);
                    strcat(transfer,"]"); 
                    fprintf(out,"%s\n",transfer);
                        
                break;

                case 0xa: // instruction ADD Ln,sp|pc,#immed8*4
                    strcpy(transfer,"ADD ");
                    //buscando  operando
                    strcat(transfer,resister[(ins[i]>>8)&0x7]); // Ld
                    if((ins[i] >> 11) & 0x1) strcat(transfer,",sp,#");
                    else strcat(transfer,",pc,#");
                    aux = (ins[i]) & 0xff; // value immed8
                    sprintf(_buffer,"%d",(aux*4));
                    strcat(transfer,_buffer);
                    fprintf(out,"%s\n",transfer);
                break;

                case 0xb:
                    if((ins[i]>>11)&0x1 == 1){
                        if(((ins[i]>>9) & 0x3) == 0x1){
                            if(((ins[i]>>6) & 0x3) == 0x0){
                                strcpy(transfer,"REV ");
                                strcat(transfer,resister[(ins[i] >> 3) & 0x7]); //Lm
                                strcat(transfer,resister[(ins[i]) & 0x7]); //Ld
                                fprintf(out,"%s\n",transfer);
                            }else if(((ins[i]>>6) & 0x3) == 0x1){
                                strcpy(transfer,"REV16 ");
                                strcat(transfer,resister[(ins[i] >> 3) & 0x7]); //Lm
                                strcat(transfer,resister[(ins[i]) & 0x7]); //Ld
                                fprintf(out,"%s\n",transfer);
                            }else if(((ins[i]>>6) & 0x3) == 0x2){
                                strcpy(transfer,"REVSH ");
                                strcat(transfer,resister[(ins[i] >> 3) & 0x7]); //Lm
                                strcat(transfer,resister[(ins[i]) & 0x7]); //Ld
                                fprintf(out,"%s\n",transfer);
                            }else{
                                strcpy(transfer,"undefined");
                                fprintf(out,"%s\n",transfer);
                            }
                        }else if(((ins[i]>>9) & 0x3) == 0x2){
                            strcpy(transfer,"POP {");
                            int temp = 0;
                            for(int j = 7; j >= 0; j--){
                                if(((ins[i]>>(7-j)) & 0x1) == 1){
                                    if(temp == 0){
                                        temp == 1;
                                        strcat(transfer,resister[7-j]);
                                    }else{
                                       strcat(transfer,","); 
                                       strcat(transfer,resister[7-j]);
                                    }
                                }
                            }
                            strcat(transfer,"}");
                            fprintf(out,"%s\n",transfer);
                        }else if(((ins[i]>>9) & 0x3) == 0x3){
                            strcpy(transfer,"BKPT #");
                            aux = (ins[i]) & 0xff; // value immed8
                            sprintf(_buffer,"%d",(aux));
                            strcat(transfer,_buffer);
                            fprintf(out,"%s\n",transfer);
                        }else{
                            strcpy(transfer,"undefined");
                            fprintf(out,"%s\n",transfer);
                        }
                    }else{
                        switch ((ins[i]>>9) & 0x3)
                        {
                        case 0x0:
                            if(((ins[i]>>7) & 0x1) == 0)
                                strcpy(transfer,"ADD sp,#");
                            else
                                strcpy(transfer,"SUB sp,#"); 

                            aux = (ins[i]) & 0x7f; // value immed7
                            sprintf(_buffer,"%d",(aux*4));
                            strcat(transfer,_buffer);
                            fprintf(out,"%s\n",transfer);
                        break;

                        case 0x1:
                            if(((ins[i]>>6) & 0x3) == 0x0)
                                strcpy(transfer,"SXTH ");
                            else if(((ins[i]>>6) & 0x3) == 0x1)
                                strcpy(transfer,"SXTB ");
                            else if(((ins[i]>>6) & 0x3) == 0x2)
                                strcpy(transfer,"UXTH ");
                            else
                                strcpy(transfer,"UXTB ");

                            strcat(transfer,resister[((ins[i])>>3)&0x7]); // Lm
                            strcat(transfer,",");
                            strcat(transfer,resister[(ins[i])&0x7]); // Ld
                            fprintf(out,"%s\n",transfer);
                        break;

                        case 0x2:
                            strcpy(transfer,"PUSH {");
                            int temp = 0;
                            for(int j = 7; j >= 0; j--){
                                if(((ins[i]>>(7-j)) & 0x1) == 1){
                                    if(temp == 0){
                                        temp == 1;
                                        strcat(transfer,resister[7-j]);
                                    }else{
                                       strcat(transfer,","); 
                                       strcat(transfer,resister[7-j]);
                                    }
                                }
                            }
                            strcat(transfer,"}");
                            fprintf(out,"%s\n",transfer);
                        break;
                        case 0x3:
                            if(((ins[i]>>5) & 0x7) == 0x2){
                                if(((ins[i]>>3) & 0x1) == 0x1)
                                    strcpy(transfer,"SETEND BE");
                                else 
                                    strcpy(transfer,"SETEND LE");
                            }else if(((ins[i]>>5) & 0x7) == 0x3){
                                if(((ins[i]>>4) & 0x1) == 0x1)
                                    strcpy(transfer,"CPSID");
                                else 
                                    strcpy(transfer,"CPSIE");
                            }else{
                                strcpy(transfer,"undefined");
                            }
                            fprintf(out,"%s\n",transfer);
                        break;
                        default:
                        break;
                        }
                    }
                break;

                case 0xc:
                    if(((ins[i]>>11) & 0x1) == 1)
                        strcpy(transfer,"LDMIA ");
                    else
                        strcpy(transfer,"STMIA ");
                    strcat(transfer,resister[(ins[i]>>8)&0x7]); // Ln
                    strcat(transfer,"!,{");
                    int temp = 0;
                    for(int j = 7; j >= 0; j--){
                        if(((ins[i]>>(7-j)) & 0x1) == 1){
                            if(temp == 0){
                                temp == 1;
                                strcat(transfer,resister[7-j]);
                            }else{
                                strcat(transfer,","); 
                                strcat(transfer,resister[7-j]);
                            }
                        }
                    }
                    strcat(transfer,"}");
                    fprintf(out,"%s\n",transfer);
                break;

                case 0xd:
                    if(((ins[i]>>8) & 0xf) < 0xe){
                        strcpy(transfer,"B");
                        strcat(transfer,condition[(ins[i]>>8)&0xf]); // condition
                        strcat(transfer," #0x");
                        int aux2;
                        if(i == 0) aux2 = ins_addr;
                        else aux2 = ins_addr+(0x2);
                        aux = (ins[i]) & 0x7f; // signed 8-bit offset
                        if(((ins[i]>>7) & 0x1) == 0x1) aux = -aux;
                        aux2 += 4+(aux*2);
                        sprintf(_buffer,"%x",(aux2));
                        strcat(transfer,_buffer);
                        fprintf(out,"%s\n",transfer);
                    }else if(((ins[i]>>8) & 0xf) == 0xe){
                        strcpy(transfer,"undefined");
                        fprintf(out,"%s\n",transfer);
                    }else{
                        strcpy(transfer,"SWI #");
                        aux = (ins[i]) & 0xff; // value immed8
                        sprintf(_buffer,"%d",(aux));
                        strcat(transfer,_buffer);
                        fprintf(out,"%s\n",transfer);
                    }
                break;

                case 0xe:
                    if(((ins[i]>>11) & 0x1) == 0x0){
                        strcpy(transfer,"B #0x");
                        int aux2;
                        if(i == 0) aux2 = ins_addr;
                        else aux2 = ins_addr+(0x2);
                        aux = (ins[i]) & 0x3ff; // signed 11-bit offset
                        if(((ins[i]>>10) & 0x1) == 0x1) aux = -aux;
                        aux2 += 4+(aux*2);
                        sprintf(_buffer,"%x",(aux2));
                        strcat(transfer,_buffer);
                        fprintf(out,"%s\n",transfer);
                        pl = 0;
                    }else{
                        strcpy(transfer, "BLX #0x");
                        int aux2;
                        if(i == 0) aux2 = ins_addr;
                        else aux2 = ins_addr+(0x2);
                        aux = (ins[i]>>1) & 0x3ff; // signed 11-bit offset
                        if(((ins[i]>>10) & 0x1) == 0x1) aux = -aux;
                        poff = 0;
                        aux2 = aux2 + 4 + (poff<<12) + (aux*4);
                        aux2 &= ~3;
                        sprintf(_buffer,"%x",(aux2));
                        strcat(transfer,_buffer);
                        fprintf(out,"%s\n",transfer);
                        pl = 1;
                    }
                break;

                case 0xf:
                    if(((ins[i] >> 11) & 0x1) == 0x1){
                        strcpy(transfer, "BL #0x");
                        int aux2;
                        if(i == 0) aux2 = ins_addr;
                        else aux2 = ins_addr+(0x2);
                        aux = (ins[i]>>1) & 0x3ff; // signed 11-bit offset
                        if(((ins[i]>>10) & 0x1) == 0x1) aux = -aux;
                        aux2 = aux2 + 4 + (poff<<12) + (aux*2);
                        sprintf(_buffer,"%x",(aux2));
                        strcat(transfer,_buffer);
                        fprintf(out,"%s\n",transfer);
                        pl = 0;
                    }else{
                        if(pl == 0){
                            strcpy(transfer, "BL #0x");
                            int aux2;
                            if(i == 0) aux2 = ins_addr;
                            else aux2 = ins_addr+(0x2);
                            aux = (ins[i]>>1) & 0x3ff; // signed 11-bit offset
                            if(((ins[i]>>10) & 0x1) == 0x1) aux = -aux;
                            aux2 = aux2 + 4 + (poff<<12) + (aux*2);
                            sprintf(_buffer,"%x",(aux2));
                            strcat(transfer,_buffer);
                            fprintf(out,"%s\n",transfer);
                            pl = 0;
                        }else{
                            strcpy(transfer, "BLX #0x");
                            int aux2;
                            if(i == 0) aux2 = ins_addr;
                            else aux2 = ins_addr+(0x2);
                            aux = (ins[i]>>1) & 0x3ff; // signed 11-bit offset
                            if(((ins[i]>>10) & 0x1) == 0x1) aux = -aux;
                            poff = 0;
                            aux2 = aux2 + 4 + (poff<<12) + (aux*2);
                            sprintf(_buffer,"%x",(aux2));
                            strcat(transfer,_buffer);
                            fprintf(out,"%s\n",transfer);
                            pl = 1;
                        }
                    }
                break;

                default:
                    strcpy(transfer,"undefined");
                    fprintf(out,"%s\n",transfer);
                break;
            }
        }
    }
    fclose(in);
    fclose(out);
    return 0;
}
