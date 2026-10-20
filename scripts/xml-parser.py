#!/usr/bin/env python3

import sys
from pathlib import Path
import re

from xml.etree import ElementTree as ET
from dataclasses import dataclass

@dataclass
class Module:
    name: str
    brief: str
    detailed: str


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

            # get the first location element and find its filepath
            location = cpdef.find(".//location")
            filepath = location.get("file")
            filename = Path(filepath).name

            # compute the file id
            base, ext = filename.rsplit('.', 1)
            tmp = base.replace('_', '__').replace('.', '__')
            file_refid = f"{tmp}_8{ext}"

            data = self._parse_module_file(file_refid)

            module = Module(
                name=cpdef.findtext("title"),
                brief=data["brief"],
                detailed=data["detailed"]
            )

            modules.append(module)

        print("modules parsed successfully")
        return modules

    def _find_modules_refids(self) -> list:
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
        data["detailed"] = self._xml_tags_to_html(element)

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
            "programlisting": ("<pre><code>", "</code></pre>"),
            "codeline": ("", "\n"),
            "highlight": ("", ""),
            
            "table": ("<table>", "</table>"),
            "row": ("<tr>", "</tr>"),
            "entry": ("<td>", "</td>"),
            "tablecols": ("", ""),
            
            "itemizedlist": ("<ul>", "</ul>"),
            "orderedlist": ("<ol>", "</ol>"),
            "listitem": ("<li>", "</li>"),
        }

        if element.tag == "ulink":
            url = element.get("url", "#")
            open_tag = f'<a href="{url}" target="_blank" rel="noopener noreferrer">'
            close_tag = "</a>"
        else:
            open_tag, close_tag = tag_map.get(element.tag, ("", ""))
        
        if element.tag == "entry" and element.get("thead") == "yes":
            open_tag, close_tag = ("<th>", "</th>")
            
        if element.tag == "simplesect":
            kind = element.get("kind", "note")
            open_tag, close_tag = (f"<div class='doxygen-callout callout-{kind}'>", "</div>")
        
        html_out += open_tag
        if element.text:
            html_out += element.text
        
        for child in element:
            html_out += self._xml_tags_to_html(child)
            if child.tail:
                html_out += child.tail
            
        html_out += close_tag
        return html_out

def main():
    parser = XMLParser()
    modules = parser.parse_modules()

    for module in modules:
        print(module.name)
        print(f"  {module.brief}")
        print(f"  {module.detailed}")

if __name__ == "__main__":
    main()