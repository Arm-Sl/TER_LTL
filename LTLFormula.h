#pragma once
#include<vector>
#include<set>
#include<memory>
#include <sstream>

enum class OperatorType
{
	CONJONCTIVE,
	DISJONCTIVE,
	SUCCESSOR,
	VARIABLE,
};


class LTLFormula
{
public:
	LTLFormula();
	~LTLFormula();

	virtual OperatorType getOperatorType() const = 0;
	virtual std::vector<std::unique_ptr<LTLFormula>> getComponents() const = 0;
	///<summary> <returns> Returns a <see cref="LTLFormula"/> that must be true in the future, or nullptr if not needed</returns> </summary>
	virtual LTLFormula* getEventuality() const = 0;

	///<summary> Return a copy of this <see cref="LTLFormula"/>, negated</summary>
	virtual std::unique_ptr<LTLFormula> neg() const = 0;
	virtual std::unique_ptr<LTLFormula> copy() const = 0;

	virtual operator std::string() const = 0;
	virtual bool operator==(const LTLFormula& formula) const = 0;

	///<summary> return a new LTL represented by the string s, and get all variable names</summary>
	static std::unique_ptr<LTLFormula> readFormule(std::string s);
	


};




class NextOp : public LTLFormula
{
public:
	static const char SYMBOL = 'N';

	NextOp(LTLFormula* operand);
	NextOp(std::unique_ptr<LTLFormula>&& operand);
	NextOp(NextOp&& op);
	~NextOp();

	OperatorType getOperatorType() const override { return OperatorType::SUCCESSOR; }
	std::vector<std::unique_ptr<LTLFormula>> getComponents() const override;
	std::unique_ptr<LTLFormula> neg() const override;
	LTLFormula* getEventuality() const override { return nullptr; };

	std::unique_ptr<LTLFormula> copy() const override;
	
	operator std::string() const override;
	bool operator==(const LTLFormula& formula) const;

private:
	std::unique_ptr<LTLFormula> operand;
};


class GloballyOp : public LTLFormula
{
public:
	static const char SYMBOL = 'G';

	GloballyOp(LTLFormula* operand);
	GloballyOp(std::unique_ptr<LTLFormula>&& operand);
	~GloballyOp();

	OperatorType getOperatorType() const override { return OperatorType::CONJONCTIVE; }
	std::vector<std::unique_ptr<LTLFormula>> getComponents() const override;
	std::unique_ptr<LTLFormula> neg() const override;
	LTLFormula* getEventuality() const override { return nullptr; };

	std::unique_ptr<LTLFormula> copy() const override;

	operator std::string() const override;
	bool operator==(const LTLFormula& formula) const;

private:
	std::unique_ptr<LTLFormula> operand;
};


class FinallyOp : public LTLFormula
{
public:
	static const char SYMBOL = 'F';

	FinallyOp(LTLFormula* operand);
	FinallyOp(std::unique_ptr<LTLFormula>&& operand);
	~FinallyOp();

	OperatorType getOperatorType() const override { return OperatorType::DISJONCTIVE; }
	std::vector<std::unique_ptr<LTLFormula>> getComponents() const override;
	std::unique_ptr<LTLFormula> neg() const override;
	LTLFormula* getEventuality() const override { return this->operand.get(); };

	std::unique_ptr<LTLFormula> copy() const override;

	operator std::string() const override;
	bool operator==(const LTLFormula& formula) const;

private:
	std::unique_ptr<LTLFormula> operand;
};





class UntilOp : public LTLFormula
{
public:
	static const char SYMBOL = 'U';
	static const int PRIORITY = 0;

	UntilOp(LTLFormula* leftOperand, LTLFormula* rightOperand);
	UntilOp(std::unique_ptr<LTLFormula>&& leftOperand, std::unique_ptr<LTLFormula>&& rightOperand);
	~UntilOp();

	OperatorType getOperatorType() const override { return OperatorType::DISJONCTIVE; }
	std::vector<std::unique_ptr<LTLFormula>> getComponents() const override;
	std::unique_ptr<LTLFormula> neg() const override;
	LTLFormula* getEventuality() const override { return this->rightOperand.get(); };

	std::unique_ptr<LTLFormula> copy() const override;

	operator std::string() const override;
	bool operator==(const LTLFormula& formula) const;

private:
	std::unique_ptr<LTLFormula> leftOperand;
	std::unique_ptr<LTLFormula> rightOperand;
};


class ReleaseOp : public LTLFormula
{
public:
	static const char SYMBOL = 'R';
	static const int PRIORITY = UntilOp::PRIORITY + 1;

	ReleaseOp(LTLFormula* leftOperand, LTLFormula* rightOperand);
	ReleaseOp(std::unique_ptr<LTLFormula>&& leftOperand, std::unique_ptr<LTLFormula>&& rightOperand);
	~ReleaseOp();

	OperatorType getOperatorType() const override { return OperatorType::CONJONCTIVE; }
	std::vector<std::unique_ptr<LTLFormula>> getComponents() const override;
	std::unique_ptr<LTLFormula> neg() const override;
	LTLFormula* getEventuality() const override { return nullptr; };

	std::unique_ptr<LTLFormula> copy() const override;

	operator std::string() const override;
	bool operator==(const LTLFormula& formula) const;

private:
	std::unique_ptr<LTLFormula> leftOperand;
	std::unique_ptr<LTLFormula> rightOperand;
};


class AndOp : public LTLFormula
{
public:
	static const char SYMBOL = '&';
	static const int PRIORITY = ReleaseOp::PRIORITY + 1;

	//AndOp(LTLFormula* leftOperand, LTLFormula* rightOperand);
	AndOp(std::unique_ptr<LTLFormula>&& leftOperand, std::unique_ptr<LTLFormula>&& rightOperand);
	~AndOp();

	OperatorType getOperatorType() const override { return OperatorType::CONJONCTIVE; }
	std::vector<std::unique_ptr<LTLFormula>> getComponents() const override;
	std::unique_ptr<LTLFormula> neg() const override;
	LTLFormula* getEventuality() const override { return nullptr; };

	std::unique_ptr<LTLFormula> copy() const override;

	operator std::string() const override;
	bool operator==(const LTLFormula& formula) const;

private:
	std::unique_ptr<LTLFormula> leftOperand;
	std::unique_ptr<LTLFormula> rightOperand;
};


class OrOp : public LTLFormula
{
public:
	static const char SYMBOL = '|';
	static const int PRIORITY = AndOp::PRIORITY + 1;

	OrOp(LTLFormula* leftOperand, LTLFormula* rightOperand);
	OrOp(std::unique_ptr<LTLFormula>&& leftOperand, std::unique_ptr<LTLFormula>&& rightOperand);
	~OrOp();

	OperatorType getOperatorType() const override { return OperatorType::DISJONCTIVE; }
	std::vector<std::unique_ptr<LTLFormula>> getComponents() const override;
	std::unique_ptr<LTLFormula> neg() const override;
	LTLFormula* getEventuality() const override { return nullptr; };

	std::unique_ptr<LTLFormula> copy() const override;

	operator std::string() const override;
	bool operator==(const LTLFormula& formula) const;

private:
	std::unique_ptr<LTLFormula> leftOperand;
	std::unique_ptr<LTLFormula> rightOperand;
};


class ImpliesOp : public LTLFormula
{
public:
	static const char SYMBOL = '>';
	static const int PRIORITY = OrOp::PRIORITY + 1;

	ImpliesOp(LTLFormula* leftOperand, LTLFormula* rightOperand);
	ImpliesOp(std::unique_ptr<LTLFormula>&& leftOperand, std::unique_ptr<LTLFormula>&& rightOperand);
	~ImpliesOp();

	OperatorType getOperatorType() const override { return OperatorType::DISJONCTIVE; }
	std::vector<std::unique_ptr<LTLFormula>> getComponents() const override;
	std::unique_ptr<LTLFormula> neg() const override;
	LTLFormula* getEventuality() const override { return nullptr; };

	std::unique_ptr<LTLFormula> copy() const override;

	operator std::string() const override;
	bool operator==(const LTLFormula& formula) const;

private:
	std::unique_ptr<LTLFormula> leftOperand;
	std::unique_ptr<LTLFormula> rightOperand;
};




class VariableOp : public LTLFormula
{
public:
	VariableOp(char name, bool neg = false);
	~VariableOp();

	OperatorType getOperatorType() const override { return OperatorType::VARIABLE; }
	std::vector<std::unique_ptr<LTLFormula>> getComponents() const override;
	std::unique_ptr<LTLFormula> neg() const override;
	LTLFormula* getEventuality() const override { return nullptr; };
 
	std::unique_ptr<LTLFormula> copy() const override;

	char getName() const { return this->name; };
	bool isNeg() const { return this->_isNeg; };

	operator std::string() const override;
	bool operator==(const LTLFormula& formula) const;

private:
	char name;
	bool _isNeg;
};
