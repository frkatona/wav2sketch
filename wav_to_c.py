import wave
import os
import sys
import struct

def read_wav(filename):
    with wave.open(filename, 'rb') as wav_file:
        sample_rate = wav_file.getframerate()
        num_channels = wav_file.getnchannels()
        sample_width = wav_file.getsampwidth()
        num_frames = wav_file.getnframes()

        # Check for 16-bit audio
        if sample_width != 2:
            raise ValueError(f"Unsupported sample width: {sample_width*8} bits. Only 16-bit audio is supported.")

        # Read raw audio data
        raw_data = wav_file.readframes(num_frames)
        return raw_data, sample_rate, num_channels, sample_width, num_frames


def format_for_c(raw_data, sample_rate, num_channels, sample_width, num_frames):
    # Assuming 16-bit audio
    fmt = "<" + "h" * (len(raw_data) // 2)
    audio_data = struct.unpack(fmt, raw_data)

    # Format as C array
    c_array = []
    for sample in audio_data:
        c_array.append(f"0x{sample & 0xffff:04x}")
    return c_array

def write_to_file(c_array, filename, sample_rate):
    base_name = os.path.splitext(os.path.basename(filename))[0]
    c_file_name = f"AudioSample{base_name}.c"
    h_file_name = f"AudioSample{base_name}.h"

    with open(c_file_name, 'w') as c_file, open(h_file_name, 'w') as h_file:
        array_len = len(c_array)
        h_file.write(f"extern const unsigned int AudioSample{base_name}[{array_len}];\n")
        c_file.write(f"#include \"{h_file_name}\"\n\n")
        c_file.write(f"const unsigned int AudioSample{base_name}[{array_len}] = " + "{\n")
        for line in range(0, len(c_array), 8):
            c_file.write(", ".join(c_array[line:line+8]) + ",\n")
        c_file.write("};\n")

def main():
    if len(sys.argv) < 2:
        print("Usage: python wav_to_c.py <filename.wav>")
        sys.exit(1)

    filename = sys.argv[1]
    try:
        raw_data, sample_rate, num_channels, sample_width, num_frames = read_wav(filename)
        c_array = format_for_c(raw_data, sample_rate, num_channels, sample_width, num_frames)
        write_to_file(c_array, filename, sample_rate)
    except ValueError as e:
        print(f"Error: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main()