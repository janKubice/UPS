#include <stdio.h>
#include <stdlib.h>

typedef enum e_state {
    ST_NOT_ALLOWED = 0,
    ST_INIT,
    ST_WAITING,
    ST_WORKING,
    ST_PAUSE,
    ST_QUIT
} State;

typedef enum e_event {
  EV_WAIT = 0,
  EV_WORK,
  EV_PAUSE,
  EV_QUIT
} Event;

#define STATES_COUNT 6
#define EVENTS_COUNT 4

const char * const state_str[] = {
    [ST_NOT_ALLOWED] = "not allowed",
    [ST_INIT] = "init",
    [ST_WAITING] = "waiting",
    [ST_WORKING]  = "working",
    [ST_PAUSE]  = "pause",
    [ST_QUIT] = "quit"
};

const char * const event_str[] = {
    [EV_WAIT] = "start waiting",
    [EV_WORK] = "start working",
    [EV_PAUSE] = "start pause",
    [EV_QUIT] = "start quit",
};

State transitions[STATES_COUNT][EVENTS_COUNT] = {
  [ST_INIT][EV_WAIT] = ST_WAITING,
  [ST_WAITING][EV_WORK] = ST_WORKING,
  [ST_WAITING][EV_PAUSE] = ST_PAUSE
};

typedef struct {
  int socket;
} Player;

typedef struct {
  State state;
  Player players[2];
} Game;

void print_states(){
  for (int i=0; i<STATES_COUNT;i++){
    printf("State: %s(%d)\n", state_str[i], i);
  }
}

void print_state(State state){
  printf("%s\n", state_str[state]);
}

int allowed_transition(State state, Event event) {
  return transitions[state][event];
}

Game *new_game(){
  Game *game;
  game = malloc(sizeof(Game));
  game->state = ST_INIT;
  return game;
}

int main() {
  Game *game;

  printf("Allowed states:\n");
  print_states();

  game = new_game();
  printf("Curent game state: ");
  print_state(game->state);

  printf("Transition: %s -> %s (%d)\n", \
    state_str[game->state], \
    event_str[EV_PAUSE], \
    allowed_transition(game->state, EV_PAUSE));

  if (allowed_transition(game->state, EV_PAUSE) != ST_NOT_ALLOWED)
    game->state = allowed_transition(game->state, EV_PAUSE);

  printf("Curent game state: ");
  print_state(game->state);

  printf("Transition: %s -> %s (%d)\n", \
    state_str[game->state], \
    event_str[EV_WAIT], \
    allowed_transition(game->state, EV_WAIT));

  if (allowed_transition(game->state, EV_WAIT) != ST_NOT_ALLOWED)
    game->state = allowed_transition(game->state, EV_WAIT);

  printf("Curent game state: ");
  print_state(game->state);

  return 0;
}
