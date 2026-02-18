#pragma once
#include <mutex>
#include <thread>
#include "search.h"

struct option{
  std::string name;
  std::string type;
  int default_value;
  int min_value;
  int max_value;
};

namespace uci{
  constexpr int default_contempt = 1;
  constexpr size_t default_hash = 256;
  constexpr thread_id default_threads = 1;

  extern board pos;
  extern std::string start_fen;
  extern int contempt;
  extern search search_info;
  extern std::jthread thread;
  extern std::mutex search_mutex;
  extern std::vector<option> ucioptions;

  u16 to_move(
    const std::string& str,
    board& b);

  void get_bestmove();

  void go(
    const std::string& str);

  void info();

  void init();

  void loop();

  void newgame();

  void perft(
    std::istringstream& ss);

  void position(
    std::istringstream& ss);

  void setoption(
    std::istringstream& ss);

  void stop();
}
