#pragma once
#include "Chess\PieceColor.h"
#include "Chess\PieceType.h"
#include <vector>
#include <memory>
#include "Chess/Move.h"

#define MAX_DEPTH 3

using namespace std;

class Piece;
class Board;
class GameStatus;
class Gameplay;
class Move;


typedef vector<PieceInPosition> vecPieces;

class ChessPlayer
{
public:
	static void		setupPlayers(ChessPlayer** playerWhite, ChessPlayer** playerBlack, Board* pBoard, GameStatus* pGameStatus, Gameplay* pGamePlay);
	ChessPlayer(Board* pBoard, GameStatus* pGameStatus, Gameplay* pGamePlay, PieceColor colour);

	void			setAI() { m_bAI = true; }
	bool			isAI() { return m_bAI; }
	unsigned int	getAllLivePieces(vecPieces& vpieces);
	vector<std::shared_ptr<Move>>	getValidMovesForPiece(PieceInPosition pip);
	bool			chooseAIMove(std::shared_ptr<Move>* moveToMake);
	int				minimax(Board* board, GameStatus* status, int depth);

protected:
	PieceColor		getColour() { return m_colour; }


private:
	PieceColor	m_colour;
	Board*		m_pBoard;
	GameStatus* m_pGameStatus;
	Gameplay*	m_pGamePlay;
	bool		m_bAI;
};

