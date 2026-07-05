#!/usr/bin/env python3
"""Render Markdown docs into MkDocs Markdown.

  * Converts GitHub alerts (``> [!NOTE]``) to MkDocs admonitions
  * Inserts a blank line before lists/tables ( required by python-markdown)
  * Turns links to repo files outside ``docs/`` into GitHub URLs:
    * ``/blob`` for files
    * ``/tree`` for directories

Usage:
  render_docs.py --docs docs --root . \
      --repo-url https://github.com/USER/REPO --repo-branch main
"""

from __future__ import annotations

import argparse
import os
import re
import sys

FENCE_RE = re.compile(r"^(\s*)(`{3,}|~{3,})(.*)$")
HEADING_RE = re.compile(r"^(#{1,6})(?:\s+(.*?))?\s*$")
ALERT_RE = re.compile(r"^(\s*)>\s*\[!(NOTE|TIP|IMPORTANT|WARNING|CAUTION)\]\s*$", re.IGNORECASE)
LIST_ITEM_RE = re.compile(r"^\s*([-*+]|\d+\.)\s")
TABLE_ROW_RE = re.compile(r"^\s*\|.*\|\s*$")
LINK_RE = re.compile(r"(!?)\[([^\]]*)\]\(\s*(<[^>]*>|[^)\s]+)((?:\s+\"[^\"]*\")?)\s*\)")

# GitHub alert type -> MkDocs Material admonitions.
ALERT_MAP = {
    "note": "note",
    "tip": "tip",
    "important": "info",
    "warning": "warning",
    "caution": "danger",
}


def iter_fence_state(lines: list[str]):
    """Yield (index, line, in_code) tracking fenced code blocks."""
    fence = None
    for i, line in enumerate(lines):
        m = FENCE_RE.match(line)
        if m:
            char, length, info = m.group(2)[0], len(m.group(2)), m.group(3).strip()
            if fence is None:
                fence = (char, length)
                yield i, line, True
                continue
            if char == fence[0] and length >= fence[1] and info == "":
                yield i, line, True
                fence = None
                continue
            yield i, line, fence is not None
            continue
        yield i, line, fence is not None


def convert_alerts(lines: list[str]) -> list[str]:
    out: list[str] = []
    states = list(iter_fence_state(lines))
    n = len(states)
    i = 0
    while i < n:
        _, line, in_code = states[i]
        if not in_code:
            m = ALERT_RE.match(line)
            if m:
                indent = m.group(1)
                kind = ALERT_MAP.get(m.group(2).lower(), "note")
                out.append(f"{indent}!!! {kind}")
                i += 1
                while i < n:
                    _, bline, b_in_code = states[i]
                    if b_in_code:
                        break
                    bm = re.match(r"^(\s*)>\s?(.*)$", bline)
                    if bm is None:
                        break
                    out.append(f"{indent}    {bm.group(2)}")
                    i += 1
                continue
        out.append(line)
        i += 1
    return out


def normalize_blank_lines(lines: list[str]) -> list[str]:
    out: list[str] = []
    in_code = False
    for line in lines:
        is_fence = bool(FENCE_RE.match(line))
        if not in_code and not is_fence:
            starts_list = bool(LIST_ITEM_RE.match(line))
            starts_table = bool(TABLE_ROW_RE.match(line))
            if starts_list or starts_table:
                prev = out[-1] if out else ""
                prev_blank = prev.strip() == ""
                hm = HEADING_RE.match(prev)
                prev_heading = bool(hm and hm.group(2) is not None)
                same_list = starts_list and bool(LIST_ITEM_RE.match(prev))
                same_table = starts_table and bool(TABLE_ROW_RE.match(prev))
                if prev and not prev_blank and not prev_heading and not same_list and not same_table:
                    out.append("")
        out.append(line)
        if is_fence:
            in_code = not in_code
    return out


def rewrite_links(text: str, file_relpath: str, docs_dir: str, root: str,
                  repo_url: str | None, repo_branch: str) -> str:
    """Repo files outside docs -> GitHub URLs, in-docs links stay relative."""
    file_dir = os.path.dirname(file_relpath)
    docs_prefix = docs_dir.rstrip("/") + "/"

    def repl(m: re.Match) -> str:
        bang, label, raw, title = m.group(1), m.group(2), m.group(3), m.group(4)
        angle = raw.startswith("<") and raw.endswith(">")
        url = raw[1:-1] if angle else raw
        if (
            url == ""
            or url.startswith(("http://", "https://", "//", "#", "/", "mailto:", "tel:", "("))
            or re.match(r"^[a-zA-Z][a-zA-Z0-9+.\-]*:", url)
        ):
            return m.group(0)
        frag = ""
        if "#" in url:
            url, frag = url.split("#", 1)
            frag = "#" + frag
        if url == "":
            return m.group(0)
        resolved = os.path.normpath(os.path.join(file_dir, url)).replace(os.sep, "/")
        if resolved == docs_dir or resolved.startswith(docs_prefix):
            return m.group(0)
        if not repo_url:
            return m.group(0)
        kind = "tree" if os.path.isdir(os.path.join(root, resolved)) else "blob"
        new_url = f"{repo_url.rstrip('/')}/{kind}/{repo_branch}/{resolved}{frag}"
        if angle:
            new_url = f"<{new_url}>"
        return f"{bang}[{label}]({new_url}{title})"

    return LINK_RE.sub(repl, text)


def main(argv: list[str]) -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--docs", default="docs", help="docs directory (default: docs)")
    ap.add_argument("--root", default=".", help="repository root (default: .)")
    ap.add_argument("--repo-url", default=None, help="repo base URL, e.g. https://github.com/user/repo")
    ap.add_argument("--repo-branch", default="main", help="branch for source links (default: main)")
    ap.add_argument("--dry-run", action="store_true", help="print actions without writing")
    args = ap.parse_args(argv)

    base = os.path.join(args.root, args.docs)
    for dirpath, _dirs, files in os.walk(base):
        for name in sorted(files):
            if not name.endswith(".md"):
                continue
            path = os.path.join(dirpath, name)
            rel = os.path.relpath(path, args.root).replace(os.sep, "/")
            lines = open(path, encoding="utf-8").read().split("\n")
            lines = convert_alerts(lines)
            lines = normalize_blank_lines(lines)
            body = rewrite_links("\n".join(lines), rel, args.docs, args.root, args.repo_url, args.repo_branch)
            print(f"render {rel}")
            if not args.dry_run:
                with open(path, "w", encoding="utf-8") as fh:
                    fh.write(body.rstrip() + "\n")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
