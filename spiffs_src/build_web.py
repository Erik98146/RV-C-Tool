#!/usr/bin/env python3
"""Build the RV-C Tools web assets: spiffs_src/ -> spiffs/.

Simplified from hub_core's _optimize.py: gzip only, no esbuild/minify (these
pages are small — rvc-tools.html gzips to ~20 KB unminified — and this keeps
the standalone project free of a node_modules dependency).

Steps:
  1. Bump the build number in ../version.txt (vMAJOR.MINOR.BUILD).
  2. Stamp /common.css?v=<build> into each page (cache-buster: common.css.gz
     is served immutable, so its URL must change when its content can).
  3. Gzip rvc-tools.html, network.html, common.css into ../spiffs/.
  4. Write ../spiffs/webui-version.json (the ETag source — see rt_web.c).

Run from anywhere:  python firmware/rvc_tools/spiffs_src/build_web.py
Then rebuild/flash — SPIFFS is flashed with the app (FLASH_IN_PROJECT).
"""
import gzip
import json
import re
import sys
from datetime import datetime, timezone
from pathlib import Path

SRC = Path(__file__).resolve().parent
ROOT = SRC.parent
OUT = ROOT / "spiffs"

PAGES = ["rvc-tools.html", "network.html"]


def bump_version() -> str:
    vfile = ROOT / "version.txt"
    text = vfile.read_text(encoding="utf-8").strip()
    m = re.fullmatch(r"v(\d+)\.(\d+)\.(\d+)", text)
    if not m:
        sys.exit(f"version.txt does not look like vMAJOR.MINOR.BUILD: {text!r}")
    major, minor, build = int(m.group(1)), int(m.group(2)), int(m.group(3)) + 1
    version = f"v{major}.{minor}.{build}"
    vfile.write_text(version + "\n", encoding="utf-8")
    return version


def gzip_bytes(data: bytes, dest: Path) -> None:
    # mtime=0 keeps the output deterministic for identical input.
    dest.write_bytes(gzip.compress(data, 9, mtime=0))


def main() -> None:
    OUT.mkdir(exist_ok=True)
    version = bump_version()
    print(f"version: {version}")

    for name in PAGES:
        src = SRC / name
        html = src.read_text(encoding="utf-8")
        stamped = html.replace('href="/common.css"', f'href="/common.css?v={version}"')
        if stamped == html:
            sys.exit(f"{name}: expected a /common.css link to stamp — none found")
        data = stamped.encode("utf-8")
        dest = OUT / f"{name}.gz"
        gzip_bytes(data, dest)
        print(f"{name}: {len(data):,} -> {dest.stat().st_size:,} bytes ({dest.name})")

    css = (SRC / "common.css").read_bytes()
    dest = OUT / "common.css.gz"
    gzip_bytes(css, dest)
    print(f"common.css: {len(css):,} -> {dest.stat().st_size:,} bytes")

    meta = {
        "version": version,
        "packaged": datetime.now(timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ"),
    }
    (OUT / "webui-version.json").write_text(json.dumps(meta) + "\n", encoding="utf-8")
    print(f"webui-version.json: {meta}")

    font = OUT / "inter.woff2"
    if not font.exists():
        print("WARNING: spiffs/inter.woff2 is missing — pages will fall back to system fonts")


if __name__ == "__main__":
    main()
