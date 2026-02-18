#include "uci.h"
#include <functional>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include "movegen.h"

namespace uci{
  board pos;
  int contempt = default_contempt;
  search search_info;
  std::jthread thread;
  std::mutex search_mutex;
  std::string start_fen="rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
  std::vector<option> ucioptions = {
    {.name = "Hash",.type = "spin",.default_value = default_hash,.min_value = 1,.max_value = max_hash_size},
    {.name = "Threads",.type = "spin",.default_value = default_threads,.min_value = 1,.max_value = max_threads},
    {.name = "Contempt",.type = "spin",.default_value = default_contempt,.min_value = -100,.max_value = 100}
  };

  void init(){
    pos = board(start_fen);
    search_info.set_hash_size(default_hash);
    search_info.set_num_threads(default_threads);
    loop();
  }

  void loop(){
    std::string line, token;
    std::unordered_map<std::string, std::function<void(
      std::istringstream&)>> commands = {

      {
        "uci",[](
        std::istringstream&){
          info();
        }},
      {
        "ucinewgame",[](
        std::istringstream&){
          newgame();
        }},
      {
        "setoption",[](
        std::istringstream& ss){
          setoption(ss);
        }},
      {
        "isready",[](
        std::istringstream&){
          SO << "readyok" << NL;
        }},
      {
        "position",[](
        std::istringstream& ss){
          position(ss);
        }},
      {
        "go",[](
        const std::istringstream& ss){
          go(ss.str());
        }},
      {
        "stop",[](
        std::istringstream&){
          stop();
        }},
      {
        "quit",[](
        std::istringstream&){
          stop();
          exit(0);
        }},
      {
        "print",[](
        std::istringstream&){
          SO << pos << NL << pos.fen() << NL;
        }},
      {"perft",perft}};

    while (std::getline(std::cin,line)){
      std::istringstream ss(line);
      ss >> token;
      if (auto it = commands.find(token); it != commands.end()){
        it->second(ss);
      }
    }
  }

  void info(){
    SO << "id name bbce" << NL;
    SO << "id author jasper" << NL;
    for (const auto& [name, type, default_value, min_value, max_value] : ucioptions){
      SO << "option name " << name << " type " << type << " default " << default_value << " min " << min_value << " max " << max_value << NL;
    }
    SO << "uciok" << SE;
  }

  void newgame(){
    search_info.clear();
  }

  void setoption(
    std::istringstream& ss){
    std::string name, token;
    std::unordered_map<std::string, std::string> options;
    while (ss >> token){
      if (token == "name") ss >> name;
      else if (token == "value") ss >> options[name];
    }
    if (name == "Hash"){
      const u64 hash_size = std::stoull(options[name]);
      search_info.hash.set_size(hash_size);
    } else if (name == "Threads"){
      search_info.set_num_threads(std::stoi(options[name]));
    } else if (name == "Contempt"){
      contempt = std::stoi(options[name]);
    }
  }

  void position(
    std::istringstream& ss){
    std::string token, fen;
    ss >> token;
    if (token == "startpos"){
      fen = start_fen;
      ss >> token;
    } else if (token == "fen"){
      while (ss >> token && token != "moves") fen += token + " ";
    } else return;
    pos = board(fen);
    while (ss >> token){
      if (const u16 move = to_move(token,pos); move){
        pos.apply_move(move);
      } else{
        std::cerr << "Invalid move: " << token << NL;
        break;
      }
    }
  }

  void go(
    const std::string& str){
    std::scoped_lock lock(search_mutex);
    stop();
    search_info.time = {};
    search_info.time.start();
    std::istringstream ss(str);
    std::string token;
    while (ss >> token){
      if (token == "wtime") ss >> search_info.time.time[white];
      else if (token == "btime") ss >> search_info.time.time[black];
      else if (token == "winc") ss >> search_info.time.inc[white];
      else if (token == "binc") ss >> search_info.time.inc[black];
      else if (token == "nodes"){
        search_info.time.use_node_limit = true;
        ss >> search_info.time.node_limit;
      } else if (token == "depth"){
        search_info.time.use_depth_limit = true;
        ss >> search_info.time.depth_limit;
      } else if (token == "movetime"){
        search_info.time.use_move_limit = true;
        ss >> search_info.time.move_time_limit;
      }
    }
    search_info.time.init_time(pos.side_to_move);
    thread = std::jthread(get_bestmove);
  }

  void stop(){
    search_info.stop();
    if (thread.joinable()) thread.join();
  }

  void perft(
    std::istringstream& ss){
    i32 depth;
    ss >> depth;
    const auto begin = std::chrono::steady_clock::now();
    const auto node_cnt = perft(pos,depth);
    const auto end = std::chrono::steady_clock::now();
    SO << "node " << node_cnt << NL;
    SO << "time " << std::chrono::duration<double>(end - begin).count() << NL;
  }

  u16 to_move(
    const std::string& str,
    board& b){
    move_list moves;
    gen_moves(b,moves);
    for (const auto& [move, score] : moves){
      if (move::move_to_string(move) == str) return move;
    }
    return 0;
  }

  void get_bestmove(){
    const u16 move = search_info.best_move(pos);
    SO << "bestmove " << move::move_to_string(move) << SE;
  }
}
