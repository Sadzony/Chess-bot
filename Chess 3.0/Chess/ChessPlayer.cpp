#include "ChessPlayer.h"
#include "Chess\GameStatus.h"
#include "Chess\Gameplay.h"
#include "Chess\Board.h"
#include "Chess\Piece.h"
#include "Chess/Game.h"

using namespace std;



void ChessPlayer::setupPlayers(ChessPlayer** playerWhite, ChessPlayer** playerBlack, Board* pBoard, GameStatus* pGameStatus, Gameplay* pGamePlay, Game* p_game)
{
	*playerBlack = new ChessPlayer(pBoard, pGameStatus, pGamePlay, PieceColor::BLACK, p_game);
	(*playerBlack)->setAI();

	*playerWhite = new ChessPlayer(pBoard, pGameStatus, pGamePlay, PieceColor::WHITE, p_game);
	(*playerWhite)->setAI();

}

ChessPlayer::ChessPlayer(Board* pBoard, GameStatus* pGameStatus, Gameplay* pGamePlay, PieceColor colour, Game* p_game)
{
	m_colour = colour;
	m_pBoard = pBoard;
	m_pGameStatus = pGameStatus;
	m_pGamePlay = pGamePlay;
	m_bAI = false;
	m_chess = p_game;
}

unsigned int ChessPlayer::getAllLivePieces(vecPieces& vpieces, Board* board)
{
	vpieces.clear();

	PieceInPosition pip;

	unsigned int count = 0;
	for (int i = board->MIN_ROW_INDEX; i < board->MAX_ROW_INDEX; i++)
	{
		for (int j = board->MIN_COL_INDEX; j < board->MAX_COL_INDEX; j++)
		{
			std::shared_ptr<Piece> pPiece = board->getSquare(i, j)->getOccupyingPiece();

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

vector<std::shared_ptr<Move>> ChessPlayer::getValidMovesForPiece(PieceInPosition pip, Board* board, GameStatus* status)
{
	return Gameplay::getValidMoves(status, board, pip.piece, pip.row, pip.col);
}

// chooseAIMove
// in this method - for an AI chess player - choose a move to make. This is called once per play. 
bool ChessPlayer::chooseAIMove(std::shared_ptr<Move>& moveToMake)
{
	//Get all pieces of the player
	vecPieces vPieces;
	unsigned int pieceCount = getAllLivePieces(vPieces, m_pBoard);

	//best move value is set to the minimal value for each player
	int bestMoveValue;
	if (m_colour == PieceColor::BLACK)
		bestMoveValue = std::numeric_limits<int>::max();
	else
		bestMoveValue = std::numeric_limits<int>::min();
	//Run minimax on all the valid moves for all pieces of this player. The first pass of the function is done here, as the move needs to be output
	int alpha; alpha = std::numeric_limits<int>::min();
	int beta; beta = std::numeric_limits<int>::max();
	for (PieceInPosition pip : vPieces)
	{
		vector<shared_ptr<Move>> pieceMoves = Gameplay::getValidMoves(m_pGameStatus, m_pBoard, pip.piece, pip.row, pip.col);
		for (shared_ptr<Move> move : pieceMoves)
		{
			//Generate a board state from this move
			Board* nextBoard;
			GameStatus* nextStatus;
			//The minimax function starts from next turn, so the opposite color is needed.
			PieceColor opposingColor = (m_colour == PieceColor::WHITE) ? PieceColor::BLACK : PieceColor::WHITE;
			GenerateNextTurn(opposingColor, nextBoard, nextStatus, m_pBoard, m_pGameStatus, move);

			//Find the value of this move by finding the future possibilities using minimax
			int moveValue = minimax(nextBoard, nextStatus, 1, opposingColor, alpha, beta);
			
			delete nextBoard;
			delete nextStatus;

			//If the move is better than current best, set it as best move.
			if (m_colour == PieceColor::BLACK)
			{
				if (moveValue < bestMoveValue)
				{
					bestMoveValue = moveValue;
					moveToMake = move;
				}
				//Find the value of beta, which is the best value that the black player can take
				beta = min(beta, bestMoveValue);
				//If beta (best of black) is smaller than alpha (best of white) the loop can exit early.
				if (beta <= alpha)
					break;
			}
			else if (m_colour == PieceColor::WHITE)
			{
				if (moveValue > bestMoveValue)
				{
					bestMoveValue = moveValue;
					moveToMake = move;
				}
				//Find the value of alpha, which is the best value that the white player can take
				alpha = max(alpha, bestMoveValue);
				//If alpha (best of white) is bigger than beta (best of black), the loop can be exit early
				if (alpha >= beta)
					break;
					
			}
		}
	}
	if (moveToMake != nullptr)
	{
		return true;
	}


	return false; // if there are no moves to make return false
}

int ChessPlayer::minimax(Board* board, GameStatus* status, int depth, PieceColor currentPlayerColor, int alpha, int beta)
{
	bool isCheckMate = Gameplay::isCheckMateState(status, board, currentPlayerColor);
	//Return heuristic at end of tree or on check mate
	if (depth >= MAX_DEPTH || isCheckMate)
	{
		//Evaluate the heuristic of the board
		int heuristic = board->GetHeuristic(status);
		//CheckMateState checks for valid moves. To find if there's a winner, find check state too.
		bool check = Gameplay::isCheckState(status, board, currentPlayerColor);
		if (isCheckMate && check && currentPlayerColor == PieceColor::WHITE)
		{
			heuristic -= (int)PieceType::KING;
		}
		else if (isCheckMate && check && currentPlayerColor == PieceColor::BLACK)
		{
			heuristic += (int)PieceType::KING;
		}

		else if (isCheckMate && !check)
			//Stalemate
			return 0;
		return heuristic;
	}
	//Get all pieces of the player
	vecPieces vPieces;
	if (currentPlayerColor == PieceColor::WHITE)
		m_chess->getWhitePlayer()->getAllLivePieces(vPieces, board);
	else
		m_chess->getBlackPlayer()->getAllLivePieces(vPieces, board);

	//Get all possible moves
	std::vector<shared_ptr<Move>> possibleMoves = std::vector<shared_ptr<Move>>();
	for (PieceInPosition pip : vPieces)
	{
		vector<shared_ptr<Move>> pieceMoves = getValidMovesForPiece(pip, board, status);
		for (shared_ptr<Move> move : pieceMoves)
			possibleMoves.push_back(move);
	}
	//If there are no possible moves and check mate wasn't reached, then it is a tie and heuristic is 0.
	if (possibleMoves.size() == 0)
		return 0;
	
	PieceColor opposingColor = (currentPlayerColor == PieceColor::WHITE) ? PieceColor::BLACK : PieceColor::WHITE;
	//Maximizer
	if (currentPlayerColor == PieceColor::WHITE) 
	{
		//Evaluate possible moves, set the bestMoveValue to -INFINITY
		int bestMoveValue; bestMoveValue = std::numeric_limits<int>::min();
		for (shared_ptr<Move> move : possibleMoves)
		{
			//Generate next board state
			Board* nextBoard;
			GameStatus* nextStatus;
			GenerateNextTurn(opposingColor, nextBoard, nextStatus, board, status, move);
			//Run minimax on this board state
			bestMoveValue = max(bestMoveValue, minimax(nextBoard, nextStatus, depth + 1, opposingColor, alpha, beta));
			delete nextBoard;
			delete nextStatus;
			//Find the value of alpha, which is the best value that the white player can take
			alpha = max(alpha, bestMoveValue);
			if (alpha >= beta)
				break;
		}
		return bestMoveValue;

	}
	//Minimizer
	else 
	{
		//Evaluate possible moves, set the bestMoveValue to INFINITY
		int bestMoveValue; bestMoveValue = std::numeric_limits<int>::max();
		for (shared_ptr<Move> move : possibleMoves)
		{
			//Generate next board state
			Board* nextBoard;
			GameStatus* nextStatus;
			GenerateNextTurn(opposingColor, nextBoard, nextStatus, board, status, move);
			//Run minimax on this board state
			bestMoveValue = min(bestMoveValue, minimax(nextBoard, nextStatus, depth + 1, opposingColor, alpha, beta));
			delete nextBoard;
			delete nextStatus;
			//Find the value of beta, which is the best value that the black player can take
			beta = min(beta, bestMoveValue);
			//If beta (best of black player) is smaller than alpha, the loop can be exit early
			if (beta <= alpha)
				break;
		}
		return bestMoveValue;
	}
}

void ChessPlayer::GenerateNextTurn(PieceColor opposingPlayerColor, Board*& outBoard, GameStatus*& outStatus, Board* currentBoard, GameStatus* currentStatus, shared_ptr<Move> moveToMake)
{
	outBoard = currentBoard->hardCopy();
	outStatus = new GameStatus(*currentStatus);
	Gameplay::move(outStatus, outBoard, moveToMake);
	//Reset EnPassantable status, like at the end of turn.
	outStatus->setPieceEnPassantable(opposingPlayerColor, NULL);
}
