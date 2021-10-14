#!/usr/bin/env python3
# -*- coding: utf-8 -*-
__author__ = "MPZinke"

########################################################################################################################
#                                                                                                                      #
#   created by: MPZinke                                                                                                #
#   on 2021.10.07                                                                                                      #
#                                                                                                                      #
#   DESCRIPTION:                                                                                                       #
#   BUGS:                                                                                                              #
#   FUTURE:                                                                                                            #
#                                                                                                                      #
########################################################################################################################


from os import listdir as os_listdir;
import os.path;
from re import search as re_search;
from subprocess import call as subprocess_call, check_output as subprocess_check_output;

from Class.ZWidget import ZWidget;
from Other.Global import DB_DIR, substr;
from Updater.Version import Version;


class Updater(ZWidget):
	def __init__(self):
		ZWidget.__init__(self, "Updater", None);

		self.local_version = self.get_local_version();
		self.origin_Production_version = self.get_remote_version();


	# ——————————————————————————————————————————————————— ZWIDGET  ——————————————————————————————————————————————————— #

	# Increment versions, checking for DB updates. If found, update DB
	def _loop_process(self):
		self.origin_Production_version = self.get_remote_version();

		if(self.origin_Production_version != self.local_version):
			self.update_git();
			self.update_db();
			self.restart_service();


	# Compliments of https://jacobbridges.github.io/post/how-many-seconds-until-midnight/
	def sleep_time(self) -> int:
		return (tomorrow_00_00() - datetime.now()).seconds + 30;  # give time to let event creators to do their thing


	# ————————————————————————————————————————————— GIT VERSION GETTERS  ————————————————————————————————————————————— #

	# Get current version from repo
	def get_local_version(self):
		git_describe = subprocess_check_output(["git", "describe"]);
		if(not git_describe): raise Exception("git describe was unable to get version number");

		describe_version = Version.version_string(git_describe.decode("utf-8"))
		if(not describe_version): raise Exception("Unable to search version number from git describe");
		return Version(describe_version);


	# Get remote version from remote repo
	def get_remote_version(self):
		git_ls_remote = subprocess_check_output(["git", "ls-remote", "--tag", "origin"]);
		if(not git_ls_remote): raise Exception("git describe was unable to get version number");

		remote_version = Version.version_string(git_ls_remote.decode("utf-8"));
		if(not remote_version):  raise Exception("Unable to search version number from git describe");

		return Version(remote_version);


	# ———————————————————————————————————————————————————— UPDATE ———————————————————————————————————————————————————— #

	# Updates the DB incrementally with known update files.
	# Gets all files in folder except ./.add. Creates a Version object for all files. Sorts files by version number.
	#  Runs DB update file in mysql.
	def update_db(self):
		DB_update_folder = DB_DIR+"/Updates";
		if(not os.path.exists(DB_update_folder)): return;

		update_files = [];
		for file in os_listdir(DB_update_folder):
			filepath = join(DB_update_folder, file);
			if(os.path.isfile(filepath) and file != ".add"):
				update_files.append(filepath);

		files_versions = [];
		for file in update_files:
			files_versions.append({"path": file, "version": Version(Version.version_string(os.path.basename(file)))});
		files_versions.sort(key=lambda file_version : file_version["version"]);

		for file in files_versions:
			if(file["version"] > self.local_version):
				if(self.call_shell_command(["sudo", "mysql", "-u", "root", "<", file["path"]])):
					raise Exception(f"Failed to update DB with file {file['path']}");


	# Updates the Python and DB repository for the Hub.
	def update_git(self):
		if(self.call_shell_command(["git", "-C", "/usr/local/SmartCurtain", "stash"])):
			raise Exception("Could not stash local repository");
		if(self.call_shell_command(["git", "-C", "/usr/local/SmartCurtain", "checkout", "Production"])):
			raise Exception("Could not checkout Production branch");
		if(self.call_shell_command(["git", "-C", "/usr/local/SmartCurtain", "pull"])):
			raise Exception("Could not pull repository");


	def restart_service(self):
		if(self.call_shell_command(["sudo", "systemctl", "restart", "SmartCurtain.service"])):
			raise Exception("Could not restart systemctl service");


	# ——————————————————————————————————————————————————— UTILITY  ——————————————————————————————————————————————————— #

	def call_shell_command(self, params: list):
		with open(devnull, 'w') as FNULL:
			return subprocess_call(params, stdout=FNULL);
