#!/bin/python

player_levels = (
    5, 27, 65, 114, 175, 245, 326, 415, 513, 621, 735, 859,
    989, 1127, 1273, 1425, 1586, 1752, 1926, 2106, 2292, 2487,
    2685, 2892, 3105, 3322, 3547, 3778, 4014, 4257,
)

growth_levels = (
    16, 21, 57, 111, 183, 273, 381, 507, 651, 813, 993, 1191,
    1407, 1641, 1893, 2163, 2451, 2757, 3081, 3423, 3783, 4161,
    4557, 4971, 5403, 5853, 6321, 6807, 7311,
)

build_levels = (
    5, 8, 11, 17, 25, 38, 57, 85, 128, 192, 288, 432, 649, 973,
    1460, 2189, 3284, 4926, 7389, 11084, 16626, 24939, 37409,
    56114, 84171, 126256, 189384, 284076, 426113, 639170,
)

science_levels = (
    29, 74, 138, 221, 325, 451, 603, 780, 986, 1223, 1492, 1796,
    2138, 2520, 2945, 3415, 3934, 4505, 5131, 5816, 6563, 7377,
    8261, 9221, 10260, 11382, 12595, 13901, 15400, 16715,
)

culture_levels = (
    0, 318, 765, 1315, 2084, 2985, 4059, 5320, 6785, 8467,
    10382, 12547, 14979, 17697, 20721, 24071, 27768, 31835,
    36298, 41179, 46507, 52310, 58616, 65456, 72863,
)

def check_levels(name, formula, levels):
    correct = True

    for l in range(len(levels)):
        if formula(l) != levels[l]:
            args = name, l, formula(l), levels[l]
            print ' * %s formula is wrong for level %d: %d != %d' % args
            correct = False

    if correct:
        print '* %s formula seems to be correct' % name

def player_cost(l):
    return (int(5 * (l + 1) ** 2.7 + 0.5) -
            int(5 * (l + 0) ** 2.7 + 0.5))
def growth_cost(l):
    return int(9 * (l + 1) * l + 3 if l > 0 else 16)
def build_cost(l):
    return int(1.5 ** l * 5 + 0.5)

check_levels('player',   player_cost,  player_levels)
check_levels('build',     build_cost,   build_levels)
check_levels('growth',   growth_cost,  growth_levels)
#check_levels('science', science_cost, science_levels)
#check_levels('culture', culture_cost, culture_levels)

#print science_levels[:20]
#print [sum(science_levels[:i+1]) for i in range(20)]

deltas = [science_levels[i+1] - science_levels[i] for i in range(20)]

print [deltas[i+1] - deltas[i] - 19 for i in range(19)]
print [int(l ** 1.3 + 0.5) for l in range(19)]

def foo(l):
    return l ** 1.3

#print deltas
#print [int(45 + sum([foo(i) for i in range(l)]) + 0.5) for l in range(19)]
#print [foo(i) for i in range(19)]

#print [19 * 2 ** i for i in range(19)]

print [i - 45 for i in deltas]
print [l * (l+1) for l in range(19)]
