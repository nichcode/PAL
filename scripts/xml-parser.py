#!/usr/bin/env python3

import html
from pathlib import Path
from xml.etree import ElementTree as ET
from models import *


class XMLParser:
    def __init__(self, root_dir):
        self.build_dir = root_dir / "build"
        self.xml_dir = self.build_dir / "xml"
        self.root = ET.parse(self.xml_dir / "index.xml").getroot()

    def parse(self):
        """Parses pages and modules"""

        self._parse_pages()
        self._parse_modules()

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
                open_tag, close_tag = (
                    "<div><strong>Since</strong> ", "</div>")
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

    def _parse_pages(self):
        """Parses XML pages"""

        pages = []
        for cp in self.root.iterfind("compound"):
            if cp.get("kind") == "page":
                refid = cp.get("refid")
                root = ET.parse(self.xml_dir / f"{refid}.xml").getroot()
                cpdef = root.find("compounddef")

                name = cpdef.findtext("title")
                brief = self._xml_tags_to_html(cpdef.find("briefdescription"))
                detailed = brief + \
                    self._xml_tags_to_html(cpdef.find("detaileddescription"))

                page = Page(name, brief, detailed, refid)
                pages.append(page)

        save_cache(self.build_dir / "pages.pkl", pages)

    def _parse_modules(self):
        """Parses XML modules"""

        modules = []
        refids = self._find_modules()

        for refid in refids:
            root = ET.parse(self.xml_dir / f"{refid}.xml").getroot()
            cpdef = root.find("compounddef")
            name = cpdef.findtext("title")

            brief = self._xml_tags_to_html(cpdef.find("briefdescription"))
            detailed = brief + \
                self._xml_tags_to_html(cpdef.find("detaileddescription"))

            functions = self._parse_functions(cpdef)
            structs = self._parse_structs(cpdef)
            typedefs = self._parse_typedefs(cpdef)
            constants = self._parse_constants(cpdef)
            constant_groups = self._parse_constant_groups(cpdef)

            # compute file refid
            cpname = cpdef.findtext("compoundname")
            tmp = cpname.replace('_', '__').replace('.', '__')

            root = ET.parse(self.xml_dir / f"{tmp}_8h.xml").getroot()
            cpdef = root.find("compounddef")
            license_str = self._xml_tags_to_html(
                cpdef.find("detaileddescription"))

            module = Module(
                name,
                brief,
                detailed,
                license_str,
                refid,
                f"{cpname}.h",
                functions,
                structs,
                typedefs,
                constants,
                constant_groups)

            modules.append(module)
            save_cache(self.build_dir / "modules.pkl", modules)

    def _find_modules(self):
        """Find modules"""

        modules = []
        children = []

        for cp in self.root.iterfind("compound"):
            if cp.get("kind") == "group":
                refid = cp.get("refid")
                root = ET.parse(self.xml_dir / f"{refid}.xml").getroot()
                cpdef = root.find("compounddef")

                # find nested sub-groups
                for inner in cpdef.findall("innergroup"):
                    group_refid = inner.get("refid")
                    children.append(group_refid)

                # filter modules
                if refid not in children:
                    modules.append(refid)

        return modules

    def _parse_functions(self, cpdef) -> list:
        """Parses module functions"""

        functions = []
        memberdefs = cpdef.findall(".//memberdef[@kind='function']")

        for memberdef in memberdefs:
            brief = self._xml_tags_to_html(memberdef.find("briefdescription"))
            detailed = brief + \
                self._xml_tags_to_html(memberdef.find("detaileddescription"))
            file = memberdef.find("location").get("file")

            params = []
            for param in memberdef.findall("param"):
                type = self._xml_tags_to_html(param.find("type"))
                name = param.findtext("declname", default="").strip()

                parameter = Param(type, name)
                params.append(parameter)

            function = Function(
                memberdef.findtext("name", default=""),
                brief,
                detailed,
                memberdef.get("id"),
                Path(file).name,
                "".join(memberdef.find("type").itertext()).strip(),
                params)

            functions.append(function)

        return functions

    def _parse_structs(self, cpdef) -> list:
        """Parses module structs"""

        structs = []
        memberdefs = cpdef.findall(".//innerclass")

        for memberdef in memberdefs:
            refid = memberdef.get('refid')
            root = ET.parse(self.xml_dir / f"{refid}.xml").getroot()
            tmp_cpdef = root.find("compounddef")

            brief = self._xml_tags_to_html(tmp_cpdef.find("briefdescription"))
            detailed = brief + \
                self._xml_tags_to_html(tmp_cpdef.find("detaileddescription"))

            fields = []
            for f in tmp_cpdef.findall(".//memberdef"):
                name = f.findtext("name")
                type = self._xml_tags_to_html(f.find("type"))
                argsstring = f.findtext("argsstring")

                fbrief = self._xml_tags_to_html(f.find("briefdescription"))
                fdetailed = fbrief + \
                    self._xml_tags_to_html(f.find("detaileddescription"))

                field = Field(name, fbrief, fdetailed, type, argsstring)
                fields.append(field)

            struct = Struct(
                tmp_cpdef.findtext("compoundname"),
                brief,
                detailed,
                refid,
                tmp_cpdef.findtext("includes"),
                fields)

            structs.append(struct)

        return structs

    def _parse_typedefs(self, cpdef) -> list:
        """Parses module typedefs"""

        typedefs = []
        memberdefs = cpdef.findall(".//memberdef[@kind='typedef']")

        for memberdef in memberdefs:
            brief = self._xml_tags_to_html(memberdef.find("briefdescription"))
            detailed = brief + \
                self._xml_tags_to_html(memberdef.find("detaileddescription"))
            file = memberdef.find("location").get("file")

            typedef = Typedef(
                memberdef.findtext("name"),
                brief,
                detailed,
                memberdef.get("id"),
                Path(file).name,
                memberdef.findtext("type"))

            typedefs.append(typedef)

        return typedefs

    def _parse_constants(self, cpdef) -> list:
        """Parses module constants"""

        constants = []

        memberdefs = cpdef.findall(".//memberdef[@kind='define']")
        for memberdef in memberdefs:
            brief = self._xml_tags_to_html(memberdef.find("briefdescription"))
            detailed = brief + \
                self._xml_tags_to_html(memberdef.find("detaileddescription"))
            file = memberdef.find("location").get("file")

            constant = Constant(
                memberdef.findtext("name"),
                brief,
                detailed,
                memberdef.get("id"),
                Path(file).name,
                memberdef.findtext("initializer"))

            constants.append(constant)

        return constants

    def _parse_constant_groups(self, cpdef) -> list:
        """Parses module constants groups"""

        groups = []
        memberdefs = cpdef.findall(".//innergroup")

        for memberdef in memberdefs:
            refid = memberdef.get('refid')
            root = ET.parse(self.xml_dir / f"{refid}.xml").getroot()
            tmp_cpdef = root.find("compounddef")

            brief = self._xml_tags_to_html(tmp_cpdef.find("briefdescription"))
            detailed = brief + \
                self._xml_tags_to_html(tmp_cpdef.find("detaileddescription"))
            constants = self._parse_constants(tmp_cpdef)

            group = ConstantGroup(
                tmp_cpdef.findtext("title"),
                brief,
                detailed,
                refid,
                constants)

            groups.append(group)

        return groups


def main():
    root_dir = Path(__file__).resolve().parents[1]
    parser = XMLParser(root_dir)
    parser.parse()

    print(f"xml files parsed successfully")


if __name__ == "__main__":
    main()
