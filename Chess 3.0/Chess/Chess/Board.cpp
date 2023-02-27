#include "Board.h"
#include "Gameplay.h"

Board::Board()
{
}

Board::~Board()
{
}

Board::Board(const Board& board)
{
	for (int row = MIN_ROW_INDEX; row < MAX_ROW_INDEX; row++)
	{
		for (int col = MIN_COL_INDEX; col < MAX_COL_INDEX; col++)
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
int Board::GetHeuristic()
{
	int heuristic = 0;
	Board* evaluationBoard = new Board(*this);
	//iterate every square
	for (int row = MIN_ROW_INDEX; row < MAX_ROW_INDEX; row++) {
		for (int col = MIN_COL_INDEX; col < MAX_COL_INDEX; col++) {
			// get the piece on the square
			if (squares[row][col].hasOccupyingPiece())
			{
				std::shared_ptr<Piece> pieceOnSquare = squares[row][col].getOccupyingPiece();
				PieceType type = pieceOnSquare.get()->getType();
				float multiplier = 1.0f;
				if (type != PieceType::KING)
				{
					//calculate multiplier based on how far the piece is from the start
					if (pieceOnSquare.get()->getColor() == PieceColor::WHITE)
					{
						multiplier += (0.15f * row);
						//Promotion check
						if (type == PieceType::PAWN && row == (MAX_ROW_INDEX - 1))
						{
							Gameplay::pawnPromotion(evaluationBoard, row, col, PieceType::QUEEN);
						}

					}
						
					else 
					{
						multiplier += (((MAX_ROW_INDEX - 1) - row) * 0.15f);
						if (type == PieceType::PAWN && row == 0)
						{
							Gameplay::pawnPromotion(evaluationBoard, row, col, PieceType::QUEEN);
						}
					}
						
				}
				else
				{
					if (pieceOnSquare.get()->getColor() == PieceColor::WHITE)
						multiplier += 0.05 * row;
					else
						multiplier += (((MAX_ROW_INDEX - 1) - row) * 0.05);
				}
				pieceOnSquare = evaluationBoard->squares[row][col].getOccupyingPiece();
				// white piece gives a positive to heuristic
				if (pieceOnSquare.get()->getColor() == PieceColor::WHITE)
					heuristic += (int)((int)pieceOnSquare.get()->getType());

				//black piece gives a negative to heuristic
				else if (pieceOnSquare.get()->getColor() == PieceColor::BLACK)
					heuristic -= (int)((int)pieceOnSquare.get()->getType());
			}
		}
	}
	return heuristic;
}

vecPieces Board::GetLivePieces(PieceColor playerColor)
{
	PieceInPosition pip;
	vecPieces outPieces = vecPieces();
	for (int i = MIN_ROW_INDEX; i < MAX_ROW_INDEX; i++)
	{
		for (int j = MIN_COL_INDEX; j < MAX_COL_INDEX; j++)
		{
			std::shared_ptr<Piece> pPiece = getSquare(i, j)->getOccupyingPiece();

			if (pPiece == nullptr)
				continue;
			if (pPiece->getColor() != playerColor)
				continue;

			pip.piece = pPiece;
			pip.row = i;
			pip.col = j;
			outPieces.emplace_back(pip);
		}
	}
	return outPieces;
}
