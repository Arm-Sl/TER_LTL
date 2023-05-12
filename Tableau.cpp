#include "Tableau.h"

#include <algorithm>
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

Tableau::Tableau(Model* model, std::unique_ptr<LTLFormula>&& formula, std::vector<std::unique_ptr<TableauState>>&& states, std::vector<TableauState*>&& rootStates):
	model(model),
	formula(std::move(formula)),
	states(std::move(states)),
	roots(rootStates)
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
				std::vector<const TableauState*> alreadyVisitedStates;
				if (!this->isEventualityRealised(f, state.get(), alreadyVisitedStates))
				{
					state->dissociate(true, true);
					break;
				}
			}
		}
	}
}

bool Tableau::isEventualityRealised(const LTLFormula * eventuality, const TableauState * fromState, std::vector<const TableauState*>& alreadyVisitedStates) const
{
	alreadyVisitedStates.push_back(fromState);

	if (fromState->getFormulas().containsFormula(eventuality))
		return true;

	for (const TableauState* child : fromState->getChildren())
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

		this->states.erase(std::remove_if(this->states.begin(), this->states.end(), [](const std::unique_ptr<TableauState>& x) 
		{
			return x->isDissociated();
		}), this->states.end());
	}
}



Tableau::operator std::string() const
{
	std::string str;

	str += "\n\nTableau : \n" + std::to_string(this->states.size()) + " States : \n";

	for (const auto& state : this->states)
	{
		str += "\t" + std::to_string(state->id) + " : ";

		for (const auto& formula : state->getFormulas())
		{
			str += formula->operator std::string() + ", ";
		}
		str += "\n\t\t" + std::to_string(state->getChildren().size()) + " Children : ";

		for (const auto& child : state->getChildren())
		{
			str += std::to_string(child->id) + ", ";
		}
		str += "\n";

	}

	return str;
}
#pragma endregion

