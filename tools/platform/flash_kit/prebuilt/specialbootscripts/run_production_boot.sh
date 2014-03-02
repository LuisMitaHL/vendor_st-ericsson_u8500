#!/usr/bin/env bash

../flashkit/flash-tool-cli/flash-tool.sh -port 8088 -host localhost set_active_profile -profile_name STE_DB8500_productionboot

exit $?
