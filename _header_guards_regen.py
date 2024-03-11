#!/bin/python3

# this script home is here: https:://github.com/AnimusPEXUS/ccutils

import os.path
import datetime


def edit_file(fullpath_first, fullpath):

    lns = []

    relp = os.path.relpath(fullpath, fullpath_first)
    print("editing: {}".format(relp))

    with open(fullpath, 'r') as f:
        lns = f.readlines()

    for i in range(len(lns)-1, -1, -1):
        lns[i] = lns[i].rstrip()

    if len(lns) > 0:
        if lns[0].startswith('#ifndef'):
            del lns[0]
        else:
            print(f"skipping 1 {relp}")
            return

    if len(lns) > 0:
        if lns[0].startswith('#define'):
            del lns[0]
        else:
            print(f"skipping 2 {relp}")
            return

    while len(lns) > 0:
        if lns[-1].isspace():
            del lns[-1]
        else:
            break

    if len(lns) > 0:
        if lns[-1].startswith('#endif'):
            del lns[-1]
        else:
            print(f"skipping 4 {relp}: {lns[-1]}")
            return

    a = datetime.datetime.now(datetime.UTC)

    gened_name = 'WAYROUND_I2P_{:04d}{:02d}{:02d}_{:02d}{:02d}{:02d}_{:d}'.format(
        a.year, a.month, a.day, a.hour, a.minute, a.second, a.microsecond
    )

    lns.insert(0, "#define {}".format(gened_name))
    lns.insert(0, "#ifndef {}".format(gened_name))
    lns.append("#endif")
    lns.append("")

    with open(fullpath, 'w') as f:
        f.write("\n".join(lns))


def walk_dir(fullpath_first, fullpath):

    relp = os.path.relpath(fullpath, fullpath_first)
    print("working in: {}".format(relp))

    names = os.listdir(fullpath)

    for i in names:
        joined = os.path.join(fullpath, i)

        if os.path.islink(joined):
            continue

        if os.path.isdir(joined):

            if i in ['.git', 'build']:
                continue

            walk_dir(fullpath_first, joined)
            continue

        if i.endswith(".hpp"):
            edit_file(fullpath_first, joined)


def main():
    a = os.path.dirname(__file__)
    a = os.path.abspath(a)
    print(f"walking {a} dirs and replacing .hpp headers")
    walk_dir(a, a)


if __name__ == '__main__':
    main()
