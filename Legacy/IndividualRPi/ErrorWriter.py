#!/usr/bin/env python3
# -*- coding: utf-8 -*-
__author__ = "MPZinke"

###########################################################################
#
#	created by: MPZinke
#	on ..
#
#	DESCRIPTION:
#	BUGS:
#	FUTURE:
#
###########################################################################

from datetime import datetime

from Definitions import *


def write_error(error, module=None):
	try:
		if not module:
			import traceback
			module = traceback.format_exc().split("\n")[-4].strip()

		# change delimiter if exists
		error_message = str(error).replace(DELIMITER, ',' if ',' is not DELIMITER else ' ')
		current_timestamp = datetime.now().strftime(DATETIME_STRING_FORMAT)
		with open(ERROR_FILE_NAME, "a") as error_file:
			error_file.write("%s%c%s%c%s\n" % (	module, DELIMITER,
														error_message, DELIMITER, 
														current_timestamp))
	except: pass
