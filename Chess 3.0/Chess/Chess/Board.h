#ifndef BOARD_H
#define BOARD_H

/*
Board - this is the chess board
it contains Square objects (which contain a piece, or is empty).
*/

#include "Square.h"
#include <memory>
#include <vector>
#include <utility>
#include <map>


class GameStatus;
typedef std::vector<PieceInPosition> vecPieces;
typedef std::pair<PieceColor, PieceType> PieceCharacteristics;

class Board
{
public:
		static const int WIDTH = 8;
		static const int HEIGHT = 8;

	private:
		//Piece scores for spaces
		std::map<PieceCharacteristics, std::array<std::array<int, WIDTH>, HEIGHT>> pieceSquareTables;
		//Maps a piece to a position on board
		//std::map<Piece*, 
		Square squares[WIDTH][HEIGHT];
	public:
		static const int MIN_COL_INDEX = 0;
		static const int MIN_ROW_INDEX = 0;
		static const int MAX_COL_INDEX = HEIGHT;
		static const int MAX_ROW_INDEX = WIDTH;
		Board();
		~Board();
		Board(const Board& board);

		Square* getSquare(int row, int col);

		Board* hardCopy();
		int GetHeuristic(GameStatus* p_status);
		vecPieces GetLivePieces(PieceColor playerColor);

		void GeneratePieceSquareTables();
		void GenerateEndgameTable();
		
};

#endif