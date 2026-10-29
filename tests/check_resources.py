"""Check Qt resources and local includes on case-sensitive filesystems."""
from pathlib import Path
import re
import xml.etree.ElementTree as ET

root = Path(__file__).resolve().parent.parent
qrc = root / 'Dataset/Image/resources.qrc'
for entry in ET.parse(qrc).iter('file'):
    resource = qrc.parent / entry.text
    assert resource.is_file(), f'Missing Qt resource: {entry.text}'
files = {p.name for p in root.iterdir() if p.is_file()}
for source in root.iterdir():
    if source.suffix not in ('.h', '.cpp'):
        continue
    for include in re.findall(r'#include\s+"([^\"]+)"', source.read_text(encoding='utf-8')):
        if include.startswith('ui_'):
            continue  # qmake-generated header in the original unused window
        assert include in files, f'{source.name}: wrong-case or missing include {include}'
print('All Qt resources and local include names resolve')
