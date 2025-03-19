# My tester for sin.ou to check if the values there are correct

def ron_sin(deg):
    cursine = 0
    prevsine = 0
    loopnum = 0
    power = 0
    sign = 0
    deg = deg*3.14159/180
    cursine = deg
    loopnum = 1
    power = deg
    sign = 1
    while 0.0001 < power and loopnum < 200:
        loopnum = loopnum + 2
        power = power*deg*deg/(loopnum*(loopnum-1))
        prevsine = cursine
        sign = sign * (0-1)
        cursine = cursine + sign*power

    return cursine

print(ron_sin(70))