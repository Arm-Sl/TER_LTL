#include "PreTableau.h"
#include "LTLFormula.h"
#include <iostream>


#pragma region PreTableauState

PreTableauState::PreTableauState(int state, LTLFormulaSet&& formulas, const InterpretationFunction& interpretation) :
	state(state),
	formulas(std::move(formulas)),
	interpretation(interpretation)
{
}

PreTableauState::PreTableauState(int state, LTLFormulaSet && formulas, InterpretationFunction && interpretation) :
state(state),
formulas(std::move(formulas)),
interpretation(std::move(interpretation))
{
}

PreTableauState::~PreTableauState()
{
}


void PreTableauState::addChild(PreTableauState& child)
{
	this->children.push_back(&child);
	child.parents.push_back(this);
}


void PreTableauState::dissociate(bool fromParents, bool fromChildren)
{
	if (fromParents)
	{
		for (auto parent : this->parents)
		{
			parent->children.erase(std::find(parent->children.begin(), parent->children.end(), this));
		}
		this->parents.clear();
	}
	if (fromChildren)
	{
		for (auto child : this->children)
		{
			child->parents.erase(std::find(child->parents.begin(), child->parents.end(), this));
		}
		this->children.clear();
	}
}



bool PreTableauState::operator==(const PreTableauState & other) const
{
//	std::cout << "\t" << a << "  " << (this->state == other.state) << "  " << (this->formulas == other.formulas) << "  " << (this->interpretation == other.interpretation)  << std::endl;;
	return this->state == other.state && this->formulas == other.formulas && this->interpretation == other.interpretation;
}

//bool PreTableauState::operator==(const TableauState & other) const
//{
//	return this->state == other.getState() && this->formulas == other.getFormulas() && this->interpretation == other.getInterpretationFunction();
//}

int PreTableauState::lastID = 0;

#pragma endregion





#pragma region PreTableau
PreTableau::PreTableau(Model* model, std::unique_ptr<LTLFormula>&& formula) :
	model(model),
	formula(std::move(formula))
{
}


PreTableau::~PreTableau()
{
}


void PreTableau::createInitialState()
{
	LTLFormulaSet set;
	set.addFormula(this->formula->copy());
	const auto& ints = this->model->getInterpretationFunction().getInterpretations(0);
	std::unique_ptr<LTLFormula>* formula = set.addFormulas(ints.size());

	for (const auto& interp : ints)
	{
		formula++->reset(new VariableOp(interp.first, interp.second == Interpretation::FALSE));
	}


	this->preStates.push_back(std::make_unique<PreTableauState>(0, std::move(set), this->model->getInterpretationFunction()));
	this->rootState = this->preStates.back().get();
	this->rootState->setId();
	this->currentPreStates.push_back(this->rootState);
}


void PreTableau::expRule()
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

			PreTableauState newState(prestate->getState(), std::move(formulaSet), std::move(inter));


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
				this->states.push_back(std::make_unique<PreTableauState>(std::move(newState)));

				PreTableauState* newStatePointer = this->states.back().get();

				prestate->addChild(*newStatePointer);
				this->currentStates.push_back(newStatePointer);
			}

		}
	}

	this->currentPreStates.clear();
}



void PreTableau::nextRule()
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

			PreTableauState newPreState(modelState, std::move(formulaSet), state->getInterpretationFunction());

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
				this->preStates.push_back(std::make_unique<PreTableauState>(std::move(newPreState)));
				PreTableauState* newPreStatePointer = this->preStates.back().get();

				state->addChild(*newPreStatePointer);
				this->currentPreStates.push_back(newPreStatePointer);
			}

		}
	}

	this->currentStates.clear();
}


void PreTableau::preTableauComputation()
{
	this->createInitialState();

	while (this->currentPreStates.size() > 0 || this->currentStates.size() > 0)
	{
		this->expRule();
		this->nextRule();
	}
}


Tableau PreTableau::convertToTableau()
{
	//prestateElim + StateElim1
	std::vector<std::unique_ptr<TableauState>> states;
	std::map<PreTableauState*, TableauState*> correspondingState;
	std::vector<TableauState*> roots;
	roots.reserve(this->rootState->getChildren().size());

	PreTableauState::resetNextIds();
	for (const std::unique_ptr<PreTableauState>& prestate : this->preStates)
	{
		for (PreTableauState* parent : prestate->getParents())
		{
			TableauState* currentparentTableau;
			if (auto it = correspondingState.find(parent); it == correspondingState.end())
			{
				currentparentTableau = states.emplace_back(new TableauState(parent->getState(), std::move(parent->getFormulas()), std::move(parent->getInterpretationFunction()))).get();
				currentparentTableau->setId();
				if (parent == this->rootState)
					roots.push_back(currentparentTableau);
				correspondingState.insert_or_assign(parent, currentparentTableau);
			}
			else
				currentparentTableau = it->second;

			for (PreTableauState* child : prestate->getChildren())
			{
				TableauState* currentChildTableau;
				if (auto it = correspondingState.find(child); it == correspondingState.end())
				{
					currentChildTableau = states.emplace_back(new TableauState(child->getState(), std::move(child->getFormulas()), std::move(child->getInterpretationFunction()))).get();
					currentChildTableau->setId();
					correspondingState.insert_or_assign(child, currentChildTableau);
				}
				else
					currentChildTableau = it->second;

				currentparentTableau->addChild(*currentChildTableau);
			}
		}
	}
		
	return Tableau(this->model, std::move(this->formula), std::move(states), std::move(roots));
}

#pragma endregion


