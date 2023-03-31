#pragma once
#include<vector>
#include "LTLFormulaSet.h"
#include "Model.h"

class TableauState
{
public:
	TableauState(int state, LTLFormulaSet&& formulas, const InterpretationFunction& interpretation);
	TableauState(int state, LTLFormulaSet&& formulas, InterpretationFunction&& interpretation);
	TableauState(TableauState&& state) = default;
	~TableauState();

	void addChild(TableauState& child);

	const std::vector<TableauState*>& getChildren() const { return this->children; };
	const LTLFormulaSet& getFormulas() const { return this->formulas; };
	const InterpretationFunction& getInterpretationFunction() const { return this->interpretation; };
	int getState() const { return this->state; };

	bool operator==(const TableauState& other) const;

	void setId() { this->id = TableauState::lastID++; };
	int id;
	static int lastID;
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


	void createInitialState();

	void expRule();
	void nextRule();

	void preTableauComputation();

	//TODO move to private, for tests
	std::vector<std::unique_ptr<TableauState>> states;
	std::vector<std::unique_ptr<TableauState>> preStates;

private:
	Model* const model;
	std::unique_ptr<LTLFormula> formula;



	std::vector<TableauState*> currentStates;
	std::vector<TableauState*> currentPreStates;

	TableauState* rootState;


};

