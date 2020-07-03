#!/usr/bin/env python
# encoding: utf-8

import subprocess
import sys


os = 'posix'  # default OS
port='linux'
app_src="examples/linux/app"
build_name='sfsf_example'


options = []

# SFSF options
options += [
    '--with-port='+port,
    '--app-src='+app_src,
    '--with-build-name='+build_name,
    '--enable-ground-station'
]

# CSP options
options += [
    '--with-os=' + os,
    '--enable-rdp',
    '--enable-promisc',
    '--enable-crc32',
    '--enable-hmac',
    '--enable-xtea',
    '--enable-dedup',
    '--with-loglevel=debug',
    '--enable-debug-timestamp',
    '--enable-qos',
    '--with-rtable=cidr',
    '--disable-stlib',
    #'--enable-shlib',
    '--enable-python3-bindings'
]

waf = ['./waf']
if os in ['posix']:
    options += [
        '--with-driver-usart=linux',
        '--enable-if-zmqhub'
    ]

if os in ['macosx']:
    options += [
        '--with-driver-usart=linux',
    ]

if os in ['windows']:
    options += [
        '--with-driver-usart=windows',
    ]
    waf = ['python', '-x', 'waf']

# Build
waf += ['distclean', 'configure', 'build']
print("Waf build command:", waf)
subprocess.check_call(waf + options)
