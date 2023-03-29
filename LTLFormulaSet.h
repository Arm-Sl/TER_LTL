#pragma once
#include<vector>
#include<memory>
#include<string>

class LTLFormula;

class LTLFormulaSet
{
public:
	LTLFormulaSet();
	LTLFormulaSet(const LTLFormulaSet& fs);
	LTLFormulaSet(LTLFormulaSet&& fs);
	LTLFormulaSet(std::unique_ptr<LTLFormula>&& formula);
	
	LTLFormulaSet& operator=(LTLFormulaSet&& fs);
	operator std::string() const;
	std::vector<LTLFormulaSet> fullExpansion() const;

	bool isPatentlyInconsistent() const;

	~LTLFormulaSet();


private:

	std::vector<std::unique_ptr<LTLFormula>> formulas;
	int readIndex = 0;
};

