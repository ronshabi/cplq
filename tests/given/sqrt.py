# My tester for sqrt.ou to check if the values there are correct


def ron_sqrt(a):
    x = a / 2.0
    stop = 0

    while stop == 0:
        x = x / 2.0 + a / (2.0 * x)
        delta = x * x - a

        if delta < 0:
            delta = 0 - delta

        if delta < 0.0001:
            stop = 1

    print(a)
    print(x)

ron_sqrt(3.14)