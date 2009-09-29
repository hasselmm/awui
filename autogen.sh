#!/bin/sh -e

autoreconf -i -f
exec ./configure "$@"
