/* Included once with native keywords and once after obfus.h. */
static unsigned FAST_NAME(unsigned input) {
    unsigned sum = 0, steps = 0;
    for (unsigned i = 0; i < 17; ++i) {
        if ((i + input) % 3 == 0)
            continue;
        else
            sum += i;
        for (unsigned j = 0; j < 5; ++j) {
            switch ((j + input) % 4) {
                default:
                    if (j & 1)
                        continue;
                    else
                        sum += 7;
                    break;
                case 0:
                    sum += 3;
                    continue;
                case 1:
                    sum += 5;
                case 2:
                    sum += j;
                    break;
            }
            sum += 11;
        }
    }
    unsigned i = 0;
    while (++i < 13) {
        if (i & 1) continue;
        sum += i;
    }
    i = 0;
    do {
        ++i;
        if (i < 3) continue;
        sum += i;
    } while (i < 7);
    /* First iteration, nested targets, switch breaks, side-effecting conditions. */
    unsigned conditions = 0, outer = 0;
    do {
        ++outer;
        unsigned inner = 0;
        do {
            ++inner;
            if (inner == 2) continue;
            switch ((input + inner) & 3) {
                case 0:
                    sum += 59;
                    break;
                default:
                    sum += 61;
            }
            if (inner == 4) break;
            sum += inner;
        } while (++conditions < 100);
        if (outer == 2) continue;
        if (outer == 4) break;
        sum += outer;
    } while (++conditions < 100);
    sum ^= conditions << 8;
    do sum += 67;
    while (0);
    do
        ;
    while (0);
    do break;
    while (++conditions); /* break must not evaluate the condition. */
    i = 0;
    do continue;
    while (++i < 3);
    sum += i;
    if (input & 1) do
            if (input & 2)
                sum += 71;
            else
                sum += 73;
        while (0);
    else
        sum += 79;
    i = 0;
    do do
            ++i;
        while (i < 2);
    while (i < 2);
    sum += i;
    /* Common statement macro retains the outer loop's continue target. */
#define FAST_ONCE(body) \
    do { \
        body; \
    } while (0)
    FAST_ONCE(sum += 83);
#undef FAST_ONCE
    /* Continue must still reach the for increment; dangling else stays matched. */
    for (i = 0; i < 9; ++i, ++steps)
        if (i & 1)
            if (input & 1)
                continue;
            else
                sum += i;
        else
            continue;
    switch (input % 3) {
        case 0:
            sum += 19;
        default:
            sum += 23; /* Fallthrough and direct default entry. */
        case 1:
            sum += 29;
    }
    switch (input % 3) {
        case 0:
            sum += 31;
            break;
        default:
            switch (input % 2) {
                default:
                    sum += 37;
                    break;
                case 0:
                    sum += 41;
                    break;
            }
            break;
    }
    switch (input) { default:; }
    switch (input) {
        default:
        case 0:
            sum += 43;
    }
    switch (input) {
        default: {
            unsigned local = input ^ 47;
            sum += local;
        }
    }
    /* An ordinary label remains usable after the injected default label. */
    if (input & 1) goto user_entry;
    switch (input) {
        default:
        user_entry:
            sum += 53;
    }
    return sum ^ (steps << 16);
}
