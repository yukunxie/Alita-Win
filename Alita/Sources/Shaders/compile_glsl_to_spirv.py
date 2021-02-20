# -*- coding: utf-8 -*-

import os

def getAllFiles(root_dir, suffix = []):
    ret = []
    for root, dirs, files in os.walk(root_dir):
        if root.find(".svn") >= 0:
            continue
        for file in files:
            if not suffix:
                ret.append(os.path.join(root,file))
            else:
                base, suf = os.path.splitext(file)
                if suf in suffix:
                    ret.append(os.path.join(root,file))
    return ret

if __name__ == "__main__":
    path = os.path.dirname(__file__)
    if not path:
        path = "./"
    filenames = getAllFiles(path, [".vert", ".frag"])
    for filename in filenames:
        cmd = "glslangValidator -H -V --iy %s -o %s.spv" % (filename, filename)
        print (cmd)
        os.system(cmd)
