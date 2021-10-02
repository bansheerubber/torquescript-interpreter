from preprocessor import preprocess

def get_generated_code(folder, file, number_of_tabs):
	tabs = ""
	for i in range(0, number_of_tabs):
		tabs = tabs + "\t"

	with open(f"gen/{folder}/{file}.cc.gen") as start:
		return ''.join(preprocess("", [f"{tabs}{line}" for line in start.readlines()], "."))
