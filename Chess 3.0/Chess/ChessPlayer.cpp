#include "ChessPlayer.h"
#include "Chess\GameStatus.h"
#include "Chess\Gameplay.h"
#include "Chess\Board.h"
#include "Chess\Piece.h"

using namespace std;



void ChessPlayer::setupPlayers(ChessPlayer** playerWhite, ChessPlayer** playerBlack, Board* pBoard, GameStatus* pGameStatus, Gameplay* pGamePlay)
{
	*playerBlack = new ChessPlayer(pBoard, pGameStatus, pGamePlay, PieceColor::BLACK);
	//(*playerBlack)->setAI();

	*playerWhite = new ChessPlayer(pBoard, pGameStatus, pGamePlay, PieceColor::WHITE);
	(*playerWhite)->setAI();

}

ChessPlayer::ChessPlayer(Board* pBoard, GameStatus* pGameStatus, Gameplay* pGamePlay, PieceColor colour)
{
	m_colour = colour;
	m_pBoard = pBoard;
	m_pGameStatus = pGameStatus;
	m_pGamePlay = pGamePlay;
	m_bAI = false;
}

unsigned int ChessPlayer::getAllLivePieces(vecPieces& vpieces)
{
	vpieces.clear();

	PieceInPosition pip;

	unsigned int count = 0;
	for (int i = m_pBoard->MIN_ROW_INDEX; i < m_pBoard->MAX_ROW_INDEX; i++)
	{
		for (int j = m_pBoard->MIN_COL_INDEX; j < m_pBoard->MAX_COL_INDEX; j++)
		{
			std::shared_ptr<Piece> pPiece = m_pBoard->getSquare(i, j)->getOccupyingPiece();

			if (pPiece == nullptr)
				continue;
			if (pPiece->getColor() != m_colour)
				continue;

			count++;
			pip.piece = pPiece;
			pip.row = i;
			pip.col = j;
			vpieces.emplace_back(pip);
		}
	}

	return count;
}

vector<std::shared_ptr<Move>> ChessPlayer::getValidMovesForPiece(PieceInPosition pip)
{
	return Gameplay::getValidMoves(m_pGameStatus, m_pBoard, pip.piece, pip.row, pip.col);
}

// chooseAIMove
// in this method - for an AI chess player - choose a move to make. This is called once per play. 
bool ChessPlayer::chooseAIMove(std::shared_ptr<Move>* moveToMake)
{
	//Get all pieces of the player
	vecPieces vPieces;
	unsigned int pieceCount = getAllLivePieces(vPieces);

	//Find the possible moves of every piece and try to find the best move
	shared_ptr<Move> bestMove = nullptr;
	int bestMoveValue;
	if (m_colour == PieceColor::BLACK)
		bestMoveValue = std::numeric_limits<int>::max();
	else
		bestMoveValue = std::numeric_limits<int>::min();

	for (PieceInPosition pip : vPieces)
	{
		vector<shared_ptr<Move>> pieceMoves = getValidMovesForPiece(pip); // get all the valid moves for this piece
		for (shared_ptr<Move> move : pieceMoves)
		{
			//Generate a board state from this move


			//If the move is better than current best, set it as best move.
			int moveValue = minimax(m_pBoard, m_pGameStatus, 0);
			if (m_colour == PieceColor::BLACK && moveValue < bestMoveValue)
			{
				bestMoveValue = moveValue;
				bestMove = move;
			}
			else if (m_colour == PieceColor::WHITE && moveValue > bestMoveValue)
			{
				bestMoveValue = moveValue;
				bestMove = move;
			}
		}
	}

	return false; // if there are no moves to make return false
}

int ChessPlayer::minimax(Board* board, GameStatus* status, int depth)
{
	return 0;
}
