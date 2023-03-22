__author__ = 'Antonin'
__Filename = 'Model'
__Creationdate__ = '21/03/2023'



from typing import Dict, Text, List, Optional
from enum import Enum
from copy import deepcopy
from  LTLFormula import LTLFormula, OperatorType
from  LTL_Formules import Variable
from FormulaSet import FormulaSet

class Interpretation(Enum):
    TRUE = 1
    FALSE = 2
    UNKNOWN = 3


class InterpretationFunction:

    def __init__(self, stateCount : int = 0):
        self.interpretations : List[Dict[Text, Interpretation]] = list()
        for i in range(stateCount):
            self.interpretations.append(dict())

    def __deepcopy__(self, memodict):
        cpy = InterpretationFunction()
        cpy.interpretations = deepcopy(self.interpretations)
        return cpy

    def add(self, state : int,  **interpretations : Interpretation):
        self.interpretations[state] |= interpretations

    def set(self, state : int, name : str, interpretation : Interpretation):
        self.interpretations[state][name] = interpretation

    def get(self, state : int,  name : Text) -> Interpretation:
        return self.interpretations[state].get(name, Interpretation.UNKNOWN)

    def __eq__(self, other):
        if(not isinstance(other, InterpretationFunction)):
            return False
        return self.interpretations == other.interpretations

class Model:


    def __init__(self, stateCount : int, transitions : List[List[int]] = None, interpretationFunction : InterpretationFunction = None):
        self.stateCount : int = stateCount
        self.transitions : List[List[int]]
        self.interpretationFunction : InterpretationFunction

        if(transitions is None):
            for i in range(stateCount):
                self.transitions.append(list())
        else:
            self.transitions = transitions
        if(interpretationFunction is None):
            self.interpretationFunction = InterpretationFunction(stateCount)
        else:
            self.interpretationFunction = interpretationFunction




class TableauState:

    def __init__(self, state : int, formulas : FormulaSet, interpretation : InterpretationFunction):
        self.children : List['TableauState'] = list()
        self.state : int = state
        self.formulas : FormulaSet = formulas
        self.interpretation : InterpretationFunction = interpretation

    def __eq__(self, other) -> bool:
        if(not isinstance(other, TableauState)):
            return False
        return self.state == other.state and self.formulas == other.formulas and self.interpretation == other.interpretation

class Tableau:

    def __init__(self, model : Model, formula : LTLFormula, litterals : List[Variable]):
        self.model : Model = model
        self.rootState : Optional[TableauState] = None
        self.formula : LTLFormula = formula
        self.litterals : List[Variable] = litterals

        self.currentStates : List[TableauState] = list()
        self.currentPreStates : List[TableauState] = list()

        self.preStates : List[TableauState] = list()
        self.states : List[TableauState] = list()

    def getTrueLitteralsInFormula(self, state : int) -> List[Variable]:
        res : List[Variable] = list()

        for l in self.litterals:
            if((self.model.interpretationFunction.get(state, l.name) == Interpretation.TRUE and l.isNeg == False) or (self.model.interpretationFunction.get(state, l.name) == Interpretation.FALSE and l.isNeg == True)):
                res.append(l)

        return res



    def createInitialState(self):
        fSet = FormulaSet(deepcopy(self.formula), *self.getTrueLitteralsInFormula(0))
        self.rootState = TableauState(0, fSet, deepcopy(self.model.interpretationFunction))
        self.currentPreStates.append(self.rootState)
        self.preStates.append(self.rootState)


    #TODO finish ExpRule
    #TODO keep track of previous TableauStates / PreStates for edge creation


    def expRule(self):

        for preState in self.currentPreStates:
            fullExp = preState.formulas.fullExpansion()
            for formulaSet in fullExp:
                f : InterpretationFunction = deepcopy(preState.interpretation)

                #update interpretation function
                for litteral in formulaSet.getLitterals():
                    if(f.get(preState.state, litteral.name) == Interpretation.UNKNOWN):
                        f.set(preState.state, litteral.name, Interpretation.FALSE if litteral.isNeg else Interpretation.TRUE)
                newState = TableauState(preState.state,formulaSet, f)

                #check if state is already present
                stateAlreadyPresent : bool = False
                for s2 in self.states:
                    if(s2 == newState):
                        preState.children.append(s2)
                        stateAlreadyPresent = True
                        break

                #don't add offspring State if model state has no successors
                if(not stateAlreadyPresent and len(self.model.transitions[preState.state]) > 0):
                    preState.children.append(newState)
                    self.states.append(newState)
                    self.currentStates.append(newState)
        self.currentPreStates.clear()


    def nextRule(self):
        for state in self.currentStates:
            for s in self.model.transitions[state.state] :

                #calculate scomp
                scomp : List[LTLFormula] = list()

                for formula in state.formulas.formulas:
                    if(formula.getType() == OperatorType.SUCCESSOR):
                        scomp.extend(formula.getComponents())

                formulaSet = FormulaSet(*scomp, *self.getTrueLitteralsInFormula(s))
                newPreState = TableauState(s,formulaSet, state.interpretation)

                #check if state is already present
                preStateAlreadyPresent : bool = False
                for s2 in self.states:
                    if(s2 == newPreState):
                        state.children.append(s2)
                        preStateAlreadyPresent = True
                        break
                if(not preStateAlreadyPresent):
                    state.children.append(newPreState)
                    self.preStates.append(newPreState)
                    self.currentPreStates.append(newPreState)

        self.currentStates.clear()


    #TODO next rule