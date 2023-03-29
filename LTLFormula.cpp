#include "LTLFormula.h"
#include <cctype>
#include <iostream>

#pragma region LTLFormula
LTLFormula::LTLFormula()
{
}


LTLFormula::~LTLFormula()
{
}


std::unique_ptr<LTLFormula> LTLFormula::readFormule(std::string s)
{
	int parentheseDeepness = 0;

	int firstUnaryIndex = -1;

	int maxBinaryOpPriority = -1;
	int maxBinaryOpIndex = -1;

	int firstOpenParenthesIndex = -1;
	int lastCloseParenthesIndex = -1;

	for (int i = 0; i < static_cast<int>(s.size()); i++)
	{
		switch (s[i])
		{
		case '(':
			if (firstOpenParenthesIndex < 0)
				firstOpenParenthesIndex = i;
			parentheseDeepness++;
			continue;
		case ')':
			lastCloseParenthesIndex = i;
			parentheseDeepness--;
			continue;
		case '!':
		case NextOp::SYMBOL:
		case GloballyOp::SYMBOL:
		case FinallyOp::SYMBOL:
			if (parentheseDeepness == 0 && firstUnaryIndex == -1)
				firstUnaryIndex = i;
			break;
		case UntilOp::SYMBOL:
			if (parentheseDeepness == 0 && UntilOp::PRIORITY > maxBinaryOpPriority)
			{
				maxBinaryOpPriority = UntilOp::PRIORITY;
				maxBinaryOpIndex = i;
			}
			break;
		case ReleaseOp::SYMBOL:
			if (parentheseDeepness == 0 && ReleaseOp::PRIORITY > maxBinaryOpPriority)
			{
				maxBinaryOpPriority = ReleaseOp::PRIORITY;
				maxBinaryOpIndex = i;
			}
			break;
		case AndOp::SYMBOL:
			if (parentheseDeepness == 0 && AndOp::PRIORITY > maxBinaryOpPriority)
			{
				maxBinaryOpPriority = AndOp::PRIORITY;
				maxBinaryOpIndex = i;
			}
			break;
		case OrOp::SYMBOL:
			if (parentheseDeepness == 0 && OrOp::PRIORITY > maxBinaryOpPriority)
			{
				maxBinaryOpPriority = OrOp::PRIORITY;
				maxBinaryOpIndex = i;
			}
			break;
		case ImpliesOp::SYMBOL:
			if (parentheseDeepness == 0 && ImpliesOp::PRIORITY > maxBinaryOpPriority)
			{
				maxBinaryOpPriority = ImpliesOp::PRIORITY;
				maxBinaryOpIndex = i;
			}
			break;
		default:
			break;
		}
	}

	if (maxBinaryOpIndex > -1)
	{
		switch (s[maxBinaryOpIndex])
		{
		case UntilOp::SYMBOL:
			return std::make_unique<UntilOp>(LTLFormula::readFormule(s.substr(0, maxBinaryOpIndex)), LTLFormula::readFormule(s.substr(maxBinaryOpIndex + 1)));
		case ReleaseOp::SYMBOL:
			return std::make_unique<ReleaseOp>(LTLFormula::readFormule(s.substr(0, maxBinaryOpIndex)), LTLFormula::readFormule(s.substr(maxBinaryOpIndex + 1)));
		case AndOp::SYMBOL:
			return std::make_unique<AndOp>(LTLFormula::readFormule(s.substr(0, maxBinaryOpIndex)), LTLFormula::readFormule(s.substr(maxBinaryOpIndex + 1)));
		case OrOp::SYMBOL:
			return std::make_unique<OrOp>(LTLFormula::readFormule(s.substr(0, maxBinaryOpIndex)), LTLFormula::readFormule(s.substr(maxBinaryOpIndex + 1)));
		case ImpliesOp::SYMBOL:
			return std::make_unique<ImpliesOp>(LTLFormula::readFormule(s.substr(0, maxBinaryOpIndex)), LTLFormula::readFormule(s.substr(maxBinaryOpIndex + 1)));
		default:
			break;
		}
	}
	else if (firstUnaryIndex > -1)
	{
		switch (s[firstUnaryIndex])
		{
		case '!':
			return readFormule(s.substr(firstUnaryIndex + 1))->neg();
		case NextOp::SYMBOL:
			return std::make_unique<NextOp>(readFormule(s.substr(firstUnaryIndex + 1)));
		case GloballyOp::SYMBOL:
			return std::make_unique<GloballyOp>(readFormule(s.substr(firstUnaryIndex + 1)));
		case FinallyOp::SYMBOL:
			return std::make_unique<FinallyOp>(readFormule(s.substr(firstUnaryIndex + 1)));
		default:
			break;
		}
	}
	else if (firstOpenParenthesIndex > -1)
		return readFormule(s.substr(firstOpenParenthesIndex + 1, lastCloseParenthesIndex - firstOpenParenthesIndex - 1));
	else
	{
		char name = 0;
		for (auto c : s)
		{
			if (isalpha(c))
			{
				name = c;
				break;
			}
		}
		return std::make_unique<VariableOp>(name);

	}

	return nullptr;
}

#pragma endregion




#pragma region NextOp
NextOp::NextOp(LTLFormula * operand) : operand(operand)
{
}

NextOp::NextOp(std::unique_ptr<LTLFormula>&& operand) : operand(std::move(operand))
{
}
NextOp::NextOp(NextOp && op)
{
	
}
NextOp::~NextOp()
{
}


void NextOp::getComponents(std::vector<std::unique_ptr<LTLFormula>>& comps) const
{
	comps.push_back(this->operand->copy());
}

std::unique_ptr<LTLFormula> NextOp::neg() const
{
	return std::make_unique<NextOp>(this->operand->neg());
}


std::unique_ptr<LTLFormula> NextOp::copy() const
{
	return std::make_unique<NextOp>(this->operand->copy());
}

NextOp::operator std::string() const
{
	return std::string(1, NextOp::SYMBOL) + "(" + this->operand->operator std::string() + ")";
}

bool NextOp::operator==(const LTLFormula & formula) const
{
	if (const NextOp* other = dynamic_cast<const NextOp*>(&formula))
	{
		return *this->operand == *other->operand;
	}
	return false;
}

#pragma endregion


#pragma region GloballyOp

GloballyOp::GloballyOp(LTLFormula * operand) : operand(operand)
{
}

GloballyOp::GloballyOp(std::unique_ptr<LTLFormula>&& operand) : operand(std::move(operand))
{
}
GloballyOp::~GloballyOp()
{
}


void GloballyOp::getComponents(std::vector<std::unique_ptr<LTLFormula>>& comps) const
{
	 comps.push_back(this->operand->copy());
	 comps.push_back(std::make_unique<NextOp>(std::make_unique<GloballyOp>(this->operand->copy())));
}

std::unique_ptr<LTLFormula> GloballyOp::neg() const
{
	return std::make_unique<FinallyOp>(this->operand->neg());
}


std::unique_ptr<LTLFormula> GloballyOp::copy() const
{
	return std::make_unique<GloballyOp>(this->operand->copy());
}

GloballyOp::operator std::string() const
{
	return std::string(1, GloballyOp::SYMBOL) + "(" + this->operand->operator std::string() + ")";
}

bool GloballyOp::operator==(const LTLFormula & formula) const
{
	if (const GloballyOp* other = dynamic_cast<const GloballyOp*>(&formula))
	{
		return *this->operand == *other->operand;
	}
	return false;
}

#pragma endregion


#pragma region FinallyOp

FinallyOp::FinallyOp(LTLFormula * operand) : operand(operand)
{
}

FinallyOp::FinallyOp(std::unique_ptr<LTLFormula>&& operand) : operand(std::move(operand))
{
}
FinallyOp::~FinallyOp()
{
}


void FinallyOp::getComponents(std::vector<std::unique_ptr<LTLFormula>>& comps) const
{
	comps.push_back(this->operand->copy());
	comps.push_back(std::make_unique<NextOp>(std::make_unique<FinallyOp>(this->operand->copy())));
}

std::unique_ptr<LTLFormula> FinallyOp::neg() const
{
	return std::make_unique<GloballyOp>(this->operand->neg());
}


std::unique_ptr<LTLFormula> FinallyOp::copy() const
{
	return std::make_unique<FinallyOp>(this->operand->copy());
}

FinallyOp::operator std::string() const
{
	return std::string(1, FinallyOp::SYMBOL) + "(" + this->operand->operator std::string() + ")";
}

bool FinallyOp::operator==(const LTLFormula & formula) const
{
	if (const FinallyOp* other = dynamic_cast<const FinallyOp*>(&formula))
	{
		return *this->operand == *other->operand;
	}
	return false;
}

#pragma endregion




#pragma region UntilOp
UntilOp::UntilOp(LTLFormula * leftOperand, LTLFormula * rightOperand) : leftOperand(leftOperand), rightOperand(rightOperand)
{
}

UntilOp::UntilOp(std::unique_ptr<LTLFormula>&& leftOperand, std::unique_ptr<LTLFormula>&& rightOperand) : leftOperand(std::move(leftOperand)), rightOperand(std::move(rightOperand))
{
}
UntilOp::~UntilOp()
{
}

void UntilOp::getComponents(std::vector<std::unique_ptr<LTLFormula>>& comps) const
{
	comps.push_back(this->leftOperand->copy());
	comps.push_back(std::make_unique<AndOp>(this->rightOperand->copy(), std::make_unique<NextOp>(std::make_unique<UntilOp>(this->leftOperand->copy(), this->rightOperand->copy()))));
}

std::unique_ptr<LTLFormula> UntilOp::neg() const
{
	return std::make_unique<ReleaseOp>(this->leftOperand->neg(), this->rightOperand->neg());
}


std::unique_ptr<LTLFormula> UntilOp::copy() const
{
	return std::make_unique<UntilOp>(this->leftOperand->copy(), this->rightOperand->copy());
}

UntilOp::operator std::string() const
{
	return "(" + this->leftOperand->operator std::string() + ") " + UntilOp::SYMBOL + " (" + this->rightOperand->operator std::string() + ")";
}


bool UntilOp::operator==(const LTLFormula & formula) const
{
	if (const UntilOp* other = dynamic_cast<const UntilOp*>(&formula))
	{
		return *this->leftOperand == *other->leftOperand && *this->rightOperand == *other->rightOperand;
	}
	return false;
}
#pragma endregion


#pragma region ReleaseOp
ReleaseOp::ReleaseOp(LTLFormula * leftOperand, LTLFormula * rightOperand) : leftOperand(leftOperand), rightOperand(rightOperand)
{
}

ReleaseOp::ReleaseOp(std::unique_ptr<LTLFormula>&& leftOperand, std::unique_ptr<LTLFormula>&& rightOperand) : leftOperand(std::move(leftOperand)), rightOperand(std::move(rightOperand))
{
}
ReleaseOp::~ReleaseOp()
{
}

void ReleaseOp::getComponents(std::vector<std::unique_ptr<LTLFormula>>& comps) const
{
	comps.push_back(std::make_unique<OrOp>(this->leftOperand->copy(), std::make_unique<NextOp>(std::make_unique<ReleaseOp>(this->leftOperand->copy(), this->rightOperand->copy()))));
	comps.push_back(this->rightOperand->copy());
}

std::unique_ptr<LTLFormula> ReleaseOp::neg() const
{
	return std::make_unique<UntilOp>(this->leftOperand->neg(), this->rightOperand->neg());
}


std::unique_ptr<LTLFormula> ReleaseOp::copy() const
{
	return std::make_unique<ReleaseOp>(this->leftOperand->copy(), this->rightOperand->copy());
}

ReleaseOp::operator std::string() const
{
	return "(" + this->leftOperand->operator std::string() + ") " + ReleaseOp::SYMBOL + " (" + this->rightOperand->operator std::string() + ")";
}


bool ReleaseOp::operator==(const LTLFormula & formula) const
{
	if (const ReleaseOp* other = dynamic_cast<const ReleaseOp*>(&formula))
	{
		return *this->leftOperand == *other->leftOperand && *this->rightOperand == *other->rightOperand;
	}
	return false;
}

#pragma endregion


#pragma region AndOp

//AndOp::AndOp(LTLFormula * leftOperand, LTLFormula * rightOperand) : leftOperand(leftOperand), rightOperand(rightOperand)
//{
//}

AndOp::AndOp(std::unique_ptr<LTLFormula>&&  leftOperand, std::unique_ptr<LTLFormula>&& rightOperand) : leftOperand(std::move(leftOperand)), rightOperand(std::move(rightOperand))
{
	
}

AndOp::~AndOp()
{
}


void AndOp::getComponents(std::vector<std::unique_ptr<LTLFormula>>& comps) const
{	
	comps.push_back(this->leftOperand->copy());
	comps.push_back(this->rightOperand->copy());
}

std::unique_ptr<LTLFormula> AndOp::neg() const
{
	return std::make_unique<OrOp>(this->leftOperand->neg(), this->rightOperand->neg());
}

std::unique_ptr<LTLFormula> AndOp::copy() const
{
	return std::make_unique<AndOp>(this->leftOperand->copy(), this->rightOperand->copy());
}

AndOp::operator std::string() const
{
	return "(" + this->leftOperand->operator std::string() + ") " + AndOp::SYMBOL + " (" + this->rightOperand->operator std::string() + ")";
}


bool AndOp::operator==(const LTLFormula & formula) const
{
	if (const AndOp* other = dynamic_cast<const AndOp*>(&formula))
	{
		return *this->leftOperand == *other->leftOperand && *this->rightOperand == *other->rightOperand;
	}
	return false;
}

#pragma endregion


#pragma region OrOp
OrOp::OrOp(LTLFormula * leftOperand, LTLFormula * rightOperand) : leftOperand(leftOperand), rightOperand(rightOperand)
{
}

OrOp::OrOp(std::unique_ptr<LTLFormula>&& leftOperand, std::unique_ptr<LTLFormula>&& rightOperand) : leftOperand(std::move(leftOperand)), rightOperand(std::move(rightOperand))
{
}

OrOp::~OrOp()
{
}

void OrOp::getComponents(std::vector<std::unique_ptr<LTLFormula>>& comps) const
{
	comps.push_back(this->leftOperand->copy());
	comps.push_back(this->rightOperand->copy());
}

std::unique_ptr<LTLFormula> OrOp::neg() const
{
	return std::make_unique<AndOp>(this->leftOperand->neg(), this->rightOperand->neg());
}


std::unique_ptr<LTLFormula> OrOp::copy() const
{
	return std::make_unique<OrOp>(this->leftOperand->copy(), this->rightOperand->copy());
}

OrOp::operator std::string() const
{
	return "(" + this->leftOperand->operator std::string() + ") " + OrOp::SYMBOL + " (" + this->rightOperand->operator std::string() + ")";
}


bool OrOp::operator==(const LTLFormula & formula) const
{
	if (const OrOp* other = dynamic_cast<const OrOp*>(&formula))
	{
		return *this->leftOperand == *other->leftOperand && *this->rightOperand == *other->rightOperand;
	}
	return false;
}

#pragma endregion


#pragma region ImpliesOp
ImpliesOp::ImpliesOp(LTLFormula * leftOperand, LTLFormula * rightOperand) : leftOperand(leftOperand), rightOperand(rightOperand)
{
}

ImpliesOp::ImpliesOp(std::unique_ptr<LTLFormula>&& leftOperand, std::unique_ptr<LTLFormula>&& rightOperand) : leftOperand(std::move(leftOperand)), rightOperand(std::move(rightOperand))
{
}
ImpliesOp::~ImpliesOp()
{
}

void ImpliesOp::getComponents(std::vector<std::unique_ptr<LTLFormula>>& comps) const
{
	comps.push_back(this->leftOperand->neg());
	comps.push_back(this->rightOperand->copy());
}

std::unique_ptr<LTLFormula> ImpliesOp::neg() const
{
	return std::make_unique<AndOp>(this->leftOperand->copy(), this->rightOperand->neg());
}


std::unique_ptr<LTLFormula> ImpliesOp::copy() const
{
	return std::make_unique<ImpliesOp>(this->leftOperand->copy(), this->rightOperand->copy());
}

ImpliesOp::operator std::string() const
{
	return "(" + this->leftOperand->operator std::string() + ") " + ImpliesOp::SYMBOL + " (" + this->rightOperand->operator std::string() + ")";
}


bool ImpliesOp::operator==(const LTLFormula & formula) const
{
	if (const ImpliesOp* other = dynamic_cast<const ImpliesOp*>(&formula))
	{
		return *this->leftOperand == *other->leftOperand && *this->rightOperand == *other->rightOperand;
	}
	return false;
}

#pragma endregion




#pragma region VariableOp

VariableOp::VariableOp(char name, bool neg) : name(name), isNeg(neg)
{
}

VariableOp::~VariableOp()
{
}

void VariableOp::getComponents(std::vector<std::unique_ptr<LTLFormula>>& comps) const
{
}

std::unique_ptr<LTLFormula> VariableOp::neg() const
{
	return std::make_unique<VariableOp>(this->name, !this->isNeg);
}

std::unique_ptr<LTLFormula> VariableOp::copy() const
{
	return std::make_unique<VariableOp>(this->name, this->isNeg);
}

VariableOp::operator std::string() const
{
	if (this->isNeg)
		return std::string(1, '!') + this->name;
	return std::string(1, this->name);
}



bool VariableOp::operator==(const LTLFormula & formula) const
{
	if (const VariableOp* other = dynamic_cast<const VariableOp*>(&formula))
	{
		return this->name == other->name && this->isNeg == other->isNeg;
	}
	return false;
}

#pragma endregion
