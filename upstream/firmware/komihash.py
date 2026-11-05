#!/usr/bin/env python3


"""
KOMIHASH Python Implementation

A fast 64-bit hash function for ASCII strings.
"""

PRIME1 = 0x27d4eb2f165667c5
PRIME2 = 0x9e3779b97f4a7c15
PRIME3 = 0xc2b2ae3d27d4eb4f
PRIME4 = 0x165667b19e3779f9
PRIME5 = 0x85ebca77c2b2ae63

def komihash(data, seed=0):
    """
    Compute KOMIHASH of the given data.

    Args:
        data: ASCII string or bytes
        seed: 64-bit integer seed (default: 0)

    Returns:
        64-bit hash value as integer
    """
    if isinstance(data, str):
        p = data.encode('ascii')
    elif isinstance(data, bytes):
        p = data
    else:
        raise ValueError("Data must be string or bytes")

    h = (seed ^ PRIME5) & 0xFFFFFFFFFFFFFFFF
    i = 0

    # Process 8-byte blocks
    while i + 8 <= len(p):
        # Read 8 bytes as little-endian 64-bit integer
        k = 0
        for j in range(8):
            k |= p[i + j] << (j * 8)
        h ^= k
        h &= 0xFFFFFFFFFFFFFFFF
        h = ((h * PRIME1) + PRIME2) & 0xFFFFFFFFFFFFFFFF
        i += 8

    # Handle remaining bytes
    tail = 0
    rem = len(p) - i
    for j in range(rem):
        tail |= p[i + j] << (j * 8)

    h ^= tail
    h &= 0xFFFFFFFFFFFFFFFF
    h = ((h * PRIME1) + PRIME2) & 0xFFFFFFFFFFFFFFFF

    # Finalization
    h ^= len(p)
    h &= 0xFFFFFFFFFFFFFFFF
    h = ((h * PRIME3) + PRIME4) & 0xFFFFFFFFFFFFFFFF
    h ^= (h >> 33)
    h &= 0xFFFFFFFFFFFFFFFF
    h = ((h * PRIME1) + PRIME2) & 0xFFFFFFFFFFFFFFFF
    h ^= (h >> 29)
    h &= 0xFFFFFFFFFFFFFFFF
    h = ((h * PRIME3) + PRIME4) & 0xFFFFFFFFFFFFFFFF
    h ^= (h >> 32)
    h &= 0xFFFFFFFFFFFFFFFF

    return h

def main():
    import sys
    if len(sys.argv) != 2:
        print("Usage: python komihash.py <ascii_string>")
        print("Or run without args to generate unlock hashes")
        generate_unlock_hashes()
        sys.exit(0)

    ascii_string = sys.argv[1]
    try:
        hash_value = komihash(ascii_string)
        print(f"KOMIHASH of '{ascii_string}': {hash_value:016x}")
    except ValueError as e:
        print(f"Error: {e}")
        sys.exit(1)

def generate_unlock_hashes():
    KOMISEED = 0x50726f66506c756d
    strings = [
        "APOGEE_DRIFT",
        "PERIGEE_SHIFT",
        "LAGRANGE_LOCK",
        "ION_WAKE",
        "SOLAR_SHEAR",
        "PLASMA_ARC",
        "VOID_VECTOR",
        "STAR_OFFSET",
        "ORBIT_DECAY",
        "DUST_FLUX",
        "ECHO_ORBIT",
        "PHASE_DRIFT",
        "GRAV_SHIFT",
        "CORE_FLUX",
        "VECTOR_BURN",
        "NOVA_TRACE",
        "FIELD_ECHO",
        "QUANTUM_WAKE",
        "DEEP_PARALLAX",
        "RADIAL_DRIFT",
        "COSMIC_BIAS",
        "THRUST_LOSS",
        "SIGNAL_GHOST",
        "FLUX_VECTOR",
        "ORBIT_TRACE",
        "ASTRA_SHIFT",
        "ZENITH_DRIFT",
        "PULSE_FIELD",
        "ECHO_VECTOR",
        "DRIFT_LOCK"
    ]
    
    print("uint64_t unlock_hashes[30] = {")
    for s in strings:
        prefixed = "SC_" + s
        h = komihash(prefixed, KOMISEED)
        print(f"    0x{h:016x}ULL, // {prefixed}")
    print("};")

if __name__ == "__main__":
    main()