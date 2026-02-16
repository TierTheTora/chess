#ifndef CHESS_H
#define CHESS_H

#include <stdbool.h>

#define STDIN 0
#define STDOUT 1

#define BOARD_W 8
#define BOARD_H 8

#define START_W 0
#define START_B 7

typedef unsigned char  uchar_t;
typedef unsigned short ushort_t;
typedef unsigned int   uint_t;
typedef unsigned long  ulong_t;

enum Piece {
	PAWN,
	ROOK,
	KNIGHT,
	BISHOP,
	KING,
	QUEEN,
	NONE,
};

enum Team {
	WHITE,
	BLACK,
	NO_TEAM,
};

extern bool __chess_error;
extern const uchar_t __pieces[];

#define PIECES_SZ ((int)sizeof(__pieces))

struct Space {
	int x, y;
};

struct Move {
	enum Piece piece;
	struct Space space;
	struct Space dest;
};

struct Board {
	enum Team team[8][8];
	enum Piece piece[8][8];
	struct Space space[8][8];
};

int get_dir (enum Team team);
struct Move parse_move (uchar_t notation[6], struct Board board);
struct Board init_board ();
void update_board (enum Team player, struct Move target,
                   struct Board *board);
bool is_path_blocked (struct Move move, struct Board board);
bool find_king (enum Team team, struct Board board,
                struct Space *space);
bool is_check (enum Team team, struct Board board);
bool is_checkmate (enum Team team, struct Board board);
bool move_pawn (struct Move move, struct Board board);
bool is_move_valid (struct Move move, struct Board board);
bool is_move_legal (enum Team team, struct Move move,
                    struct Board board);


#endif // CHESS_H
