
from dataclasses import dataclass


@dataclass
class Constant:
    name: str
    value: int
    desc: str
    refid: str


@dataclass
class ConstantGroup:
    name: str
    refid: str
    constants: list[Constant]


@dataclass
class Param:
    type: str
    name: str


@dataclass
class Function:
    name: str
    return_type: str
    brief: str
    detailed: str
    file: str
    params: list[Param]


@dataclass
class Field:
    type: str
    name: str
    argsstring: str
    desc: str


@dataclass
class Struct:
    name: str
    brief: str
    detailed: str
    file: str
    fields: list[Param]


@dataclass
class Typedef:
    name: str
    type: str
    brief: str
    detailed: str


@dataclass
class Module:
    name: str
    desc: str
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
    body: str
    refid: str
    desc: str

