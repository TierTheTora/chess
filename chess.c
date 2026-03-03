#include "chess.h"
#include <stdbool.h>
#include <stdlib.h>

/**
 * TODO: add castling and en passent
 * */

bool __chess_error = false;
const uchar_t __pieces[] = {
	'p', 'r', 'k', 'b', 'K', 'q', ' '
};

/* get piece movement direction */
inline int
get_dir (enum Team team)
{
	if (team == BLACK) return (START_W - START_B) < 0 ? -1 : 1;
	else               return (START_B - START_W) < 0 ? -1 : 1;
}

struct Move
parse_move (uchar_t notation[6], struct Board board)
{
	int x, y, dx, dy;
	enum Piece piece;
	struct Move move = { 0 };
	struct Space space, dest;

	if (notation[0] < 'a' || notation[0] > 'h')
		goto return_err;
	if (notation[2] < 'a' || notation[2] > 'h')
		goto return_err;

	x  = notation[0] - 'a';
	dx = notation[2] - 'a';

	if (notation[1] < '1' || notation[1] > '8')
		goto return_err;
	if (notation[3] < '1' || notation[3] > '8')
		goto return_err;

	y  = notation[1] - '1';
	dy = notation[3] - '1';

	piece = board.piece[y][x];

	if (piece == NONE)
		goto return_err;

	space.x = x; space.y = y;
	dest.x = dx; dest.y = dy;
	move.dest = dest;
	move.space = space;
	move.piece = piece;

	return move;

	return_err:
		__chess_error = true;
		return move;
}

struct Board
init_board ()
{
	struct Board b;
	const uchar_t tmp[8][8] = {
		{"rkbqKbkr"},
		{"pppppppp"},
		{"        "},
		{"        "},
		{"        "},
		{"        "},
		{"pppppppp"},
		{"rkbqKbkr"},
	};

	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			switch (tmp[i][j]) {
			case 'r':
				b.piece[i][j] = ROOK;
				break;
			case 'k':
				b.piece[i][j] = KNIGHT;
				break;
			case 'b':
				b.piece[i][j] = BISHOP;
				break;
			case 'K':
				b.piece[i][j] = KING;
				break;
			case 'q':
				b.piece[i][j] = QUEEN;
				break;
			case 'p':
				b.piece[i][j] = PAWN;
				break;
			default:
				b.piece[i][j] = NONE;
				break;
			}
			b.space[i][j].x = j;
			b.space[i][j].y = i;

			if (i <= 1) b.team[i][j] = WHITE;
			else if (i >= 6) b.team[i][j] = BLACK;
			else b.team[i][j] = NO_TEAM;
		}
	}

	return b;
}

void
update_board (enum Team player, struct Move target,
              struct Board *board)
{
	int ty = target.space.y;
	int tx = target.space.x;
	int dy = target.dest.y;
	int dx = target.dest.x;

	enum Team piece_team = board->team[ty][tx];
	enum Team dpiece_team = board->team[dy][dx];
	enum Piece piece = board->piece[ty][tx];
	const enum Piece dest_piece = board->piece[dy][dx];

	if (player != piece_team)
		goto return_err;
	if (dpiece_team == player && dest_piece != NONE)
		goto return_err;
	if (piece == NONE)
		goto return_err;

	if (piece == PAWN) {
		if ((player == WHITE && dy == START_B) ||
			(player == BLACK && dy == START_W))
			piece = QUEEN;
	}

	board->piece[ty][tx] = NONE;
	board->team[ty][tx] = NO_TEAM;

	board->piece[dy][dx] = piece;
	board->team[dy][dx] = player;

	return;

	return_err:
		__chess_error = true;
}

bool
is_path_blocked (struct Move move, struct Board board)
{
	int dx = move.dest.x - move.space.x;
	int dy = move.dest.y - move.space.y;
	
	int step_x = (dx == 0) ? 0 : (dx > 0 ? 1 : -1);
	int step_y = (dy == 0) ? 0 : (dy > 0 ? 1 : -1);

	int x = move.space.x + step_x;
	int y = move.space.y + step_y;

	while (x != move.dest.x || y != move.dest.y) {
		if (board.piece[y][x] != NONE)
			return true;

		x += step_x;
		y += step_y;
	}

	return false;
}

bool
find_king (enum Team team, struct Board board, struct Space *space)
{
	for (int i = 0; i < BOARD_H; i++) {
		for (int j = 0; j < BOARD_W; j++) {
			if ((board.piece[i][j] == KING) &&
			(board.team[i][j] == team)) {
				space->x = j;
				space->y = i;
				return true;
			}
		}
	}
	return false;
}

bool
is_check (enum Team team, struct Board board)
{
	struct Space king_sp;

	if (!find_king(team, board, &king_sp))
		return false;

	enum Team enemy = (team == WHITE) ? BLACK : WHITE;

	for (int i = 0; i < BOARD_H; i++) {
		for (int j = 0; j < BOARD_W; j++) {
			if (board.team[i][j] != enemy)
				continue;
			
			struct Move move;
			move.space.x = j;
			move.space.y = i;
			move.dest = king_sp;
			move.piece = board.piece[i][j];
			if (is_move_valid(move, board))
				return true;
		}
	}
	return false;
}

bool
is_checkmate (enum Team team, struct Board board)
{
	if (!is_check(team, board))
		return false;
	for (int i = 0; i < BOARD_H; i++) {
		for (int j = 0; j < BOARD_W; j++) {
			if (board.team[i][j] != team)
				continue;
			for (int di = 0; di < BOARD_H; di++) {
				for (int dj = 0; dj < BOARD_W; dj++) {
					struct Move move;
					move.space.x = j;
					move.space.y = i;
					move.dest.x = dj;
					move.dest.y = di;
					move.piece = board.piece[i][j];

					if (!is_move_valid(move, board))
						continue;

					struct Board tmp = board;
					update_board(team, move, &tmp);

					if (!is_check(team, tmp))
						return false; /* escape found */
				}
			}
		}
	}
	return true; /* in check */
}

bool
is_draw (struct Board board)
{
	for (int i = 0; i < BOARD_H; i++) {
		for (int j = 0; j < BOARD_W; j++) {
			/* check if other pieces than kings exist */
			if (board.piece[i][j] != KING
			 && board.piece[i][j] != NONE)
				return false;
		}
	}
	/* only kings, it a draw! */
	return true;
}

bool
move_pawn (struct Move move, struct Board board)
{
	const enum Team team = board.team[move.space.y][move.space.x];
	int dir, start, dx, dy;

	dx = move.dest.x - move.space.x;
	dy = move.dest.y - move.space.y;

	dir = get_dir(team);
	if (team == WHITE)
		start = START_W + dir;
	else
		start = START_B + dir;
	if (dx == 0 && dy == dir) {
		if (board.piece[move.dest.y][move.dest.x] == NONE)
			return true;
		return false;
	}
	if (dx == 0 && dy == 2 * dir && move.space.y == start) {
		if (board.piece[move.space.y + dir][move.space.x] == NONE &&
			board.piece[move.dest.y][move.dest.x] == NONE) {
			return true;
		}
		return false;
	}
	if (abs(dx) == 1 && dy == dir) {
		if (board.piece[move.dest.y][move.dest.x] == NONE)
			return false;
		if (board.team[move.dest.y][move.dest.x] == team)
			return false;
		return true;
	}
	return false;
}

bool
is_move_valid (struct Move move, struct Board board)
{
	int dx, dy;
	dx = move.dest.x - move.space.x;
	dy = move.dest.y - move.space.y;

	switch (move.piece) {
	case ROOK:
		if (!(dx == 0 && dy != 0) && !(dy == 0 && dx != 0))
			return false;
		if (is_path_blocked(move, board))
			return false;
		return true;
	case KNIGHT:
		if (!(abs(dx) == 2 && abs(dy) == 1) &&
			!(abs(dy) == 2 && abs(dx) == 1))
			return false;
		return true;
	case BISHOP:
		if (abs(dx) != abs(dy))
			return false;
		if (is_path_blocked(move, board))
			return false;
		return true;
	case KING:
		if (abs(dx) > 1 || abs(dy) > 1)
			return false;
		return true;
	case QUEEN:
		if (is_path_blocked(move, board))
			return false;
		if (abs(dx) == abs(dy))
			return true;
		if (!(dx == 0 && dy != 0) && !(dy == 0 && dx != 0))
			return false;
		return true;
	case PAWN:
		return move_pawn(move, board);
	default:
		return false;
	}
}

bool
is_move_legal (enum Team team, struct Move move, struct Board board)
{
	if (!is_move_valid(move, board))
		return false;

	struct Board tmp = board;
	update_board(team, move, &tmp);

	if (is_check(team, tmp))
		return false;
	return true;

}
