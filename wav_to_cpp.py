from pydub import AudioSegment
import os
import sys
import struct

input_directory = 'wav_input'
output_directory = 'cpp_output'

def read_wav(filename):
    audio = AudioSegment.from_file(filename, format="wav")
    sample_rate = audio.frame_rate
    num_channels = audio.channels
    sample_width = audio.sample_width
    num_frames = len(audio.get_array_of_samples()) // num_channels

    # Print file information
    print(f"Processing File: {filename}")
    print(f"File Type: .wav")
    print(f"Bit Depth: {sample_width * 8}-bit")
    channel_info = "Mono" if num_channels == 1 else "Stereo"
    print(f"Channels: {channel_info}")

    # Get raw audio data
    raw_data = audio.raw_data
    return raw_data, sample_rate, num_channels, sample_width, num_frames


def format_for_cpp(raw_data, sample_rate, num_channels, sample_width, num_frames):
    # Assuming 16-bit audio
    fmt = "<" + "h" * (len(raw_data) // 2)
    audio_data = struct.unpack(fmt, raw_data)

    # Format as C array with 32-bit values
    c_array = []
    for i in range(0, len(audio_data), 2):
        sample1 = audio_data[i] & 0xffff
        sample2 = audio_data[i + 1] & 0xffff if i + 1 < len(audio_data) else 0
        combined_sample = (sample1 << 16) | (sample2 & 0xffff)
        c_array.append(f"0x{combined_sample:08X}")
    return c_array

def write_to_file(c_array, filename, sample_rate):
    base_name = os.path.splitext(os.path.basename(filename))[0]
    cpp_file_path = os.path.join(output_directory, f"{base_name}.cpp")
    h_file_path = os.path.join(output_directory, f"{base_name}.h")

    with open(cpp_file_path, 'w') as cpp_file, open(h_file_path, 'w') as h_file:
        array_len = len(c_array)
        h_file.write(f"extern const unsigned int AudioSample{base_name}[{array_len}];\n")
        cpp_file.write("#include <Arduino.h>\n")
        cpp_file.write(f"#include \"{os.path.basename(h_file_path)}\"\n\n")
        cpp_file.write(f"// Converted from {os.path.basename(filename)}, using {sample_rate} Hz, 16 bit PCM encoding\n")
        cpp_file.write(f"PROGMEM const unsigned int AudioSample{base_name}[{array_len}] = " + "{\n")
        for line in range(0, len(c_array), 8):
            cpp_file.write(", ".join(c_array[line:line+8]) + ",\n")
        cpp_file.write("};\n")


def process_file(filename):
    raw_data, sample_rate, num_channels, sample_width, num_frames = read_wav(filename)
    c_array = format_for_cpp(raw_data, sample_rate, num_channels, sample_width, num_frames)
    write_to_file(c_array, filename, sample_rate)

def main():
    if not os.path.exists(input_directory):
        print(f"Input directory does not exist: {input_directory}")
        sys.exit(1)

    if not os.path.exists(output_directory):
        os.makedirs(output_directory)

    for file in os.listdir(input_directory):
        if file.lower().endswith('.wav'):
            full_path = os.path.join(input_directory, file)
            raw_data, sample_rate, num_channels, sample_width, num_frames = read_wav(full_path)
            c_array = format_for_cpp(raw_data, sample_rate, num_channels, sample_width, num_frames)
            write_to_file(c_array, full_path, sample_rate)
            print("\n")

    print("Conversion completed.")

if __name__ == "__main__":
    main()