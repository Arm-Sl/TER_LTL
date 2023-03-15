from LTLFormule import *
from typing import Text

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


def ReadFormule(s : Text) -> LTLFormule:
    
    pass
