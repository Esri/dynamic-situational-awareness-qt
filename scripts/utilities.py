import json
import os
import pathlib
import requests
import shutil
import sys
import typing
import zipfile

path_home: pathlib.Path = pathlib.Path(os.environ["HOME"])
path_repo: pathlib.Path = path_home / "dynamic-situational-awareness-qt"


def zip_apks() -> None:
	path_build: pathlib.Path = path_repo / "build"
	path_zip: pathlib.Path = path_build / "apks.zip"
	if path_zip.exists():
		os.remove(path_zip)
	with zipfile.ZipFile(path_zip.as_posix(), "w", zipfile.ZIP_DEFLATED) as zip_apks:
		path_apk: pathlib.Path | None = None
		for path_apk in path_build.rglob("*.apk"):
			path_absolute_apk: str = path_apk.absolute().as_posix()
			folder_arc: str = "Vehicle" if "Vehicle" in path_absolute_apk else "Handheld"
			print(f"adding {path_absolute_apk} to zip")
			zip_apks.write(path_apk, f"{folder_arc}/{path_apk.name}")


def create_build() -> None:
	# replace all tokens in the defaults from the res folder
	path_res: pathlib.Path = path_repo / "scripts/res"
	for file_name, replacements in [["DsaAppConfig.json", {
		"DsaVersion": "3.1"
	}]]:
		entire_file: str = ""
		with open(path_res / f"default_{file_name}") as input_file_h:
			entire_file = input_file_h.read()

		with open(path_res / file_name, "w") as output_file_h:
			for replacement_name, replacement_value in replacements.items():
				entire_file = entire_file.replace(f"<|||{replacement_name}|||>", replacement_value)
			output_file_h.write(entire_file)



# parameter list for methods to be executed from main
tasks: dict[str, typing.Callable] = {
	"zip_apks": zip_apks,
	"create_build": create_build,
}


def main(task_to_run: str) -> None:
	if task_to_run in tasks:
		tasks[task_to_run]()
		return
	print(f"{task_to_run} does not have a defined task.")

if __name__ == "__main__":
	if len(sys.argv) != 2:
		raise ValueError(f"Invalid arguments. utilities.py should only be passed a single argument of the main method to be executed")

	task_name: str = sys.argv[-1]
	if task_name not in tasks:
		raise KeyError(f"Invalid task name for main. [{task_name}]. Valid args are: {list(tasks.keys())}")
	
	print(f"Executing: {task_name}")
	main(task_name)
