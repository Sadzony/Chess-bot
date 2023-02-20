#pragma once
#define LOOKAHEAD_MAX 5;
#include "Chess/Board.h"
#include "Chess/GameStatus.h"
#include "Chess/Move.h"
#include <memory>
#include <utility>

typedef std::pair<Board*, GameStatus*> Outcome;

struct Node
{
	int turn;
	Board* currentBoard;
	GameStatus* currentStatus;
	int heuristic;
	std::shared_ptr<Move> consequentMove;
	std::vector<Outcome> possibleOutcomes;
	void CleanUp();
};

class GameTree
{
public:
	GameTree(Board* p_root, GameStatus* p_status);
	void UpdateTree(std::shared_ptr<Move> moveMade);
private:
	void GenerateNode(Node parent);
	void FindPossibleOutcomes(Node target, std::vector<Outcome>& output);
	PieceColor GetTurnColor(int p_turn);
	Node root;
};


