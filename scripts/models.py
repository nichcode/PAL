
from dataclasses import dataclass


@dataclass
class Constant:
    name: str
    brief: str
    detailed: str
    refid: str
    value: int


@dataclass
class ConstantGroup:
    name: str
    brief: str
    detailed: str
    refid: str
    constants: list[Constant]


@dataclass
class Param:
    type: str
    name: str


@dataclass
class Function:
    name: str
    brief: str
    detailed: str
    refid: str
    file: str
    return_type: str
    params: list[Param]


@dataclass
class Field:
    name: str
    brief: str
    detailed: str
    type: str
    argsstring: str


@dataclass
class Struct:
    name: str
    brief: str
    detailed: str
    refid: str
    file: str
    fields: list[Param]


@dataclass
class Typedef:
    name: str
    brief: str
    detailed: str
    refid: str
    type: str


@dataclass
class Module:
    name: str
    brief: str
    detailed: str
    license: str
    refid: str
    file: str
    functions: list[Function]
    structs: list[Struct]
    typedefs: list[Typedef]
    constants: list[Constant]
    constant_groups: list[ConstantGroup]


@dataclass
class Page:
    name: str
    brief: str
    detailed: str
    refid: str
