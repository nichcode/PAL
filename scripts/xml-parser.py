#!/usr/bin/env python3

from pathlib import Path
from xml.etree import ElementTree as ET
import pickle
from models import *

import html


class XMLParser:
    def __init__(self, root_dir) -> None:
        self.xml_dir = root_dir / "build/xml"

    def parse_modules(self) -> list[Module]:
        """parses modules"""
        modules = []

        refids = self._find_modules_refids()
        for refid in refids:
            root = ET.parse(self.xml_dir / f"{refid}.xml").getroot()
            cpdef = root.find("compounddef")

            # compute module file id
            cpname = cpdef.findtext("compoundname")
            tmp = cpname.replace('_', '__').replace('.', '__')
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
                refid=refid,
                functions=functions,
                structs=structs,
                typedefs=typedefs,
                constants=constants,
                constant_groups=constant_groups
            )

            modules.append(module)

        print("modules parsed successfully")
        return modules
    
    def parse_pages(self) -> list[Page]:
        """parses pages"""
        refids = []
        pages = []

        root = ET.parse(self.xml_dir / "index.xml").getroot()
        for cp in root.iterfind("compound"):
            if cp.get("kind") == "page":
                refids.append(cp.get("refid"))

        for refid in refids:
            root = ET.parse(self.xml_dir / f"{refid}.xml").getroot()
            cpdef = root.find("compounddef")
            name = cpdef.findtext("title")
            
            element = cpdef.find("detaileddescription")
            body = self._xml_tags_to_html(element)
            pages.append(Page(name=name, body=body, refid=refid))

        return pages

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
                dir_badge = f"<span>{html.escape(direction)}</span> "

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
                open_tag, close_tag = (f'<div-{html.escape(kind)}">', "</div>")
            
        elif element.tag == "ulink":
            url = element.get("url", "#")
            open_tag = f'<a href="{html.escape(url)}" target="_blank" rel="noopener noreferrer">'
            close_tag = "</a>"

        elif element.tag == "ref":
            refid = element.get("refid", "#")
            open_tag = f'<a href="{html.escape(refid)}.html">'
            close_tag = "</a>"

        html_out += open_tag

        if element.text:
            html_out += html.escape(element.text)

        for child in element:
            html_out += self._xml_tags_to_html(child)
            if child.tail:
                html_out += html.escape(child.tail)

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
                element = param.find("type")
                type = self._xml_tags_to_html(element)

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
            root = ET.parse(self.xml_dir / f"{memberdef.get('refid')}.xml").getroot()
            cpdef = root.find("compounddef")
            
            element = cpdef.find("briefdescription")
            brief = self._xml_tags_to_html(element)
            
            element = cpdef.find("detaileddescription")
            detailed = brief + self._xml_tags_to_html(element)

            fields = []
            for field in cpdef.findall(".//memberdef"):
                argsstring = field.findtext("argsstring")
                type = self._xml_tags_to_html(field.find("type"))
                name = field.findtext("name")

                desc = self._xml_tags_to_html(field.find("detaileddescription"))
                fields.append(Field(type=type, name=name, argsstring=argsstring, desc=desc))

            struct = Struct(
                name=cpdef.findtext("compoundname"),
                brief=brief,
                detailed=detailed,
                file=cpdef.findtext("includes"),
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
            name = memberdef.findtext("name")
            value = memberdef.findtext("initializer")

            element = memberdef.find("detaileddescription")
            desc = self._xml_tags_to_html(element)
            constants.append(Constant(name=name, value=value, desc=desc))

        return constants

    def _parse_constant_groups(self, cpdef) -> list:
        """parses constant groups"""
        constant_groups = []

        memberdefs = cpdef.findall(".//innergroup")
        for memberdef in memberdefs:
            root = ET.parse(self.xml_dir / f"{memberdef.get('refid')}.xml").getroot()
            tmp_cpdef = root.find("compounddef")

            name = tmp_cpdef.findtext("title")
            constants = self._parse_constants(tmp_cpdef)
            constant_groups.append(ConstantGroup(name=name, constants=constants))

        return constant_groups
    

def main():
    root_dir = Path(__file__).resolve().parents[1]
    parser = XMLParser(root_dir)
    modules = parser.parse_modules()
    pages = parser.parse_pages()

    filepath = root_dir / "build/modules.pkl"
    with open(filepath, "wb") as file:
        pickle.dump(modules, file, protocol=pickle.HIGHEST_PROTOCOL)

    print(f"modules cached successfully to {filepath}")

    filepath = root_dir / "build/pages.pkl"
    with open(filepath, "wb") as file:
        pickle.dump(pages, file, protocol=pickle.HIGHEST_PROTOCOL)

    print(f"pages cached successfully to {filepath}")

if __name__ == "__main__":
    main()
