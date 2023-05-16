#include "Solver.h"

#include <iostream>

#include "PreTableau.h"
#include "LTLFormula.h"


Solver::Solver(Model* model, std::unique_ptr<LTLFormula>&& formula) :
	model(model),
	formula(std::move(formula))
{
}


Solver::~Solver()
{
}



bool Solver::solveEE(bool verbose)
{
	if (verbose)
	{
		std::cout << this->model->operator std::string();
		std::cout.flush();
	}

	Tableau tableau;
	this->computeTableau(&tableau, this->formula, verbose);

	return tableau.isOpen();
}

bool Solver::solveAA(bool verbose)
{
	if (verbose)
	{
		std::cout << this->model->operator std::string();
		std::cout.flush();
	}

	Tableau tableau;
	this->computeTableau(&tableau, this->formula->neg(), verbose);

	return !tableau.isOpen();
}



void Solver::solveEES()
{
}


void Solver::computeTableau(Tableau * tableau, const std::unique_ptr<LTLFormula>& formula, bool verbose) const
{
	PreTableau preTab(this->model, formula.get());
	preTab.preTableauComputation();

	if (verbose)
	{
		std::cout << "PreTableau Computation finished" << std::endl;
		std::cout << "State count : " << preTab.states.size() << ", Prestate count : " << preTab.preStates.size() << std::endl;
	}

	*tableau = std::move(preTab.convertToTableau());

	if (verbose)
		std::cout << "\nState count before Tableau state elimination : " << tableau->getStateCount() << std::endl;

	tableau->tableauComputation();

	if (verbose)
	{
		std::cout << "Tableau Computation finished\nFinal Tableau : \n" << tableau->operator std::string();
		std::cout.flush();
	}
}