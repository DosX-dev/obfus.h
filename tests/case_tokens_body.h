/* Native and protected copies must agree, including unusual label placement. */
static unsigned CASE_NAME(int input, int condition) {
    unsigned sum = 0;
    int evaluations = 0;
    switch ((++evaluations, input)) {
        case -4:
        case -3:
        case -2:
            sum += 17;
        case -1:
            sum += 19;
            break;
        case 0:
            if (condition)
            case 1:
                sum += 23;
            else
                sum += 29;
            break;
        case 2:
            if (condition)
                if (input == 2)
                    sum += 31;
                else
                    sum += 37;
            else
                sum += 41;
            break;
        case 3 ... 5:
            switch (condition) {
                case 0:
                    sum += 43;
                    break;
                case 1:
                    sum += 47;
            }
            break;
        default:
            sum += 53;
        case 6:
            sum += 59;
    }
    switch (input)
    case 7:
        sum += 61;
    for (int i = 0; i < 4; ++i) {
        switch ((input + i) & 3) {
            case 0:
                continue;
            case 1:
                break;
            case 2:
                sum += 67;
            case 3:
                sum += 71;
                break;
        }
        sum += 73;
    }
    int count = 3;
    switch ((unsigned)input & 3u) {
        case 0:
            do {
                sum += 79;
                case 3:
                    sum += 83;
                case 2:
                    sum += 89;
                case 1:
                    sum += 97;
            } while (--count > 0);
    }
    switch ((long long)input) {
        case (-9223372036854775807LL - 1LL):
            sum += 101;
            break;
        case 0x100000000LL:
            sum += 103;
            break;
        case 8:
            goto case_done;
        default:
            sum += 107;
    }
case_done:
    return sum + evaluations * 109u;
}
