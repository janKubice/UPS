/**
 * Server pro hru
 * prelozi se prikazem gcc server.c -lpthread -o server
 * 
 * Author: Jan Kubice
 * Date: 27.12.2021
 * Version: Fakt obri cislo
 */ 

#include<stdio.h>
#include<string.h>    
#include<stdlib.h>    
#include<sys/socket.h>
#include<arpa/inet.h> 
#include<unistd.h> 
#include<pthread.h>
#include<ctype.h>
#define CLIENT_MSG_SIZE 1024
#define MAX_SERVER_PLAYERS 128
#define MAX_SERVER_GAMES 64
#define MAX_GAME_PLAYERS 5
#define WORDS 10
#define MIN_GAME_PLAYERS 1

//nadefinovani zprav od klienta
 
#define CLIENT_GET_GAMES "1"
#define CLIENT_SET_PIXEL "2"
#define CLIENT_GET_ITEM "3"
#define CLIENT_SEND_QUESS "4"
#define CLIENT_CREATE_GAME "5"
#define CLIENT_CONNECT_TO_GAME "6"
#define CLIENT_RECONNECT "7"
#define CLIENT_LEAVE "8"
#define CLIENT_GET_POINTS "9"
#define CLIENT_GET_ID "10"
#define CLIENT_CANCEL_GAME "11"
#define CLIENT_TIME "12"
#define CLIENT_START_GAME "13"
#define CLIENT_NEXT_ROUND "14"
#define CLIENT_WAIT_TO_START "15"

#define OK "100"/*  */
#define ERR "-1"

//Nadefinovani stavu hry

#define STATE_FREE -1
#define STATE_LOBBY 0
#define STATE_IN_GAME 1
#define STATE_END 2
#define STATE_PAUSE 3

//Nadefinovani pouzivanych stringu

#define MSG_DELIMITER ";"
#define MSG_DELIMITER_CHAR ';'
#define MSG_PARAM_DELIMITER ","
#define MSG_PARAM_DELIMITER_CHAR ','
#define NADA "NADA"

//Nadefinovani parametru hry

#define ROUND_TIME 60
#define ROUND_X 600
#define ROUND_Y 600

void *connection_handler(void *);
void* timer(int game_id);

/*
Struktura na uchovani jednotlivych hracu
*/
typedef struct Player
{
    char name[255];
    int id;
    int game_id;
    int points;
    int socket;
    int state;
} player;

/*
Struktura na uchovani jednotlivych her
*/
typedef struct Game
{
    int id;
    int id_creator;
    int players;
    int free_space[MAX_GAME_PLAYERS];
    int player_on_turn;
    player players_ingame[MAX_GAME_PLAYERS];
    char word[255];
    int round;
    int game_state;
} game;

player* players = NULL;
game* games = NULL;

/**
 * @brief Odesle zpravu na prislusny socket
 * 
 * @param msg zprava na odeslani
 * @param socket socket na ktery se ma zprava odeslat
 */
void send_msg(char* msg, int socket){
    puts("Odesílám: ");
    puts(msg);
    write(socket, msg, strlen(msg));
}

/**
 * @brief Spocita vyskyt znaku v retezci
 * 
 * @param msg zprava na zkontrolovani
 * @param symbol jaky znak se ma kontrolovat
 * @return int pokud ma zprava spravny format vrati 1, jinak 0
 */
int check_msg(char* msg, char symbol, int count){
    int i;
    int occ = 0;

    //spocita vyskyt znaku ve zprave
    for (i = 0; msg[i]; i++){
        if (msg[i] == symbol){
            occ++;
        }
    }
    if (occ == count){
        return 1;
    }
    else{
        return 0;
    }
}

void send_error(char* error_msg, int socket){
    char* msg = malloc(strlen(&ERR) + strlen(&MSG_DELIMITER) + strlen(&error_msg) + 1);
    memcpy(msg, &ERR, strlen(&ERR));
    memcpy(msg + strlen(&ERR), &MSG_DELIMITER, strlen(&MSG_DELIMITER));
    memcpy(msg + strlen(&ERR) + strlen(&MSG_DELIMITER), &error_msg, strlen(&error_msg) + 1);

    send_msg(msg, socket);
    free(msg);
}

/**
 * @brief prijme zadost o zaslani bodu a odesle hraci jeho aktualni pocet bodu
 * 
 * @param params pole prijatych parametru
 */
void client_get_points(int player_id){
    int points = players[player_id].points;
    char* points_text[8];
    sprintf(points_text, "%d", points);

    char* msg = malloc(strlen(&CLIENT_GET_POINTS) + strlen(&MSG_DELIMITER) + strlen(points_text));
    memcpy(msg, &CLIENT_GET_POINTS, strlen(&CLIENT_GET_POINTS));
    memcpy(msg + strlen(&CLIENT_GET_POINTS), &MSG_DELIMITER, strlen(&MSG_DELIMITER));
    memcpy(msg + strlen(&CLIENT_GET_POINTS) + strlen(&MSG_DELIMITER), points_text, strlen(points_text));
    send_msg(msg, players[player_id].socket);
}

/**
 * @brief Ziska zadost od hrace na pripojeni na server pod jeho prezdivkou, pokud hrac neexistuje priradi mu id
 * pokud na serveru uz hrac existuje a je ve hre tak ho zpatky pripoji do hry nebo mu vrati jeho puvodni ID
 * 
 * @param player_name jmeno hrace 
 * @param sock socket uzivatele
 */
void client_get_id(char* player_name, int sock){
    int p;
    for (p = 0; p < MAX_SERVER_PLAYERS; p++){
        if (strcmp(players[p].name, player_name) == 0){
            (&players[p])->socket = sock;

            if (players[p].game_id == -1){
                char* player_id[8];
                sprintf(player_id, "%d", players[p].id);

                char* msg = malloc(strlen(&CLIENT_GET_ID) + strlen(&MSG_DELIMITER) + strlen(player_id) + 1);
                memcpy(msg, &CLIENT_GET_ID, strlen(&CLIENT_GET_ID));
                memcpy(msg + strlen(&CLIENT_GET_ID), &MSG_DELIMITER, strlen(&MSG_DELIMITER));
                memcpy(msg + strlen(&CLIENT_GET_ID) + strlen(&MSG_DELIMITER), player_id, strlen(player_id) + 1);
                send_msg(msg, sock);
                free(msg);
            }
            else {
                players[p].socket = sock;
                client_reconnect(sock, &players[p]);
            }
            return;
        }
    }
    
    for (p = 0; p < MAX_SERVER_PLAYERS; p++){
        if (players[p].socket == -1){
            players[p].socket = sock;

            memcpy(players[p].name, player_name, strlen(player_name));
            break;
        }
    }
    
    char* player_id[8];
    sprintf(player_id, "%d", p);

    char* msg = malloc(strlen(&CLIENT_GET_ID) + strlen(&MSG_DELIMITER) + strlen(player_id) + 1);
    memcpy(msg, &CLIENT_GET_ID, strlen(&CLIENT_GET_ID));
    memcpy(msg + strlen(&CLIENT_GET_ID), &MSG_DELIMITER, strlen(&MSG_DELIMITER));
    memcpy(msg + strlen(&CLIENT_GET_ID) + strlen(&MSG_DELIMITER), player_id, strlen(player_id) + 1);
    send_msg(msg, sock);
    free(msg);
}

/**
 * @brief Zajisti aby hrac zmizel z pripadne aktivni hry 
 * 
 * @param player_id id hrace co odchazi
 */
void client_leave(int player_id){
    int game_id = players[player_id].game_id;

    int p;
    for (p = 0; p < MAX_GAME_PLAYERS; p++){
        if (games[game_id].free_space[p] == players[player_id].id){
            (&games[game_id])->free_space[p] = -1;
            (&games[game_id])->players--;

            (&players[player_id])->game_id = -1;
            (&players[player_id])->points = -1;
            (&players[player_id])->socket = -1;
            memcpy(players[player_id].name, NADA, strlen(NADA));
            break;
        }
    }

    //TODO kdyz leavne zakladatel -> ukonci to hru, kdyz leaven hrac na rade -> dalsi kolo
    if (games[game_id].id_creator == player_id){
        client_cancel_game(player_id);
    }
    else{
        if (games[game_id].game_state == STATE_LOBBY){
            update_lobby(game_id);
        }
        else if(games[game_id].game_state == STATE_IN_GAME){
            //TODO kdyz je hra v behu aktualizuje se ingame
        }
    }
}

/**
 * @brief Zadost o pripojeni hrace do lobby pokud hra nebezi
 * 
 * @param p_id id hrace
 * @param game_id id hry do ktere se chci pripojit
 */
void client_connect(int p_id, char* id){
    int game_id = atoi(id);
    if (games[game_id].players < MAX_GAME_PLAYERS || games[game_id].game_state == STATE_LOBBY){
        int pl;
        //nalezeni volneho mista a prirazeni hrace do hry
        for (pl = 0; pl < MAX_GAME_PLAYERS; pl++){
            if (games[game_id].free_space[pl] == -1){
                (&games[game_id])->free_space[pl] = p_id;
                (&games[game_id])->players_ingame[pl] = players[p_id];
                (&games[game_id])->players++;

                (&players[p_id])->game_id = game_id;
                break;
            }
        }

        update_lobby(game_id);
    }
    else{
        if (games[game_id].players < MAX_GAME_PLAYERS){
            
        }
        else if (games[game_id].game_state == STATE_LOBBY){

        }
    }
}

void update_lobby(int game_id){
    int p_name;
    int string_size = -1; //proc -1? protoze kdyz mam 3 hrace, kazdem cyklu prictu jeden za oddelovac ale potrebuji n-1
    for (p_name = 0; p_name < MAX_GAME_PLAYERS; p_name++){
        if (games[game_id].free_space[p_name] != -1){
            string_size += strlen(games[game_id].players_ingame[p_name].name);
            string_size += 1; //za oddelovac
        }
    }

    char* players_string = malloc(sizeof(char)*string_size);
    int name_counter = 0;
    for (p_name = 0; p_name < MAX_GAME_PLAYERS; p_name++){
        if (games[game_id].free_space[p_name] != -1){
            strcat(players_string, games[game_id].players_ingame[p_name].name);
            name_counter++;
            if (name_counter < games[game_id].players){
                strcat(players_string, ",");
            }
        }
    }
        

    int p;
    for (p = 0; p < MAX_GAME_PLAYERS; p++){
        if (games[game_id].free_space[p] != -1){
            int is_admin = 0;

            if (games[game_id].id_creator == games[game_id].players_ingame[p].id){
                is_admin = 1;
            }

            char* string_is_admin[8];
            sprintf(string_is_admin, "%d", is_admin);
    
            char* msg = malloc(strlen(&CLIENT_CONNECT_TO_GAME) + strlen(&MSG_DELIMITER) + strlen(string_is_admin) + strlen(&MSG_PARAM_DELIMITER) + strlen(players_string) + 1);
            memcpy(msg, &CLIENT_CONNECT_TO_GAME, strlen(&CLIENT_CONNECT_TO_GAME));
            memcpy(msg + strlen(&CLIENT_CONNECT_TO_GAME), &MSG_DELIMITER, strlen(&MSG_DELIMITER));
            memcpy(msg + strlen(&CLIENT_CONNECT_TO_GAME) + strlen(&MSG_DELIMITER), string_is_admin, strlen(string_is_admin));
            memcpy(msg + strlen(&CLIENT_CONNECT_TO_GAME) + strlen(&MSG_DELIMITER) + strlen(string_is_admin), &MSG_PARAM_DELIMITER, strlen(&MSG_PARAM_DELIMITER));
            memcpy(msg + strlen(&CLIENT_CONNECT_TO_GAME) + strlen(&MSG_DELIMITER) + strlen(string_is_admin) + strlen(&MSG_PARAM_DELIMITER), players_string, strlen(players_string) + 1);
            send_msg(msg, games[game_id].players_ingame[p].socket);
            free(msg);
        }
    }
}

/**
 * @brief Zruseni hry, 
 * pokud ji zakladatel sam zrusi a nebo se zakladatel odpoji uplne tak se odpojí zbytek hracu
 * pokud to zrusi nekdo kdo neni admin tak se pouze aktualizuje stav a okna
 * 
 * @param params porametry od uzivatele
 */
void client_cancel_game(int player_id){
    int game_id = players[player_id].game_id;

    if (games[game_id].id_creator == player_id){
        char* msg = malloc(strlen(&CLIENT_CANCEL_GAME) + strlen(&MSG_DELIMITER) + strlen(&OK) + 1);
        memcpy(msg, &CLIENT_CANCEL_GAME, strlen(&CLIENT_CANCEL_GAME));
        memcpy(msg + strlen(&CLIENT_CANCEL_GAME), &MSG_DELIMITER, strlen(&MSG_DELIMITER));
        memcpy(msg + strlen(&CLIENT_CANCEL_GAME) + strlen(&MSG_DELIMITER), &OK, strlen(&OK) + 1);
;
        int p;
        for (p = 0; p < MAX_GAME_PLAYERS; p++){
            if (games[game_id].free_space[p] != -1){
                send_msg(msg, games[game_id].players_ingame[p].socket);
                (&games[game_id])->free_space[p] = -1;
                (&games[game_id])->players--;
            }
        }

        (&games[game_id])->game_state = STATE_FREE;
        (&games[game_id])->id_creator = -1;
        (&games[game_id])->round = -1;
        memcpy(games[game_id].word, &NADA, strlen(&NADA));
    }else{
        int p;
        for (p = 0; p < MAX_GAME_PLAYERS; p++){
            if (games[game_id].free_space[p] == players[player_id].id){
                (&games[game_id])->free_space[p] = -1;
                (&games[game_id])->players--;
                break;
            }
        }
        update_lobby(game_id);
        char* msg = malloc(strlen(&CLIENT_CANCEL_GAME) + strlen(&MSG_DELIMITER) + strlen(&OK) + 1);
        memcpy(msg, &CLIENT_CANCEL_GAME, strlen(&CLIENT_CANCEL_GAME));
        memcpy(msg + strlen(&CLIENT_CANCEL_GAME), &MSG_DELIMITER, strlen(&MSG_DELIMITER));
        memcpy(msg + strlen(&CLIENT_CANCEL_GAME) + strlen(&MSG_DELIMITER), &OK, strlen(&OK) + 1);
        send_msg(msg, players[player_id].socket);
        free(msg);
    }
    
}

/**
 * @brief Zalozeni hry
 * 
 * @param params Parametry od uzivatele
 */
void client_create_game(int player_id){
    int g_id;
    int g;
    for (g = 0; g < MAX_SERVER_GAMES; g++){
        if (games[g].game_state == STATE_FREE){
            (&games[g])->game_state = STATE_LOBBY;
            (&games[g])->free_space[0] = player_id;
            (&games[g])->players_ingame[0] = players[player_id];
            (&games[g])->id_creator = player_id;
            (&games[g])->players = 1;
            (&games[g])->round = 0;
            (&games[g])->player_on_turn = 0;
            g_id = games[g].id;

            (&players[player_id])->game_id = g_id;
            break;
        }
    }
    
    char* game_id[8];
    sprintf(game_id, "%d", g_id);

    char* msg = malloc(strlen(&CLIENT_CREATE_GAME) + strlen(&MSG_DELIMITER) + strlen(game_id) + strlen("\0"));
    memcpy(msg, &CLIENT_CREATE_GAME, strlen(&CLIENT_CREATE_GAME));
    memcpy(msg + strlen(&CLIENT_CREATE_GAME), &MSG_DELIMITER, strlen(&MSG_DELIMITER));
    memcpy(msg + strlen(&CLIENT_CREATE_GAME) + strlen(&MSG_DELIMITER), game_id, strlen(game_id) + 1);
    memcpy(msg + strlen(&CLIENT_CREATE_GAME) + strlen(&MSG_DELIMITER) + strlen(game_id), "\0", strlen("\0"));
    send_msg(msg, players[player_id].socket);
    free(msg);
}

/**
 * @brief Prijme od klienta zadost o vykresleni pixelu, pokud je pixel platny bude namalovan
 *  format prijate zpravy: 290,210,69,139,0
    vyznam:                 x   y   R  G  B 
    ? -> alfa to asi neni, netusim    

    Fakt oskliva funkce, chtelo by to predelat
 * 
 * @param params zprava od klienta
 */
void client_set_pixel(char* params, int player_id, int socket){
    char* params_copy = malloc(strlen(params));
    memcpy(params_copy, params, strlen(params));
    if (check_msg(params, MSG_PARAM_DELIMITER_CHAR, 4) == 0){
        return;
    }

    int i = 0;
    char *p = strtok(&params[2], &MSG_PARAM_DELIMITER);
    char *pixel_values[5];

    while (p != NULL)
    {
        pixel_values[i++] = p;
        p = strtok(NULL, &MSG_PARAM_DELIMITER);
    }

    int w, c;
    for (w = 0; w < 5; w++){
        for (c = 0; c < strlen(pixel_values[w]); c++){
            if (pixel_values[w][c] <= '0'  && pixel_values[w][c] >= '9'){
                send_error("pixel nema platne hodnoty", socket);
                return;
            }
        }
    }

    int x_value, y_value, color_value_r, color_value_g, color_value_b;
    x_value = atoi(pixel_values[0]);
    y_value = atoi(pixel_values[1]);
    color_value_r = atoi(pixel_values[2]);
    color_value_g = atoi(pixel_values[3]);
    color_value_b = atoi(pixel_values[4]);

    if (x_value < 0 || x_value > ROUND_X || y_value < 0 || y_value > ROUND_Y){
        send_error("pixel nema platne souradnice", socket);
        return;
    }

    if (color_value_r < 0 || color_value_r > 255 || color_value_g < 0 || color_value_g > 255 || color_value_b < 0 || color_value_b > 255){
        send_error("pixel nema platne RGBA hodnoty", socket);
        return;
    }

    int game_id = players[player_id].game_id;
    
    char* msg = malloc(strlen(&CLIENT_SET_PIXEL) + strlen(&MSG_DELIMITER) + strlen(&params_copy[0]) + strlen(&MSG_PARAM_DELIMITER) + strlen(&params_copy[1]) + strlen(&MSG_PARAM_DELIMITER)+ strlen(&params_copy[2]) + strlen(&MSG_PARAM_DELIMITER)+ strlen(&params_copy[3]) + strlen(&MSG_PARAM_DELIMITER)+ strlen(&params_copy[4]) + strlen(&MSG_PARAM_DELIMITER)+ strlen(&params_copy[5]) + 1);
    memcpy(msg, &CLIENT_SET_PIXEL, strlen(&CLIENT_SET_PIXEL));
    memcpy(msg + strlen(&CLIENT_SET_PIXEL), &MSG_DELIMITER, strlen(&MSG_DELIMITER));
    memcpy(msg + strlen(&CLIENT_SET_PIXEL) + strlen(&MSG_DELIMITER), params_copy, strlen(params_copy));

    int player;
    for (player = 0; player < MAX_GAME_PLAYERS; player++){
        if (games[game_id].free_space[player] != -1){
            send_msg(msg, players[games[game_id].free_space[player]].socket);
        }
    }
    free(msg);
}

/**
 * @brief prijme od uzivatele jeho tip a vyhodnoti jestli uhadl nebo ne
 * 
 * @param params 
 */
void client_send_quess(int player_id, char* word){
    int game_id = players[player_id].game_id;

    if (strcmp(word, games[game_id].word) == 0){
        (&players[player_id])->points++;

        char* points_text[8];
        sprintf(points_text, "%d", players[player_id].points);

        char* msg = malloc(strlen(&CLIENT_SEND_QUESS) + strlen(&MSG_DELIMITER) + strlen(points_text) + 1);
        memcpy(msg, &CLIENT_SEND_QUESS, strlen(&CLIENT_SEND_QUESS));
        memcpy(msg + strlen(&CLIENT_SEND_QUESS), &MSG_DELIMITER, strlen(&MSG_DELIMITER));
        memcpy(msg + strlen(&CLIENT_SEND_QUESS) + strlen(&MSG_DELIMITER), points_text, strlen(points_text) + 1);
        send_msg(msg, players[player_id].socket);
        free(msg);

        next_round(game_id);    
    }
    else{
        char* msg = malloc(strlen(&CLIENT_SEND_QUESS) + strlen(&MSG_DELIMITER) + strlen("-1") + 1);
        memcpy(msg, &CLIENT_SEND_QUESS, strlen(&CLIENT_SEND_QUESS));
        memcpy(msg + strlen(&CLIENT_SEND_QUESS), &MSG_DELIMITER, strlen(&MSG_DELIMITER));
        memcpy(msg + strlen(&CLIENT_SEND_QUESS) + strlen(&MSG_DELIMITER), "-1", strlen("-1") + 1);
        send_msg(msg, players[player_id].socket);
        free(msg);
    }
}

/**
 * @brief Odesle vsem hracum v lobby informaci o startu hry
 * 
 * @param player_id od jakeho hrace prisla informace o startu hry
 */
void client_start_game(int player_id){
    int game_id = players[player_id].game_id;

    if (games[game_id].players < MIN_GAME_PLAYERS){
        send_error("Musi byt pritomno 2 a vice hracu", players[player_id].socket);
        return;
    }
    games[game_id].player_on_turn = player_id;
    //pthread_t thread;
    //pthread_create( &thread , NULL ,  timer , game_id);
    next_round(game_id);
}

/**
 * @brief Odesle informaci ze ma zacit dalsi kolo
 * 
 * @param game_id 
 */
void next_round(int game_id){
    const char* words[WORDS];
    words[0] = "pampeliška";
    words[1] = "slunce";
    words[2] = "auto";
    words[3] = "morče";
    words[4] = "ježek";
    words[5] = "strach";
    words[6] = "vřískot";
    words[7] = "dům";
    words[8] = "paralelní";
    words[9] =  "čas";

    

    int random_word_index = rand() % 10;
    char* word = words[random_word_index];

    (&games[game_id])->game_state = STATE_IN_GAME;
    memcpy(games[game_id].word, word, strlen(word));

    if (games[game_id].round >= games[game_id].players){
        client_send_final_table(game_id);
        return;
    }

    int player_turn = games[game_id].player_on_turn;

    int p;
    for (p = 0; p < MAX_GAME_PLAYERS; p++){
        if (games[game_id].free_space[p] != -1){
            if (games[game_id].players_ingame[p].id == player_turn){
                char* msg = malloc(strlen(&CLIENT_NEXT_ROUND) + strlen(&MSG_DELIMITER) + strlen("1") + strlen(&MSG_PARAM_DELIMITER) + strlen(word) + 1);
                memcpy(msg, &CLIENT_NEXT_ROUND, strlen(&CLIENT_NEXT_ROUND));
                memcpy(msg + strlen(&CLIENT_NEXT_ROUND), &MSG_DELIMITER, strlen(&MSG_DELIMITER));
                memcpy(msg + strlen(&CLIENT_NEXT_ROUND) + strlen(&MSG_DELIMITER), "1", strlen("1"));
                memcpy(msg + strlen(&CLIENT_NEXT_ROUND) + strlen(&MSG_DELIMITER) + strlen("1"), &MSG_PARAM_DELIMITER, strlen(&MSG_PARAM_DELIMITER));
                memcpy(msg + strlen(&CLIENT_NEXT_ROUND) + strlen(&MSG_DELIMITER) + strlen("1") + strlen(&MSG_PARAM_DELIMITER), word, strlen(word) + 1);
                
                send_msg(msg, players[games[game_id].free_space[p]].socket);
                free(msg);
            }
            else {
                char* msg = malloc(strlen(&CLIENT_NEXT_ROUND) + strlen(&MSG_DELIMITER) + strlen("0") + strlen(&MSG_PARAM_DELIMITER) + strlen("NADA") + 1);
                memcpy(msg, &CLIENT_NEXT_ROUND, strlen(&CLIENT_NEXT_ROUND));
                memcpy(msg + strlen(&CLIENT_NEXT_ROUND), &MSG_DELIMITER, strlen(&MSG_DELIMITER));
                memcpy(msg + strlen(&CLIENT_NEXT_ROUND) + strlen(&MSG_DELIMITER), "0", strlen("0"));
                memcpy(msg + strlen(&CLIENT_NEXT_ROUND) + strlen(&MSG_DELIMITER) + strlen("0"), &MSG_PARAM_DELIMITER, strlen(&MSG_PARAM_DELIMITER));
                memcpy(msg + strlen(&CLIENT_NEXT_ROUND) + strlen(&MSG_DELIMITER) + strlen("0") + strlen(&MSG_PARAM_DELIMITER), "NADA", strlen("NADA") + 1);

                send_msg(msg, players[games[game_id].free_space[p]].socket);
                free(msg);
            }
        }
        
    }
    games[game_id].round++;
    games[game_id].player_on_turn = games[game_id].free_space[1];
}

void client_send_final_table(int game_id){

}

/**
 * @brief Pokud se hrac pripoji zpet tak se mu odesle aktualni stav hry
 * 
 * @param params parametry od uzivatele
 */
void client_reconnect(int socket, player* player){
    int game_id = player->game_id;

    if (games[game_id].game_state == STATE_LOBBY){
        int p_name;
        int string_size = -1; //proc -1? protoze kdyz mam 3 hrace, kazdem cyklu prictu jeden za oddelovac ale potrebuji n-1
        for (p_name = 0; p_name < MAX_GAME_PLAYERS; p_name++){
            if (games[game_id].free_space[p_name] != -1){
                string_size += strlen(games[game_id].players_ingame[p_name].name);
                string_size += 1; //za oddelovac
            }
        }

        char* players_string = malloc(sizeof(char)*string_size);
        int name_counter = 0;
        for (p_name = 0; p_name < MAX_GAME_PLAYERS; p_name++){
            if (games[game_id].free_space[p_name] != -1){
                strcat(players_string, games[game_id].players_ingame[p_name].name);
                name_counter++;
                if (name_counter < games[game_id].players){
                    strcat(players_string, ",");
                }
            }
        }
            
        char* msg = malloc(strlen(&CLIENT_CONNECT_TO_GAME) + strlen(&MSG_DELIMITER) + strlen("0") + strlen(&MSG_PARAM_DELIMITER) + strlen(players_string) + 1);
        memcpy(msg, &CLIENT_CONNECT_TO_GAME, strlen(&CLIENT_CONNECT_TO_GAME));
        memcpy(msg + strlen(&CLIENT_CONNECT_TO_GAME), &MSG_DELIMITER, strlen(&MSG_DELIMITER));
        memcpy(msg + strlen(&CLIENT_CONNECT_TO_GAME) + strlen(&MSG_DELIMITER), "0", strlen("0"));
        memcpy(msg + strlen(&CLIENT_CONNECT_TO_GAME) + strlen(&MSG_DELIMITER) + strlen("0"), &MSG_PARAM_DELIMITER, strlen(&MSG_PARAM_DELIMITER));
        memcpy(msg + strlen(&CLIENT_CONNECT_TO_GAME) + strlen(&MSG_DELIMITER) + strlen("0") + strlen(&MSG_PARAM_DELIMITER), players_string, strlen(players_string) + 1);
        send_msg(msg, games[game_id].players_ingame[player->id].socket);
        free(msg);   
    }
    else if (games[game_id].game_state == STATE_IN_GAME){
        char* msg = malloc(strlen(&CLIENT_WAIT_TO_START) + strlen(&MSG_DELIMITER) + strlen("1"));
        memcpy(msg, &CLIENT_WAIT_TO_START, strlen(&CLIENT_WAIT_TO_START));
        memcpy(msg + strlen(&CLIENT_WAIT_TO_START), &MSG_DELIMITER, strlen(&MSG_DELIMITER));
        memcpy(msg + strlen(&CLIENT_WAIT_TO_START) + strlen(&MSG_DELIMITER), "1", strlen("1"));
    }

}

/**
 * @brief NEVYUZIVA SE
 * Funkce ktera bezi pro kazdou hru ve vlastim vlakne a odesila cas do konce kola
 * 
 * 
 * @param game_id id hry pro ktere ma bezet
 * @return void* 
 */
void* timer(int game_id){
    int time = ROUND_TIME;

    while (1){
        sleep(1);
        time--;

        char* time_text = malloc(sizeof(char) * 8);
        sprintf(time_text, "%d", time);

        char* msg = malloc(strlen(&CLIENT_TIME) + strlen(&MSG_DELIMITER) + strlen(time_text));
        memcpy(msg, &CLIENT_TIME, strlen(&CLIENT_TIME));
        memcpy(msg + strlen(&CLIENT_TIME), &MSG_DELIMITER, strlen(&MSG_DELIMITER));
        memcpy(msg + strlen(&CLIENT_TIME) + strlen(&MSG_DELIMITER), time_text, strlen(time_text));

        int p;
        for (p = 0; p < MAX_GAME_PLAYERS; p++){
            if (games[game_id].free_space[p] != -1){
                send_msg(msg, players[games[game_id].free_space[p]].socket);
            }
        }

        free(msg);
        free(time_text);

        if (time <= 0){
            next_round(game_id);
            time = ROUND_TIME;
        }
    }
}

/**
 * @brief Vstupni bod programu
 * 
 * @param argc pocet argumentu
 * @param argv pole argumentu
 * @return int navratova hodnota programu
 */
int main(int argc , char *argv[])
{
    setvbuf(stdout, NULL, _IONBF, 0);
    //alokace pameti pro struktury

    players = calloc(MAX_SERVER_PLAYERS, sizeof(player));
    games = calloc(MAX_SERVER_GAMES, sizeof(game));

    //prvotni nastaveni hracu

    int p;
    for (p = 0; p < MAX_SERVER_PLAYERS; p++){
        (&players[p])->id = p;
        (&players[p])->points = -1;
        (&players[p])->socket = -1;
        (&players[p])->game_id = -1;
        (&players[p])->state = STATE_FREE;
        memcpy(players[p].name, NADA, strlen(NADA));
    }
    //nastaveni her

    int g;
    for (g = 0; g < MAX_SERVER_GAMES; g++){
        (&games[g])->id = g;
        (&games[g])->round = 0;
        (&games[g])->game_state = STATE_FREE;
        (&games[g])->player_on_turn = -1;
        (&games[g])->players = 0;
        
        int fr;
        for (fr = 0; fr < MAX_GAME_PLAYERS; fr++){
            (&games[g])->free_space[fr] = -1;
        }
    }

    
    //nastaveni socketu a serveru

    int socket_desc , client_sock , c;
    struct sockaddr_in server , client;
     
    //vytvoreni socketu
    
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket\n");
    }
    printf("Socket created\n");
     
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 10000 );
     
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        printf("bind failed. Error\n");
        return 1;
    }
    printf("bind done\n");

    listen(socket_desc , 3);

    c = sizeof(struct sockaddr_in);
	pthread_t thread_id;

    //nekonecny cyklus na prijimani uzivatelu
    while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
    {
        printf("Connection accepted\n");
        if( pthread_create( &thread_id , NULL ,  connection_handler , (void*) &client_sock) < 0)
        {
            printf("could not create thread\n");
        }
        printf("Handler assigned\n");
    }
     
    if (client_sock < 0)
    {
        printf("accept failed\n");
        return 1;
    }
     
    return 0;
}


/**
 * @brief Funkce je spustena ve vlastnim vlakne a obstarava jednoho klienta, zpracovava prijmute zpravy
 * 
 * @param socket_desc socket uzivatele
 * @return void* 
 */
void *connection_handler(void *socket_desc)
{
    int sock = *(int*)socket_desc;
    int read_size;
    char *message , client_message[CLIENT_MSG_SIZE];
 
    //prijimani zpravy
    while( (read_size = recv(sock , client_message , CLIENT_MSG_SIZE , 0)) > 0 )
    {
        client_message[read_size] = '\0';
        printf("server prijmul: %s\n", client_message);	

        if (check_msg(client_message, MSG_DELIMITER_CHAR, 2) == 0){
            printf("Prijata zprava nesplnovala format");
            continue;
        }
		
        int i = 0;
        char *p = strtok(client_message, &MSG_DELIMITER);
        char *params[3];

        while (p != NULL)
        {
            params[i++] = p;
            p = strtok(NULL, &MSG_DELIMITER);
        }

        if (strlen(params[0]) < 1 || strlen(params[1]) < 1 || strlen(params[2]) < 1){
            printf("Prijata zprava nesplnovala format");
            continue;
        }


        int player_id = atoi(params[0]);

        if (strcmp(params[1], CLIENT_SET_PIXEL) == 0){
            client_set_pixel(params[2], player_id, sock);
        }
        else if (strcmp(params[1], CLIENT_SEND_QUESS) == 0){
            client_send_quess(player_id, params[2]);
        }
        else if (strcmp(params[1], CLIENT_CREATE_GAME) == 0){
            client_create_game(player_id);
        }
        else if (strcmp(params[1], CLIENT_CONNECT_TO_GAME) == 0){
            client_connect(player_id, params[2]);
        }
        else if (strcmp(params[1], CLIENT_LEAVE) == 0){
            client_leave(player_id);
        }
        else if (strcmp(params[1], CLIENT_GET_POINTS) == 0){
            client_get_points(player_id);
        }
        else if (strcmp(params[1], CLIENT_GET_ID) == 0){
            char* client_name = params[2];
            client_get_id(client_name, sock);
        }
        else if (strcmp(params[1], CLIENT_CANCEL_GAME) == 0){
            client_cancel_game(player_id);
        }
        else if (strcmp(params[1], CLIENT_START_GAME) == 0){
            client_start_game(player_id);
        }

	    memset(client_message, 0, CLIENT_MSG_SIZE);
    }
     
    if(read_size == 0)
    {
        puts("Client disconnected");
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        perror("recv failed");
    }
         
    return 0;
} 

