# Normalize_Volume_of_Music_File
This project aims to create a file that normalizes the volume of a .wav file so that the entire song has a similar volume

## Usage
- The C program reads a `.wav` audio file, applies volume adjustments, and outputs a new file with uniform volume.
- This code works specifically with a version provided of the song "Kid A.wav" by Radiohead

## Code Structure
- The program reads the .wav file header and processes each audio sample.
- It calculates the average volume for each 100ms block.
- A reference volume is determined and used to adjust the amplification of quieter segments.

## Screenshots
- Original File:

![image](https://github.com/user-attachments/assets/82eda7b6-736d-41eb-bc5f-8dd06885fc55)
- Edited File Result:


![image](https://github.com/user-attachments/assets/e44011b8-5474-4345-992d-2328e54694c3)
