#include "GameTree.h"
#include "Chess/Gameplay.h"
//Utility static function to get the color of a turn
PieceColor GetTurnColor(int p_turn)
{
	return (p_turn % 2 == 0) ? PieceColor::BLACK : PieceColor::WHITE;
}
Node::~Node()
{
	//iterate children vector
	std::vector<Node*>::iterator vecIterator = children.begin();
	while(vecIterator != children.end())
	{
		Node* child = *vecIterator;
		//erase from vector at iterator position and delete
		children.erase(vecIterator);
		vecIterator = children.begin();
		delete child;
	}
}
//Creates Children for a particular node
void Node::GenerateChildren()
{
	//find which player's turn it will be, get their pieces
	int child_turn = turn + 1;
	PieceColor playerColor = GetTurnColor(child_turn);
	vecPieces livePieces = currentBoard->GetLivePieces(playerColor);
	for (auto pip : livePieces)
	{
		//Get valid moves of every live piece
		std::vector<std::shared_ptr<Move>> moves = Gameplay::getValidMoves(currentStatus, currentBoard, pip.piece, pip.row, pip.col);
		for (std::shared_ptr<Move> move : moves)
		{
			//Generate a Node for every possible move
			Board* nextBoard = new Board(*currentBoard);
			GameStatus* nextStatus = new GameStatus(*currentStatus);
			Gameplay::move(nextStatus, nextBoard, move);
			nextStatus->setPieceEnPassantable(playerColor, NULL);
			Node* nextChild = new Node();
			nextChild->turn = turn;
			nextChild->currentBoard = nextBoard;
			nextChild->currentStatus = nextStatus;
			nextChild->consequentMove = move;
			nextChild->heuristic = nextBoard->GetHeuristic();
			children.push_back(nextChild);
		}
	}
	generatedChildren = true;
}
GameTree::GameTree(Board* p_root, GameStatus* p_status)
{
	treeDepth = 0;
	root = new Node();
	root->turn = 1;
	root->currentBoard = new Board(*p_root);
	root->currentStatus = new GameStatus(*p_status);
	root->heuristic = p_root->GetHeuristic();
	root->consequentMove = nullptr;
}

GameTree::~GameTree()
{
	delete root;
}

void GameTree::GenerateTree()
{
	if (treeDepth == 0)
	{
		treeDepth++;
		root->GenerateChildren();
	}
	while (treeDepth <= MAX_DEPTH)
	{
			
	}
}





