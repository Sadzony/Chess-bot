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
int Board::GetHeuristic(GameStatus* p_status)
{
	GameStatus* thisStatus = new GameStatus(*p_status);
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
				PieceColor color = pieceOnSquare.get()->getColor();
				//Multiplier is found by calculating the mobility and move value
				int mobilityValue = 0; 
				//Space value is taken from piece-square tables and helpful pawn positions
				int spaceValue = 0;
				//PAWNS
				if (type == PieceType::PAWN)
				{
					//White
					if (color == PieceColor::WHITE)
					{
						//Promotion check
						if(row == (MAX_ROW_INDEX - 1))
							//The bot always goes for queen
							Gameplay::pawnPromotion(evaluationBoard, row, col, PieceType::QUEEN);
							pieceOnSquare = evaluationBoard->squares[row][col].getOccupyingPiece();
							type = pieceOnSquare.get()->getType();
						
					}
					//Black
					else if(color == PieceColor::BLACK)
					{
						//Promotion check
						if (row == 0) {
							//The bot always goes for queen
							Gameplay::pawnPromotion(evaluationBoard, row, col, PieceType::QUEEN);
							pieceOnSquare = evaluationBoard->squares[row][col].getOccupyingPiece();
							type = pieceOnSquare.get()->getType();
						}
					}
				}

				//Mobility check
				//Get all possible moves
				std::vector<std::shared_ptr<Move>> pieceMoves = Gameplay::getValidMoves(thisStatus, evaluationBoard, pieceOnSquare, row, col);
				for (std::shared_ptr<Move> move : pieceMoves)
				{
					Move* nextMove = move.get();
					//Give a multiplier bonus for each move available (mobility bonus). Only applies to forward mobility
					if ((color == PieceColor::WHITE && nextMove->getDestinationPosition().first > nextMove->getOriginPosition().first)
						|| (color == PieceColor::BLACK && nextMove->getDestinationPosition().first < nextMove->getOriginPosition().first))
					{
						switch (type)
						{
						case(PieceType::PAWN):
							mobilityValue += 10;
						case(PieceType::ROOK):
							mobilityValue += 3;
						case(PieceType::KNIGHT):
							mobilityValue += 50;
						case(PieceType::BISHOP):
							mobilityValue += 7;
						case(PieceType::QUEEN):
							mobilityValue += 2;
						}
					}

					//give additional bonus for special move types
					//MoveType moveType = move.get()->getType();
					//if (moveType == MoveType::CAPTURE)
					//	mobilityValue += 15;
					//else if (moveType == MoveType::EN_PASSANT)
					//	mobilityValue += 10;
					//else if (moveType == MoveType::CASTLING)
					//	mobilityValue += 100;
				}
				
				//Value is sum of values
				int value = (int)type + spaceValue + mobilityValue;

				//Evaluation of points
				
				// white piece gives a positive to heuristic
				if (color == PieceColor::WHITE)
					heuristic += value;

				//black piece gives a negative to heuristic
				else if (color == PieceColor::BLACK)
					heuristic -= value;
			}
		}
	}
	delete evaluationBoard;
	delete thisStatus;
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
