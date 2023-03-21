__author__ = 'Antonin'
__Filename = 'Model'
__Creationdate__ = '21/03/2023'



from typing import Dict, Text, List, Optional
from enum import Enum
from copy import deepcopy
from  LTLFormula import FormulaSet, LTLFormula
from  LTL_Formules import Variable

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

    def get(self, state : int,  name : Text) -> Interpretation:
        return self.interpretations[state][name]


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
        self.children : List['TableauState']
        self.state : int = state
        self.formulas : FormulaSet = formulas
        self.interpretation : InterpretationFunction = interpretation

class Tableau:

    def __init__(self, model : Model, formula : LTLFormula, litterals : List[Variable]):
        self.model : Model = model
        self.rootState : Optional[TableauState] = None
        self.formula : LTLFormula = formula
        self.litterals : List[Variable] = litterals

        self.currentStates : List[TableauState] = list()
        self.currentPreStates : List[TableauState] = list()

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


    def ExpRule(self):

        for s in self.currentPreStates:
            fullExp = s.formulas.fullExpansion()
            #TODO finish ExpRule
            #TODO keep track of previous TableauStates / PreStates for edge