#pragma once
#include "Model.h"
#include "LTLFormulaSet.h"

class PreTableauState;
//class TableauState
//{
//public:
//	TableauState(int state, LTLFormulaSet&& formulas, const InterpretationFunction& interpretation);
//	TableauState(int state, LTLFormulaSet&& formulas, InterpretationFunction&& interpretation);
//	TableauState(TableauState&& state) = default;
//	~TableauState();
//
//	void addChild(TableauState& child) { this->children.push_back(&child); };
//
//	const std::vector<TableauState*>& getChildren() const { return this->children; };
//	const LTLFormulaSet& getFormulas() const { return this->formulas; };
//	const InterpretationFunction& getInterpretationFunction() const { return this->interpretation; };
//	int getState() const { return this->state; };
//
//	bool operator==(const TableauState& other) const;
//
//	int id;
//private:
//	static int lastID;
//	int state;
//	LTLFormulaSet formulas;
//	InterpretationFunction interpretation;
//
//	std::vector<TableauState*> children;
//};

class Tableau
{
public:
	Tableau(Model* model, std::unique_ptr<LTLFormula>&& formula, std::vector<std::unique_ptr<PreTableauState>>&& states);
	Tableau(Tableau&& tab) = default;
	~Tableau();

	void stateElim1();
	void stateElim2();
	void tableauComputation();

	//TODO move to private, for tests
	std::vector<std::unique_ptr<PreTableauState>> states;
	void print();
private:
	Model* model;
	std::unique_ptr<LTLFormula> formula;

	bool isEventualityRealised(const LTLFormula* eventuality, const PreTableauState* fromState, std::vector<const PreTableauState*>& alreadyVisitedStates) const;

};

