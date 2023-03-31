#include "Tableau.h"
#include "LTLFormula.h"
#include <iostream>


#pragma region TableauState

TableauState::TableauState(int state, LTLFormulaSet&& formulas, const InterpretationFunction& interpretation) :
	state(state),
	formulas(std::move(formulas)),
	interpretation(interpretation)
{
}

TableauState::TableauState(int state, LTLFormulaSet && formulas, InterpretationFunction && interpretation) :
state(state),
formulas(std::move(formulas)),
interpretation(std::move(interpretation))
{
}

TableauState::~TableauState()
{
}


void TableauState::addChild(TableauState& child)
{
	this->children.push_back(&child);
	child.parents.push_back(this);
}

bool TableauState::operator==(const TableauState & other) const
{
//	std::cout << "\t" << a << "  " << (this->state == other.state) << "  " << (this->formulas == other.formulas) << "  " << (this->interpretation == other.interpretation)  << std::endl;;
	return this->state == other.state && this->formulas == other.formulas && this->interpretation == other.interpretation;
}

int TableauState::lastID = 0;

#pragma endregion





#pragma region Tableau
Tableau::Tableau(Model* model, std::unique_ptr<LTLFormula>&& formula) :
	model(model),
	formula(std::move(formula))
{
}


Tableau::~Tableau()
{
}


void Tableau::createInitialState()
{
	LTLFormulaSet set;
	set.addFormula(this->formula->copy());
	const auto& ints = this->model->getInterpretationFunction().getInterpretations(0);
	std::unique_ptr<LTLFormula>* formula = set.addFormulas(ints.size());

	for (const auto& interp : ints)
	{
		formula++->reset(new VariableOp(interp.first, interp.second == Interpretation::FALSE));
	}


	this->preStates.push_back(std::make_unique<TableauState>(0, std::move(set), this->model->getInterpretationFunction()));
	this->rootState = this->preStates.back().get();
	this->rootState->setId();
	this->currentPreStates.push_back(this->rootState);
}


void Tableau::expRule()
{
	for (const auto& prestate : this->currentPreStates)
	{
		std::vector<LTLFormulaSet> fullExp = prestate->getFormulas().fullExpansion();

		for (LTLFormulaSet& formulaSet : fullExp)
		{
			//update interpretation function

			InterpretationFunction inter = prestate->getInterpretationFunction();

			for (VariableOp* var : formulaSet.getLitterals())
			{
				if (inter.get(prestate->getState(), var->getName()) == Interpretation::UNKNOWN)
					inter.set(prestate->getState(), var->getName(), var->isNeg() ? Interpretation::FALSE : Interpretation::TRUE);
			}

			TableauState newState(prestate->getState(), std::move(formulaSet), std::move(inter));


			//check if state already present
			bool stateAlreadyPresent = false;

			for (auto& state : this->states)
			{
				if (*state == newState)
				{
					prestate->addChild(*state);
					stateAlreadyPresent = true;
					break;
				}
			}


			//don't add offspring State if model state has no successors

			if (!stateAlreadyPresent && this->model->getSuccessors(prestate->getState()).size() > 0)
			{
				newState.setId();
				this->states.push_back(std::make_unique<TableauState>(std::move(newState)));

				TableauState* newStatePointer = this->states.back().get();

				prestate->addChild(*newStatePointer);
				this->currentStates.push_back(newStatePointer);
			}

		}
	}

	this->currentPreStates.clear();
}



void Tableau::nextRule()
{
	for (const auto& state : this->currentStates)
	{
		for (int modelState : this->model->getSuccessors(state->getState()))
		{
			//calculate scomp

			std::vector<std::unique_ptr<LTLFormula>> scomps;
			
			for (const auto& formula : state->getFormulas())
			{
				if (formula->getOperatorType() == OperatorType::SUCCESSOR)
					for (auto& comp : formula->getComponents())
						scomps.push_back(std::move(comp));
			}

			//create formula set
			LTLFormulaSet formulaSet;
			std::unique_ptr<LTLFormula>* currFormula = formulaSet.addFormulas(scomps.size() + state->getInterpretationFunction().getInterpretations(modelState).size());
			
			for (auto& comp : scomps)
				currFormula++->swap(comp);

			//add litterals
			for (const auto& litteral : state->getInterpretationFunction().getInterpretations(modelState))
			{
				currFormula++->reset(new VariableOp(litteral.first, litteral.second == Interpretation::FALSE));
			}

			TableauState newPreState(modelState, std::move(formulaSet), state->getInterpretationFunction());

			//check if prestate is already present
			bool preStateAlreadyPresent = false;

			for (auto& alreadyPresentPrestate : this->preStates)
			{
				if (*alreadyPresentPrestate == newPreState)
				{
					state->addChild(*alreadyPresentPrestate);
					preStateAlreadyPresent = true;
					break;
				}
			}

			if (!preStateAlreadyPresent)
			{
				newPreState.setId();
				this->preStates.push_back(std::make_unique<TableauState>(std::move(newPreState)));
				TableauState* newPreStatePointer = this->preStates.back().get();

				state->addChild(*newPreStatePointer);
				this->currentPreStates.push_back(newPreStatePointer);
			}

		}
	}

	this->currentStates.clear();
}


void Tableau::preTableauComputation()
{
	this->createInitialState();

	while (this->currentPreStates.size() > 0 || this->currentStates.size() > 0)
	{
		this->expRule();
		this->nextRule();
	}
}
#pragma endregion


