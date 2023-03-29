#pragma once
#include<vector>
#include<map>
#include<string>


enum class Interpretation
{
	UNKNOWN,
	TRUE,
	FALSE,
};

class InterpretationFunction
{
public:
	InterpretationFunction(int stateCount);
	~InterpretationFunction();

	///<summary> Get the value of a variable at a given state</summary>
	Interpretation get(int state, char name);

	Interpretation set(int state, char name, Interpretation i);

	bool operator==(const InterpretationFunction& other) const;

	operator std::string() const;

private:
	std::vector<std::map<char, Interpretation>> interpretations;
};



struct Transition
{
	Transition(int f, int s)
	{
		this->f = f;
		this->s = s;
	}

	int f;
	int s;

	bool operator==(const Transition& other)
	{
		return this->f == other.f && this->s == other.s;
	}
};

class Model
{
public:

	Model();
	~Model();	

private:
	int stateCount;
	std::vector<Transition> transitions;
	InterpretationFunction interpretationFunction;

};

