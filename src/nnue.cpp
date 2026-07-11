#include "nnue.h"
#include <algorithm>
#include <iostream>
#include "bitboard.h"
#include "net.h"

namespace nnue{
  namespace{
    struct network {
      i16 in_weights[768][l1_size];
      i16 in_biases[l1_size];
      i16 out_weights[2][l1_size];
      i16 out_bias;
    };
    const network* net;
    int feature_index(
      const int pc,
      const int sq,
      const int perspective){
      const int piece_color = make(pc);
      const int piece_type = ptmake(pc);
      const int index_color = piece_color != perspective;
      const int index_piece = piece_type - 1;
      const int relative_sq = perspective == white?sq:sq ^ 56;
      return 384 * index_color + 64 * index_piece + relative_sq;
    }
  }

  bool init() {
    net=reinterpret_cast<const network*>(g_default_net);
    std::cout << "Embedded NNUE loaded\n";
    return true;
  }

  void add_feature(
    i16 acc[2][l1_size],
    const int pc,
    const int sq){
    for (int p = 0; p < 2; ++p){
      const int idx = feature_index(pc,sq,p);
      for (int i = 0; i < l1_size; ++i)
        acc[p][i] = static_cast<i16>(acc[p][i] + net->in_weights[idx][i]);
    }
  }

  void sub_feature(
    i16 acc[2][l1_size],
    const int pc,
    const int sq){
    for (int p = 0; p < 2; ++p){
      const int idx = feature_index(pc,sq,p);
      for (int i = 0; i < l1_size; ++i)
        acc[p][i] = static_cast<i16>(acc[p][i] - net->in_weights[idx][i]);
    }
  }

  void refresh_accumulator(
    const board& pos,
    i16 acc[2][l1_size]){
    for (int p = 0; p < 2; ++p)
      for (int i = 0; i < l1_size; ++i)
        acc[p][i] = net->in_biases[i];

    for (u8 sq = 0; sq < 64; ++sq){
      const int pc = pos.piece_on(sq);
      if (! pc) continue;

      add_feature(acc,pc,sq);
    }
  }

  namespace{
    int evaluate_from_acc(
      const board& pos,
      const i16 acc[2][l1_size]) {
      const int stm=pos.side_to_move;
      const int nstm=!stm;

      int64_t score=0;

      for (int i=0; i < l1_size; ++i) {
        int x=acc[stm][i];
        x=std::clamp(x, 0, 255);
        score+=static_cast<int64_t>(x) * x * net->out_weights[0][i];
      }

      for (int i=0; i < l1_size; ++i) {
        int x=acc[nstm][i];
        x=std::clamp(x, 0, 255);
        score+=static_cast<int64_t>(x) * x * net->out_weights[1][i];
      }

      int64_t tmp=score / 255;
      tmp+=net->out_bias;
      tmp=tmp * 400LL;
      score=tmp / (255LL * 64LL);

      return static_cast<int>(score);
    }
  }

  int evaluate(
    const board& pos){
    return evaluate_from_acc(pos,pos.st->acc);
  }
}
