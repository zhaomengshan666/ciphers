def S_BOX(a):
    list=[0xc,0x5,0x6,0xb,0x9,0x0,0xa,0xd,0x3,0xe,0xf,0x8,0x4,0x7,0x1,0x2]
    return list[a]

def pChange(a):
    listp=[]
    res=0
    for i in range(64):
        listp.append(a>>i&1)
    listnp=[]
    for k in range(64):
        listnp.append(listp[k])
    for i in range(63):
        listnp[i*16%63] = listp[i]
    for i in range(64):
        res = res ^ (listnp[i]<<i)
    return res

def key_scheduile(key):
    rk=[]
    rk.append(key>>16)
    for i in range(1,32):
        temp = key
        temp1 = key >> 19
        temp2 = temp & 0x7ffff
        temp2 = temp2<<61
        k1 = temp1 ^ temp2
        b = S_BOX(k1>>76)<<76
        k1 = k1 & 0xfffffffffffffffffff
        k1 = k1 ^ b
        k2 = k1 & 0x7fff
        k3= ((k1>>15) ^ i)<<15
        fk = k2 ^ k3
        key = fk
        rk.append(fk>>16)
    return rk

def encryptP(plain,key):
    #generate round key
    list_rk = key_scheduile(key)
    for i in range(31):
        plain = plain^list_rk[i]
        pBeforeSbox=[]
        for j in range(16):
            temp1 = plain
            temp1 = temp1>>(4*j) & 0xf
            pBeforeSbox.append(S_BOX(temp1))
        pAfterSbox=0
        for k in range(15,-1,-1):
            pAfterSbox=pAfterSbox^(pBeforeSbox[k]<<(4*k))
        plain = pChange(pAfterSbox)
    cipher=plain
    cipher=cipher^list_rk[31]
    return cipher

print(hex(encryptP(0xffffffffffffffff,0xffffffffffffffffffff)))
