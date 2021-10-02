import sys
sys.path.insert(0, "../../tools")
from gen import get_generated_code

print(get_generated_code("call", "main", 4))