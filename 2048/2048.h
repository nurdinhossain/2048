#pragma once
#include <iostream>
#include <vector>

using namespace std;
using uint64 = unsigned long long;

// BITBOARD FUNCTIONS
const int lsb(uint64 bb)
{
	unsigned long index;
	_BitScanForward64(&index, bb);
	return index;
}

// MASKS
const uint64 masks[16] = {
	0xF,
	0xF0,
	0xF00,
	0xF000,

	0xF0000,
	0xF00000,
	0xF000000,
	0xF0000000,

	0xF00000000,
	0xF000000000,
	0xF0000000000,
	0xF00000000000,

	0xF000000000000,
	0xF0000000000000,
	0xF00000000000000,
	0xF000000000000000
};

// HELPER FUNCTIONS 
const uint64 add_tile(uint64 board, uint64 num, int tile)
{
	// create mask for new tile data
	uint64 write_mask = num << 4 * tile; 

	// write from the write mask
	board |= write_mask;

	// return new board
	return board;
}

const uint64 double_tile(uint64 board, int tile)
{
	// retrieve read mask
	uint64 read_mask = masks[tile];

	// double value found from read
	uint64 value = board & read_mask;
	value++;

	// nullify old value and replace with new one
	board &= ~read_mask;
	board |= value;

	return board;
}

const uint64 reset_tile(uint64 board, int tile)
{
	// retrieve read mask
	uint64 read_mask = masks[tile];

	// reset and return
	board &= ~read_mask;
	return board;
}

// display a board in conventional 2048 format
void display(uint64 board)
{
	for (int i = 15; i >= 0; i--)
	{
		// get read mask
		uint64 read_mask = masks[i];

		// read value
		uint64 value = board & read_mask;

		// shift value to the right by i*4
		value = value >> i * 4; 

		// print out 2 to the power of value
		int disp_value = static_cast<int>(pow(2, value));
		disp_value = disp_value == 1 ? 0 : disp_value;
		cout << disp_value << "\t";

		// indent every 4
		if (i % 4 == 0) cout << endl << endl;
	}
	cout << endl;
}

// TILE SHIFT AND MERGE FUNCTIONS 

// 3 2 1 0
const uint64 shift_down_col(uint64 board, int col)
{
	int last_dropped = col; 
	for (int i = 1; i < 4; i++)
	{
		// get tile we are CURRENTLY looking at to drop
		int current_tile = i * 4 + col;

		// get tile value at tile
		uint64 current_tile_value = masks[current_tile] & board;

		// if tile is empty, increment
		if (current_tile_value == 0) continue;
		
		// get tile at last_dropped
		uint64 tile_at_last_dropped = masks[last_dropped] & board;

		// drop current tile
		
		//if tile_at_last_dropped is 0 (empty tile), we can replace it 
		if (tile_at_last_dropped == 0)
		{
			uint64 adjusted_current_tile = current_tile_value >> (current_tile - last_dropped) * 4;
			board |= adjusted_current_tile;
		}

		// if tile_at_last_dropped is NOT 0 (occupied tile), check if its merge-able. Otherwise, drop the current tile above it
		else
		{
			uint64 adjusted_last_dropped_tile = tile_at_last_dropped >> last_dropped * 4;
			uint64 adjusted_current_tile = current_tile_value >> current_tile * 4;

			// if they're equal, then merge
			if (adjusted_last_dropped_tile == adjusted_current_tile)
			{
				uint64 addition = 1ULL << last_dropped * 4;
				board += addition;
				last_dropped += 4;
			}

			// otherwise, drop it on top
			else
			{
				// if the tile is resting directly on top, don't bother
				last_dropped += 4;
				if (last_dropped == current_tile) continue;

				uint64 adjusted_current_tile = current_tile_value >> (current_tile - last_dropped) * 4;
				board |= adjusted_current_tile;
			}
		}

		// set tile at original location to 0
		board ^= current_tile_value;
	}

	return board;
}

const uint64 shift_down(uint64 board)
{
	board = shift_down_col(board, 0);
	board = shift_down_col(board, 1);
	board = shift_down_col(board, 2);
	board = shift_down_col(board, 3);

	return board;
}

const uint64 shift_up_col(uint64 board, int col)
{
	int last_dropped = col + 12;
	for (int i = 2; i >= 0; i--)
	{
		// get tile we are CURRENTLY looking at to drop
		int current_tile = i * 4 + col;

		// get tile value at tile
		uint64 current_tile_value = masks[current_tile] & board;

		// if tile is empty, increment
		if (current_tile_value == 0) continue;

		// get tile at last_dropped
		uint64 tile_at_last_dropped = masks[last_dropped] & board;

		// drop current tile

		//if tile_at_last_dropped is 0 (empty tile), we can replace it 
		if (tile_at_last_dropped == 0)
		{
			uint64 adjusted_current_tile = current_tile_value << (last_dropped - current_tile) * 4;
			board |= adjusted_current_tile;
		}

		// if tile_at_last_dropped is NOT 0 (occupied tile), check if its merge-able. Otherwise, drop the current tile above it
		else
		{
			uint64 adjusted_last_dropped_tile = tile_at_last_dropped >> last_dropped * 4;
			uint64 adjusted_current_tile = current_tile_value >> current_tile * 4;

			// if they're equal, then merge
			if (adjusted_last_dropped_tile == adjusted_current_tile)
			{
				uint64 addition = 1ULL << last_dropped * 4;
				board += addition;
				last_dropped -= 4;
			}

			// otherwise, drop it on top
			else
			{
				// if the tile is resting directly on top, don't bother
				last_dropped -= 4;
				if (last_dropped == current_tile) continue;

				uint64 adjusted_current_tile = current_tile_value << (last_dropped - current_tile) * 4;
				board |= adjusted_current_tile;
			}
		}

		// set tile at original location to 0
		board ^= current_tile_value;
	}

	return board;
}

const uint64 shift_up(uint64 board)
{
	board = shift_up_col(board, 0);
	board = shift_up_col(board, 1);
	board = shift_up_col(board, 2);
	board = shift_up_col(board, 3);

	return board;
}

const uint64 shift_left_row(uint64 board, int row)
{
	int last_dropped = row * 4 + 3;
	for (int i = 2; i >= 0; i--)
	{
		// get tile we are CURRENTLY looking at to drop
		int current_tile = row * 4 + i;

		// get tile value at tile
		uint64 current_tile_value = masks[current_tile] & board;

		// if tile is empty, increment
		if (current_tile_value == 0) continue;

		// get tile at last_dropped
		uint64 tile_at_last_dropped = masks[last_dropped] & board;

		// drop current tile

		//if tile_at_last_dropped is 0 (empty tile), we can replace it 
		if (tile_at_last_dropped == 0)
		{
			uint64 adjusted_current_tile = current_tile_value << (last_dropped - current_tile) * 4;
			board |= adjusted_current_tile;
		}

		// if tile_at_last_dropped is NOT 0 (occupied tile), check if its merge-able. Otherwise, drop the current tile above it
		else
		{
			uint64 adjusted_last_dropped_tile = tile_at_last_dropped >> last_dropped * 4;
			uint64 adjusted_current_tile = current_tile_value >> current_tile * 4;

			// if they're equal, then merge
			if (adjusted_last_dropped_tile == adjusted_current_tile)
			{
				uint64 addition = 1ULL << last_dropped * 4;
				board += addition;
				last_dropped--;
			}

			// otherwise, drop it on top
			else
			{
				// if the tile is resting directly on top, don't bother
				last_dropped--;
				if (last_dropped == current_tile) continue;

				uint64 adjusted_current_tile = current_tile_value << (last_dropped - current_tile) * 4;
				board |= adjusted_current_tile;
			}
		}

		// set tile at original location to 0
		board ^= current_tile_value;
	}

	return board;
}

const uint64 shift_left(uint64 board)
{
	board = shift_left_row(board, 0);
	board = shift_left_row(board, 1);
	board = shift_left_row(board, 2);
	board = shift_left_row(board, 3);

	return board;
}

const uint64 shift_right_row(uint64 board, int row)
{
	int last_dropped = row * 4;
	for (int i = 1; i < 4; i++)
	{
		// get tile we are CURRENTLY looking at to drop
		int current_tile = row * 4 + i;

		// get tile value at tile
		uint64 current_tile_value = masks[current_tile] & board;

		// if tile is empty, increment
		if (current_tile_value == 0) continue;

		// get tile at last_dropped
		uint64 tile_at_last_dropped = masks[last_dropped] & board;

		// drop current tile

		//if tile_at_last_dropped is 0 (empty tile), we can replace it 
		if (tile_at_last_dropped == 0)
		{
			uint64 adjusted_current_tile = current_tile_value >> (current_tile - last_dropped) * 4;
			board |= adjusted_current_tile;
		}

		// if tile_at_last_dropped is NOT 0 (occupied tile), check if its merge-able. Otherwise, drop the current tile above it
		else
		{
			uint64 adjusted_last_dropped_tile = tile_at_last_dropped >> last_dropped * 4;
			uint64 adjusted_current_tile = current_tile_value >> current_tile * 4;

			// if they're equal, then merge
			if (adjusted_last_dropped_tile == adjusted_current_tile)
			{
				uint64 addition = 1ULL << last_dropped * 4;
				board += addition;
				last_dropped++;
			}

			// otherwise, drop it on top
			else
			{
				// if the tile is resting directly on top, don't bother
				last_dropped++;
				if (last_dropped == current_tile) continue;

				uint64 adjusted_current_tile = current_tile_value >> (current_tile - last_dropped) * 4;
				board |= adjusted_current_tile;
			}
		}

		// set tile at original location to 0
		board ^= current_tile_value;
	}

	return board;
}

const uint64 shift_right(uint64 board)
{
	// loop unrolling
	board = shift_right_row(board, 0);
	board = shift_right_row(board, 1);
	board = shift_right_row(board, 2);
	board = shift_right_row(board, 3);

	return board;
}

// FUNCTIONS FOR GENERATING NEW BOARD STATES

// code from https://github.com/nneonneo/2048-ai/blob/master/2048.cpp#L52
const int count_empty(uint64 board)
{
	// sets nibble to 0 if occupied, sets to 1 if unoccupied
	board |= (board >> 2) & 0x3333333333333333ULL;
	board |= (board >> 1);
	board = ~board & 0x1111111111111111ULL;
	
	// sums up nibbles and masks to get answer 
	board += board >> 32;
	board += board >> 16;
	board += board >> 8;
	board += board >> 4;
	return board & 0xF;
}

const uint64 draw_tile()
{
	return ((rand() % 10) < 9) ? 1 : 2;
}

const uint64 insert_tile(uint64 board, uint64 tile, int index)
{
	// while loop to iterate through board and find unoccupied tile 
	uint64 tmp = board;
	while (true)
	{
		// get through any occupied spaces
		while ((tmp & 0xF) != 0)
		{
			tmp >>= 4;
			tile <<= 4;
		}

		// stop condition
		if (index == 0) break;

		// iterate through unoccupied spaces while decrementing index
		index--;
		tmp >>= 4;
		tile <<= 4;
	}

	// bitwise OR the shifted tile with the board
	return board | tile;
}

const uint64 insert_rand_tile(uint64 board, uint64 tile)
{
	// get index of unoccupied tile we would like to fill
	int index = rand() % count_empty(board);

	return insert_tile(board, tile, index);
}

const uint64 initialize_board()
{
	// initial random tile
	uint64 first_tile = draw_tile();
	int r = rand() % 16;
	uint64 board = first_tile << (r * 4);

	// second random tile
	uint64 second_tile = draw_tile();
	return insert_rand_tile(board, second_tile);
}

// EVAL
const int MULTIPLIER = 1000;
const int eval(uint64 board)
{
	int score = 0;
	int occupied = 0;
	int highest_tile = 0;

	// prioritize having a high score while occupying minimal tiles
	for (int i = 0; i < 16; i++)
	{
		uint64 intersect = masks[i] & board;
		int tile = (intersect >> (i * 4));
		score += tile * MULTIPLIER;
		occupied += (intersect > 0) ? 1 : 0;

		highest_tile = max(highest_tile, tile);
	}

	// give a boost for having highest tile in a corner
	if ((masks[0] & board) == highest_tile) score += highest_tile * MULTIPLIER;
	else if (((masks[3] & board) >> 12) == highest_tile) score += highest_tile * MULTIPLIER;
	else if (((masks[12] & board) >> 48) == highest_tile) score += highest_tile * MULTIPLIER;
	else if (((masks[15] & board) >> 60) == highest_tile) score += highest_tile * MULTIPLIER;

	return score / occupied;
}

// SEARCH 
const int expectimax(uint64 board, int depth)
{
	// terminal node: evaluate
	if (depth == 0) return eval(board);

	// function is executed only after PLAYER's move, so must add random tile before continuing
	int empty = count_empty(board);
	int score = 0;
	int possible_states = 0;
	uint64 states[8];

	for (int i = 0; i < empty; i++)
	{
		// add a 2
		uint64 tmp = insert_tile(board, 1, i);
		uint64 tmp2 = insert_tile(board, 2, i);

		// if depth is 1, do not make a move after inserting a random tile (this is the new base case)
		/*if (depth == 1)
		{
			score += static_cast<int>( 0.9 * eval(tmp) );
			score += static_cast<int>( 0.1 * eval(tmp2) );
			possible_states += 2;
			continue;
		}*/

		// possible move states after placing a 2
		states[0] = shift_right(tmp);
		states[1] = shift_up(tmp);
		states[2] = shift_left(tmp);
		states[3] = shift_down(tmp);

		// possible move states after placing a 4
		states[4] = shift_right(tmp2);
		states[5] = shift_up(tmp2);
		states[6] = shift_left(tmp2);
		states[7] = shift_down(tmp2);

		// get weighted score
		for (int k = 0; k < 8; k++)
		{
			if (k < 4)
			{
				if (states[k] == tmp) continue;
				score += static_cast<int>( 0.9 * expectimax(states[k], depth-1) );
			}
			else
			{
				if (states[k] == tmp2) continue;
				score += static_cast<int>( 0.1 * expectimax(states[k], depth - 1) );
			}

			// increment possible states if we get to this point
			possible_states++;
		}
	}

	// terminal node: evaluate as 0
	if (possible_states == 0) return 0;

	return score / possible_states;
}
