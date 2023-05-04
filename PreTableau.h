#pragma once
#include<vector>
#include "LTLFormulaSet.h"
#include "Model.h"
#include "Tableau.h"

class PreTableauState
{
public:
	PreTableauState(int state, LTLFormulaSet&& formulas, const InterpretationFunction& interpretation);
	PreTableauState(int state, LTLFormulaSet&& formulas, InterpretationFunction&& interpretation);
	PreTableauState(PreTableauState&& state) = default;
	~PreTableauState();

	void addChild(PreTableauState& child);
	void dissociate(bool fromParents = false, bool fromChildren = false);
	bool isDissociated() const { return this->parents.size() == 0 && this->children.size() == 0; };

	const std::vector<PreTableauState*>& getChildren() const { return this->children; };
	const std::vector<PreTableauState*>& getParents() const { return this->parents; };
	const LTLFormulaSet& getFormulas() const { return this->formulas; };
	const InterpretationFunction& getInterpretationFunction() const { return this->interpretation; };
	int getState() const { return this->state; };


	LTLFormulaSet& getFormulas() { return this->formulas; };
	InterpretationFunction& getInterpretationFunction() { return this->interpretation; };

	bool operator==(const PreTableauState& other) const;
	//bool operator==(const TableauState& other) const;

	void setId() { this->id = PreTableauState::lastID++; };
	int id;
	static int lastID;
	static void resetNextIds() { PreTableauState::lastID = 0; };
private:
	int state;
	LTLFormulaSet formulas;
	InterpretationFunction interpretation;

	std::vector<PreTableauState*> children;
	std::vector<PreTableauState*> parents;
};


class PreTableau
{
public:
	PreTableau(Model* model, std::unique_ptr<LTLFormula>&& formula);
	~PreTableau();


	void createInitialState();

	void expRule();
	void nextRule();

	void preTableauComputation();

	Tableau convertToTableau();

	//TODO move to private, for tests
	std::vector<std::unique_ptr<PreTableauState>> states;
	std::vector<std::unique_ptr<PreTableauState>> preStates;

private:
	Model* const model;
	std::unique_ptr<LTLFormula> formula;



	std::vector<PreTableauState*> currentStates;
	std::vector<PreTableauState*> currentPreStates;

	PreTableauState* rootState;


};

