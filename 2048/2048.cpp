#include "2048.h"

int main()
{
	// 2048 'board'
	srand(time(0));
	uint64 board = initialize_board();

	// SIMULATION
	while (true)
	{
		// display board
		display(board);

		// generate board state for every directional shift
		uint64 states[4] = { shift_right(board), shift_up(board), shift_left(board), shift_down(board) };

		// find move with highest expectimax value
		int num_valid = 0;
		int max_score = 0;
		uint64 new_board = board;
		for (int i = 0; i < 4; i++)
		{
			if (states[i] == board) continue;
			num_valid++;
			
			int score = expectimax(states[i], 4);
			if (score > max_score)
			{
				max_score = score;
				new_board = states[i];
			}
		}
		cout << "SCORE: " << max_score << endl;

		// if no direction is valid, the game is over 
		if (num_valid == 0) break;

		// insert random tile
		uint64 r = draw_tile();
		board = insert_rand_tile(new_board, r);

	}

	cout << "Final board:" << endl;
	display(board);

	return 0;
}