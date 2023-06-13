#!/bin/bash
# Program the PIC with built hex. Run anywhere in repo
repo_root=$(git rev-parse --show-toplevel)
java -jar $repo_root/zeppp/zeppp-cli.jar -C ttyACM0 -wait 2000 -i $repo_root/mplabx_prj/dist/default/production/mplabx_prj.production.hex -p
