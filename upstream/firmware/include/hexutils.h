#ifndef HEXUTILS_H
#define HEXUTILS_H

#include <stdint.h>
#include <stdlib.h>
#include <math.h>

typedef struct {
    int x;
    int y;
} Point;

typedef struct {
    int q;
    int r;
} Hex;

static inline Point hex_to_pixel(Hex h, int size) {
    const double sqrt3 = 1.73205080757; // Pre-calculated value of sqrt(3)
    int x = size * (3.0/2 * h.q);
    int y = size * (sqrt3 * (h.r*2 + h.q)/2);
    return (Point){x, y};
}

static inline Hex pixel_to_hex(int x, int y, int size) {
    const double sqrt3 = 1.73205080757; // Pre-calculated value of sqrt(3)
    int q = (2.0/3 * x) / size;
    int r = (-1.0/3 * x + sqrt3/3 * y) / size;
    return (Hex){q, r};
}

static inline int hex_distance(Hex h1, Hex h2) {
    return (abs(h1.q - h2.q) + abs(h1.r - h2.r) + abs((-h1.q-h1.r) - (-h2.q-h2.r))) / 2;
}

static inline int hex_equal(Hex h1, Hex h2) {
    return (h1.q == h2.q) && (h1.r == h2.r);
}

static inline int hex_path(Hex start, Hex end, Hex *path, int max_path_len) {
    if (path == NULL || max_path_len <= 0) {
        return 0;
    }

    path[0] = start;
    if (hex_equal(start, end)) {
        return 1;
    }

    Hex current = start;
    int count = 1;
    while (count < max_path_len && !hex_equal(current, end)) {
        const Hex neighbors[6] = {
            {current.q + 1, current.r},
            {current.q - 1, current.r},
            {current.q, current.r + 1},
            {current.q, current.r - 1},
            {current.q + 1, current.r - 1},
            {current.q - 1, current.r + 1},
        };

        int best_index = -1;
        int best_distance = 999999;
        for (int i = 0; i < 6; ++i) {
            int skip = 0;
            for (int j = 0; j < count; ++j) {
                if (hex_equal(neighbors[i], path[j])) {
                    skip = 1;
                    break;
                }
            }
            if (skip) {
                continue;
            }

            const int dist = hex_distance(neighbors[i], end);
            if (dist < best_distance) {
                best_distance = dist;
                best_index = i;
            }
        }

        if (best_index < 0) {
            break;
        }

        current = neighbors[best_index];
        path[count++] = current;
    }

    return count;
}

static inline int hex_get_heading(Hex start, Hex end) {
    Hex dif=(Hex){end.q-start.q, end.r-start.r};
    if (dif.q < 0) {
        return dif.r > 0 ? 4:5;
    }
    else if (dif.q == 0) {
        return dif.r > 0 ? 3 : 0;
    }
    else {
        return dif.r == 0 ? 2 : 1;
    }
}


#endif // HEXUTILS_H