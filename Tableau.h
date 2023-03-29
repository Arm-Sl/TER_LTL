#pragma once
#include<vector>
#include "LTLFormulaSet.h"
#include "Model.h"

class TableauState
{
public:
	TableauState(int state, LTLFormulaSet&& formulas, InterpretationFunction interpretation);
	~TableauState();

	void addChild(TableauState& child);
	const std::vector<TableauState*>& getChildren() const { return this->children; };

	bool operator==(const TableauState& other) const;

private:
	int state;
	LTLFormulaSet formulas;
	InterpretationFunction interpretation;

	std::vector<TableauState*> children;
	std::vector<TableauState*> parents;
};


class Tableau
{
public:
	Tableau(Model* model, std::unique_ptr<LTLFormula>&& formula);
	~Tableau();

private:
	Model* const model;
	std::unique_ptr<LTLFormula> formula;
};

