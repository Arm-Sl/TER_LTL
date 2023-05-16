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

typedef PreTableauState TableauState;
class Tableau
{
public:
	Tableau() = default;
	Tableau(const Model* model, LTLFormula* formula, std::vector<std::unique_ptr<TableauState>>&& states, std::vector<TableauState*>&& rootStates);
	Tableau(Tableau&& tab) = default;
	Tableau& operator=(Tableau&& tab) = default;
	~Tableau();

	void stateElim1();
	void stateElim2();
	void tableauComputation();
	size_t getStateCount() { return this->states.size(); };
	operator std::string() const;

	bool isOpen() const;

private:
	std::vector<std::unique_ptr<TableauState>> states;
	std::vector<TableauState*> roots;
	const Model* model;
	LTLFormula* formula;

	bool isEventualityRealised(const LTLFormula* eventuality, const TableauState* fromState, std::vector<const TableauState*>& alreadyVisitedStates) const;

};

