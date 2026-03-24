# Arena Allocator Benchmark

## Summary

Rounds: Base=5, Optimized=5 (1000 iterations each, paired by run number).

Metrics: **Δmean%** = relative change of arithmetic means; **Δgeom%** = geometric
mean of per-run ratios minus 1, which better captures consistent per-run speedups.


### Conclusion

The arena allocator optimization delivers a **median +3.3% parse throughput
improvement** across 18 benchmark expressions (17 of 18 improved, 1 within noise).
The heaviest expression `exprtk_special` benefits the most at **+10.2%**, and
complex expressions with deeper ASTs (more allocation pressure) show larger gains
(up to +7.4%).

EXPRTK eval and NATIVE eval phases are unaffected (median Δgeom% ≈ 0), confirming
the optimization targets compile-time allocation only, with no runtime regression.


### Cross-phase overview

| Phase | expressions | median Δgeom% |
|---|--:|--:|
| EXPRTK eval | 18 | +0.4% |
| NATIVE eval | 17 | +0.1% |
| PARSE | 18 | **+3.3%** |

### PARSE (primary target of arena allocator)

| Expression | base μ (parse/s) | opt μ (parse/s) | Δmean% | Δgeom% |
|---|--:|--:|--:|--:|
| (y + x) | 91,334 | 90,161 | −1.28 | −1.30 |
| 2 * (y + x) | 76,950 | 77,082 | +0.17 | +0.12 |
| (2 * y + 2 * x) | 67,270 | 70,167 | +4.31 | +4.27 |
| ((1.23 * x^2) / y) - 123.123 | 54,454 | 56,254 | +3.31 | +3.30 |
| (y + x / y) * (x - y / x) | 50,449 | 52,713 | +4.49 | +4.44 |
| x / ((x + y) + (x - y)) / y | 48,258 | 51,871 | +7.49 | +7.44 |
| 1 - ((x * y) + (y / x)) - 3 | 50,755 | 51,614 | +1.69 | +1.68 |
| (5.5 + x) + (2 * x - 2/3 * y) * (x/3 + y/4) + (y + 7.7) | 29,427 | 30,316 | +3.02 | +3.02 |
| 1.1x^1 + 2.2y^2 - 3.3x^3 + 4.4y^15 - 5.5x^23 + 6.6y^55 | 23,548 | 24,857 | +5.56 | +5.55 |
| sin(2 * x) + cos(pi / y) | 55,954 | 57,490 | +2.75 | +2.74 |
| 1 - sin(2 * x) + cos(pi / y) | 48,935 | 51,537 | +5.32 | +5.28 |
| sqrt(111.111 - sin(2 * x) + cos(pi / y) / 333.333) | 42,869 | 43,480 | +1.43 | +1.42 |
| (x^2 / sin(2 * pi / y)) - x / 2 | 46,072 | 46,869 | +1.73 | +1.72 |
| x + (cos(y - sin(2/x * pi)) - sin(x - cos(2 * y/pi))) - y | 29,228 | 30,478 | +4.28 | +4.26 |
| clamp(-1.0, sin(2 * pi * x) + cos(y/2 * pi), +1.0) | 37,316 | 38,688 | +3.68 | +3.65 |
| max(3.33, min(sqrt(1 - sin(2 * x) + cos(pi / y) / 3), 1.11)) | 35,655 | 36,715 | +2.97 | +2.96 |
| if((y + (x * 2.2)) <= (x + y + 1.1), x - y, x * y) + 2 * pi / x | 29,959 | 30,592 | +2.11 | +2.09 |
| exprtk_special | 2,768 | 3,050 | +10.21 | +10.15 |

### EXPRTK eval

| Expression | base μ (evals/s) | opt μ (evals/s) | Δmean% | Δgeom% |
|---|--:|--:|--:|--:|
| (y + x) | 266,855,894 | 267,127,176 | +0.10 | +0.10 |
| 2 * (y + x) | 260,902,161 | 282,113,035 | +8.13 | +8.19 |
| (2 * y + 2 * x) | 269,411,201 | 277,165,158 | +2.88 | +2.84 |
| ((1.23 * x^2) / y) - 123.123 | 124,898,654 | 121,243,192 | −2.93 | −2.93 |
| (y + x / y) * (x - y / x) | 108,095,923 | 112,893,660 | +4.43 | +4.12 |
| x / ((x + y) + (x - y)) / y | 104,291,855 | 101,452,804 | −2.72 | −2.55 |
| 1 - ((x * y) + (y / x)) - 3 | 158,416,312 | 157,500,445 | −0.58 | −0.58 |
| (5.5 + x) + (2 * x - 2/3 * y) * (x/3 + y/4) + (y + 7.7) | 55,163,150 | 54,850,068 | −0.57 | −0.45 |
| 1.1x^1 + 2.2y^2 - 3.3x^3 + 4.4y^15 - 5.5x^23 + 6.6y^55 | 20,507,287 | 21,025,382 | +2.53 | +2.52 |
| sin(2 * x) + cos(pi / y) | 27,830,342 | 28,818,030 | +3.55 | +3.38 |
| 1 - sin(2 * x) + cos(pi / y) | 26,825,613 | 26,940,063 | +0.43 | +0.42 |
| sqrt(111.111 - sin(2 * x) + cos(pi / y) / 333.333) | 22,321,774 | 22,328,122 | +0.03 | +0.02 |
| (x^2 / sin(2 * pi / y)) - x / 2 | 40,348,598 | 40,925,327 | +1.43 | +1.43 |
| x + (cos(y - sin(2/x * pi)) - sin(x - cos(2 * y/pi))) - y | 10,430,834 | 10,399,770 | −0.30 | −0.30 |
| clamp(-1.0, sin(2 * pi * x) + cos(y/2 * pi), +1.0) | 21,364,611 | 21,586,915 | +1.04 | +0.78 |
| max(3.33, min(sqrt(1 - sin(2 * x) + cos(pi / y) / 3), 1.11)) | 16,030,703 | 16,017,308 | −0.08 | −0.08 |
| if((y + (x * 2.2)) <= (x + y + 1.1), x - y, x * y) + 2 * pi / x | 58,826,680 | 59,764,721 | +1.60 | +1.57 |
| exprtk_special | 429,311 | 423,268 | −1.41 | −1.41 |

### NATIVE eval (control group)

| Expression | base μ (evals/s) | opt μ (evals/s) | Δmean% | Δgeom% |
|---|--:|--:|--:|--:|
| (y + x) | 267,853,236 | 267,859,316 | +0.00 | +0.00 |
| 2 * (y + x) | 267,860,287 | 267,171,801 | −0.26 | −0.26 |
| (2 * y + 2 * x) | 267,224,369 | 267,579,798 | +0.13 | +0.13 |
| ((1.23 * x^2) / y) - 123.123 | 264,117,810 | 265,355,003 | +0.47 | +0.47 |
| (y + x / y) * (x - y / x) | 272,530,092 | 272,356,808 | −0.06 | −0.06 |
| x / ((x + y) + (x - y)) / y | 260,579,150 | 254,181,637 | −2.45 | −2.46 |
| 1 - ((x * y) + (y / x)) - 3 | 261,645,894 | 260,467,643 | −0.45 | −0.45 |
| (5.5 + x) + (2 * x - 2/3 * y) * (x/3 + y/4) + (y + 7.7) | 233,118,262 | 233,689,824 | +0.25 | +0.24 |
| 1.1x^1 + 2.2y^2 - 3.3x^3 + 4.4y^15 - 5.5x^23 + 6.6y^55 | 10,052,742 | 10,042,036 | −0.11 | −0.11 |
| sin(2 * x) + cos(pi / y) | 38,172,160 | 38,361,437 | +0.50 | +0.49 |
| 1 - sin(2 * x) + cos(pi / y) | 37,847,053 | 38,165,325 | +0.84 | +0.83 |
| sqrt(111.111 - sin(2 * x) + cos(pi / y) / 333.333) | 31,763,935 | 31,653,867 | −0.35 | −0.35 |
| (x^2 / sin(2 * pi / y)) - x / 2 | 74,061,456 | 74,477,147 | +0.56 | +0.56 |
| x + (cos(y - sin(2/x * pi)) - sin(x - cos(2 * y/pi))) - y | 13,092,381 | 13,105,233 | +0.10 | +0.10 |
| clamp(-1.0, sin(2 * pi * x) + cos(y/2 * pi), +1.0) | 29,775,007 | 30,199,476 | +1.43 | +1.40 |
| max(3.33, min(sqrt(1 - sin(2 * x) + cos(pi / y) / 3), 1.11)) | 35,262,619 | 35,165,216 | −0.28 | −0.28 |
| if((y + (x * 2.2)) <= (x + y + 1.1), x - y, x * y) + 2 * pi / x | 255,408,357 | 255,609,251 | +0.08 | +0.08 |


## Detail Data

### Base 1
--- EXPRTK EVAL ---
[exprtk] Total Time: 1.21657900  Rate: 266883088.562 evals/sec Expression: (y + x)
[exprtk] Total Time: 1.20395400  Rate: 269681699.633 evals/sec Expression: 2 * (y + x)
[exprtk] Total Time: 1.20848600  Rate: 268670353.649 evals/sec Expression: (2 * y + 2 * x)
[exprtk] Total Time: 2.59907500  Rate: 124923044.160 evals/sec Expression: ((1.23 * x^2) / y) - 123.123
[exprtk] Total Time: 2.85518300  Rate: 113717530.890 evals/sec Expression: (y + x / y) * (x - y / x)
[exprtk] Total Time: 3.09599400  Rate: 104872412.866 evals/sec Expression: x / ((x + y) + (x - y)) / y
[exprtk] Total Time: 2.06097300  Rate: 157539356.896 evals/sec Expression: 1 - ((x * y) + (y / x)) - 3
[exprtk] Total Time: 5.89344000  Rate: 55092503.020 evals/sec Expression: (5.5 + x) + (2 * x - 2 / 3 * y) * (x / 3 + y / 4) + (y + 7.7)
[exprtk] Total Time: 15.85577900  Rate: 20477351.570 evals/sec Expression: 1.1x^1 + 2.2y^2 - 3.3x^3 + 4.4y^15 - 5.5x^23 + 6.6y^55
[exprtk] Total Time: 11.26572500  Rate: 28820547.368 evals/sec Expression: sin(2 * x) + cos(pi / y)
[exprtk] Total Time: 12.05233500  Rate: 26939540.014 evals/sec Expression: 1 - sin(2 * x) + cos(pi / y)
[exprtk] Total Time: 14.48482200  Rate: 22415488.502 evals/sec Expression: sqrt(111.111 - sin(2 * x) + cos(pi / y) / 333.333)
[exprtk] Total Time: 8.03398100  Rate: 40413882.109 evals/sec Expression: (x^2 / sin(2 * pi / y)) - x / 2
[exprtk] Total Time: 31.36544400  Rate: 10351658.373 evals/sec Expression: x + (cos(y - sin(2 / x * pi)) - sin(x - cos(2 * y / pi))) - y
[exprtk] Total Time: 15.92347700  Rate: 20390292.962 evals/sec Expression: clamp(-1.0, sin(2 * pi * x) + cos(y / 2 * pi), +1.0)
[exprtk] Total Time: 20.22437300  Rate: 16054112.580 evals/sec Expression: max(3.33, min(sqrt(1 - sin(2 * x) + cos(pi / y) / 3), 1.11))
[exprtk] Total Time: 5.40810400  Rate: 60036634.096 evals/sec Expression: if((y + (x * 2.2)) <= (x + y + 1.1), x - y, x * y) + 2 * pi / x
[exprtk] Total Time: 4.65350600  Rate: 429783.479 evals/sec Expression: exprtk_special
--- NATIVE EVAL ---
[native] Total Time: 1.20975700  Rate: 268388082.069 evals/sec Expression: (y + x)
[native] Total Time: 1.21007100  Rate: 268318438.340 evals/sec Expression: 2 * (y + x)
[native] Total Time: 1.22168100  Rate: 265768527.955 evals/sec Expression: (2 * y + 2 * x)
[native] Total Time: 1.23324100  Rate: 263277300.220 evals/sec Expression: ((1.23 * x^2) / y) - 123.123
[native] Total Time: 1.21407900  Rate: 267432647.299 evals/sec Expression: (y + x / y) * (x - y / x)
[native] Total Time: 1.24454800  Rate: 260885366.414 evals/sec Expression: x / ((x + y) + (x - y)) / y
[native] Total Time: 1.24196400  Rate: 261428158.143 evals/sec Expression: 1 - ((x * y) + (y / x)) - 3
[native] Total Time: 1.39667900  Rate: 232468850.036 evals/sec Expression: (5.5 + x) + (2 * x - 2 / 3 * y) * (x / 3 + y / 4) + (y + 7.7)
[native] Total Time: 32.37212500  Rate: 10029751.244 evals/sec Expression: 1.1x^1 + 2.2y^2 - 3.3x^3 + 4.4y^15 - 5.5x^23 + 6.6y^55
[native] Total Time: 8.50570800  Rate: 38172526.144 evals/sec Expression: sin(2 * x) + cos(pi / y)
[native] Total Time: 8.52481800  Rate: 38086955.170 evals/sec Expression: 1 - sin(2 * x) + cos(pi / y)
[native] Total Time: 10.25923700  Rate: 31648002.771 evals/sec Expression: sqrt(111.111 - sin(2 * x) + cos(pi / y) / 333.333)
[native] Total Time: 4.45087400  Rate: 72948450.349 evals/sec Expression: (x^2 / sin(2 * pi / y)) - x / 2
[native] Total Time: 24.90677000  Rate: 13035988.247 evals/sec Expression: x + (cos(y - sin(2 / x * pi)) - sin(x - cos(2 * y / pi))) - y
[native] Total Time: 10.88703100  Rate: 29823040.001 evals/sec Expression: clamp(-1.0, sin(2 * pi * x) + cos(y / 2 * pi), +1.0)
[native] Total Time: 9.21226800  Rate: 35244780.221 evals/sec Expression: max(3.33, min(sqrt(1 - sin(2 * x) + cos(pi / y) / 3), 1.11))
[native] Total Time: 1.26535100  Rate: 256596281.190 evals/sec Expression: if((y + (x * 2.2)) <= (x + y + 1.1), x - y, x * y) + 2 * pi / x
--- PARSE ----
[parse] Total Time: 1.11247300  Rate: 89889.822 parse/sec Expression: (y + x)
[parse] Total Time: 1.38650100  Rate: 72124.001 parse/sec Expression: 2 * (y + x)
[parse] Total Time: 1.56788700  Rate: 63780.107 parse/sec Expression: (2 * y + 2 * x)
[parse] Total Time: 1.84021200  Rate: 54341.565 parse/sec Expression: ((1.23 * x^2) / y) - 123.123
[parse] Total Time: 2.12392800  Rate: 47082.575 parse/sec Expression: (y + x / y) * (x - y / x)
[parse] Total Time: 2.26286600  Rate: 44191.746 parse/sec Expression: x / ((x + y) + (x - y)) / y
[parse] Total Time: 1.97196500  Rate: 50710.839 parse/sec Expression: 1 - ((x * y) + (y / x)) - 3
[parse] Total Time: 3.40662000  Rate: 29354.610 parse/sec Expression: (5.5 + x) + (2 * x - 2 / 3 * y) * (x / 3 + y / 4) + (y + 7.7)
[parse] Total Time: 4.38204400  Rate: 22820.401 parse/sec Expression: 1.1x^1 + 2.2y^2 - 3.3x^3 + 4.4y^15 - 5.5x^23 + 6.6y^55
[parse] Total Time: 1.80404100  Rate: 55431.113 parse/sec Expression: sin(2 * x) + cos(pi / y)
[parse] Total Time: 2.45649400  Rate: 40708.424 parse/sec Expression: 1 - sin(2 * x) + cos(pi / y)
[parse] Total Time: 2.35750200  Rate: 42417.779 parse/sec Expression: sqrt(111.111 - sin(2 * x) + cos(pi / y) / 333.333)
[parse] Total Time: 2.16793900  Rate: 46126.759 parse/sec Expression: (x^2 / sin(2 * pi / y)) - x / 2
[parse] Total Time: 3.49955700  Rate: 28575.045 parse/sec Expression: x + (cos(y - sin(2 / x * pi)) - sin(x - cos(2 * y / pi))) - y
[parse] Total Time: 2.74052300  Rate: 36489.385 parse/sec Expression: clamp(-1.0, sin(2 * pi * x) + cos(y / 2 * pi), +1.0)
[parse] Total Time: 2.85098600  Rate: 35075.584 parse/sec Expression: max(3.33, min(sqrt(1 - sin(2 * x) + cos(pi / y) / 3), 1.11))
[parse] Total Time: 3.34574700  Rate: 29888.692 parse/sec Expression: if((y + (x * 2.2)) <= (x + y + 1.1), x - y, x * y) + 2 * pi / x
[parse] Total Time: 0.37656200  Rate: 2655.605 parse/sec Expression: exprtk_special

### Optimized 1
--- EXPRTK EVAL ---
[exprtk] Total Time: 1.23884100  Rate: 262087193.595 evals/sec Expression: (y + x)
[exprtk] Total Time: 1.15532800  Rate: 281032192.589 evals/sec Expression: 2 * (y + x)
[exprtk] Total Time: 1.14414500  Rate: 283779032.378 evals/sec Expression: (2 * y + 2 * x)
[exprtk] Total Time: 2.65905800  Rate: 122105031.556 evals/sec Expression: ((1.23 * x^2) / y) - 123.123
[exprtk] Total Time: 2.85022100  Rate: 113915503.745 evals/sec Expression: (y + x / y) * (x - y / x)
[exprtk] Total Time: 3.14145400  Rate: 103354803.540 evals/sec Expression: x / ((x + y) + (x - y)) / y
[exprtk] Total Time: 2.05546900  Rate: 157961205.448 evals/sec Expression: 1 - ((x * y) + (y / x)) - 3
[exprtk] Total Time: 5.90492400  Rate: 54985358.152 evals/sec Expression: (5.5 + x) + (2 * x - 2 / 3 * y) * (x / 3 + y / 4) + (y + 7.7)
[exprtk] Total Time: 15.39136300  Rate: 21095231.202 evals/sec Expression: 1.1x^1 + 2.2y^2 - 3.3x^3 + 4.4y^15 - 5.5x^23 + 6.6y^55
[exprtk] Total Time: 11.26081200  Rate: 28833121.537 evals/sec Expression: sin(2 * x) + cos(pi / y)
[exprtk] Total Time: 12.05537400  Rate: 26932748.913 evals/sec Expression: 1 - sin(2 * x) + cos(pi / y)
[exprtk] Total Time: 14.51879900  Rate: 22363031.612 evals/sec Expression: sqrt(111.111 - sin(2 * x) + cos(pi / y) / 333.333)
[exprtk] Total Time: 7.95504200  Rate: 40814914.742 evals/sec Expression: (x^2 / sin(2 * pi / y)) - x / 2
[exprtk] Total Time: 31.29061300  Rate: 10376414.198 evals/sec Expression: x + (cos(y - sin(2 / x * pi)) - sin(x - cos(2 * y / pi))) - y
[exprtk] Total Time: 14.81551100  Rate: 21915164.519 evals/sec Expression: clamp(-1.0, sin(2 * pi * x) + cos(y / 2 * pi), +1.0)
[exprtk] Total Time: 20.33385600  Rate: 15967672.880 evals/sec Expression: max(3.33, min(sqrt(1 - sin(2 * x) + cos(pi / y) / 3), 1.11))
[exprtk] Total Time: 5.39865500  Rate: 60141713.260 evals/sec Expression: if((y + (x * 2.2)) <= (x + y + 1.1), x - y, x * y) + 2 * pi / x
[exprtk] Total Time: 4.70180600  Rate: 425368.465 evals/sec Expression: exprtk_special
--- NATIVE EVAL ---
[native] Total Time: 1.21041100  Rate: 268243068.677 evals/sec Expression: (y + x)
[native] Total Time: 1.21811500  Rate: 266546558.412 evals/sec Expression: 2 * (y + x)
[native] Total Time: 1.21468500  Rate: 267299226.548 evals/sec Expression: (2 * y + 2 * x)
[native] Total Time: 1.22037300  Rate: 266053379.581 evals/sec Expression: ((1.23 * x^2) / y) - 123.123
[native] Total Time: 1.18903400  Rate: 273065665.910 evals/sec Expression: (y + x / y) * (x - y / x)
[native] Total Time: 1.28562000  Rate: 252550801.170 evals/sec Expression: x / ((x + y) + (x - y)) / y
[native] Total Time: 1.24748100  Rate: 260271988.912 evals/sec Expression: 1 - ((x * y) + (y / x)) - 3
[native] Total Time: 1.38554600  Rate: 234336760.382 evals/sec Expression: (5.5 + x) + (2 * x - 2 / 3 * y) * (x / 3 + y / 4) + (y + 7.7)
[native] Total Time: 32.31820100  Rate: 10046486.220 evals/sec Expression: 1.1x^1 + 2.2y^2 - 3.3x^3 + 4.4y^15 - 5.5x^23 + 6.6y^55
[native] Total Time: 8.46015600  Rate: 38378058.395 evals/sec Expression: sin(2 * x) + cos(pi / y)
[native] Total Time: 8.48756600  Rate: 38254119.143 evals/sec Expression: 1 - sin(2 * x) + cos(pi / y)
[native] Total Time: 10.27467000  Rate: 31600466.098 evals/sec Expression: sqrt(111.111 - sin(2 * x) + cos(pi / y) / 333.333)
[native] Total Time: 4.35484100  Rate: 74557110.351 evals/sec Expression: (x^2 / sin(2 * pi / y)) - x / 2
[native] Total Time: 24.69386100  Rate: 13148383.762 evals/sec Expression: x + (cos(y - sin(2 / x * pi)) - sin(x - cos(2 * y / pi))) - y
[native] Total Time: 10.85396000  Rate: 29913908.011 evals/sec Expression: clamp(-1.0, sin(2 * pi * x) + cos(y / 2 * pi), +1.0)
[native] Total Time: 9.26353500  Rate: 35049725.726 evals/sec Expression: max(3.33, min(sqrt(1 - sin(2 * x) + cos(pi / y) / 3), 1.11))
[native] Total Time: 1.26572700  Rate: 256520056.063 evals/sec Expression: if((y + (x * 2.2)) <= (x + y + 1.1), x - y, x * y) + 2 * pi / x
--- PARSE ----
[parse] Total Time: 1.14237600  Rate: 87536.853 parse/sec Expression: (y + x)
[parse] Total Time: 1.34356300  Rate: 74428.962 parse/sec Expression: 2 * (y + x)
[parse] Total Time: 1.45291500  Rate: 68827.151 parse/sec Expression: (2 * y + 2 * x)
[parse] Total Time: 1.79423200  Rate: 55734.153 parse/sec Expression: ((1.23 * x^2) / y) - 123.123
[parse] Total Time: 1.92502400  Rate: 51947.404 parse/sec Expression: (y + x / y) * (x - y / x)
[parse] Total Time: 1.93552200  Rate: 51665.649 parse/sec Expression: x / ((x + y) + (x - y)) / y
[parse] Total Time: 1.94771000  Rate: 51342.346 parse/sec Expression: 1 - ((x * y) + (y / x)) - 3
[parse] Total Time: 3.42880300  Rate: 29164.697 parse/sec Expression: (5.5 + x) + (2 * x - 2 / 3 * y) * (x / 3 + y / 4) + (y + 7.7)
[parse] Total Time: 4.08347300  Rate: 24488.958 parse/sec Expression: 1.1x^1 + 2.2y^2 - 3.3x^3 + 4.4y^15 - 5.5x^23 + 6.6y^55
[parse] Total Time: 1.75659000  Rate: 56928.481 parse/sec Expression: sin(2 * x) + cos(pi / y)
[parse] Total Time: 1.95650000  Rate: 51111.679 parse/sec Expression: 1 - sin(2 * x) + cos(pi / y)
[parse] Total Time: 2.31784300  Rate: 43143.561 parse/sec Expression: sqrt(111.111 - sin(2 * x) + cos(pi / y) / 333.333)
[parse] Total Time: 2.13479700  Rate: 46842.861 parse/sec Expression: (x^2 / sin(2 * pi / y)) - x / 2
[parse] Total Time: 3.31120800  Rate: 30200.459 parse/sec Expression: x + (cos(y - sin(2 / x * pi)) - sin(x - cos(2 * y / pi))) - y
[parse] Total Time: 2.61881600  Rate: 38185.195 parse/sec Expression: clamp(-1.0, sin(2 * pi * x) + cos(y / 2 * pi), +1.0)
[parse] Total Time: 2.77748900  Rate: 36003.743 parse/sec Expression: max(3.33, min(sqrt(1 - sin(2 * x) + cos(pi / y) / 3), 1.11))
[parse] Total Time: 3.31818700  Rate: 30136.939 parse/sec Expression: if((y + (x * 2.2)) <= (x + y + 1.1), x - y, x * y) + 2 * pi / x
[parse] Total Time: 0.32454500  Rate: 3081.237 parse/sec Expression: exprtk_special

### Base 2
--- EXPRTK EVAL ---
[exprtk] Total Time: 1.21478200  Rate: 267277882.781 evals/sec Expression: (y + x)
[exprtk] Total Time: 1.30623700  Rate: 248564663.993 evals/sec Expression: 2 * (y + x)
[exprtk] Total Time: 1.20644900  Rate: 269123983.691 evals/sec Expression: (2 * y + 2 * x)
[exprtk] Total Time: 2.55721000  Rate: 126968204.019 evals/sec Expression: ((1.23 * x^2) / y) - 123.123
[exprtk] Total Time: 2.85702900  Rate: 113644055.066 evals/sec Expression: (y + x / y) * (x - y / x)
[exprtk] Total Time: 3.12226000  Rate: 103990174.105 evals/sec Expression: x / ((x + y) + (x - y)) / y
[exprtk] Total Time: 2.05284300  Rate: 158163269.670 evals/sec Expression: 1 - ((x * y) + (y / x)) - 3
[exprtk] Total Time: 5.84184100  Rate: 55579116.412 evals/sec Expression: (5.5 + x) + (2 * x - 2 / 3 * y) * (x / 3 + y / 4) + (y + 7.7)
[exprtk] Total Time: 15.82681700  Rate: 20514823.732 evals/sec Expression: 1.1x^1 + 2.2y^2 - 3.3x^3 + 4.4y^15 - 5.5x^23 + 6.6y^55
[exprtk] Total Time: 13.26786700  Rate: 24471481.437 evals/sec Expression: sin(2 * x) + cos(pi / y)
[exprtk] Total Time: 12.04336200  Rate: 26959611.527 evals/sec Expression: 1 - sin(2 * x) + cos(pi / y)
[exprtk] Total Time: 14.49315900  Rate: 22402594.286 evals/sec Expression: sqrt(111.111 - sin(2 * x) + cos(pi / y) / 333.333)
[exprtk] Total Time: 8.02401200  Rate: 40464092.152 evals/sec Expression: (x^2 / sin(2 * pi / y)) - x / 2
[exprtk] Total Time: 31.04235100  Rate: 10459399.837 evals/sec Expression: x + (cos(y - sin(2 / x * pi)) - sin(x - cos(2 * y / pi))) - y
[exprtk] Total Time: 14.28580900  Rate: 22727754.585 evals/sec Expression: clamp(-1.0, sin(2 * pi * x) + cos(y / 2 * pi), +1.0)
[exprtk] Total Time: 20.32581500  Rate: 15973989.776 evals/sec Expression: max(3.33, min(sqrt(1 - sin(2 * x) + cos(pi / y) / 3), 1.11))
[exprtk] Total Time: 5.42481200  Rate: 59851725.922 evals/sec Expression: if((y + (x * 2.2)) <= (x + y + 1.1), x - y, x * y) + 2 * pi / x
[exprtk] Total Time: 4.67602900  Rate: 427713.344 evals/sec Expression: exprtk_special
--- NATIVE EVAL ---
[native] Total Time: 1.21302100  Rate: 267665902.734 evals/sec Expression: (y + x)
[native] Total Time: 1.21405100  Rate: 267438815.173 evals/sec Expression: 2 * (y + x)
[native] Total Time: 1.21546200  Rate: 267128352.018 evals/sec Expression: (2 * y + 2 * x)
[native] Total Time: 1.24325100  Rate: 261157530.539 evals/sec Expression: ((1.23 * x^2) / y) - 123.123
[native] Total Time: 1.18752500  Rate: 273412653.207 evals/sec Expression: (y + x / y) * (x - y / x)
[native] Total Time: 1.24103200  Rate: 261624487.523 evals/sec Expression: x / ((x + y) + (x - y)) / y
[native] Total Time: 1.23724300  Rate: 262425700.529 evals/sec Expression: 1 - ((x * y) + (y / x)) - 3
[native] Total Time: 1.38781500  Rate: 233953632.869 evals/sec Expression: (5.5 + x) + (2 * x - 2 / 3 * y) * (x / 3 + y / 4) + (y + 7.7)
[native] Total Time: 32.20847400  Rate: 10080712.331 evals/sec Expression: 1.1x^1 + 2.2y^2 - 3.3x^3 + 4.4y^15 - 5.5x^23 + 6.6y^55
[native] Total Time: 8.40978800  Rate: 38607912.708 evals/sec Expression: sin(2 * x) + cos(pi / y)
[native] Total Time: 8.46149400  Rate: 38371989.746 evals/sec Expression: 1 - sin(2 * x) + cos(pi / y)
[native] Total Time: 10.13252700  Rate: 32043769.634 evals/sec Expression: sqrt(111.111 - sin(2 * x) + cos(pi / y) / 333.333)
[native] Total Time: 4.35560000  Rate: 74544118.147 evals/sec Expression: (x^2 / sin(2 * pi / y)) - x / 2
[native] Total Time: 24.64781000  Rate: 13172949.686 evals/sec Expression: x + (cos(y - sin(2 / x * pi)) - sin(x - cos(2 * y / pi))) - y
[native] Total Time: 10.66307800  Rate: 30449403.165 evals/sec Expression: clamp(-1.0, sin(2 * pi * x) + cos(y / 2 * pi), +1.0)
[native] Total Time: 9.15142300  Rate: 35479111.937 evals/sec Expression: max(3.33, min(sqrt(1 - sin(2 * x) + cos(pi / y) / 3), 1.11))
[native] Total Time: 1.27617100  Rate: 254420732.801 evals/sec Expression: if((y + (x * 2.2)) <= (x + y + 1.1), x - y, x * y) + 2 * pi / x
--- PARSE ----
[parse] Total Time: 1.07698400  Rate: 92851.890 parse/sec Expression: (y + x)
[parse] Total Time: 1.28282700  Rate: 77952.834 parse/sec Expression: 2 * (y + x)
[parse] Total Time: 1.44819000  Rate: 69051.713 parse/sec Expression: (2 * y + 2 * x)
[parse] Total Time: 1.83378100  Rate: 54532.139 parse/sec Expression: ((1.23 * x^2) / y) - 123.123
[parse] Total Time: 1.88435300  Rate: 53068.613 parse/sec Expression: (y + x / y) * (x - y / x)
[parse] Total Time: 1.93163500  Rate: 51769.615 parse/sec Expression: x / ((x + y) + (x - y)) / y
[parse] Total Time: 1.90535100  Rate: 52483.768 parse/sec Expression: 1 - ((x * y) + (y / x)) - 3
[parse] Total Time: 3.36993200  Rate: 29674.189 parse/sec Expression: (5.5 + x) + (2 * x - 2 / 3 * y) * (x / 3 + y / 4) + (y + 7.7)
[parse] Total Time: 4.21736400  Rate: 23711.494 parse/sec Expression: 1.1x^1 + 2.2y^2 - 3.3x^3 + 4.4y^15 - 5.5x^23 + 6.6y^55
[parse] Total Time: 1.84364200  Rate: 54240.465 parse/sec Expression: sin(2 * x) + cos(pi / y)
[parse] Total Time: 1.98766500  Rate: 50310.289 parse/sec Expression: 1 - sin(2 * x) + cos(pi / y)
[parse] Total Time: 2.35672300  Rate: 42431.800 parse/sec Expression: sqrt(111.111 - sin(2 * x) + cos(pi / y) / 333.333)
[parse] Total Time: 2.17779700  Rate: 45917.962 parse/sec Expression: (x^2 / sin(2 * pi / y)) - x / 2
[parse] Total Time: 3.32623500  Rate: 30064.021 parse/sec Expression: x + (cos(y - sin(2 / x * pi)) - sin(x - cos(2 * y / pi))) - y
[parse] Total Time: 2.61213600  Rate: 38282.846 parse/sec Expression: clamp(-1.0, sin(2 * pi * x) + cos(y / 2 * pi), +1.0)
[parse] Total Time: 2.77991500  Rate: 35972.323 parse/sec Expression: max(3.33, min(sqrt(1 - sin(2 * x) + cos(pi / y) / 3), 1.11))
[parse] Total Time: 3.24468900  Rate: 30819.595 parse/sec Expression: if((y + (x * 2.2)) <= (x + y + 1.1), x - y, x * y) + 2 * pi / x
[parse] Total Time: 0.38052700  Rate: 2627.934 parse/sec Expression: exprtk_special

### Optimized 2
--- EXPRTK EVAL ---
[exprtk] Total Time: 1.20664200  Rate: 269080937.842 evals/sec Expression: (y + x)
[exprtk] Total Time: 1.16349900  Rate: 279058564.726 evals/sec Expression: 2 * (y + x)
[exprtk] Total Time: 1.18386800  Rate: 274257232.225 evals/sec Expression: (2 * y + 2 * x)
[exprtk] Total Time: 2.74123900  Rate: 118444382.631 evals/sec Expression: ((1.23 * x^2) / y) - 123.123
[exprtk] Total Time: 2.87711000  Rate: 112850868.059 evals/sec Expression: (y + x / y) * (x - y / x)
[exprtk] Total Time: 3.12716200  Rate: 103827163.735 evals/sec Expression: x / ((x + y) + (x - y)) / y
[exprtk] Total Time: 2.06203500  Rate: 157458220.156 evals/sec Expression: 1 - ((x * y) + (y / x)) - 3
[exprtk] Total Time: 5.77603900  Rate: 56212286.828 evals/sec Expression: (5.5 + x) + (2 * x - 2 / 3 * y) * (x / 3 + y / 4) + (y + 7.7)
[exprtk] Total Time: 15.90866000  Rate: 20409284.063 evals/sec Expression: 1.1x^1 + 2.2y^2 - 3.3x^3 + 4.4y^15 - 5.5x^23 + 6.6y^55
[exprtk] Total Time: 11.98141200  Rate: 27099006.444 evals/sec Expression: sin(2 * x) + cos(pi / y)
[exprtk] Total Time: 11.96103000  Rate: 27145184.069 evals/sec Expression: 1 - sin(2 * x) + cos(pi / y)
[exprtk] Total Time: 14.41338400  Rate: 22526587.858 evals/sec Expression: sqrt(111.111 - sin(2 * x) + cos(pi / y) / 333.333)
[exprtk] Total Time: 7.90448900  Rate: 41075945.706 evals/sec Expression: (x^2 / sin(2 * pi / y)) - x / 2
[exprtk] Total Time: 31.01410200  Rate: 10468926.716 evals/sec Expression: x + (cos(y - sin(2 / x * pi)) - sin(x - cos(2 * y / pi))) - y
[exprtk] Total Time: 14.36623100  Rate: 22600524.870 evals/sec Expression: clamp(-1.0, sin(2 * pi * x) + cos(y / 2 * pi), +1.0)
[exprtk] Total Time: 20.20877000  Rate: 16066507.808 evals/sec Expression: max(3.33, min(sqrt(1 - sin(2 * x) + cos(pi / y) / 3), 1.11))
[exprtk] Total Time: 5.46608400  Rate: 59399811.821 evals/sec Expression: if((y + (x * 2.2)) <= (x + y + 1.1), x - y, x * y) + 2 * pi / x
[exprtk] Total Time: 4.75731100  Rate: 420405.561 evals/sec Expression: exprtk_special
--- NATIVE EVAL ---
[native] Total Time: 1.21380400  Rate: 267493236.964 evals/sec Expression: (y + x)
[native] Total Time: 1.21140200  Rate: 268023629.646 evals/sec Expression: 2 * (y + x)
[native] Total Time: 1.21268500  Rate: 267740065.227 evals/sec Expression: (2 * y + 2 * x)
[native] Total Time: 1.23935200  Rate: 261979131.837 evals/sec Expression: ((1.23 * x^2) / y) - 123.123
[native] Total Time: 1.19646700  Rate: 271369257.155 evals/sec Expression: (y + x / y) * (x - y / x)
[native] Total Time: 1.27952400  Rate: 253754021.808 evals/sec Expression: x / ((x + y) + (x - y)) / y
[native] Total Time: 1.24182300  Rate: 261457841.415 evals/sec Expression: 1 - ((x * y) + (y / x)) - 3
[native] Total Time: 1.39595100  Rate: 232590084.466 evals/sec Expression: (5.5 + x) + (2 * x - 2 / 3 * y) * (x / 3 + y / 4) + (y + 7.7)
[native] Total Time: 32.24404700  Rate: 10069590.861 evals/sec Expression: 1.1x^1 + 2.2y^2 - 3.3x^3 + 4.4y^15 - 5.5x^23 + 6.6y^55
[native] Total Time: 8.45609100  Rate: 38396507.441 evals/sec Expression: sin(2 * x) + cos(pi / y)
[native] Total Time: 8.59396800  Rate: 37780494.528 evals/sec Expression: 1 - sin(2 * x) + cos(pi / y)
[native] Total Time: 10.31330500  Rate: 31482086.586 evals/sec Expression: sqrt(111.111 - sin(2 * x) + cos(pi / y) / 333.333)
[native] Total Time: 4.36795200  Rate: 74333317.079 evals/sec Expression: (x^2 / sin(2 * pi / y)) - x / 2
[native] Total Time: 24.81673800  Rate: 13083281.171 evals/sec Expression: x + (cos(y - sin(2 / x * pi)) - sin(x - cos(2 * y / pi))) - y
[native] Total Time: 10.75870900  Rate: 30178747.376 evals/sec Expression: clamp(-1.0, sin(2 * pi * x) + cos(y / 2 * pi), +1.0)
[native] Total Time: 9.24483100  Rate: 35120637.792 evals/sec Expression: max(3.33, min(sqrt(1 - sin(2 * x) + cos(pi / y) / 3), 1.11))
[native] Total Time: 1.27357100  Rate: 254940133.687 evals/sec Expression: if((y + (x * 2.2)) <= (x + y + 1.1), x - y, x * y) + 2 * pi / x
--- PARSE ----
[parse] Total Time: 1.08502600  Rate: 92163.690 parse/sec Expression: (y + x)
[parse] Total Time: 1.26857100  Rate: 78828.855 parse/sec Expression: 2 * (y + x)
[parse] Total Time: 1.40818300  Rate: 71013.498 parse/sec Expression: (2 * y + 2 * x)
[parse] Total Time: 1.74516900  Rate: 57301.041 parse/sec Expression: ((1.23 * x^2) / y) - 123.123
[parse] Total Time: 1.85754000  Rate: 53834.642 parse/sec Expression: (y + x / y) * (x - y / x)
[parse] Total Time: 1.91433800  Rate: 52237.379 parse/sec Expression: x / ((x + y) + (x - y)) / y
[parse] Total Time: 1.88697300  Rate: 52994.929 parse/sec Expression: 1 - ((x * y) + (y / x)) - 3
[parse] Total Time: 3.21650800  Rate: 31089.616 parse/sec Expression: (5.5 + x) + (2 * x - 2 / 3 * y) * (x / 3 + y / 4) + (y + 7.7)
[parse] Total Time: 3.94762200  Rate: 25331.706 parse/sec Expression: 1.1x^1 + 2.2y^2 - 3.3x^3 + 4.4y^15 - 5.5x^23 + 6.6y^55
[parse] Total Time: 1.70721400  Rate: 58574.965 parse/sec Expression: sin(2 * x) + cos(pi / y)
[parse] Total Time: 1.88452700  Rate: 53063.713 parse/sec Expression: 1 - sin(2 * x) + cos(pi / y)
[parse] Total Time: 2.34604600  Rate: 42624.910 parse/sec Expression: sqrt(111.111 - sin(2 * x) + cos(pi / y) / 333.333)
[parse] Total Time: 2.20877600  Rate: 45273.944 parse/sec Expression: (x^2 / sin(2 * pi / y)) - x / 2
[parse] Total Time: 3.25629500  Rate: 30709.748 parse/sec Expression: x + (cos(y - sin(2 / x * pi)) - sin(x - cos(2 * y / pi))) - y
[parse] Total Time: 2.54384600  Rate: 39310.556 parse/sec Expression: clamp(-1.0, sin(2 * pi * x) + cos(y / 2 * pi), +1.0)
[parse] Total Time: 2.67202000  Rate: 37424.870 parse/sec Expression: max(3.33, min(sqrt(1 - sin(2 * x) + cos(pi / y) / 3), 1.11))
[parse] Total Time: 3.16608500  Rate: 31584.749 parse/sec Expression: if((y + (x * 2.2)) <= (x + y + 1.1), x - y, x * y) + 2 * pi / x
[parse] Total Time: 0.32582800  Rate: 3069.104 parse/sec Expression: exprtk_special

### Base 3
--- EXPRTK EVAL ---
[exprtk] Total Time: 1.20982900  Rate: 268372109.612 evals/sec Expression: (y + x)
[exprtk] Total Time: 1.19806500  Rate: 271007300.105 evals/sec Expression: 2 * (y + x)
[exprtk] Total Time: 1.19425100  Rate: 271872798.097 evals/sec Expression: (2 * y + 2 * x)
[exprtk] Total Time: 2.60869500  Rate: 124462369.499 evals/sec Expression: ((1.23 * x^2) / y) - 123.123
[exprtk] Total Time: 3.07968900  Rate: 105427645.779 evals/sec Expression: (y + x / y) * (x - y / x)
[exprtk] Total Time: 3.16924800  Rate: 102448391.858 evals/sec Expression: x / ((x + y) + (x - y)) / y
[exprtk] Total Time: 2.06764500  Rate: 157030999.519 evals/sec Expression: 1 - ((x * y) + (y / x)) - 3
[exprtk] Total Time: 5.90299800  Rate: 55003298.493 evals/sec Expression: (5.5 + x) + (2 * x - 2 / 3 * y) * (x / 3 + y / 4) + (y + 7.7)
[exprtk] Total Time: 15.84641100  Rate: 20489457.266 evals/sec Expression: 1.1x^1 + 2.2y^2 - 3.3x^3 + 4.4y^15 - 5.5x^23 + 6.6y^55
[exprtk] Total Time: 11.55105000  Rate: 28108644.755 evals/sec Expression: sin(2 * x) + cos(pi / y)
[exprtk] Total Time: 12.02441300  Rate: 27002096.568 evals/sec Expression: 1 - sin(2 * x) + cos(pi / y)
[exprtk] Total Time: 14.36840900  Rate: 22597099.025 evals/sec Expression: sqrt(111.111 - sin(2 * x) + cos(pi / y) / 333.333)
[exprtk] Total Time: 7.89926600  Rate: 41103105.149 evals/sec Expression: (x^2 / sin(2 * pi / y)) - x / 2
[exprtk] Total Time: 31.02705500  Rate: 10464556.207 evals/sec Expression: x + (cos(y - sin(2 / x * pi)) - sin(x - cos(2 * y / pi))) - y
[exprtk] Total Time: 15.67968300  Rate: 20707329.415 evals/sec Expression: clamp(-1.0, sin(2 * pi * x) + cos(y / 2 * pi), +1.0)
[exprtk] Total Time: 20.31878100  Rate: 15979519.687 evals/sec Expression: max(3.33, min(sqrt(1 - sin(2 * x) + cos(pi / y) / 3), 1.11))
[exprtk] Total Time: 5.42228300  Rate: 59879641.288 evals/sec Expression: if((y + (x * 2.2)) <= (x + y + 1.1), x - y, x * y) + 2 * pi / x
[exprtk] Total Time: 4.66818000  Rate: 428432.494 evals/sec Expression: exprtk_special
--- NATIVE EVAL ---
[native] Total Time: 1.21065900  Rate: 268188119.859 evals/sec Expression: (y + x)
[native] Total Time: 1.20613100  Rate: 269194939.024 evals/sec Expression: 2 * (y + x)
[native] Total Time: 1.20638800  Rate: 269137591.720 evals/sec Expression: (2 * y + 2 * x)
[native] Total Time: 1.20999200  Rate: 268335956.767 evals/sec Expression: ((1.23 * x^2) / y) - 123.123
[native] Total Time: 1.19163200  Rate: 272470327.249 evals/sec Expression: (y + x / y) * (x - y / x)
[native] Total Time: 1.25191800  Rate: 259349542.861 evals/sec Expression: x / ((x + y) + (x - y)) / y
[native] Total Time: 1.25483300  Rate: 258747069.132 evals/sec Expression: 1 - ((x * y) + (y / x)) - 3
[native] Total Time: 1.38479000  Rate: 234464692.119 evals/sec Expression: (5.5 + x) + (2 * x - 2 / 3 * y) * (x / 3 + y / 4) + (y + 7.7)
[native] Total Time: 32.17376800  Rate: 10091586.444 evals/sec Expression: 1.1x^1 + 2.2y^2 - 3.3x^3 + 4.4y^15 - 5.5x^23 + 6.6y^55
[native] Total Time: 8.45867500  Rate: 38384777.876 evals/sec Expression: sin(2 * x) + cos(pi / y)
[native] Total Time: 8.51683100  Rate: 38122672.741 evals/sec Expression: 1 - sin(2 * x) + cos(pi / y)
[native] Total Time: 10.21752900  Rate: 31777190.062 evals/sec Expression: sqrt(111.111 - sin(2 * x) + cos(pi / y) / 333.333)
[native] Total Time: 4.36518500  Rate: 74380435.423 evals/sec Expression: (x^2 / sin(2 * pi / y)) - x / 2
[native] Total Time: 24.78181800  Rate: 13101716.791 evals/sec Expression: x + (cos(y - sin(2 / x * pi)) - sin(x - cos(2 * y / pi))) - y
[native] Total Time: 10.67525600  Rate: 30414667.433 evals/sec Expression: clamp(-1.0, sin(2 * pi * x) + cos(y / 2 * pi), +1.0)
[native] Total Time: 9.17550100  Rate: 35386009.004 evals/sec Expression: max(3.33, min(sqrt(1 - sin(2 * x) + cos(pi / y) / 3), 1.11))
[native] Total Time: 1.27446900  Rate: 254760501.040 evals/sec Expression: if((y + (x * 2.2)) <= (x + y + 1.1), x - y, x * y) + 2 * pi / x
--- PARSE ----
[parse] Total Time: 1.09158600  Rate: 91609.823 parse/sec Expression: (y + x)
[parse] Total Time: 1.29264300  Rate: 77360.880 parse/sec Expression: 2 * (y + x)
[parse] Total Time: 1.44895400  Rate: 69015.303 parse/sec Expression: (2 * y + 2 * x)
[parse] Total Time: 1.84496600  Rate: 54201.541 parse/sec Expression: ((1.23 * x^2) / y) - 123.123
[parse] Total Time: 1.93957500  Rate: 51557.687 parse/sec Expression: (y + x / y) * (x - y / x)
[parse] Total Time: 1.97500600  Rate: 50632.758 parse/sec Expression: x / ((x + y) + (x - y)) / y
[parse] Total Time: 1.98138000  Rate: 50469.875 parse/sec Expression: 1 - ((x * y) + (y / x)) - 3
[parse] Total Time: 3.41523800  Rate: 29280.536 parse/sec Expression: (5.5 + x) + (2 * x - 2 / 3 * y) * (x / 3 + y / 4) + (y + 7.7)
[parse] Total Time: 4.13180200  Rate: 24202.515 parse/sec Expression: 1.1x^1 + 2.2y^2 - 3.3x^3 + 4.4y^15 - 5.5x^23 + 6.6y^55
[parse] Total Time: 1.76578200  Rate: 56632.132 parse/sec Expression: sin(2 * x) + cos(pi / y)
[parse] Total Time: 1.94206400  Rate: 51491.609 parse/sec Expression: 1 - sin(2 * x) + cos(pi / y)
[parse] Total Time: 2.32941100  Rate: 42929.307 parse/sec Expression: sqrt(111.111 - sin(2 * x) + cos(pi / y) / 333.333)
[parse] Total Time: 2.21485600  Rate: 45149.662 parse/sec Expression: (x^2 / sin(2 * pi / y)) - x / 2
[parse] Total Time: 3.36416900  Rate: 29725.023 parse/sec Expression: x + (cos(y - sin(2 / x * pi)) - sin(x - cos(2 * y / pi))) - y
[parse] Total Time: 2.63783900  Rate: 37909.819 parse/sec Expression: clamp(-1.0, sin(2 * pi * x) + cos(y / 2 * pi), +1.0)
[parse] Total Time: 2.80182300  Rate: 35691.048 parse/sec Expression: max(3.33, min(sqrt(1 - sin(2 * x) + cos(pi / y) / 3), 1.11))
[parse] Total Time: 3.36984800  Rate: 29674.929 parse/sec Expression: if((y + (x * 2.2)) <= (x + y + 1.1), x - y, x * y) + 2 * pi / x
[parse] Total Time: 0.35123800  Rate: 2847.072 parse/sec Expression: exprtk_special

### Optimized 3
--- EXPRTK EVAL ---
[exprtk] Total Time: 1.20694800  Rate: 269012717.201 evals/sec Expression: (y + x)
[exprtk] Total Time: 1.13075500  Rate: 287139443.115 evals/sec Expression: 2 * (y + x)
[exprtk] Total Time: 1.13238100  Rate: 286727136.008 evals/sec Expression: (2 * y + 2 * x)
[exprtk] Total Time: 2.66728900  Rate: 121728227.050 evals/sec Expression: ((1.23 * x^2) / y) - 123.123
[exprtk] Total Time: 2.88524600  Rate: 112532644.010 evals/sec Expression: (y + x / y) * (x - y / x)
[exprtk] Total Time: 3.11609600  Rate: 104195878.753 evals/sec Expression: x / ((x + y) + (x - y)) / y
[exprtk] Total Time: 2.06225100  Rate: 157441727.995 evals/sec Expression: 1 - ((x * y) + (y / x)) - 3
[exprtk] Total Time: 5.88792700  Rate: 55144087.384 evals/sec Expression: (5.5 + x) + (2 * x - 2 / 3 * y) * (x / 3 + y / 4) + (y + 7.7)
[exprtk] Total Time: 15.15449400  Rate: 21424955.594 evals/sec Expression: 1.1x^1 + 2.2y^2 - 3.3x^3 + 4.4y^15 - 5.5x^23 + 6.6y^55
[exprtk] Total Time: 11.26341800  Rate: 28826450.461 evals/sec Expression: sin(2 * x) + cos(pi / y)
[exprtk] Total Time: 12.09137600  Rate: 26852556.814 evals/sec Expression: 1 - sin(2 * x) + cos(pi / y)
[exprtk] Total Time: 14.85097400  Rate: 21862832.768 evals/sec Expression: sqrt(111.111 - sin(2 * x) + cos(pi / y) / 333.333)
[exprtk] Total Time: 7.96532600  Rate: 40762218.772 evals/sec Expression: (x^2 / sin(2 * pi / y)) - x / 2
[exprtk] Total Time: 31.72563200  Rate: 10234133.744 evals/sec Expression: x + (cos(y - sin(2 / x * pi)) - sin(x - cos(2 * y / pi))) - y
[exprtk] Total Time: 14.52728500  Rate: 22349968.421 evals/sec Expression: clamp(-1.0, sin(2 * pi * x) + cos(y / 2 * pi), +1.0)
[exprtk] Total Time: 20.34910700  Rate: 15955705.624 evals/sec Expression: max(3.33, min(sqrt(1 - sin(2 * x) + cos(pi / y) / 3), 1.11))
[exprtk] Total Time: 5.43033400  Rate: 59790863.877 evals/sec Expression: if((y + (x * 2.2)) <= (x + y + 1.1), x - y, x * y) + 2 * pi / x
[exprtk] Total Time: 4.84089200  Rate: 413146.999 evals/sec Expression: exprtk_special
--- NATIVE EVAL ---
[native] Total Time: 1.21221800  Rate: 267843210.545 evals/sec Expression: (y + x)
[native] Total Time: 1.21323200  Rate: 267619351.451 evals/sec Expression: 2 * (y + x)
[native] Total Time: 1.21237300  Rate: 267808967.207 evals/sec Expression: (2 * y + 2 * x)
[native] Total Time: 1.21978900  Rate: 266180758.311 evals/sec Expression: ((1.23 * x^2) / y) - 123.123
[native] Total Time: 1.19432500  Rate: 271855952.944 evals/sec Expression: (y + x / y) * (x - y / x)
[native] Total Time: 1.29478500  Rate: 250763146.777 evals/sec Expression: x / ((x + y) + (x - y)) / y
[native] Total Time: 1.25800600  Rate: 258094445.495 evals/sec Expression: 1 - ((x * y) + (y / x)) - 3
[native] Total Time: 1.40286300  Rate: 231444097.535 evals/sec Expression: (5.5 + x) + (2 * x - 2 / 3 * y) * (x / 3 + y / 4) + (y + 7.7)
[native] Total Time: 32.51817500  Rate: 9984704.277 evals/sec Expression: 1.1x^1 + 2.2y^2 - 3.3x^3 + 4.4y^15 - 5.5x^23 + 6.6y^55
[native] Total Time: 8.50938400  Rate: 38156035.854 evals/sec Expression: sin(2 * x) + cos(pi / y)
[native] Total Time: 8.44553900  Rate: 38444480.690 evals/sec Expression: 1 - sin(2 * x) + cos(pi / y)
[native] Total Time: 10.26157000  Rate: 31640807.498 evals/sec Expression: sqrt(111.111 - sin(2 * x) + cos(pi / y) / 333.333)
[native] Total Time: 4.32984300  Rate: 74987559.826 evals/sec Expression: (x^2 / sin(2 * pi / y)) - x / 2
[native] Total Time: 24.74485300  Rate: 13121288.738 evals/sec Expression: x + (cos(y - sin(2 / x * pi)) - sin(x - cos(2 * y / pi))) - y
[native] Total Time: 10.79333900  Rate: 30081920.062 evals/sec Expression: clamp(-1.0, sin(2 * pi * x) + cos(y / 2 * pi), +1.0)
[native] Total Time: 9.25627100  Rate: 35077231.533 evals/sec Expression: max(3.33, min(sqrt(1 - sin(2 * x) + cos(pi / y) / 3), 1.11))
[native] Total Time: 1.27788900  Rate: 254078688.368 evals/sec Expression: if((y + (x * 2.2)) <= (x + y + 1.1), x - y, x * y) + 2 * pi / x
--- PARSE ----
[parse] Total Time: 1.11015500  Rate: 90077.512 parse/sec Expression: (y + x)
[parse] Total Time: 1.28782900  Rate: 77650.061 parse/sec Expression: 2 * (y + x)
[parse] Total Time: 1.42533200  Rate: 70159.093 parse/sec Expression: (2 * y + 2 * x)
[parse] Total Time: 1.78416700  Rate: 56048.565 parse/sec Expression: ((1.23 * x^2) / y) - 123.123
[parse] Total Time: 1.91755400  Rate: 52149.770 parse/sec Expression: (y + x / y) * (x - y / x)
[parse] Total Time: 1.97038400  Rate: 50751.529 parse/sec Expression: x / ((x + y) + (x - y)) / y
[parse] Total Time: 1.96075100  Rate: 51000.867 parse/sec Expression: 1 - ((x * y) + (y / x)) - 3
[parse] Total Time: 3.26424600  Rate: 30634.946 parse/sec Expression: (5.5 + x) + (2 * x - 2 / 3 * y) * (x / 3 + y / 4) + (y + 7.7)
[parse] Total Time: 3.96886800  Rate: 25196.101 parse/sec Expression: 1.1x^1 + 2.2y^2 - 3.3x^3 + 4.4y^15 - 5.5x^23 + 6.6y^55
[parse] Total Time: 1.75427400  Rate: 57003.638 parse/sec Expression: sin(2 * x) + cos(pi / y)
[parse] Total Time: 1.98071200  Rate: 50486.896 parse/sec Expression: 1 - sin(2 * x) + cos(pi / y)
[parse] Total Time: 2.31613700  Rate: 43175.339 parse/sec Expression: sqrt(111.111 - sin(2 * x) + cos(pi / y) / 333.333)
[parse] Total Time: 2.14042800  Rate: 46719.628 parse/sec Expression: (x^2 / sin(2 * pi / y)) - x / 2
[parse] Total Time: 3.31529600  Rate: 30163.219 parse/sec Expression: x + (cos(y - sin(2 / x * pi)) - sin(x - cos(2 * y / pi))) - y
[parse] Total Time: 2.58919300  Rate: 38622.073 parse/sec Expression: clamp(-1.0, sin(2 * pi * x) + cos(y / 2 * pi), +1.0)
[parse] Total Time: 2.72758100  Rate: 36662.523 parse/sec Expression: max(3.33, min(sqrt(1 - sin(2 * x) + cos(pi / y) / 3), 1.11))
[parse] Total Time: 3.22327500  Rate: 31024.346 parse/sec Expression: if((y + (x * 2.2)) <= (x + y + 1.1), x - y, x * y) + 2 * pi / x
[parse] Total Time: 0.33457400  Rate: 2988.875 parse/sec Expression: exprtk_special

### Base 4
--- EXPRTK EVAL ---
[exprtk] Total Time: 1.22196100  Rate: 265707629.785 evals/sec Expression: (y + x)
[exprtk] Total Time: 1.21670200  Rate: 266856108.562 evals/sec Expression: 2 * (y + x)
[exprtk] Total Time: 1.20225800  Rate: 270062133.918 evals/sec Expression: (2 * y + 2 * x)
[exprtk] Total Time: 2.65889300  Rate: 122112608.894 evals/sec Expression: ((1.23 * x^2) / y) - 123.123
[exprtk] Total Time: 3.37089800  Rate: 96319841.478 evals/sec Expression: (y + x / y) * (x - y / x)
[exprtk] Total Time: 3.09191800  Rate: 105010663.608 evals/sec Expression: x / ((x + y) + (x - y)) / y
[exprtk] Total Time: 2.03191300  Rate: 159792452.236 evals/sec Expression: 1 - ((x * y) + (y / x)) - 3
[exprtk] Total Time: 5.80203200  Rate: 55960456.785 evals/sec Expression: (5.5 + x) + (2 * x - 2 / 3 * y) * (x / 3 + y / 4) + (y + 7.7)
[exprtk] Total Time: 15.79898700  Rate: 20550960.704 evals/sec Expression: 1.1x^1 + 2.2y^2 - 3.3x^3 + 4.4y^15 - 5.5x^23 + 6.6y^55
[exprtk] Total Time: 11.29153500  Rate: 28754669.848 evals/sec Expression: sin(2 * x) + cos(pi / y)
[exprtk] Total Time: 12.35132900  Rate: 26287402.837 evals/sec Expression: 1 - sin(2 * x) + cos(pi / y)
[exprtk] Total Time: 14.84946500  Rate: 21865054.465 evals/sec Expression: sqrt(111.111 - sin(2 * x) + cos(pi / y) / 333.333)
[exprtk] Total Time: 7.92955800  Rate: 40946085.646 evals/sec Expression: (x^2 / sin(2 * pi / y)) - x / 2
[exprtk] Total Time: 30.98047300  Rate: 10480290.633 evals/sec Expression: x + (cos(y - sin(2 / x * pi)) - sin(x - cos(2 * y / pi))) - y
[exprtk] Total Time: 14.42677500  Rate: 22505678.573 evals/sec Expression: clamp(-1.0, sin(2 * pi * x) + cos(y / 2 * pi), +1.0)
[exprtk] Total Time: 20.30064500  Rate: 15993795.320 evals/sec Expression: max(3.33, min(sqrt(1 - sin(2 * x) + cos(pi / y) / 3), 1.11))
[exprtk] Total Time: 5.93156400  Rate: 54738406.430 evals/sec Expression: if((y + (x * 2.2)) <= (x + y + 1.1), x - y, x * y) + 2 * pi / x
[exprtk] Total Time: 4.67698000  Rate: 427626.374 evals/sec Expression: exprtk_special
--- NATIVE EVAL ---
[native] Total Time: 1.21250200  Rate: 267780474.589 evals/sec Expression: (y + x)
[native] Total Time: 1.21076200  Rate: 268165304.990 evals/sec Expression: 2 * (y + x)
[native] Total Time: 1.20908200  Rate: 268537916.370 evals/sec Expression: (2 * y + 2 * x)
[native] Total Time: 1.21591300  Rate: 267029270.186 evals/sec Expression: ((1.23 * x^2) / y) - 123.123
[native] Total Time: 1.18731900  Rate: 273460090.338 evals/sec Expression: (y + x / y) * (x - y / x)
[native] Total Time: 1.24378000  Rate: 261046455.965 evals/sec Expression: x / ((x + y) + (x - y)) / y
[native] Total Time: 1.23493900  Rate: 262915302.699 evals/sec Expression: 1 - ((x * y) + (y / x)) - 3
[native] Total Time: 1.40482000  Rate: 231121681.781 evals/sec Expression: (5.5 + x) + (2 * x - 2 / 3 * y) * (x / 3 + y / 4) + (y + 7.7)
[native] Total Time: 32.20647600  Rate: 10081337.710 evals/sec Expression: 1.1x^1 + 2.2y^2 - 3.3x^3 + 4.4y^15 - 5.5x^23 + 6.6y^55
[native] Total Time: 8.38770000  Rate: 38709582.007 evals/sec Expression: sin(2 * x) + cos(pi / y)
[native] Total Time: 8.45674900  Rate: 38393519.898 evals/sec Expression: 1 - sin(2 * x) + cos(pi / y)
[native] Total Time: 10.20830400  Rate: 31805906.348 evals/sec Expression: sqrt(111.111 - sin(2 * x) + cos(pi / y) / 333.333)
[native] Total Time: 4.36382100  Rate: 74403684.523 evals/sec Expression: (x^2 / sin(2 * pi / y)) - x / 2
[native] Total Time: 24.66497400  Rate: 13163782.820 evals/sec Expression: x + (cos(y - sin(2 / x * pi)) - sin(x - cos(2 * y / pi))) - y
[native] Total Time: 10.71856400  Rate: 30291777.984 evals/sec Expression: clamp(-1.0, sin(2 * pi * x) + cos(y / 2 * pi), +1.0)
[native] Total Time: 9.14279700  Rate: 35512585.591 evals/sec Expression: max(3.33, min(sqrt(1 - sin(2 * x) + cos(pi / y) / 3), 1.11))
[native] Total Time: 1.26981000  Rate: 255695230.783 evals/sec Expression: if((y + (x * 2.2)) <= (x + y + 1.1), x - y, x * y) + 2 * pi / x
--- PARSE ----
[parse] Total Time: 1.10380800  Rate: 90595.466 parse/sec Expression: (y + x)
[parse] Total Time: 1.29629200  Rate: 77143.113 parse/sec Expression: 2 * (y + x)
[parse] Total Time: 1.46152800  Rate: 68421.542 parse/sec Expression: (2 * y + 2 * x)
[parse] Total Time: 1.84470600  Rate: 54209.180 parse/sec Expression: ((1.23 * x^2) / y) - 123.123
[parse] Total Time: 1.95237700  Rate: 51219.616 parse/sec Expression: (y + x / y) * (x - y / x)
[parse] Total Time: 2.00137200  Rate: 49965.724 parse/sec Expression: x / ((x + y) + (x - y)) / y
[parse] Total Time: 2.03281700  Rate: 49192.820 parse/sec Expression: 1 - ((x * y) + (y / x)) - 3
[parse] Total Time: 3.38959400  Rate: 29502.058 parse/sec Expression: (5.5 + x) + (2 * x - 2 / 3 * y) * (x / 3 + y / 4) + (y + 7.7)
[parse] Total Time: 4.11021500  Rate: 24329.628 parse/sec Expression: 1.1x^1 + 2.2y^2 - 3.3x^3 + 4.4y^15 - 5.5x^23 + 6.6y^55
[parse] Total Time: 1.77138700  Rate: 56452.938 parse/sec Expression: sin(2 * x) + cos(pi / y)
[parse] Total Time: 1.97600600  Rate: 50607.134 parse/sec Expression: 1 - sin(2 * x) + cos(pi / y)
[parse] Total Time: 2.34602800  Rate: 42625.237 parse/sec Expression: sqrt(111.111 - sin(2 * x) + cos(pi / y) / 333.333)
[parse] Total Time: 2.16137900  Rate: 46266.758 parse/sec Expression: (x^2 / sin(2 * pi / y)) - x / 2
[parse] Total Time: 3.41552600  Rate: 29278.067 parse/sec Expression: x + (cos(y - sin(2 / x * pi)) - sin(x - cos(2 * y / pi))) - y
[parse] Total Time: 2.71147400  Rate: 36880.309 parse/sec Expression: clamp(-1.0, sin(2 * pi * x) + cos(y / 2 * pi), +1.0)
[parse] Total Time: 2.80672000  Rate: 35628.777 parse/sec Expression: max(3.33, min(sqrt(1 - sin(2 * x) + cos(pi / y) / 3), 1.11))
[parse] Total Time: 3.35354200  Rate: 29819.218 parse/sec Expression: if((y + (x * 2.2)) <= (x + y + 1.1), x - y, x * y) + 2 * pi / x
[parse] Total Time: 0.35319800  Rate: 2831.273 parse/sec Expression: exprtk_special

### Optimized 4
--- EXPRTK EVAL ---
[exprtk] Total Time: 1.22039300  Rate: 266049019.455 evals/sec Expression: (y + x)
[exprtk] Total Time: 1.14515300  Rate: 283529241.071 evals/sec Expression: 2 * (y + x)
[exprtk] Total Time: 1.29103500  Rate: 251491525.017 evals/sec Expression: (2 * y + 2 * x)
[exprtk] Total Time: 2.72538700  Rate: 119133305.105 evals/sec Expression: ((1.23 * x^2) / y) - 123.123
[exprtk] Total Time: 2.96135800  Rate: 109640361.280 evals/sec Expression: (y + x / y) * (x - y / x)
[exprtk] Total Time: 3.57977800  Rate: 90699579.974 evals/sec Expression: x / ((x + y) + (x - y)) / y
[exprtk] Total Time: 2.04705900  Rate: 158610162.677 evals/sec Expression: 1 - ((x * y) + (y / x)) - 3
[exprtk] Total Time: 5.80325400  Rate: 55948673.107 evals/sec Expression: (5.5 + x) + (2 * x - 2 / 3 * y) * (x / 3 + y / 4) + (y + 7.7)
[exprtk] Total Time: 15.15562800  Rate: 21423352.500 evals/sec Expression: 1.1x^1 + 2.2y^2 - 3.3x^3 + 4.4y^15 - 5.5x^23 + 6.6y^55
[exprtk] Total Time: 11.30053300  Rate: 28731774.068 evals/sec Expression: sin(2 * x) + cos(pi / y)
[exprtk] Total Time: 12.08578000  Rate: 26864990.179 evals/sec Expression: 1 - sin(2 * x) + cos(pi / y)
[exprtk] Total Time: 14.46356300  Rate: 22448435.493 evals/sec Expression: sqrt(111.111 - sin(2 * x) + cos(pi / y) / 333.333)
[exprtk] Total Time: 7.92515800  Rate: 40968818.666 evals/sec Expression: (x^2 / sin(2 * pi / y)) - x / 2
[exprtk] Total Time: 31.07135100  Rate: 10449637.706 evals/sec Expression: x + (cos(y - sin(2 / x * pi)) - sin(x - cos(2 * y / pi))) - y
[exprtk] Total Time: 14.96109400  Rate: 21701913.042 evals/sec Expression: clamp(-1.0, sin(2 * pi * x) + cos(y / 2 * pi), +1.0)
[exprtk] Total Time: 20.28313400  Rate: 16007603.214 evals/sec Expression: max(3.33, min(sqrt(1 - sin(2 * x) + cos(pi / y) / 3), 1.11))
[exprtk] Total Time: 5.39226800  Rate: 60212949.542 evals/sec Expression: if((y + (x * 2.2)) <= (x + y + 1.1), x - y, x * y) + 2 * pi / x
[exprtk] Total Time: 4.69508900  Rate: 425977.016 evals/sec Expression: exprtk_special
--- NATIVE EVAL ---
[native] Total Time: 1.21358200  Rate: 267542169.380 evals/sec Expression: (y + x)
[native] Total Time: 1.21172600  Rate: 267951963.563 evals/sec Expression: 2 * (y + x)
[native] Total Time: 1.21174500  Rate: 267947762.112 evals/sec Expression: (2 * y + 2 * x)
[native] Total Time: 1.21372900  Rate: 267509766.183 evals/sec Expression: ((1.23 * x^2) / y) - 123.123
[native] Total Time: 1.18745200  Rate: 273429461.570 evals/sec Expression: (y + x / y) * (x - y / x)
[native] Total Time: 1.28077900  Rate: 253505375.244 evals/sec Expression: x / ((x + y) + (x - y)) / y
[native] Total Time: 1.23695800  Rate: 262486164.445 evals/sec Expression: 1 - ((x * y) + (y / x)) - 3
[native] Total Time: 1.38014400  Rate: 235253974.223 evals/sec Expression: (5.5 + x) + (2 * x - 2 / 3 * y) * (x / 3 + y / 4) + (y + 7.7)
[native] Total Time: 32.21368600  Rate: 10079081.326 evals/sec Expression: 1.1x^1 + 2.2y^2 - 3.3x^3 + 4.4y^15 - 5.5x^23 + 6.6y^55
[native] Total Time: 8.38817100  Rate: 38707408.445 evals/sec Expression: sin(2 * x) + cos(pi / y)
[native] Total Time: 8.47000800  Rate: 38333418.457 evals/sec Expression: 1 - sin(2 * x) + cos(pi / y)
[native] Total Time: 10.27436400  Rate: 31601407.250 evals/sec Expression: sqrt(111.111 - sin(2 * x) + cos(pi / y) / 333.333)
[native] Total Time: 4.35803000  Rate: 74502552.988 evals/sec Expression: (x^2 / sin(2 * pi / y)) - x / 2
[native] Total Time: 24.65215100  Rate: 13170630.060 evals/sec Expression: x + (cos(y - sin(2 / x * pi)) - sin(x - cos(2 * y / pi))) - y
[native] Total Time: 10.65701700  Rate: 30466720.753 evals/sec Expression: clamp(-1.0, sin(2 * pi * x) + cos(y / 2 * pi), +1.0)
[native] Total Time: 9.24923200  Rate: 35103926.575 evals/sec Expression: max(3.33, min(sqrt(1 - sin(2 * x) + cos(pi / y) / 3), 1.11))
[native] Total Time: 1.26785800  Rate: 256088900.334 evals/sec Expression: if((y + (x * 2.2)) <= (x + y + 1.1), x - y, x * y) + 2 * pi / x
--- PARSE ----
[parse] Total Time: 1.08038600  Rate: 92559.511 parse/sec Expression: (y + x)
[parse] Total Time: 1.26697300  Rate: 78928.280 parse/sec Expression: 2 * (y + x)
[parse] Total Time: 1.38926000  Rate: 71980.767 parse/sec Expression: (2 * y + 2 * x)
[parse] Total Time: 1.75865400  Rate: 56861.668 parse/sec Expression: ((1.23 * x^2) / y) - 123.123
[parse] Total Time: 1.85638800  Rate: 53868.049 parse/sec Expression: (y + x / y) * (x - y / x)
[parse] Total Time: 1.90022900  Rate: 52625.236 parse/sec Expression: x / ((x + y) + (x - y)) / y
[parse] Total Time: 1.89685800  Rate: 52718.759 parse/sec Expression: 1 - ((x * y) + (y / x)) - 3
[parse] Total Time: 3.27211000  Rate: 30561.320 parse/sec Expression: (5.5 + x) + (2 * x - 2 / 3 * y) * (x / 3 + y / 4) + (y + 7.7)
[parse] Total Time: 3.95338700  Rate: 25294.766 parse/sec Expression: 1.1x^1 + 2.2y^2 - 3.3x^3 + 4.4y^15 - 5.5x^23 + 6.6y^55
[parse] Total Time: 1.71958900  Rate: 58153.431 parse/sec Expression: sin(2 * x) + cos(pi / y)
[parse] Total Time: 1.91889500  Rate: 52113.326 parse/sec Expression: 1 - sin(2 * x) + cos(pi / y)
[parse] Total Time: 2.26780400  Rate: 44095.521 parse/sec Expression: sqrt(111.111 - sin(2 * x) + cos(pi / y) / 333.333)
[parse] Total Time: 2.12057400  Rate: 47157.043 parse/sec Expression: (x^2 / sin(2 * pi / y)) - x / 2
[parse] Total Time: 3.27487000  Rate: 30535.563 parse/sec Expression: x + (cos(y - sin(2 / x * pi)) - sin(x - cos(2 * y / pi))) - y
[parse] Total Time: 2.59260500  Rate: 38571.244 parse/sec Expression: clamp(-1.0, sin(2 * pi * x) + cos(y / 2 * pi), +1.0)
[parse] Total Time: 2.72623800  Rate: 36680.583 parse/sec Expression: max(3.33, min(sqrt(1 - sin(2 * x) + cos(pi / y) / 3), 1.11))
[parse] Total Time: 3.17484000  Rate: 31497.650 parse/sec Expression: if((y + (x * 2.2)) <= (x + y + 1.1), x - y, x * y) + 2 * pi / x
[parse] Total Time: 0.32056700  Rate: 3119.473 parse/sec Expression: exprtk_special

### Base 5
--- EXPRTK EVAL ---
[exprtk] Total Time: 1.22643300  Rate: 264738767.629 evals/sec Expression: (y + x)
[exprtk] Total Time: 1.21877900  Rate: 266401341.835 evals/sec Expression: 2 * (y + x)
[exprtk] Total Time: 1.21456000  Rate: 267326736.431 evals/sec Expression: (2 * y + 2 * x)
[exprtk] Total Time: 2.59504500  Rate: 125117044.598 evals/sec Expression: ((1.23 * x^2) / y) - 123.123
[exprtk] Total Time: 2.87663400  Rate: 112869541.624 evals/sec Expression: (y + x / y) * (x - y / x)
[exprtk] Total Time: 3.08818400  Rate: 105137634.610 evals/sec Expression: x / ((x + y) + (x - y)) / y
[exprtk] Total Time: 2.03491800  Rate: 159556483.848 evals/sec Expression: 1 - ((x * y) + (y / x)) - 3
[exprtk] Total Time: 6.10522700  Rate: 53181374.091 evals/sec Expression: (5.5 + x) + (2 * x - 2 / 3 * y) * (x / 3 + y / 4) + (y + 7.7)
[exprtk] Total Time: 15.82799300  Rate: 20513299.507 evals/sec Expression: 1.1x^1 + 2.2y^2 - 3.3x^3 + 4.4y^15 - 5.5x^23 + 6.6y^55
[exprtk] Total Time: 11.19741500  Rate: 28996367.554 evals/sec Expression: sin(2 * x) + cos(pi / y)
[exprtk] Total Time: 11.92490500  Rate: 27227416.990 evals/sec Expression: 1 - sin(2 * x) + cos(pi / y)
[exprtk] Total Time: 14.54116400  Rate: 22328636.208 evals/sec Expression: sqrt(111.111 - sin(2 * x) + cos(pi / y) / 333.333)
[exprtk] Total Time: 8.36474100  Rate: 38815829.564 evals/sec Expression: (x^2 / sin(2 * pi / y)) - x / 2
[exprtk] Total Time: 31.22485500  Rate: 10398266.413 evals/sec Expression: x + (cos(y - sin(2 / x * pi)) - sin(x - cos(2 * y / pi))) - y
[exprtk] Total Time: 14.50646800  Rate: 22382040.963 evals/sec Expression: clamp(-1.0, sin(2 * pi * x) + cos(y / 2 * pi), +1.0)
[exprtk] Total Time: 20.10168200  Rate: 16152099.163 evals/sec Expression: max(3.33, min(sqrt(1 - sin(2 * x) + cos(pi / y) / 3), 1.11))
[exprtk] Total Time: 5.39549000  Rate: 60176992.451 evals/sec Expression: if((y + (x * 2.2)) <= (x + y + 1.1), x - y, x * y) + 2 * pi / x
[exprtk] Total Time: 4.62318700  Rate: 432602.012 evals/sec Expression: exprtk_special
--- NATIVE EVAL ---
[native] Total Time: 1.20591300  Rate: 269243602.980 evals/sec Expression: (y + x)
[native] Total Time: 1.20535500  Rate: 269368245.040 evals/sec Expression: 2 * (y + x)
[native] Total Time: 1.20858000  Rate: 268649457.214 evals/sec Expression: (2 * y + 2 * x)
[native] Total Time: 1.22642700  Rate: 264740062.800 evals/sec Expression: ((1.23 * x^2) / y) - 123.123
[native] Total Time: 1.19248800  Rate: 272274740.710 evals/sec Expression: (y + x / y) * (x - y / x)
[native] Total Time: 1.24748700  Rate: 260270737.090 evals/sec Expression: x / ((x + y) + (x - y)) / y
[native] Total Time: 1.23166800  Rate: 263613539.525 evals/sec Expression: 1 - ((x * y) + (y / x)) - 3
[native] Total Time: 1.38941600  Rate: 233684052.149 evals/sec Expression: (5.5 + x) + (2 * x - 2 / 3 * y) * (x / 3 + y / 4) + (y + 7.7)
[native] Total Time: 32.23727300  Rate: 10071706.779 evals/sec Expression: 1.1x^1 + 2.2y^2 - 3.3x^3 + 4.4y^15 - 5.5x^23 + 6.6y^55
[native] Total Time: 8.80237400  Rate: 36885999.277 evals/sec Expression: sin(2 * x) + cos(pi / y)
[native] Total Time: 8.95455500  Rate: 36259128.566 evals/sec Expression: 1 - sin(2 * x) + cos(pi / y)
[native] Total Time: 10.29247300  Rate: 31545806.435 evals/sec Expression: sqrt(111.111 - sin(2 * x) + cos(pi / y) / 333.333)
[native] Total Time: 4.38279400  Rate: 74081592.929 evals/sec Expression: (x^2 / sin(2 * pi / y)) - x / 2
[native] Total Time: 24.99981500  Rate: 12987470.547 evals/sec Expression: x + (cos(y - sin(2 / x * pi)) - sin(x - cos(2 * y / pi))) - y
[native] Total Time: 11.43410000  Rate: 28396144.952 evals/sec Expression: clamp(-1.0, sin(2 * pi * x) + cos(y / 2 * pi), +1.0)
[native] Total Time: 9.35943100  Rate: 34690608.970 evals/sec Expression: max(3.33, min(sqrt(1 - sin(2 * x) + cos(pi / y) / 3), 1.11))
[native] Total Time: 1.27043700  Rate: 255569037.268 evals/sec Expression: if((y + (x * 2.2)) <= (x + y + 1.1), x - y, x * y) + 2 * pi / x
--- PARSE ----
[parse] Total Time: 1.07845700  Rate: 92725.069 parse/sec Expression: (y + x)
[parse] Total Time: 1.27926200  Rate: 78170.070 parse/sec Expression: 2 * (y + x)
[parse] Total Time: 1.42691500  Rate: 70081.259 parse/sec Expression: (2 * y + 2 * x)
[parse] Total Time: 1.81204300  Rate: 55186.328 parse/sec Expression: ((1.23 * x^2) / y) - 123.123
[parse] Total Time: 1.91166000  Rate: 52310.557 parse/sec Expression: (y + x / y) * (x - y / x)
[parse] Total Time: 1.96885600  Rate: 50790.916 parse/sec Expression: x / ((x + y) + (x - y)) / y
[parse] Total Time: 1.96416100  Rate: 50912.323 parse/sec Expression: 1 - ((x * y) + (y / x)) - 3
[parse] Total Time: 3.40996500  Rate: 29325.814 parse/sec Expression: (5.5 + x) + (2 * x - 2 / 3 * y) * (x / 3 + y / 4) + (y + 7.7)
[parse] Total Time: 4.24224400  Rate: 23572.430 parse/sec Expression: 1.1x^1 + 2.2y^2 - 3.3x^3 + 4.4y^15 - 5.5x^23 + 6.6y^55
[parse] Total Time: 1.74658700  Rate: 57254.520 parse/sec Expression: sin(2 * x) + cos(pi / y)
[parse] Total Time: 1.93964100  Rate: 51555.932 parse/sec Expression: 1 - sin(2 * x) + cos(pi / y)
[parse] Total Time: 2.32884900  Rate: 42939.667 parse/sec Expression: sqrt(111.111 - sin(2 * x) + cos(pi / y) / 333.333)
[parse] Total Time: 2.17082500  Rate: 46065.436 parse/sec Expression: (x^2 / sin(2 * pi / y)) - x / 2
[parse] Total Time: 3.36726100  Rate: 29697.728 parse/sec Expression: x + (cos(y - sin(2 / x * pi)) - sin(x - cos(2 * y / pi))) - y
[parse] Total Time: 2.70154800  Rate: 37015.815 parse/sec Expression: clamp(-1.0, sin(2 * pi * x) + cos(y / 2 * pi), +1.0)
[parse] Total Time: 2.78468900  Rate: 35910.653 parse/sec Expression: max(3.33, min(sqrt(1 - sin(2 * x) + cos(pi / y) / 3), 1.11))
[parse] Total Time: 3.27487100  Rate: 30535.554 parse/sec Expression: if((y + (x * 2.2)) <= (x + y + 1.1), x - y, x * y) + 2 * pi / x
[parse] Total Time: 0.34775400  Rate: 2875.596 parse/sec Expression: exprtk_special

### Optimized 5
--- EXPRTK EVAL ---
[exprtk] Total Time: 1.20518200  Rate: 269406911.985 evals/sec Expression: (y + x)
[exprtk] Total Time: 1.16039400  Rate: 279805273.898 evals/sec Expression: 2 * (y + x)
[exprtk] Total Time: 1.14320300  Rate: 284012866.481 evals/sec Expression: (2 * y + 2 * x)
[exprtk] Total Time: 2.73291800  Rate: 118805013.908 evals/sec Expression: ((1.23 * x^2) / y) - 123.123
[exprtk] Total Time: 2.86763400  Rate: 113223779.952 evals/sec Expression: (y + x / y) * (x - y / x)
[exprtk] Total Time: 3.08707000  Rate: 105175574.574 evals/sec Expression: x / ((x + y) + (x - y)) / y
[exprtk] Total Time: 2.08624200  Rate: 155631207.214 evals/sec Expression: 1 - ((x * y) + (y / x)) - 3
[exprtk] Total Time: 6.24896100  Rate: 51958135.280 evals/sec Expression: (5.5 + x) + (2 * x - 2 / 3 * y) * (x / 3 + y / 4) + (y + 7.7)
[exprtk] Total Time: 15.70489600  Rate: 20674085.393 evals/sec Expression: 1.1x^1 + 2.2y^2 - 3.3x^3 + 4.4y^15 - 5.5x^23 + 6.6y^55
[exprtk] Total Time: 11.35268000  Rate: 28599798.550 evals/sec Expression: sin(2 * x) + cos(pi / y)
[exprtk] Total Time: 12.06743200  Rate: 26905837.215 evals/sec Expression: 1 - sin(2 * x) + cos(pi / y)
[exprtk] Total Time: 14.46853400  Rate: 22440722.813 evals/sec Expression: sqrt(111.111 - sin(2 * x) + cos(pi / y) / 333.333)
[exprtk] Total Time: 7.88590700  Rate: 41172735.235 evals/sec Expression: (x^2 / sin(2 * pi / y)) - x / 2
[exprtk] Total Time: 31.01169700  Rate: 10469738.596 evals/sec Expression: x + (cos(y - sin(2 / x * pi)) - sin(x - cos(2 * y / pi))) - y
[exprtk] Total Time: 15.26640100  Rate: 21267904.662 evals/sec Expression: clamp(-1.0, sin(2 * pi * x) + cos(y / 2 * pi), +1.0)
[exprtk] Total Time: 20.18032800  Rate: 16089151.822 evals/sec Expression: max(3.33, min(sqrt(1 - sin(2 * x) + cos(pi / y) / 3), 1.11))
[exprtk] Total Time: 5.40525600  Rate: 60068267.072 evals/sec Expression: if((y + (x * 2.2)) <= (x + y + 1.1), x - y, x * y) + 2 * pi / x
[exprtk] Total Time: 4.63562100  Rate: 431441.656 evals/sec Expression: exprtk_special
--- NATIVE EVAL ---
[native] Total Time: 1.20488600  Rate: 269473096.210 evals/sec Expression: (y + x)
[native] Total Time: 1.20602300  Rate: 269219045.574 evals/sec Expression: 2 * (y + x)
[native] Total Time: 1.20860900  Rate: 268643011.098 evals/sec Expression: (2 * y + 2 * x)
[native] Total Time: 1.21571500  Rate: 267072760.474 evals/sec Expression: ((1.23 * x^2) / y) - 123.123
[native] Total Time: 1.19341300  Rate: 272063703.848 evals/sec Expression: (y + x / y) * (x - y / x)
[native] Total Time: 1.27660400  Rate: 254334438.087 evals/sec Expression: x / ((x + y) + (x - y)) / y
[native] Total Time: 1.24386900  Rate: 261027777.845 evals/sec Expression: 1 - ((x * y) + (y / x)) - 3
[native] Total Time: 1.38858200  Rate: 233824405.761 evals/sec Expression: (5.5 + x) + (2 * x - 2 / 3 * y) * (x / 3 + y / 4) + (y + 7.7)
[native] Total Time: 32.37029300  Rate: 10030318.879 evals/sec Expression: 1.1x^1 + 2.2y^2 - 3.3x^3 + 4.4y^15 - 5.5x^23 + 6.6y^55
[native] Total Time: 8.48823300  Rate: 38251113.159 evals/sec Expression: sin(2 * x) + cos(pi / y)
[native] Total Time: 8.43018200  Rate: 38514513.803 evals/sec Expression: 1 - sin(2 * x) + cos(pi / y)
[native] Total Time: 10.16400600  Rate: 31944526.695 evals/sec Expression: sqrt(111.111 - sin(2 * x) + cos(pi / y) / 333.333)
[native] Total Time: 4.32882500  Rate: 75005194.481 evals/sec Expression: (x^2 / sin(2 * pi / y)) - x / 2
[native] Total Time: 24.77961200  Rate: 13102883.169 evals/sec Expression: x + (cos(y - sin(2 / x * pi)) - sin(x - cos(2 * y / pi))) - y
[native] Total Time: 10.67475800  Rate: 30416086.341 evals/sec Expression: clamp(-1.0, sin(2 * pi * x) + cos(y / 2 * pi), +1.0)
[native] Total Time: 9.15257100  Rate: 35474661.819 evals/sec Expression: max(3.33, min(sqrt(1 - sin(2 * x) + cos(pi / y) / 3), 1.11))
[native] Total Time: 1.26376000  Rate: 256919320.915 evals/sec Expression: if((y + (x * 2.2)) <= (x + y + 1.1), x - y, x * y) + 2 * pi / x
--- PARSE ----
[parse] Total Time: 1.06989600  Rate: 93467.029 parse/sec Expression: (y + x)
[parse] Total Time: 1.28908700  Rate: 77574.283 parse/sec Expression: 2 * (y + x)
[parse] Total Time: 1.41136100  Rate: 70853.595 parse/sec Expression: (2 * y + 2 * x)
[parse] Total Time: 1.80595000  Rate: 55372.519 parse/sec Expression: ((1.23 * x^2) / y) - 123.123
[parse] Total Time: 1.97381100  Rate: 50663.412 parse/sec Expression: (y + x / y) * (x - y / x)
[parse] Total Time: 1.96946800  Rate: 50775.133 parse/sec Expression: x / ((x + y) + (x - y)) / y
[parse] Total Time: 1.98844500  Rate: 50290.554 parse/sec Expression: 1 - ((x * y) + (y / x)) - 3
[parse] Total Time: 3.31877000  Rate: 30131.645 parse/sec Expression: (5.5 + x) + (2 * x - 2 / 3 * y) * (x / 3 + y / 4) + (y + 7.7)
[parse] Total Time: 4.10284100  Rate: 24373.355 parse/sec Expression: 1.1x^1 + 2.2y^2 - 3.3x^3 + 4.4y^15 - 5.5x^23 + 6.6y^55
[parse] Total Time: 1.73032600  Rate: 57792.578 parse/sec Expression: sin(2 * x) + cos(pi / y)
[parse] Total Time: 1.92419300  Rate: 51969.839 parse/sec Expression: 1 - sin(2 * x) + cos(pi / y)
[parse] Total Time: 2.25422900  Rate: 44361.065 parse/sec Expression: sqrt(111.111 - sin(2 * x) + cos(pi / y) / 333.333)
[parse] Total Time: 2.16642100  Rate: 46159.080 parse/sec Expression: (x^2 / sin(2 * pi / y)) - x / 2
[parse] Total Time: 3.30083900  Rate: 30295.328 parse/sec Expression: x + (cos(y - sin(2 / x * pi)) - sin(x - cos(2 * y / pi))) - y
[parse] Total Time: 2.58056500  Rate: 38751.204 parse/sec Expression: clamp(-1.0, sin(2 * pi * x) + cos(y / 2 * pi), +1.0)
[parse] Total Time: 2.71866200  Rate: 36782.800 parse/sec Expression: max(3.33, min(sqrt(1 - sin(2 * x) + cos(pi / y) / 3), 1.11))
[parse] Total Time: 3.25037100  Rate: 30765.719 parse/sec Expression: if((y + (x * 2.2)) <= (x + y + 1.1), x - y, x * y) + 2 * pi / x
[parse] Total Time: 0.32977300  Rate: 3032.389 parse/sec Expression: exprtk_special
