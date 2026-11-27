#!/usr/bin/env python3
"""Rebuild viewer_html.h from viewer.html (run after editing the page):
    python3 gen_viewer_html.py
"""
from pathlib import Path

here = Path(__file__).resolve().parent
html = (here / "viewer.html").read_text(encoding="utf-8")
assert ')HTML"' not in html
(here / "viewer_html.h").write_text(
    "/* Generated from viewer.html by gen_viewer_html.py. Do not edit. */\n"
    "#pragma once\n#include <pgmspace.h>\n"
    'static const char VIEWER_HTML[] PROGMEM = R"HTML(' + html + ')HTML";\n',
    encoding="utf-8")
print("wrote viewer_html.h")
