#!/usr/bin/env python3

#
# SG
# shdc.py
#
# Copyright © 2021 Gustavo C. Viegas.
#

import subprocess

dPbrsg        = 1 << 0
dUnlit        = 1 << 1
dColorMap     = 1 << 4
dPbrMap       = 1 << 5
dNormalMap    = 1 << 6
dOcclusionMap = 1 << 7
dEmissiveMap  = 1 << 8
dAlphaBlend   = 1 << 12
dAlphaMask    = 1 << 13
dNormal       = 1 << 14
dTangent      = 1 << 15
dTexCoord0    = 1 << 16
dTexCoord1    = 1 << 17
dColor0       = 1 << 18
dSkin         = 1 << 19

optDefs = {
    dPbrsg        : 'MATERIAL_PBRSG',
    dUnlit        : 'MATERIAL_UNLIT',
    dColorMap     : 'HAS_COLOR_MAP',
    dPbrMap       : 'HAS_PBR_MAP',
    dNormalMap    : 'HAS_NORMAL_MAP',
    dOcclusionMap : 'HAS_OCCLUSION_MAP',
    dEmissiveMap  : 'HAS_EMISSIVE_MAP',
    dAlphaBlend   : 'ALPHA_BLEND',
    dAlphaMask    : 'ALPHA_MASK',
    dNormal       : 'HAS_NORMAL',
    dTangent      : 'HAS_TANGENT',
    dTexCoord0    : 'HAS_TEXCOORD0',
    dTexCoord1    : 'HAS_TEXCOORD1',
    dColor0       : 'HAS_COLOR0',
    dSkin         : 'HAS_SKIN'
}

optMask = 0xFFFFFF
optLastBit = 23

def defsForMask(mask):
    defs = []
    for i in range(optLastBit + 1):
        bit = (1 << i) & mask
        if bit != 0:
            defs.append('-D' + optDefs[bit])
    # defaults
    if not mask & (dPbrsg | dUnlit):
        defs.append('-DMATERIAL_PBRMR')
    if not mask & (dAlphaBlend | dAlphaMask):
        defs.append('-DALPHA_OPAQUE')
    return defs

def nameForMask(mask):
    mask = mask & optMask
    return hex(mask)[2:].upper()

vportN = 1
instN  = 1
jointN = 100
lightN = 16

baseDefs = [
    '-DVPORT_N={}'.format(vportN),
    '-DINST_N={}'.format(instN),
    '-DJOINT_N={}'.format(jointN),
    '-DLIGHT_N={}'.format(lightN)
]

# TODO: Separate vert/frag shaders (filter by stage-specific bits)
def shdForMask(mask):
    return ('Main', nameForMask(mask), baseDefs + defsForMask(mask))

vert = [
    shdForMask(dNormal),
    shdForMask(dNormal | dTexCoord0 | dColorMap),
    shdForMask(dNormal | dTexCoord0 | dColorMap | dSkin)
]
frag = [
    shdForMask(dNormal),
    shdForMask(dNormal | dTexCoord0 | dColorMap),
    shdForMask(dNormal | dTexCoord0 | dColorMap | dSkin)
]

srcDir = 'tmp/shd/'
dstDir = 'bin/'
lang = ''
prefix = ''
suffix = '.bin'

compiler = 'tmp/shdc'

def compile(src, type, out, extra):
    i = srcDir + src + type + lang
    o = dstDir + prefix + (src if out == '' else out) + type + suffix
    subprocess.run([compiler, '-V', i, '-o', o] + extra)

def compileVert():
    for src, out, extra in vert:
        compile(src, '.vert', out, extra)

def compileFrag():
    for src, out, extra in frag:
        compile(src, '.frag', out, extra)

if __name__ == '__main__':
    compileVert()
    compileFrag()
