import ctypes

testlib = ctypes.CDLL('simulator.so')

testlib.run_simulation.restype = ctypes.POINTER(ctypes.c_float)

# GameUnit(int x, int y, float health_, int movementSpeed_, int attack_, int creationIndex_, int shielding_, int playerNumber_, float range_) {

inputArr = (ctypes.c_float * (9 * 2))()
inputArr[0] = 0
inputArr[1] = 13
inputArr[2] = 40
inputArr[3] = 4
inputArr[4] = 20
inputArr[5] = 1
inputArr[6] = 0
inputArr[7] = 0
inputArr[8] = 3.5

inputArr[9] = 5
inputArr[10] = 8
inputArr[11] = 25
inputArr[12] = 1
inputArr[13] = 2
inputArr[14] = 2
inputArr[15] = 0
inputArr[16] = 0
inputArr[17] = 2.5

resultArr = (ctypes.c_float * 10)()
for i in range(10):
    resultArr[i] = 0

result = testlib.run_simulation(inputArr, 2, resultArr)
for i in range(10):
    print(result[i])

