#!/usr/bin/env python
# encoding: utf-8

# The MIT License (MIT)
#
# Copyright 2020 olmanqj
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

import os
import urllib.request, zipfile, io

top = '.'
out = 'build'


APPNAME = 'sfsf'
VERSION = '0.1'


libcsp_path='extern/libcsp'


def options(ctx):
    # Load compiler
    ctx.load('compiler_c')

    # Set libcsp options
    sfsf_opt = ctx.add_option_group('sfsf options')
    # App source files and name
    sfsf_opt.add_option('--app-src', metavar='APP_SRC', default='app', help='Path to the app source files')
    sfsf_opt.add_option('--with-build-name', metavar='BUILD_NAME', default='sfsf_app', help='Set the name of pragram build')
    # Port
    sfsf_opt.add_option('--with-port', metavar='PORT', help='Set port files, use one of the ports in folder "ports/", e.g.: "linux"')
    # Ground Station
    sfsf_opt.add_option('--enable-ground-station', action='store_true', help='Build the Ground Station example')

    # Call Load libcsp options
    ctx.recurse(libcsp_path)


def configure(ctx):
    # Load compiler
    ctx.load('compiler_c')

    # Validate options
    if not ctx.options.with_port:
        ctx.fatal('--with-port must be defined, use one of the ports in folder "ports/", e.g.: "linux"')

    # Setup default include path
    ctx.env.append_unique('INCLUDES_SFSF', ['include',
                                            'ports/{0}'.format(ctx.options.with_port),
                                            ctx.options.app_src])

    # Add SFSF source files
    ctx.env.append_unique('FILES_SFSF', ['src/*.c',
                                         'ports/{0}/*.c'.format(ctx.options.with_port),
                                         '{0}/*.c'.format(ctx.options.app_src)])

    # Call Config libcsp options
    ctx.recurse(libcsp_path)



def build(ctx):

    # Build libcsp
    ctx.recurse(libcsp_path)

    ctx(export_includes=ctx.env.INCLUDES_SFSF, name='sfsf_h')

    ctx.program(source=ctx.path.ant_glob(ctx.env.FILES_SFSF),
                target=ctx.options.with_build_name,
                includes=ctx.env.INCLUDES_SFSF,
                lib=ctx.env.LIBS,
                use=['csp'])

    # Build ground station demo
    if ctx.options.enable_ground_station:
        ctx.program(source='examples/ground_station/ground_station.c',
                    target='ground_station',
                    lib=ctx.env.LIBS,
                    use='csp')

def test(ctx):
    print("Test...")


def dist(ctx):
    ctx.excl = 'build/* **/.* **/*.pyc **/*.o **/*~ *.tar.gz'
