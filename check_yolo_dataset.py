#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
YOLO 空白标签补全（递归版）
- 递归扫描 datasets/train/images 和 datasets/val/images 下所有图片
- 保持子目录结构，在对应 labels 目录生成空 txt
- 绝不覆盖已有 txt
"""
from __future__ import annotations
import os
import sys
from pathlib import Path

# -------------------------------------------------
BASE_DIR      = Path(__file__).resolve().parent   # 脚本所在目录
IMG_DIR_TRAIN = BASE_DIR / 'datasets' / 'train' / 'images'
LBL_DIR_TRAIN = BASE_DIR / 'datasets' / 'train' / 'labels'
IMG_DIR_VAL   = BASE_DIR / 'datasets' / 'val'   / 'images'
LBL_DIR_VAL   = BASE_DIR / 'datasets' / 'val'   / 'labels'

IMG_SUFFIX = {'.jpg', '.jpeg', '.png', '.bmp', '.webp'}
# -------------------------------------------------


def collect_img_stems(root: Path) -> set[str]:
    """递归收集所有图片的**相对 stem**（相对于 root）"""
    if not root.is_dir():
        return set()
    return {str(p.relative_to(root).with_suffix(''))
            for p in root.rglob('*')
            if p.suffix.lower() in IMG_SUFFIX}


def ensure_empty_txt(txt_path: Path) -> bool:
    """创建空 txt，若已存在则跳过；返回是否创建"""
    txt_path.parent.mkdir(parents=True, exist_ok=True)
    if not txt_path.exists():
        txt_path.write_text('', encoding='utf-8')
        return True
    return False


def process(phase: str, img_root: Path, lbl_root: Path):
    img_stems = collect_img_stems(img_root)
    if not img_stems:
        print(f'{phase:>5} | 未找到任何图片，跳过')
        return

    # 已存在 txt 的相对 stem
    exist_stems = {str(p.relative_to(lbl_root).with_suffix(''))
                   for p in lbl_root.rglob('*.txt')} if lbl_root.is_dir() else set()

    missing = img_stems - exist_stems
    extra   = exist_stems - img_stems

    created = 0
    for stem in missing:
        txt_path = (lbl_root / stem).with_suffix('.txt')
        created += int(ensure_empty_txt(txt_path))

    print(f'{phase:>5} | 图片:{len(img_stems):>5}  标注:{len(exist_stems):>5}  '
          f'新增空白txt:{created:>5}  多余txt:{len(extra):>5}')


def main():
    for d in (IMG_DIR_TRAIN, IMG_DIR_VAL):
        if not d.is_dir():
            sys.exit(f'ERROR: 图片目录不存在 -> {d}')
    process('train', IMG_DIR_TRAIN, LBL_DIR_TRAIN)
    process('val',   IMG_DIR_VAL,   LBL_DIR_VAL)
    print('\n===== 全部完成，未覆盖任何已有 txt =====')


if __name__ == '__main__':
    main()