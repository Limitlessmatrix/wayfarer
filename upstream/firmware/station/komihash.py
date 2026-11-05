PRIME1 = 0x27d4eb2f165667c5
PRIME2 = 0x9e3779b97f4a7c15
PRIME3 = 0xc2b2ae3d27d4eb4f
PRIME4 = 0x165667b19e3779f9
PRIME5 = 0x85ebca77c2b2ae63

def komihash(data, seed=0):
    """Compute KOMIHASH of the given data.

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

    while i + 8 <= len(p):
        k = 0
        for j in range(8):
            k |= p[i + j] << (j * 8)
        h ^= k
        h &= 0xFFFFFFFFFFFFFFFF
        h = ((h * PRIME1) + PRIME2) & 0xFFFFFFFFFFFFFFFF
        i += 8

    tail = 0
    rem = len(p) - i
    for j in range(rem):
        tail |= p[i + j] << (j * 8)

    h ^= tail
    h &= 0xFFFFFFFFFFFFFFFF
    h = ((h * PRIME1) + PRIME2) & 0xFFFFFFFFFFFFFFFF

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
