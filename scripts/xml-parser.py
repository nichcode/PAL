#!/usr/bin/env python3

from pathlib import Path
from xml.etree import ElementTree as ET
from dataclasses import dataclass


@dataclass
class Constant:
    name: str
    value: int
    desc: str


@dataclass
class ConstantGroup:
    name: str
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
    brief: str
    detailed: str
    functions: list[Function]
    structs: list[Struct]
    typedefs: list[Typedef]
    constants: list[Constant]
    constant_groups: list[ConstantGroup]


class XMLParser:
    def __init__(self) -> None:
        self.root_dir = Path(__file__).resolve().parents[1]
        self.xml_dir = self.root_dir / "build/xml"

    def parse_modules(self) -> list[Module]:
        """parses modules"""
        modules = []

        refids = self._find_modules_refids()
        for refid in refids:
            root = ET.parse(self.xml_dir / f"{refid}.xml").getroot()
            cpdef = root.find("compounddef")

            # compute module file id
            cpname = cpdef.find("compoundname")
            tmp = cpname.text.replace('_', '__').replace('.', '__')
            file_refid = f"{tmp}_8h"

            data = self._parse_module_file(file_refid)
            functions = self._parse_functions(cpdef)
            structs = self._parse_structs(cpdef)
            typedefs = self._parse_typedefs(cpdef)
            constants = self._parse_constants(cpdef)
            constant_groups = self._parse_constant_groups(cpdef)

            module = Module(
                name=cpdef.findtext("title"),
                brief=data["brief"],
                detailed=data["detailed"],
                functions=functions,
                structs=structs,
                typedefs=typedefs,
                constants=constants,
                constant_groups=constant_groups
            )

            modules.append(module)

        print("modules parsed successfully")
        return modules

    def _find_modules_refids(self) -> list:
        """find all modules"""
        root = ET.parse(self.xml_dir / "index.xml").getroot()
        refids = []
        children = []

        for cp in root.iterfind("compound"):
            if cp.get("kind") == "group":
                refids.append(cp.get("refid"))

        for refid in refids:
            root = ET.parse(self.xml_dir / f"{refid}.xml").getroot()
            cpdef = root.find("compounddef")

            # find nested sub-groups and cache them
            for inner in cpdef.findall("innergroup"):
                group_refid = inner.get("refid")
                children.append(group_refid)

        # filter modules
        modules = []
        for refid in refids:
            if refid not in children:
                modules.append(refid)

        return modules

    def _parse_module_file(self, file_refid) -> dict:
        """parses module file"""
        root = ET.parse(self.xml_dir / f"{file_refid}.xml").getroot()
        cpdef = root.find("compounddef")

        data = {}
        element = cpdef.find("briefdescription")
        data["brief"] = self._xml_tags_to_html(element)

        element = cpdef.find("detaileddescription")
        data["detailed"] = data["brief"] + self._xml_tags_to_html(element)

        return data

    def _xml_tags_to_html(self, element: ET.Element | None) -> str:
        """converts XML tags into HTML"""
        html_out = ""
        if element is None:
            return ""

        tag_map = {
            "para": ("<p>", "</p>"),
            "computeroutput": ("<code>", "</code>"),
            "c": ("<code>", "</code>"),
            "codeline": ("", "\n"),
            "highlight": ("", ""),

            "table": ("<table>", "</table>"),
            "row": ("<tr>", "</tr>"),
            "entry": ("<td>", "</td>"),
            "tablecols": ("", ""),

            "itemizedlist": ("<ul>", "</ul>"),
            "orderedlist": ("<ol>", "</ol>"),
            "listitem": ("<li>", "</li>"),

            "parameterlist": ("<div><h4>Parameters</h4><dl>", "</dl></div>"),
            "parameteritem": ("<div>", "</div>"),
            "parameternamelist": ("", ""), 
            "parameterdescription": ("<dd>", "</dd>"),
        }

        open_tag, close_tag = tag_map.get(element.tag, ("", ""))
        dir_badge = ""

        if element.tag == "parametername":
            direction = element.get("direction")
            if direction:
                dir_badge = f"<span>{direction}</span> "

            open_tag = f"<dt>{dir_badge}<code>"
            close_tag = "</code></dt>"

        elif element.tag == "entry" and element.get("thead") == "yes":
            open_tag, close_tag = ("<th>", "</th>")

        elif element.tag == "simplesect":
            kind = element.get("kind", "note")
            if kind == "since":
                open_tag, close_tag = ("<div><strong>Since:</strong> ", "</div>")
            elif kind == "par":
                open_tag, close_tag = ("<div>", "</div>")
            else:
                open_tag, close_tag = (f"<div callout-{kind}'>", "</div>")
            
        elif element.tag == "ulink":
            url = element.get("url", "#")
            open_tag = f'<a href="{url}" target="_blank" rel="noopener noreferrer">'
            close_tag = "</a>"

        elif element.tag == "ref":
            refid = element.get("refid", "#")
            open_tag = f'<a" href="{refid}.html">'
            close_tag = "</a>"

        html_out += open_tag
        if element.text:
            html_out += element.text

        for child in element:
            html_out += self._xml_tags_to_html(child)
            if child.tail:
                html_out += child.tail

        html_out += close_tag

        return html_out

    def _parse_functions(self, cpdef) -> list:
        """parses function"""
        functions = []

        memberdefs = cpdef.findall(".//memberdef[@kind='function']")
        for memberdef in memberdefs:
            element = memberdef.find("briefdescription")
            brief = self._xml_tags_to_html(element)

            element = memberdef.find("detaileddescription")
            detailed = brief + self._xml_tags_to_html(element)

            element = memberdef.find("location")
            filepath = element.get("file")

            params = []
            for param in memberdef.findall("param"):
                type = self._xml_tags_to_html(param)
                name = param.findtext("declname", default="").strip()
                params.append(Param(type=type, name=name))

            function = Function(
                name=memberdef.findtext("name", default=""),
                return_type="".join(memberdef.find("type").itertext()).strip(),
                brief=brief,
                detailed=detailed,
                file=Path(filepath).name,
                params=params
            )

            functions.append(function)

        return functions

    def _parse_structs(self, cpdef) -> list:
        """parses structs"""
        structs = []

        memberdefs = cpdef.findall(".//innerclass")
        for memberdef in memberdefs:
            root = ET.parse(self.xml_dir / f"{memberdef.get("refid")}.xml").getroot()
            cpdef = root.find("compounddef")
            
            element = cpdef.find("briefdescription")
            brief = self._xml_tags_to_html(element)
            
            element = cpdef.find("detaileddescription")
            detailed = brief + self._xml_tags_to_html(element)

            fields = []
            for field in cpdef.findall(".//memberdef"):
                argsstring = field.find("argsstring").text
                type = self._xml_tags_to_html(field.find("type"))
                name = field.find("name").text

                desc = self._xml_tags_to_html(field.find("detaileddescription"))
                fields.append(Field(type=type, name=name, argsstring=argsstring, desc=desc))

            struct = Struct(
                name=cpdef.find("compoundname").text,
                brief=brief,
                detailed=detailed,
                file=cpdef.find("includes").text,
                fields=fields
            )
            
            structs.append(struct)

        return structs

    def _parse_typedefs(self, cpdef) -> list:
        """parses typedefs"""
        typedefs = []

        memberdefs = cpdef.findall(".//memberdef[@kind='typedef']")
        for memberdef in memberdefs:
            element = memberdef.find("briefdescription")
            brief = self._xml_tags_to_html(element)

            element = memberdef.find("detaileddescription")
            detailed = brief + self._xml_tags_to_html(element)

            typedef = Typedef(
                name=memberdef.findtext("name"),
                type=memberdef.findtext("type"),
                brief=brief,
                detailed=detailed
            )

            typedefs.append(typedef)

        return typedefs

    def _parse_constants(self, cpdef) -> list:
        """parses constants"""
        constants = []

        memberdefs = cpdef.findall(".//memberdef[@kind='define']")
        for memberdef in memberdefs:
            name = memberdef.find("name").text
            value = memberdef.find("initializer").text

            element = memberdef.find("detaileddescription")
            desc = self._xml_tags_to_html(element)
            constants.append(Constant(name=name, value=value, desc=desc))

        return constants

    def _parse_constant_groups(self, cpdef) -> list:
        """parses constant groups"""
        constant_groups = []

        memberdefs = cpdef.findall(".//innergroup")
        for memberdef in memberdefs:
            root = ET.parse(self.xml_dir / f"{memberdef.get("refid")}.xml").getroot()
            tmp_cpdef = root.find("compounddef")

            name = tmp_cpdef.findtext("title")
            constants = self._parse_constants(tmp_cpdef)
            constants.append(ConstantGroup(name=name, constants=constants))

        return constant_groups
    

def main():
    parser = XMLParser()
    modules = parser.parse_modules()

    # for module in modules:
    #     print(module.detailed)

        # for func in module.functions:
        #     #print(func.brief)

            # argstring = "("
            # for index, param in enumerate(func.params):
            #     if index > 0 and index < len(func.params):
            #         argstring += ", "

            #     argstring += param.type + param.name
                
            # argstring += ")"
            # print(f"  {func.return_type} {func.name}{argstring}")


if __name__ == "__main__":
    main()
