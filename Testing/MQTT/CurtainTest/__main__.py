#!/opt/homebrew/bin/python3
# -*- coding: utf-8 -*-
__author__ = "MPZinke"

########################################################################################################################
#                                                                                                                      #
#   created by: MPZinke                                                                                                #
#   on 2023.05.08                                                                                                      #
#                                                                                                                      #
#   DESCRIPTION:                                                                                                       #
#   BUGS:                                                                                                              #
#   FUTURE:                                                                                                            #
#                                                                                                                      #
########################################################################################################################


import json
import sys
from time import sleep


import Global
import Control



# —————————————————————————————————————————————————————— CONTROL ————————————————————————————————————————————————————— #

def main():
	id = sys.argv[1]
	Control.loop()


if(__name__ == "__main__"):
	main()
