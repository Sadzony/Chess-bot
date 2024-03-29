#include "Board.h"
#include "Gameplay.h"
#include <array>

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
	this->pieceSquareTables = board.pieceSquareTables;
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
	pBoard->pieceSquareTables = pieceSquareTables;

	return pBoard;
}
int Board::GetHeuristic(GameStatus* p_status)
{
	int heuristic = 0;
	int pieceCount = 0;

	valuedPieceList whitePieces = valuedPieceList();
	valuedPieceList blackPieces = valuedPieceList();
	
	int whiteAttackTable[WIDTH][HEIGHT] = {};
	int blackAttackTable[WIDTH][HEIGHT] = {};
	//iterate every square
	for (int row = MIN_ROW_INDEX; row < MAX_ROW_INDEX; row++) {
		for (int col = MIN_COL_INDEX; col < MAX_COL_INDEX; col++) {
			// get the piece on the square
			if (squares[row][col].hasOccupyingPiece())
			{
				std::shared_ptr<Piece> pieceOnSquare = squares[row][col].getOccupyingPiece();
				PieceType type = pieceOnSquare.get()->getType();
				PieceColor color = pieceOnSquare.get()->getColor();
				ValuedPiece pip = ValuedPiece(pieceOnSquare, col, row);
				pieceCount++;


				//Generate space bonus from piece square table
				int pieceSquareValue = pieceSquareTables[std::make_pair(color, type)].at(col).at(row);
				pip.value += pieceSquareValue;

				//attack table for pawns
				if (type == PieceType::PAWN && color == PieceColor::WHITE)
				{
					int attackRow = row + 1;
					int attackCol = col + 1;
					if (row < MAX_ROW_INDEX)
					{
						if(col < MAX_COL_INDEX)
							whiteAttackTable[attackRow][attackCol]++;
						attackCol = col - 1;
						if (col >= MIN_COL_INDEX)
							whiteAttackTable[attackRow][attackCol]++;
					}
				}
				else if (type == PieceType::PAWN && color == PieceColor::BLACK)
				{
					int attackRow = row - 1;
					int attackCol = col + 1;
					if (row >= MIN_COL_INDEX)
					{
						if (col < MAX_COL_INDEX)
							blackAttackTable[attackRow][attackCol]++;
						attackCol = col - 1;
						if (col >= MIN_COL_INDEX)
							blackAttackTable[attackRow][attackCol]++;
					}
				}

				//Get all available moves for this piece
				std::vector<std::shared_ptr<Move>> pieceMoves = Gameplay::getValidMoves(p_status, this, pieceOnSquare, row, col);
				for (std::shared_ptr<Move> move : pieceMoves)
				{
					Move* nextMove = move.get();
					bool mobility = true;
					//std::pair<int, int> origin = nextMove->getOriginPosition();
					std::pair<int, int> destination = nextMove->getDestinationPosition();

					//Generate attack tables and check if mobility bonus applies.
					//mobility bonus: having more forward mobility means more board control. 
					if (color == PieceColor::WHITE)
					{
						//check for row growth
						//if (destination.first > origin.first)
							//mobility = true;
						//Increment attack table at that position
						if (type != PieceType::PAWN)
							whiteAttackTable[destination.first][destination.second]++;
					}
					else if (color == PieceColor::BLACK)
					{
						//if (destination.first < origin.first)
							//mobility = true;
						if(type != PieceType::PAWN)
							blackAttackTable[destination.first][destination.second]++;
					}

					//There's a different mobility bonus based on the piece type
					if(mobility){
						switch (type) {
						case(PieceType::ROOK):
							pip.value += 2;
						case(PieceType::BISHOP):
							pip.value += 2;
						case(PieceType::KNIGHT):
							pip.value += 20;
						case(PieceType::QUEEN):
							pip.value += 1;
						}
					}
				}

				//Add the piece to the litst of pieces.
				if (color == PieceColor::WHITE)
					whitePieces.push_back(pip);
				else
					blackPieces.push_back(pip);
			}
		}
	}
	for (ValuedPiece pip : whitePieces)
	{
		float multiplier = 1.0f;
		//Threat analysis: if less friendly pieces attack this spot than enemy pieces, then apply a penalty
		int defenceValue = whiteAttackTable[pip.row][pip.col];
		int threatValue = blackAttackTable[pip.row][pip.col];
		
		if (threatValue > 0)
		{
			int difference = defenceValue - threatValue;
			if (difference <= 0)
			{
				multiplier = 0.9f + (0.1f * difference);
			}
				
		}
		heuristic += pip.value * multiplier;

	}
	for (ValuedPiece pip : blackPieces)
	{
		float multiplier = 1.0f;
		//Threat analysis: if less friendly pieces attack this spot than enemy pieces, then apply a penalty
		int defenceValue = blackAttackTable[pip.row][pip.col];
		int threatValue = whiteAttackTable[pip.row][pip.col];

		if (threatValue > 0)
		{
			int difference = defenceValue - threatValue;
			if (difference <= 0)
			{
				multiplier = 0.9f + (0.1f * difference);
			}
		}
		heuristic -= pip.value * multiplier;
	}
	//Determine that this board is now in endgame
	if (abs(heuristic) > 1000 || pieceCount < 8)
		GenerateEndgameTable();
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

void Board::GeneratePieceSquareTables()
{
	//White Pawn
	std::array<std::array<int, WIDTH>, HEIGHT> pieceTable =
	{ {
		{  0,   0,   0,   0,   0,    0,   0,   0},
		{  5,  10,  10, -20, -20,   10,  10,   5},
		{  5,  -5, -10,   0,   0,  -10,  -5,   5},
		{  0,   0,   0,  20,  20,    0,   0,   0},
		{  5,   5,  10,  25,  25,   10,   5,   5},
		{ 10,  10,  20,  30,  30,   20,  10,  10},
		{ 50,  50,  50,  50,  50,   50,  50,  50},
		{  0,   0,   0,   0,   0,    0,   0,   0}
	} };
	pieceSquareTables[std::make_pair(PieceColor::WHITE, PieceType::PAWN)] = pieceTable;
	//Black Pawn
	pieceTable =
	{ {
		{  0,   0,   0,   0,   0,    0,   0,   0},
		{ 50,  50,  50,  50,  50,   50,  50,  50},
		{ 10,  10,  20,  30,  30,   20,  10,  10},
		{  5,   5,  10,  25,  25,   10,   5,   5},
		{  0,   0,   0,  20,  20,    0,   0,   0},
		{  5,  -5, -10,   0,   0,  -10,  -5,   5},
		{  5,  10,  10, -20, -20,   10,  10,   5},
		{  0,   0,   0,   0,   0,    0,   0,   0}
	} };
	pieceSquareTables[std::make_pair(PieceColor::BLACK, PieceType::PAWN)] = pieceTable;

	//White Knight
	pieceTable =
	{ {
		{-50, -40, -30, -30, -30,  -30, -40, -50},
		{-40, -20,   0,   5,   5,    0, -20, -40},
		{-30,   5,  10,  15,  15,   10,   5, -30},
		{-30,   0,  15,  20,  20,   15,   0, -30},
		{-30,   5,  15,  20,  20,   15,   5, -30},
		{-30,   0,  10,  15,  15,   10,   0, -30},
		{-40, -20,   0,   0,   0,    0, -20, -40},
		{-50, -40, -30, -30, -30,  -30, -40, -50},
	} };
	pieceSquareTables[std::make_pair(PieceColor::WHITE, PieceType::KNIGHT)] = pieceTable;
	//Black Knight
	pieceTable =
	{ {
		{-50, -40, -30, -30, -30,  -30, -40, -50},
		{-40, -20,   0,   0,   0,    0, -20, -40},
		{-30,   0,  10,  15,  15,   10,   0, -30},
		{-30,   5,  15,  20,  20,   15,   5, -30},
		{-30,   0,  15,  20,  20,   15,   0, -30},
		{-30,   5,  10,  15,  15,   10,   5, -30},
		{-40, -20,   0,   5,   5,    0, -20, -40},
		{-50, -40, -30, -30, -30,  -30, -40, -50}
	} };
	pieceSquareTables[std::make_pair(PieceColor::BLACK, PieceType::KNIGHT)] = pieceTable;

	//White Bishop
	pieceTable =
	{ {
		{-20, -10, -10, -10, -10, -10, -10, -20},
		{-10,   5,   0,   0,   0,   0,   5, -10},
		{-10,  10,  10,  10,  10,  10,  10, -10},
		{-10,   0,  10,  10,  10,  10,   0, -10},
		{-10,   5,   5,  10,  10,   5,   5, -10},
		{-10,   0,   5,  10,  10,   5,   0, -10},
		{-10,   0,   0,   0,   0,   0,   0, -10},
		{-20, -10, -10, -10, -10, -10, -10, -20}
	} };
	pieceSquareTables[std::make_pair(PieceColor::WHITE, PieceType::BISHOP)] = pieceTable;
	//Black Bishop
	pieceTable =
	{ {
		{-20, -10, -10, -10, -10, -10, -10, -20},
		{-10,   0,   0,   0,   0,   0,   0, -10},
		{-10,   0,   5,  10,  10,   5,   0, -10},
		{-10,   5,   5,  10,  10,   5,   5, -10},
		{-10,   0,  10,  10,  10,  10,   0, -10},
		{-10,  10,  10,  10,  10,  10,  10, -10},
		{-10,   5,   0,   0,   0,   0,   5, -10},
		{-20, -10, -10, -10, -10, -10, -10, -20}
	} };
	pieceSquareTables[std::make_pair(PieceColor::BLACK, PieceType::BISHOP)] = pieceTable;

	//White Rook
	pieceTable =
	{ {
		{  0,  0,  0,  5,  5,  0,  0,  0},
		{ -5,  0,  0,  0,  0,  0,  0, -5},
		{ -5,  0,  0,  0,  0,  0,  0, -5},
		{ -5,  0,  0,  0,  0,  0,  0, -5},
		{ -5,  0,  0,  0,  0,  0,  0, -5},
		{ -5,  0,  0,  0,  0,  0,  0, -5},
		{  5, 10, 10, 10, 10, 10, 10,  5},
		{  0,  0,  0,  0,  0,  0,  0,  0},
	} };
	pieceSquareTables[std::make_pair(PieceColor::WHITE, PieceType::ROOK)] = pieceTable;
	//Black Rook
	pieceTable =
	{ {
		{  0,  0,  0,  0,  0,  0,  0,  0},
		{  5, 10, 10, 10, 10, 10, 10,  5},
		{ -5,  0,  0,  0,  0,  0,  0, -5},
		{ -5,  0,  0,  0,  0,  0,  0, -5},
		{ -5,  0,  0,  0,  0,  0,  0, -5},
		{ -5,  0,  0,  0,  0,  0,  0, -5},
		{ -5,  0,  0,  0,  0,  0,  0, -5},
		{  0,  0,  0,  5,  5,  0,  0,  0}
	} };
	pieceSquareTables[std::make_pair(PieceColor::BLACK, PieceType::ROOK)] = pieceTable;

	//White Queen
	pieceTable =
	{ {
		{-20, -10, -10,  -5,  -5, -10, -10, -20},
		{-10,   0,   0,   0,   0,   5,   0, -10},
		{-10,   0,   5,   5,   5,   5,   5, -10},
		{ -5,   0,   5,   5,   5,   5,   0,   0},
		{ -5,   0,   5,   5,   5,   5,   0,  -5},
		{-10,   0,   5,   5,   5,   5,   0, -10},
		{-10,   0,   0,   0,   0,   0,   0, -10},
		{-20, -10, -10,  -5,  -5, -10, -10, -20},
	} };
	pieceSquareTables[std::make_pair(PieceColor::WHITE, PieceType::QUEEN)] = pieceTable;
	//Black Queen
	pieceTable =
	{ {
		{-20, -10, -10,  -5,  -5, -10, -10, -20},
		{-10,   0,   0,   0,   0,   0,   0, -10},
		{-10,   0,   5,   5,   5,   5,   0, -10},
		{ -5,   0,   5,   5,   5,   5,   0,  -5},
		{ -5,   0,   5,   5,   5,   5,   0,   0},
		{-10,   0,   5,   5,   5,   5,   5, -10},
		{-10,   0,   0,   0,   0,   5,   0, -10},
		{-20, -10, -10,  -5,  -5, -10, -10, -20}
	} };
	pieceSquareTables[std::make_pair(PieceColor::BLACK, PieceType::QUEEN)] = pieceTable;

	//White King Early game
	pieceTable =
	{ {
		{ 20,  20,  30,   0,  10,  30,  20,  20},
		{ 20,  20,   0,   0,   0,   0,  20,  20},
		{-10, -20, -20, -20, -20, -20, -20, -10},
		{-20, -30, -30, -40, -40, -30, -30, -20},
		{-30, -40, -40, -50, -50, -40, -40, -30},
		{-30, -40, -40, -50, -50, -40, -40, -30},
		{-30, -40, -40, -50, -50, -40, -40, -30},
		{-30, -40, -40, -50, -50, -40, -40, -30}
	} };
	pieceSquareTables[std::make_pair(PieceColor::WHITE, PieceType::KING)] = pieceTable;
	//Black King Early game
	pieceTable =
	{ {
		{-30, -40, -40, -50, -50, -40, -40, -30},
		{-30, -40, -40, -50, -50, -40, -40, -30},
		{-30, -40, -40, -50, -50, -40, -40, -30},
		{-30, -40, -40, -50, -50, -40, -40, -30},
		{-20, -30, -30, -40, -40, -30, -30, -20},
		{-10, -20, -20, -20, -20, -20, -20, -10},
		{ 20,  20,   0,   0,   0,   0,  20,  20},
		{ 20,  20,  30,   0,   0,  30,  20,  20}
	} };
	pieceSquareTables[std::make_pair(PieceColor::BLACK, PieceType::KING)] = pieceTable;
}

void Board::GenerateEndgameTable()
{
	//White King Endgame
	std::array<std::array<int, WIDTH>, HEIGHT> pieceTable =
	{ {
		{ -50, -30, -30, -30, -30, -30, -30, -50},
		{ -30, -30,   0,   0,   0,   0, -30, -30,},
		{ -30, -10,  20,  30,  30,  20, -10, -30,},
		{ -30, -10,  30,  40,  40,  30, -10, -30,},
		{ -30, -10,  30,  40,  40,  30, -10, -30,},
		{ -30, -10,  20,  30,  30,  20, -10, -30,},
		{ -30, -20, -10,   0,   0, -10, -20, -30,},
		{ -50, -40, -30, -20, -20, -30, -40, -50,}
	} };
	pieceSquareTables[std::make_pair(PieceColor::WHITE, PieceType::KING)] = pieceTable;
	//Black King Endgame
	pieceTable =
	{ {
		{ -50, -40, -30, -20, -20, -30, -40, -50,},
		{ -30, -20, -10,   0,   0, -10, -20, -30,},
		{ -30, -10,  20,  30,  30,  20, -10, -30,},
		{ -30, -10,  30,  40,  40,  30, -10, -30,},
		{ -30, -10,  30,  40,  40,  30, -10, -30,},
		{ -30, -10,  20,  30,  30,  20, -10, -30,},
		{ -30, -30,   0,   0,   0,   0, -30, -30,},
		{ -50, -30, -30, -30, -30, -30, -30, -50}
	} };
	pieceSquareTables[std::make_pair(PieceColor::BLACK, PieceType::KING)] = pieceTable;
}
