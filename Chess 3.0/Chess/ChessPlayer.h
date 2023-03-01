#pragma once
#include "Chess\PieceColor.h"
#include "Chess\PieceType.h"
#include <vector>
#include <memory>
#include "Chess/Move.h"

#define MAX_DEPTH 2

using namespace std;

class Piece;
class Board;
class GameStatus;
class Gameplay;
class Move;
class Game;


typedef vector<PieceInPosition> vecPieces;

class ChessPlayer
{
public:
	static void		setupPlayers(ChessPlayer** playerWhite, ChessPlayer** playerBlack, Board* pBoard, GameStatus* pGameStatus, Gameplay* pGamePlay, Game* p_game);
	ChessPlayer(Board* pBoard, GameStatus* pGameStatus, Gameplay* pGamePlay, PieceColor colour, Game* p_game);

	void			setAI() { m_bAI = true; }
	bool			isAI() { return m_bAI; }
	unsigned int	getAllLivePieces(vecPieces& vpieces, Board* board);
	vector<std::shared_ptr<Move>>	getValidMovesForPiece(PieceInPosition pip, Board* board, GameStatus* status);
	bool			chooseAIMove(std::shared_ptr<Move>& moveToMake);

protected:
	PieceColor		getColour() { return m_colour; }
	int				minimax(Board* board, GameStatus* status, int depth, PieceColor currentPlayerColor, int alpha, int beta);
	void GenerateNextTurn(PieceColor currentPlayerColor, Board*& outBoard, GameStatus*& outStatus, Board* currentBoard, GameStatus* currentStatus, shared_ptr<Move> moveToMake);


private:
	PieceColor	m_colour;
	Board*		m_pBoard;
	GameStatus* m_pGameStatus;
	Gameplay*	m_pGamePlay;
	Game*		m_chess;
	bool		m_bAI;
};

