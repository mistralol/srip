#!/bin/bash

if [ ! -d config ] ; then mkdir config ; fi
if [ ! -d m4 ] ; then mkdir  m4 ; fi

autoreconf --force --install -I config -I m4

./configure $*

