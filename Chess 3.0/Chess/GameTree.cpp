#include "GameTree.h"
#include "Chess/Gameplay.h"

GameTree::GameTree(Board* p_root, GameStatus* p_status)
{
	root = Node();
	root.turn = 1;
	root.currentBoard = new Board(*p_root);
	root.currentStatus = new GameStatus(*p_status);
	root.heuristic = p_root->GetHeuristic();
	root.consequentMove = nullptr;
	FindPossibleOutcomes(root, root.possibleOutcomes);
}

void GameTree::GenerateNode(Node parent)
{
	int turn = parent.turn + 1;
	PieceColor playerColor = GetTurnColor(turn);

}

void GameTree::FindPossibleOutcomes(Node target, std::vector<Outcome>& output)
{
	output.clear();
	PieceColor playerColor = GetTurnColor(target.turn);
	vecPieces livePieces = target.currentBoard->GetLivePieces(playerColor);
	
	for (auto pip : livePieces)
	{
		std::vector<std::shared_ptr<Move>> moves = Gameplay::getValidMoves(target.currentStatus, target.currentBoard, pip.piece, pip.row, pip.col);
		for (std::shared_ptr<Move> move : moves)
		{
			Board* nextBoard = new Board(*target.currentBoard);
			GameStatus* nextStatus = new GameStatus(*target.currentStatus);
			Gameplay::move(nextStatus, nextBoard, move);
			nextStatus->setPieceEnPassantable(playerColor, NULL);
			output.push_back(std::make_pair(nextBoard, nextStatus));
		}
	}
	return;
}

PieceColor GameTree::GetTurnColor(int p_turn)
{
	return (p_turn % 2 == 0) ? PieceColor::BLACK : PieceColor::WHITE;
}
