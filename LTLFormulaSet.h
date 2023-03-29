#pragma once
#include<vector>
#include<memory>
#include<string>

class LTLFormula;

class LTLFormulaSet
{
public:
	LTLFormulaSet();
	~LTLFormulaSet();

	LTLFormulaSet(const LTLFormulaSet& fs);
	LTLFormulaSet(LTLFormulaSet&& fs);
	LTLFormulaSet(std::unique_ptr<LTLFormula>&& formula);
	
	LTLFormulaSet& operator=(LTLFormulaSet&& fs);

	std::vector<LTLFormulaSet> fullExpansion() const;
	bool isPatentlyInconsistent() const;

	operator std::string() const;
	bool operator==(const LTLFormulaSet& other) const;

private:

	std::vector<std::unique_ptr<LTLFormula>> formulas;
	int readIndex = 0;
};

