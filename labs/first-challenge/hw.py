def aiuda(f):
    maxC=0
    for x in range(len(f)):
        usedChars=""
        c=0
        largest=True
        for y in range(x,len(f)):
            if f[y] in usedChars:
                c=0
                usedChars=""
                largest=False
                break
            else:
                c+=1
                usedChars+=f[y]
            if(maxC<=c):
                maxC=c
            if not largest:
                break
        if largest:
            break
    return maxC

superf=aiuda(input("input\n"))
print("output:",superf)
