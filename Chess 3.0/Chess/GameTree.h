#pragma once
#define MAX_DEPTH 2
#include "Chess/Board.h"
#include "Chess/GameStatus.h"
#include "Chess/Move.h"
#include <memory>
#include <utility>

static PieceColor GetTurnColor(int p_turn);

struct Node
{
	~Node();
	void GenerateChildren();
	int turn;
	int heuristic;
	Board* currentBoard;
	GameStatus* currentStatus;
	std::shared_ptr<Move> consequentMove;
	std::vector<Node*> children;
	bool generatedChildren = false;
};

class GameTree
{
public:
	GameTree(Board* p_root, GameStatus* p_status);
	~GameTree();
	void GenerateTree();
	void UpdateTree(std::shared_ptr<Move> moveMade);
private:
	Node* root;
	int treeDepth;
};




