#ifndef PIECE_H
#define PIECE_H

#include "PieceType.h"
#include "PieceColor.h"
#include <memory>

class Piece
{
	private:
		PieceType type;
		PieceColor color;
	public:
		Piece(PieceType pType, PieceColor pColor);
		Piece(const Piece &piece);
		~Piece();
		PieceType getType();
		PieceColor getColor();
};
struct PieceInPosition
{
	std::shared_ptr<Piece> piece;
	int col;
	int row;
};
struct ValuedPiece
{
	ValuedPiece(std::shared_ptr<Piece> pPiece, int pCol, int pRow);
	std::shared_ptr<Piece> piece;
	int value;
	int col;
	int row;
};

#endif