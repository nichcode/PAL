#!/usr/bin/env python3

import sys
from pathlib import Path
import re

from xml.etree import ElementTree as ET
from dataclasses import dataclass


@dataclass
class Param:
    type_str: str
    name_str: str


@dataclass
class Function:
    name: str
    return_type: str
    brief: str
    detailed: str
    file: str
    params: list[Param]


@dataclass
class Module:
    name: str
    brief: str
    detailed: str
    functions: list[Function]


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
            functions = self._parse_function(cpdef)

            module = Module(
                name=cpdef.findtext("title"),
                brief=data["brief"],
                detailed=data["detailed"],
                functions=functions
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

    def _parse_function(self, cpdef) -> list:
        """parses function"""
        tmp = []
        functions = []

        for element in cpdef.findall(".//memberdef"):
            if element.get("kind") == "function":
                tmp.append(element.get("id"))

        for refid in tmp:
            memberdef = cpdef.find(f".//memberdef[@id='{refid}']")
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
                params.append(Param(type_str=type, name_str=name))

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

            #     argstring += param.type_str + param.name_str
                
            # argstring += ")"
            # print(f"  {func.return_type} {func.name}{argstring}")


if __name__ == "__main__":
    main()
