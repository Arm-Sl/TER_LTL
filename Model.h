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
	InterpretationFunction();
	InterpretationFunction(int stateCount);
	InterpretationFunction(InterpretationFunction&& interpretation) = default;
	InterpretationFunction(const InterpretationFunction& interpretation) = default;
	~InterpretationFunction();

	void setStateCount(int stateCount);

	///<summary> Get the value of a variable at a given state</summary>
	Interpretation get(int state, char name);

	void set(int state, char name, Interpretation i);
	const std::map<char, Interpretation>& getInterpretations(int state) const{ return this->interpretations[state]; }

	bool operator==(const InterpretationFunction& other) const;

	operator std::string() const;

private:
	std::vector<std::map<char, Interpretation>> interpretations;
};



class Transitions
{
public:
	Transitions();
	Transitions(int stateCount);
	~Transitions();
	
	void setStateCount(int stateCount);
	const std::vector<int>& getSuccessors(int state) const { return this->transitions[state]; };
	void setSuccessors(int state, const std::vector<int>& succs) { this->transitions[state] = succs; };
	void setSuccessors(int state, std::vector<int>&& succs) { this->transitions[state] = std::move(succs); };

	operator std::string() const;

private:
	std::vector<std::vector<int>> transitions;
};

class Model
{
public:

	Model(int stateCount);
	Model(const std::string & filepath, bool relative = true);
	~Model();	
	const InterpretationFunction& getInterpretationFunction() const { return this->interpretationFunction; };
	const std::vector<int>& getSuccessors(int state) const { return this->transitions.getSuccessors(state); };

	//TODO just for test
	void setSuccessors(int state, const std::vector<int>& succs) { this->transitions.setSuccessors(state, succs); };
	void setInt(int state, char name, Interpretation inter) { this->interpretationFunction.set(state, name, inter); };
	int getStateCount() const { return this->stateCount; };

	operator std::string() const;


private:
	int stateCount;
	Transitions transitions;
	InterpretationFunction interpretationFunction;

};

