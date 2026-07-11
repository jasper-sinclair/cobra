#include "attack.h"
#include "nnue.h"
#include "uci.h"

int main(){
  attack::init();
  nnue::init();
  search::init();
  zobrist::init();
  uci::init();
}
