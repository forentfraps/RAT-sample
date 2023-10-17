import math

def calculate_entropy(file_path):
    # Initialize a dictionary to store the frequency of each byte
    byte_frequency = {}

    # Total number of bytes
    total_bytes = 0

    # Read the file and count byte frequencies
    with open(file_path, "rb") as file:
        while True:
            byte = file.read(1)
            if not byte:
                break
            total_bytes += 1
            byte = byte[0]  # Get the integer value of the byte
            if byte in byte_frequency:
                byte_frequency[byte] += 1
            else:
                byte_frequency[byte] = 1

    # Calculate the entropy
    entropy = 0
    for frequency in byte_frequency.values():
        probability = frequency / total_bytes
        entropy -= probability * math.log2(probability)

    return entropy

if __name__ == "__main__":
    file_path = "loader.exe"  # Replace with the path to your file
    entropy = calculate_entropy(file_path)
    print(f"Entropy of the file is: {entropy:.2f} bits/byte")