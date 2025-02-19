import json
from array import array

if __name__ == "__main__":
    double_array = array("d")
    with open("build/decompressed_doubles.dat", "rb") as file:
        double_array.frombytes(file.read())

    with open("doubles.json", "w") as json_file:
        json.dump(double_array.tolist(), json_file)
