from copy import deepcopy

from LTLFormule import *
from typing import Text, ClassVar

class And(LTLFormule):
    comp1 : LTLFormule
    comp2 : LTLFormule

    def __init__(self, comp1 : LTLFormule, comp2 : LTLFormule):
        self.comp1 = comp1
        self.comp2 = comp2

    def getType(self) -> OperatorType:
        return OperatorType.CONJONCTIVE

    def getComponents(self) -> List[LTLFormule]:
        return [self.comp1, self.comp2]

    def neg(self) -> LTLFormule:
        return Or(self.comp1.neg(), self.comp2.neg())

class Or(LTLFormule):
    comp1 : LTLFormule
    comp2 : LTLFormule

    def __init__(self, comp1 : LTLFormule, comp2 : LTLFormule):
        self.comp1 = comp1
        self.comp2 = comp2

    def getType(self) -> OperatorType:
        return OperatorType.DISJONCTIVE

    def getComponents(self) -> List[LTLFormule]:
        return [self.comp1, self.comp2]

    def neg(self) -> LTLFormule:
        return And(self.comp1.neg(), self.comp2.neg())

class Implies(LTLFormule):
    comp1 : LTLFormule
    comp2 : LTLFormule

    def __init__(self, comp1 : LTLFormule, comp2 : LTLFormule):
        self.comp1 = comp1
        self.comp2 = comp2


    def getType(self) -> OperatorType:
        return OperatorType.DISJONCTIVE

    def getComponents(self) -> List[LTLFormule]:
        return [self.comp1.neg(), self.comp2]

    def neg(self) -> LTLFormule:
        return And(self.comp1, self.comp2.neg())

class Next(LTLFormule):
    comp1 : LTLFormule

    def __init__(self, comp1 : LTLFormule):
        self.comp1 = comp1

    def getType(self) -> OperatorType:
        return OperatorType.SUCCESSOR

    def getComponents(self) -> List[LTLFormule]:
        return [self.comp1]

    def neg(self) -> LTLFormule:
        return self.comp1.neg()

class Globally(LTLFormule):
    comp1 : LTLFormule

    def __init__(self, comp1 : LTLFormule):
        self.comp1 = comp1

    def getType(self) -> OperatorType:
        return OperatorType.CONJONCTIVE

    def getComponents(self) -> List[LTLFormule]:
        return [self.comp1, Next(self)]

    def neg(self) -> LTLFormule:
        return Finally(self.comp1.neg())

class Finally(LTLFormule):
    comp1 : LTLFormule

    def __init__(self, comp1 : LTLFormule):
        self.comp1 = comp1

    def getType(self) -> OperatorType:
        return OperatorType.DISJONCTIVE

    def getComponents(self) -> List[LTLFormule]:
        return [self.comp1, Next(self)]

    def neg(self) -> LTLFormule:
        return Globally(self.comp1.neg())

class Until(LTLFormule):
    comp1 : LTLFormule
    comp2 : LTLFormule

    def __init__(self, comp1 : LTLFormule, comp2 : LTLFormule):
        self.comp1 = comp1
        self.comp2 = comp2


    def getType(self) -> OperatorType:
        return OperatorType.DISJONCTIVE

    def getComponents(self) -> List[LTLFormule]:
        return [self.comp2, And(self.comp2, Next(self))]

    def neg(self) -> LTLFormule:
        return Release(self.comp1.neg(), self.comp2.neg())

class Release(LTLFormule):
    comp1 : LTLFormule
    comp2 : LTLFormule

    def __init__(self, comp1 : LTLFormule, comp2 : LTLFormule):
        self.comp1 = comp1
        self.comp2 = comp2


    def getType(self) -> OperatorType:
        return OperatorType.CONJONCTIVE

    def getComponents(self) -> List[LTLFormule]:
        return [Or(self.comp1, Next(self)), self.comp2]

    def neg(self) -> LTLFormule:
        return Until(self.comp1.neg(), self.comp2.neg())

class Variable(LTLFormule):
    _neg:bool
    name:str

    def __init__(self, name):
        self.name = name
        self._neg = False

    def getType(self) -> OperatorType:
        return OperatorType.VARIABLE

    def getComponents(self) -> List[LTLFormule]:
            return []

    def neg(self) -> LTLFormule:
        self._neg = not self._neg
        return self




def readFormule(s : Text) -> LTLFormule:

    def getOperatorFromString(op : Text) -> ClassVar[LTLFormule]:
        if(op == "G") : return Globally
        elif(op == "F") : return Finally
        elif(op == "N") : return Next
        elif(op == "U") : return Until
        elif(op == "R") : return Release
        elif(op == "&") : return And
        elif(op == "|") : return Or
        elif(op == ">") : return Implies

    unary = ["G", "F", "N", "!"]
    binaryOrder = [ "U", "R", "&", "|", ">"]
    maxOp = -1
    maxOpIndexB = -1

    parentheseDeepness = 0
    hasParenthese = False


    for i in range(len(s)):
        if(s[i] == "("):
            parentheseDeepness += 1
            hasParenthese = True
            continue
        if(s[i] == ")"):
            parentheseDeepness -= 1
            continue
        if(parentheseDeepness == 0 and s[i] in binaryOrder):
            idx = binaryOrder.index(s[i])

            if(idx >= maxOpIndexB):
                maxOp = i
                maxOpIndexB = idx

    if(maxOpIndexB < 0):
        for i in range(len(s)):
            if(s[i] in unary):
                if(s[i] == "!"):
                    return readFormule(s[i+1:]).neg()
                else:
                    return getOperatorFromString(s[i])(readFormule(s[i+1:]))
        if(hasParenthese):
            return readFormule(s[s.find('(')+1:s.find(')')])
        else:
            return Variable(s)
    else :
        return getOperatorFromString(s[maxOp])(readFormule(s[:maxOp]), readFormule(s[maxOp+1:]))



print(readFormule("a&(a|b)"))








