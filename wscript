#!/usr/bin/env python
# encoding: utf-8

from waflib.Tools import waf_unit_test
import os

VERSION = '0.1'
APPNAME = 'cxxjson'

top = '.'
out = 'build'


def init(ctx):
    ctx.load('build_logs')


def options(ctx):
    ctx.load('compiler_cxx')
    ctx.load('gnu_dirs')
    ctx.load('use_config')
    ctx.load('waf_unit_test')


def configure(ctx):
    ctx.env.DEFINES = os.environ.get('DEFINES', '').split()
    ctx.load('compiler_cxx')
    ctx.load('gnu_dirs')
    ctx.load('waf_unit_test')
    ctx.msg('Used CXXFLAGS', ' '.join(ctx.env.CXXFLAGS))
    ctx.msg('Used LINKFLAGS', ' '.join(ctx.env.LINKFLAGS))
    ctx.msg('Used DEFINES', ' '.join(ctx.env.DEFINES))
    ctx.env.INCLUDES = [ctx.path.abspath(), './inc']


def build(ctx):
    incs = ctx.path.find_dir('inc')
    ctx.install_files(
        '${INCLUDEDIR}',
        incs.ant_glob('**/*.(h|hpp)'),
        cwd=incs,
        relative_trick=True
    )

    ctx.objects(
        source=ctx.path.ant_glob(['src/**/*.(c|cpp)']),
        target='SRCLIB',
        features='cxx',
    )

    ctx(
        target=APPNAME,
        features='cxx cxxshlib',
        vnum=VERSION,
        use=['SRCLIB'],
        install_path='${LIBDIR}',
    )

    ctx(
        source=ctx.path.ant_glob(['test/test_main.cpp']),
        target='test_main',
        features='cxx',
        includes=['test'],
    )

    for suite in ctx.path.ant_glob(
        ['test/**/*.(c|cpp)'],
            excl=['test/test_main.cpp']):
        target, _ = suite.name.split('.')
        ctx(
            source=suite,
            target='ut_' + target,
            features='cxx cxxprogram test',
            use=['test_main', 'SRCLIB'],
            lib=['pthread'],
            install_path=None,
        )

    ctx.add_post_fun(waf_unit_test.summary)
    ctx.add_post_fun(waf_unit_test.set_exit_code)
