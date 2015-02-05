#! /bin/bash
# Remove old versions for flot

rm -rf flot flot-downsample flot-selection

# Clone from github

git clone https://github.com/flot/flot.git
git clone https://github.com/sveinn-steinarsson/flot-downsample.git
git clone https://github.com/cclibs/flot-selection.git

# Checkout v0.7 of flot

cd flot

git checkout v0.7

cd ..

# EOF
