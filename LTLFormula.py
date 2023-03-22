from copy import deepcopy
from enum import Enum
from abc import ABC
from abc import abstractmethod
from typing import List, Text


class OperatorType (Enum):
    CONJONCTIVE = 1
    DISJONCTIVE = 2
    SUCCESSOR = 3
    VARIABLE = 4



class LTLFormula(ABC):

    @abstractmethod
    def getType(self) -> OperatorType:
        pass

    @abstractmethod
    def getComponents(self) -> List['LTLFormula'] :
        pass

    @abstractmethod
    def neg(self) -> 'LTLFormula':
        pass

    @abstractmethod
    def __eq__(self, other):
        pass




