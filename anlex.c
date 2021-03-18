/*
 *	Analizador L�xico	
 *	Curso: Compiladores y Lenguajes de Bajo de Nivel
 *	Pr�ctica de Programaci�n Nro. 1
 *	
 *	Descripcion:
 *	Implementa un analizador l�xico que reconoce n�meros, identificadores, 
 * 	palabras reservadas, operadores y signos de puntuaci�n para un lenguaje
 * 	con sintaxis tipo Json.
 *	
 */

/*********** Inclusi�n de cabecera **************/
#include "anlex.h"


/************* Variables globales **************/

token t;				// token global para recibir componentes del Analizador Lexico

// variables para el analizador lexico

FILE *archivo;			// Fuente Json
char lexema[TAMLEX];	// Utilizado por el analizador lexico

int numLinea=1;			// Numero de Linea

/**************** Funciones **********************/


// Rutinas del analizador lexico

void error(const char* mensaje){
	printf("Linea %d: Error. %s.\n",numLinea,mensaje);	
}

void getToken(){
	int i=0;
	char c=0;
	int acepto=0;
	int estado=0;
	char msg[41];
	entrada e;

	while((c=fgetc(archivo))!=EOF){	
		if (c==' ' || c=='\t' || c=='\n'){
			printf("%c", c);
			numLinea++;
		}else if (tolower(c)=='t' || tolower(c)=='T' || tolower(c)=='f' || tolower(c)=='F' || tolower(c)=='n' || tolower(c)=='N'){
			//es un boolean o null
			i=0;
			do{
				lexema[i]=c;
				i++;
				c=fgetc(archivo);
			}while(isalpha(c));
			lexema[i]='\0';
			if (c!=EOF)
				ungetc(c,archivo);
			else
				c=0;
			
			if (strcmp(lexema, "true")==0 || strcmp(lexema, "TRUE")==0){
				strcpy(e.lexema,lexema);
				e.compLex=TRUE;
				insertar(e);
				t.pe=buscar(lexema);
				t.compLex=TRUE;
				strcpy(t.componente,"PR_TRUE");
			}else if (strcmp(lexema, "false")==0 || strcmp(lexema, "FALSE")==0){
				strcpy(e.lexema,lexema);
				e.compLex=FALSE;
				insertar(e);
				t.pe=buscar(lexema);
				t.compLex=FALSE;
				strcpy(t.componente,"PR_FALSE");
			}else if (strcmp(lexema, "null")==0 || strcmp(lexema, "NULL")==0){
				strcpy(e.lexema,lexema);
				e.compLex=A_NULL;
				insertar(e);
				t.pe=buscar(lexema);
				t.compLex=A_NULL;
				strcpy(t.componente,"PR_NULL");
			}else{
				error("No se reconoce");
			}
			break;
		}else if (isdigit(c)){
			//es un numero
			i=0;
			estado=0;
			acepto=0;
			lexema[i]=c;
			
			while(!acepto){
				switch(estado){
				case 0: //una secuencia netamente de digitos, puede ocurrir . o e
					c=fgetc(archivo);
					if (isdigit(c)){
						lexema[++i]=c;
						estado=0;
					}else if(c=='.'){
						lexema[++i]=c;
						estado=1;
					}else if(tolower(c)=='e'){
						lexema[++i]=c;
						estado=3;
					}else{
						estado=6;
					}
					break;
				case 1://un punto, debe seguir un digito (caso especial de array, puede venir otro punto)
					c=fgetc(archivo);						
					if (isdigit(c)){
						lexema[++i]=c;
						estado=2;
					}else{
						sprintf(msg,"No se esperaba '%c'",c);
						estado=-1;
					}
					break;
				case 2://la fraccion decimal, pueden seguir los digitos o e
					c=fgetc(archivo);
					if (isdigit(c)){
						lexema[++i]=c;
						estado=2;
					}else if(tolower(c)=='e'){
						lexema[++i]=c;
						estado=3;
					}else
						estado=6;
					
					break;
				case 3://una e, puede seguir +, - o una secuencia de digitos
					c=fgetc(archivo);
					if (c=='+' || c=='-'){
						lexema[++i]=c;
						estado=4;
					}else if(isdigit(c)){
						lexema[++i]=c;
						estado=5;
					}else{
						sprintf(msg,"No se esperaba '%c'",c);
						estado=-1;
					}
					break;
				case 4://necesariamente debe venir por lo menos un digito
					c=fgetc(archivo);
					if (isdigit(c)){
						lexema[++i]=c;
						estado=5;
					}else{
						sprintf(msg,"No se esperaba '%c'",c);
						estado=-1;
					}
					break;
				case 5://una secuencia de digitos correspondiente al exponente
					c=fgetc(archivo);
					if (isdigit(c)){
						lexema[++i]=c;
						estado=5;
					}else{
						estado=6;
					}break;
				case 6://estado de aceptacion, devolver el caracter correspondiente a otro componente lexico
					if (c!=EOF)
						ungetc(c,archivo);
					else
						c=0;
					
					lexema[++i]='\0';
					acepto=1;
					t.pe=buscar(lexema);
					if (t.pe->compLex==-1){
						strcpy(e.lexema,lexema);
						e.compLex=NUM;
						insertar(e);
						t.pe=buscar(lexema);
					}
					t.compLex=NUM;
					strcpy(t.componente,"NUMBER");
					break;
				case -1:
					if (c==EOF)
						error("No se esperaba el fin de archivo");
					else
						error(msg);
					exit(1);
				}
			}
			break;
		}else if (c==':'){
			t.compLex=':';
			t.pe=buscar(":");
			strcpy(t.componente,"DOS_PUNTOS");
			break;
		}else if (c==','){
			t.compLex=',';
			strcpy(t.componente,"COMA");
			t.pe=buscar(",");
			break;
		}else if (c=='['){
			t.compLex='[';
			t.pe=buscar("[");
			strcpy(t.componente,"L_CORCHETE");
			break;
		}else if (c==']'){
			t.compLex=']';
			t.pe=buscar("]");
			strcpy(t.componente,"R_CORCHETE");
			break;
		}
		else if (c=='\"'){ 
			//un caracter o una cadena de caracteres
			i=0;
			lexema[i]=c;
			i++;
			do{
				c=fgetc(archivo);
				if (c=='\"'){
					c=fgetc(archivo);
					if (c=='\"'){
						lexema[i]=c;
						i++;
						lexema[i]=c;
						i++;
					}else{
						lexema[i]='\"';
						i++;
						break;
					}
				}else if(c==EOF){
					error("Se llego al fin de archivo sin finalizar la cadena");
					break;
				}else{	
					lexema[i]=c;
					i++;
				}
			}while(c!=EOF);
			lexema[i]='\0';
			if (c!=EOF)
				ungetc(c,archivo);
			else
				c=0;
			
			t.pe=buscar(lexema);
			t.compLex=t.pe->compLex;
			if (t.pe->compLex==-1){
				strcpy(e.lexema,lexema);
				e.compLex=STRING;
				insertar(e);
				t.pe=buscar(lexema);
			}
			t.compLex=STRING;
			strcpy(t.componente,"STRING");
			break;
		}else if (c=='{'){
			t.compLex='{';
			t.pe=buscar("{");
			strcpy(t.componente,"L_LLAVE");
			break;
		}else if (c=='}'){
			t.compLex='}';
			t.pe=buscar("}");
			strcpy(t.componente,"R_LLAVE");
			break;
		}else if (c!=EOF){
			sprintf(msg,"%c no esperado",c);
			error(msg);
		}
	}if (c==EOF){
		t.compLex=EOF;
		t.pe=&e;
	}	
}

int main(int argc,char* args[]){
	// inicializar analizador lexico
	initTabla();
	initTablaSimbolos();
	
	if(argc > 1){
		if (!(archivo=fopen(args[1],"rt"))){
			printf("Archivo no encontrado.\n");
			exit(1);
		}while (t.compLex!=EOF){
			getToken();
			printf("%s ", t.componente);
		}
		fclose(archivo);
	}else{
		printf("Debe pasar como parametro el path al archivo fuente.\n");
		exit(1);
	}
	return 0;
}