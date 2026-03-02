#include "chess.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int
main ()
{
	struct Board board = init_board();
	enum Team player = WHITE;
	uchar_t move[6] = { 0 };
start:
	puts(player == WHITE ? "White turn" : "Black Turn");
	puts(" +a--b--c--d--e--f--g--h+");
	for (int i = 7; i >= 0; i--) {
		putchar('1' + i);
		putchar(' ');
		fflush(stdout);

		for (int j = 0; j < 8; j++) {
			if (board.team[i][j] == WHITE)
				putchar('w');
			else if (board.team[i][j] == BLACK)
				putchar('b');
			else 
				putchar(' ');
			putchar(__pieces[board.piece[i][j]]);
			putchar(' ');
		}
		putchar(10);
	}
	puts(" +----------------------+");
err_start:

	memset(move, 0, sizeof(move));
	int r = read (STDIN, move, 5);

	if (r != 5)
		goto invalid_err;

	for (int i = 0; i < ((int)sizeof(move)); i++) {
		if (move[i] == '\n') {
			move[i] = 0;
			break;
		}
	}

	if (strcmp((char*)move, "exit") == 0) goto end;

	struct Move p_move = parse_move(move, board);
	
	if (__chess_error)
		goto invalid_err;
	if (!is_move_legal(player, p_move, board))
		goto invalid_err;

	update_board(player, p_move, &board);

	enum Team enemy = (player == WHITE) ? BLACK : WHITE;
	
	if (is_checkmate(enemy, board)) {
		puts("Checkmate!");
		return EXIT_SUCCESS;
	}
	if (is_draw(board)) {
		puts("Draw!");
		return EXIT_SUCCESS;
	}
	if (is_check(enemy, board))
		puts("Check!");
	if (__chess_error)
		goto invalid_err;

	player = enemy;
goto start;

end:
	return EXIT_SUCCESS;

invalid_err:
	printf("Invalid move!\n");
	__chess_error = false;
	goto err_start;
}
