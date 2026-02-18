#include "eval.h"
#include "attack.h"
#include "main.h"
#include "uci.h"

namespace eval {

  namespace {

    // ============================
    // Phase weights
    // ============================

    constexpr int phase_values[n_piece_types]={
        0, 0, 1, 1, 2, 4, 0
    };

    constexpr int max_phase=24;

    // ============================
    // Mobility
    // ============================

    int evaluate_mobility(const board& pos, bool us) {

      int score=0;

      for (i32 pt=knight; pt <= queen; ++pt) {

        bitboard pieces=pos.get_pieces(us, pt);

        while (pieces) {

          const u8 sq=pop_lsb(pieces);
          bitboard moves{};

          switch (pt) {
          case knight: moves=attack::knight_att[sq]; break;
          case bishop: moves=attack::atts<bishop>(sq, pos.occupied()); break;
          case rook:   moves=attack::atts<rook>(sq, pos.occupied()); break;
          case queen:  moves=attack::atts<queen>(sq, pos.occupied()); break;
          default: break;
          }

          moves&=~pos.get_color(us);
          int mobility=popcnt(moves);

          switch (pt) {
          case knight: score+=5 * mobility; break;
          case bishop: score+=6 * mobility; break;
          case rook:   score+=2 * mobility; break;
          case queen:  score+=1 * mobility; break;
          default: break;
          }
        }
      }

      return score;
    }

    // ============================
    // King safety (nonlinear)
    // ============================

    int evaluate_king_safety(const board& pos, bool us) {

      const u8 king_sq=pos.ksq(us);
      const bitboard occ=pos.occupied();

      bitboard king_zone=
        bitboard::from_sq(king_sq)
        | bitboard::from_sq(king_sq).shift<north>()
        | bitboard::from_sq(king_sq).shift<south>()
        | bitboard::from_sq(king_sq).shift<east>()
        | bitboard::from_sq(king_sq).shift<west>()
        | bitboard::from_sq(king_sq).shift<northeast>()
        | bitboard::from_sq(king_sq).shift<northwest>()
        | bitboard::from_sq(king_sq).shift<southeast>()
        | bitboard::from_sq(king_sq).shift<southwest>();

      int score=0;

      // Pawn shield
      int shield=popcnt(pos.get_pieces(us, pawn) & king_zone);

      if (shield == 3) score+=10;
      else if (shield == 2) score+=5;
      else if (shield == 1) score-=10;
      else score-=40;

      // Attacks on king
      bitboard attackers=
        pos.attackers_to(king_sq, occ) & ~pos.get_color(us);

      int count=popcnt(attackers);

      score-=count * count * 8;

      if (attackers & pos.get_pieces(queen))  score-=20;
      if (attackers & pos.get_pieces(rook))   score-=10;
      if (attackers & pos.get_pieces(bishop)) score-=5;
      if (attackers & pos.get_pieces(knight)) score-=3;

      return score;
    }

    // ============================
    // Pawn structure
    // ============================

    int evaluate_pawns(const board& pos, bool us) {

      int score=0;
      bitboard pawns=pos.get_pieces(us, pawn);

      while (pawns) {

        u8 sq=pop_lsb(pawns);
        int file=fmake(sq);

        // Doubled pawn
        bitboard same_file=filea << file;
        if (popcnt(pos.get_pieces(us, pawn) & same_file) > 1)
          score-=10;

        // Isolated pawn
        bitboard adjacent{};
        if (file > 0) adjacent|=filea << (file - 1);
        if (file < 7) adjacent|=filea << (file + 1);

        if (!(pos.get_pieces(us, pawn) & adjacent))
          score-=15;
      }

      return score;
    }

  } // namespace


  // ============================================================
  // TAPERED EVALUATION
  // ============================================================

  int evaluate(const board& pos) {

    const bool us=pos.side_to_move;
    const bool them=!us;

    int mg_score=0;
    int eg_score=0;

    // ---------------------------
    // Phase calculation
    // ---------------------------

    int phase=0;
    for (int pt=knight; pt <= queen; ++pt) {
      phase+=phase_values[pt] *
        (popcnt(pos.get_pieces(white, pt)) +
        popcnt(pos.get_pieces(black, pt)));
    }
    phase=std::min(phase, max_phase);

    // ---------------------------
    // Material
    // ---------------------------

    for (int pt=pawn; pt <= queen; ++pt) {

      int diff=
        popcnt(pos.get_pieces(us, pt)) -
        popcnt(pos.get_pieces(them, pt));

      mg_score+=pt_values[pt] * diff;
      eg_score+=pt_values[pt] * diff;
    }

    // ---------------------------
    // Bishop pair
    // ---------------------------

    if (popcnt(pos.get_pieces(us, bishop)) >= 2) {
      mg_score+=30;
      eg_score+=40;
    }

    if (popcnt(pos.get_pieces(them, bishop)) >= 2) {
      mg_score-=30;
      eg_score-=40;
    }

    // ---------------------------
    // PSQT
    // ---------------------------

    for (u8 sq=a1; sq < n_sqs; ++sq) {

      i32 pc=pos.piece_on(sq);
      if (!pc) continue;

      i32 pt=ptmake(pc);
      bool c=make(pc);

      int mg=psq_table[c][pt][midgame][sq];
      int eg=psq_table[c][pt][endgame][sq];

      if (c == us) {
        mg_score+=mg;
        eg_score+=eg;
      }
      else {
        mg_score-=mg;
        eg_score-=eg;
      }
    }

    // ---------------------------
    // Pawn structure
    // ---------------------------

    mg_score+=evaluate_pawns(pos, us);
    mg_score-=evaluate_pawns(pos, them);

    eg_score+=evaluate_pawns(pos, us);
    eg_score-=evaluate_pawns(pos, them);

    // ---------------------------
    // King safety
    // ---------------------------

    mg_score+=evaluate_king_safety(pos, us);
    mg_score-=evaluate_king_safety(pos, them);

    // ---------------------------
    // Mobility
    // ---------------------------

    mg_score+=evaluate_mobility(pos, us);
    mg_score-=evaluate_mobility(pos, them);

    // ---------------------------
    // Blend MG / EG
    // ---------------------------

    int score=
      (mg_score * phase +
      eg_score * (max_phase - phase)) / max_phase;

    // Contempt
    score+=uci::contempt * 10 * (us == white ? 1 : -1);

    return score;
  }

} // namespace eval
