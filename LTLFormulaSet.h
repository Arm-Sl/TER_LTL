#pragma once
#include<vector>
#include<memory>
#include<string>

class LTLFormula;
class VariableOp;

class LTLFormulaSet
{
public:
	LTLFormulaSet();
	~LTLFormulaSet();

	LTLFormulaSet(const LTLFormulaSet& fs);
	LTLFormulaSet(LTLFormulaSet&& fs);
	LTLFormulaSet(std::unique_ptr<LTLFormula>&& formula);
	
	LTLFormulaSet& operator=(LTLFormulaSet&& fs);

	void addFormula(std::unique_ptr<LTLFormula>&& formula);
	///<summary>Add null formulas and return a pointer to the first added element or null pointer if no elements added</summary>
	std::unique_ptr<LTLFormula>* addFormulas(size_t size);
	size_t size() { return this->formulas.size(); }

	std::vector<LTLFormulaSet> fullExpansion() const;
	bool isPatentlyInconsistent() const;

	std::vector<VariableOp*> getLitterals() const;

	operator std::string() const;
	bool operator==(const LTLFormulaSet& other) const;

	bool containsFormula(const LTLFormula* const ptr) const;

	std::vector<std::unique_ptr<LTLFormula>>::const_iterator begin() const;
	std::vector<std::unique_ptr<LTLFormula>>::const_iterator end() const;

	//std::vector<std::unique_ptr<LTLFormula>>::iterator begin();
	//std::vector<std::unique_ptr<LTLFormula>>::iterator end();

private:

	std::vector<std::unique_ptr<LTLFormula>> formulas;
	int readIndex = 0;
};

