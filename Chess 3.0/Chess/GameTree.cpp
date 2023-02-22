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
	PieceColor opposingPlayerColor = GetTurnColor(child_turn);
	PieceColor playerColor = GetTurnColor(turn);
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
			nextStatus->setPieceEnPassantable(opposingPlayerColor, NULL);
			Node* nextChild = new Node();
			nextChild->turn = child_turn;
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
	GenerateTree();
}

GameTree::~GameTree()
{
	delete root;
}

void GameTree::GenerateTree()
{
	std::vector<Node*> currentDepthNodes = std::vector<Node*>();
	currentDepthNodes.push_back(root);
	int currentDepth = 0;
	//Generate until max depth reached
	while (currentDepth < MAX_DEPTH)
	{
		//Generate children at current depth. Save them to nextDepth list
		std::vector<Node*> nextDepthNodes = std::vector<Node*>();
		for (Node* node : currentDepthNodes)
		{
			if (!node->generatedChildren)
				node->GenerateChildren();
			for (Node* child : node->children)
			{
				nextDepthNodes.push_back(child);
			}
		}
		//Clear the current vector and replace with nextDepth
		currentDepthNodes = nextDepthNodes;
		currentDepth++;
	}
}

void GameTree::UpdateTree(std::shared_ptr<Move> moveMade)
{
	//iterate children vector
	std::vector<Node*>::iterator vecIterator = root->children.begin();
	while (vecIterator != root->children.end())
	{
		Node* child = *vecIterator;
		//If the move made matches one of the children states, erase that child, and set it as the new root
		if (moveMade->operator==(*child->consequentMove))
		{
			root->children.erase(vecIterator);
			delete root;
			root = child;
			break;
		}
		vecIterator++;
	}
	GenerateTree();
}





