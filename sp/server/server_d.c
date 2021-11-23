#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <time.h> 
#include <arpa/inet.h> 
#include <netdb.h>
#include <sys/un.h>
#include <unistd.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <math.h>


/**
 * GLobální proměnné 
 */


int server_socket;			/* Deklarace server socketu */
int pole_klientu[5][15]; 	/* Deklarace globalniho pole, do ktereho si ukladam veskere potrebne udaje o hrach a o hracich */  // 0 sloupec id hry, 1 sloupec socket klienta, 2 sloupec aktivni radek_klientac 0 neradek_klientaje 1 radek_klientaje, 4 poradi radek_klientace, aktivni hra
char* jmena[2][15];			/* Deklarace pole, pro ukladani jmen klientu */
int pocet = 0;				/* Deklarace globalni pomocne promenne */
FILE *soubor;				/* Deklarace souboru */
time_t cas1, cas2;			/* Deklarace casu pro celkovy cas behu serveru */
int celkemP;				/* Deklarace poctu pro celkovy soucet paketu */
int celkemB;				/* Deklarace poctu pro celkovy soucet bytu */


/* ____________________________________________________________________________

    struktura otazka

    Deklarace struktury pro jednotlive otazky, hadanka odpovida otazce, ktera
    ma prislusne moznosti a,b,c, dalsi kolonka je spravna odpoved 
   ____________________________________________________________________________
 */
typedef struct {
	const char *hadanka;
	const char *a;
	const char *b;
	const char *c;
	const char *odpoved;
	const char *skupina;
}otazka;


/* ____________________________________________________________________________

    int aktivni_hrac(int radek_klienta)

    Funkce pro zjisteni aktivniho hrace. Prijmaci parametr je radek_klienta,
    ktery odpovida klientoci, co si funkci zavolal. Vraci radek aktivniho hrace
    v prislusne hre.
   ____________________________________________________________________________
 */
int aktivni_hrac(int radek_klienta){
	int i;
	for(i=0;i<15;i++){
		if(pole_klientu[0][i]==pole_klientu[0][radek_klienta] && pole_klientu[2][i]==1){
			return i; 
		}
	}	
}


/* ____________________________________________________________________________

    void nastav_aktivni(int radek_klienta)

    Funkce ktera nastavi nasledujici aktivniho hrace v dane hre. Prijmaci parametr
    radek_klienta, ktery odpovida klientoci, co si funkci zavolal. 
   ____________________________________________________________________________
 */
void nastav_aktivni(int radek_klienta){
	int i;

	int pristi_aktivni;
	pristi_aktivni = -1;
	for(i=radek_klienta+1;i<15;i++){
		if(pole_klientu[0][i]==pole_klientu[0][radek_klienta]){
			pristi_aktivni = i;	
			break;
		}
	}
	if(pristi_aktivni==-1){
		for(i=0;i<radek_klienta;i++){
			if(pole_klientu[0][i]==pole_klientu[0][radek_klienta]){
				pristi_aktivni = i;
				break;
			}
		}
	}
	pole_klientu[2][pristi_aktivni] = 1;
}

/* ____________________________________________________________________________

   otazka vytvor_otazku(const char *hadanka, const char *a, const char *b, const char *c, const char *odpoved, const char *skupina)

   Funkce pro vytvoreni otazky. Parametry jsou otazka, moznosti, odpoved a prislusna skupina.
   Funkce vraci ukazatel na vytovrenou otazku a uklada si ji na pozici v poli.  
   ____________________________________________________________________________
 */
otazka vytvor_otazku(const char *hadanka, const char *a, const char *b, const char *c, const char *odpoved, const char *skupina){

	otazka *ot;        
	ot = (otazka *) malloc ( sizeof(*ot) );

	ot->hadanka = hadanka;
	ot->a = a;
	ot->b = b;
	ot->c = c;
	ot->odpoved = odpoved;
	ot->skupina = skupina;

	return *ot;      
}

otazka pole[18];			/* Deklarace pole s otazkami (pole struktur) */

/* ____________________________________________________________________________

    void zapis(int client_socket, int pocet, int prijem)

    Funkce zapisujici potrebna data do souboru. Parametry jsou klientsky socket
    a cislo, co znaci, co se bude zapisovat. Kdyz je prijema 0 - server odeslal,
    1 - server prijal, 2 - klient se odpojil, 3 - klient se pripojil.  
   ____________________________________________________________________________
 */
void zapis(int client_socket, int pocet, int prijem){
	time_t cas;
	struct tm *ltmcas;
	cas = time(NULL);
	ltmcas = localtime(&cas);
	int packet;
	double p;
	if(client_socket==0){
		soubor = fopen("serverInfo.txt", "w");
		fclose(soubor);
		soubor = fopen("serverInfo.txt", "a+");
		(void) time(&cas1);
		fprintf(soubor,"--------------------------------------------------\n");  
		fprintf(soubor,"Server startuje v case %s a nasloucha na portu: %d\n", ctime(&cas),pocet);	
		fprintf(soubor,"--------------------------------------------------\n");
	}
	else if(client_socket==-1){
		cas2 = time(NULL); 
		packet = (unsigned int) difftime(cas2, cas1);
		p = ((double)packet)/60;
		fprintf(soubor,"--------------------------------------------------\n"); 
		fprintf(soubor,"Server konci v case %s\n", ctime(&cas));
		fprintf(soubor,"Celkem bylo odeslano %02d bytu a %02d packetu\n", celkemB, celkemP); 	
		fprintf(soubor,"Server bezel %.2f minut\n", p);
		fprintf(soubor,"--------------------------------------------------\n");
		fclose(soubor);
	}
	else if(prijem==3){
		fprintf(soubor,"%02i:%02i:%02i - Se pripoji novy klient se socketem %02d\n",ltmcas->tm_hour, ltmcas->tm_min,ltmcas->tm_sec, client_socket);							
	}
	else if(prijem==2){
		fprintf(soubor,"%02i:%02i:%02i - Se odpojil klient se socketem %02d\n",ltmcas->tm_hour, ltmcas->tm_min,ltmcas->tm_sec,client_socket);							
	}
	else{
		if(pocet<1500){
			packet = 1;
		}
		else{
			packet = 2;
		}

		if(prijem==0){
			fprintf(soubor,"%02i:%02i:%02i - Klientovi bylo odeslano  --- bytu - %02d --- packetu - %02d\n",ltmcas->tm_hour, ltmcas->tm_min,ltmcas->tm_sec, pocet, packet);			
			celkemP = celkemP + packet;
			celkemB = celkemB + pocet;
		}	
		else{
			fprintf(soubor,"%02i:%02i:%02i - Server prijal od klienta --- bytu - %02d --- packetu - %02d\n",ltmcas->tm_hour, ltmcas->tm_min,ltmcas->tm_sec, pocet,packet);
			celkemP = celkemP + packet;
			celkemB = celkemB + pocet;
		}
	}
}

/* ____________________________________________________________________________

    int prijem(int client_socket)

    Funkce do ktere skoci vlakno po spravnem pripojeni k serveru. Zde probiha
    veskere vyhodnocovani od klienta. Parametr je klientsky socket.   
   ____________________________________________________________________________
 */
int prijem(int client_socket){
	int client_sock_In = 0;
	char cbuf[256]; //null
	int client_sock_Out = 0;
	int i,k,x,z,skupina;
	char skup[256];
	char pokus[256];
	char * pch;
	char blabol[255][255];
	int radek_klienta;
	char p[] = "X";
	char u[] = "ukonci";
	int pocet_radek_hracu, je_aktivni;

	while(1){
		int res;
		bzero(pokus,256);

		res = recv(client_socket, &cbuf, 256*sizeof(char), 0);
		if(res!=0){
			zapis(client_socket, res, 1);	
		}
		for(i=0;i<15;i++){
			if(client_socket==pole_klientu[1][i]){
				radek_klienta = i;
				break;
			}
		}
		if(strcmp(cbuf, p)==0){
			pocet_radek_hracu=0;
			for(k=0; k<15; k++){
				if(pole_klientu[0][k]==pole_klientu[0][radek_klienta]){
					pocet_radek_hracu=pocet_radek_hracu+1;
				}
			}
			strcat(pokus, "Z");
			strcat(pokus, ";");
			strcat(pokus, "odpojilse");
			strcat(pokus, ";");
			char *poradi_odpojeneho = (char *) malloc (sizeof(char)*2);	
			sprintf(poradi_odpojeneho, "%d", pole_klientu[3][radek_klienta]);
			strcat(pokus, poradi_odpojeneho);	
			x = pole_klientu[0][radek_klienta];
			z = pole_klientu[3][radek_klienta];
			zapis(pole_klientu[1][radek_klienta],0,2);
			pole_klientu[0][radek_klienta] = 0;
			pole_klientu[1][radek_klienta] = 0;
			pole_klientu[2][radek_klienta] = 0;
			pole_klientu[3][radek_klienta] = 0;
			for(k=0;k<15;k++){
				if(pole_klientu[0][k] == x){
					res = send(pole_klientu[1][k], pokus, strlen(pokus), 0);
					zapis(pole_klientu[1][k], res, 0);
				}
			}
			bzero(pokus,256);
			strcat(pokus, "Z");
			strcat(pokus, ";");
			strcat(pokus, "hraj");	

			je_aktivni=-1;
			for(k=z+1;k<15;k++){
				if(pole_klientu[0][k]==x){
					je_aktivni=k;
					res = send(pole_klientu[1][k], pokus, strlen(pokus), 0);
					zapis(pole_klientu[1][k], res, 0);
					break;
				}				
			}
			usleep(50000);
			if(je_aktivni==-1){
				for(k=0;k<z;k++){
					if(pole_klientu[0][k]==x){
						pole_klientu[3][k] = 1;
						res = send(pole_klientu[1][k], pokus, strlen(pokus), 0);	
						zapis(pole_klientu[1][k], res, 0);
						break;
					}
				}
			}
			if(pocet_radek_hracu==2){
				for(k=0; k<15; k++){
					if((pole_klientu[0][k]==x && pole_klientu[4][k]==1) || (pole_klientu[0][k]==0 && pole_klientu[4][k]==1)){
						pole_klientu[0][k]=0;
						pole_klientu[1][k]=0;
						pole_klientu[2][k]=0;
						pole_klientu[3][k]=0;
						pole_klientu[4][k]=0;
					}
				}

			}
			return 1;
		}


		for(k=0; k<255;k++){
			bzero(blabol[k],255);
		}

		for(i=0; i<18; i++){				
			if(strcmp(pole[i].skupina,cbuf)==0){				
				bzero(skup, 256);
				strncpy(skup, cbuf, strlen(cbuf));			
				strcat(pokus, "Z");
				strcat(pokus, ";");
				strcat(pokus, "otazka");
				strcat(pokus, ";");
				strcat(pokus, pole[i].hadanka);
				strcat(pokus, ";");
				strcat(pokus, pole[i].a);
				strcat(pokus, ";");
				strcat(pokus, pole[i].b);
				strcat(pokus, ";");
				strcat(pokus, pole[i].c);
				res = send(client_socket, pokus, strlen(pokus), 0);
				zapis(client_socket, res, 0);
				break;
			}
			else if(cbuf[0]=='P'){	  
				k = 0;
				pch = strtok (cbuf,";");
				while (pch != NULL)
				{					
					strcpy(blabol[k],pch);
					pch = strtok (NULL, ";");
					k++;
				}
				if(strcmp(blabol[1],pole[i].hadanka)==0){
					skupina = i; 
				}
				if(strcmp(blabol[1],pole[i].hadanka)==0 && strcmp(blabol[2],pole[i].odpoved)==0){
					strcat(pokus, "Z");
					strcat(pokus, ";");
					strcat(pokus, "odpoved");
					strcat(pokus, ";");
					strcat(pokus, "spravne");
					strcat(pokus, ";");
					strcat(pokus, pole[i].skupina);
					strcat(pokus, ";");
					char *hrajici2 = (char *) malloc (sizeof(char)*2);	
					sprintf(hrajici2, "%d", pole_klientu[3][radek_klienta]);
					strcat(pokus, hrajici2);	
					for(x=0; x<15;x++){
						if(pole_klientu[0][x] == pole_klientu[0][radek_klienta]){
							res = send(pole_klientu[1][x], pokus, strlen(pokus), 0);
							zapis(pole_klientu[1][x], res, 0);
						}
					}
					pole_klientu[2][radek_klienta]=0; 
					nastav_aktivni(radek_klienta);								 						 		 
					break;
				}
				else if(i==17){
					strcat(pokus, "Z");
					strcat(pokus, ";");
					strcat(pokus, "odpoved");
					strcat(pokus, ";");
					strcat(pokus, "spatne");
					strcat(pokus, ";");
					strcat(pokus, pole[skupina].skupina);
					strcat(pokus, ";");
					char *hrajici2 = (char *) malloc (sizeof(char)*2);	
					sprintf(hrajici2, "%d", pole_klientu[3][radek_klienta]);
					strcat(pokus, hrajici2);
					for(x=0; x<15;x++){
						if(pole_klientu[0][x] == pole_klientu[0][radek_klienta]){
							res = send(pole_klientu[1][x], pokus, strlen(pokus), 0);
							zapis(pole_klientu[1][x], res, 0);
						}
					}
					pole_klientu[2][radek_klienta]=0;
					nastav_aktivni(radek_klienta);
					break;

				}	 
			}  
		}


		if(cbuf[0]=='T'){
			pocet_radek_hracu = 0;
			for(i=0;i<15;i++){
				if(pole_klientu[0][i]==pole_klientu[0][radek_klienta]){
					pocet_radek_hracu=pocet_radek_hracu+1;
				}
			}
			if(pocet_radek_hracu==3){
				bzero(pokus, 256);
				int prvni;
				prvni = 0;
				strcat(pokus, "Z");
				strcat(pokus, ";");
				strcat(pokus, "jmena");	
				for(i=0;i<15;i++){
					if(pole_klientu[0][i]==pole_klientu[0][radek_klienta] && pole_klientu[0][radek_klienta]==atoi(jmena[0][i])){
						strcat(pokus, ";");
						strcat(pokus, jmena[1][i]);	
					}
				}					
				for(i=0;i<15;i++){					
					if(pole_klientu[0][i]==pole_klientu[0][radek_klienta] && pole_klientu[0][radek_klienta]==atoi(jmena[0][i])){
						res = send(pole_klientu[1][i], pokus, strlen(pokus), 0);
						zapis(pole_klientu[1][i], res, 0);
					}
				}
				for(i=0;i<15;i++){					
					if(pole_klientu[0][i]==pole_klientu[0][radek_klienta]){
						pole_klientu[4][i]=1;								
					}
				}
				bzero(pokus, 256);				
				int pomocna;
				pomocna = pole_klientu[0][radek_klienta];
				prvni = 0;
				for(i=0;i<15;i++){
					if(pole_klientu[0][i]==pomocna){
						prvni = i;
						break;
					}
				}
				pole_klientu[2][prvni] = 1;														
				strcat(pokus, "Z");
				strcat(pokus, ";");
				strcat(pokus, "hraj");				
				res = send(pole_klientu[1][prvni], pokus, strlen(pokus), 0);
				zapis(pole_klientu[1][prvni], res, 0);
				bzero(pokus, 256);
				strcat(pokus, "Z");
				strcat(pokus, ";");
				strcat(pokus, "hrajedalsi");
				strcat(pokus, ";");
				strcat(pokus, "1");
				for(i=0;i<15;i++){
					if(pole_klientu[0][i]==pole_klientu[0][radek_klienta] && pole_klientu[2][i]==0){
						res = send(pole_klientu[1][i], pokus, strlen(pokus), 0);
						zapis(pole_klientu[1][i], res, 0);
					}
				}							
				pole_klientu[2][prvni] = 0;			
			}
			else{
				bzero(pokus, 256);
				strcat(pokus, "Z");
				strcat(pokus, ";");
				strcat(pokus, "malo");		
				for(i=0;i<15;i++){
					if(pole_klientu[0][i]==pole_klientu[0][radek_klienta]){
						res = send(pole_klientu[1][i], pokus, strlen(pokus), 0);
						zapis(pole_klientu[1][i], res, 0);
					}
				}
			}
		}
		

		if(strcmp(cbuf, u)==0){
			for(k=0;k<15;k++){
				if(pole_klientu[0][k]==pole_klientu[0][radek_klienta]){
					pole_klientu[0][k] = 0;
					pole_klientu[1][k] = 0;
					pole_klientu[2][k] = 0;
					pole_klientu[3][k] = 0;
					pole_klientu[4][k] = 0;
				}
			}	
		}

		if(cbuf[0]=='D'){

			pocet_radek_hracu = 0;
			for(k=0;k<15;k++){
				if(pole_klientu[0][k]==pole_klientu[0][radek_klienta]){
					pocet_radek_hracu=pocet_radek_hracu+1;
				}
			}

			pocet = pocet + 1;
			int i;
			if((pocet == 3 && pocet_radek_hracu==3) || (pocet == 2 && pocet_radek_hracu==2)) {

				int radek_aktivni;
				int radek_ak;
				radek_ak = aktivni_hrac(radek_klienta);


				bzero(pokus,256);
				strcat(pokus, "Z");
				strcat(pokus, ";");
				strcat(pokus, "hraj");				
				res = send(pole_klientu[1][radek_ak], pokus, strlen(pokus), 0);
				zapis(pole_klientu[1][radek_ak], res, 0);
				usleep(50000);
				bzero(pokus, 256);
				strcat(pokus, "Z");
				strcat(pokus, ";");
				strcat(pokus, "hrajedalsi");

				for(i=0;i<15;i++){
					if(pole_klientu[2][i]==0 && pole_klientu[0][i]==pole_klientu[0][radek_ak]){
						res = send(pole_klientu[1][i], pokus, strlen(pokus), 0);
						zapis(pole_klientu[1][i], res, 0);
					}
				}					

				pocet = 0;

			}
		}		
		bzero(cbuf, 256);
	}
}

/* ____________________________________________________________________________

    void *vyrizeni_klienta(void * args)

    Funkce vlakna. Zde prijmam klientske spojeni, ktere je bud v poradku a pokracuju,
    nebo jej rovnou zavrhnu. Parametr je beztypovy, ale dalo by se rict, ze je to
    cislo vlakna.  
   ____________________________________________________________________________
 */
void *vyrizeni_klienta(void * args){
	int client_socket;
	int len_addr;
	struct sockaddr_in peer_addr;
	int i,k;
	char pokus[256];
	char cbuf[256]; 
	len_addr = sizeof(peer_addr);


	char * pch;
	char zprava[255][255];
	int res;
	int timeout = 3; // 30 s

	while(1){
		client_socket = accept(server_socket, (struct sockaddr *) &peer_addr, &len_addr);
		if (client_socket>0) {
			printf("Hura nove spojeni\n");
			printf("Prijato spojeni z adresy ");
			printf("%s\n",inet_ntoa(peer_addr.sin_addr));
			zapis(client_socket, 0, 3);
			bzero(pokus,256);
			res = recv(client_socket, &cbuf, 256*sizeof(char), 0);
			if(res!=0){
				zapis(client_socket, res, 1);	
			}
			k = 0;
			pch = strtok (cbuf,";");
			while (pch != NULL)
			{					
				strcpy(zprava[k],pch);
				pch = strtok (NULL, ";");
				k++;
			}
			int pocet_radek_klientu = 0;
			int chyba = 0;
			for(i=0; i<15; i++){
				if(pole_klientu[0][i]==atoi(zprava[0])){
					pocet_radek_klientu=pocet_radek_klientu+1;
				}
				if(pole_klientu[0][i]==atoi(zprava[0]) && pole_klientu[4][i]==1){
					chyba=chyba+1;
				}
			}


			if(pocet_radek_klientu<3 && chyba==0){
				for(i=0;i<15;i++){
					if(pole_klientu[0][i]==0){
						pole_klientu[0][i]=atoi(zprava[0]);
						pole_klientu[1][i]=client_socket;
						pole_klientu[2][i]=0;
						pole_klientu[3][i]=pocet_radek_klientu+1;
						pole_klientu[4][i]=0;
						printf("%d klient a ma socket: %d, id hry je %d a je %d.hrĂˇÄŤem a leĹľĂ­ na Ĺ™Ăˇdku %d a pĹ™ezdĂ­vka je %s\n", i+1,client_socket,atoi(zprava[0]),pocet_radek_klientu+1,i,zprava[1]);						
						bzero(pokus,256);
						jmena[0][i] = zprava[0];
						jmena[1][i] = zprava[1];
						strcat(pokus, "Z");
						strcat(pokus, ";");
						strcat(pokus, "nehraj");
						res = send(client_socket, pokus, strlen(pokus), 0);
						zapis(client_socket, res, 0);
						prijem(client_socket);
						break;
					}
				}
			}
			else{
				strcat(pokus, "Z");
				strcat(pokus, ";");
				strcat(pokus, "obsazeno");
				strcat(pokus, ";");
				strcat(pokus, "Ty ulicniku.. uz o tobe vim...");				
				res = send(client_socket, pokus, strlen(pokus), 0);
				zapis(client_socket, res, 0);
				zapis(client_socket, 0, 3);
			}
			close(client_socket);

		}
		else {
			printf ("Brutal Fatal ERROR - klient se nedokĂˇzal pĹ™ipojit\n");
		}
	}

}
/* ____________________________________________________________________________

    void online()

   Funkce pro vypsani online hracu na serveru.   
   ____________________________________________________________________________
 */
void online(){
	int i;
	char aktivni[4];
	printf("Vypis vsech pripojenych hracu\n");
	printf("-----------------------------\n");
	for(i=0; i<15; i++){
		if(pole_klientu[1][i]!=0){
			if(pole_klientu[4][i]==1){
				strcpy(aktivni, "ano");
			}
			else{
				strcpy(aktivni, "ne");
			}
			printf("Hrac %s se socketem %d je ve hre %d a hra je aktivni %s\n", jmena[1][i],pole_klientu[1][i],pole_klientu[0][i],aktivni);	
		}
	}
	printf("-----------------------------\n");
}
/* ____________________________________________________________________________

    void *poslouchej(void * arg)

   Funkce vlakna pro naslouchani na serveru. Prikaz whoisonline vypise vsechny 
   pripojene, help napovi povolene prikazy a exit ukonci server.    
   ____________________________________________________________________________
 */
void *poslouchej(void * arg){
	int i;
	while(1){
		char prijmi[100];

		char prezdivka[30];
		scanf("%s",&prijmi);
		if(strcmp(prijmi, "exit")==0){
			printf("koncime..\n");
			zapis(-1,0,0);
			exit(0);
		}
		else if(strcmp(prijmi, "whoisonline")==0){
			online();
		}
		else if(strcmp(prijmi, "help")==0){
			printf("Seznam prikazu na server\n");
			printf("-----------------------------\n");
			printf("1)whoisonline - vypise seznam pripojenych hracu\n");
			printf("2)help - napoveda\n");
			printf("3)exit - ukonci server\n");
			printf("-----------------------------\n");
		}
		else{
			printf("Spatny vstup, pro napovedu muzete vyuzit prikaz help\n");
			printf("-----------------------------\n");
		}

	}


}


/* ____________________________________________________________________________

    int main (int argc, char *argv[])

   Hlavni fuknce serveru. Zde projistotu vse nuluji, nacitam port, zdali je zadany
   v parametru, vytvarim vlakna... Nastavuji port, na kterym server mam naslouchat.  
   ____________________________________________________________________________
 */
int main (int argc, char *argv[]){

	int return_value;
	char cbuf[256];

	struct sockaddr_in my_addr;
	int i,k;
	int port;

	if(argc==2){
		port = atoi(argv[1]);
		if(port<65535 && port>1024){
			printf("Port je nastaven na %d\n", port);
		}
		else{
			port = 10000;
			printf("Port je nastaven defaultnÄ› na %d\n", port);
		}
	}
	else{
		port = 10000;
		printf("Port je nastaven defaultnÄ› %d\n",port);
	}


	pthread_t vlakna[16]; 
	pthread_t naslouchej;
	pthread_attr_t attr;



	server_socket = socket(AF_INET, SOCK_STREAM, 0);

	int flag = 1;
	setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));

	memset(&my_addr, 0, sizeof(struct sockaddr_in)); // nulovani pameti


	for(i=0;i<4;i++){
		for(k=0;k<15;k++){
			pole_klientu[i][k] = 0;
		}
	}

	for(i=0;i<2;i++){
		for(k=0;k<15;k++){
			jmena[i][k] = '\0';
		}
	}

	pole[0] = vytvor_otazku("JakĂˇ je nejvyĹˇĹˇĂ­ hora Afriky?", "KilimandĹľĂˇro", "Mount Kenya", "Makalu", "KilimandĹľĂˇro", "z1");
	pole[1] = vytvor_otazku("JakĂ© je nejvÄ›tĹˇĂ­ jezero svÄ›ta (podle rozlohy)?", "MichiganĹľĂˇro", "KaspickĂ© moĹ™e", "Bajkal", "KaspickĂ© moĹ™e", "z2");
	pole[2] = vytvor_otazku("JakĂ© barvy se nachĂˇzĂ­ na GuatemalskĂ© vlajce?", "bĂ­lĂˇ, ÄŤervenĂˇ, zelenĂˇ", "bĂ­lĂˇ, modrĂˇ, ĹľlutĂˇ","bĂ­lĂˇ, modrĂˇ","bĂ­lĂˇ, modrĂˇ", "z3");
	pole[3] = vytvor_otazku("PrvnĂ­m prezidentem USA byl?", "Abraham Lincoln", "George Washington","Thomas Jefferson","George Washington", "h1"); 
	pole[4] = vytvor_otazku("Bitva u Waterloo se odehrĂˇla na ĂşzemĂ­ dneĹˇnĂ­:", "Belgie", "Francie","Anglie","Belgie", "h2"); 
	pole[5] = vytvor_otazku("JakĂ©ho roku byl JezuitskĂ˝ Ĺ™Ăˇd zaloĹľen?", "1243", "1132","1534","1534", "h3");
	pole[6] = vytvor_otazku ("Do jakĂ©ho slotu nelze vloĹľit sĂ­ĹĄovĂˇ karta?", "ISA", "PCI","AGP","AGP", "t1");
	pole[7] = vytvor_otazku("KterĂ˝ parametr je nepotĹ™ebnĂ˝ u procesoru?", "taktovacĂ­ kmitoÄŤet", "poÄŤet otĂˇÄŤek za minutu","vyrovnĂˇvacĂ­ pamÄ›ĹĄ","poÄŤet otĂˇÄŤek za minutu", "t2");      
	pole[8] = vytvor_otazku("NAT tabulka slouĹľĂ­?", "k rozdÄ›lenĂ­ velkĂ˝ch objemĹŻ dat na bloky ethernet", "k pĹ™ekladu adres mezi privĂˇtnĂ­ a veĹ™ejnou sĂ­tĂ­","k fragmentaci datagramĹŻ na rĂˇmce","k pĹ™ekladu adres mezi privĂˇtnĂ­ a veĹ™ejnou sĂ­tĂ­", "t3");   
	pole[9] = vytvor_otazku("Kdo vyhrĂˇl mistrovstvĂ­ Evropy ve fotbale v roce 2008?",  "Portugalsko", "NÄ›mecko","Ĺ panÄ›lsko","Ĺ panÄ›lsko", "s1");
	pole[10] = vytvor_otazku("KterĂ˝ hrĂˇÄŤ jako prvnĂ­ v NHL pĹ™ekonal hranici1000 bodĹŻ?", "Peter Ĺ ĹĄastnĂ˝", "Wayne Gretzky","Gordie Howe","Gordie Howe", "s2");
	pole[11] = vytvor_otazku("KterĂ˝ prvnĂ­ evropskĂ˝ hrĂˇÄŤ,jako prvnĂ­ zĂ­skal cenu Hart Trophy?","JaromĂ­r JĂˇgr", "Sergej Fedorov","Cook Bun","Sergej Fedorov", "s3");
	pole[12] = vytvor_otazku("ParnĂ­ stroj zkonstruoval?", "Salvino degli Armati", "James Watt","FrantiĹˇek KĹ™iĹľĂ­k","James Watt", "v1");
	pole[13] = vytvor_otazku("BratĹ™i LumierovĂ© vynalezli kinematografickĂ˝ pĹ™Ă­stroj, kterĂ˝ nebyl zĂˇroveĹ?", "pĹ™Ă­strojem na kopĂ­rovĂˇnĂ­ filmĹŻ", "kamerou","pĹ™Ă­strojem na nahrĂˇvĂˇnĂ­ filmĹŻ","pĹ™Ă­strojem na nahrĂˇvĂˇnĂ­ filmĹŻ", "v2");
	pole[14] = vytvor_otazku("PrvnĂ­ zrcadlovĂ˝ dalekohled zkonstruoval?", "Francouz Marin Mersenne", "HolanÄŹan Hans Lippershey","AngliÄŤan William Lee","AngliÄŤan William Lee", "v3");
	pole[15] = vytvor_otazku("Jmenujte herce, kteĹ™Ă­ tvoĹ™ili TĹ™i veterĂˇny.", "MiloĹˇ KopeckĂ˝, LubomĂ­r LipskĂ˝, JiĹ™Ă­ KrytinĂˇĹ™", "Josef Somr, Rudolf HruĹˇĂ­nskĂ˝, Petr ÄŚepek","JĂşlius SatinskĂ˝, ZdenÄ›k SvÄ›rĂˇk, Milan Lasica","Josef Somr, Rudolf HruĹˇĂ­nskĂ˝, Petr ÄŚepek", "f1"); 
	pole[16] = vytvor_otazku("Jak pojmenoval Pavel KĹ™Ă­Ĺľ alias Ĺ tÄ›pĂˇn Ĺ afrĂˇnek v Â»bĂˇsnĂ­cĂ­chÂ« tĹ™i svoje nejvÄ›tĹˇĂ­ lĂˇsky?", "JahĹŻdka, FlĂ©tniÄŤka, Ĺ katulka", "Vanilka, PoĹˇtulka, MuĹˇliÄŤka","BorĹŻvka, PĂ­ĹˇĹĄalka, JeskyĹka","BorĹŻvka, PĂ­ĹˇĹĄalka, JeskyĹka", "f2");
	pole[17] = vytvor_otazku("â€žJĂˇ, Ĺľe jsem tajtrlĂ­k? Za prvĂ© nevĂ­m, co to je a za druhĂ© mÄ› to urĂˇĹľĂ­!â€ś Z kterĂ©ho filmu je tato hlĂˇĹˇka?", "Jak dostat tatĂ­nka do polepĹˇovny", "VesniÄŤko mĂˇ, stĹ™ediskovĂˇ","Byl jednou jeden krĂˇl","Byl jednou jeden krĂˇl", "f3");

	zapis(0,port,0);


	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(port);
	my_addr.sin_addr.s_addr = INADDR_ANY;

	return_value = bind(server_socket, (struct sockaddr *) &my_addr, \
			sizeof(struct sockaddr_in));

	if (return_value == 0) 
		printf("Bind - OK\n");
	else {
		printf("Bind - ERR\n");
		return -1;
	}

	return_value = listen(server_socket, 5);

	if (return_value == 0) 
		printf("Listen - OK\n");
	else {
		printf("Listen - ERR\n");
		return -1;
	}

	pthread_attr_init( &attr);
	pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_JOINABLE);	

	if(pthread_create( &naslouchej, NULL, &poslouchej, (void *)1) != 0){
		printf("Nemohl jsem vytvoĹ™it vlĂˇkno");
		return -1;
	}

	for(i=0; i<16; i++){
		if(pthread_create( &vlakna[i], &attr, &vyrizeni_klienta, (void *)(intptr_t) i) != 0){
			printf("Nemohl jsem vytvoĹ™it vlĂˇkno");
			return -1;
		}
	}



	pthread_attr_destroy( &attr);

	for(i=0;i<16;i++ ){
		pthread_join(vlakna[i], NULL);
	}

	return 0;
}











