#include "Tableau.h"

#include <algorithm>
//TODO remove, for tests
#include <iostream>
#include "LTLFormula.h"
#include "PreTableau.h"




//
//#pragma region TableauState
//
//TableauState::TableauState(int state, LTLFormulaSet&& formulas, const InterpretationFunction& interpretation) :
//	state(state),
//	formulas(std::move(formulas)),
//	interpretation(interpretation)
//{
//	this->id = TableauState::lastID++;
//}
//
//TableauState::TableauState(int state, LTLFormulaSet&& formulas, InterpretationFunction&& interpretation) :
//	state(state),
//	formulas(std::move(formulas)),
//	interpretation(std::move(interpretation))
//{
//	this->id = TableauState::lastID++;
//}
//
//
//TableauState::~TableauState()
//{
//}
//
//
//bool TableauState::operator==(const TableauState & other) const
//{
//	return this->state == other.state && this->formulas == other.formulas && this->interpretation == other.interpretation;
//}
//
//int TableauState::lastID = 0;
//
//#pragma endregion
//


#pragma region Tableau

Tableau::Tableau(Model* model, std::unique_ptr<LTLFormula>&& formula, std::vector<std::unique_ptr<PreTableauState>>&& states):
	model(model),
	formula(std::move(formula)),
	states(std::move(states))
{
}


Tableau::~Tableau()
{
}


void Tableau::stateElim1()
{
	//suppr all states without any sucessors

	for (const auto& state : this->states)
	{
		if (!state->isDissociated() && state->getChildren().size() == 0)
			state->dissociate(true, false);
	}


}

void Tableau::stateElim2()
{
	for (const auto& state : this->states)
	{
		if (state->isDissociated())
			continue;


		for (const auto& formula : state->getFormulas())
		{
			LTLFormula* f = formula->getEventuality();

			if (f != nullptr)
			{
				std::vector<const PreTableauState*> alreadyVisitedStates;
				if (!this->isEventualityRealised(f, state.get(), alreadyVisitedStates))
				{
					state->dissociate(true, true);
					break;
				}
			}
		}
	}
}

bool Tableau::isEventualityRealised(const LTLFormula * eventuality, const PreTableauState * fromState, std::vector<const PreTableauState*>& alreadyVisitedStates) const
{
	alreadyVisitedStates.push_back(fromState);

	if (fromState->getFormulas().containsFormula(eventuality))
		return true;

	for (const PreTableauState* child : fromState->getChildren())
	{
		if (std::find(alreadyVisitedStates.begin(), alreadyVisitedStates.end(), child) == alreadyVisitedStates.end())
		{
			if (isEventualityRealised(eventuality, child, alreadyVisitedStates))
				return true;
		}
	}

	return false;
}

void Tableau::tableauComputation()
{
	size_t oldStateCount = 0;
	size_t oldOldStateCount = 0;

	while (oldStateCount != this->states.size() || oldOldStateCount != this->states.size())
	{
		oldOldStateCount = oldStateCount;
		oldStateCount = this->states.size();
		this->stateElim1();
		this->stateElim2();

		this->states.erase(std::remove_if(this->states.begin(), this->states.end(), [](const std::unique_ptr<PreTableauState>& x) 
		{
			return x->isDissociated();
		}), this->states.end());
	}
}


void Tableau::print()
{
	std::cout << std::endl << std::endl << "Tableau : " << std::endl;
	std::cout << this->states.size() << " States : " << std::endl;

	for (const auto& state : this->states)
	{
		std::cout << "\t" << state->id << " : ";
		
		for (const auto& formula : state->getFormulas())
		{
			std::cout << formula->operator std::string() << ", ";
		}
		std::cout << std::endl;
		std::cout << "\t\t" << state->getChildren().size() << " Children : ";

		for (const auto& child : state->getChildren())
		{
			std::cout << child->id << ", ";
		}
		std::cout << std::endl;

	}

	
}
#pragma endregion

