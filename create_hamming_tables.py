import numpy as np
import copy
import itertools
generator_M = np.array([
    [1,0,1,1,1,0,0,0],
    [1,1,0,1,0,1,0,0],
    [1,1,1,0,0,0,1,0],
    [0,1,1,1,0,0,0,1]
],dtype=np.uint8)
parity_check_M = np.array([
    [1,0,0,0,1,1,1,0],
    [0,1,0,0,0,1,1,1],
    [0,0,1,0,1,0,1,1],
    [0,0,0,1,1,1,0,1]
],dtype=np.uint8)
print(generator_M)
data_bytes = np.array([
    [0,0,0,0],
    [0,0,0,1],
    [0,0,1,0],
    [0,0,1,1],
    [0,1,0,0],
    [0,1,0,1],
    [0,1,1,0],
    [0,1,1,1],
    [1,0,0,0],
    [1,0,0,1],
    [1,0,1,0],
    [1,0,1,1],
    [1,1,0,0],
    [1,1,0,1],
    [1,1,1,0],
    [1,1,1,1]
],dtype=np.uint8)
hamming_bytes = np.matmul(data_bytes,generator_M) % 2
# hamming_bytes[0][0] = 1
print(hamming_bytes)
print(parity_check_M.shape, hamming_bytes.shape)
print(np.matmul(parity_check_M,hamming_bytes.transpose()) % 2)

n = 8
code_words = np.array([list(i) for i in itertools.product([0, 1], repeat=n)])
syndromes = np.matmul(parity_check_M,code_words.transpose()) % 2
corrected = copy.deepcopy(syndromes)
print("#"*30)
for i_ind in range(len(corrected)):
    i = corrected[i_ind]
    for j in range(3,8):
        if(i[j] == 1):
            i[j-4] ^= i[j-4]
for i,j in zip(corrected, syndromes):
    print(i-j)

valid_code_words = []
for bits_ in data_bytes:
    bits = bits_.tolist() + [0,0,0,0]
    bits[4] = (bits[1] ^ bits[2] ^ bits[3])
    bits[5] = (bits[0] ^ bits[2] ^ bits[3])
    bits[6] = (bits[0] ^ bits[1] ^ bits[3])
    bits[7] = (bits[0] ^ bits[1] ^ bits[2])

    valid_code_words.append(bits)
valid_code_words = valid_code_words
print("VALID CODES: ",valid_code_words)
vals = []
count = 0
for cw in valid_code_words:
    for i in range(8):
        bits = copy.deepcopy(cw)
        syndrome = [0]*4
        bits[i] = int(not bits[i])
        syndrome[0] = (bits[1] ^ bits[2] ^ bits[3] ^ bits[4])
        syndrome[1] = (bits[0] ^ bits[2] ^ bits[3] ^ bits[5])
        syndrome[2] = (bits[0] ^ bits[1] ^ bits[3] ^ bits[6])
        syndrome[3] = (bits[0] ^ bits[1] ^ bits[2] ^ bits[7])
        print(cw," : ",i," = ",syndrome)
        count += 1
print(count)