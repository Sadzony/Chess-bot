#include "Piece.h"
#include <iostream>

Piece::Piece(PieceType pType, PieceColor pColor)
{
	type = pType;
	color = pColor;
}

Piece::Piece(const Piece &piece)
{
	this->type = piece.type;
	this->color = piece.color;
}

Piece::~Piece()
{
	//std::cout << "Piece Deleted" << std::endl; //THIS IS TOO SLOW
}

PieceType Piece::getType()
{
	return type;
}

PieceColor Piece::getColor()
{
	return color;
}

ValuedPiece::ValuedPiece(std::shared_ptr<Piece> pPiece, int pCol, int pRow)
{
	piece = pPiece; 
	value = (int)piece.get()->getType(); 
	col = pCol; 
	row = pRow;
}
