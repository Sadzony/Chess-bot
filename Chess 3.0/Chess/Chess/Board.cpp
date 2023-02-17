#include "Board.h"

Board::Board()
{
}

Board::~Board()
{
}

Board::Board(const Board& board)
{
	for(int row= MIN_ROW_INDEX; row< MAX_ROW_INDEX; row++)
	{
		for(int col= MIN_COL_INDEX; col< MAX_COL_INDEX; col++)
		{
			this->squares[row][col] = board.squares[row][col];
		}
	}
}

Square* Board::getSquare(int row, int col)
{
	return &squares[row][col];
}

Board* Board::hardCopy()
{
	Board* pBoard = new Board();

	for (int row = MIN_ROW_INDEX; row < MAX_ROW_INDEX; row++)
	{
		for (int col = MIN_COL_INDEX; col < MAX_COL_INDEX; col++)
		{
			Square temp = this->squares[row][col].hardCopy();
			if (temp.getOccupyingPiece())
			{
				pBoard->squares[row][col] = temp;
			}
		}
	}

	return pBoard;
}
int Board::GetHeuristic(PieceColor playerColor)
{
	int heuristic = 0;
	//iterate every square
	
	for (int y = MIN_COL_INDEX; y < MAX_COL_INDEX; y++) {
		for (int x = MIN_ROW_INDEX; x < MAX_ROW_INDEX; x++)
		{
			// get the piece on the square
			if (squares[x][y].hasOccupyingPiece())
			{
				std::shared_ptr<Piece> pieceOnSquare = squares[x][y].getOccupyingPiece();

				float multiplier = 1.0f;
				//calculate multiplier based on how far the piece is from the start
				if (pieceOnSquare.get()->getColor() == PieceColor::WHITE)
					multiplier += (0.1f * y);
				else
					multiplier += (((MAX_COL_INDEX - 1) - y) * 0.1f);

				//friendly color piece gives a positive to heuristic
				if (pieceOnSquare.get()->getColor() == playerColor)
					heuristic += (int)((int)pieceOnSquare.get()->getType() * multiplier);

				//opposite color piece gives a negative to heuristic
				else
					heuristic -= (int)((int)pieceOnSquare.get()->getType() * multiplier);
			}
		}
	}
	return heuristic;
}
