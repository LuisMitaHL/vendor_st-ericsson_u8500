#!/bin/sh

echo "Stop all vibrators"
sk-ab W 0xD08 0x30
sk-ab W 0xD09 0x30
exit

