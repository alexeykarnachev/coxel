import re
from pathlib import Path
from argparse import ArgumentParser
from collections import namedtuple

import cv2
import matplotlib.pyplot as plt
import numpy as np

Glyph = namedtuple(
    "Glyph",
    ("x", "y", "width", "height", "xoffset", "yoffset", "xadvance", "page", "chnl"),
)


def parse_args():
    parser = ArgumentParser(
        description="Converts font glyphs png sheet into raw pixels binary file."
    )
    parser.add_argument(
        "--glyphs-file-path", "-g",
        type=str,
        required=True,
        help="Path to the png file with font glyphs.",
    )
    parser.add_argument(
        "--fnt-file-path", "-f",
        type=str,
        required=True,
        help="Path to the fnt file which describes the font.",
    )
    parser.add_argument(
        "--out-file-path", "-o",
        type=str,
        required=True,
        help="Path to the output rasterized font binary file.",
    )

    return parser.parse_args()


def read_texture(file_path):
    return cv2.imread(file_path, cv2.IMREAD_UNCHANGED)[..., -1]


def read_font(file_path):
    font = {}
    with open(file_path) as f:
        for line in f:
            if line.startswith("char id="):
                id_, *numbers = [int(x) for x in re.findall("\d+", line)]
                font[id_] = Glyph(*numbers)
    return font


def extract_glyph(texture, glyph):
    return texture[glyph.y : glyph.y + glyph.height, glyph.x : glyph.x + glyph.width]


def main(glyphs_file_path, fnt_file_path, out_file_path):
    args = parse_args()
    texture = read_texture(glyphs_file_path)
    font = read_font(fnt_file_path)

    glyphs = [font[i] for i in range(32, 126)]
    xadvance = set(g.xadvance for g in glyphs)
    if len(xadvance) != 1:
        raise ValueError("Can process only monospace fonts")
    xadvance = xadvance.pop()

    screen_width = len(glyphs) * xadvance
    screen_height = max(g.height + g.yoffset for g in glyphs)
    screen = np.zeros((screen_height, screen_width), dtype=np.uint8)

    text = []
    x_offset = 0
    for glyph in glyphs:
        glyph_img = extract_glyph(texture, glyph)
        screen[
            0 + glyph.yoffset : 0 + glyph.height + glyph.yoffset,
            x_offset + glyph.xoffset : x_offset + glyph.width + glyph.xoffset,
        ] = glyph_img
        x_offset += glyph.xadvance

    constants = f"""
#define GUI_FONT_TEXTURE_WIDTH {int(screen_width)}
#define GUI_FONT_TEXTURE_HEIGHT {int(screen_height)}
#define GUI_FONT_GLYPH_WIDTH {int(xadvance)}
    """
    print(constants)


    with open(out_file_path, "wb") as f:
        f.write(screen.tobytes())

    plt.imshow(screen)
    plt.show()


if __name__ == "__main__":
    args = parse_args()
    main(
        glyphs_file_path=args.glyphs_file_path,
        fnt_file_path=args.fnt_file_path,
        out_file_path=args.out_file_path,
    )
