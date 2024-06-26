#!/usr/bin/env python3

# This file creates the static 640byte data array

import random

# List of hexademical values with utf representation
allowed_hex_values = [
    "0x21",
    "0x22",
    "0x23",
    "0x24",
    "0x25",
    "0x26",
    "0x27",
    "0x28",
    "0x29",
    "0x2a",
    "0x2b",
    "0x2c",
    "0x2d",
    "0x2e",
    "0x2f",
    "0x30",
    "0x31",
    "0x32",
    "0x33",
    "0x34",
    "0x35",
    "0x36",
    "0x37",
    "0x38",
    "0x39",
    "0x3a",
    "0x3b",
    "0x3c",
    "0x3d",
    "0x3e",
    "0x3f",
    "0x40",
    "0x41",
    "0x42",
    "0x43",
    "0x44",
    "0x45",
    "0x46",
    "0x47",
    "0x48",
    "0x49",
    "0x4a",
    "0x4b",
    "0x4c",
    "0x4d",
    "0x4e",
    "0x4f",
    "0x50",
    "0x51",
    "0x52",
    "0x53",
    "0x54",
    "0x55",
    "0x56",
    "0x57",
    "0x58",
    "0x59",
    "0x5a",
    "0x5b",
    "0x5c",
    "0x5d",
    "0x5e",
    "0x5f",
    "0x60",
    "0x61",
    "0x62",
    "0x63",
    "0x64",
    "0x65",
    "0x66",
    "0x67",
    "0x68",
    "0x69",
    "0x6a",
    "0x6b",
    "0x6c",
    "0x6d",
    "0x6e",
    "0x6f",
    "0x70",
    "0x71",
    "0x72",
    "0x73",
    "0x74",
    "0x75",
    "0x76",
    "0x77",
    "0x78",
    "0x79",
    "0x7a",
    "0x7b",
    "0x7c",
    "0x7d",
    "0x7e",
]

# Generate a list of 640 random hexadecimal values from the allowed list
random_hex_values = [random.choice(allowed_hex_values) for _ in range(640)]

# Convert the list to a C-style array initialization
c_array_init = ", ".join(random_hex_values)

# Create the content of the data.h file
data_h_content = f"""// data.h

#ifndef DATA_H
#define DATA_H

// random data
const unsigned char byteArray[640] = {{ {c_array_init} }};

#endif // DATA_H
"""

# Write the content to data.h
with open("data.h", "w") as file:
    file.write(data_h_content)

print("data.h file generated successfully.")
