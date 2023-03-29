__Filename = 'Model'
__author__ = 'Antonin'
__Creationdate__ = '21/03/2023'



from typing import Dict, Text, List, Optional, Any, cast, Tuple
from enum import Enum
from copy import deepcopy
from  LTLFormula import LTLFormula, OperatorType
from  LTL_Formules import Variable
from FormulaSet import FormulaSet
import networkx as nx
from networkx.drawing.nx_agraph import graphviz_layout
import matplotlib.pyplot as plt

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

    def __str__(self):
        return self.interpretations.__str__()

    def __eq__(self, other : Any):
        if(not isinstance(other, InterpretationFunction)):
            return False
        inter = cast(InterpretationFunction, other)
        if(len(self.interpretations) != len(inter.interpretations)):
            return False

        for s in range(len(self.interpretations)):
            for litteral in self.interpretations[s] :
                if(self.get(s, litteral) != inter.get(s, litteral)):
                    return False
            for litteral in other.interpretations[s] :
                if(self.get(s, litteral) != inter.get(s, litteral)):
                    return False
        return True



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




class PreTableauState:
    TableauStatePreStateCount = 0

    def __init__(self, state : int, formulas : FormulaSet, interpretation : InterpretationFunction):
        self.children : List['PreTableauState'] = list()
        self.parents : List['PreTableauState'] = list()
        self.state : int = state
        self.formulas : FormulaSet = formulas
        self.interpretation : InterpretationFunction = interpretation
        self.id = -1


    def setId(self):
        self.id = PreTableauState.TableauStatePreStateCount
        PreTableauState.TableauStatePreStateCount += 1

    def __eq__(self, other) -> bool:
        if(not isinstance(other, PreTableauState)):
            return False
        return self.state == other.state and self.formulas == other.formulas and self.interpretation == other.interpretation


class TableauState:
    TableauStateCount = 0

    def __init__(self, state : int, formulas : FormulaSet, interpretation : InterpretationFunction):
        self.children : List['TableauState'] = list()
        self.state : int = state
        self.formulas : FormulaSet = formulas
        self.interpretation : InterpretationFunction = interpretation
        self.id = -1


    def setId(self):
        self.id = PreTableauState.TableauStatePreStateCount
        PreTableauState.TableauStatePreStateCount += 1

    def __eq__(self, other) -> bool:
        if(not isinstance(other, PreTableauState)):
            return False
        return self.state == other.state and self.formulas == other.formulas and self.interpretation == other.interpretation


class Tableau:
    def __init__(self, model : Model, formula : LTLFormula, litterals : List[Variable]):
        self.model : Model = model
        self.rootState : Optional[PreTableauState] = None
        self.formula : LTLFormula = formula
        self.litterals : List[Variable] = litterals

        self.states : List[PreTableauState] = list()


class PreTableau:

    def __init__(self, model : Model, formula : LTLFormula, litterals : List[Variable]):
        self.model : Model = model
        self.rootState : Optional[PreTableauState] = None
        self.formula : LTLFormula = formula
        self.litterals : List[Variable] = litterals

        self.currentStates : List[PreTableauState] = list()
        self.currentPreStates : List[PreTableauState] = list()

        self.preStates : List[PreTableauState] = list()
        self.states : List[PreTableauState] = list()

    def getTrueLitteralsInFormula(self, state : int, interpretationFunction : InterpretationFunction = None) -> List[Variable]:
        res : List[Variable] = list()
        if(interpretationFunction is None):
            interpretationFunction = self.model.interpretationFunction

        for l in self.litterals:
            if((interpretationFunction.get(state, l.name) == Interpretation.TRUE and l.isNeg == False) or (interpretationFunction.get(state, l.name) == Interpretation.FALSE and l.isNeg == True)):
                res.append(l)

        return res


    def createInitialState(self):
        fSet = FormulaSet(deepcopy(self.formula), *self.getTrueLitteralsInFormula(0))
        self.rootState = PreTableauState(0, fSet, deepcopy(self.model.interpretationFunction))
        self.rootState.setId()
        self.currentPreStates.append(self.rootState)
        self.preStates.append(self.rootState)



    def expRule(self):

        for preState in self.currentPreStates:
            fullExp = deepcopy(preState.formulas).fullExpansion()
            for formulaSet in fullExp:
                f : InterpretationFunction = deepcopy(preState.interpretation)

                #update interpretation function

                for litteral in formulaSet.getLitterals():
                    if(f.get(preState.state, litteral.name) == Interpretation.UNKNOWN):
                        f.set(preState.state, litteral.name, Interpretation.FALSE if litteral.isNeg else Interpretation.TRUE)
                newState = PreTableauState(preState.state, formulaSet, f)

                #check if state is already present
                stateAlreadyPresent : bool = False
                for s2 in self.states:
                    if(s2 == newState):
                        preState.children.append(s2)
                        stateAlreadyPresent = True
                        break

                #don't add offspring State if model state has no successors
                if(not stateAlreadyPresent and len(self.model.transitions[preState.state]) > 0):
                    newState.setId()
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

                formulaSet = FormulaSet(*scomp, *self.getTrueLitteralsInFormula(s, state.interpretation))
                newPreState = PreTableauState(s, formulaSet, state.interpretation)

                #check if state is already present
                preStateAlreadyPresent : bool = False
                for s2 in self.preStates:
                    if(s2 == newPreState):
                        state.children.append(s2)
                        preStateAlreadyPresent = True
                        break
                if(not preStateAlreadyPresent):
                    newPreState.setId()
                    state.children.append(newPreState)
                    self.preStates.append(newPreState)
                    self.currentPreStates.append(newPreState)

        self.currentStates.clear()


    def preTableauComputation(self):
        while(len(self.currentPreStates) > 0 or len(self.currentStates) > 0):
            self.expRule()
            self.nextRule()


    def toTableau(self) -> Tableau:
        tab : Tableau = Tableau(self.model, self.formula, self.litterals)


        for state in self.states:
            state : TableauState = TableauState(state.state, state.formulas, state.interpretation)

            tab.states.append()

        return tab


    def stateElim1(self):
        pass

    def stateElim2(self):
        pass



    def show(self):

        g = nx.Graph()

        #        colors : List[Text] = list("" for i in range(TableauState.TableauStatePreStateCount))
        colors : List[Text] = list()
        #            colors[state.id] = '#1f78b4'
        #            colors[preState.id] = '#1fb460'
#        colors[10] = '#b4601f'

        statesIds : List[int] = list()
        preStatesIds : List[int] = list()
        fromStateTrans : List[Tuple[int, int]] = list()
        fromPreStateTrans : List[Tuple[int, int]] = list()
        labels : Dict[int, str] = dict()

        for state in self.states:
            g.add_node(state.id)
            statesIds.append(state.id)
            labels[state.id] = str(state.id)
            for child in state.children:
                fromStateTrans.append((state.id, child.id))

        for preState in self.preStates:
            g.add_node(preState.id)
            preStatesIds.append(preState.id)
            labels[preState.id] = str(preState.id)
            for child in preState.children:
                fromPreStateTrans.append((preState.id, child.id))

        widths = list()
        alreadyExplored = set()
        def getWidths(root : PreTableauState, depth = 0):
            alreadyExplored.add(root.id)
            if(len(widths) <= depth):
                widths.append(1)
            else:
                widths[depth] += 1
            for e in root.children:
                if(e.id > root.id and not e.id in alreadyExplored):
                    getWidths(e, depth+1)

        getWidths(self.rootState)
        maxWidth = max(widths)

        pos : Dict[int, Tuple[int,int]] = dict()

        alreadyExplored.clear()
        def getPos(root : PreTableauState, depth = 0, currentPos = 0):
            pos[root.id] = (currentPos, -depth)

            validChildren : List[PreTableauState] = list()

            for e in root.children:
                if(e.id > root.id and not e.id in alreadyExplored):
                    validChildren.append(e)
                    alreadyExplored.add(e.id)

            step = maxWidth/widths[depth]
            childPos =  currentPos - (step * (len(validChildren) - 1))/2
            for e in validChildren:
                getPos(e, depth+1, childPos)
                childPos += step
        getPos(self.rootState)

        nx.draw_networkx_nodes(g, pos, statesIds, node_shape="o")
        nx.draw_networkx_nodes(g, pos, preStatesIds, node_shape="s", node_color='#1fb460')
        nx.draw_networkx_edges(g, pos, edgelist=fromStateTrans, style="-", arrows=True, arrowstyle='-|>')
        nx.draw_networkx_edges(g, pos, edgelist=fromPreStateTrans, style="--", arrows=True, arrowstyle='-|>')
        nx.draw_networkx_labels(g, pos, labels, font_size=10, font_color="whitesmoke")
        plt.show()
