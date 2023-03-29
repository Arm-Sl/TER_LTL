#include "Tableau.h"
#include "LTLFormula.h"


#pragma region TableauState

TableauState::TableauState(int state, LTLFormulaSet&& formulas, InterpretationFunction interpretation) :
	state(state),
	formulas(std::move(formulas)),
	interpretation(interpretation)
{
}

TableauState::~TableauState()
{
}


void TableauState::addChild(TableauState & child)
{
	this->children.push_back(&child);
	child.parents.push_back(this);
}

bool TableauState::operator==(const TableauState & other) const
{
	return this->state == other.state && this->formulas == other.formulas && this->interpretation == other.interpretation;
}

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
#pragma endregion


