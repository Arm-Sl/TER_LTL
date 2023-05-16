#pragma once
#include "Model.h"
#include "Tableau.h"

class Solver
{
public:
	Solver(Model* model, std::unique_ptr<LTLFormula>&& formula);
	~Solver();

	bool solveEE(bool verbose = false);

	bool solveAA(bool verbose = false);

	void solveEES();

private:

	void computeTableau(Tableau* tableau, const std::unique_ptr<LTLFormula>& formula, bool verbose = false) const;

	const Model* const model;
	std::unique_ptr<LTLFormula> formula;
};

