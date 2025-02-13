num = int(input())
binstrl = []
for i in range(0,num):
    binstrl.append(input())

for s in binstrl:
    print(format(int(s , 2) , '04X'))

